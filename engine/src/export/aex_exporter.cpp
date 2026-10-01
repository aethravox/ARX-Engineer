// ==============================================================================
// engine/src/export/aex_exporter.cpp
// ==============================================================================

#include "aex_exporter.hpp"
#include "core/logging.hpp"

#include "arx/format/aex.h"
#include "arx/crypto/ed25519.h"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <cstring>

namespace arx {

namespace {

// Lee un archivo binario completo a un std::vector<uint8_t>
std::vector<uint8_t> read_binary_file(const fs::path& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return {};
    auto sz = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> buf(sz);
    f.read(reinterpret_cast<char*>(buf.data()), sz);
    return buf;
}

// Convierte un fs::path relativo a un path "assets/..." compatible con .aex
std::string normalize_asset_path(const fs::path& project_root,
                                   const fs::path& asset_abs) {
    auto rel = fs::relative(asset_abs, project_root);
    std::string s = rel.string();
    // Reemplazar backslashes por forward slashes (Windows)
    for (auto& c : s) if (c == '\\') c = '/';
    return s;
}

} // namespace

// ------------------------------------------------------------------------------
bool AexExporter::compile_runtime(const fs::path& root,
                                    const ExportPreset& preset,
                                    ExportResult& result) {
    // Para .aex no hay "compilacion" como tal. Lo que hacemos aqui es
    // preparar el manifest + recolectar assets + scripts. El verdadero
    // empaquetado (firma incluida) ocurre en package_output().

    ARX_LOG_INFO("AexExporter: preparando paquete .aex para '{}'", preset.app_name);

    // 1. Recolectar archivos .arx (scripts ARXScript).
    auto arx_files = find_arx_files(root);
    if (arx_files.empty()) {
        result.warnings.push_back("No se encontraron archivos .arx en el proyecto");
    }

    // 2. Recolectar assets.
    auto asset_files = find_asset_files(root);
    result.logs.push_back("Assets encontrados: " + std::to_string(asset_files.size()));
    result.logs.push_back("Scripts .arx: " + std::to_string(arx_files.size()));

    return true;
}

// ------------------------------------------------------------------------------
bool AexExporter::package_output(const fs::path& root,
                                   const ExportPreset& preset,
                                   ExportResult& result) {
    // Construir el manifest JSON.
    std::string manifest;
    {
        std::ostringstream ss;
        ss << "{";
        ss << "\"schema\":1,";
        ss << "\"package\":{";
        ss << "\"name\":\"" << preset.app_name << "\",";
        ss << "\"version\":\"1.0.0\"";
        ss << "},";
        ss << "\"engine\":{\"min_version\":\"0.0.1\"},";
        ss << "\"permissions\":{";
        ss << "\"network\":{\"allowed\":false},";
        ss << "\"filesystem\":{\"allowed\":false}";
        ss << "},";
        ss << "\"scripts\":{\"entry\":\"main.arx\",\"type\":\"arxscript_source\"},";
        ss << "\"metadata\":{\"title\":\"" << preset.app_name << "\"}";
        ss << "}";
        manifest = ss.str();
    }

    // Crear builder.
    AexBuilder* b = aex_builder_new();
    if (!b) {
        result.error_message = "aex_builder_new devolvio null";
        return false;
    }

    AexResult r = aex_builder_set_manifest(b, manifest.c_str());
    if (r != AEX_OK) {
        result.error_message = std::string("set_manifest: ") + aex_strerror(r);
        aex_builder_free(b);
        return false;
    }

    // Agregar assets.
    result.total_bytes = 0;
    auto asset_files = find_asset_files(root);
    for (const auto& asset_path : asset_files) {
        auto data = read_binary_file(asset_path);
        if (data.empty()) {
            result.warnings.push_back("Asset vacio o ilegible: " + asset_path.string());
            continue;
        }
        std::string norm = normalize_asset_path(root, asset_path);
        r = aex_builder_add_asset(b, norm.c_str(), data.data(), data.size());
        if (r != AEX_OK) {
            result.warnings.push_back(std::string("add_asset ") + norm + ": " + aex_strerror(r));
        } else {
            result.total_bytes += static_cast<int64_t>(data.size());
        }
    }

    // Scripts: por ahora agregamos los .arx como bloque de scripts.
    // En el futuro: transpilar a Luau bytecode.
    auto arx_files = find_arx_files(root);
    if (!arx_files.empty()) {
        // Concatenar todos los .arx en un solo buffer (separados por \n).
        std::string scripts_blob;
        for (const auto& f : arx_files) {
            std::ifstream in(f);
            std::stringstream ss;
            ss << in.rdbuf();
            scripts_blob += "-- script: " + f.filename().string() + "\n";
            scripts_blob += ss.str();
            scripts_blob += "\n";
        }
        r = aex_builder_set_scripts(b, AEX_SCRIPTS_ARXSCRIPT_CPP,
                                      reinterpret_cast<const uint8_t*>(scripts_blob.data()),
                                      scripts_blob.size());
        if (r != AEX_OK) {
            result.warnings.push_back(std::string("set_scripts: ") + aex_strerror(r));
        }
        result.arx_files_transpiled = arx_files.size();
    }

    // Clave privada del creador.
    uint8_t priv_seed[32];
    bool has_key = false;
    if (!creator_key_path_.empty()) {
        auto key_data = read_binary_file(creator_key_path_);
        if (key_data.size() >= 32) {
            std::memcpy(priv_seed, key_data.data(), 32);
            has_key = true;
            result.logs.push_back("Usando clave del creador: " + creator_key_path_);
        } else {
            result.warnings.push_back("Clave privada invalida (menos de 32 bytes), "
                                      "se generara una clave efimera");
        }
    }
    if (!has_key) {
        // Clave efimera (solo para testing — el .aex no sera verificable
        // por un trust store real, pero el formato sera valido).
        uint8_t priv64[64], pub[32];
        if (!arx_ed25519_keygen(priv64, pub)) {
            result.error_message = "arx_ed25519_keygen fallo";
            aex_builder_free(b);
            return false;
        }
        std::memcpy(priv_seed, priv64, 32);
        result.warnings.push_back("Usando clave efimera — el .aex no sera verificable "
                                  "por un trust store real");
    }
    r = aex_builder_set_creator_key(b, priv_seed);
    if (r != AEX_OK) {
        result.error_message = std::string("set_creator_key: ") + aex_strerror(r);
        aex_builder_free(b);
        return false;
    }

    // Sin compresion por ahora (zstd pendiente).
    aex_builder_set_compressed(b, false);

    // Path de salida.
    fs::path out_dir = root / "_export_aex";
    fs::create_directories(out_dir);
    fs::path out_file = out_dir / (preset.app_name + ".aex");

    r = aex_builder_write(b, out_file.string().c_str());
    if (r != AEX_OK) {
        result.error_message = std::string("aex_builder_write: ") + aex_strerror(r);
        aex_builder_free(b);
        return false;
    }

    result.output_path = out_file.string();
    result.logs.push_back("Paquete .aex escrito: " + out_file.string());

    // Tamano del archivo generado.
    std::error_code ec;
    auto sz = fs::file_size(out_file, ec);
    if (!ec) {
        result.logs.push_back("Tamano: " + std::to_string(sz) + " bytes");
    }

    aex_builder_free(b);
    return true;
}

} // namespace arx

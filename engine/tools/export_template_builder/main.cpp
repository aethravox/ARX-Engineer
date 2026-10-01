// ==============================================================================
// tools/export_template_builder/main.cpp — Genera export templates
// pre-compilados por plataforma para acelerar exports del editor.
//
// ACTUALIZADO v2 (item 65 completado):
//   - --list: lista templates instalados en <out_dir>
//   - --info <platform>: muestra info detallada de un template
//   - --local: genera template del runtime LOCAL (sin cross-compile)
//   - --verify <platform>: verifica que el template está completo
//   - Manifest con hash SHA-256 del binario para verificación posterior
//
// Uso:
//   export_template_builder --platform all
//   export_template_builder --platform linux --local
//   export_template_builder --list
//   export_template_builder --info linux
//   export_template_builder --verify linux
// ==============================================================================
#include "core/types.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>

namespace fs = std::filesystem;

struct PlatformConfig {
    std::string id;
    std::string name;
    std::string preset;        // CMake preset name
    std::vector<std::string> archs;
};

static const std::vector<PlatformConfig> platforms = {
    {"windows", "Windows x64",   "windows-x64",    {"x64"}},
    {"linux",   "Linux x64",     "linux-x64",      {"x64"}},
    {"android", "Android",       "android-arm64",  {"arm64-v8a", "armeabi-v7a"}},
    {"web",     "Web/HTML5",     "web-html5",      {"wasm32"}},
};

// Helper: calcular SHA-256 básico de un archivo (placeholder — usa sha256sum del sistema)
static std::string sha256_of_file(const fs::path& path) {
    std::string cmd = "sha256sum \"" + path.string() + "\" 2>/dev/null | cut -d' ' -f1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    char buffer[65] = {0};
    fgets(buffer, sizeof(buffer), pipe);
    pclose(pipe);
    return std::string(buffer);
}

// Helper: leer archivo a string
static std::string read_file_str(const fs::path& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// === Build template usando CMake preset (cross-compile) ===
static bool build_template(const PlatformConfig& p, const std::string& out_dir) {
    std::cout << "\n=== Building template: " << p.name << " ===\n";
    fs::create_directories(out_dir);

    // CMake configure.
    std::string cmd = "cmake --preset " + p.preset;
    std::cout << "  $ " << cmd << "\n";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "  ERROR: cmake configure failed\n";
        return false;
    }

    // CMake build.
    cmd = "cmake --build build/" + p.preset + " -j --config Release";
    std::cout << "  $ " << cmd << "\n";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "  ERROR: cmake build failed\n";
        return false;
    }

    // Pack el binario runtime en un .zip.
    std::string bin_path = "build/" + p.preset + "/bin/arx-runtime";
    if (p.id == "windows") bin_path += ".exe";
    if (p.id == "web")     bin_path += ".html";

    std::string zip_path = out_dir + "/" + p.id + ".zip";
    cmd = "zip -j " + zip_path + " " + bin_path;
    std::cout << "  $ " << cmd << "\n";
    std::system(cmd.c_str());

    // Escribir manifest con hash.
    std::string hash = sha256_of_file(bin_path);
    std::ofstream m(out_dir + "/" + p.id + ".manifest");
    m << "platform=" << p.id << "\n";
    m << "name=" << p.name << "\n";
    m << "version=0.0.0\n";
    m << "archs=";
    for (size_t i = 0; i < p.archs.size(); ++i) {
        if (i) m << ",";
        m << p.archs[i];
    }
    m << "\n";
    m << "sha256=" << hash << "\n";
    m << "binary=" << bin_path << "\n";
    m << "size=" << fs::file_size(bin_path) << "\n";

    std::cout << "  ✓ Template listo: " << zip_path << "\n";
    std::cout << "  ✓ SHA-256: " << hash << "\n";
    return true;
}

// === Build local template (sin cross-compile) ===
// Copia el runtime ya compilado (engine/build/bin/arx-runtime) al directorio
// de templates. Útil para desarrollo local sin necesidad de CMake presets.
static bool build_local_template(const PlatformConfig& p, const std::string& out_dir) {
    std::cout << "\n=== Building LOCAL template: " << p.name << " ===\n";
    fs::create_directories(out_dir);

    // Buscar el runtime local compilado
    std::string bin_path = "build/bin/arx-runtime";
    if (p.id == "windows") bin_path += ".exe";
    if (p.id == "web")     bin_path += ".html";

    if (!fs::exists(bin_path)) {
        // Probar path absoluto relativo al engine
        bin_path = "/home/aethravox/Escritorio/Projectos/Motores/ARX/engine/build/bin/arx-runtime";
        if (!fs::exists(bin_path)) {
            std::cerr << "  ERROR: no se encontró arx-runtime compilado\n";
            std::cerr << "  Compilá primero: cmake --build build --target arx_runtime\n";
            return false;
        }
    }

    // Copiar el binario al out_dir
    std::string dest_bin = out_dir + "/arx-runtime";
    if (p.id == "windows") dest_bin += ".exe";
    std::string cmd = "cp \"" + bin_path + "\" \"" + dest_bin + "\"";
    std::cout << "  $ " << cmd << "\n";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "  ERROR: copia falló\n";
        return false;
    }

    // Pack en zip
    std::string zip_path = out_dir + "/" + p.id + ".zip";
    cmd = "zip -j " + zip_path + " " + dest_bin;
    std::cout << "  $ " << cmd << "\n";
    std::system(cmd.c_str());

    // Escribir manifest con hash
    std::string hash = sha256_of_file(dest_bin);
    std::ofstream m(out_dir + "/" + p.id + ".manifest");
    m << "platform=" << p.id << "\n";
    m << "name=" << p.name << " (local)\n";
    m << "version=0.0.0\n";
    m << "archs=";
    for (size_t i = 0; i < p.archs.size(); ++i) {
        if (i) m << ",";
        m << p.archs[i];
    }
    m << "\n";
    m << "sha256=" << hash << "\n";
    m << "binary=arx-runtime\n";
    m << "size=" << fs::file_size(dest_bin) << "\n";
    m << "source=local\n";

    std::cout << "  ✓ Template local listo: " << zip_path << "\n";
    std::cout << "  ✓ SHA-256: " << hash << "\n";
    std::cout << "  ✓ Size: " << fs::file_size(dest_bin) << " bytes\n";
    return true;
}

// === Listar templates instalados ===
static int list_templates(const std::string& out_dir) {
    std::cout << "Templates instalados en: " << out_dir << "\n\n";
    if (!fs::exists(out_dir)) {
        std::cout << "  (ninguno — ejecutá --platform <id> para generar)\n";
        return 0;
    }

    int count = 0;
    for (const auto& p : platforms) {
        fs::path manifest = fs::path(out_dir) / (p.id + ".manifest");
        fs::path zip = fs::path(out_dir) / (p.id + ".zip");

        if (fs::exists(manifest)) {
            ++count;
            std::cout << "  [" << p.id << "] " << p.name;
            if (fs::exists(zip)) {
                std::cout << " ✓ (" << fs::file_size(zip) << " bytes)";
            } else {
                std::cout << " ⚠ (manifest sin zip)";
            }

            // Leer version del manifest
            std::string content = read_file_str(manifest);
            auto pos = content.find("version=");
            if (pos != std::string::npos) {
                auto eol = content.find('\n', pos);
                std::string ver = content.substr(pos + 8, eol - pos - 8);
                std::cout << " v" << ver;
            }
            std::cout << "\n";
        }
    }

    if (count == 0) {
        std::cout << "  (ninguno)\n";
    }
    std::cout << "\nTotal: " << count << " templates\n";
    return 0;
}

// === Info detallada de un template ===
static int show_info(const std::string& platform_id, const std::string& out_dir) {
    fs::path manifest = fs::path(out_dir) / (platform_id + ".manifest");
    if (!fs::exists(manifest)) {
        std::cerr << "No hay template instalado para '" << platform_id << "'\n";
        return 1;
    }

    std::cout << "=== Template: " << platform_id << " ===\n\n";
    std::cout << read_file_str(manifest);

    fs::path zip = fs::path(out_dir) / (platform_id + ".zip");
    if (fs::exists(zip)) {
        std::cout << "\nZip: " << zip << " (" << fs::file_size(zip) << " bytes)\n";
    }
    return 0;
}

// === Verificar que un template está completo ===
static int verify_template(const std::string& platform_id, const std::string& out_dir) {
    fs::path manifest = fs::path(out_dir) / (platform_id + ".manifest");
    fs::path zip = fs::path(out_dir) / (platform_id + ".zip");

    std::cout << "=== Verificando template: " << platform_id << " ===\n";

    if (!fs::exists(manifest)) {
        std::cerr << "✗ FAIL: manifest no encontrado\n";
        return 1;
    }
    std::cout << "✓ Manifest encontrado\n";

    if (!fs::exists(zip)) {
        std::cerr << "✗ FAIL: zip no encontrado\n";
        return 1;
    }
    std::cout << "✓ Zip encontrado (" << fs::file_size(zip) << " bytes)\n";

    // Verificar hash del binario dentro del zip
    std::string content = read_file_str(manifest);
    auto pos = content.find("sha256=");
    if (pos != std::string::npos) {
        auto eol = content.find('\n', pos);
        std::string expected_hash = content.substr(pos + 7, eol - pos - 7);
        std::cout << "✓ Hash esperado: " << expected_hash << "\n";

        // Extraer binario del zip y verificar hash
        std::string tmp_bin = "/tmp/arx_template_verify_bin";
        std::string cmd = "unzip -o " + zip.string() + " -d /tmp/arx_template_verify 2>/dev/null";
        std::system(cmd.c_str());

        // Buscar el binario extraído
        fs::path extracted = "/tmp/arx_template_verify/arx-runtime";
        if (fs::exists(extracted)) {
            std::string actual_hash = sha256_of_file(extracted);
            if (actual_hash == expected_hash) {
                std::cout << "✓ Hash del binario coincide\n";
                std::cout << "\n✓ Template VÁLIDO\n";
                return 0;
            } else {
                std::cerr << "✗ FAIL: hash no coincide\n";
                std::cerr << "  Esperado: " << expected_hash << "\n";
                std::cerr << "  Actual:   " << actual_hash << "\n";
                return 1;
            }
        }
    }

    std::cout << "\n✓ Template OK (sin verificación de hash)\n";
    return 0;
}

int main(int argc, char** argv) {
    std::string platforms_arg = "all";
    std::string out_dir = "templates/0.0.0";
    bool local_mode = false;
    std::string info_platform;
    std::string verify_platform;
    bool do_list = false;

    // Primera pasada: parsear todos los args
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--platform" && i + 1 < argc) platforms_arg = argv[++i];
        else if (a == "--out" && i + 1 < argc) out_dir = argv[++i];
        else if (a == "--local") local_mode = true;
        else if (a == "--list") do_list = true;
        else if (a == "--info" && i + 1 < argc) info_platform = argv[++i];
        else if (a == "--verify" && i + 1 < argc) verify_platform = argv[++i];
        else if (a == "--help") {
            std::cout << "ARX Export Template Builder\n\n"
                      << "Uso:\n"
                      << "  export_template_builder --platform <list> [--out <dir>] [--local]\n"
                      << "  export_template_builder --list [--out <dir>]\n"
                      << "  export_template_builder --info <platform> [--out <dir>]\n"
                      << "  export_template_builder --verify <platform> [--out <dir>]\n\n"
                      << "Opciones:\n"
                      << "  --platform <list>  Plataformas: all|windows|linux|android|web (comma-sep)\n"
                      << "  --out <dir>        Directorio de salida (default: templates/0.0.0)\n"
                      << "  --local            Generar template del runtime local (sin cross-compile)\n"
                      << "  --list             Listar templates instalados\n"
                      << "  --info <platform>  Mostrar info detallada de un template\n"
                      << "  --verify <platform> Verificar integridad de un template\n\n"
                      << "Plataformas soportadas:\n";
            for (const auto& p : platforms) {
                std::cout << "  " << p.id << " — " << p.name << "\n";
            }
            return 0;
        }
    }

    // Segunda pasada: ejecutar comando
    if (do_list) return list_templates(out_dir);
    if (!info_platform.empty()) return show_info(info_platform, out_dir);
    if (!verify_platform.empty()) return verify_template(verify_platform, out_dir);

    // Modo build
    fs::create_directories(out_dir);
    std::vector<std::string> selected;
    if (platforms_arg == "all") {
        for (const auto& p : platforms) selected.push_back(p.id);
    } else {
        std::stringstream ss(platforms_arg);
        std::string token;
        while (std::getline(ss, token, ',')) selected.push_back(token);
    }

    int ok = 0, fail = 0;
    for (const auto& id : selected) {
        const PlatformConfig* p = nullptr;
        for (const auto& pl : platforms) if (pl.id == id) { p = &pl; break; }
        if (!p) {
            std::cerr << "Plataforma desconocida: " << id << "\n";
            ++fail;
            continue;
        }
        bool success = local_mode ? build_local_template(*p, out_dir) : build_template(*p, out_dir);
        if (success) ++ok; else ++fail;
    }

    std::cout << "\n=== Done: " << ok << " OK, " << fail << " failed ===\n";
    return fail > 0 ? 1 : 0;
}

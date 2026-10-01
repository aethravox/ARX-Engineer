// ==============================================================================
// src/export/export_plugin.cpp — Implementación base del sistema de export.
//
// El flujo de exportación es:
//   1. Encontrar todos los .arx del proyecto.
//////   2. Transpilarlos a .cpp + .h con arxscript::Transpiler.
//   3. Compilar el runtime (que enlaza arx_core + los .arx.cpp generados) con
//      el toolchain de la plataforma destino (MSVC/GCC/NDK/Emscripten).
//   4. Empaquetar el binario resultante + assets + icon + manifest.
// ==============================================================================
#include "export_plugin.hpp"
////#include "arxscript/lexer.hpp"  // removed
////#include "arxscript/parser.hpp"  // removed
#include "core/logging.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace arx {

// ===================== Registry ==============================================
ExportPluginRegistry& ExportPluginRegistry::instance() {
    static ExportPluginRegistry r;
    return r;
}
void ExportPluginRegistry::register_plugin(std::unique_ptr<ExportPlugin> p) {
    plugins_.push_back(std::move(p));
}
ExportPlugin* ExportPluginRegistry::get_plugin(const std::string& platform) {
    for (auto& p : plugins_) {
        if (p->platform_id() == platform) return p.get();
    }
    return nullptr;
}
std::vector<ExportPlugin*> ExportPluginRegistry::all_plugins() {
    std::vector<ExportPlugin*> out;
    for (auto& p : plugins_) out.push_back(p.get());
    return out;
}

// ===================== ExportPlugin ==========================================
ExportResult ExportPlugin::export_project(const fs::path& project_root,
                                            const ExportPreset& preset) {
    ExportResult r;
    ARX_LOG_INFO("Exportando proyecto '{}' a '{}' (AOT={})",
                 project_root.string(), preset.platform,
                 preset.aot_arxscript ? "sí" : "no");

    if (!fs::exists(project_root / "project.arx")) {
        r.error_message = "No se encontró project.arx en " + project_root.string();
        ARX_LOG_ERROR("{}", r.error_message);
        return r;
    }

    // 1. Transpile .arx → .cpp
    if (preset.aot_arxscript) {
        if (!transpile_arx_files(project_root, preset, r)) {
            r.error_message = "Transpilación ARXScript fallida";
            return r;
        }
    }

    // 2. Compile runtime + .arx.cpp generados
    if (!compile_runtime(project_root, preset, r)) {
        r.error_message = "Compilación fallida";
        return r;
    }

    // 3. Package (binario + assets + manifest)
    if (!package_output(project_root, preset, r)) {
        r.error_message = "Empaquetado fallido";
        return r;
    }

    r.success = true;
    ARX_LOG_INFO("Export OK: {} ({} bytes, {} archivos ARX transpilados, {} fuentes C++ compiladas)",
                 r.output_path, r.total_bytes, r.arx_files_transpiled,
                 r.source_files_compiled);
    return r;
}

bool ExportPlugin::transpile_arx_files(const fs::path& root,
                                         const ExportPreset& preset,
                                         ExportResult& result) {
    // ARXScript transpiler pendiente de integración (Zen VM aún no cableada al engine).
    // Por ahora: no-op, solo reporta los .arx encontrados.
    auto arx_files = find_arx_files(root);
    ARX_LOG_INFO("Encontrados {} archivos .arx (transpile deshabilitado)", arx_files.size());
    result.arx_files_transpiled = 0;
    for (const auto& f : arx_files) {
        result.logs.push_back("[skip] " + f.string() + " (ARXScript transpiler no integrado)");
    }
    return true;
}

std::vector<fs::path> ExportPlugin::find_arx_files(const fs::path& root) const {
    std::vector<fs::path> out;
    if (!fs::exists(root)) return out;
    for (auto& e : fs::recursive_directory_iterator(root)) {
        if (!e.is_regular_file()) continue;
        // Buscar .arx (ARXScript) y .zen (Zen Lang)
        auto ext = e.path().extension().string();
        if (ext == ".arx" || ext == ".zen") {
            // Ignorar archivos en _export_ (output de exports previos)
            if (e.path().string().find("_export_") != std::string::npos)
                continue;
            // Ignorar project.arx (es el manifest del proyecto, no código)
            if (e.path().filename() == "project.arx")
                continue;
            out.push_back(e.path());
        }
    }
    return out;
}

std::vector<fs::path> ExportPlugin::find_asset_files(const fs::path& root) const {
    std::vector<fs::path> out;
    if (!fs::exists(root / "assets")) return out;
    static const std::vector<std::string> exts = {
        ".png", ".jpg", ".jpeg", ".webp", ".tga", ".bmp",
        ".ogg", ".wav", ".mp3",
        ".gltf", ".glb", ".obj", ".fbx",
        ".ttf", ".otf",
        ".svg",
        ".scene"
    };
    for (auto& e : fs::recursive_directory_iterator(root / "assets")) {
        if (!e.is_regular_file()) continue;
        std::string ext = e.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (std::find(exts.begin(), exts.end(), ext) != exts.end())
            out.push_back(e.path());
    }
    return out;
}

bool ExportPlugin::copy_directory_recursive(const fs::path& src,
                                              const fs::path& dst) const {
    try {
        fs::create_directories(dst);
        for (auto& e : fs::recursive_directory_iterator(src)) {
            auto rel = fs::relative(e.path(), src);
            auto target = dst / rel;
            if (e.is_directory()) fs::create_directories(target);
            else                   fs::copy_file(e.path(), target,
                                                  fs::copy_options::overwrite_existing);
        }
        return true;
    } catch (const std::exception& e) {
        ARX_LOG_ERROR("copy_directory_recursive: {}", e.what());
        return false;
    }
}

} // namespace arx

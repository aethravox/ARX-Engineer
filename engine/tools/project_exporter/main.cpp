// ==============================================================================
// tools/project_exporter/main.cpp — Exportar proyecto a binario desde CLI.
// Uso: project_exporter <project_dir> --platform <windows|linux|android|web>
//                              [--preset <name>] [--out <dir>]
// ==============================================================================
#include "export/export_plugin.hpp"
#include "core/logging.hpp"

#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: project_exporter <project_dir> --platform <name> [--out <dir>]\n";
        return 1;
    }

    std::string project_dir = argv[1];
    std::string platform;
    std::string out_dir;

    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--platform" && i + 1 < argc) platform = argv[++i];
        else if (a == "--out" && i + 1 < argc) out_dir = argv[++i];
        else if (a == "--help") {
            std::cout << "Uso: project_exporter <project_dir> --platform <windows|linux|android|web> [--out <dir>]\n";
            return 0;
        }
    }

    if (platform.empty()) {
        std::cerr << "Error: --platform requerido\n";
        return 1;
    }

    // Registrar exporters disponibles.
    arx::ExportPluginRegistry::instance().register_plugin(
        std::make_unique<arx_export::WindowsExporter>());
    arx::ExportPluginRegistry::instance().register_plugin(
        std::make_unique<arx_export::LinuxExporter>());
    arx::ExportPluginRegistry::instance().register_plugin(
        std::make_unique<arx_export::WebExporter>());
    arx::ExportPluginRegistry::instance().register_plugin(
        std::make_unique<arx_export::AndroidExporter>());

    arx::ExportPreset preset;
    preset.platform = platform;
    preset.app_name = "arx_app";
    preset.aot_arxscript = true;

    auto* plugin = arx::ExportPluginRegistry::instance().get_plugin(platform);
    if (!plugin) {
        std::cerr << "Error: no hay exporter para '" << platform << "'\n";
        return 1;
    }

    auto result = plugin->export_project(project_dir, preset);
    if (!result.success) {
        std::cerr << "Export fallido: " << result.error_message << "\n";
        return 1;
    }
    std::cout << "OK: " << result.output_path << " (" << result.total_bytes << " bytes)\n";
    std::cout << "  ARX files transpiled: " << result.arx_files_transpiled << "\n";
    std::cout << "  C++ files compiled:   " << result.source_files_compiled << "\n";
    return 0;
}

// ==============================================================================
// src/export/export_plugin.hpp — Base de exporters por plataforma.
// ==============================================================================
#pragma once

#include "core/types.hpp"
////#include "arxscript/transpiler.hpp"  // ARXScript removed

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace arx {

namespace fs = std::filesystem;

// ExportPreset: configuración de export (estilo Godot export_presets.cfg).
struct ExportPreset {
    std::string name;
    std::string platform;        // "windows", "linux", "android", "web"
    std::string target_arch;     // "x64", "arm64", "wasm32"
    bool        aot_arxscript   = true;   // Transpilar .arx → .cpp antes de compilar
    bool        embed_assets    = true;   // Empaquetar assets en el binario
    bool        debug_symbols   = false;
    bool        optimize        = true;
    std::string app_name;
    std::string app_version     = "1.0.0";
    std::string company_name;
    std::string icon_path;       // Ruta al ícono (.ico, .icns, .png)
    std::vector<std::string> extra_libs;
    std::vector<std::string> extra_flags;

    // Android específico.
    std::string android_package = "com.arx.game";
    int         android_min_sdk = 24;

    // Web específico.
    int         web_canvas_size_w = 1280;
    int         web_canvas_size_h = 720;
    bool        web_enable_threads = false;
};

// Resultado de exportación.
struct ExportResult {
    bool        success       = false;
    std::string output_path;
    std::string error_message;
    std::vector<std::string> logs;
    std::vector<std::string> warnings;
    int         arx_files_transpiled = 0;
    int         source_files_compiled = 0;
    int64_t     total_bytes = 0;
};

// ExportPlugin: interfaz base. Cada plataforma la implementa.
class ExportPlugin {
public:
    virtual ~ExportPlugin() = default;
    virtual std::string platform_id() const = 0;
    virtual std::string platform_name() const = 0;
    virtual bool can_export() const = 0;

    // Ejecuta el export completo: transpile ARX → C++ → compile → empaqueta.
    ExportResult export_project(const fs::path& project_root,
                                  const ExportPreset& preset);

protected:
    // Pasos que cada plataforma override.
    virtual bool transpile_arx_files(const fs::path& project_root,
                                      const ExportPreset& preset,
                                      ExportResult& result);
    virtual bool compile_runtime(const fs::path& project_root,
                                  const ExportPreset& preset,
                                  ExportResult& result) = 0;
    virtual bool package_output(const fs::path& project_root,
                                 const ExportPreset& preset,
                                 ExportResult& result) = 0;

    // Helpers compartidos.
    std::vector<fs::path> find_arx_files(const fs::path& root) const;
    std::vector<fs::path> find_asset_files(const fs::path& root) const;
    bool copy_directory_recursive(const fs::path& src, const fs::path& dst) const;

    ////arxscript::Transpiler transpiler_;  // ARXScript removed
};

// Registro de plugins.
class ExportPluginRegistry {
public:
    static ExportPluginRegistry& instance();
    void register_plugin(std::unique_ptr<ExportPlugin> p);
    ExportPlugin* get_plugin(const std::string& platform);
    std::vector<ExportPlugin*> all_plugins();
private:
    std::vector<std::unique_ptr<ExportPlugin>> plugins_;
};

} // namespace arx

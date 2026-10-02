// ==============================================================================
// engine/src/export/standalone_exporter.hpp
//
// StandaloneExporter — ExportPlugin que genera un ejecutable standalone
// SIN el motor ARX embebido. El .zen usa FFI para llamar a raylib u otras
// librerías directamente.
//
// Salida:
//   - Linux: ELF binario (sin extensión)
//   - Windows: .exe (PE)
//   - Android: .apk (con .so adentro)
//   - Web: .html + .wasm + .js
//
// Flujo:
//   1. Compilar todos los .zen del proyecto con Zen compiler (LLVM)
//   2. Linkear los .o resultantes contra:
//      - raylib (si el .zen usa FFI "raylib")
//      - libc del sistema target
//   3. Generar el ejecutable final
//
// No incluye:
//   - ARX OS runtime
//   - Sandbox
//   - .aex
//   - Firma criptográfica
// ==============================================================================

#pragma once

#include "export/export_plugin.hpp"

#include <string>

namespace arx {

class StandaloneExporter : public ExportPlugin {
public:
    StandaloneExporter(const std::string& platform) : platform_(platform) {}

    std::string platform_id()   const override { return "standalone_" + platform_; }
    std::string platform_name() const override {
        if (platform_ == "linux")   return "Standalone (Linux ELF)";
        if (platform_ == "windows") return "Standalone (Windows .exe)";
        if (platform_ == "android") return "Standalone (Android .apk)";
        if (platform_ == "web")     return "Standalone (Web .html)";
        return "Standalone (" + platform_ + ")";
    }

    // Se puede exportar si hay LLVM disponible y el target es soportado.
    bool can_export() const override;

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;

private:
    std::string platform_;  // "linux", "windows", "android", "web"

    // Mapea plataforma ARX a LLVM target triple
    std::string llvm_target_triple() const;

    // Mapea plataforma ARX a extensión del ejecutable
    std::string executable_extension() const;

    // Linka los .o generados contra las libs necesarias
    bool link_objects(const std::vector<std::string>& obj_paths,
                       const std::vector<std::string>& ffi_libs,
                       const fs::path& output_path,
                       ExportResult& result);
};

} // namespace arx

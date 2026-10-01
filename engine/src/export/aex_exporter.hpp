// ==============================================================================
// engine/src/export/aex_exporter.hpp
//
// AexExporter — ExportPlugin que genera un paquete .aex firmado.
//
// A diferencia de los demás exporters (Linux/Windows/Android/Web) que
// transpilan ARXScript a C++ y compilan un binario nativo por plataforma,
// este exporter empaqueta el proyecto tal cual:
//   - Manifest JSON con permisos
//   - Assets (TAR)
//   - Scripts (bytecode Luau en el futuro; por ahora ARXScript sin transpilar)
//   - Firma Ed25519 del creador
//
// El resultado es un .aex distribuíble que el ARX Client puede cargar
// y ejecutar sin necesidad de un compilador C++ en la máquina del usuario.
// ==============================================================================

#pragma once

#include "export/export_plugin.hpp"

#include <string>

namespace arx {

class AexExporter : public ExportPlugin {
public:
    std::string platform_id()   const override { return "aex"; }
    std::string platform_name() const override { return "ARX Package (.aex)"; }

    // Siempre se puede exportar a .aex (no requiere toolchain externo).
    bool can_export() const override { return true; }

    // Setter extra: ruta al archivo de clave privada del creador (64 bytes).
    // Si no se establece, se usará una clave efímera (solo para testing).
    void set_creator_key_path(const std::string& path) { creator_key_path_ = path; }
    const std::string& creator_key_path() const { return creator_key_path_; }

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;

private:
    std::string creator_key_path_;
};

} // namespace arx

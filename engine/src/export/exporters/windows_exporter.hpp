// ==============================================================================
// src/export/exporters/windows_exporter.hpp / .cpp
// Exporta a .exe (Windows x64). Usa MSVC (cl.exe) detectado del PATH.
// ==============================================================================
#pragma once

#include "export/export_plugin.hpp"

namespace arx {

class WindowsExporter : public ExportPlugin {
public:
    std::string platform_id()   const override { return "windows"; }
    std::string platform_name() const override { return "Windows x64"; }
    bool can_export() const override;

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;

private:
    std::string find_msvc_();
    std::string find_cl_exe_();
};

} // namespace arx

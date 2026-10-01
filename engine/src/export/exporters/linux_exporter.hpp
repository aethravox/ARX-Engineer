// ==============================================================================
// src/export/exporters/linux_exporter.hpp / .cpp
// ==============================================================================
#pragma once

#include "export/export_plugin.hpp"

namespace arx {

class LinuxExporter : public ExportPlugin {
public:
    std::string platform_id()   const override { return "linux"; }
    std::string platform_name() const override { return "Linux x64"; }
    bool can_export() const override {
        return std::system("which g++ > /dev/null 2>&1") == 0;
    }

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;
};

} // namespace arx

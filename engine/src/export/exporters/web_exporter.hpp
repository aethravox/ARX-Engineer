// ==============================================================================
// src/export/exporters/web_exporter.hpp / .cpp — Export a HTML5 via Emscripten.
// ==============================================================================
#pragma once

#include "export/export_plugin.hpp"

namespace arx {

class WebExporter : public ExportPlugin {
public:
    std::string platform_id()   const override { return "web"; }
    std::string platform_name() const override { return "Web / HTML5"; }
    bool can_export() const override {
        return std::system("which emcc > /dev/null 2>&1") == 0;
    }

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;

private:
    void write_html_shell_(const fs::path& path, const ExportPreset& preset);
};

} // namespace arx

// ==============================================================================
// src/export/exporters/android_exporter.hpp / .cpp — Export a APK.
// Requiere Android NDK + SDK. Genera un proyecto Gradle + compila con
// `./gradlew assembleRelease` y firma con debug keystore por defecto.
// ==============================================================================
#pragma once

#include "export/export_plugin.hpp"

namespace arx {

class AndroidExporter : public ExportPlugin {
public:
    std::string platform_id()   const override { return "android"; }
    std::string platform_name() const override { return "Android (APK)"; }
    bool can_export() const override {
        const char* ndk = std::getenv("ANDROID_NDK");
        const char* sdk = std::getenv("ANDROID_SDK_ROOT");
        return ndk && sdk;
    }

protected:
    bool compile_runtime(const fs::path& root, const ExportPreset& preset,
                          ExportResult& result) override;
    bool package_output(const fs::path& root, const ExportPreset& preset,
                         ExportResult& result) override;

private:
    void write_gradle_project_(const fs::path& dir, const ExportPreset& preset);
    void write_android_manifest_(const fs::path& dir, const ExportPreset& preset);
    void write_main_activity_(const fs::path& dir, const ExportPreset& preset);
};

} // namespace arx

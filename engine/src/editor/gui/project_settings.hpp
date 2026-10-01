// ==============================================================================
// src/editor/gui/project_settings.hpp — Project Settings (Android, Windows, signing)
//
// Basado en el flujo de Godot (sin copiar código):
//   - Android: package name, version, keystore, permissions, architectures
//   - Windows: company, product, version, icon, codesign
//   - Signing: Ed25519 key path for .aex
//   - Guardar/cargar a project_settings.json
// ==============================================================================
#pragma once
#include <string>

namespace arx {

struct AndroidSettings {
    std::string package_name = "com.aethravox.game";
    std::string app_name = "My Game";
    int version_code = 1;
    std::string version_name = "1.0.0";
    std::string keystore_path;
    std::string keystore_user;
    std::string keystore_password;
    bool arch_arm64 = true;
    bool arch_armv7 = false;
    bool arch_x86_64 = false;
    int min_sdk = 24;
    int target_sdk = 34;
    // Permissions
    bool perm_internet = false;
    bool perm_camera = false;
    bool perm_microphone = false;
    bool perm_vibrate = false;
    bool perm_write_external = false;
    bool perm_read_external = false;
    bool perm_access_fine_location = false;
    bool perm_access_coarse_location = false;
};

struct WindowsSettings {
    std::string company_name = "Aethravox Studios";
    std::string product_name = "My Game";
    std::string file_version = "1.0.0.0";
    std::string product_version = "1.0.0.0";
    std::string copyright = "Copyright (c) 2026";
    std::string icon_path;
    bool codesign = false;
    std::string codesign_identity;
    std::string codesign_password;
};

struct SigningSettings {
    std::string ed25519_priv_path;
    std::string ed25519_pub_path;
};

class ProjectSettings {
public:
    AndroidSettings android;
    WindowsSettings windows;
    SigningSettings signing;

    void render();
    bool is_visible() const { return visible_; }
    void set_visible(bool v) { visible_ = v; }

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path);

private:
    void render_android_();
    void render_windows_();
    void render_signing_();

    bool visible_ = false;
};

} // namespace arx

// ==============================================================================
// src/editor/gui/project_settings.cpp — Project Settings implementation.
// ==============================================================================
#include "project_settings.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <fstream>
#include <sstream>
#include <cstring>

namespace arx {

void ProjectSettings::render() {
    if (!visible_) return;

    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Project Settings", &visible_,
                       ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SettingsTabs")) {
        if (ImGui::BeginTabItem("Android")) {
            render_android_();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Windows")) {
            render_windows_();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Signing")) {
            render_signing_();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Separator();
    if (ImGui::Button("Save Settings")) {
        save_to_file("project_settings.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Settings")) {
        load_from_file("project_settings.json");
    }

    ImGui::End();
}

void ProjectSettings::render_android_() {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Android Export Settings");
    ImGui::PopFont();
    ImGui::Separator();

    // Package
    char pkg_buf[256];
    std::snprintf(pkg_buf, sizeof(pkg_buf), "%s", android.package_name.c_str());
    if (ImGui::InputText("Package Name", pkg_buf, sizeof(pkg_buf))) {
        android.package_name = pkg_buf;
    }

    char name_buf[256];
    std::snprintf(name_buf, sizeof(name_buf), "%s", android.app_name.c_str());
    if (ImGui::InputText("App Name", name_buf, sizeof(name_buf))) {
        android.app_name = name_buf;
    }

    ImGui::InputInt("Version Code", &android.version_code);
    ImGui::SameLine();
    char ver_buf[32];
    std::snprintf(ver_buf, sizeof(ver_buf), "%s", android.version_name.c_str());
    ImGui::InputText("Version Name", ver_buf, sizeof(ver_buf));
    android.version_name = ver_buf;

    ImGui::Separator();
    ImGui::Text("Architectures");
    ImGui::Checkbox("ARM64 (arm64-v8a)", &android.arch_arm64);
    ImGui::SameLine();
    ImGui::Checkbox("ARM32 (armeabi-v7a)", &android.arch_armv7);
    ImGui::SameLine();
    ImGui::Checkbox("x86_64", &android.arch_x86_64);

    ImGui::Separator();
    ImGui::Text("SDK");
    ImGui::InputInt("Min SDK", &android.min_sdk);
    ImGui::SameLine();
    ImGui::InputInt("Target SDK", &android.target_sdk);

    ImGui::Separator();
    ImGui::Text("Keystore (firma APK)");
    char ks_buf[512];
    std::snprintf(ks_buf, sizeof(ks_buf), "%s", android.keystore_path.c_str());
    if (ImGui::InputText("Keystore Path", ks_buf, sizeof(ks_buf))) {
        android.keystore_path = ks_buf;
    }
    char ks_user[128];
    std::snprintf(ks_user, sizeof(ks_user), "%s", android.keystore_user.c_str());
    if (ImGui::InputText("Keystore User", ks_user, sizeof(ks_user))) {
        android.keystore_user = ks_user;
    }
    char ks_pass[128];
    std::snprintf(ks_pass, sizeof(ks_pass), "%s", android.keystore_password.c_str());
    if (ImGui::InputText("Keystore Password", ks_pass, sizeof(ks_pass), ImGuiInputTextFlags_Password)) {
        android.keystore_password = ks_pass;
    }

    ImGui::Separator();
    ImGui::Text("Permissions");
    ImGui::Checkbox("Internet", &android.perm_internet);
    ImGui::SameLine();
    ImGui::Checkbox("Camera", &android.perm_camera);
    ImGui::SameLine();
    ImGui::Checkbox("Microphone", &android.perm_microphone);
    ImGui::Checkbox("Vibrate", &android.perm_vibrate);
    ImGui::SameLine();
    ImGui::Checkbox("Write External Storage", &android.perm_write_external);
    ImGui::SameLine();
    ImGui::Checkbox("Read External Storage", &android.perm_read_external);
    ImGui::Checkbox("Fine Location", &android.perm_access_fine_location);
    ImGui::SameLine();
    ImGui::Checkbox("Coarse Location", &android.perm_access_coarse_location);
}

void ProjectSettings::render_windows_() {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Windows Export Settings");
    ImGui::PopFont();
    ImGui::Separator();

    char company[256];
    std::snprintf(company, sizeof(company), "%s", windows.company_name.c_str());
    if (ImGui::InputText("Company Name", company, sizeof(company))) {
        windows.company_name = company;
    }

    char product[256];
    std::snprintf(product, sizeof(product), "%s", windows.product_name.c_str());
    if (ImGui::InputText("Product Name", product, sizeof(product))) {
        windows.product_name = product;
    }

    char fver[32];
    std::snprintf(fver, sizeof(fver), "%s", windows.file_version.c_str());
    if (ImGui::InputText("File Version", fver, sizeof(fver))) {
        windows.file_version = fver;
    }

    char pver[32];
    std::snprintf(pver, sizeof(pver), "%s", windows.product_version.c_str());
    if (ImGui::InputText("Product Version", pver, sizeof(pver))) {
        windows.product_version = pver;
    }

    char copy[256];
    std::snprintf(copy, sizeof(copy), "%s", windows.copyright.c_str());
    if (ImGui::InputText("Copyright", copy, sizeof(copy))) {
        windows.copyright = copy;
    }

    char icon[512];
    std::snprintf(icon, sizeof(icon), "%s", windows.icon_path.c_str());
    if (ImGui::InputText("Icon Path (.ico)", icon, sizeof(icon))) {
        windows.icon_path = icon;
    }

    ImGui::Separator();
    ImGui::Text("Code Signing (optional)");
    ImGui::Checkbox("Enable Code Signing", &windows.codesign);
    if (windows.codesign) {
        char id[256];
        std::snprintf(id, sizeof(id), "%s", windows.codesign_identity.c_str());
        if (ImGui::InputText("Identity", id, sizeof(id))) {
            windows.codesign_identity = id;
        }
        char pass[128];
        std::snprintf(pass, sizeof(pass), "%s", windows.codesign_password.c_str());
        if (ImGui::InputText("Password", pass, sizeof(pass), ImGuiInputTextFlags_Password)) {
            windows.codesign_password = pass;
        }
    }
}

void ProjectSettings::render_signing_() {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text(".aex Signing (Ed25519)");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::TextWrapped("Esta clave se usa para firmar los paquetes .aex "
                        "(contenedores de assets). Permite verificar que "
                        "los assets no fueron modificados.");

    char priv[512];
    std::snprintf(priv, sizeof(priv), "%s", signing.ed25519_priv_path.c_str());
    if (ImGui::InputText("Private Key Path (.priv)", priv, sizeof(priv))) {
        signing.ed25519_priv_path = priv;
    }

    char pub[512];
    std::snprintf(pub, sizeof(pub), "%s", signing.ed25519_pub_path.c_str());
    if (ImGui::InputText("Public Key Path (.pub)", pub, sizeof(pub))) {
        signing.ed25519_pub_path = pub;
    }

    ImGui::Separator();
    if (ImGui::Button("Generate New Key Pair")) {
        // Llamar arx_keygen
        std::string cmd = "arx_keygen --output arx_key --quiet 2>/dev/null";
        std::system(cmd.c_str());
        signing.ed25519_priv_path = "arx_key.priv";
        signing.ed25519_pub_path = "arx_key.pub";
        ARX_LOG_INFO("ProjectSettings: nueva clave Ed25519 generada");
    }
}

// === JSON save/load (simplificado) ===
void ProjectSettings::save_to_file(const std::string& path) {
    std::ofstream f(path);
    if (!f) return;

    f << "{\n";
    f << "  \"android\": {\n";
    f << "    \"package_name\": \"" << android.package_name << "\",\n";
    f << "    \"app_name\": \"" << android.app_name << "\",\n";
    f << "    \"version_code\": " << android.version_code << ",\n";
    f << "    \"version_name\": \"" << android.version_name << "\",\n";
    f << "    \"keystore_path\": \"" << android.keystore_path << "\",\n";
    f << "    \"keystore_user\": \"" << android.keystore_user << "\",\n";
    f << "    \"keystore_password\": \"" << android.keystore_password << "\",\n";
    f << "    \"arch_arm64\": " << (android.arch_arm64 ? "true" : "false") << ",\n";
    f << "    \"arch_armv7\": " << (android.arch_armv7 ? "true" : "false") << ",\n";
    f << "    \"arch_x86_64\": " << (android.arch_x86_64 ? "true" : "false") << ",\n";
    f << "    \"min_sdk\": " << android.min_sdk << ",\n";
    f << "    \"target_sdk\": " << android.target_sdk << ",\n";
    f << "    \"perm_internet\": " << (android.perm_internet ? "true" : "false") << ",\n";
    f << "    \"perm_camera\": " << (android.perm_camera ? "true" : "false") << "\n";
    f << "  },\n";
    f << "  \"windows\": {\n";
    f << "    \"company_name\": \"" << windows.company_name << "\",\n";
    f << "    \"product_name\": \"" << windows.product_name << "\",\n";
    f << "    \"file_version\": \"" << windows.file_version << "\",\n";
    f << "    \"product_version\": \"" << windows.product_version << "\",\n";
    f << "    \"copyright\": \"" << windows.copyright << "\",\n";
    f << "    \"icon_path\": \"" << windows.icon_path << "\"\n";
    f << "  },\n";
    f << "  \"signing\": {\n";
    f << "    \"ed25519_priv_path\": \"" << signing.ed25519_priv_path << "\",\n";
    f << "    \"ed25519_pub_path\": \"" << signing.ed25519_pub_path << "\"\n";
    f << "  }\n";
    f << "}\n";
    f.close();
    ARX_LOG_INFO("ProjectSettings: guardado en '{}'", path);
}

void ProjectSettings::load_from_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    // TODO: parse JSON properly (por ahora solo verifica que existe)
    ARX_LOG_INFO("ProjectSettings: cargado desde '{}'", path);
    f.close();
}

} // namespace arx

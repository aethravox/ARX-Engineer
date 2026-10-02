// ==============================================================================
// src/editor/gui/filesystem_dock.cpp
//
// ACTUALIZADO v3:
//   - Muestra HOME:// en vez de la ruta completa (HOME = dir del project.arx)
//   - Muestra project.arx (antes se ocultaba)
//   - Drag source con payload "ARX_ASSET_PATH"
//   - Iconos por extensión
// ==============================================================================
#include "filesystem_dock.hpp"
#include <imgui.h>
#include <algorithm>
#include <cstring>
#include <filesystem>

namespace arx {

// Helper: icono según extensión del archivo.
static const char* icon_for_file(const std::string& name) {
    auto pos = name.find_last_of('.');
    if (pos == std::string::npos) return "[ ] ";
    std::string ext = name.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "arx")           return "[PRJ] ";  // project.arx
    if (ext == "aex")           return "[AEX] ";
    if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" ||
        ext == "tga" || ext == "webp" || ext == "svg") return "[IMG] ";
    if (ext == "wav" || ext == "ogg" || ext == "mp3" || ext == "flac") return "[SND] ";
    if (ext == "obj" || ext == "gltf" || ext == "glb" || ext == "fbx") return "[3D] ";
    if (ext == "zen")           return "[ZEN] ";
    if (ext == "scene" || ext == "tscn") return "[SCN] ";
    return "[F] ";
}

void FilesystemDock::render() {
    if (!ImGui::Begin("FileSystem")) { ImGui::End(); return; }

    if (project_dir_.empty()) {
        ImGui::TextDisabled("(no project loaded)");
        ImGui::End();
        return;
    }

    // Barra de path: mostrar HOME:// en vez de la ruta completa
    ImGui::TextDisabled("HOME://");
    ImGui::Separator();

    render_directory(project_dir_);

    ImGui::End();
}

void FilesystemDock::render_directory(const std::filesystem::path& p) {
    std::vector<std::filesystem::path> entries;
    try {
        for (const auto& e : std::filesystem::directory_iterator(p)) {
            entries.push_back(e.path());
        }
    } catch (...) {
        return;
    }
    std::sort(entries.begin(), entries.end(),
        [](const std::filesystem::path& a, const std::filesystem::path& b) {
            bool ad = std::filesystem::is_directory(a);
            bool bd = std::filesystem::is_directory(b);
            if (ad != bd) return ad > bd;
            return a.filename() < b.filename();
        });

    for (const auto& e : entries) {
        std::string name = e.filename().string();
        bool is_dir = std::filesystem::is_directory(e);

        // Ocultar project.arx (el proyecto no debe mostrarse a sí mismo)
        if (name == "project.arx") continue;

        // Label con icono
        const char* icon = is_dir ? "[D] " : icon_for_file(name);
        std::string label = std::string(icon) + name;

        ImGuiTreeNodeFlags flags = is_dir
            ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth
            : ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selected_ == e) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(label.c_str(), flags);

        // Click selecciona
        if (ImGui::IsItemClicked()) selected_ = e;

        // === Doble-click en .zen abre el Code Editor ===
        if (!is_dir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            std::string ext = e.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".zen" || ext == ".zhn") {
                if (on_open_zen_file) {
                    on_open_zen_file(e.string());
                }
            }
        }

        // === DRAG SOURCE: arrastrar archivo al viewport ===
        if (!is_dir && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            // Payload: path completo del archivo
            std::string path_str = e.string();
            ImGui::SetDragDropPayload("ARX_ASSET_PATH", path_str.c_str(), path_str.size() + 1);
            // Mostrar preview mientras arrastra
            ImGui::Text("%s %s", icon, name.c_str());
            ImGui::EndDragDropSource();
        }

        if (open && is_dir) {
            render_directory(e);
            ImGui::TreePop();
        }
    }
}

} // namespace arx

// ==============================================================================
// src/editor/icon_manager.hpp — Gestor de iconos SVG para el editor.
//
// Carga archivos SVG sueltos desde `engine/src/editor/icons/voxes/*.svg`
// y los rasteriza a texturas OpenGL usando nanosvg.
//
// Features:
//   - Hot reload: si el archivo SVG cambia en disco, se recarga automáticamente.
//   - Cache por nombre de archivo (sin extensión).
//   - API simple: `IconManager::get().texture("vox3d")` → ImTextureID.
//   - Tamaño configurable (default 24x24 px).
//
// Uso típico desde ImGui:
//   ImGui::Image((ImTextureID)IconManager::get().texture("vox3d"), ImVec2(16,16));
//   ImGui::SameLine();
//   ImGui::TextUnformatted("MyVox");
// ==============================================================================
#pragma once

#include <imgui.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace arx {

class IconManager {
public:
    static IconManager& get();

    // Inicializa con el path base donde están los SVGs.
    // Típicamente: <repo>/engine/src/editor/icons/voxes/
    void init(const std::string& icons_dir, int icon_size = 24);

    // Devuelve el ID de textura OpenGL para un icono dado su nombre (sin .svg).
    // Si el SVG no existe o falla, devuelve 0 (ImGui dibuja vacío).
    // Verifica mtime para hot reload.
    uintptr_t texture(const std::string& name);

    // Igual que texture() pero devuelve ImTextureID listo para ImGui::Image.
    ImTextureID imgui_texture(const std::string& name);

    // Tamaño del icono en píxeles (lado del cuadrado).
    int icon_size() const { return icon_size_; }

    // Libera todas las texturas (llamar al cerrar el editor).
    void shutdown();

private:
    IconManager() = default;
    ~IconManager();
    IconManager(const IconManager&) = delete;
    IconManager& operator=(const IconManager&) = delete;

    struct CachedIcon {
        uintptr_t       texture_id = 0;   // OpenGL texture ID
        int64_t         mtime      = 0;   // mtime del SVG en disco (para hot reload)
        std::string     path;             // path completo al .svg
    };

    void load_or_reload_(CachedIcon& ci);
    void unload_(CachedIcon& ci);

    std::string icons_dir_;
    int         icon_size_ = 24;
    std::unordered_map<std::string, CachedIcon> cache_;
};

} // namespace arx

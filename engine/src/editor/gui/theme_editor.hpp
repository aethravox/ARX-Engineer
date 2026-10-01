// ==============================================================================
// src/editor/gui/theme_editor.hpp — Editor de tema visual en vivo.
//
// Permite cambiar los colores del editor sin recompilar. Guarda/carga desde
// arx_theme.json.
//
// Features:
//   - Color picker para cada ImGuiCol_*
//   - Presets: ARX Dark, Godot, VS Code, Light, Solarized
//   - Save/Load a arx_theme.json
//   - Reset al default
//   - Sliders de rounding/border/padding
// ==============================================================================
#pragma once

#include <string>

namespace arx {

class ThemeEditor {
public:
    void render();

    // Cargar tema desde archivo (si existe)
    void load_from_file(const std::string& path);

    // Guardar tema actual a archivo
    void save_to_file(const std::string& path);

    // Aplicar un preset por nombre
    void apply_preset(const std::string& name);

    // ¿Está visible el panel?
    bool is_visible() const { return visible_; }
    void set_visible(bool v) { visible_ = v; }

private:
    void render_color_editor_();
    void render_style_editor_();
    void render_presets_();

    bool visible_ = false;
    std::string current_preset_ = "ARX Dark";
};

} // namespace arx

// ==============================================================================
// src/theme/theme.hpp — Sistema de temas para UI.
// Permite definir colores, fonts, sizes por nombre y aplicarlos a los widgets.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <unordered_map>
#include <memory>

namespace arx {

class Theme : public Object {
public:
    ARX_CLASS(Theme, Object);
public:

    // Estilos predefinidos.
    enum class Preset { Default, Dark, Light, ARX, Cyberpunk, Retro };

    static std::shared_ptr<Theme> create_preset(Preset p);
    static std::shared_ptr<Theme> get_default() {
        static auto t = create_preset(Preset::ARX);
        return t;
    }

    // Colors por categoría.
    void set_color(const std::string& key, Color c) { colors_[key] = c; }
    Color get_color(const std::string& key, Color fallback = Color::white) const {
        auto it = colors_.find(key);
        return it != colors_.end() ? it->second : fallback;
    }

    // Font sizes.
    void set_font_size(const std::string& key, int s) { font_sizes_[key] = s; }
    int  get_font_size(const std::string& key, int fallback = 14) const {
        auto it = font_sizes_.find(key);
        return it != font_sizes_.end() ? it->second : fallback;
    }

    // Constantes por categoría.
    void set_constant(const std::string& key, int v) { constants_[key] = v; }
    int  get_constant(const std::string& key, int fallback = 0) const {
        auto it = constants_.find(key);
        return it != constants_.end() ? it->second : fallback;
    }

    // Aplicar a un control.
    void apply_to(class Control* c) const;

private:
    std::unordered_map<std::string, Color> colors_;
    std::unordered_map<std::string, int>   font_sizes_;
    std::unordered_map<std::string, int>   constants_;
};

} // namespace arx

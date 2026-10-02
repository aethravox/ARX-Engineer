// ==============================================================================
// src/scene/ui/theme.hpp — Temas UI: colores, fuentes, padding por defecto.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <unordered_map>
#include <memory>

namespace arx::ui {

class Font;

class Theme : public Object {
public:
    ARX_CLASS(Theme, Object);
public:

    // Color roles (estilo CSS).
    enum class ColorRole {
        Bg, BgHover, BgPressed, BgDisabled,
        Fg, FgHover, FgPressed, FgDisabled,
        Border, BorderFocus,
        Accent, AccentHover, AccentPressed,
        Success, Warning, Error, Info,
        Scrollbar, ScrollbarHover
    };

    // Size roles.
    enum class SizeRole {
        FontSizeSmall, FontSizeNormal, FontSizeLarge, FontSizeTitle,
        SpacingSmall, SpacingNormal, SpacingLarge,
        BorderWidth, CornerRadius, ScrollbarSize
    };

    // Constant defaults (dark theme ARX).
    void set_default_dark_theme();
    void set_default_light_theme();

    Color  get_color(ColorRole r) const { return colors_.at(r); }
    void   set_color(ColorRole r, Color c) { colors_[r] = c; }
    float  get_size(SizeRole r) const { return sizes_.at(r); }
    void   set_size(SizeRole r, float s) { sizes_[r] = s; }

    void   set_font(const std::string& role, std::shared_ptr<Font> f);
    std::shared_ptr<Font> get_font(const std::string& role) const;

    static Theme* get_default();

private:
    std::unordered_map<ColorRole, Color>   colors_;
    std::unordered_map<SizeRole, float>    sizes_;
    std::unordered_map<std::string, std::shared_ptr<Font>> fonts_;
};

// ==============================================================================
// Font — wrapper sobre FreeType para rasterizar glifos.
// ==============================================================================
class Font : public Object {
public:
    ARX_CLASS(Font, Object);
public:

    bool load_from_file(const std::string& path);
    bool load_from_memory(const uint8_t* data, size_t size);

    int  get_size() const { return size_; }
    void set_size(int px);

    // Métricas.
    int  get_ascent()  const;
    int  get_descent() const;
    int  get_line_height() const;

    // Render: rasteriza el glifo de un codepoint. Devuelve el bitmap en
    // escala de grises (un canal). Ancho/alto de salida en out_w/out_h.
    std::vector<uint8_t> rasterize_char(uint32_t codepoint,
                                          int* out_w = nullptr,
                                          int* out_h = nullptr,
                                          int* out_advance = nullptr,
                                          int* out_bearing_x = nullptr,
                                          int* out_bearing_y = nullptr);

    // Mide el ancho de un string.
    int  measure_text(const std::string& text) const;

    // Cache de glifos (LRU).
    struct Glyph {
        uint32_t codepoint;
        int      width, height;
        int      advance;
        int      bearing_x, bearing_y;
        std::vector<uint8_t> bitmap;
    };
    const Glyph& get_glyph(uint32_t codepoint);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> p_;
    int size_ = 16;
    std::unordered_map<uint32_t, Glyph> glyph_cache_;
};

} // namespace arx::ui

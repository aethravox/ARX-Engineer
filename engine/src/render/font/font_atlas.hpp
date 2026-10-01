// ==============================================================================
// src/render/font/font_atlas.hpp — Glyph atlas con FreeType.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "render/texture.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace arx {

struct Glyph {
    uint32_t codepoint = 0;
    Rect2    region;           // En el atlas.
    Vector2  size;
    Vector2  bearing;          // Offset desde el baseline.
    float    advance = 0.0f;   // Avance horizontal.
};

class FontAtlas {
public:
    FontAtlas();
    ~FontAtlas();

    bool load_font(const std::string& path, int size = 16);
    bool load_system_default(int size = 16);

    const Glyph* get_glyph(uint32_t codepoint) const;
    std::shared_ptr<Texture> get_atlas_texture() const { return atlas_texture_; }

    int  get_font_size() const { return font_size_; }
    int  get_line_height() const { return line_height_; }
    int  get_ascent() const { return ascent_; }
    int  get_descent() const { return descent_; }

    // Mide el tamaño de un texto.
    Vector2 measure_text(const std::string& text) const;
    Vector2 measure_text(const std::string& text, int size) const;

    // Renderiza texto al atlas del renderer actual (placeholder).
    void draw_text(const std::string& text, Vector2 position,
                    Color color, int size = -1) const;

private:
    bool rasterize_atlas();
    bool rasterize_range(uint32_t from, uint32_t to);

    int      font_size_   = 16;
    int      line_height_ = 0;
    int      ascent_      = 0;
    int      descent_     = 0;
    std::unordered_map<uint32_t, Glyph> glyphs_;
    std::shared_ptr<Texture> atlas_texture_;

    void*    ft_library_  = nullptr;
    void*    ft_face_     = nullptr;
    std::string font_path_;
};

// FontServer — cache de fonts cargadas.
class FontServer {
public:
    static FontServer& instance();
    std::shared_ptr<FontAtlas> load_font(const std::string& path, int size = 16);
    std::shared_ptr<FontAtlas> get_default(int size = 16);

private:
    FontServer() = default;
    std::unordered_map<std::string, std::shared_ptr<FontAtlas>> cache_;
};

} // namespace arx

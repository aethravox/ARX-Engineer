#include "render/renderer.hpp"
// ==============================================================================
// src/render/font/font_atlas.cpp
// ==============================================================================
#include "font_atlas.hpp"
#include "core/logging.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fstream>
#include <cstring>

namespace arx {

FontAtlas::FontAtlas() {
    FT_Library lib;
    if (FT_Init_FreeType(&lib) == 0) {
        ft_library_ = lib;
    }
}

FontAtlas::~FontAtlas() {
    if (ft_face_) FT_Done_Face(static_cast<FT_Face>(ft_face_));
    if (ft_library_) FT_Done_FreeType(static_cast<FT_Library>(ft_library_));
}

bool FontAtlas::load_font(const std::string& path, int size) {
    if (!ft_library_) return false;
    font_size_ = size;
    font_path_ = path;

    FT_Face face;
    if (FT_New_Face(static_cast<FT_Library>(ft_library_), path.c_str(), 0, &face) != 0) {
        ARX_LOG_ERROR("FontAtlas: no se pudo cargar '{}'", path);
        return false;
    }
    if (FT_Set_Pixel_Sizes(face, 0, size) != 0) {
        ARX_LOG_ERROR("FontAtlas: no se pudo setear size {}", size);
        FT_Done_Face(face);
        return false;
    }
    ft_face_ = face;
    ascent_      = face->size->metrics.ascender  >> 6;
    descent_     = face->size->metrics.descender >> 6;
    line_height_ = face->size->metrics.height    >> 6;

    return rasterize_atlas();
}

bool FontAtlas::load_system_default(int size) {
#ifdef _WIN32
    return load_font("C:/Windows/Fonts/arial.ttf", size);
#else
    return load_font("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", size);
#endif
}

bool FontAtlas::rasterize_atlas() {
    // Rasterizar ASCII 32..126.
    return rasterize_range(32, 126);
}

bool FontAtlas::rasterize_range(uint32_t from, uint32_t to) {
    FT_Face face = static_cast<FT_Face>(ft_face_);
    if (!face) return false;

    // Layout simple: 16 columnas x N filas.
    constexpr int ATLAS_W = 512;
    int atlas_h = 512;
    int cell_w = font_size_ + 4;
    int cell_h = font_size_ + 4;
    int cols = ATLAS_W / cell_w;

    std::vector<uint8_t> atlas_data(ATLAS_W * atlas_h, 0);

    int idx = 0;
    for (uint32_t cp = from; cp <= to; ++cp) {
        if (FT_Load_Char(face, cp, FT_LOAD_RENDER) != 0) continue;
        FT_GlyphSlot g = face->glyph;

        int col = idx % cols;
        int row = idx / cols;
        int x0  = col * cell_w;
        int y0  = row * cell_h;

        // Copiar bitmap al atlas.
        for (unsigned int y = 0; y < g->bitmap.rows; ++y) {
            for (unsigned int x = 0; x < g->bitmap.width; ++x) {
                int ax = x0 + x;
                int ay = y0 + y;
                if (ax >= ATLAS_W || ay >= atlas_h) continue;
                atlas_data[ay * ATLAS_W + ax] = g->bitmap.buffer[y * g->bitmap.pitch + x];
            }
        }

        Glyph glyph;
        glyph.codepoint = cp;
        glyph.region    = Rect2(x0, y0, g->bitmap.width, g->bitmap.rows);
        glyph.size      = Vector2(g->bitmap.width, g->bitmap.rows);
        glyph.bearing   = Vector2(g->bitmap_left, g->bitmap_top);
        glyph.advance   = (float)(g->advance.x >> 6);
        glyphs_[cp] = glyph;

        ++idx;
    }

    // Crear textura (1 canal).
    // atlas_texture_ = renderer->create_texture(ATLAS_W, atlas_h, 1, atlas_data.data());
    return true;
}

const Glyph* FontAtlas::get_glyph(uint32_t cp) const {
    auto it = glyphs_.find(cp);
    return it != glyphs_.end() ? &it->second : nullptr;
}

Vector2 FontAtlas::measure_text(const std::string& text) const {
    return measure_text(text, font_size_);
}

Vector2 FontAtlas::measure_text(const std::string& text, int /*size*/) const {
    float w = 0, h = line_height_;
    for (unsigned char c : text) {
        if (const Glyph* g = get_glyph(c)) {
            w += g->advance;
        }
    }
    return { w, h };
}

void FontAtlas::draw_text(const std::string& /*text*/, Vector2 /*pos*/,
                            Color /*color*/, int /*size*/) const {
    // En una impl completa: dibujar quads con la textura del atlas.
}

// ===================== FontServer ============================================
FontServer& FontServer::instance() {
    static FontServer s;
    return s;
}

std::shared_ptr<FontAtlas> FontServer::load_font(const std::string& path, int size) {
    std::string key = path + ":" + std::to_string(size);
    auto it = cache_.find(key);
    if (it != cache_.end()) return it->second;
    auto atlas = std::make_shared<FontAtlas>();
    if (atlas->load_font(path, size)) {
        cache_[key] = atlas;
        return atlas;
    }
    return nullptr;
}

std::shared_ptr<FontAtlas> FontServer::get_default(int size) {
    static std::shared_ptr<FontAtlas> def;
    if (!def) {
        def = std::make_shared<FontAtlas>();
        def->load_system_default(size);
    }
    return def;
}

} // namespace arx

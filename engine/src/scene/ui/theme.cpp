// ==============================================================================
// src/scene/ui/theme.cpp
// ==============================================================================
#include "theme.hpp"
#include "core/logging.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace arx::ui {

// ===================== Theme =================================================
void Theme::set_default_dark_theme() {
    colors_[ColorRole::Bg]          = Color::from_hex(0x1A1A22FF);
    colors_[ColorRole::BgHover]      = Color::from_hex(0x2A2A35FF);
    colors_[ColorRole::BgPressed]    = Color::from_hex(0x36363FFF);
    colors_[ColorRole::BgDisabled]   = Color::from_hex(0x141418FF);
    colors_[ColorRole::Fg]           = Color::from_hex(0xE6E6ECFF);
    colors_[ColorRole::FgHover]      = Color::from_hex(0xFFFFFFFF);
    colors_[ColorRole::FgPressed]    = Color::from_hex(0xFFFFFFFF);
    colors_[ColorRole::FgDisabled]   = Color::from_hex(0x7F7F88FF);
    colors_[ColorRole::Border]       = Color::from_hex(0x3A3A45FF);
    colors_[ColorRole::BorderFocus]  = Color::arx_cyan;
    colors_[ColorRole::Accent]       = Color::arx_purple;
    colors_[ColorRole::AccentHover]  = Color::from_hex(0x8B55E6FF);
    colors_[ColorRole::AccentPressed]= Color::from_hex(0x4A2299FF);
    colors_[ColorRole::Success]      = Color::from_hex(0x4ADE80FF);
    colors_[ColorRole::Warning]      = Color::from_hex(0xFACC15FF);
    colors_[ColorRole::Error]        = Color::from_hex(0xF87171FF);
    colors_[ColorRole::Info]         = Color::from_hex(0x60A5FAFF);
    colors_[ColorRole::Scrollbar]    = Color::from_hex(0x4D4D58FF);
    colors_[ColorRole::ScrollbarHover] = Color::from_hex(0x666671FF);

    sizes_[SizeRole::FontSizeSmall]  = 12.0f;
    sizes_[SizeRole::FontSizeNormal] = 14.0f;
    sizes_[SizeRole::FontSizeLarge]  = 18.0f;
    sizes_[SizeRole::FontSizeTitle]  = 28.0f;
    sizes_[SizeRole::SpacingSmall]   = 4.0f;
    sizes_[SizeRole::SpacingNormal]  = 8.0f;
    sizes_[SizeRole::SpacingLarge]   = 16.0f;
    sizes_[SizeRole::BorderWidth]    = 1.0f;
    sizes_[SizeRole::CornerRadius]   = 4.0f;
    sizes_[SizeRole::ScrollbarSize]  = 12.0f;
}

void Theme::set_default_light_theme() {
    colors_[ColorRole::Bg]          = Color::from_hex(0xFFFFFFFF);
    colors_[ColorRole::BgHover]      = Color::from_hex(0xF0F0F5FF);
    colors_[ColorRole::BgPressed]    = Color::from_hex(0xE0E0E8FF);
    colors_[ColorRole::Fg]           = Color::from_hex(0x1A1A22FF);
    colors_[ColorRole::Border]       = Color::from_hex(0xC0C0CCFF);
    colors_[ColorRole::Accent]       = Color::arx_purple;
    colors_[ColorRole::AccentHover]  = Color::from_hex(0x8B55E6FF);
    sizes_[SizeRole::FontSizeNormal] = 14.0f;
    sizes_[SizeRole::CornerRadius]   = 4.0f;
}

void Theme::set_font(const std::string& role, std::shared_ptr<Font> f) {
    fonts_[role] = std::move(f);
}
std::shared_ptr<Font> Theme::get_font(const std::string& role) const {
    auto it = fonts_.find(role);
    if (it != fonts_.end()) return it->second;
    auto def = fonts_.find("default");
    return def != fonts_.end() ? def->second : nullptr;
}

Theme* Theme::get_default() {
    static Theme t;
    static bool inited = false;
    if (!inited) {
        t.set_default_dark_theme();
        inited = true;
    }
    return &t;
}

// ===================== Font ==================================================
struct Font::Pimpl {
    FT_Library library = nullptr;
    FT_Face    face    = nullptr;
};

bool Font::load_from_file(const std::string& path) {
    if (!p_) p_ = std::make_unique<Pimpl>();
    if (!p_->library) {
        if (FT_Init_FreeType(&p_->library)) {
            ARX_LOG_ERROR("Font: no se pudo inicializar FreeType");
            return false;
        }
    }
    if (FT_New_Face(p_->library, path.c_str(), 0, &p_->face)) {
        ARX_LOG_ERROR("Font: no se pudo cargar {}", path);
        return false;
    }
    set_size(size_);
    ARX_LOG_INFO("Font cargada: {} ({} glifos)", path,
                 p_->face->num_glyphs);
    return true;
}

bool Font::load_from_memory(const uint8_t* data, size_t size) {
    if (!p_) p_ = std::make_unique<Pimpl>();
    if (!p_->library) FT_Init_FreeType(&p_->library);
    if (FT_New_Memory_Face(p_->library, data, static_cast<FT_Long>(size),
                            0, &p_->face)) {
        return false;
    }
    set_size(size_);
    return true;
}

void Font::set_size(int px) {
    size_ = px;
    if (p_ && p_->face) FT_Set_Pixel_Sizes(p_->face, 0, px);
    glyph_cache_.clear();
}

int Font::get_ascent() const {
    if (!p_ || !p_->face) return 0;
    return p_->face->size->metrics.ascender >> 6;
}
int Font::get_descent() const {
    if (!p_ || !p_->face) return 0;
    return p_->face->size->metrics.descender >> 6;
}
int Font::get_line_height() const {
    if (!p_ || !p_->face) return size_;
    return p_->face->size->metrics.height >> 6;
}

std::vector<uint8_t> Font::rasterize_char(uint32_t codepoint,
                                            int* out_w, int* out_h,
                                            int* out_advance,
                                            int* out_bearing_x,
                                            int* out_bearing_y) {
    if (!p_ || !p_->face) return {};
    FT_UInt glyph_index = FT_Get_Char_Index(p_->face, codepoint);
    if (FT_Load_Glyph(p_->face, glyph_index, FT_LOAD_DEFAULT)) return {};
    if (FT_Render_Glyph(p_->face->glyph, FT_RENDER_MODE_NORMAL)) return {};

    auto* slot = p_->face->glyph;
    if (out_w)         *out_w         = static_cast<int>(slot->bitmap.width);
    if (out_h)         *out_h         = static_cast<int>(slot->bitmap.rows);
    if (out_advance)   *out_advance   = slot->advance.x >> 6;
    if (out_bearing_x) *out_bearing_x = slot->bitmap_left;
    if (out_bearing_y) *out_bearing_y = slot->bitmap_top;

    std::vector<uint8_t> out(slot->bitmap.width * slot->bitmap.rows);
    std::memcpy(out.data(), slot->bitmap.buffer, out.size());
    return out;
}

int Font::measure_text(const std::string& text) const {
    if (!p_ || !p_->face) return 0;
    int total = 0;
    for (unsigned char c : text) {
        FT_UInt gi = FT_Get_Char_Index(p_->face, c);
        if (FT_Load_Glyph(p_->face, gi, FT_LOAD_DEFAULT)) continue;
        total += p_->face->glyph->advance.x >> 6;
    }
    return total;
}

const Font::Glyph& Font::get_glyph(uint32_t codepoint) {
    auto it = glyph_cache_.find(codepoint);
    if (it != glyph_cache_.end()) return it->second;

    Glyph g;
    g.codepoint = codepoint;
    g.bitmap = rasterize_char(codepoint, &g.width, &g.height, &g.advance,
                                &g.bearing_x, &g.bearing_y);
    auto [inserted, _] = glyph_cache_.emplace(codepoint, std::move(g));
    return inserted->second;
}

} // namespace arx::ui

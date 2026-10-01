#include "scene/scene_tree.hpp"
// ==============================================================================
// src/scene/2d/sprite_2d.hpp
// ==============================================================================
#pragma once

#include "vox2d.hpp"
#include "render/renderer.hpp"
#include "render/texture.hpp"

#include <memory>

namespace arx {

class VoxSprite2D : public Vox2D {
public:
    ARX_CLASS(VoxSprite2D, Vox2D);
public:

    void set_texture(std::shared_ptr<Texture> tex) { texture_ = tex; }
    std::shared_ptr<Texture> get_texture() const { return texture_; }

    void set_modulate(Color c) { modulate_ = c; }
    Color get_modulate() const { return modulate_; }

    void set_region(const Rect2& r) { region_ = r; has_region_ = true; }
    void clear_region() { has_region_ = false; }

    void set_size(Vector2 s) { size_ = s; has_size_ = true; }
    Vector2 get_size() const {
        if (has_size_) return size_;
        if (texture_) return Vector2(texture_->width(), texture_->height());
        return {1,1};
    }

    void draw() override {
        if (!texture_) return;
        if (auto* tree = get_tree()) {
            if (auto* r = tree->get_renderer()) {
                r->draw_sprite_2d(texture_, position_ - get_size() * 0.5f,
                                  get_size(), rotation_, modulate_,
                                  has_region_ ? &region_ : nullptr);
            }
        }
    }

private:
    std::shared_ptr<Texture> texture_;
    Color    modulate_   = Color::white;
    Rect2    region_;
    bool     has_region_ = false;
    Vector2  size_{1,1};
    bool     has_size_   = false;
};

} // namespace arx

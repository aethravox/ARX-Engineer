// ==============================================================================
// src/scene/2d/camera_2d.hpp, label.hpp — Nodos 2D adicionales.
// ==============================================================================
#pragma once

#include "vox2d.hpp"
#include "core/types.hpp"

namespace arx {

class VoxCamera2D : public Vox2D {
public:
    ARX_CLASS(VoxCamera2D, Vox2D);
public:

    void set_zoom(float z)  { zoom_ = z; }
    float get_zoom() const  { return zoom_; }

    void set_offset(Vector2 o) { offset_ = o; }
    Vector2 get_offset() const { return offset_; }

    Transform2D get_camera_transform() const {
        Transform2D t;
        t.origin = -position_ + offset_;
        t.basis_x = { zoom_, 0 };
        t.basis_y = { 0, zoom_ };
        return t;
    }

    void make_current();
    void clear_current()  { current_ = false; }
    bool is_current() const { return current_; }

private:
    float   zoom_   = 1.0f;
    Vector2 offset_ {0,0};
    bool    current_ = false;
};

class Label2D : public Vox2D {
public:
    ARX_CLASS(Label, Vox2D);
public:

    void set_text(const std::string& s)  { text_ = s; }
    const std::string& get_text() const  { return text_; }
    void set_color(Color c)              { color_ = c; }
    Color get_color() const              { return color_; }
    void set_font_size(int s)            { font_size_ = s; }

private:
    std::string text_;
    Color       color_     = Color::white;
    int         font_size_ = 16;
};

} // namespace arx

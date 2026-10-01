// ==============================================================================
// src/scene/2d/node_2d.hpp — Base de nodos 2D: transform, rotation, scale.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"

namespace arx {

class Vox2D : public Vox {
public:
    ARX_CLASS(Vox2D, Vox);
public:

    void set_position(Vector2 p) { position_ = p; dirty_ = true; }
    void set_rotation(float r)   { rotation_ = r; dirty_ = true; }
    void set_scale(Vector2 s)    { scale_    = s; dirty_ = true; }

    Vector2 get_position() const { return position_; }
    float   get_rotation() const { return rotation_; }
    Vector2 get_scale()    const { return scale_; }

    const Transform2D& get_transform() {
        if (dirty_) {
            transform_ = Transform2D::translation(position_) *
                         Transform2D::rotation(rotation_) *
                         Transform2D::scale(scale_);
            // Combinar con padre si existe (simplificación).
            dirty_ = false;
        }
        return transform_;
    }

    void translate(Vector2 v) { set_position(position_ + v); }
    void rotate(float r)      { set_rotation(rotation_ + r); }

protected:
    Vector2 position_{0,0};
    float   rotation_ = 0.0f;
    Vector2 scale_{1,1};
    Transform2D transform_;
    bool dirty_ = true;
};

} // namespace arx

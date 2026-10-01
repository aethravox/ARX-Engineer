// ==============================================================================
// src/scene/3d/node_3d.hpp — Base de nodos 3D con Transform3D.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"

namespace arx {

class Vox3D : public Vox {
public:
    ARX_CLASS(Vox3D, Vox);
public:

    void set_position(Vector3 p) { position_ = p; dirty_ = true; }
    void set_rotation(Vector3 r) { rotation_ = r; dirty_ = true; }
    void set_scale(Vector3 s)    { scale_    = s; dirty_ = true; }

    Vector3 get_position() const { return position_; }
    Vector3 get_rotation() const { return rotation_; }
    Vector3 get_scale()    const { return scale_; }

    void translate(Vector3 v) { set_position(position_ + v); }

    const Transform3D& get_transform() {
        if (dirty_) {
            // Construir rotación Euler XYZ → quaternion → matrix.
            Quaternion q = glm::quat(glm::radians(rotation_));
            Transform3D t = Transform3D::rotation(q) *
                            Transform3D::scale(scale_);
            t.origin = position_;
            transform_ = t;
            dirty_ = false;
        }
        return transform_;
    }

    // World transform (combinada con padre si es Vox3D).
    Transform3D get_global_transform() {
        Transform3D local = get_transform();
        if (auto* p3d = dynamic_cast<Vox3D*>(get_parent())) {
            return p3d->get_global_transform() * local;
        }
        return local;
    }

    Vector3 get_global_position() {
        return get_global_transform().origin;
    }

    void look_at(Vector3 target, Vector3 up = {0,1,0}) {
        Vector3 dir = target - position_;
        if (glm::length(dir) < 1e-6f) return;
        Quaternion q = glm::quatLookAt(glm::normalize(dir), up);
        rotation_ = glm::degrees(glm::eulerAngles(q));
        dirty_ = true;
    }

protected:
    Vector3 position_{0,0,0};
    Vector3 rotation_{0,0,0};
    Vector3 scale_{1,1,1};
    Transform3D transform_;
    bool dirty_ = true;
};

} // namespace arx

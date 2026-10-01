// ==============================================================================
// src/editor/gizmos.cpp
// ==============================================================================
#include "gizmos.hpp"
#include "core/math.hpp"

#include <cmath>

namespace arx {

void Gizmo::render(const Matrix4& view, const Matrix4& proj) {
    if (!target_) return;
    // Stub: en una impl completa se renderizarían 3 líneas coloreadas (R/G/B)
    // desde el target hacia cada axis, más un cubo/cono como handle.
    (void)view; (void)proj;
}

Gizmo::Axis Gizmo::hit_test(Vector2 /*mouse_pos*/, const Matrix4&, const Matrix4&) {
    return Axis::None;
}

void Gizmo::begin_drag(Vector2 mouse_pos) {
    dragging_   = true;
    drag_start_ = mouse_pos;
    if (target_) {
        initial_pos_   = target_->get_position();
        initial_rot_   = target_->get_rotation();
        initial_scale_ = target_->get_scale();
    }
}

void Gizmo::update_drag(Vector2 mouse_pos, const Matrix4& view, const Matrix4& proj) {
    if (!dragging_ || !target_) return;
    float dx = mouse_pos.x - drag_start_.x;
    float dy = mouse_pos.y - drag_start_.y;
    auto& gm = GizmoManager::instance();
    switch (mode_) {
        case Mode::Move: {
            Vector3 offset{0,0,0};
            switch (active_axis_) {
                case Axis::X:  offset.x = dx * 0.01f; break;
                case Axis::Y:  offset.y = -dy * 0.01f; break;
                case Axis::Z:  offset.z = dx * 0.01f; break;
                case Axis::XY: offset.x = dx * 0.01f; offset.y = -dy * 0.01f; break;
                case Axis::XZ: offset.x = dx * 0.01f; offset.z = dy * 0.01f; break;
                case Axis::YZ: offset.y = -dy * 0.01f; offset.z = dx * 0.01f; break;
                case Axis::XYZ: offset = Vector3(dx * 0.01f, -dy * 0.01f, 0); break;
                default: break;
            }
            Vector3 new_pos = initial_pos_ + offset;
            if (gm.is_snap_enabled()) {
                float s = gm.get_move_snap();
                new_pos = Vector3(std::round(new_pos.x / s) * s,
                                   std::round(new_pos.y / s) * s,
                                   std::round(new_pos.z / s) * s);
            }
            target_->set_position(new_pos);
            break;
        }
        case Mode::Rotate: {
            float angle = (dx + dy) * 0.5f;
            if (gm.is_snap_enabled())
                angle = std::round(angle / gm.get_rotate_snap()) * gm.get_rotate_snap();
            Vector3 rot = initial_rot_;
            switch (active_axis_) {
                case Axis::X: rot.x += angle; break;
                case Axis::Y: rot.y += angle; break;
                case Axis::Z: rot.z += angle; break;
                default: break;
            }
            target_->set_rotation(rot);
            break;
        }
        case Mode::Scale: {
            float s = 1.0f + (dx - dy) * 0.01f;
            if (gm.is_snap_enabled())
                s = std::round(s / gm.get_scale_snap()) * gm.get_scale_snap();
            Vector3 scale = initial_scale_ * s;
            target_->set_scale(scale);
            break;
        }
        case Mode::Universal:
            // Move + rotate + scale a la vez. Stub.
            break;
    }
    (void)view; (void)proj;
}

void Gizmo::end_drag() { dragging_ = false; active_axis_ = Axis::None; }

Vector3 Gizmo::compute_axis_vector(Axis a) const {
    switch (a) {
        case Axis::X:  return {1,0,0};
        case Axis::Y:  return {0,1,0};
        case Axis::Z:  return {0,0,1};
        default:       return {0,0,0};
    }
}

float Gizmo::compute_drag_amount(Vector2 mouse_pos, const Matrix4&, const Matrix4&, Axis) const {
    return (mouse_pos.x - drag_start_.x) * 0.01f;
}

// ===================== GizmoManager ==========================================
GizmoManager& GizmoManager::instance() {
    static GizmoManager g;
    return g;
}

} // namespace arx

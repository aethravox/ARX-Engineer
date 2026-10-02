// ==============================================================================
// src/editor/gizmos.hpp — Gizmos 3D para el editor (move/rotate/scale).
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "scene/3d/vox3d.hpp"

#include <vector>
#include <memory>

namespace arx {

class Vox3D;

// Gizmo — herramienta visual para manipular Vox3D en el viewport 3D.
class Gizmo {
public:
    enum class Mode { Move, Rotate, Scale, Universal };
    enum class Axis { None, X, Y, Z, XY, XZ, YZ, XYZ };

    void set_mode(Mode m) { mode_ = m; }
    Mode get_mode() const { return mode_; }
    void set_target(Vox3D* t) { target_ = t; }
    Vox3D* get_target() const { return target_; }
    void set_active_axis(Axis a) { active_axis_ = a; }

    // Render del gizmo (lineas + handles).
    void render(const Matrix4& view, const Matrix4& proj);

    // Hit testing para detectar qué axis clickeó el usuario.
    Axis hit_test(Vector2 mouse_pos, const Matrix4& view, const Matrix4& proj);

    // Drag — el usuario arrastra el mouse con la gizmo agarrada.
    void begin_drag(Vector2 mouse_pos);
    void update_drag(Vector2 mouse_pos, const Matrix4& view, const Matrix4& proj);
    void end_drag();

    bool is_dragging() const { return dragging_; }

private:
    Mode    mode_         = Mode::Move;
    Axis    active_axis_  = Axis::None;
    Axis    hovered_axis_ = Axis::None;
    Vox3D* target_       = nullptr;
    bool    dragging_     = false;
    Vector2 drag_start_;
    Vector3 initial_pos_;
    Vector3 initial_rot_;
    Vector3 initial_scale_;

    Vector3 compute_axis_vector(Axis a) const;
    float compute_drag_amount(Vector2 mouse_pos, const Matrix4& view,
                                const Matrix4& proj, Axis axis) const;
};

// GizmoManager — singleton para gestión central de gizmos.
class GizmoManager {
public:
    static GizmoManager& instance();

    Gizmo& get_move_gizmo()     { return move_gizmo_; }
    Gizmo& get_rotate_gizmo()   { return rotate_gizmo_; }
    Gizmo& get_scale_gizmo()    { return scale_gizmo_; }
    Gizmo& get_universal_gizmo(){ return universal_gizmo_; }

    Gizmo& get_active_gizmo()   { return *active_; }
    void set_active(Gizmo& g)   { active_ = &g; }

    void set_snap_enabled(bool s) { snap_enabled_ = s; }
    void set_move_snap(float s)   { move_snap_ = s; }
    void set_rotate_snap(float s) { rotate_snap_ = s; }
    void set_scale_snap(float s)  { scale_snap_ = s; }

    bool  is_snap_enabled() const { return snap_enabled_; }
    float get_move_snap()   const { return move_snap_; }
    float get_rotate_snap() const { return rotate_snap_; }
    float get_scale_snap()  const { return scale_snap_; }

private:
    GizmoManager() : active_(&move_gizmo_) {
        move_gizmo_.set_mode(Gizmo::Mode::Move);
        rotate_gizmo_.set_mode(Gizmo::Mode::Rotate);
        scale_gizmo_.set_mode(Gizmo::Mode::Scale);
        universal_gizmo_.set_mode(Gizmo::Mode::Universal);
    }
    Gizmo move_gizmo_;
    Gizmo rotate_gizmo_;
    Gizmo scale_gizmo_;
    Gizmo universal_gizmo_;
    Gizmo* active_;

    bool  snap_enabled_ = false;
    float move_snap_    = 0.25f;     // metros
    float rotate_snap_  = 15.0f;     // grados
    float scale_snap_   = 0.1f;
};

} // namespace arx

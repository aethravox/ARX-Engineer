// ==============================================================================
// src/scene/2d/camera_effects.hpp — Efectos de cámara 2D.
// VoxCamera2D + shake, follow, lookahead, smoothing.
// ==============================================================================
#pragma once

#include "scene/2d/vox_camera_2d.hpp"

namespace arx {

class CameraEffects2D {
public:
    void set_camera(VoxCamera2D* c) { cam_ = c; }

    // Shake.
    void shake(float intensity, float duration, float decay = 4.0f);
    void stop_shake();
    bool is_shaking() const { return shake_time_ > 0.0f; }

    // Follow con suavizado.
    void follow(Vector2 target, float smoothing = 0.1f);
    void set_lookahead(float amount) { lookahead_ = amount; }
    void set_bounds(Rect2 bounds) { bounds_ = bounds; bounds_active_ = true; }
    void clear_bounds() { bounds_active_ = false; }

    void update(float delta);

private:
    VoxCamera2D* cam_ = nullptr;
    float shake_intensity_ = 0.0f;
    float shake_duration_ = 0.0f;
    float shake_time_     = 0.0f;
    float shake_decay_    = 4.0f;

    Vector2 follow_target_;
    float   follow_smoothing_ = 0.1f;
    float   lookahead_ = 0.0f;
    Vector2 last_velocity_;

    Rect2 bounds_;
    bool  bounds_active_ = false;
};

// Clase utilitaria static para usar desde cualquier parte.
class Camera2DController {
public:
    static void shake(VoxCamera2D* cam, float intensity, float duration);
    static void follow(VoxCamera2D* cam, Vector2 target, float smoothing);
};

} // namespace arx

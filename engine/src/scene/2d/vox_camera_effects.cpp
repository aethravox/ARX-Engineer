// ==============================================================================
// src/scene/2d/camera_effects.cpp
// ==============================================================================
#include "vox_camera_effects.hpp"
#include "core/math.hpp"

#include <cmath>
#include <random>

namespace arx {

void CameraEffects2D::shake(float intensity, float duration, float decay) {
    shake_intensity_ = intensity;
    shake_duration_  = duration;
    shake_time_      = duration;
    shake_decay_     = decay;
}

void CameraEffects2D::stop_shake() {
    shake_time_ = 0.0f;
    if (cam_) cam_->set_offset({0, 0});
}

void CameraEffects2D::follow(Vector2 target, float smoothing) {
    follow_target_   = target;
    follow_smoothing_ = smoothing;
}

void CameraEffects2D::update(float delta) {
    if (!cam_) return;

    // Follow.
    Vector2 current_pos = cam_->get_position();
    Vector2 new_pos = math::lerp(current_pos, follow_target_, follow_smoothing_);
    cam_->set_position(new_pos);

    // Lookahead: si nos estamos moviendo rápido, adelanta la cámara.
    Vector2 velocity = new_pos - current_pos;
    if (glm::length(velocity) > 0.1f && lookahead_ > 0.0f) {
        last_velocity_ = math::lerp(last_velocity_, velocity, 0.1f);
        Vector2 ahead = last_velocity_ * lookahead_;
        cam_->set_offset(ahead);
    }

    // Shake.
    if (shake_time_ > 0.0f) {
        shake_time_ -= delta;
        float t = shake_time_ / shake_duration_;
        float intensity = shake_intensity_ * std::exp(-shake_decay_ * (1.0f - t));
        static std::mt19937 rng(42);
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        Vector2 shake_off(dis(rng) * intensity, dis(rng) * intensity);
        Vector2 cur_off = cam_->get_offset();
        cam_->set_offset(cur_off + shake_off);
        if (shake_time_ <= 0.0f) {
            shake_time_ = 0.0f;
            cam_->set_offset({0, 0});
        }
    }

    // Bounds.
    if (bounds_active_) {
        Vector2 p = cam_->get_position();
        p.x = std::clamp(p.x, bounds_.position.x, bounds_.end().x);
        p.y = std::clamp(p.y, bounds_.position.y, bounds_.end().y);
        cam_->set_position(p);
    }
}

// ===================== Static helpers =======================================
void Camera2DController::shake(VoxCamera2D* cam, float intensity, float duration) {
    static CameraEffects2D effects;
    effects.set_camera(cam);
    effects.shake(intensity, duration);
}

void Camera2DController::follow(VoxCamera2D* cam, Vector2 target, float smoothing) {
    static CameraEffects2D effects;
    effects.set_camera(cam);
    effects.follow(target, smoothing);
}

} // namespace arx

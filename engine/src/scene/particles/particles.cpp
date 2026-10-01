// ==============================================================================
// src/scene/particles/particles.cpp
// ==============================================================================
#include "particles.hpp"
#include "core/math.hpp"

#include <cmath>
#include <algorithm>

namespace arx {

// ===================== VoxParticles2D ==========================================
void VoxParticles2D::restart() {
    particles_.clear();
    particles_.resize(amount_);
}

void VoxParticles2D::emit_particle(Particle& p) {
    p.position = position_;
    float angle = std::atan2(direction_.y, direction_.x)
                + math::random(-spread_, spread_);
    p.velocity  = Vector2(std::cos(angle), std::sin(angle)) * initial_velocity_;
    p.life      = lifetime_;
    p.lifetime  = lifetime_;
    p.color     = color_;
    p.scale     = scale_;
    p.rotation  = 0.0f;
    p.active    = true;
}

void VoxParticles2D::process(float delta) {
    if (particles_.empty()) particles_.resize(amount_);

    if (emitting_) {
        time_since_emit_ += delta;
        float interval = lifetime_ / amount_ * (1.0f - explosiveness_);
        while (time_since_emit_ >= interval) {
            time_since_emit_ -= interval;
            // Encuentra partícula inactiva.
            for (auto& p : particles_) {
                if (!p.active) {
                    emit_particle(p);
                    break;
                }
            }
        }
    }

    for (auto& p : particles_) {
        if (!p.active) continue;
        p.life     -= delta;
        p.velocity += gravity_ * delta;
        p.position += p.velocity * delta;
        if (p.life <= 0) {
            p.active = false;
            if (one_shot_ && emitting_) {
                // No reemitir si one_shot.
            }
        }
    }
}

// ===================== VoxParticles3D ==========================================
void VoxParticles3D::restart() {
    particles_.clear();
    particles_.resize(amount_);
}

void VoxParticles3D::emit_particle(Particle3D& p) {
    p.position = position_;
    // Distribución cónica simple.
    Vector3 dir = glm::normalize(direction_);
    float theta = math::random(-spread_, spread_);
    float phi   = math::random(0.0f, 6.28318530718f);
    Vector3 offset(
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        std::cos(theta)
    );
    p.velocity  = (dir + offset * 0.3f) * initial_velocity_;
    p.life      = lifetime_;
    p.lifetime  = lifetime_;
    p.color     = color_;
    p.scale     = scale_;
    p.active    = true;
}

void VoxParticles3D::process(float delta) {
    if (particles_.empty()) particles_.resize(amount_);

    if (emitting_) {
        time_since_emit_ += delta;
        float interval = lifetime_ / amount_;
        while (time_since_emit_ >= interval) {
            time_since_emit_ -= interval;
            for (auto& p : particles_) {
                if (!p.active) { emit_particle(p); break; }
            }
        }
    }

    for (auto& p : particles_) {
        if (!p.active) continue;
        p.life     -= delta;
        p.velocity += gravity_ * delta;
        p.position += p.velocity * delta;
        if (p.life <= 0) p.active = false;
    }
}

} // namespace arx

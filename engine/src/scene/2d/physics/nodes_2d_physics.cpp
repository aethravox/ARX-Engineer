// ==============================================================================
// src/scene/2d/physics/nodes_2d_physics.cpp
// ==============================================================================
#include "nodes_2d_physics.hpp"
#include "core/math.hpp"

namespace arx {

void VoxRigidBody2D::_physics_process(float delta) {
    // Gravedad
    Vector2 gravity(0.0f, 980.0f);  // 9.8 m/s² * 100 px/m
    linear_velocity_ += gravity * gravity_scale_ * delta;

    // Aplicar forces acumuladas
    linear_velocity_ += force_accum_ * inv_mass_ * delta;
    force_accum_ = {0, 0};

    // Damping
    linear_velocity_ *= (1.0f - linear_damp_ * delta);
    angular_velocity_ *= (1.0f - angular_damp_ * delta);

    // Integrar
    set_position(get_position() + linear_velocity_ * delta);
    set_rotation(get_rotation() + angular_velocity_ * delta);
}

void VoxCharacterBody2D::move_and_slide() {
    // Versión simplificada: solo integrar velocity, sin colisiones todavía.
    set_position(get_position() + velocity_ * 1.0f / 60.0f);
    on_floor_   = false;
    on_wall_    = false;
    on_ceiling_ = false;
}

} // namespace arx

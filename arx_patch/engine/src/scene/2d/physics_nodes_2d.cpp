// ==============================================================================
// src/scene/2d/physics_nodes_2d.cpp
// ==============================================================================
#include "physics_nodes_2d.hpp"

namespace arx {

// NOTA: RigidBody2D::_physics_process se eliminó de aquí porque está duplicado
// en src/scene/2d/physics/nodes_2d_physics.cpp (versión con Bullet).
// Mantener ambos causaba "multiple definition of `arx::RigidBody2D::_physics_process`".

// CharacterBody2D::move_and_slide(Vector2, float) - versión vieja con argumentos.
// NOTA: La versión nueva en physics/nodes_2d_physics.cpp usa move_and_slide() sin args,
// pero como las firmas son diferentes, ambas pueden coexistir (overload).
// Si la nueva define la misma firma, eliminar esta.
Vector2 CharacterBody2D::move_and_slide(Vector2 velocity, float delta) {
    // Sin motor de física: simplemente avanza.
    position_ += velocity * delta;
    dirty_ = true;
    return velocity;
}

void RayCast2D::_physics_process(float delta) {
    (void)delta;
    if (!enabled_) { colliding_ = false; return; }
    // Placeholder: sin motor 2D, no detectamos colisiones reales.
    colliding_ = false;
}

} // namespace arx

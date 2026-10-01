// ==============================================================================
// src/scene/3d/physics_nodes_3d.cpp
// ==============================================================================
#include "physics_nodes_3d.hpp"
#include "core/logging.hpp"

namespace arx {

// ===================== RigidBody3D ==========================================
void RigidBody3D::_enter_tree() {
    // Crear shape y body en el PhysicsServer.
    auto srv_up = PhysicsServer::create(); PhysicsServer* srv = srv_up.get();
    if (!srv) return;
    RID space;  // TODO: obtener el space del SceneTree.
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Dynamic, space, shape, get_global_transform());
    srv->body_set_mass(body_, mass_);
    srv->body_set_friction(body_, friction_);
    srv->body_set_restitution(body_, restitution_);
}

void RigidBody3D::_exit_tree() {
    // El server limpia recursos cuando se destruye el world.
    body_ = {};
}

// NOTA: RigidBody3D::_physics_process se eliminó de aquí porque está duplicado
// en src/scene/3d/physics/nodes_3d_physics.cpp. Si se deja en ambos, el linker
// falla con "multiple definition of `arx::RigidBody3D::_physics_process`".

// ===================== StaticBody3D =========================================
void StaticBody3D::_enter_tree() {
    auto srv_up = PhysicsServer::create(); PhysicsServer* srv = srv_up.get();
    if (!srv) return;
    RID space;
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Static, space, shape, get_global_transform());
}
void StaticBody3D::_exit_tree() { body_ = {}; }

// ===================== KinematicBody3D ======================================
void KinematicBody3D::_enter_tree() {
    auto srv_up = PhysicsServer::create(); PhysicsServer* srv = srv_up.get();
    if (!srv) return;
    RID space;
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Kinematic, space, shape, get_global_transform());
}
void KinematicBody3D::_exit_tree() { body_ = {}; }

Vector3 KinematicBody3D::move_and_slide(Vector3 velocity, float delta) {
    if (!body_.valid()) return velocity;
    auto srv_up = PhysicsServer::create(); PhysicsServer* srv = srv_up.get();
    if (!srv) return velocity;
    srv->body_set_linear_velocity(body_, velocity);
    set_position(position_ + velocity * delta);
    return velocity;
}

Vector3 KinematicBody3D::move_and_collide(Vector3 velocity, float delta) {
    return move_and_slide(velocity, delta);
}

// ===================== CharacterBody3D ======================================
Vector3 CharacterBody3D::move_and_slide_with_gravity(Vector3 velocity, float delta) {
    velocity.y += -9.81f * delta;  // Gravedad simple.
    return move_and_slide(velocity, delta);
}

// ===================== Area3D ================================================
void Area3D::_enter_tree() {
    auto srv_up = PhysicsServer::create(); PhysicsServer* srv = srv_up.get();
    if (!srv) return;
    RID space;
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Kinematic, space, shape, get_global_transform());
}
void Area3D::_exit_tree() { body_ = {}; }

// ===================== RayCast3D =============================================
void RayCast3D::_physics_process(float delta) {
    (void)delta;
    if (!enabled_) { colliding_ = false; return; }
    // En una implementación completa se haría un raytest contra el PhysicsServer.
    // Por ahora dejamos colliding_ en false (placeholder).
    colliding_ = false;
}

} // namespace arx

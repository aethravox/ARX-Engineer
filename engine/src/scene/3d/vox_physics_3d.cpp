// ==============================================================================
// src/scene/3d/physics_nodes_3d.cpp
// ==============================================================================
#include "vox_physics_3d.hpp"
#include "scene/scene_tree.hpp"
#include "core/logging.hpp"

namespace arx {

// Helper: obtener el PhysicsServer y space del SceneTree
static PhysicsServer* get_ps(Vox* v) {
    if (!v || !v->get_tree()) return nullptr;
    return v->get_tree()->get_physics_server();
}

static RID get_space(Vox* v) {
    if (!v || !v->get_tree()) return {};
    return v->get_tree()->get_default_space();
}

// ===================== VoxRigidBody3D ==========================================
void VoxRigidBody3D::_enter_tree() {
    PhysicsServer* srv = get_ps(this);
    if (!srv) return;
    RID space = get_space(this);
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Dynamic, space, shape, get_global_transform());
    srv->body_set_mass(body_, mass_);
    srv->body_set_friction(body_, friction_);
    srv->body_set_restitution(body_, restitution_);
    ARX_LOG_INFO("VoxRigidBody3D '{}': body creado (mass={})", get_name(), mass_);
}

void VoxRigidBody3D::_exit_tree() {
    body_ = {};
}

void VoxRigidBody3D::_physics_process(float delta) {
    (void)delta;
    if (!body_.valid()) return;
    PhysicsServer* srv = get_ps(this);
    if (!srv) return;
    Transform3D t = srv->body_get_transform(body_);
    set_position(t.origin);
}

// ===================== VoxStaticBody3D =========================================
void VoxStaticBody3D::_enter_tree() {
    PhysicsServer* srv = get_ps(this);
    if (!srv) return;
    RID space = get_space(this);
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Static, space, shape, get_global_transform());
    ARX_LOG_INFO("VoxStaticBody3D '{}': body creado", get_name());
}
void VoxStaticBody3D::_exit_tree() { body_ = {}; }

// ===================== VoxKinematicBody3D ======================================
void VoxKinematicBody3D::_enter_tree() {
    PhysicsServer* srv = get_ps(this);
    if (!srv) return;
    RID space = get_space(this);
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Kinematic, space, shape, get_global_transform());
}
void VoxKinematicBody3D::_exit_tree() { body_ = {}; }

Vector3 VoxKinematicBody3D::move_and_slide(Vector3 velocity, float delta) {
    if (!body_.valid()) return velocity;
    PhysicsServer* srv = get_ps(this);
    if (!srv) return velocity;
    srv->body_set_linear_velocity(body_, velocity);
    set_position(position_ + velocity * delta);
    return velocity;
}

Vector3 VoxKinematicBody3D::move_and_collide(Vector3 velocity, float delta) {
    return move_and_slide(velocity, delta);
}

// ===================== VoxCharacterBody3D ======================================
Vector3 VoxCharacterBody3D::move_and_slide_with_gravity(Vector3 velocity, float delta) {
    velocity.y += -9.81f * delta;
    return move_and_slide(velocity, delta);
}

// ===================== VoxArea3D ================================================
void VoxArea3D::_enter_tree() {
    PhysicsServer* srv = get_ps(this);
    if (!srv) return;
    RID space = get_space(this);
    RID shape = srv->shape_create(ShapeType::Box, half_extents_, radius_, height_);
    body_ = srv->body_create(BodyType::Kinematic, space, shape, get_global_transform());
}
void VoxArea3D::_exit_tree() { body_ = {}; }

// ===================== VoxRayCast3D =============================================
void VoxRayCast3D::_physics_process(float delta) {
    (void)delta;
    if (!enabled_) { colliding_ = false; return; }
    colliding_ = false;
}

} // namespace arx

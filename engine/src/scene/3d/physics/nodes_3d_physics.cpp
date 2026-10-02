// ==============================================================================
// src/scene/3d/physics/nodes_3d_physics.cpp
// ==============================================================================
#include "nodes_3d_physics.hpp"
#include "physics/physics_server.hpp"
#include "core/logging.hpp"

namespace arx {

void VoxRigidBody3D::set_mass(float m) {
    mass_ = std::max(0.0001f, m);
    if (body_rid_.valid()) {
        if (auto ps_up = PhysicsServer::create()) { PhysicsServer* ps = ps_up.get();
            ps->body_set_mass(body_rid_, mass_);
            (void)ps;
        }
    }
}

void VoxRigidBody3D::apply_central_impulse(Vector3 impulse) {
    if (auto ps_up = PhysicsServer::create()) { PhysicsServer* ps = ps_up.get();
        ps->body_apply_impulse(body_rid_, impulse);
        (void)ps;
    }
}

void VoxRigidBody3D::apply_impulse(Vector3 impulse, Vector3 position) {
    if (auto ps_up = PhysicsServer::create()) { PhysicsServer* ps = ps_up.get();
        ps->body_apply_impulse(body_rid_, impulse, position);
        (void)ps;
    }
}

void VoxRigidBody3D::apply_torque_impulse(Vector3 torque) {
    (void)torque;  // TODO
}

void VoxRigidBody3D::apply_central_force(Vector3 force) {
    apply_central_impulse(force * 0.016f);
}

Vector3 VoxRigidBody3D::get_linear_velocity() const {
    if (auto ps_up = PhysicsServer::create()) { PhysicsServer* ps = ps_up.get();
        auto v = ps->body_get_linear_velocity(body_rid_);
        (void)ps;
        return v;
    }
    return {};
}

void VoxRigidBody3D::set_linear_velocity(Vector3 v) {
    if (auto ps_up = PhysicsServer::create()) { PhysicsServer* ps = ps_up.get();
        ps->body_set_linear_velocity(body_rid_, v);
        (void)ps;
    }
}

Vector3 VoxRigidBody3D::get_angular_velocity() const { return {}; }

void VoxRigidBody3D::set_angular_velocity(Vector3 v) { (void)v; }

// NOTE: VoxRigidBody3D::_physics_process está implementado en vox_physics_3d.cpp
// (usa el PhysicsServer del SceneTree en vez de crear uno nuevo).
// No duplicar acá para evitar "multiple definition" del linker.

void VoxCharacterBody3D::move_and_slide() {
    set_position(get_position() + velocity_ * 0.016f);
}

} // namespace arx

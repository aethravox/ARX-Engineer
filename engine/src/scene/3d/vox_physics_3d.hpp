// ==============================================================================
// src/scene/3d/physics_nodes_3d.hpp — Nodos físicos 3D.
// VoxRigidBody3D, VoxStaticBody3D, VoxKinematicBody3D, VoxArea3D, VoxCollisionShape3D,
// VoxCharacterBody3D, VoxRayCast3D.
// ==============================================================================
#pragma once

#include "scene/3d/vox3d.hpp"
#include "physics/physics_server.hpp"

#include <memory>

namespace arx {

// VoxCollisionShape3D — define la shape de colisión de un cuerpo.
class VoxCollisionShape3D : public Vox3D {
public:
    ARX_CLASS(VoxCollisionShape3D, Vox3D);
public:

    enum class Shape { Box, Sphere, Capsule, Cylinder, Convex, Concave };

    void set_shape(Shape s)    { shape_ = s; }
    Shape get_shape() const    { return shape_; }
    void set_half_extents(Vector3 e) { half_extents_ = e; }
    void set_radius(float r)   { radius_ = r; }
    void set_height(float h)   { height_ = h; }

    Vector3 get_half_extents() const { return half_extents_; }
    float   get_radius() const { return radius_; }
    float   get_height() const { return height_; }

private:
    Shape   shape_ = Shape::Box;
    Vector3 half_extents_{0.5f, 0.5f, 0.5f};
    float   radius_ = 0.5f;
    float   height_ = 1.0f;
};

// PhysicsBody3D — base de cuerpos físicos.
class PhysicsBody3D : public Vox3D {
public:
    Vector3 half_extents_ = {0.5f, 0.5f, 0.5f};
    float   radius_ = 0.5f;
    float   height_ = 1.0f;
public:
    ARX_CLASS(PhysicsBody3D, Vox3D);
public:

    void set_mass(float m)     { mass_ = m; }
    void set_friction(float f) { friction_ = f; }
    void set_restitution(float r) { restitution_ = r; }
    void set_collision_layer(uint32_t l) { layer_ = l; }
    void set_collision_mask(uint32_t m)  { mask_ = m; }

    float    get_mass()        const { return mass_; }
    float    get_friction()    const { return friction_; }
    float    get_restitution() const { return restitution_; }
    uint32_t get_collision_layer() const { return layer_; }
    uint32_t get_collision_mask()  const { return mask_; }

    Vector3 get_linear_velocity() const { return linear_velocity_; }
    void    set_linear_velocity(Vector3 v) { linear_velocity_ = v; }

    void apply_impulse(Vector3 impulse, Vector3 rel_pos = {}) {
        if (body_.valid()) {
            PhysicsServer::create()->body_apply_impulse(body_, impulse, rel_pos);
        }
    }

protected:
    RID     body_;
    float   mass_        = 1.0f;
    float   friction_    = 0.5f;
    float   restitution_ = 0.0f;
    uint32_t layer_      = 1;
    uint32_t mask_       = 0xFFFFFFFF;
    Vector3 linear_velocity_{0,0,0};
};

class VoxRigidBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxRigidBody3D, PhysicsBody3D);
public:
    void _enter_tree() override;
    void _exit_tree() override;
    void _physics_process(float delta) override;
};

class VoxStaticBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxStaticBody3D, PhysicsBody3D);
public:
    void _enter_tree() override;
    void _exit_tree() override;
};

class VoxKinematicBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxKinematicBody3D, PhysicsBody3D);
public:
    void _enter_tree() override;
    void _exit_tree() override;

    Vector3 move_and_slide(Vector3 velocity, float delta);
    Vector3 move_and_collide(Vector3 velocity, float delta);
};

class VoxCharacterBody3D : public VoxKinematicBody3D {
public:
    ARX_CLASS(VoxCharacterBody3D, VoxKinematicBody3D);
public:

    void set_up_direction(Vector3 u) { up_ = u; }
    Vector3 get_up_direction() const { return up_; }
    void set_floor_max_angle(float r) { floor_max_angle_ = r; }

    // Movimiento simple con gravedad.
    Vector3 move_and_slide_with_gravity(Vector3 velocity, float delta);

private:
    Vector3 up_{0, 1, 0};
    float   floor_max_angle_ = 0.7853982f; // 45 grados
};

class VoxArea3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxArea3D, PhysicsBody3D);
public:
    void _enter_tree() override;
    void _exit_tree() override;

    void set_monitoring(bool m) { monitoring_ = m; }
    bool is_monitoring() const { return monitoring_; }

    // Callbacks vía signals.
    // signal body_entered(PhysicsBody3D* body)
    // signal body_exited(PhysicsBody3D* body)
    // signal area_entered(VoxArea3D* area)
    // signal area_exited(VoxArea3D* area)

private:
    bool monitoring_ = true;
};

class VoxRayCast3D : public Vox3D {
public:
    ARX_CLASS(VoxRayCast3D, Vox3D);
public:

    void set_target_position(Vector3 p) { target_ = p; }
    Vector3 get_target_position() const { return target_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool is_enabled() const { return enabled_; }
    void set_collision_mask(uint32_t m) { mask_ = m; }

    bool is_colliding() const { return colliding_; }
    Vector3 get_collision_point() const { return collision_point_; }
    Vector3 get_collision_normal() const { return collision_normal_; }

    void _physics_process(float delta) override;

private:
    Vector3 target_{0, 0, -1};
    bool    enabled_ = true;
    uint32_t mask_   = 0xFFFFFFFF;
    bool    colliding_ = false;
    Vector3 collision_point_;
    Vector3 collision_normal_;
};

} // namespace arx

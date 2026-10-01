// ==============================================================================
// src/scene/2d/physics_nodes_2d.hpp — Nodos físicos 2D.
// VoxRigidBody2D, VoxStaticBody2D, VoxCharacterBody2D, VoxArea2D, VoxCollisionShape2D,
// VoxRayCast2D.
// ==============================================================================
#pragma once

#include "scene/2d/vox2d.hpp"
#include "core/types.hpp"

namespace arx {

// VoxCollisionShape2D — define la shape de colisión 2D.
class VoxCollisionShape2D : public Vox2D {
public:
    ARX_CLASS(VoxCollisionShape2D, Vox2D);
public:

    enum class Shape { Rect, Circle, Capsule };

    void set_shape(Shape s)  { shape_ = s; }
    Shape get_shape() const  { return shape_; }
    void set_size(Vector2 s) { size_ = s; }
    void set_radius(float r) { radius_ = r; }

    Vector2 get_size() const { return size_; }
    float   get_radius() const { return radius_; }

private:
    Shape   shape_ = Shape::Rect;
    Vector2 size_{32, 32};
    float   radius_ = 16.0f;
};

// PhysicsBody2D — base.
class PhysicsBody2D : public Vox2D {
public:
    ARX_CLASS(PhysicsBody2D, Vox2D);
public:

    void set_mass(float m) { mass_ = m; }
    float get_mass() const { return mass_; }
    void set_friction(float f) { friction_ = f; }
    void set_bounce(float b) { bounce_ = b; }
    void set_collision_layer(uint32_t l) { layer_ = l; }
    void set_collision_mask(uint32_t m) { mask_ = m; }

    Vector2 get_linear_velocity() const { return linear_velocity_; }
    void set_linear_velocity(Vector2 v) { linear_velocity_ = v; }

    void apply_impulse(Vector2 impulse) { linear_velocity_ += impulse / mass_; }

protected:
    float    mass_     = 1.0f;
    float    friction_ = 0.5f;
    float    bounce_   = 0.0f;
    uint32_t layer_    = 1;
    uint32_t mask_     = 0xFFFFFFFF;
    Vector2  linear_velocity_{0, 0};
};

class VoxRigidBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxRigidBody2D, PhysicsBody2D);
public:
    void _physics_process(float delta) override;
};

class VoxStaticBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxStaticBody2D, PhysicsBody2D);
public:
};

class VoxCharacterBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxCharacterBody2D, PhysicsBody2D);
public:

    void set_floor_max_angle(float r) { floor_max_angle_ = r; }
    Vector2 move_and_slide(Vector2 velocity, float delta);

private:
    float floor_max_angle_ = 0.7853982f;
};

class VoxArea2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxArea2D, PhysicsBody2D);
public:
    void set_monitoring(bool m) { monitoring_ = m; }
    bool is_monitoring() const { return monitoring_; }

private:
    bool monitoring_ = true;
};

class VoxRayCast2D : public Vox2D {
public:
    ARX_CLASS(VoxRayCast2D, Vox2D);
public:

    void set_target_position(Vector2 p) { target_ = p; }
    Vector2 get_target_position() const { return target_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool is_enabled() const { return enabled_; }
    bool is_colliding() const { return colliding_; }
    Vector2 get_collision_point() const { return collision_point_; }

    void _physics_process(float delta) override;

private:
    Vector2 target_{0, -1};
    bool    enabled_ = true;
    bool    colliding_ = false;
    Vector2 collision_point_;
};

} // namespace arx

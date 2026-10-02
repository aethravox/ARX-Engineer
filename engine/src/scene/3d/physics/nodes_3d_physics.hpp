// ==============================================================================
// src/scene/3d/physics/nodes_3d_physics.hpp — VoxRigidBody3D, VoxStaticBody3D, VoxArea3D,
// VoxCharacterBody3D, VoxCollisionShape3D, VehicleBody3D.
// ==============================================================================
#pragma once

#include "scene/3d/vox3d.hpp"
#include "physics/physics_server.hpp"
#include "core/types.hpp"

#include <memory>

namespace arx {

class Shape3D : public Object {
public:
    ARX_CLASS(Shape3D, Object);
public:
    enum class Kind { Box, Sphere, Capsule, Cylinder, Convex, Concave };
    virtual Kind get_kind() const = 0;
    virtual ShapeType to_physics_shape() const = 0;
    virtual Vector3 get_half_extents() const { return {0.5f, 0.5f, 0.5f}; }
    virtual float   get_radius() const { return 0.5f; }
    virtual float   get_height() const { return 1.0f; }
};

class BoxShape3D : public Shape3D {
public:
    ARX_CLASS(BoxShape3D, Shape3D);
public:
    Kind get_kind() const override { return Kind::Box; }
    ShapeType to_physics_shape() const override { return ShapeType::Box; }
    void set_half_extents(Vector3 h) { half_extents_ = h; }
    Vector3 get_half_extents() const override { return half_extents_; }
private:
    Vector3 half_extents_ = {0.5f, 0.5f, 0.5f};
};

class SphereShape3D : public Shape3D {
public:
    ARX_CLASS(SphereShape3D, Shape3D);
public:
    Kind get_kind() const override { return Kind::Sphere; }
    ShapeType to_physics_shape() const override { return ShapeType::Sphere; }
    void set_radius(float r) { radius_ = r; }
    float get_radius() const override { return radius_; }
private:
    float radius_ = 0.5f;
};

class CapsuleShape3D : public Shape3D {
public:
    ARX_CLASS(CapsuleShape3D, Shape3D);
public:
    Kind get_kind() const override { return Kind::Capsule; }
    ShapeType to_physics_shape() const override { return ShapeType::Capsule; }
    void set_radius(float r) { radius_ = r; }
    void set_height(float h) { height_ = h; }
    float get_radius() const override { return radius_; }
    float get_height() const override { return height_; }
private:
    float radius_ = 0.5f;
    float height_ = 1.0f;
};

class VoxCollisionShape3D : public Vox3D {
public:
    ARX_CLASS(VoxCollisionShape3D, Vox3D);
public:
    void set_shape(std::shared_ptr<Shape3D> s) { shape_ = s; }
    std::shared_ptr<Shape3D> get_shape() const { return shape_; }
private:
    std::shared_ptr<Shape3D> shape_;
};

class PhysicsBody3D : public Vox3D {
public:
    ARX_CLASS(PhysicsBody3D, Vox3D);
public:
    void set_collision_layer(uint32_t l) { collision_layer_ = l; }
    uint32_t get_collision_layer() const { return collision_layer_; }
    void set_collision_mask(uint32_t m) { collision_mask_ = m; }
    uint32_t get_collision_mask() const { return collision_mask_; }

    // RID del cuerpo en el PhysicsServer.
    RID  get_body_rid() const { return body_rid_; }
    void set_body_rid(RID r)  { body_rid_ = r; }

protected:
    uint32_t collision_layer_ = 1;
    uint32_t collision_mask_  = 0xFFFFFFFF;
    RID      body_rid_;
    std::shared_ptr<Shape3D> shape_;
};

class VoxStaticBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxStaticBody3D, PhysicsBody3D);
public:
};

class VoxRigidBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxRigidBody3D, PhysicsBody3D);
public:

    void set_mass(float m);
    float get_mass() const { return mass_; }
    void set_gravity_scale(float s) { gravity_scale_ = s; }
    float get_gravity_scale() const { return gravity_scale_; }
    void set_linear_damp(float d) { linear_damp_ = d; }
    void set_angular_damp(float d) { angular_damp_ = d; }
    void set_bounce(float b) { bounce_ = b; }
    float get_bounce() const { return bounce_; }
    void set_friction(float f) { friction_ = f; }
    float get_friction() const { return friction_; }

    void apply_central_impulse(Vector3 impulse);
    void apply_impulse(Vector3 impulse, Vector3 position);
    void apply_torque_impulse(Vector3 torque);
    void apply_central_force(Vector3 force);

    Vector3 get_linear_velocity() const;
    void    set_linear_velocity(Vector3 v);
    Vector3 get_angular_velocity() const;
    void    set_angular_velocity(Vector3 v);

    void _physics_process(float delta) override;

private:
    float mass_          = 1.0f;
    float gravity_scale_ = 1.0f;
    float linear_damp_   = 0.0f;
    float angular_damp_  = 0.0f;
    float bounce_        = 0.0f;
    float friction_      = 0.0f;
};

class VoxCharacterBody3D : public PhysicsBody3D {
public:
    ARX_CLASS(VoxCharacterBody3D, PhysicsBody3D);
public:

    void set_velocity(Vector3 v) { velocity_ = v; }
    Vector3 get_velocity() const { return velocity_; }
    void move_and_slide();

    bool is_on_floor() const { return on_floor_; }
    bool is_on_wall() const { return on_wall_; }
    bool is_on_ceiling() const { return on_ceiling_; }

private:
    Vector3 velocity_   = {0,0,0};
    bool on_floor_      = false;
    bool on_wall_       = false;
    bool on_ceiling_    = false;
};

class VoxArea3D : public Vox3D {
public:
    ARX_CLASS(VoxArea3D, Vox3D);
public:

    void set_monitoring(bool m) { monitoring_ = m; }
    bool is_monitoring() const { return monitoring_; }

    // Signals: body_entered, body_exited, area_entered, area_exited
private:
    bool monitoring_ = true;
};

} // namespace arx

// ==============================================================================
// src/scene/2d/physics/nodes_2d_physics.hpp — Nodos físicos 2D.
// VoxRigidBody2D, VoxStaticBody2D, VoxArea2D, VoxCharacterBody2D, VoxCollisionShape2D.
// Física 2D custom (sin Box2D): SAT + AABB broadphase.
// ==============================================================================
#pragma once

#include "scene/2d/vox2d.hpp"
#include "core/types.hpp"
#include "core/object.hpp"

#include <vector>
#include <memory>

namespace arx {

// Shape2D: primitiva de colisión 2D.
class Shape2D : public Object {
public:
    ARX_CLASS(Shape2D, Object);
public:
    enum class Kind { Box, Circle, Capsule, Segment };
    virtual Kind get_kind() const = 0;
    virtual Rect2 get_aabb(const Transform2D& t) const = 0;
};

class BoxShape2D : public Shape2D {
public:
    ARX_CLASS(BoxShape2D, Shape2D);
public:
    Kind get_kind() const override { return Kind::Box; }
    void set_half_extents(Vector2 h) { half_extents_ = h; }
    Vector2 get_half_extents() const { return half_extents_; }
    Rect2 get_aabb(const Transform2D& t) const override {
        return Rect2(t.origin - half_extents_, half_extents_ * 2.0f);
    }
private:
    Vector2 half_extents_ = {0.5f, 0.5f};
};

class CircleShape2D : public Shape2D {
public:
    ARX_CLASS(CircleShape2D, Shape2D);
public:
    Kind get_kind() const override { return Kind::Circle; }
    void set_radius(float r) { radius_ = r; }
    float get_radius() const { return radius_; }
    Rect2 get_aabb(const Transform2D& t) const override {
        return Rect2(t.origin - Vector2(radius_, radius_),
                      Vector2(radius_ * 2.0f, radius_ * 2.0f));
    }
private:
    float radius_ = 0.5f;
};

class VoxCollisionShape2D : public Vox2D {
public:
    ARX_CLASS(VoxCollisionShape2D, Vox2D);
public:
    void set_shape(std::shared_ptr<Shape2D> s) { shape_ = s; }
    std::shared_ptr<Shape2D> get_shape() const { return shape_; }

    // Helper para construir desde descripción.
    void set_box(Vector2 half_extents) {
        auto s = std::make_shared<BoxShape2D>();
        s->set_half_extents(half_extents);
        shape_ = s;
    }
    void set_circle(float radius) {
        auto s = std::make_shared<CircleShape2D>();
        s->set_radius(radius);
        shape_ = s;
    }

private:
    std::shared_ptr<Shape2D> shape_;
};

// PhysicsBody2D: base de todos los cuerpos físicos 2D.
class PhysicsBody2D : public Vox2D {
public:
    ARX_CLASS(PhysicsBody2D, Vox2D);
public:

    void set_collision_layer(uint32_t l) { collision_layer_ = l; }
    uint32_t get_collision_layer() const { return collision_layer_; }
    void set_collision_mask(uint32_t m) { collision_mask_ = m; }
    uint32_t get_collision_mask() const { return collision_mask_; }

    void add_collision_exception_with(PhysicsBody2D* other) {
        if (other) exceptions_.push_back(other);
    }

    std::shared_ptr<Shape2D> get_shape() const { return shape_; }
    void set_shape(std::shared_ptr<Shape2D> s) { shape_ = s; }

    // Velocidad lineal y angular (acceso para ARXScript).
    Vector2 get_linear_velocity() const { return linear_velocity_; }
    void    set_linear_velocity(Vector2 v) { linear_velocity_ = v; }
    float   get_angular_velocity() const { return angular_velocity_; }
    void    set_angular_velocity(float w) { angular_velocity_ = w; }

    void apply_impulse(Vector2 impulse) { linear_velocity_ += impulse; }
    void apply_force(Vector2 force)     { force_accum_ += force; }

    // Signals: body_entered, body_exited, area_entered, area_exited

protected:
    uint32_t collision_layer_ = 1;
    uint32_t collision_mask_  = 0xFFFFFFFF;
    std::vector<PhysicsBody2D*> exceptions_;
    std::shared_ptr<Shape2D>    shape_;
    Vector2 linear_velocity_  = {0, 0};
    float   angular_velocity_ = 0.0f;
    Vector2 force_accum_      = {0, 0};
};

// VoxStaticBody2D: cuerpo estático (no se mueve, pero otros colisionan con él).
class VoxStaticBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxStaticBody2D, PhysicsBody2D);
public:
};

// VoxRigidBody2D: cuerpo dinámico afectado por gravedad y fuerzas.
class VoxRigidBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxRigidBody2D, PhysicsBody2D);
public:

    void set_mass(float m) { mass_ = std::max(0.0001f, m); inv_mass_ = 1.0f / mass_; }
    float get_mass() const { return mass_; }
    void set_gravity_scale(float s) { gravity_scale_ = s; }
    float get_gravity_scale() const { return gravity_scale_; }
    void set_linear_damp(float d) { linear_damp_ = d; }
    void set_angular_damp(float d) { angular_damp_ = d; }
    void set_bounce(float b) { bounce_ = b; }
    float get_bounce() const { return bounce_; }
    void set_friction(float f) { friction_ = f; }
    float get_friction() const { return friction_; }

    void _physics_process(float delta) override;

private:
    float mass_         = 1.0f;
    float inv_mass_     = 1.0f;
    float gravity_scale_ = 1.0f;
    float linear_damp_  = 0.0f;
    float angular_damp_ = 0.0f;
    float bounce_       = 0.0f;
    float friction_     = 0.0f;
};

// VoxCharacterBody2D: cuerpo controlado por script (no afectado por física automática).
// Estilo Godot 4 VoxCharacterBody2D: el usuario llama move_and_slide().
class VoxCharacterBody2D : public PhysicsBody2D {
public:
    ARX_CLASS(VoxCharacterBody2D, PhysicsBody2D);
public:

    void set_velocity(Vector2 v) { velocity_ = v; }
    Vector2 get_velocity() const { return velocity_; }

    // Llamar desde _physics_process para mover + colisionar.
    void move_and_slide();

    bool is_on_floor() const { return on_floor_; }
    bool is_on_wall()  const { return on_wall_; }
    bool is_on_ceiling() const { return on_ceiling_; }

private:
    Vector2 velocity_   = {0, 0};
    bool    on_floor_   = false;
    bool    on_wall_    = false;
    bool    on_ceiling_ = false;
};

// VoxArea2D: zona de detección (no bloquea cuerpos, solo detecta).
class VoxArea2D : public Vox2D {
public:
    ARX_CLASS(VoxArea2D, Vox2D);
public:

    void set_monitoring(bool m) { monitoring_ = m; }
    bool is_monitoring() const { return monitoring_; }
    void set_monitorable(bool m) { monitorable_ = m; }
    bool is_monitorable() const { return monitorable_; }

    // Signals: body_entered(body), body_exited(body), area_entered(area), area_exited(area)

private:
    bool monitoring_  = true;
    bool monitorable_ = false;
};

} // namespace arx

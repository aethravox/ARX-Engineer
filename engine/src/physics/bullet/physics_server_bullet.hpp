// ==============================================================================
// src/physics/bullet/physics_server_bullet.hpp / .cpp
// Wrapper de Bullet Physics 3.24.
// ==============================================================================
#pragma once

#include "physics/physics_server.hpp"

// Bullet forward declarations
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btCollisionShape;
class btRigidBody;
class btMotionState;

namespace arx {

class PhysicsServerBullet : public PhysicsServer {
public:
    PhysicsServerBullet() = default;
    ~PhysicsServerBullet() override;

    bool init() override;
    void step(float delta) override;
    void shutdown() override;

    RID  space_create() override;
    void space_set_gravity(RID space, Vector3 g) override;
    void space_set_active(RID space, bool active) override;

    RID  shape_create(ShapeType type, Vector3 half_extents,
                      float radius, float height) override;

    RID  body_create(BodyType type, RID space, RID shape,
                     const Transform3D& initial) override;
    void body_set_transform(RID body, const Transform3D& t) override;
    Transform3D body_get_transform(RID body) const override;
    void body_set_linear_velocity(RID body, Vector3 v) override;
    Vector3 body_get_linear_velocity(RID body) const override;
    void body_apply_impulse(RID body, Vector3 impulse, Vector3 rel_pos) override;
    void body_set_mass(RID body, float m) override;
    void body_set_friction(RID body, float f) override;
    void body_set_restitution(RID body, float r) override;
    void body_set_collision_mask(RID body, uint32_t mask) override;
    void body_set_collision_layer(RID body, uint32_t layer) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> p_;
    uint64_t next_id_ = 1;
};

} // namespace arx

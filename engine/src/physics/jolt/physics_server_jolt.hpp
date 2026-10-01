// ==============================================================================
// physics/jolt/physics_server_jolt.hpp
//
// Implementación de PhysicsServer con Jolt Physics.
// Reemplaza a PhysicsServerBullet. Misma interfaz, distinta engine.
// ==============================================================================
#pragma once

#include "physics/physics_server.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <unordered_map>

namespace JPH {
    // Ya definidos por los includes arriba
}

namespace arx {

class PhysicsServerJolt : public PhysicsServer {
public:
    PhysicsServerJolt() = default;
    ~PhysicsServerJolt() override;

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
    // Miembros directos (sin Pimpl para evitar problemas de incomplete type)
    JPH::PhysicsSystem* physics_system_ = nullptr;
    JPH::BodyInterface* body_interface_ = nullptr;
    JPH::TempAllocatorImpl* temp_allocator_ = nullptr;
    JPH::JobSystemThreadPool* job_system_ = nullptr;

    // Broadphase: estáticos en el .cpp (sin estado, seguros como static)

    std::unordered_map<uint64_t, JPH::Ref<JPH::Shape>> shapes_;
    std::unordered_map<uint64_t, JPH::BodyID> bodies_;
    JPH::Vec3 gravity_ = JPH::Vec3(0.0f, -9.8f, 0.0f);
    uint64_t next_id_ = 1;
};

} // namespace arx

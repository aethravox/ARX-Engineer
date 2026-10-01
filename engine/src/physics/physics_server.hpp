// ==============================================================================
// src/physics/physics_server.hpp — Servidor de física 3D (wrapper de Bullet).
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <memory>
#include <vector>
#include <cstdint>

namespace arx {

// Tipos de cuerpo.
enum class BodyType : uint8_t { Static, Kinematic, Dynamic };

// Shape primitivas.
enum class ShapeType : uint8_t {
    Box, Sphere, Capsule, Cylinder, ConvexMesh, ConcaveMesh, Heightfield
};

// RID: Resource ID dentro del PhysicsServer (estilo Godot).
struct RID { uint64_t id = 0; bool valid() const { return id != 0; } };

class PhysicsServer {
public:
    virtual ~PhysicsServer() = default;
    static std::unique_ptr<PhysicsServer> create();

    virtual bool init() = 0;
    virtual void step(float delta) = 0;
    virtual void shutdown() = 0;

    // Espacio (world).
    virtual RID  space_create() = 0;
    virtual void space_set_gravity(RID space, Vector3 g) = 0;
    virtual void space_set_active(RID space, bool active) = 0;

    // Shape.
    virtual RID  shape_create(ShapeType type,
                              Vector3 half_extents = {0.5f,0.5f,0.5f},
                              float radius = 0.5f,
                              float height = 1.0f) = 0;

    // Body.
    virtual RID  body_create(BodyType type, RID space, RID shape,
                              const Transform3D& initial = {}) = 0;
    virtual void body_set_transform(RID body, const Transform3D& t) = 0;
    virtual Transform3D body_get_transform(RID body) const = 0;
    virtual void body_set_linear_velocity(RID body, Vector3 v) = 0;
    virtual Vector3 body_get_linear_velocity(RID body) const = 0;
    virtual void body_apply_impulse(RID body, Vector3 impulse, Vector3 rel_pos = {}) = 0;
    virtual void body_set_mass(RID body, float m) = 0;
    virtual void body_set_friction(RID body, float f) = 0;
    virtual void body_set_restitution(RID body, float r) = 0;
    virtual void body_set_collision_mask(RID body, uint32_t mask) = 0;
    virtual void body_set_collision_layer(RID body, uint32_t layer) = 0;
};

} // namespace arx

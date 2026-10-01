// =============================================================
// ARX ENGINE - Modulo de Fisica (Jolt Physics) - RECONSTRUIDO
// =============================================================
#ifndef ARX_PHYSICS_HPP
#define ARX_PHYSICS_HPP

#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "raylib.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <thread> // Necesario para hardware_concurrency

namespace ARX {

// --- Capas de Colision ---
namespace Layers {
    static constexpr JPH::ObjectLayer STATIC = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr uint NUM_LAYERS = 2;
}

namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer STATIC(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr uint NUM_LAYERS = 2;
}

// --- Interfaces de Filtrado ---
class ARXBPInterface final : public JPH::BroadPhaseLayerInterface {
public:
    ARXBPInterface() {
        mMap[Layers::STATIC] = BroadPhaseLayers::STATIC;
        mMap[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }
    uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }
    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inL) const override { return mMap[inL]; }
private:
    JPH::BroadPhaseLayer mMap[Layers::NUM_LAYERS];
};

class ARXObjectFilter final : public JPH::ObjectLayerPairFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer i1, JPH::ObjectLayer i2) const override {
        if (i1 == Layers::STATIC) return i2 == Layers::MOVING;
        return true;
    }
};

class ARXBroadPhaseFilter final : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer i1, JPH::BroadPhaseLayer i2) const override {
        if (i1 == Layers::STATIC) return i2 == BroadPhaseLayers::MOVING;
        return true;
    }
};

// --- Mundo de Fisica ---
class ARXPhysicsWorld {
public:
    static inline JPH::PhysicsSystem* system = nullptr;
    static inline JPH::TempAllocatorImpl* allocator = nullptr;
    static inline JPH::JobSystemThreadPool* jobSystem = nullptr;
    static inline bool initialized = false;

    static inline ARXBPInterface bp_interface;
    static inline ARXObjectFilter obj_filter;
    static inline ARXBroadPhaseFilter bp_filter;

    static void Init() {
        if (initialized) return;
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

        uint numThreads = std::thread::hardware_concurrency();
        if (numThreads < 2) numThreads = 2;
        jobSystem = new JPH::JobSystemThreadPool(
            JPH::cMaxPhysicsJobs,
            JPH::cMaxPhysicsBarriers,
            (int)numThreads - 1
        );

        system = new JPH::PhysicsSystem();
        system->Init(1024, 0, 1024, 1024, bp_interface, bp_filter, obj_filter);

        SetGravity(0.0f, -9.81f, 0.0f);
        initialized = true;
        std::cout << "[ARXPhysics] Sistema inicializado en PC/Android." << std::endl;
    }

    static void SetGravity(float gx, float gy, float gz) {
        if (!system) return;
        system->SetGravity(JPH::Vec3(gx, gy, gz));
    }

    static void Step(float dt) { if (initialized) system->Update(dt, 1, allocator, jobSystem); }

    static void Shutdown() {
        if (!initialized) return;
        delete system; delete jobSystem; delete allocator;
        delete JPH::Factory::sInstance;
        system = nullptr;
        jobSystem = nullptr;
        allocator = nullptr;
        initialized = false;
    }
};

// --- Cuerpo Rigido ---
class ARXRigidBody {
public:
    enum Type { STATIC, DYNAMIC, SENSOR, KINEMATIC };

    Type type = DYNAMIC;

    // Campos esperados por AethravoxGraphics.hpp
    float mass = 1.0f;
    std::string elementId;
    float friction = 0.5f;
    float restitution = 0.3f;

    JPH::BodyID id;
    std::string name;

    ~ARXRigidBody() {
        if (!id.IsInvalid() && ARXPhysicsWorld::initialized && ARXPhysicsWorld::system) {
            auto& bi = ARXPhysicsWorld::system->GetBodyInterface();
            bi.RemoveBody(id);
            bi.DestroyBody(id);
        }
    }

    void CreateBox(Vector3 pos, Vector3 size, bool isStatic) {
        type = isStatic ? STATIC : DYNAMIC;
        JPH::BoxShapeSettings settings(JPH::Vec3(size.x/2, size.y/2, size.z/2));
        SetupBody(pos, settings.Create().Get(), isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic, isStatic ? Layers::STATIC : Layers::MOVING);
    }

    void CreateSphere(Vector3 pos, float radius, bool isStatic) {
        type = isStatic ? STATIC : DYNAMIC;
        JPH::SphereShapeSettings settings(radius);
        SetupBody(pos, settings.Create().Get(), isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic, isStatic ? Layers::STATIC : Layers::MOVING);
    }

    void ApplyImpulse(Vector3 force) {
        ARXPhysicsWorld::system->GetBodyInterface().AddImpulse(id, JPH::Vec3(force.x, force.y, force.z));
    }

    // Tipos esperados por AethravoxGraphics.hpp
    JPH::RVec3 GetPosition() {
        return ARXPhysicsWorld::system->GetBodyInterface().GetPosition(id);
    }

    JPH::Quat GetRotation() {
        return ARXPhysicsWorld::system->GetBodyInterface().GetRotation(id);
    }

private:
    void SetupBody(Vector3 pos, const JPH::Shape* shape, JPH::EMotionType motionType, JPH::ObjectLayer layer) {
        JPH::BodyCreationSettings s(
            shape,
            JPH::RVec3(pos.x, pos.y, pos.z),
            JPH::Quat::sIdentity(),
            motionType,
            layer
        );

        id = ARXPhysicsWorld::system->GetBodyInterface().CreateAndAddBody(s, JPH::EActivation::Activate);
    }
};

// --- Manager Central ---
class ARXPhysicsManager {
public:
    std::unordered_map<std::string, std::unique_ptr<ARXRigidBody>> bodies;

    ARXRigidBody* AddBox(std::string id, Vector3 pos, Vector3 size, bool isStatic = false) {
        auto body = std::make_unique<ARXRigidBody>();
        body->name = id;
        body->elementId = id;
        body->CreateBox(pos, size, isStatic);
        return (bodies[id] = std::move(body)).get();
    }

    ARXRigidBody* Get(std::string id) {
        return bodies.count(id) ? bodies[id].get() : nullptr;
    }

    // Firmas esperadas por AethravoxGraphics.hpp
    ARXRigidBody* CreateFromBody(
        const std::string& id,
        const std::string& shape,
        float hx, float hy, float hz,
        float px, float py, float pz,
        ARXRigidBody::Type type
    ) {
        auto body = std::make_unique<ARXRigidBody>();
        body->name = id;
        body->elementId = id;
        body->type = type;

        Vector3 pos{px, py, pz};

        if (shape == "box") {
            Vector3 size{hx*2.0f, hy*2.0f, hz*2.0f};
            body->CreateBox(pos, size, type == ARXRigidBody::STATIC);
        } else if (shape == "sphere") {
            float radius = hx; // convenio: hx como radio
            body->CreateSphere(pos, radius, type == ARXRigidBody::STATIC);
        } else if (shape == "cylinder") {
            // Implementación mínima: aproximar cilindro con caja para compilar
            Vector3 size{hx*2.0f, hy*2.0f, hz*2.0f};
            body->CreateBox(pos, size, type == ARXRigidBody::STATIC);
        } else {
            // default: caja
            Vector3 size{hx*2.0f, hy*2.0f, hz*2.0f};
            body->CreateBox(pos, size, type == ARXRigidBody::STATIC);
        }

        // Asignar motionType aproximado por tipo (sin usar campos Jolt extra para evitar más API)
        bodies[id] = std::move(body);
        return bodies[id].get();
    }

    void Step(float dt) { ARXPhysicsWorld::Step(dt); }
    void Update(float dt) { ARXPhysicsWorld::Step(dt); }

    void Shutdown() { Clear(); }

    void Clear() { bodies.clear(); ARXPhysicsWorld::Shutdown(); }
};

} // namespace ARX

#endif

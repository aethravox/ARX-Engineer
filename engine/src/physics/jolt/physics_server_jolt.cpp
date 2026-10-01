// ==============================================================================
// physics/jolt/physics_server_jolt.cpp
//
// Implementación de PhysicsServer con Jolt Physics.
// ==============================================================================

#include "physics_server_jolt.hpp"
#include "core/logging.hpp"

// Jolt includes
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

JPH_SUPPRESS_WARNINGS

namespace arx {

// Layers para colisión (simplificación: 2 layers)
namespace Layers {
    static constexpr JPH::ObjectLayer STATIC = 0;
    static constexpr JPH::ObjectLayer DYNAMIC = 1;
    static constexpr JPH::ObjectLayer NUM = 2;
}

// Filtro de colisión simple: todo choca con todo
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    JPH::uint GetNumBroadPhaseLayers() const override { return Layers::NUM; }
    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        return JPH::BroadPhaseLayer(inLayer);
    }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        return (inLayer == Layers::STATIC) ? "STATIC" : "DYNAMIC";
    }
#endif
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::BroadPhaseLayer) const override { return true; }
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::ObjectLayer) const override { return true; }
};

// Broadphase estáticos (simples, sin estado)
static BPLayerInterfaceImpl s_bpl_interface;
static ObjectVsBroadPhaseLayerFilterImpl s_obj_vs_bp_filter;
static ObjectLayerPairFilterImpl s_obj_vs_obj_filter;

PhysicsServerJolt::~PhysicsServerJolt() {
    shutdown();
}

bool PhysicsServerJolt::init() {
    if (physics_system_) return true;  // ya inicializado

    // Registrar tipos de Jolt
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // Temp allocator (para thread-local allocations durante step)
    temp_allocator_ = new JPH::TempAllocatorImpl(10 * 1024 * 1024);  // 10 MB

    // Job system (thread pool)
    const uint32_t num_threads = std::max(1u, std::thread::hardware_concurrency() - 1);
    job_system_ = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, num_threads);

    // PhysicsSystem
    const uint32_t max_bodies = 10240;
    const uint32_t num_body_mutexes = 0;
    const uint32_t max_body_pairs = 65536;
    const uint32_t max_contact_constraints = 10240;
    physics_system_ = new JPH::PhysicsSystem();
    physics_system_->Init(
        max_bodies, num_body_mutexes, max_body_pairs, max_contact_constraints,
        s_bpl_interface,
        s_obj_vs_bp_filter,
        s_obj_vs_obj_filter
    );

    // Configurar gravedad
    physics_system_->SetGravity(gravity_);

    // Obtener body interface
    body_interface_ = &physics_system_->GetBodyInterface();

    // Optimizar broadphase (necesario antes de step)
    physics_system_->OptimizeBroadPhase();

    ARX_LOG_INFO("PhysicsServerJolt inicializado (Jolt Physics, {} threads)", num_threads);
    return true;
}

void PhysicsServerJolt::step(float delta) {
    if (!physics_system_) return;

    // Jolt usa un collision step interno. Mejor performance con múltiples sub-steps.
    const int collision_steps = 1;
    physics_system_->Update(delta, collision_steps, temp_allocator_, job_system_);
}

void PhysicsServerJolt::shutdown() {
    if (!physics_system_) return;

    // Limpiar bodies
    if (body_interface_ && physics_system_) {
        for (auto& [id, body_id] : bodies_) {
            body_interface_->RemoveBody(body_id);
            body_interface_->DestroyBody(body_id);
        }
    }

    bodies_.clear();
    shapes_.clear();

    delete physics_system_;
    delete job_system_;
    delete temp_allocator_;

    physics_system_ = nullptr;
    job_system_ = nullptr;
    temp_allocator_ = nullptr;

    // Cleanup Jolt
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    body_interface_ = nullptr;
    ARX_LOG_INFO("PhysicsServerJolt apagado");
}

RID PhysicsServerJolt::space_create() {
    // En Jolt, el "space" es el PhysicsSystem mismo.
    // Como tenemos un solo mundo, retornamos un RID fijo.
    RID r;
    r.id = next_id_++;
    return r;
}

void PhysicsServerJolt::space_set_gravity(RID space, Vector3 g) {
    if (!physics_system_) return;
    gravity_ = JPH::Vec3(g.x, g.y, g.z);
    physics_system_->SetGravity(gravity_);
}

void PhysicsServerJolt::space_set_active(RID space, bool active) {
    // En Jolt no hay concept de "active space". El mundo siempre está activo.
    (void)space;
    (void)active;
}

RID PhysicsServerJolt::shape_create(ShapeType type, Vector3 half_extents,
                                      float radius, float height) {
    if (!physics_system_) return {};

    RID rid;
    rid.id = next_id_++;

    JPH::Ref<JPH::Shape> shape;

    switch (type) {
        case ShapeType::Box:
            shape = new JPH::BoxShape(JPH::Vec3(half_extents.x, half_extents.y, half_extents.z));
            break;
        case ShapeType::Sphere:
            shape = new JPH::SphereShape(radius);
            break;
        case ShapeType::Capsule:
            shape = new JPH::CapsuleShape(height * 0.5f, radius);
            break;
        case ShapeType::Cylinder:
            shape = new JPH::CylinderShape(height * 0.5f, radius);
            break;
        default:
            shape = new JPH::BoxShape(JPH::Vec3(half_extents.x, half_extents.y, half_extents.z));
            break;
    }

    shapes_[rid.id] = shape;
    return rid;
}

RID PhysicsServerJolt::body_create(BodyType type, RID space, RID shape,
                                     const Transform3D& initial) {
    if (!body_interface_) return {};

    // Buscar la shape
    auto shape_it = shapes_.find(shape.id);
    if (shape_it == shapes_.end()) return {};

    // Mapear BodyType a JPH::EMotionType
    JPH::EMotionType motion_type;
    JPH::ObjectLayer layer;
    switch (type) {
        case BodyType::Static:
            motion_type = JPH::EMotionType::Static;
            layer = Layers::STATIC;
            break;
        case BodyType::Kinematic:
            motion_type = JPH::EMotionType::Kinematic;
            layer = Layers::DYNAMIC;
            break;
        case BodyType::Dynamic:
        default:
            motion_type = JPH::EMotionType::Dynamic;
            layer = Layers::DYNAMIC;
            break;
    }

    // Crear body
    JPH::BodyCreationSettings settings(
        shape_it->second,
        JPH::RVec3(initial.origin.x, initial.origin.y, initial.origin.z),
        JPH::Quat::sIdentity(),
        motion_type,
        layer
    );

    JPH::BodyID body_id = body_interface_->CreateAndAddBody(settings, JPH::EActivation::Activate);

    RID rid;
    rid.id = next_id_++;
    bodies_[rid.id] = body_id;

    return rid;
}

void PhysicsServerJolt::body_set_transform(RID body, const Transform3D& t) {
    if (!body_interface_) return;
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return;
    body_interface_->SetPositionAndRotation(
        it->second,
        JPH::RVec3(t.origin.x, t.origin.y, t.origin.z),
        JPH::Quat::sIdentity(),
        JPH::EActivation::Activate
    );
}

Transform3D PhysicsServerJolt::body_get_transform(RID body) const {
    if (!body_interface_) return {};
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return {};

    JPH::RVec3 pos;
    JPH::Quat rot;
    body_interface_->GetPositionAndRotation(it->second, pos, rot);

    Transform3D t;
    t.origin = Vector3{(float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ()};
    return t;
}

void PhysicsServerJolt::body_set_linear_velocity(RID body, Vector3 v) {
    if (!body_interface_) return;
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return;
    body_interface_->SetLinearVelocity(it->second, JPH::Vec3(v.x, v.y, v.z));
}

Vector3 PhysicsServerJolt::body_get_linear_velocity(RID body) const {
    if (!body_interface_) return {};
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return {};
    JPH::Vec3 v = body_interface_->GetLinearVelocity(it->second);
    return Vector3{v.GetX(), v.GetY(), v.GetZ()};
}

void PhysicsServerJolt::body_apply_impulse(RID body, Vector3 impulse, Vector3 rel_pos) {
    if (!body_interface_) return;
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return;
    body_interface_->AddImpulse(it->second, JPH::Vec3(impulse.x, impulse.y, impulse.z),
                                    JPH::Vec3(rel_pos.x, rel_pos.y, rel_pos.z));
}

void PhysicsServerJolt::body_set_mass(RID body, float m) {
    // Jolt calcula masa automáticamente desde la shape + densidad
    // Para override, se usa MotionProperties::SetMassProperties
    (void)body;
    (void)m;
    // TODO: implementar override de masa
}

void PhysicsServerJolt::body_set_friction(RID body, float f) {
    if (!body_interface_) return;
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return;
    body_interface_->SetFriction(it->second, f);
}

void PhysicsServerJolt::body_set_restitution(RID body, float r) {
    if (!body_interface_) return;
    auto it = bodies_.find(body.id);
    if (it == bodies_.end()) return;
    body_interface_->SetRestitution(it->second, r);
}

void PhysicsServerJolt::body_set_collision_mask(RID body, uint32_t mask) {
    (void)body;
    (void)mask;
    // TODO: implementar collision mask en Jolt
}

void PhysicsServerJolt::body_set_collision_layer(RID body, uint32_t layer) {
    (void)body;
    (void)layer;
    // TODO: implementar collision layer en Jolt
}

} // namespace arx

namespace arx {
std::unique_ptr<PhysicsServer> create_physics_server_jolt() {
    auto s = std::make_unique<PhysicsServerJolt>();
    if (s->init()) return s;
    return nullptr;
}
} // namespace arx

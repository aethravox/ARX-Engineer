// ==============================================================================
// src/physics/bullet/physics_server_bullet.cpp
// Implementación que envuelve Bullet Physics. Notas:
//  - Cada RID es un uint64 incremental; mapea internamente a btRigidBody*.
//  - Se mantiene un único btDiscreteDynamicsWorld por space_create.
//  - En esta versión se omite el motion-state personalizado; se sincroniza
//    transform manualmente desde body_get_transform.
// ==============================================================================
#include "physics_server_bullet.hpp"
#include "core/logging.hpp"

#include <btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>

namespace arx {

struct PhysicsServerBullet::Pimpl {
    std::unordered_map<uint64_t, std::unique_ptr<btDiscreteDynamicsWorld>> spaces;
    std::unordered_map<uint64_t, std::unique_ptr<btCollisionShape>>         shapes;
    std::unordered_map<uint64_t, std::unique_ptr<btRigidBody>>              bodies;
    std::unordered_map<uint64_t, std::unique_ptr<btDefaultMotionState>>     motion_states;

    // Componentes del dynamics world por defecto.
    std::unique_ptr<btDefaultCollisionConfiguration>     cfg;
    std::unique_ptr<btCollisionDispatcher>               dispatcher;
    std::unique_ptr<btDbvtBroadphase>                    broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;

    uint64_t next_rid = 1;
    uint64_t new_rid() { return next_rid++; }
};

PhysicsServerBullet::~PhysicsServerBullet() { shutdown(); }

bool PhysicsServerBullet::init() {
    p_ = std::make_unique<Pimpl>();
    p_->cfg        = std::make_unique<btDefaultCollisionConfiguration>();
    p_->dispatcher = std::make_unique<btCollisionDispatcher>(p_->cfg.get());
    p_->broadphase = std::make_unique<btDbvtBroadphase>();
    p_->solver     = std::make_unique<btSequentialImpulseConstraintSolver>();
    ARX_LOG_INFO("PhysicsServerBullet inicializado (Bullet 3.24)");
    return true;
}

void PhysicsServerBullet::shutdown() {
    if (!p_) return;
    p_->bodies.clear();
    p_->motion_states.clear();
    p_->shapes.clear();
    p_->spaces.clear();
    p_->solver.reset();
    p_->broadphase.reset();
    p_->dispatcher.reset();
    p_->cfg.reset();
    p_.reset();
}

void PhysicsServerBullet::step(float delta) {
    if (!p_) return;
    for (auto& [_, world] : p_->spaces) {
        world->stepSimulation(delta, 8, 1.0f / 120.0f);
    }
}

RID PhysicsServerBullet::space_create() {
    RID r; r.id = p_->new_rid();
    auto world = std::make_unique<btDiscreteDynamicsWorld>(
        p_->dispatcher.get(), p_->broadphase.get(),
        p_->solver.get(), p_->cfg.get());
    world->setGravity(btVector3(0, -9.81f, 0));
    p_->spaces[r.id] = std::move(world);
    return r;
}

void PhysicsServerBullet::space_set_gravity(RID space, Vector3 g) {
    auto it = p_->spaces.find(space.id);
    if (it != p_->spaces.end()) it->second->setGravity(btVector3(g.x, g.y, g.z));
}

void PhysicsServerBullet::space_set_active(RID space, bool active) {
    (void)space; (void)active;
    // Bullet no tiene concepto explícito de "active" en dynamics world.
}

RID PhysicsServerBullet::shape_create(ShapeType type, Vector3 half_extents,
                                        float radius, float height) {
    RID r; r.id = p_->new_rid();
    std::unique_ptr<btCollisionShape> shape;
    switch (type) {
        case ShapeType::Box:
            shape = std::make_unique<btBoxShape>(
                btVector3(half_extents.x, half_extents.y, half_extents.z));
            break;
        case ShapeType::Sphere:
            shape = std::make_unique<btSphereShape>(radius);
            break;
        case ShapeType::Capsule:
            shape = std::make_unique<btCapsuleShape>(radius, height);
            break;
        case ShapeType::Cylinder:
            shape = std::make_unique<btCylinderShape>(
                btVector3(half_extents.x, half_extents.y, half_extents.z));
            break;
        default:
            ARX_LOG_WARN("PhysicsServerBullet: ShapeType no soportado, fallback Box");
            shape = std::make_unique<btBoxShape>(
                btVector3(half_extents.x, half_extents.y, half_extents.z));
    }
    p_->shapes[r.id] = std::move(shape);
    return r;
}

RID PhysicsServerBullet::body_create(BodyType type, RID space, RID shape,
                                       const Transform3D& initial) {
    auto sit = p_->spaces.find(space.id);
    auto shp = p_->shapes.find(shape.id);
    if (sit == p_->spaces.end() || shp == p_->shapes.end()) {
        ARX_LOG_ERROR("body_create: space o shape inválidos");
        return {};
    }

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(initial.origin.x, initial.origin.y, initial.origin.z));
    const Matrix3& m = initial.basis;
    t.setBasis(btMatrix3x3(m[0][0],m[1][0],m[2][0],
                           m[0][1],m[1][1],m[2][1],
                           m[0][2],m[1][2],m[2][2]));

    float mass = (type == BodyType::Dynamic) ? 1.0f : 0.0f;
    btVector3 local_inertia(0,0,0);
    if (mass > 0.0f) shp->second->calculateLocalInertia(mass, local_inertia);

    auto ms = std::make_unique<btDefaultMotionState>(t);
    btRigidBody::btRigidBodyConstructionInfo ci(mass, ms.get(),
                                                 shp->second.get(), local_inertia);
    auto body = std::make_unique<btRigidBody>(ci);

    if (type == BodyType::Kinematic) {
        body->setCollisionFlags(body->getCollisionFlags() |
                                 btCollisionObject::CF_KINEMATIC_OBJECT);
        body->setActivationState(DISABLE_DEACTIVATION);
    }

    sit->second->addRigidBody(body.get());

    RID r; r.id = p_->new_rid();
    p_->motion_states[r.id] = std::move(ms);
    p_->bodies[r.id]        = std::move(body);
    return r;
}

void PhysicsServerBullet::body_set_transform(RID body, const Transform3D& tr) {
    auto it = p_->bodies.find(body.id);
    if (it == p_->bodies.end()) return;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(tr.origin.x, tr.origin.y, tr.origin.z));
    const Matrix3& m = tr.basis;
    t.setBasis(btMatrix3x3(m[0][0],m[1][0],m[2][0],
                           m[0][1],m[1][1],m[2][1],
                           m[0][2],m[1][2],m[2][2]));
    it->second->setWorldTransform(t);
}

Transform3D PhysicsServerBullet::body_get_transform(RID body) const {
    auto it = p_->bodies.find(body.id);
    if (it == p_->bodies.end()) return {};
    const btTransform& t = it->second->getWorldTransform();
    Transform3D out;
    out.origin = Vector3(t.getOrigin().x(), t.getOrigin().y(), t.getOrigin().z());
    const btMatrix3x3& b = t.getBasis();
    out.basis = Matrix3(
        b[0][0], b[1][0], b[2][0],
        b[0][1], b[1][1], b[2][1],
        b[0][2], b[1][2], b[2][2]);
    return out;
}

void PhysicsServerBullet::body_set_linear_velocity(RID body, Vector3 v) {
    auto it = p_->bodies.find(body.id);
    if (it != p_->bodies.end()) it->second->setLinearVelocity(btVector3(v.x,v.y,v.z));
}
Vector3 PhysicsServerBullet::body_get_linear_velocity(RID body) const {
    auto it = p_->bodies.find(body.id);
    if (it == p_->bodies.end()) return {};
    const btVector3& v = it->second->getLinearVelocity();
    return Vector3(v.x(), v.y(), v.z());
}
void PhysicsServerBullet::body_apply_impulse(RID body, Vector3 impulse, Vector3 rel_pos) {
    auto it = p_->bodies.find(body.id);
    if (it != p_->bodies.end()) it->second->applyImpulse(
        btVector3(impulse.x,impulse.y,impulse.z),
        btVector3(rel_pos.x,rel_pos.y,rel_pos.z));
}
void PhysicsServerBullet::body_set_mass(RID body, float m) {
    auto it = p_->bodies.find(body.id);
    if (it == p_->bodies.end()) return;
    btVector3 inertia(0,0,0);
    if (m > 0) it->second->getCollisionShape()->calculateLocalInertia(m, inertia);
    it->second->setMassProps(m, inertia);
}
void PhysicsServerBullet::body_set_friction(RID body, float f) {
    auto it = p_->bodies.find(body.id);
    if (it != p_->bodies.end()) it->second->setFriction(f);
}
void PhysicsServerBullet::body_set_restitution(RID body, float r) {
    auto it = p_->bodies.find(body.id);
    if (it != p_->bodies.end()) it->second->setRestitution(r);
}
void PhysicsServerBullet::body_set_collision_mask(RID body, uint32_t mask) {
    // Simplificación: en producción se haría con world->removeRigidBody + addRigidBody(mask).
    (void)body; (void)mask;
}
void PhysicsServerBullet::body_set_collision_layer(RID body, uint32_t layer) {
    (void)body; (void)layer;
}

// Factory está en physics/physics_server_factory.cpp (separado para evitar
// problemas con Pimpl incompleto de Jolt/Bullet en el mismo TU).

} // namespace arx

namespace arx {
std::unique_ptr<PhysicsServer> create_physics_server_bullet() {
    auto s = std::make_unique<PhysicsServerBullet>();
    if (s->init()) return s;
    return nullptr;
}
} // namespace arx

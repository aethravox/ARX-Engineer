// ==============================================================================
// physics/physics_server_factory.cpp
//
// Factory de PhysicsServer. Detecta CPU y elige el mejor backend:
//   - SSE4.1+ → Jolt Physics (mejor performance, determinismo)
//   - SSE2    → Bullet Physics (compatibility, funciona en cualquier x86_64)
//
// El usuario también puede forzar un backend desde Project Settings.
// ==============================================================================
#include "physics/physics_server.hpp"
#include "core/logging.hpp"

// No incluir headers de implementaciones concretas aquí para evitar
// problemas de Pimpl incompleto. Usamos funciones auxiliares declaradas
// en los .cpp de cada implementación.

#ifdef ARX_JOLT_ENABLED
namespace arx {
    std::unique_ptr<PhysicsServer> create_physics_server_jolt();
}
#endif

#ifdef ARX_BULLET_ENABLED
namespace arx {
    std::unique_ptr<PhysicsServer> create_physics_server_bullet();
}
#endif

namespace arx {

// Detectar soporte de SSE4.1 en runtime
static bool cpu_has_sse41() {
#if defined(__x86_64__) || defined(__i386__)
    return __builtin_cpu_supports("sse4.1");
#else
    return true;
#endif
}

std::unique_ptr<PhysicsServer> PhysicsServer::create() {
    bool has_sse41 = cpu_has_sse41();

#ifdef ARX_JOLT_ENABLED
    if (has_sse41) {
        ARX_LOG_INFO("PhysicsServer: CPU soporta SSE4.1 — intentando Jolt Physics");
        auto s = create_physics_server_jolt();
        if (s) {
            ARX_LOG_INFO("PhysicsServer: Jolt Physics activado");
            return s;
        }
        ARX_LOG_WARN("PhysicsServer: Jolt init falló, fallback a Bullet");
    } else {
        ARX_LOG_INFO("PhysicsServer: CPU sin SSE4.1 — usando Bullet (compatibility)");
    }
#endif

#ifdef ARX_BULLET_ENABLED
    {
        ARX_LOG_INFO("PhysicsServer: usando Bullet Physics");
        auto s = create_physics_server_bullet();
        if (s) {
            ARX_LOG_INFO("PhysicsServer: Bullet Physics activado");
            return s;
        }
    }
#endif

    ARX_LOG_ERROR("PhysicsServer: ningún backend disponible!");
    return nullptr;
}

} // namespace arx

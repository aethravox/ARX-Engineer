// ==============================================================================
// src/navigation/navigation.cpp — Stub simplificado de pathfinding.
// Una implementación completa usaría Detour's dtNavMesh + dtNavMeshQuery.
// Aquí usamos A* sobre los polígonos para que compile.
// ==============================================================================
#include "navigation.hpp"
#include "core/logging.hpp"

#include <cmath>
#include <algorithm>

namespace arx {

// ===================== NavigationMesh =======================================
bool NavigationMesh::bake_from_geometry(const std::vector<Vector3>& verts,
                                          const std::vector<uint32_t>& idx,
                                          float cell_size,
                                          float agent_height,
                                          float agent_radius) {
    (void)cell_size; (void)agent_height; (void)agent_radius;
    vertices_ = verts;
    // Cada 3 índices forman un triángulo.
    polygons_.clear();
    for (size_t i = 0; i + 2 < idx.size(); i += 3) {
        Polygon p;
        p.indices = { idx[i], idx[i+1], idx[i+2] };
        Vector3 acc{0,0,0};
        for (uint32_t vi : p.indices) acc += verts[vi];
        p.center = acc / 3.0f;
        polygons_.push_back(std::move(p));
    }
    ARX_LOG_INFO("NavigationMesh baked: {} verts, {} polys",
                 vertices_.size(), polygons_.size());
    return true;
}

std::vector<Vector3> NavigationMesh::find_path(Vector3 from, Vector3 to) const {
    // Stub: pathfinding trivial = ir directo al target.
    // En producción se usaría Detour (dtNavMeshQuery::findPath).
    return { from, to };
}

// ===================== NavigationServer =====================================
NavigationServer& NavigationServer::instance() {
    static NavigationServer s;
    return s;
}

std::shared_ptr<NavigationMesh> NavigationServer::create_navigation_mesh() {
    return std::make_shared<NavigationMesh>();
}

std::vector<Vector3> NavigationServer::find_path(
    const std::shared_ptr<NavigationMesh>& nav, Vector3 from, Vector3 to) {
    if (!nav) return {};
    return nav->find_path(from, to);
}

// ===================== NavigationAgent3D ====================================
void NavigationAgent3D::set_target_position(Vector3 t) {
    target_ = t;
    if (navmesh_) path_ = navmesh_->find_path(position_, target_);
    path_idx_ = 0;
}

Vector3 NavigationAgent3D::get_next_location() const {
    if (path_idx_ < (int)path_.size()) return path_[path_idx_];
    return target_;
}

float NavigationAgent3D::distance_to_target() const {
    return glm::distance(position_, target_);
}

void NavigationAgent3D::_physics_process(float delta) {
    if (is_navigation_finished()) return;
    Vector3 next = get_next_location();
    Vector3 dir  = next - position_;
    float dist   = glm::length(dir);
    if (dist < 0.1f) {
        ++path_idx_;
        return;
    }
    dir /= dist;
    position_ += dir * speed_ * delta;
    dirty_ = true;
}

} // namespace arx

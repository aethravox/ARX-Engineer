// ==============================================================================
// src/navigation/navigation.hpp — Pathfinding con Recast/Detour.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <vector>
#include <memory>

namespace arx {

// NavigationMesh — malla de navegación generada por Recast.
class NavigationMesh : public Object {
public:
    ARX_CLASS(NavigationMesh, Object);
public:

    bool bake_from_geometry(const std::vector<Vector3>& vertices,
                              const std::vector<uint32_t>& indices,
                              float cell_size = 0.3f,
                              float agent_height = 2.0f,
                              float agent_radius = 0.5f);

    std::vector<Vector3> find_path(Vector3 from, Vector3 to) const;

    bool is_valid() const { return !vertices_.empty(); }
    int  get_polygon_count() const { return (int)polygons_.size(); }

private:
    struct Polygon {
        std::vector<uint32_t> indices;
        Vector3 center;
    };
    std::vector<Vector3>  vertices_;
    std::vector<Polygon>  polygons_;

    // En una impl completa aquí habría un dtNavMesh*.
};

// NavigationServer — singleton.
class NavigationServer {
public:
    static NavigationServer& instance();

    std::shared_ptr<NavigationMesh> create_navigation_mesh();
    std::vector<Vector3> find_path(const std::shared_ptr<NavigationMesh>& navmesh,
                                     Vector3 from, Vector3 to);

private:
    NavigationServer() = default;
};

// NavigationAgent3D — nodo que sigue un path automáticamente.
class NavigationAgent3D : public Vox3D {
public:
    ARX_CLASS(NavigationAgent3D, Vox3D);
public:

    void set_target_position(Vector3 t);
    Vector3 get_target_position() const { return target_; }
    void set_navigation_mesh(std::shared_ptr<NavigationMesh> n) { navmesh_ = n; }
    void set_speed(float s) { speed_ = s; }
    void set_radius(float r) { radius_ = r; }

    Vector3 get_next_location() const;
    bool    is_navigation_finished() const { return path_idx_ >= (int)path_.size(); }
    float   distance_to_target() const;

    void _physics_process(float delta) override;

private:
    Vector3 target_{0,0,0};
    float   speed_   = 4.0f;
    float   radius_  = 0.5f;
    std::shared_ptr<NavigationMesh> navmesh_;
    std::vector<Vector3> path_;
    int     path_idx_ = 0;
};

} // namespace arx

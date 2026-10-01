// ==============================================================================
// src/scene/3d/extra_nodes_3d.hpp — Más nodos 3D.
// VoxMultiMeshInstance3D, VoxSprite3D, VoxDecal, Probe, VoxVisibleOnScreenNotifier3D,
// VoxRemoteTransform3D, Position3D, VoxMarker3D, VoxSkeleton3D.
// ==============================================================================
#pragma once

#include "scene/3d/vox3d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "render/texture.hpp"

#include <vector>
#include <memory>

namespace arx {

// VoxSprite3D — un quad 2D que siempre mira a la cámara (billboard).
class VoxSprite3D : public Vox3D {
public:
    ARX_CLASS(VoxSprite3D, Vox3D);
public:

    void set_texture(std::shared_ptr<Texture> t) { texture_ = t; }
    void set_size(Vector2 s) { size_ = s; }
    void set_billboard(bool b) { billboard_ = b; }
    void set_alpha_cut(float a) { alpha_cut_ = a; }
    void set_no_depth_test(bool n) { no_depth_test_ = n; }

private:
    std::shared_ptr<Texture> texture_;
    Vector2 size_{1, 1};
    bool billboard_ = true;
    float alpha_cut_ = 0.5f;
    bool no_depth_test_ = false;
};

// VoxMultiMeshInstance3D — dibuja muchas instancias de la misma mesh.
class VoxMultiMeshInstance3D : public Vox3D {
public:
    ARX_CLASS(VoxMultiMeshInstance3D, Vox3D);
public:

    void set_mesh(std::shared_ptr<Mesh> m) { mesh_ = m; }
    void set_instance_count(int n) { instance_count_ = n; transforms_.resize(n); }
    int  get_instance_count() const { return instance_count_; }

    void set_instance_transform(int i, const Transform3D& t) {
        if (i >= 0 && i < (int)transforms_.size()) transforms_[i] = t;
    }
    Transform3D get_instance_transform(int i) const {
        return (i >= 0 && i < (int)transforms_.size()) ? transforms_[i] : Transform3D{};
    }

private:
    std::shared_ptr<Mesh> mesh_;
    int instance_count_ = 0;
    std::vector<Transform3D> transforms_;
};

// VoxDecal — proyecta una textura sobre la geometría.
class VoxDecal : public Vox3D {
public:
    ARX_CLASS(VoxDecal, Vox3D);
public:

    void set_size(Vector3 s) { size_ = s; }
    void set_texture(std::shared_ptr<Texture> t) { texture_ = t; }
    void set_emission_energy(float e) { emission_energy_ = e; }
    void set_albedo_mix(float m) { albedo_mix_ = m; }

private:
    Vector3 size_{1,1,1};
    std::shared_ptr<Texture> texture_;
    float emission_energy_ = 0.0f;
    float albedo_mix_ = 1.0f;
};

// Position3D / VoxMarker3D — puntos de referencia sin visual.
class VoxMarker3D : public Vox3D {
public:
    ARX_CLASS(VoxMarker3D, Vox3D);
public:
    void set_gizmo_color(Color c) { gizmo_color_ = c; }
    Color get_gizmo_color() const { return gizmo_color_; }
private:
    Color gizmo_color_ = Color::yellow;
};

// VoxRemoteTransform3D — sincroniza su transform con otro Vox remoto.
class VoxRemoteTransform3D : public Vox3D {
public:
    ARX_CLASS(VoxRemoteTransform3D, Vox3D);
public:
    void set_remote_path(const std::string& p) { remote_path_ = p; }
    const std::string& get_remote_path() const { return remote_path_; }
    void set_update_position(bool v) { update_position_ = v; }
    void set_update_rotation(bool v) { update_rotation_ = v; }
    void set_update_scale(bool v)    { update_scale_    = v; }

    void process(float delta) override;

private:
    std::string remote_path_;
    bool update_position_ = true;
    bool update_rotation_ = true;
    bool update_scale_    = true;
};

// VoxVisibleOnScreenNotifier3D — emite signal cuando entra/sale de pantalla.
class VoxVisibleOnScreenNotifier3D : public Vox3D {
public:
    ARX_CLASS(VoxVisibleOnScreenNotifier3D, Vox3D);
public:
    void set_aabb(AABB b) { aabb_ = b; }
    AABB get_aabb() const { return aabb_; }
    bool is_on_screen() const { return on_screen_; }

    void process(float delta) override;

    // signals: screen_entered(), screen_exited()

private:
    AABB aabb_;
    bool on_screen_ = false;
};

// VoxSkeleton3D — esqueleto para animación esquelética.
class VoxSkeleton3D : public Vox3D {
public:
    ARX_CLASS(VoxSkeleton3D, Vox3D);
public:

    struct Bone {
        std::string name;
        int         parent = -1;
        Transform3D rest;
        Transform3D pose;
    };

    int  add_bone(const std::string& name, int parent = -1);
    int  find_bone(const std::string& name) const;
    int  get_bone_count() const { return (int)bones_.size(); }
    const Bone& get_bone(int idx) const { return bones_[idx]; }
    void set_bone_pose(int idx, const Transform3D& p);

    // Skin + binding a un VoxMeshInstance3D para animación esquelética.
    void bind_mesh(VoxMeshInstance3D* m) { bound_mesh_ = m; }

private:
    std::vector<Bone> bones_;
    VoxMeshInstance3D* bound_mesh_ = nullptr;
};

} // namespace arx

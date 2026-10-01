// ==============================================================================
#include "scene/scene_tree.hpp"
// src/scene/3d/mesh_instance_3d.hpp, camera_3d.hpp, light_3d.hpp
// ==============================================================================
#pragma once

#include "vox3d.hpp"
#include "render/renderer.hpp"
#include "render/texture.hpp"
#include "core/math.hpp"

namespace arx {

// VoxMeshInstance3D — dibuja una malla 3D con un material.
class VoxMeshInstance3D : public Vox3D {
public:
    ARX_CLASS(VoxMeshInstance3D, Vox3D);
public:

    void set_mesh(std::shared_ptr<Mesh> m)     { mesh_ = m; }
    void set_shader(std::shared_ptr<Shader> s) { shader_ = s; }
    void set_albedo(Color c)                    { albedo_ = c; }
    Color get_albedo() const                    { return albedo_; }

    // Primitivas útiles.
    void set_cube_mesh() {
        if (auto* tree = get_tree()) {
            if (auto* r = tree->get_renderer()) {
                mesh_ = r->create_mesh(MeshPrimitives::cube_vertices(),
                                        MeshPrimitives::cube_indices(), 12);
            }
        }
    }
    void set_sphere_mesh(int seg = 24, int rings = 16) {
        if (auto* tree = get_tree()) {
            if (auto* r = tree->get_renderer()) {
                mesh_ = r->create_mesh(MeshPrimitives::sphere_vertices(seg, rings),
                                        MeshPrimitives::sphere_indices(seg, rings), 12);
            }
        }
    }
    void set_quad_mesh() {
        if (auto* tree = get_tree()) {
            if (auto* r = tree->get_renderer()) {
                mesh_ = r->create_mesh(MeshPrimitives::quad_vertices(),
                                        MeshPrimitives::quad_indices(), 12);
            }
        }
    }

    void draw() override {
        if (!mesh_ || !get_tree() || !get_tree()->get_renderer()) return;
        // Cámara: buscar la cámara activa en el árbol (simplificación).
        // En una implementación completa, SceneTree llevaría la referencia.
        (void)get_tree(); /*renderer unused*/
        // Aquí se usarían view/proj de la cámara activa; se omite por brevedad.
        // r.draw_mesh(mesh_, shader_, get_global_transform(), view, proj);
    }

private:
    std::shared_ptr<Mesh>   mesh_;
    std::shared_ptr<Shader> shader_;
    Color                   albedo_ = Color::white;
};

// VoxCamera3D — cámara perspectiva.
class VoxCamera3D : public Vox3D {
public:
    ARX_CLASS(VoxCamera3D, Vox3D);
public:

    void set_fov(float deg)  { fov_ = deg; }
    void set_near(float n)   { near_ = n; }
    void set_far(float f)    { far_  = f; }
    void set_orthographic(bool o) { ortho_ = o; }
    void set_size(float s)   { size_ = s; }   // Para ortho.

    float get_fov() const    { return fov_; }
    float get_near() const   { return near_; }
    float get_far()  const   { return far_; }

    Matrix4 get_view_matrix() const {
        // Posición y orientación de la cámara en world space → view = inverse(world).
        return glm::inverse(glm::translate(Matrix4(1.0f), position_) *
                             glm::mat4_cast(glm::quat(glm::radians(rotation_))));
    }

    Matrix4 get_projection_matrix(float aspect) const {
        if (ortho_) {
            float h = size_ * 0.5f;
            float w = h * aspect;
            return glm::ortho(-w, w, -h, h, near_, far_);
        }
        return glm::perspective(glm::radians(fov_), aspect, near_, far_);
    }

    void make_current();
    bool is_current() const { return current_; }

private:
    float fov_   = 75.0f;
    float near_  = 0.05f;
    float far_   = 1000.0f;
    bool  ortho_ = false;
    float size_  = 5.0f;
    bool  current_ = false;
};

// VoxLight3D — base de luces 3D.
class VoxLight3D : public Vox3D {
public:
    ARX_CLASS(VoxLight3D, Vox3D);
public:

    enum class Type { Directional, Omni, Spot };

    void set_type(Type t)         { type_ = t; }
    Type  get_type() const        { return type_; }
    void  set_color(Color c)      { color_ = c; }
    Color get_color() const       { return color_; }
    void  set_energy(float e)     { energy_ = e; }
    float get_energy() const      { return energy_; }
    void  set_range(float r)      { range_ = r; }
    float get_range() const       { return range_; }

protected:
    Type   type_   = Type::Directional;
    Color  color_  = Color::white;
    float  energy_ = 1.0f;
    float  range_  = 10.0f;
};

class VoxDirectionalLight3D : public VoxLight3D {
public:
    ARX_CLASS(VoxDirectionalLight3D, VoxLight3D);
public:
    VoxDirectionalLight3D() { set_type(Type::Directional); }
};

class VoxOmniLight3D : public VoxLight3D {
public:
    ARX_CLASS(VoxOmniLight3D, VoxLight3D);
public:
    VoxOmniLight3D() { set_type(Type::Omni); }
};

} // namespace arx

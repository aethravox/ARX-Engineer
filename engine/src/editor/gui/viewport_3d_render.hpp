// ==============================================================================
// src/editor/gui/viewport_3d_render.hpp — Render 3D real del viewport.
//
// FASE 13: Renderiza la escena 3D a un FBO (Frame Buffer Object) con OpenGL,
// y devuelve la textura para que el viewport la muestre en el ImDrawList.
//
// Esto permite ver meshes REALES (cubo, esfera, quad) con iluminación,
// en vez de solo iconos SVG planos.
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <glad/glad.h>
#include <imgui.h>  // ImTextureID
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace arx {

class Renderer;
class Mesh;
class Shader;
class Vox;

class Viewport3DRenderer {
public:
    Viewport3DRenderer() = default;
    ~Viewport3DRenderer();

    // Inicializa shader y primitivas. Llamar una sola vez.
    void init(Renderer* renderer);

    // Renderiza la escena 3D al FBO y devuelve el texture ID de OpenGL.
    // El caller (viewport) lo usa como ImTextureID para dibujar la imagen.
    // - root: árbol de Voxes a renderizar
    // - view, proj: matrices de cámara
    // - w, h: tamaño del viewport en píxeles
    // Retorna 0 si falla (FBO no creado).
    ImTextureID render(Vox* root, const glm::mat4& view, const glm::mat4& proj,
                       int w, int h);

    // Libera recursos GPU.
    void shutdown();

private:
    void ensure_fbo_(int w, int h);
    void ensure_primitives_();
    void ensure_shader_();

    Renderer* renderer_ = nullptr;

    // FBO
    GLuint fbo_        = 0;
    GLuint color_tex_  = 0;
    GLuint depth_rb_   = 0;
    int    fbo_w_      = 0;
    int    fbo_h_      = 0;
    
    // FASE 16: Shadow map FBO
    GLuint shadow_fbo_     = 0;
    GLuint shadow_depth_tex_ = 0;
    static const int SHADOW_MAP_SIZE = 1024;
    bool shadow_inited_ = false;
    
    void init_shadow_map_();
    void render_shadow_pass_(Vox* root, const glm::mat4& light_view,
                              const glm::mat4& light_proj);

    // Shader simple (MVP + directional light + albedo)
    std::shared_ptr<Shader> shader_;
    GLint  mvp_loc_    = -1;
    GLint  model_loc_  = -1;
    GLint  view_loc_   = -1;
    GLint  normal_mat_loc_ = -1;
    GLint  albedo_loc_ = -1;
    GLint  light_dir_loc_  = -1;
    GLint  light_color_loc_ = -1;
    GLint  ambient_loc_    = -1;
    GLint  selected_loc_   = -1;

    // Primitivas cacheadas (cubo, esfera, quad)
    std::shared_ptr<Mesh> cube_mesh_;
    std::shared_ptr<Mesh> sphere_mesh_;
    std::shared_ptr<Mesh> quad_mesh_;

    bool inited_ = false;

    // Vox actualmente seleccionado (para highlight en el render)
    static Vox* selected_vox_;
    friend class ViewportPanel;
};

} // namespace arx

// ==============================================================================
// src/editor/gui/viewport_3d_render.cpp — Render 3D real del viewport.
//
// FASE 13: Implementación con FBO + shader + mesh rendering.
// ==============================================================================
#include "viewport_3d_render.hpp"
#include <imgui.h>  // ImTextureID
#include "render/renderer.hpp"
#include "render/texture.hpp"
#include "core/logging.hpp"
#include "core/types.hpp"

#include "scene/vox.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "scene/3d/vox_physics_3d.hpp"
#include "scene/3d/vox_extra_3d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"  // VoxCamera3D también está acá

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace arx {

Viewport3DRenderer::~Viewport3DRenderer() {
    shutdown();
}

void Viewport3DRenderer::init(Renderer* renderer) {
    if (inited_) return;
    renderer_ = renderer;
    ensure_shader_();
    ensure_primitives_();
    inited_ = true;
    ARX_LOG_INFO("Viewport3DRenderer: init OK (shader + primitivas)");
}

void Viewport3DRenderer::shutdown() {
    if (fbo_) glDeleteFramebuffersEXT(1, &fbo_);
    if (color_tex_) glDeleteTextures(1, &color_tex_);
    if (depth_rb_) glDeleteRenderbuffersEXT(1, &depth_rb_);
    fbo_ = 0; color_tex_ = 0; depth_rb_ = 0;
    fbo_w_ = fbo_h_ = 0;
    shader_.reset();
    cube_mesh_.reset();
    sphere_mesh_.reset();
    quad_mesh_.reset();
    inited_ = false;
}

void Viewport3DRenderer::ensure_shader_() {
    if (shader_) return;
    if (!renderer_) return;

    // Shader simple GLSL 1.20 (OpenGL 2.1 compatible).
    // Vertex: transforma con MVP + pasa normal y world pos.
    // Fragment: lambert + ambient + albedo uniform + selection highlight.
    // GLSL 1.20 shader. In GLSL 1.20, attribute locations are auto-assigned
    // in declaration order. MeshGL enables attrib arrays 0,1,2,3 with layout:
    //   location 0 = pos(3 floats), 1 = uv(2), 2 = normal(3), 3 = color(4)
    // So we declare all 4 attributes in the SAME ORDER to match MeshGL.
    static const char* kVS = R"(
#version 120
attribute vec3 a_pos;     // location 0
attribute vec2 a_uv;      // location 1
attribute vec3 a_normal;  // location 2
attribute vec4 a_color;   // location 3
uniform mat4 u_mvp;
uniform mat4 u_model;
varying vec2 v_uv;
varying vec3 v_normal;
varying vec3 v_world_pos;
void main() {
    v_uv = a_uv;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
    v_normal = mat3(u_model) * a_normal;
    v_world_pos = (u_model * vec4(a_pos, 1.0)).xyz;
}
)";
    static const char* kFS = R"(
#version 120
varying vec2 v_uv;
varying vec3 v_normal;
varying vec3 v_world_pos;
uniform sampler2D u_texture;
uniform sampler2D u_shadow_map;
uniform mat4 u_light_vp;
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform vec3 u_light_dir;
uniform vec3 u_light_color;
uniform vec3 u_ambient;
uniform float u_selected;
uniform float u_has_texture;
void main() {
    vec3 albedo = u_albedo;
    if (u_has_texture > 0.5) {
        vec4 tex = texture2D(u_texture, v_uv);
        albedo *= tex.rgb;
    }
    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_light_dir);
    float diff = max(dot(N, L), 0.0);
    
    // FASE 16: Shadow calculation
    // Project fragment position into light space
    vec4 light_space_pos = u_light_vp * vec4(v_world_pos, 1.0);
    vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    proj_coords = proj_coords * 0.5 + 0.5;  // to [0,1]
    
    float shadow = 1.0;  // 1 = lit, 0 = shadowed
    if (proj_coords.x >= 0.0 && proj_coords.x <= 1.0 &&
        proj_coords.y >= 0.0 && proj_coords.y <= 1.0 &&
        proj_coords.z <= 1.0) {
        // Sample shadow map - manual depth comparison
        float shadow_val = texture2D(u_shadow_map, proj_coords.xy).r;
        // Bias to avoid acne (larger bias = fewer false shadows)
        if (proj_coords.z - 0.01 > shadow_val) {
            shadow = 0.2;  // in shadow - much darker
        }
    }
    
    // PBR simplificado: diffuse + ambient + specular
    vec3 color = u_ambient * albedo + diff * u_light_color * albedo * shadow;
    // Specular (Blinn-Phong, modulado por roughness)
    vec3 V = normalize(-v_world_pos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0) * (1.0 - u_roughness);
    color += spec * u_light_color * (1.0 - u_metallic) * shadow;
    // Highlight de selección
    if (u_selected > 0.5) {
        color = mix(color, vec3(0.10, 0.85, 0.93), 0.35);
    }
    gl_FragColor = vec4(color, 1.0);
}
)";
    shader_ = renderer_->create_shader(kVS, kFS);
    if (!shader_) {
        ARX_LOG_ERROR("Viewport3DRenderer: no se pudo crear el shader");
        return;
    }
    ARX_LOG_INFO("Viewport3DRenderer: shader compilado OK");
}

void Viewport3DRenderer::ensure_primitives_() {
    if (cube_mesh_ || !renderer_) return;
    // Cubo: 24 vértices (4 por cara), 36 índices.
    // Cada vértice: pos(3) + normal(3) = stride 6 floats.
    auto cube_v = MeshPrimitives::cube_vertices();
    auto cube_i = MeshPrimitives::cube_indices();
    cube_mesh_ = renderer_->create_mesh(cube_v, cube_i, 12);

    auto sphere_v = MeshPrimitives::sphere_vertices(24, 16);
    auto sphere_i = MeshPrimitives::sphere_indices(24, 16);
    sphere_mesh_ = renderer_->create_mesh(sphere_v, sphere_i, 12);

    auto quad_v = MeshPrimitives::quad_vertices();
    auto quad_i = MeshPrimitives::quad_indices();
    quad_mesh_ = renderer_->create_mesh(quad_v, quad_i, 12);

    ARX_LOG_INFO("Viewport3DRenderer: primitivas creadas (cube, sphere, quad)");
}

// NOTE: Usamos las extensiones EXT (GL_EXT_framebuffer_object) porque glad
// se generó para OpenGL 2.1 sin FBO core. En OpenGL 3+ se pueden usar sin EXT.
void Viewport3DRenderer::ensure_fbo_(int w, int h) {
    if (w == fbo_w_ && h == fbo_h_ && fbo_) return;
    if (fbo_) {
        glDeleteFramebuffersEXT(1, &fbo_);
        glDeleteTextures(1, &color_tex_);
        glDeleteRenderbuffersEXT(1, &depth_rb_);
        fbo_ = color_tex_ = depth_rb_ = 0;
    }
    if (w < 1 || h < 1) return;

    // Color attachment (textura que el viewport dibuja)
    glGenTextures(1, &color_tex_);
    glBindTexture(GL_TEXTURE_2D, color_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Depth buffer (renderbuffer, no necesita sampling)
    glGenRenderbuffersEXT(1, &depth_rb_);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, w, h);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    // FBO
    glGenFramebuffersEXT(1, &fbo_);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex_, 0);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb_);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        ARX_LOG_ERROR("Viewport3DRenderer: FBO incompleto (status=0x{:X})", (unsigned)status);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &fbo_);
        glDeleteTextures(1, &color_tex_);
        glDeleteRenderbuffersEXT(1, &depth_rb_);
        fbo_ = color_tex_ = depth_rb_ = 0;
        return;
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    fbo_w_ = w; fbo_h_ = h;
    ARX_LOG_INFO("Viewport3DRenderer: FBO creado {}x{}", w, h);
}

// Helper local: construir matrix model de un Vox3D
static glm::mat4 build_model_matrix(Vox3D* n) {
    auto pos = n->get_position();
    auto rot = n->get_rotation();  // euler grados
    auto scl = n->get_scale();

    glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
    glm::mat4 r = glm::mat4(1.0f);
    r = glm::rotate(r, glm::radians(rot.x), glm::vec3(1, 0, 0));
    r = glm::rotate(r, glm::radians(rot.y), glm::vec3(0, 1, 0));
    r = glm::rotate(r, glm::radians(rot.z), glm::vec3(0, 0, 1));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(scl.x, scl.y, scl.z));
    return t * r * s;
}

// FASE 16: Initialize shadow map FBO (depth-only)
void Viewport3DRenderer::init_shadow_map_() {
    if (shadow_inited_) return;
    
    // Create depth texture
    glGenTextures(1, &shadow_depth_tex_);
    glBindTexture(GL_TEXTURE_2D, shadow_depth_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Manual comparison in shader (no GL_COMPARE_R_TO_TEXTURE - not reliable on GL 2.1)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Create shadow FBO
    glGenFramebuffersEXT(1, &shadow_fbo_);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadow_fbo_);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                               GL_TEXTURE_2D, shadow_depth_tex_, 0);
    // No color attachment - depth only
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        ARX_LOG_ERROR("Shadow map FBO incomplete (0x{:X})", (unsigned)status);
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    
    shadow_inited_ = true;
    ARX_LOG_INFO("Shadow map FBO created {}x{}", SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
}

// FASE 16: Render scene from light's perspective to shadow map
void Viewport3DRenderer::render_shadow_pass_(Vox* root,
                                               const glm::mat4& light_view,
                                               const glm::mat4& light_proj) {
    if (!shadow_inited_ || !shadow_fbo_) return;
    
    // Save state
    GLint prev_fbo; glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prev_fbo);
    GLint prev_viewport[4]; glGetIntegerv(GL_VIEWPORT, prev_viewport);
    GLboolean prev_depth = glIsEnabled(GL_DEPTH_TEST);
    
    // Bind shadow FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadow_fbo_);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    
    // Clear depth only
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    // Use simple shader (no lighting, just depth)
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);  // Don't write color
    
    // Set matrices for light perspective
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(light_proj));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(light_view));
    
    // Render all meshes (cubes + loaded meshes) to depth
    glm::mat4 vp = light_proj * light_view;
    
    std::function<void(Vox*)> draw = [&](Vox* n) {
        if (!n) return;
        if (n != root) {
            auto* n3d = dynamic_cast<Vox3D*>(n);
            if (n3d) {
                std::string cls = std::string(n3d->get_class_name());
                glm::mat4 model = build_model_matrix(n3d);
                
                if (cls == "VoxMeshInstance3D") {
                    auto* mi = static_cast<VoxMeshInstance3D*>(n3d);
                    if (mi->has_loaded_mesh()) {
                        mi->ensure_vbos();
                        float scale = mi->get_loaded_scale();
                        glm::vec3 center = mi->get_loaded_center();
                        model = glm::scale(model, glm::vec3(scale));
                        model = glm::translate(model, glm::vec3(-center.x, -center.y, -center.z));
                        
                        // Use cached VBOs
                        for (const auto& lm : mi->get_loaded_meshes()) {
                            if (!lm.vbo_created) continue;
                            glBindBuffer(GL_ARRAY_BUFFER, lm.vbo);
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lm.ibo);
                            // Use fixed-function for shadow pass (simpler)
                            glMatrixMode(GL_MODELVIEW);
                            glPushMatrix();
                            glMultMatrixf(glm::value_ptr(model));
                            int stride = 8 * sizeof(float);
                            glEnableClientState(GL_VERTEX_ARRAY);
                            glVertexPointer(3, GL_FLOAT, stride, (void*)0);
                            glDrawElements(GL_TRIANGLES, (GLsizei)lm.indices.size(),
                                GL_UNSIGNED_INT, (void*)0);
                            glDisableClientState(GL_VERTEX_ARRAY);
                            glPopMatrix();
                            glBindBuffer(GL_ARRAY_BUFFER, 0);
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        }
                        goto skip_cube;
                    }
                }
                
                // Draw cube for other types
                if (cls != "VoxCamera3D" && cls != "VoxCollisionShape3D") {
                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glMultMatrixf(glm::value_ptr(model));
                    glBegin(GL_QUADS);
                    // Front
                    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
                    // Back
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
                    glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
                    // Top
                    glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
                    // Bottom
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
                    glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
                    // Right
                    glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
                    // Left
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
                    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
                    glEnd();
                    glPopMatrix();
                }
                skip_cube:;
            }
        }
        for (auto* c : n->get_children()) draw(c);
    };
    draw(root);
    
    // Restore
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // CRITICAL: Reset all GL client state to prevent conflicts with main pass
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)prev_fbo);
    glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);
    if (prev_depth) glEnable(GL_DEPTH_TEST);
}

ImTextureID Viewport3DRenderer::render(Vox* root, const glm::mat4& view,
                                        const glm::mat4& proj, int w, int h) {
    if (!inited_ || !shader_ || !root) return 0;
    if (!cube_mesh_ || !sphere_mesh_) return 0;  // primitivas no listas
    ensure_fbo_(w, h);
    if (!fbo_) return 0;

    // FASE 16: Render shadow map first
    init_shadow_map_();
    // Light direction (same as in shader)
    glm::vec3 shadow_light_dir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
    // Light position = opposite of direction, far away
    glm::vec3 shadow_light_pos = -shadow_light_dir * 30.0f;
    glm::mat4 light_view = glm::lookAt(shadow_light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 light_proj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 80.0f);
    render_shadow_pass_(root, light_view, light_proj);
    
    // Store light VP for main pass
    glm::mat4 light_vp = light_proj * light_view;

    // Guardar estado GL que tocamos
    GLint prev_fbo; glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prev_fbo);
    GLint prev_viewport[4]; glGetIntegerv(GL_VIEWPORT, prev_viewport);
    GLboolean prev_depth = glIsEnabled(GL_DEPTH_TEST);
    GLboolean prev_cull = glIsEnabled(GL_CULL_FACE);
    GLint prev_program; glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);

    // Bind FBO y configurar viewport
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glViewport(0, 0, w, h);

    // Clear
    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);  // dark background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);  // doble cara para no romper con normals

    // Usar shader
    shader_->use();
    shader_->set_uniform("u_view", view);
    shader_->set_uniform("u_proj", proj);

    // Luz direccional (sol)
    glm::vec3 light_dir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
    glm::vec3 light_color = glm::vec3(1.0f, 0.95f, 0.85f);
    glm::vec3 ambient = glm::vec3(0.4f, 0.4f, 0.45f);  // stronger ambient for shadow visibility
    shader_->set_uniform("u_light_dir", Vector3(light_dir.x, light_dir.y, light_dir.z));
    shader_->set_uniform("u_light_color", Vector3(light_color.x, light_color.y, light_color.z));
    shader_->set_uniform("u_ambient", Vector3(ambient.x, ambient.y, ambient.z));

    glm::mat4 vp = proj * view;

    // Use LEGACY OpenGL (glBegin/glEnd) instead of MeshGL VAO.
    // VAO + FBO on Intel 4500 (crocus driver) crashes in memcpy.
    // Immediate mode always works on GL 2.1.
    // First, unbind any VAO that MeshGL might have left bound.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);  // Disable shader - use fixed-function pipeline

    // Recorrer el árbol y dibujar cada Vox3D con cubos legacy
    std::function<void(Vox*, bool)> draw_recursive = [&](Vox* n, bool is_selected) {
        if (!n) return;
        bool this_selected = is_selected || (n == root);

        if (n != root) {
            auto* n3d = dynamic_cast<Vox3D*>(n);
            if (n3d) {
                std::string cls = std::string(n3d->get_class_name());
                glm::mat4 model = build_model_matrix(n3d);

                // Color por tipo
                glm::vec3 color(0.7f, 0.7f, 0.75f);  // default gray
                float size = 0.5f;

                if (cls == "VoxMeshInstance3D") {
                    // FASE 15: dibujar meshes reales con SHADER PBR + VBO
                    auto* mi = static_cast<VoxMeshInstance3D*>(n3d);
                    if (mi->has_loaded_mesh()) {
                        const auto& meshes = mi->get_loaded_meshes();
                        float scale = mi->get_loaded_scale();
                        glm::vec3 center = mi->get_loaded_center();

                        glm::mat4 mesh_model = model;
                        mesh_model = glm::scale(mesh_model, glm::vec3(scale));
                        mesh_model = glm::translate(mesh_model, glm::vec3(-center.x, -center.y, -center.z));

                        glm::mat4 mvp = vp * mesh_model;
                        bool is_sel = (n3d == selected_vox_);

                        // FASE 15B: Ensure VBOs are created once (cached)
                        mi->ensure_vbos();

                        // Usar el shader PBR
                        shader_->use();
                        shader_->set_uniform("u_mvp", mvp);
                        shader_->set_uniform("u_model", mesh_model);
                        shader_->set_uniform("u_selected", is_sel ? 1.0f : 0.0f);
                        shader_->set_uniform("u_metallic", 0.0f);
                        shader_->set_uniform("u_roughness", 0.5f);
                        // FASE 16: Shadow map uniforms
                        shader_->set_uniform("u_light_vp", light_vp);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, shadow_depth_tex_);
                        shader_->set_uniform_texture("u_shadow_map", 1);

                        for (const auto& lm : meshes) {
                            if (!lm.vbo_created) continue;

                            shader_->set_uniform("u_albedo",
                                Vector3(lm.albedo[0], lm.albedo[1], lm.albedo[2]));

                            if (lm.has_texture && lm.texture_id != 0 && !is_sel) {
                                shader_->set_uniform("u_has_texture", 1.0f);
                                glActiveTexture(GL_TEXTURE0);
                                glBindTexture(GL_TEXTURE_2D, lm.texture_id);
                                shader_->set_uniform_texture("u_texture", 0);
                            } else {
                                shader_->set_uniform("u_has_texture", 0.0f);
                                glBindTexture(GL_TEXTURE_2D, 0);
                            }

                            glBindBuffer(GL_ARRAY_BUFFER, lm.vbo);
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lm.ibo);

                            int stride = 8 * sizeof(float);
                            glEnableVertexAttribArray(0);
                            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
                            glEnableVertexAttribArray(2);
                            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
                            glEnableVertexAttribArray(1);
                            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

                            glDrawElements(GL_TRIANGLES, (GLsizei)lm.indices.size(),
                                GL_UNSIGNED_INT, (void*)0);

                            glDisableVertexAttribArray(0);
                            glDisableVertexAttribArray(1);
                            glDisableVertexAttribArray(2);
                            glBindBuffer(GL_ARRAY_BUFFER, 0);
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, 0);
                            glActiveTexture(GL_TEXTURE1);
                            glBindTexture(GL_TEXTURE_2D, 0);
                            glActiveTexture(GL_TEXTURE0);
                        }

                        glUseProgram(0);
                        goto next_vox;
                    }
                    // Sin mesh: cubo placeholder
                    color = glm::vec3(0.4f, 0.6f, 1.0f);
                    size = 0.5f;
                } else if (cls.find("Body") != std::string::npos || cls == "VoxArea3D") {
                    color = glm::vec3(0.85f, 0.35f, 0.35f);
                    auto* body = dynamic_cast<PhysicsBody3D*>(n3d);
                    if (body) {
                        auto he = body->half_extents_;
                        model = glm::scale(model, glm::vec3(he.x * 2, he.y * 2, he.z * 2));
                    } else {
                        model = glm::scale(model, glm::vec3(size * 2));
                    }
                } else if (cls.find("Light") != std::string::npos) {
                    color = glm::vec3(1.0f, 0.9f, 0.4f);
                    model = glm::scale(model, glm::vec3(0.3f));
                } else if (cls == "Vox3D") {
                    color = glm::vec3(0.45f, 0.30f, 0.85f);
                    model = glm::scale(model, glm::vec3(0.5f));
                } else if (cls == "VoxCamera3D" || cls == "VoxCollisionShape3D") {
                    // Skip
                } else {
                    color = glm::vec3(0.7f, 0.7f, 0.75f);
                }

                if (cls != "VoxCamera3D" && cls != "VoxCollisionShape3D") {
                    if (n3d == selected_vox_) {
                        color = glm::vec3(0.10f, 0.85f, 0.93f);
                    }

                    glm::mat4 mvp = vp * model;

                    glMatrixMode(GL_PROJECTION);
                    glPushMatrix();
                    glLoadMatrixf(glm::value_ptr(proj));
                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadMatrixf(glm::value_ptr(view * model));

                    glColor3f(color.r, color.g, color.b);

                    glBegin(GL_QUADS);
                    // Front
                    glNormal3f(0, 0, 1);
                    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
                    // Back
                    glNormal3f(0, 0, -1);
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
                    glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
                    // Top
                    glNormal3f(0, 1, 0);
                    glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
                    // Bottom
                    glNormal3f(0, -1, 0);
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
                    glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
                    // Right
                    glNormal3f(1, 0, 0);
                    glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
                    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
                    // Left
                    glNormal3f(-1, 0, 0);
                    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
                    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
                    glEnd();

                    glPopMatrix();
                    glMatrixMode(GL_PROJECTION);
                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);
                }
            }
        }
        next_vox:;
        for (auto* c : n->get_children()) draw_recursive(c, this_selected);
    };

    // Enable lighting for the cubes
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat legacy_light_pos[] = {-0.5f, -1.0f, -0.3f, 0.0f};
    GLfloat legacy_light_color[] = {1.0f, 0.95f, 0.85f, 1.0f};
    GLfloat legacy_ambient[] = {0.25f, 0.25f, 0.30f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, legacy_light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, legacy_light_color);
    glLightfv(GL_LIGHT0, GL_AMBIENT, legacy_ambient);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    draw_recursive(root, false);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);

    // Restaurar estado GL
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)prev_fbo);
    glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);
    if (prev_depth) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (prev_cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    glUseProgram(prev_program);


    return (ImTextureID)(uintptr_t)color_tex_;
}

Vox* Viewport3DRenderer::selected_vox_ = nullptr;

} // namespace arx

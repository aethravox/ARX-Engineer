// ==============================================================================
// src/render/renderer.hpp — Renderer abstracto (renderer_3d + renderer_2d).
// Una sola API para OpenGL desktop, GLES3 (web/android) y futuro Vulkan.
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

namespace arx {

class Texture;
class Mesh;
class Shader;

// RendererConfig: parámetros del renderer al inicio.
struct RendererConfig {
    int   viewport_w = 1280;
    int   viewport_h = 720;
    bool  vsync      = true;
    int   msaa_samples = 0;
    Color clear_color = Color::arx_purple * 0.2f;
};

// Estadísticas del renderer (se consultan después de end_frame).
struct RendererStats {
    int     draw_calls = 0;
    int     vertices   = 0;
    int     triangles  = 0;
    int     textures_bound = 0;
    int     shader_swaps   = 0;
    float   frame_ms   = 0.0f;
};

// Renderer: interfaz que se implementa por backend (GL, GLES, Vulkan, dummy).
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool init(const RendererConfig& cfg) = 0;
    virtual void shutdown() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    virtual void set_viewport(int x, int y, int w, int h) = 0;
    virtual void set_clear_color(const Color& c) = 0;
    virtual void clear(bool color = true, bool depth = true) = 0;

    // 2D API (renderer_2d).
    virtual void draw_quad_2d(Vector2 pos, Vector2 size, Color color,
                              std::shared_ptr<Texture> tex = nullptr) = 0;
    virtual void draw_sprite_2d(std::shared_ptr<Texture> tex,
                                Vector2 pos, Vector2 size,
                                float rotation = 0.0f,
                                Color modulate = Color::white,
                                const Rect2* region = nullptr) = 0;
    virtual void draw_line_2d(Vector2 a, Vector2 b, Color color, float width = 1.0f) = 0;
    virtual void draw_rect_2d(const Rect2& r, Color color, bool filled = false,
                              float width = 1.0f) = 0;
    virtual void draw_text_2d(const std::string& text, Vector2 pos,
                              Color color, int size = 16) = 0;
    virtual void set_camera_2d(const Transform2D& cam) = 0;

    // 3D API (renderer_3d).
    virtual void draw_mesh(std::shared_ptr<Mesh> mesh,
                           std::shared_ptr<Shader> shader,
                           const Transform3D& transform,
                           const Matrix4& view,
                           const Matrix4& proj) = 0;
    virtual void set_camera_3d(const Matrix4& view, const Matrix4& proj) = 0;
    virtual void set_ambient_light(Color color, float energy = 1.0f) = 0;
    virtual void add_directional_light(Vector3 dir, Color color, float energy = 1.0f) = 0;
    virtual void add_point_light(Vector3 pos, Color color, float energy = 1.0f,
                                 float range = 10.0f) = 0;

    // Recursos.
    virtual std::shared_ptr<Texture> create_texture(int w, int h, int channels,
                                                     const uint8_t* data) = 0;
    virtual std::shared_ptr<Mesh>    create_mesh(const std::vector<float>& vertices,
                                                  const std::vector<uint32_t>& indices,
                                                  int stride = 3) = 0;
    virtual std::shared_ptr<Shader>  create_shader(const std::string& vertex_src,
                                                    const std::string& fragment_src) = 0;

    virtual const RendererStats& get_stats() const = 0;

    // Factory.
    enum class Backend { OpenGL, OpenGLES, Vulkan, Dummy };
    static std::unique_ptr<Renderer> create(Backend backend = Backend::OpenGL);
};

} // namespace arx

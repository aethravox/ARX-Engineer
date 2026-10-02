// ==============================================================================
// src/render/opengl/renderer_gl.hpp — Renderer OpenGL 3.3+ / GLES 3.0.
// ==============================================================================
#pragma once

#include "render/renderer.hpp"
#include "render/texture.hpp"

#include <glad/glad.h>

#include <unordered_map>
#include <memory>

namespace arx {

class TextureGL;
class MeshGL;
class ShaderGL;
class FontAtlas;

class RendererGL : public Renderer {
public:
    RendererGL() = default;
    ~RendererGL() override;

    bool init(const RendererConfig& cfg) override;
    void shutdown() override;

    void begin_frame() override;
    void end_frame() override;

    void set_viewport(int x, int y, int w, int h) override;
    void set_clear_color(const Color& c) override;
    void clear(bool color, bool depth) override;

    // 2D
    void draw_quad_2d(Vector2 pos, Vector2 size, Color color,
                      std::shared_ptr<Texture> tex) override;
    void draw_sprite_2d(std::shared_ptr<Texture> tex,
                        Vector2 pos, Vector2 size,
                        float rotation, Color modulate,
                        const Rect2* region) override;
    void draw_line_2d(Vector2 a, Vector2 b, Color color, float width) override;
    void draw_rect_2d(const Rect2& r, Color color, bool filled, float width) override;
    void draw_text_2d(const std::string& text, Vector2 pos,
                      Color color, int size) override;
    void set_camera_2d(const Transform2D& cam) override;

    // 3D
    void draw_mesh(std::shared_ptr<Mesh> mesh,
                   std::shared_ptr<Shader> shader,
                   const Transform3D& transform,
                   const Matrix4& view,
                   const Matrix4& proj) override;
    void set_camera_3d(const Matrix4& view, const Matrix4& proj) override;
    void set_ambient_light(Color color, float energy) override;
    void add_directional_light(Vector3 dir, Color color, float energy) override;
    void add_point_light(Vector3 pos, Color color, float energy, float range) override;

    // Recursos
    std::shared_ptr<Texture> create_texture(int w, int h, int channels,
                                             const uint8_t* data) override;
    std::shared_ptr<Mesh> create_mesh(const std::vector<float>& vertices,
                                       const std::vector<uint32_t>& indices,
                                       int stride) override;
    std::shared_ptr<Shader> create_shader(const std::string& vertex,
                                           const std::string& fragment) override;

    const RendererStats& get_stats() const override { return stats_; }

private:
    void flush_batch_2d();
    void setup_default_shaders();

    RendererConfig cfg_;
    RendererStats  stats_;

    // Default shaders (built-in).
    std::shared_ptr<Shader> shader_2d_;
    std::shared_ptr<Shader> shader_3d_;
    std::shared_ptr<Shader> shader_lines_;

    // Quad 2D batch (sprite batcher).
    public:
    public:
public:
    struct Batch2D {
        std::shared_ptr<Texture> texture;
        std::vector<float>       vertices;   // pos(2), uv(2), color(4)
        std::vector<uint32_t>    indices;
        uint32_t                 vao = 0, vbo = 0, ibo = 0;
        size_t                   max_quads = 4096;
    } batch_2d_;

    // Lines batch.
    struct BatchLines {
        std::vector<float>    vertices;   // pos(2), color(4)
        uint32_t              vao = 0, vbo = 0;
        size_t                max_verts = 8192;
    } batch_lines_;

    // Estado de cámara 2D/3D.
    Transform2D camera_2d_;
    Matrix4     view_3d_ = Matrix4(1.0f);
    Matrix4     proj_3d_ = Matrix4(1.0f);

    // Lights (acumuladas en el frame).
    struct DirLight { Vector3 dir; Color color; float energy; };
    struct PointLight { Vector3 pos; Color color; float energy; float range; };
    std::vector<DirLight>   dir_lights_;
    std::vector<PointLight> point_lights_;
    Color  ambient_color_   = Color::white;
    float  ambient_energy_  = 0.3f;

    // Font atlas (FreeType + stb_rect_pack).
    std::unique_ptr<FontAtlas> font_atlas_;
};

} // namespace arx

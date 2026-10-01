// ==============================================================================
// src/render/opengl/renderer_gl.cpp — Implementación OpenGL del renderer.
// Maneja batching 2D, meshes 3D, shaders y texturas.
// ==============================================================================
#include "renderer_gl.hpp"
#include "render/font/font_atlas.hpp"
#include "core/logging.hpp"
#include "core/math.hpp"

#include <glad/glad.h>

// stb_image: solo declaraciones. La implementación está en stb_image_impl.cpp.
#include "stb_image.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

namespace arx {

// ===================== TextureGL / MeshGL / ShaderGL =========================
class TextureGL : public Texture {
public:
    ~TextureGL() override { if (id_) glDeleteTextures(1, &id_); }
    void bind(unsigned int slot = 0) const override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id_);
    }
    int  width()  const override { return w_; }
    int  height() const override { return h_; }
    int  channels() const override { return c_; }
    uint32_t id() const override { return id_; }

    bool create(int w, int h, int c, const uint8_t* data) {
        w_ = w; h_ = h; c_ = c;
        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);
        GLenum fmt = (c == 4) ? GL_RGBA : (c == 3 ? GL_RGB : GL_RED);
        // OpenGL 2.1: usar GL_GENERATE_MIPMAP (de GL_SGIS_generate_mipmap, core en 2.1)
        // en lugar de glGenerateMipmap (que es de OpenGL 3.0+).
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        return true;
    }
private:
    uint32_t id_ = 0;
    int w_ = 0, h_ = 0, c_ = 0;
};

class MeshGL : public Mesh {
public:
    ~MeshGL() override {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ibo_) glDeleteBuffers(1, &ibo_);
    }
    void bind() const override { glBindVertexArray(vao_); }
    void draw() const override {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
    }
    int vertex_count() const override { return vertex_count_; }
    int index_count()   const override { return index_count_; }

    bool create(const std::vector<float>& vertices,
                const std::vector<uint32_t>& indices, int stride) {
        vertex_count_ = static_cast<int>(vertices.size() / stride);
        index_count_  = static_cast<int>(indices.size());

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ibo_);

        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                     indices.data(), GL_STATIC_DRAW);

        // Layout: pos(3) uv(2) normal(3) color(4) - stride = 12 floats
        if (stride >= 3) {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
        }
        if (stride >= 5) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                                  (void*)(3 * sizeof(float)));
        }
        if (stride >= 8) {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                                  (void*)(5 * sizeof(float)));
        }
        if (stride >= 12) {
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                                  (void*)(8 * sizeof(float)));
        }

        glBindVertexArray(0);
        return true;
    }
private:
    uint32_t vao_ = 0, vbo_ = 0, ibo_ = 0;
    int vertex_count_ = 0, index_count_ = 0;
};

class ShaderGL : public Shader {
public:
    ~ShaderGL() override { if (program_) glDeleteProgram(program_); }

    bool compile(const std::string& vertex_src, const std::string& fragment_src) {
        GLuint vs = compile_stage(vertex_src,   GL_VERTEX_SHADER);
        GLuint fs = compile_stage(fragment_src, GL_FRAGMENT_SHADER);
        if (!vs || !fs) return false;

        program_ = glCreateProgram();
        glAttachShader(program_, vs);
        glAttachShader(program_, fs);
        glLinkProgram(program_);
        GLint ok = 0;
        glGetProgramiv(program_, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[4096];
            glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
            ARX_LOG_ERROR("Shader link error:\n{}", log);
            return false;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        return true;
    }

    void use() const override { glUseProgram(program_); }

    GLint loc(const std::string& name) const {
        auto it = cache_.find(name);
        if (it != cache_.end()) return it->second;
        GLint l = glGetUniformLocation(program_, name.c_str());
        cache_[name] = l;
        return l;
    }

    void set_uniform(const std::string& n, int v) override    { glUniform1i(loc(n), v); }
    void set_uniform(const std::string& n, float v) override  { glUniform1f(loc(n), v); }
    void set_uniform(const std::string& n, const Vector2& v) override { glUniform2f(loc(n), v.x, v.y); }
    void set_uniform(const std::string& n, const Vector3& v) override { glUniform3f(loc(n), v.x, v.y, v.z); }
    void set_uniform(const std::string& n, const Vector4& v) override { glUniform4f(loc(n), v.x, v.y, v.z, v.w); }
    void set_uniform(const std::string& n, const Matrix4& v) override {
        glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(v));
    }
    void set_uniform(const std::string& n, const Color& v) override {
        glUniform4f(loc(n), v.r, v.g, v.b, v.a);
    }
    void set_uniform_texture(const std::string& n, int slot) override {
        glUniform1i(loc(n), slot);
    }

    uint32_t program() const { return program_; }

private:
    GLuint compile_stage(const std::string& src, GLenum type) {
        GLuint sh = glCreateShader(type);
        const char* cstr = src.c_str();
        glShaderSource(sh, 1, &cstr, nullptr);
        glCompileShader(sh);
        GLint ok = 0;
        glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[4096];
            glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
            ARX_LOG_ERROR("Shader compile error ({}):\n{}",
                          type == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
            ARX_LOG_ERROR("Source:\n{}", src);
            glDeleteShader(sh);
            return 0;
        }
        return sh;
    }

    uint32_t program_ = 0;
    mutable std::unordered_map<std::string, GLint> cache_;
};

// ===================== RendererGL ============================================
RendererGL::~RendererGL() { shutdown(); }

bool RendererGL::init(const RendererConfig& cfg) {
    cfg_ = cfg;
    set_viewport(0, 0, cfg_.viewport_w, cfg_.viewport_h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Generar VAO/VBO/IBO del batch 2D.
    glGenVertexArrays(1, &batch_2d_.vao);
    glGenBuffers(1, &batch_2d_.vbo);
    glGenBuffers(1, &batch_2d_.ibo);

    glBindVertexArray(batch_2d_.vao);
    glBindBuffer(GL_ARRAY_BUFFER, batch_2d_.vbo);
    glBufferData(GL_ARRAY_BUFFER, batch_2d_.max_quads * 8 * sizeof(float),
                 nullptr, GL_DYNAMIC_DRAW);

    // Layout 2D: pos(2), uv(2), color(4)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(4 * sizeof(float)));
    glBindVertexArray(0);

    // Lines batch.
    glGenVertexArrays(1, &batch_lines_.vao);
    glGenBuffers(1, &batch_lines_.vbo);
    glBindVertexArray(batch_lines_.vao);
    glBindBuffer(GL_ARRAY_BUFFER, batch_lines_.vbo);
    glBufferData(GL_ARRAY_BUFFER, batch_lines_.max_verts * 6 * sizeof(float),
                 nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    setup_default_shaders();

    ARX_LOG_INFO("RendererGL inicializado (OpenGL {}).", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    return true;
}

void RendererGL::shutdown() {
    if (batch_2d_.vao) glDeleteVertexArrays(1, &batch_2d_.vao);
    if (batch_2d_.vbo) glDeleteBuffers(1, &batch_2d_.vbo);
    if (batch_2d_.ibo) glDeleteBuffers(1, &batch_2d_.ibo);
    if (batch_lines_.vao) glDeleteVertexArrays(1, &batch_lines_.vao);
    if (batch_lines_.vbo) glDeleteBuffers(1, &batch_lines_.vbo);
    shader_2d_.reset();
    shader_3d_.reset();
    shader_lines_.reset();
}

void RendererGL::begin_frame() {
    stats_ = {};
    dir_lights_.clear();
    point_lights_.clear();
    set_ambient_light(Color::white, 0.3f);

    glViewport(0, 0, cfg_.viewport_w, cfg_.viewport_h);
    clear(true, true);
}

void RendererGL::end_frame() {
    flush_batch_2d();
}

void RendererGL::set_viewport(int x, int y, int w, int h) {
    cfg_.viewport_w = w; cfg_.viewport_h = h;
    glViewport(x, y, w, h);
}

void RendererGL::set_clear_color(const Color& c) {
    glClearColor(c.r, c.g, c.b, c.a);
}

void RendererGL::clear(bool color, bool depth) {
    GLbitfield bits = 0;
    if (color) bits |= GL_COLOR_BUFFER_BIT;
    if (depth) bits |= GL_DEPTH_BUFFER_BIT;
    glClear(bits);
}

// ===================== 2D API ================================================
static void push_quad_2d(RendererGL::Batch2D& b, Vector2 p, Vector2 s,
                          Color col, const Rect2* region,
                          std::shared_ptr<Texture> tex) {
    if (b.texture.get() != tex.get()) {
        // Cambio de textura: flush.
        // (Manejado por el renderer via flush_batch_2d en end_frame.)
        b.texture = tex;
    }

    float u0 = 0, v0 = 0, u1 = 1, v1 = 1;
    if (region && tex) {
        u0 = region->position.x / tex->width();
        v0 = region->position.y / tex->height();
        u1 = (region->position.x + region->size.x) / tex->width();
        v1 = (region->position.y + region->size.y) / tex->height();
    }

    float x0 = p.x,       y0 = p.y;
    float x1 = p.x + s.x, y1 = p.y + s.y;

    float verts[] = {
        x0, y0,  u0, v0,  col.r, col.g, col.b, col.a,
        x1, y0,  u1, v0,  col.r, col.g, col.b, col.a,
        x1, y1,  u1, v1,  col.r, col.g, col.b, col.a,
        x0, y1,  u0, v1,  col.r, col.g, col.b, col.a,
    };
    for (float v : verts) b.vertices.push_back(v);

    uint32_t base = static_cast<uint32_t>(b.vertices.size() / 8) - 4;
    b.indices.insert(b.indices.end(), {
        base+0, base+1, base+2, base+0, base+2, base+3
    });
}

void RendererGL::draw_quad_2d(Vector2 pos, Vector2 size, Color color,
                               std::shared_ptr<Texture> tex) {
    push_quad_2d(batch_2d_, pos, size, color, nullptr, tex);
}

void RendererGL::draw_sprite_2d(std::shared_ptr<Texture> tex, Vector2 pos,
                                 Vector2 size, float rotation, Color modulate,
                                 const Rect2* region) {
    // Simplificado: sin rotación por ahora.
    // TODO: aplicar rotación con transform de los 4 vértices.
    (void)rotation;
    push_quad_2d(batch_2d_, pos, size, modulate, region, tex);
}

void RendererGL::draw_line_2d(Vector2 a, Vector2 b, Color color, float width) {
    (void)width;
    float v[] = { a.x, a.y, color.r, color.g, color.b, color.a,
                  b.x, b.y, color.r, color.g, color.b, color.a };
    for (float f : v) batch_lines_.vertices.push_back(f);
}

void RendererGL::draw_rect_2d(const Rect2& r, Color color, bool filled,
                                float width) {
    if (filled) {
        push_quad_2d(batch_2d_, r.position, r.size, color, nullptr, nullptr);
    } else {
        Vector2 p = r.position, s = r.size;
        draw_line_2d(p,             {p.x+s.x, p.y},     color, width);
        draw_line_2d({p.x+s.x, p.y},{p.x+s.x, p.y+s.y},color, width);
        draw_line_2d({p.x+s.x, p.y+s.y},{p.x, p.y+s.y},color, width);
        draw_line_2d({p.x, p.y+s.y}, p,                color, width);
    }
}

void RendererGL::draw_text_2d(const std::string& text, Vector2 pos,
                                Color color, int size) {
    (void)text; (void)pos; (void)color; (void)size;
    // TODO: implementar con FontAtlas (FreeType + stb_rect_pack).
}

void RendererGL::set_camera_2d(const Transform2D& cam) {
    camera_2d_ = cam;
}

void RendererGL::flush_batch_2d() {
    if (!batch_2d_.vertices.empty()) {
        shader_2d_->use();

        // MVP: ortho * camera_inverse
        float hw = cfg_.viewport_w * 0.5f;
        float hh = cfg_.viewport_h * 0.5f;
        Matrix4 proj = glm::ortho(-hw, hw, hh, -hh, -1.0f, 1.0f);
        Matrix4 view = camera_2d_.inverse().to_matrix4();
        Matrix4 mvp  = proj * view;
        shader_2d_->set_uniform("u_mvp", mvp);

        if (batch_2d_.texture) {
            batch_2d_.texture->bind(0);
            shader_2d_->set_uniform_texture("u_texture", 0);
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glBindVertexArray(batch_2d_.vao);
        glBindBuffer(GL_ARRAY_BUFFER, batch_2d_.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        batch_2d_.vertices.size() * sizeof(float),
                        batch_2d_.vertices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch_2d_.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     batch_2d_.indices.size() * sizeof(uint32_t),
                     batch_2d_.indices.data(), GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(batch_2d_.indices.size()),
                       GL_UNSIGNED_INT, nullptr);
        stats_.draw_calls++;
        stats_.vertices   += static_cast<int>(batch_2d_.vertices.size() / 8);
        stats_.triangles  += static_cast<int>(batch_2d_.indices.size() / 3);

        batch_2d_.vertices.clear();
        batch_2d_.indices.clear();
    }

    if (!batch_lines_.vertices.empty()) {
        shader_lines_->use();
        float hw = cfg_.viewport_w * 0.5f;
        float hh = cfg_.viewport_h * 0.5f;
        Matrix4 proj = glm::ortho(-hw, hw, hh, -hh, -1.0f, 1.0f);
        Matrix4 view = camera_2d_.inverse().to_matrix4();
        shader_lines_->set_uniform("u_mvp", proj * view);

        glBindVertexArray(batch_lines_.vao);
        glBindBuffer(GL_ARRAY_BUFFER, batch_lines_.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        batch_lines_.vertices.size() * sizeof(float),
                        batch_lines_.vertices.data());
        glDrawArrays(GL_LINES, 0,
                     static_cast<GLsizei>(batch_lines_.vertices.size() / 6));
        stats_.draw_calls++;
        batch_lines_.vertices.clear();
    }
}

// ===================== 3D API ================================================
void RendererGL::draw_mesh(std::shared_ptr<Mesh> mesh,
                            std::shared_ptr<Shader> shader,
                            const Transform3D& transform,
                            const Matrix4& view,
                            const Matrix4& proj) {
    flush_batch_2d();

    if (!shader) shader = shader_3d_;
    shader->use();
    shader->set_uniform("u_model",      transform.to_matrix4());
    shader->set_uniform("u_view",       view);
    shader->set_uniform("u_proj",       proj);
    shader->set_uniform("u_view_pos",   Vector3(glm::inverse(view)[3]));
    shader->set_uniform("u_ambient_color",   ambient_color_);
    shader->set_uniform("u_ambient_energy",  ambient_energy_);
    shader->set_uniform("u_dir_light_count",   static_cast<int>(dir_lights_.size()));
    shader->set_uniform("u_point_light_count", static_cast<int>(point_lights_.size()));
    // (Subir arrays de luces: se omite por brevedad; en producción se subiría
    //  con glUniform3fv / glUniform4fv arrays.)

    mesh->bind();
    mesh->draw();
    stats_.draw_calls++;
    stats_.vertices  += mesh->vertex_count();
    stats_.triangles += mesh->index_count() / 3;
}

void RendererGL::set_camera_3d(const Matrix4& view, const Matrix4& proj) {
    view_3d_ = view; proj_3d_ = proj;
}

void RendererGL::set_ambient_light(Color color, float energy) {
    ambient_color_ = color; ambient_energy_ = energy;
}

void RendererGL::add_directional_light(Vector3 dir, Color color, float energy) {
    dir_lights_.push_back({dir, color, energy});
}

void RendererGL::add_point_light(Vector3 pos, Color color, float energy, float range) {
    point_lights_.push_back({pos, color, energy, range});
}

// ===================== Recursos ==============================================
std::shared_ptr<Texture> RendererGL::create_texture(int w, int h, int channels,
                                                     const uint8_t* data) {
    auto t = std::make_shared<TextureGL>();
    if (t->create(w, h, channels, data)) return t;
    return nullptr;
}

std::shared_ptr<Mesh> RendererGL::create_mesh(const std::vector<float>& vertices,
                                               const std::vector<uint32_t>& indices,
                                               int stride) {
    auto m = std::make_shared<MeshGL>();
    if (m->create(vertices, indices, stride)) return m;
    return nullptr;
}

std::shared_ptr<Shader> RendererGL::create_shader(const std::string& vs,
                                                    const std::string& fs) {
    auto s = std::make_shared<ShaderGL>();
    if (s->compile(vs, fs)) return s;
    return nullptr;
}

// ===================== Default shaders =======================================
void RendererGL::setup_default_shaders() {
    static const char* vs2d = R"(
        #version 120
        attribute vec2 a_pos;
        attribute vec2 a_uv;
        attribute vec4 a_color;
        varying vec2 v_uv;
        varying vec4 v_color;
        uniform mat4 u_mvp;
        void main() {
            v_uv = a_uv;
            v_color = a_color;
            gl_Position = u_mvp * vec4(a_pos, 0.0, 1.0);
        }
    )";
    static const char* fs2d = R"(
        #version 120
        varying vec2 v_uv;
        varying vec4 v_color;
        uniform sampler2D u_texture;
        void main() {
            vec4 tex = texture2D(u_texture, v_uv);
            gl_FragColor = v_color * tex;
        }
    )";
    shader_2d_ = create_shader(vs2d, fs2d);

    static const char* vslines = R"(
        #version 120
        attribute vec2 a_pos;
        attribute vec4 a_color;
        varying vec4 v_color;
        uniform mat4 u_mvp;
        void main() {
            v_color = a_color;
            gl_Position = u_mvp * vec4(a_pos, 0.0, 1.0);
        }
    )";
    static const char* fslines = R"(
        #version 120
        varying vec4 v_color;
        void main() { gl_FragColor = v_color; }
    )";
    shader_lines_ = create_shader(vslines, fslines);

    static const char* vs3d = R"(
        #version 120
        attribute vec3 a_pos;
        attribute vec2 a_uv;
        attribute vec3 a_normal;
        attribute vec4 a_color;
        varying vec3 v_world_pos;
        varying vec3 v_normal;
        varying vec2 v_uv;
        varying vec4 v_color;
        uniform mat4 u_model, u_view, u_proj;
        void main() {
            vec4 wp = u_model * vec4(a_pos, 1.0);
            v_world_pos = wp.xyz;
            v_normal    = mat3(u_model) * a_normal;
            v_uv        = a_uv;
            v_color     = a_color;
            gl_Position = u_proj * u_view * wp;
        }
    )";
    static const char* fs3d = R"(
        #version 120
        varying vec3 v_world_pos;
        varying vec3 v_normal;
        varying vec2 v_uv;
        varying vec4 v_color;
        uniform vec3  u_view_pos;
        uniform vec4  u_ambient_color;
        uniform float u_ambient_energy;
        void main() {
            vec3 N = normalize(v_normal);
            vec3 L = normalize(vec3(0.5, 0.8, 0.3));
            float diff = max(dot(N, L), 0.0);
            vec3 ambient = u_ambient_color.rgb * u_ambient_energy;
            vec3 col = v_color.rgb * (ambient + diff * 0.7);
            gl_FragColor = vec4(col, v_color.a);
        }
    )";
    shader_3d_ = create_shader(vs3d, fs3d);
}

// ===================== Factory ===============================================
std::unique_ptr<Renderer> Renderer::create(Backend backend) {
    switch (backend) {
        case Backend::OpenGL:
        case Backend::OpenGLES:
            return std::make_unique<RendererGL>();
        case Backend::Vulkan:
            ARX_LOG_ERROR("Vulkan backend aún no implementado, fallback OpenGL");
            return std::make_unique<RendererGL>();
        case Backend::Dummy:
            return nullptr;
    }
    return std::make_unique<RendererGL>();
}

// ===================== Mesh primitives =======================================
std::vector<float> MeshPrimitives::cube_vertices() {
    // pos(3) uv(2) normal(3) color(4) — 12 floats per vertex, 24 vertices
    return {
        // Front
        -0.5f,-0.5f, 0.5f,  0,0,  0,0,1,  1,1,1,1,
         0.5f,-0.5f, 0.5f,  1,0,  0,0,1,  1,1,1,1,
         0.5f, 0.5f, 0.5f,  1,1,  0,0,1,  1,1,1,1,
        -0.5f, 0.5f, 0.5f,  0,1,  0,0,1,  1,1,1,1,
        // Back
        -0.5f,-0.5f,-0.5f,  0,0,  0,0,-1, 1,1,1,1,
         0.5f,-0.5f,-0.5f,  1,0,  0,0,-1, 1,1,1,1,
         0.5f, 0.5f,-0.5f,  1,1,  0,0,-1, 1,1,1,1,
        -0.5f, 0.5f,-0.5f,  0,1,  0,0,-1, 1,1,1,1,
        // Top
        -0.5f, 0.5f, 0.5f,  0,0,  0,1,0,  1,1,1,1,
         0.5f, 0.5f, 0.5f,  1,0,  0,1,0,  1,1,1,1,
         0.5f, 0.5f,-0.5f,  1,1,  0,1,0,  1,1,1,1,
        -0.5f, 0.5f,-0.5f,  0,1,  0,1,0,  1,1,1,1,
        // Bottom
        -0.5f,-0.5f, 0.5f,  0,0,  0,-1,0, 1,1,1,1,
         0.5f,-0.5f, 0.5f,  1,0,  0,-1,0, 1,1,1,1,
         0.5f,-0.5f,-0.5f,  1,1,  0,-1,0, 1,1,1,1,
        -0.5f,-0.5f,-0.5f,  0,1,  0,-1,0, 1,1,1,1,
        // Right
         0.5f,-0.5f, 0.5f,  0,0,  1,0,0,  1,1,1,1,
         0.5f,-0.5f,-0.5f,  1,0,  1,0,0,  1,1,1,1,
         0.5f, 0.5f,-0.5f,  1,1,  1,0,0,  1,1,1,1,
         0.5f, 0.5f, 0.5f,  0,1,  1,0,0,  1,1,1,1,
        // Left
        -0.5f,-0.5f, 0.5f,  0,0, -1,0,0,  1,1,1,1,
        -0.5f,-0.5f,-0.5f,  1,0, -1,0,0,  1,1,1,1,
        -0.5f, 0.5f,-0.5f,  1,1, -1,0,0,  1,1,1,1,
        -0.5f, 0.5f, 0.5f,  0,1, -1,0,0,  1,1,1,1,
    };
}

std::vector<uint32_t> MeshPrimitives::cube_indices() {
    return {
         0, 1, 2,  0, 2, 3,   // Front
         4, 5, 6,  4, 6, 7,   // Back
         8, 9,10,  8,10,11,   // Top
        12,13,14, 12,14,15,   // Bottom
        16,17,18, 16,18,19,   // Right
        20,21,22, 20,22,23,   // Left
    };
}

std::vector<float> MeshPrimitives::quad_vertices() {
    return {
        -0.5f,-0.5f, 0,0, 0,0,1, 1,1,1,1,
         0.5f,-0.5f, 1,0, 0,0,1, 1,1,1,1,
         0.5f, 0.5f, 1,1, 0,0,1, 1,1,1,1,
        -0.5f, 0.5f, 0,1, 0,0,1, 1,1,1,1,
    };
}

std::vector<uint32_t> MeshPrimitives::quad_indices() {
    return {0,1,2, 0,2,3};
}

std::vector<float> MeshPrimitives::sphere_vertices(int segments, int rings) {
    std::vector<float> v;
    for (int r = 0; r <= rings; ++r) {
        float theta = 3.14159265358979f * float(r) / rings;
        float sin_t = std::sin(theta), cos_t = std::cos(theta);
        for (int s = 0; s <= segments; ++s) {
            float phi = 6.28318530718f * float(s) / segments;
            float sin_p = std::sin(phi), cos_p = std::cos(phi);
            float x = sin_t * cos_p, y = cos_t, z = sin_t * sin_p;
            v.insert(v.end(), {
                x*0.5f, y*0.5f, z*0.5f,        // pos
                float(s)/segments, float(r)/rings, // uv
                x, y, z,                        // normal
                1, 1, 1, 1                      // color
            });
        }
    }
    return v;
}

std::vector<uint32_t> MeshPrimitives::sphere_indices(int segments, int rings) {
    std::vector<uint32_t> idx;
    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < segments; ++s) {
            uint32_t a = r * (segments + 1) + s;
            uint32_t b = a + segments + 1;
            idx.insert(idx.end(), { a, b, a+1, b, b+1, a+1 });
        }
    }
    return idx;
}

} // namespace arx

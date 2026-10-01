// ==============================================================================
// src/render/texture.hpp, mesh.hpp, shader.hpp — Tipos de recursos gráficos.
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace arx {

// Texture — imagen 2D cargada en GPU.
class Texture {
public:
    virtual ~Texture() = default;
    virtual void bind(unsigned int slot = 0) const = 0;
    virtual int  width()  const = 0;
    virtual int  height() const = 0;
    virtual int  channels() const = 0;
    virtual uint32_t id() const = 0;
};

// Mesh — malla 3D (VBO+VAO+IBO).
class Mesh {
public:
    virtual ~Mesh() = default;
    virtual void bind() const = 0;
    virtual void draw() const = 0;          // glDrawElements
    virtual int  vertex_count() const = 0;
    virtual int  index_count()   const = 0;
};

// Shader — programa GLSL compilado.
class Shader {
public:
    virtual ~Shader() = default;
    virtual void use() const = 0;
    virtual void set_uniform(const std::string& name, int v) = 0;
    virtual void set_uniform(const std::string& name, float v) = 0;
    virtual void set_uniform(const std::string& name, const Vector2& v) = 0;
    virtual void set_uniform(const std::string& name, const Vector3& v) = 0;
    virtual void set_uniform(const std::string& name, const Vector4& v) = 0;
    virtual void set_uniform(const std::string& name, const Matrix4& v) = 0;
    virtual void set_uniform(const std::string& name, const Color& v) = 0;
    virtual void set_uniform_texture(const std::string& name, int slot) = 0;
};

// Primitivas comunes.
struct MeshPrimitives {
    static std::vector<float> cube_vertices();
    static std::vector<uint32_t> cube_indices();
    static std::vector<float> quad_vertices();          // 2D quad 1x1 centrado
    static std::vector<uint32_t> quad_indices();
    static std::vector<float> sphere_vertices(int segments = 24, int rings = 16);
    static std::vector<uint32_t> sphere_indices(int segments = 24, int rings = 16);
};

} // namespace arx

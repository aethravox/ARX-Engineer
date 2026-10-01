// ==============================================================================
// src/core/types.hpp — Tipos matemáticos básicos del motor.
// Vector2/3/4, Color, Rect2, AABB, Transform2D, Transform3D, Quaternion.
// ==============================================================================
#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <cmath>
#include <algorithm>

namespace arx {

// Alias comunes para no depender de GLM en código de usuario.
using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;
using Vector2i = glm::ivec2;
using Vector3i = glm::ivec3;
using Vector4i = glm::ivec4;
using Matrix3 = glm::mat3;
using Matrix4 = glm::mat4;
using Quaternion = glm::quat;

// ==============================================================================
// Color — RGBA en floats [0..1] más constantes útiles.
// ==============================================================================
struct Color {
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

    constexpr Color() = default;
    constexpr Color(float r_, float g_, float b_, float a_ = 1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}

    // Construir desde uint32 0xRRGGBBAA
    static constexpr Color from_rgba8(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) {
        return Color{ r_ / 255.0f, g_ / 255.0f, b_ / 255.0f, a_ / 255.0f };
    }
    static constexpr Color from_hex(uint32_t hex) {
        return from_rgba8(
            (hex >> 24) & 0xFF,
            (hex >> 16) & 0xFF,
            (hex >> 8)  & 0xFF,
            hex & 0xFF);
    }

    constexpr Color operator*(float s) const { return { r*s, g*s, b*s, a*s }; }
    constexpr Color operator+(const Color& o) const { return { r+o.r, g+o.g, b+o.b, a+o.a }; }
    constexpr Color operator*(const Color& o) const { return { r*o.r, g*o.g, b*o.b, a*o.a }; }
    bool operator==(const Color& o) const {
        return r == o.r && g == o.g && b == o.b && a == o.a;
    }

    // Constantes
    static const Color white, black, red, green, blue, yellow, cyan, magenta, gray, transparent;
    static const Color arx_purple;   // Color de marca ARX
    static const Color arx_cyan;
};

inline constexpr Color Color::white       {1,1,1,1};
inline constexpr Color Color::black       {0,0,0,1};
inline constexpr Color Color::red         {1,0,0,1};
inline constexpr Color Color::green       {0,1,0,1};
inline constexpr Color Color::blue        {0,0,1,1};
inline constexpr Color Color::yellow      {1,1,0,1};
inline constexpr Color Color::cyan        {0,1,1,1};
inline constexpr Color Color::magenta     {1,0,1,1};
inline constexpr Color Color::gray        {0.5f,0.5f,0.5f,1};
inline constexpr Color Color::transparent {0,0,0,0};
inline constexpr Color Color::arx_purple  {0.42f, 0.20f, 0.85f, 1.0f}; // #6B36D9
inline constexpr Color Color::arx_cyan    {0.10f, 0.85f, 0.92f, 1.0f}; // #1AD9EB

// ==============================================================================
// Rect2 — rectángulo 2D con posición + tamaño.
// ==============================================================================
struct Rect2 {
    Vector2 position;
    Vector2 size;

    constexpr Rect2() = default;
    constexpr Rect2(Vector2 pos, Vector2 sz) : position(pos), size(sz) {}
    constexpr Rect2(float x, float y, float w, float h)
        : position(x, y), size(w, h) {}

    constexpr Vector2 center() const { return position + size * 0.5f; }
    constexpr Vector2 end() const { return position + size; }
    constexpr bool contains(Vector2 p) const {
        return p.x >= position.x && p.x < position.x + size.x &&
               p.y >= position.y && p.y < position.y + size.y;
    }
    constexpr bool intersects(const Rect2& o) const {
        return position.x < o.position.x + o.size.x &&
               position.x + size.x > o.position.x &&
               position.y < o.position.y + o.size.y &&
               position.y + size.y > o.position.y;
    }
    constexpr Rect2 merged(const Rect2& o) const {
        Vector2 mn(std::min(position.x, o.position.x),
                   std::min(position.y, o.position.y));
        Vector2 mx(std::max(end().x, o.end().x),
                   std::max(end().y, o.end().y));
        return Rect2(mn, mx - mn);
    }
    bool operator==(const Rect2& o) const {
        return position == o.position && size == o.size;
    }
};

// ==============================================================================
// AABB — bounding box 3D.
// ==============================================================================
struct AABB {
    Vector3 position;
    Vector3 size;

    constexpr AABB() = default;
    constexpr AABB(Vector3 pos, Vector3 sz) : position(pos), size(sz) {}

    constexpr Vector3 get_min() const { return position; }
    constexpr Vector3 get_max() const { return position + size; }
    constexpr Vector3 center() const { return position + size * 0.5f; }
    constexpr bool contains(Vector3 p) const {
        return p.x >= position.x && p.x < position.x + size.x &&
               p.y >= position.y && p.y < position.y + size.y &&
               p.z >= position.z && p.z < position.z + size.z;
    }
};

// ==============================================================================
// Transform2D — transformación afín 2D (origen, basis 2x2).
// ==============================================================================
struct Transform2D {
    Vector2 basis_x {1, 0};
    Vector2 basis_y {0, 1};
    Vector2 origin  {0, 0};

    constexpr Transform2D() = default;
    constexpr Transform2D(float rot, Vector2 pos) : origin(pos) {
        float c = std::cos(rot), s = std::sin(rot);
        basis_x = { c, s };
        basis_y = { -s, c };
    }

    static Transform2D translation(Vector2 v) { Transform2D t; t.origin = v; return t; }
    static Transform2D rotation(float rad)    { return Transform2D(rad, {0,0}); }
    static Transform2D scale(Vector2 s)       { Transform2D t; t.basis_x = {s.x,0}; t.basis_y = {0,s.y}; return t; }

    Transform2D operator*(const Transform2D& o) const {
        Transform2D r;
        r.basis_x = { basis_x.x*o.basis_x.x + basis_y.x*o.basis_x.y,
                      basis_x.y*o.basis_x.x + basis_y.y*o.basis_x.y };
        r.basis_y = { basis_x.x*o.basis_y.x + basis_y.x*o.basis_y.y,
                      basis_x.y*o.basis_y.x + basis_y.y*o.basis_y.y };
        r.origin  = basis_x * o.origin.x + basis_y * o.origin.y + origin;
        return r;
    }
    Vector2 operator*(Vector2 v) const {
        return basis_x * v.x + basis_y * v.y + origin;
    }
    Transform2D inverse() const {
        float det = basis_x.x * basis_y.y - basis_y.x * basis_x.y;
        float id = 1.0f / det;
        Transform2D r;
        r.basis_x = { basis_y.y * id, -basis_x.y * id };
        r.basis_y = { -basis_y.x * id, basis_x.x * id };
        r.origin  = -(r.basis_x * origin.x + r.basis_y * origin.y);
        return r;
    }
    Matrix4 to_matrix4() const {
        Matrix4 m(1.0f);
        m[0][0] = basis_x.x; m[1][0] = basis_x.y;
        m[0][1] = basis_y.x; m[1][1] = basis_y.y;
        m[3][0] = origin.x;  m[3][1] = origin.y;
        return m;
    }
};

// ==============================================================================
// Transform3D — basis (matriz 3x3) + origin.
// ==============================================================================
struct Transform3D {
    Matrix3 basis {1.0f};  // identidad
    Vector3 origin{0,0,0};

    constexpr Transform3D() = default;
    Transform3D(const Matrix3& b, const Vector3& o) : basis(b), origin(o) {}

    static Transform3D translation(Vector3 v) { Transform3D t; t.origin = v; return t; }
    static Transform3D rotation(const Quaternion& q) {
        Transform3D t; t.basis = glm::mat3_cast(q); return t;
    }
    static Transform3D scale(Vector3 s) {
        Transform3D t;
        t.basis = Matrix3(s.x,0,0, 0,s.y,0, 0,0,s.z);
        return t;
    }

    Transform3D operator*(const Transform3D& o) const {
        return Transform3D(basis * o.basis, basis * o.origin + origin);
    }
    Vector3 operator*(Vector3 v) const { return basis * v + origin; }
    Transform3D inverse() const {
        Matrix3 inv = glm::inverse(basis);
        return Transform3D(inv, -(inv * origin));
    }
    Matrix4 to_matrix4() const {
        Matrix4 m(basis);
        m[3] = Vector4(origin, 1.0f);
        return m;
    }
    Matrix4 to_view_matrix() const {
        // Para cámaras: view = inverse(world)
        return inverse().to_matrix4();
    }
};

// ==============================================================================
// StringID — strings internados para comparación rápida.
// ==============================================================================
struct StringID {
    uint64_t hash = 0;
    constexpr StringID() = default;
    explicit constexpr StringID(uint64_t h) : hash(h) {}
    bool operator==(const StringID& o) const { return hash == o.hash; }
    bool operator!=(const StringID& o) const { return hash != o.hash; }
    bool operator<(const StringID& o) const { return hash < o.hash; }
    constexpr bool valid() const { return hash != 0; }
};

// FNV-1a 64-bit
constexpr StringID sid(const char* s) {
    uint64_t h = 14695981039346656037ULL;
    while (*s) {
        h ^= (uint8_t)(*s++);
        h *= 1099511628211ULL;
    }
    return StringID(h);
}

} // namespace arx

// ==============================================================================
// std::hash<arx::StringID> — necesario para usar StringID como clave en
// unordered_map / unordered_set.
// ==============================================================================
#include <functional>
namespace std {
template<>
struct hash<arx::StringID> {
    size_t operator()(const arx::StringID& s) const noexcept {
        return std::hash<uint64_t>{}(s.hash);
    }
};
} // namespace std

// ==============================================================================
// src/core/math.hpp — utilidades matemáticas del motor.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include <cmath>
#include <cstdlib>

namespace arx {

namespace math {

constexpr float PI        = 3.14159265358979323846f;
constexpr float TAU       = 6.28318530717958647692f;
constexpr float EPSILON   = 1e-6f;
constexpr float DEG2RAD   = PI / 180.0f;
constexpr float RAD2DEG   = 180.0f / PI;

inline float deg_to_rad(float d) { return d * DEG2RAD; }
inline float rad_to_deg(float r) { return r * RAD2DEG; }

template<typename T>
inline T clamp(T v, T lo, T hi) { return std::max(lo, std::min(hi, v)); }

template<typename T>
inline T lerp(T a, T b, float t) { return a + (b - a) * t; }

inline float lerp_angle(float a, float b, float t) {
    float diff = std::fmod(b - a + PI, TAU) - PI;
    if (diff < -PI) diff += TAU;
    return a + diff * t;
}

inline float smoothstep(float e0, float e1, float x) {
    float t = clamp((x - e0) / (e1 - e0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float move_toward(float current, float target, float max_delta) {
    if (std::abs(target - current) <= max_delta) return target;
    return current + std::copysign(max_delta, target - current);
}

inline float random(float lo = 0.0f, float hi = 1.0f) {
    return lo + static_cast<float>(std::rand()) / RAND_MAX * (hi - lo);
}

inline int   random_int(int lo, int hi) {
    return lo + std::rand() % (hi - lo + 1);
}

inline Vector2 random_in_circle(float radius = 1.0f) {
    float a = random(0.0f, TAU);
    float r = radius * std::sqrt(random());
    return Vector2(std::cos(a) * r, std::sin(a) * r);
}

inline Vector3 random_in_sphere(float radius = 1.0f) {
    float a = random(0.0f, TAU);
    float v = random(-1.0f, 1.0f);
    float r = radius * std::cbrt(random());
    float s = std::sqrt(1.0f - v*v);
    return Vector3(s * std::cos(a) * r, s * std::sin(a) * r, v * r);
}

// Devuelve el vector más cercano a v dentro del segmento a-b.
inline Vector2 closest_point_on_segment(Vector2 p, Vector2 a, Vector2 b) {
    Vector2 ab = b - a;
    float t = glm::dot(p - a, ab) / std::max(glm::dot(ab, ab), EPSILON);
    t = clamp(t, 0.0f, 1.0f);
    return a + ab * t;
}

inline bool segments_intersect(
    Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4,
    Vector2* out_intersection = nullptr)
{
    Vector2 r = p2 - p1, s = p4 - p3;
    float rxs = r.x * s.y - r.y * s.x;
    if (std::abs(rxs) < EPSILON) return false;
    Vector2 qp = p3 - p1;
    float t = (qp.x * s.y - qp.y * s.x) / rxs;
    float u = (qp.x * r.y - qp.y * r.x) / rxs;
    if (t < 0 || t > 1 || u < 0 || u > 1) return false;
    if (out_intersection) *out_intersection = p1 + r * t;
    return true;
}

} // namespace math

} // namespace arx

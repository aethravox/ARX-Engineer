// =============================================================
// ARX ENGINE - Modulo de Matematicas (GLM)
// Utilidades matematicas usando GLM
// Conversiones entre GLM y Raylib
// =============================================================
#ifndef ARX_MATH_HPP
#define ARX_MATH_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "raylib.h"
#include <cmath>
#include <iostream>
#include <cstring>

namespace ARX {

namespace Math {

    // =============================================================
    // Conversiones GLM <-> Raylib
    // =============================================================

    inline glm::vec3 ToGLM(const Vector3& v) { return glm::vec3(v.x, v.y, v.z); }
    inline Vector3 ToRaylib(const glm::vec3& v) { return Vector3{ v.x, v.y, v.z }; }

    // El layout en memoria de Matrix (Raylib) y mat4 (GLM) es identico (16 floats)
    inline Matrix ToRaylib(const glm::mat4& m) {
        Matrix result;
        std::memcpy(&result, glm::value_ptr(m), sizeof(float) * 16);
        return result;
    }

    inline glm::mat4 ToGLM(const Matrix& m) {
        glm::mat4 result;
        std::memcpy(glm::value_ptr(result), &m, sizeof(float) * 16);
        return result;
    }

    // IMPORTANTE: GLM Quat constructor es (w, x, y, z), Raylib es {x, y, z, w}
    inline glm::quat ToGLM(const Quaternion& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    inline Quaternion ToRaylib(const glm::quat& q) {
        return Quaternion{ q.x, q.y, q.z, q.w };
    }

    // =============================================
    // Operaciones Basicas
    // =============================================

    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * glm::clamp(t, 0.0f, 1.0f);
    }

    inline Vector3 LerpV3(const Vector3& a, const Vector3& b, float t) {
        return ToRaylib(glm::lerp(ToGLM(a), ToGLM(b), glm::clamp(t, 0.0f, 1.0f)));
    }

    inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
        return ToRaylib(glm::slerp(ToGLM(a), ToGLM(b), glm::clamp(t, 0.0f, 1.0f)));
    }

    inline float Distance(const Vector3& a, const Vector3& b) {
        return glm::distance(ToGLM(a), ToGLM(b));
    }

    inline float DistanceSq(const Vector3& a, const Vector3& b) {
        return glm::distance2(ToGLM(a), ToGLM(b));
    }

    inline Vector3 Normalize(const Vector3& v) {
        glm::vec3 gv = ToGLM(v);
        if (glm::length2(gv) < 0.00001f) return {0,0,0};
        return ToRaylib(glm::normalize(gv));
    }

    inline Vector3 Cross(const Vector3& a, const Vector3& b) {
        return ToRaylib(glm::cross(ToGLM(a), ToGLM(b)));
    }

    inline float Dot(const Vector3& a, const Vector3& b) {
        return glm::dot(ToGLM(a), ToGLM(b));
    }

    // =============================================
    // Matrices de Transformacion
    // =============================================

    inline Matrix Translation(float x, float y, float z) {
        return ToRaylib(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
    }

    inline Matrix Rotation(float angleDeg, const Vector3& axis) {
        return ToRaylib(glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), ToGLM(axis)));
    }

    inline Matrix Scale(float sx, float sy, float sz) {
        return ToRaylib(glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz)));
    }

    // Matriz de transformacion completa TRS (Translation * Rotation * Scale)
    inline Matrix ComposeTransform(const Vector3& pos, const Quaternion& rot, const Vector3& sca) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), ToGLM(pos)) * 
                      glm::mat4_cast(ToGLM(rot)) * 
                      glm::scale(glm::mat4(1.0f), ToGLM(sca));
        return ToRaylib(m);
    }

    // =============================================
    // Utilidades de Angulos
    // =============================================

    inline Quaternion QuaternionFromEuler(float pitch, float yaw, float roll) {
        // GLM usa radianes para los angulos de Euler
        return ToRaylib(glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll))));
    }

    inline float Deg2Rad(float deg) { return glm::radians(deg); }
    inline float Rad2Deg(float rad) { return glm::degrees(rad); }

} // namespace Math
} // namespace ARX

#endif


//
// Created by adrian on 05/10/2025.
//

#ifndef ASHEN_MATH_H
#define ASHEN_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ash {
    // Vector
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;

    using IVec2 = glm::ivec2;
    using IVec3 = glm::ivec3;
    using IVec4 = glm::ivec4;

    using UVec2 = glm::uvec2;
    using UVec3 = glm::uvec3;
    using UVec4 = glm::uvec4;

    using BVec2 = glm::bvec2;
    using BVec3 = glm::bvec3;
    using BVec4 = glm::bvec4;

    // Matrix
    using Mat2 = glm::mat2;
    using Mat3 = glm::mat3;
    using Mat4 = glm::mat4;

    // Quaternion
    using Quat = glm::quat;

    inline constexpr float PI = 3.14159265358979323846f;
    inline constexpr float TWO_PI = PI * 2.0f;
    inline constexpr float HALF_PI = PI * 0.5f;

    inline Mat4 translate(const Mat4 &mat, const Vec3 &offset) {
        return glm::translate(mat, offset);
    }

    inline Mat4 rotate(const Mat4 &mat, const float angleRadians, const Vec3 &axis) {
        return glm::rotate(mat, angleRadians, axis);
    }

    inline Mat4 scale(const Mat4 &mat, const Vec3 &factor) {
        return glm::scale(mat, factor);
    }

    inline Mat4 perspective(const float fovRadians, const float aspect, const float nearPlane, const float farPlane) {
        return glm::perspective(fovRadians, aspect, nearPlane, farPlane);
    }

    inline Mat4 ortho(const float left, const float right, const float bottom, const float top, const float nearPlane,
                      const float farPlane) {
        return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }

    inline Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
        return glm::lookAt(eye, center, up);
    }

    inline Quat quatFromEuler(const float pitch, const float yaw, const float roll) {
        return {glm::vec3(pitch, yaw, roll)};
    }

    inline Mat4 quatToMat4(const Quat &q) {
        return glm::toMat4(q);
    }

    inline const float *valuePtr(const Mat4 &mat) {
        return glm::value_ptr(mat);
    }

    inline const float *valuePtr(const Vec3 &vec) {
        return glm::value_ptr(vec);
    }

    inline float radians(const float degrees) { return glm::radians(degrees); }
    inline float degrees(const float radians) { return glm::degrees(radians); }

    inline float dot(const Vec3 &a, const Vec3 &b) { return glm::dot(a, b); }
    inline Vec3 cross(const Vec3 &a, const Vec3 &b) { return glm::cross(a, b); }

    inline Vec3 normalize(const Vec3 &v) { return glm::normalize(v); }
    inline float length(const Vec3 &v) { return glm::length(v); }
}

#endif //ASHEN_MATH_H
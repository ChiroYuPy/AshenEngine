#ifndef ASHEN_MATH_H
#define ASHEN_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <limits>
#include <cmath>

namespace ash {
    // ========== Vector Types ==========
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

    using DVec2 = glm::dvec2;
    using DVec3 = glm::dvec3;
    using DVec4 = glm::dvec4;

    // ========== Color Types ==========
    using Color3 = Vec3;
    using Color4 = Vec4;
    using ColorRGB = Vec3;
    using ColorRGBA = Vec4;

    // ========== Matrix Types ==========
    using Mat2 = glm::mat2;
    using Mat3 = glm::mat3;
    using Mat4 = glm::mat4;

    using Mat2x3 = glm::mat2x3;
    using Mat2x4 = glm::mat2x4;
    using Mat3x2 = glm::mat3x2;
    using Mat3x4 = glm::mat3x4;
    using Mat4x2 = glm::mat4x2;
    using Mat4x3 = glm::mat4x3;

    // ========== Quaternion ==========
    using Quaternion = glm::quat;

    // ========== Constants ==========
    namespace Constants {
        inline constexpr float Pi = 3.14159265358979323846f;
        inline constexpr float TwoPi = Pi * 2.0f;
        inline constexpr float HalfPi = Pi * 0.5f;
        inline constexpr float QuarterPi = Pi * 0.25f;
        inline constexpr float E = 2.71828182845904523536f;
        inline constexpr float Sqrt2 = 1.41421356237309504880f;
        inline constexpr float Sqrt3 = 1.73205080756887729352f;
        inline constexpr float Epsilon = 1e-6f;
        inline constexpr float DEpsilon = 1e-12;
    }

    // Backward compatibility
    inline constexpr float PI = Constants::Pi;
    inline constexpr float TWO_PI = Constants::TwoPi;
    inline constexpr float HALF_PI = Constants::HalfPi;

    // ========== Numeric Limits ==========
    template<typename T>
    constexpr T MaxValue() noexcept {
        return std::numeric_limits<T>::max();
    }

    template<typename T>
    constexpr T MinValue() noexcept {
        return std::numeric_limits<T>::min();
    }

    template<typename T>
    constexpr T LowestValue() noexcept {
        return std::numeric_limits<T>::lowest();
    }

    template<typename T>
    constexpr T Infinity() noexcept {
        return std::numeric_limits<T>::infinity();
    }

    // ========== Angle Conversion ==========
    inline float ToRadians(const float degrees) noexcept {
        return glm::radians(degrees);
    }

    inline float ToDegrees(const float radians) noexcept {
        return glm::degrees(radians);
    }

    inline double ToRadians(const double degrees) noexcept {
        return glm::radians(degrees);
    }

    inline double ToDegrees(const double radians) noexcept {
        return glm::degrees(radians);
    }

    // ========== Basic Math Functions ==========
    template<typename T>
    constexpr T Abs(T value) noexcept {
        return value < T(0) ? -value : value;
    }

    template<typename T>
    constexpr T Sign(T value) noexcept {
        return (T(0) < value) - (value < T(0));
    }

    template<typename T>
    constexpr T Clamp(T value, T minVal, T maxVal) noexcept {
        return value < minVal ? minVal : value > maxVal ? maxVal : value;
    }

    template<typename T>
    constexpr T Clamp01(T value) noexcept {
        return Clamp(value, T(0), T(1));
    }

    template<typename T>
    constexpr T Min(T a, T b) noexcept {
        return a < b ? a : b;
    }

    template<typename T>
    constexpr T Max(T a, T b) noexcept {
        return a > b ? a : b;
    }

    template<typename T>
    constexpr T Min(T a, T b, T c) noexcept {
        return Min(Min(a, b), c);
    }

    template<typename T>
    constexpr T Max(T a, T b, T c) noexcept {
        return Max(Max(a, b), c);
    }

    // ========== Interpolation ==========
    template<typename T>
    T Lerp(T a, T b, float t) noexcept {
        return a + (b - a) * t;
    }

    template<typename T>
    T LerpUnclamped(T a, T b, float t) noexcept {
        return a + (b - a) * t;
    }

    template<typename T>
    T InverseLerp(T a, T b, T value) noexcept {
        if (Abs(b - a) < Constants::Epsilon) return T(0);
        return (value - a) / (b - a);
    }

    template<typename T>
    T SmoothStep(T edge0, T edge1, T x) noexcept {
        T t = Clamp01((x - edge0) / (edge1 - edge0));
        return t * t * (T(3) - T(2) * t);
    }

    template<typename T>
    T SmootherStep(T edge0, T edge1, T x) noexcept {
        T t = Clamp01((x - edge0) / (edge1 - edge0));
        return t * t * t * (t * (t * T(6) - T(15)) + T(10));
    }

    // ========== Rounding ==========
    inline float Floor(const float value) noexcept {
        return std::floor(value);
    }

    inline float Ceil(const float value) noexcept {
        return std::ceil(value);
    }

    inline float Round(const float value) noexcept {
        return std::round(value);
    }

    inline float Trunc(const float value) noexcept {
        return std::trunc(value);
    }

    inline float Fract(const float value) noexcept {
        return value - Floor(value);
    }

    template<typename T>
    T Mod(T a, T b) noexcept {
        return std::fmod(a, b);
    }

    // ========== Power & Root Functions ==========
    inline float Sqrt(const float value) noexcept {
        return std::sqrt(value);
    }

    inline float InvSqrt(const float value) noexcept {
        return 1.0f / std::sqrt(value);
    }

    inline float Pow(const float base, const float exp) noexcept {
        return std::pow(base, exp);
    }

    inline float Exp(const float value) noexcept {
        return std::exp(value);
    }

    inline float Log(const float value) noexcept {
        return std::log(value);
    }

    inline float Log10(const float value) noexcept {
        return std::log10(value);
    }

    // ========== Trigonometric Functions ==========
    inline float Sin(const float angle) noexcept {
        return std::sin(angle);
    }

    inline float Cos(const float angle) noexcept {
        return std::cos(angle);
    }

    inline float Tan(const float angle) noexcept {
        return std::tan(angle);
    }

    inline float Asin(const float value) noexcept {
        return std::asin(value);
    }

    inline float Acos(const float value) noexcept {
        return std::acos(value);
    }

    inline float Atan(const float value) noexcept {
        return std::atan(value);
    }

    inline float Atan2(const float y, const float x) noexcept {
        return std::atan2(y, x);
    }

    // ========== Vector Operations ==========
    template<typename Vec>
    float Length(const Vec &v) noexcept {
        return glm::length(v);
    }

    template<typename Vec>
    float LengthSqr(const Vec &v) noexcept {
        return glm::dot(v, v);
    }

    template<typename Vec>
    Vec Normalize(const Vec &v) noexcept {
        return glm::normalize(v);
    }

    template<typename Vec>
    Vec SafeNormalize(const Vec &v, const Vec &fallback = Vec(0)) noexcept {
        float len = Length(v);
        return len > Constants::Epsilon ? v / len : fallback;
    }

    template<typename Vec>
    float Dot(const Vec &a, const Vec &b) noexcept {
        return glm::dot(a, b);
    }

    inline Vec3 Cross(const Vec3 &a, const Vec3 &b) noexcept {
        return glm::cross(a, b);
    }

    template<typename Vec>
    float Distance(const Vec &a, const Vec &b) noexcept {
        return glm::distance(a, b);
    }

    template<typename Vec>
    float DistanceSqr(const Vec &a, const Vec &b) noexcept {
        Vec diff = a - b;
        return glm::dot(diff, diff);
    }

    template<typename Vec>
    Vec Reflect(const Vec &incident, const Vec &normal) noexcept {
        return glm::reflect(incident, normal);
    }

    template<typename Vec>
    Vec Refract(const Vec &incident, const Vec &normal, float eta) noexcept {
        return glm::refract(incident, normal, eta);
    }

    template<typename Vec>
    Vec Project(const Vec &a, const Vec &b) noexcept {
        return Dot(a, b) / LengthSqr(b) * b;
    }

    template<typename Vec>
    Vec ProjectOnPlane(const Vec &v, const Vec &planeNormal) noexcept {
        return v - Project(v, planeNormal);
    }

    inline float Angle(const Vec3 &from, const Vec3 &to) noexcept {
        const float denominator = Sqrt(LengthSqr(from) * LengthSqr(to));
        if (denominator < Constants::Epsilon) return 0.0f;
        return Acos(Clamp(Dot(from, to) / denominator, -1.0f, 1.0f));
    }

    inline float SignedAngle(const Vec3 &from, const Vec3 &to, const Vec3 &axis) noexcept {
        const float unsignedAngle = Angle(from, to);
        const float sign = Sign(Dot(axis, Cross(from, to)));
        return unsignedAngle * sign;
    }

    // ========== Matrix Operations ==========
    inline Mat4 Identity() noexcept {
        return {1.0f};
    }

    inline Mat4 Translate(const Mat4 &mat, const Vec3 &offset) noexcept {
        return glm::translate(mat, offset);
    }

    inline Mat4 Rotate(const Mat4 &mat, const float angleRad, const Vec3 &axis) noexcept {
        return glm::rotate(mat, angleRad, axis);
    }

    inline Mat4 Scale(const Mat4 &mat, const Vec3 &factor) noexcept {
        return glm::scale(mat, factor);
    }

    inline Mat4 Transpose(const Mat4 &mat) noexcept {
        return glm::transpose(mat);
    }

    inline Mat4 Inverse(const Mat4 &mat) noexcept {
        return glm::inverse(mat);
    }

    inline float Determinant(const Mat4 &mat) noexcept {
        return glm::determinant(mat);
    }

    // ========== Projection Matrices ==========
    inline Mat4 Perspective(const float fovRad, const float aspect, const float nearPlane,
                            const float farPlane) noexcept {
        return glm::perspective(fovRad, aspect, nearPlane, farPlane);
    }

    inline Mat4 Ortho(const float left, const float right, const float bottom, const float top, const float nearPlane,
                      const float farPlane) noexcept {
        return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }

    inline Mat4 Ortho2D(const float left, const float right, const float bottom, const float top) noexcept {
        return glm::ortho(left, right, bottom, top);
    }

    inline Mat4 LookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) noexcept {
        return glm::lookAt(eye, center, up);
    }

    // ========== Quaternion Operations ==========
    inline Quaternion QuatFromEuler(const float pitch, const float yaw, const float roll) noexcept {
        return {Vec3(pitch, yaw, roll)};
    }

    inline Quaternion QuatFromEuler(const Vec3 &euler) noexcept {
        return {euler};
    }

    inline Vec3 QuatToEuler(const Quaternion &q) noexcept {
        return glm::eulerAngles(q);
    }

    inline Mat4 QuatToMat4(const Quaternion &q) noexcept {
        return glm::mat4_cast(q);
    }

    inline Mat3 QuatToMat3(const Quaternion &q) noexcept {
        return glm::mat3_cast(q);
    }

    inline Quaternion QuatFromAxisAngle(const Vec3 &axis, const float angleRad) noexcept {
        return glm::angleAxis(angleRad, axis);
    }

    inline Quaternion QuatLookRotation(const Vec3 &forward, const Vec3 &up = Vec3(0, 1, 0)) noexcept {
        return glm::quatLookAt(forward, up);
    }

    inline Quaternion QuatSlerp(const Quaternion &a, const Quaternion &b, const float t) noexcept {
        return glm::slerp(a, b, t);
    }

    inline Quaternion QuatNormalize(const Quaternion &q) noexcept {
        return glm::normalize(q);
    }

    inline Quaternion QuatInverse(const Quaternion &q) noexcept {
        return glm::inverse(q);
    }

    inline float QuatDot(const Quaternion &a, const Quaternion &b) noexcept {
        return glm::dot(a, b);
    }

    // ========== Value Pointers ==========
    inline const float *ValuePtr(const Mat4 &mat) noexcept {
        return glm::value_ptr(mat);
    }

    inline const float *ValuePtr(const Mat3 &mat) noexcept {
        return glm::value_ptr(mat);
    }

    inline const float *ValuePtr(const Vec2 &vec) noexcept {
        return glm::value_ptr(vec);
    }

    inline const float *ValuePtr(const Vec3 &vec) noexcept {
        return glm::value_ptr(vec);
    }

    inline const float *ValuePtr(const Vec4 &vec) noexcept {
        return glm::value_ptr(vec);
    }

    inline const float *ValuePtr(const Quaternion &quat) noexcept {
        return glm::value_ptr(quat);
    }

    // ========== Comparison Functions ==========
    template<typename T>
    bool ApproxEqual(T a, T b, T epsilon = Constants::Epsilon) noexcept {
        return Abs(a - b) < epsilon;
    }

    inline bool ApproxEqual(const Vec2 &a, const Vec2 &b, const float epsilon = Constants::Epsilon) noexcept {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }

    inline bool ApproxEqual(const Vec3 &a, const Vec3 &b, const float epsilon = Constants::Epsilon) noexcept {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }

    inline bool ApproxEqual(const Vec4 &a, const Vec4 &b, const float epsilon = Constants::Epsilon) noexcept {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }

    inline bool ApproxZero(const float value, const float epsilon = Constants::Epsilon) noexcept {
        return Abs(value) < epsilon;
    }

    template<typename T>
    constexpr void Swap(T &a, T &b) noexcept {
        std::swap(a, b);
    }

    template<typename T>
    constexpr T LimitMin() noexcept {
        return std::numeric_limits<T>::min();
    }

    template<typename T>
    constexpr T LimitMax() noexcept {
        return std::numeric_limits<T>::max();
    }

    template<typename T>
    constexpr T LimitLowest() noexcept {
        return std::numeric_limits<T>::lowest();
    }
}

#endif // ASHEN_MATH_H
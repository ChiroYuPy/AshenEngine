#ifndef ASHEN_TRANSFORM_H
#define ASHEN_TRANSFORM_H

#include "Math.h"

namespace ash {
    struct Transform2D {
        Vec2 position = Vec2(0.f, 0.f);
        float rotation = 0.f;
        Vec2 scale = Vec2(1.f, 1.f);

        Transform2D() = default;

        explicit Transform2D(const Vec2 &pos, const float rot = 0.f, const Vec2 &scl = Vec2(1.f))
            : position(pos), rotation(rot), scale(scl) {
        }

        Mat3 ToMatrix() const {
            const float cos_r = Cos(rotation);
            const float sin_r = Sin(rotation);

            Mat3 mat(1.f);

            mat[0][0] = cos_r * scale.x;
            mat[0][1] = sin_r * scale.x;
            mat[0][2] = 0.f;

            mat[1][0] = -sin_r * scale.y;
            mat[1][1] = cos_r * scale.y;
            mat[1][2] = 0.f;

            mat[2][0] = position.x;
            mat[2][1] = position.y;
            mat[2][2] = 1.f;

            return mat;
        }

        static Transform2D FromMatrix(const Mat3 &mat) {
            Transform2D t;

            t.position = Vec2(mat[2][0], mat[2][1]);

            const Vec2 xAxis(mat[0][0], mat[0][1]);
            const Vec2 yAxis(mat[1][0], mat[1][1]);

            t.scale.x = Length(xAxis);
            t.scale.y = Length(yAxis);

            if (t.scale.x > Constants::Epsilon)
                t.rotation = Atan2(mat[0][1] / t.scale.x, mat[0][0] / t.scale.x);
            else
                t.rotation = 0.f;

            return t;
        }

        Transform2D Combine(const Transform2D &parent) const {
            const Mat3 parentMat = parent.ToMatrix();
            const Mat3 localMat = ToMatrix();
            const Mat3 result = parentMat * localMat;
            return FromMatrix(result);
        }

        Vec2 TransformPoint(const Vec2 &point) const {
            const float cos_r = Cos(rotation);
            const float sin_r = Sin(rotation);

            const Vec2 scaled(point.x * scale.x, point.y * scale.y);
            return Vec2(
                cos_r * scaled.x - sin_r * scaled.y + position.x,
                sin_r * scaled.x + cos_r * scaled.y + position.y
            );
        }

        Vec2 TransformDirection(const Vec2 &dir) const {
            const float cos_r = Cos(rotation);
            const float sin_r = Sin(rotation);

            return Vec2(
                cos_r * dir.x * scale.x - sin_r * dir.y * scale.y,
                sin_r * dir.x * scale.x + cos_r * dir.y * scale.y
            );
        }

        Transform2D Inverse() const {
            const Mat3 mat = ToMatrix();
            const Mat3 inv = glm::inverse(mat);
            return FromMatrix(inv);
        }

        Transform2D Lerp(const Transform2D &other, float t) const {
            Transform2D result;
            result.position = ash::Lerp(position, other.position, t);
            result.rotation = ash::Lerp(rotation, other.rotation, t);
            result.scale = ash::Lerp(scale, other.scale, t);
            return result;
        }

        Transform2D operator*(const Transform2D &other) const {
            return Combine(other);
        }

        bool operator==(const Transform2D &other) const {
            return ApproxEqual(position, other.position) &&
                   ApproxEqual(rotation, other.rotation) &&
                   ApproxEqual(scale, other.scale);
        }

        bool operator!=(const Transform2D &other) const {
            return !(*this == other);
        }

        static const Transform2D Identity;
    };

    struct Transform3D {
        Vec3 position = Vec3(0.f, 0.f, 0.f);
        Quaternion rotation = Quaternion(1.f, 0.f, 0.f, 0.f);
        Vec3 scale = Vec3(1.f, 1.f, 1.f);

        Transform3D() = default;

        explicit Transform3D(const Vec3 &pos, const Quaternion &rot = Quaternion(1.f, 0.f, 0.f, 0.f), const Vec3 &scl = Vec3(1.f))
        : position(pos), rotation(rot), scale(scl) {}

        static Transform3D FromEuler(const Vec3 &pos, const Vec3 &eulerRad, const Vec3 &scl = Vec3(1.f)) {
            return Transform3D(pos, QuatFromEuler(eulerRad), scl);
        }

        Mat4 ToMatrix() const {
            const Mat4 identity = ash::Identity();
            const Mat4 translation = Translate(identity, position);
            const Mat4 rotationMat = QuatToMat4(rotation);
            const Mat4 scaleMat = Scale(identity, scale);

            return translation * rotationMat * scaleMat;
        }

        static Transform3D FromMatrix(const Mat4 &mat) {
            Transform3D t;

            t.position = Vec3(mat[3]);

            Vec3 col0(mat[0]);
            Vec3 col1(mat[1]);
            Vec3 col2(mat[2]);

            t.scale.x = Length(col0);
            t.scale.y = Length(col1);
            t.scale.z = Length(col2);

            if (t.scale.x > Constants::Epsilon) col0 /= t.scale.x;
            if (t.scale.y > Constants::Epsilon) col1 /= t.scale.y;
            if (t.scale.z > Constants::Epsilon) col2 /= t.scale.z;

            Mat3 rotMat;
            rotMat[0] = col0;
            rotMat[1] = col1;
            rotMat[2] = col2;

            t.rotation = glm::quat_cast(rotMat);
            t.rotation = QuatNormalize(t.rotation);

            return t;
        }

        Transform3D Combine(const Transform3D &parent) const {
            Transform3D result;

            result.position = parent.position + parent.rotation * (parent.scale * position);
            result.rotation = QuatNormalize(parent.rotation * rotation);
            result.scale = parent.scale * scale;

            return result;
        }

        Vec3 TransformPoint(const Vec3 &point) const {
            const Vec3 scaled = scale * point;
            const Vec3 rotated = rotation * scaled;
            return position + rotated;
        }

        Vec3 TransformDirection(const Vec3 &dir) const {
            const Vec3 scaled = scale * dir;
            return rotation * scaled;
        }

        Transform3D Inverse() const {
            Transform3D inv;

            inv.rotation = QuatInverse(rotation);

            inv.scale = Vec3(
                Abs(scale.x) > Constants::Epsilon ? 1.0f / scale.x : 0.0f,
                Abs(scale.y) > Constants::Epsilon ? 1.0f / scale.y : 0.0f,
                Abs(scale.z) > Constants::Epsilon ? 1.0f / scale.z : 0.0f
            );

            inv.position = inv.rotation * (inv.scale * -position);

            return inv;
        }

        Vec3 GetEulerAngles() const {
            return QuatToEuler(rotation);
        }

        void SetEulerAngles(const Vec3 &euler) {
            rotation = QuatFromEuler(euler);
        }

        Vec3 GetRight() const {
            return rotation * Vec3(1.f, 0.f, 0.f);
        }

        Vec3 GetUp() const {
            return rotation * Vec3(0.f, 1.f, 0.f);
        }

        Vec3 GetForward() const {
            return rotation * Vec3(0.f, 0.f, -1.f);
        }

        Transform3D Lerp(const Transform3D &other, float t) const {
            Transform3D result;
            result.position = ash::Lerp(position, other.position, t);
            result.rotation = QuatSlerp(rotation, other.rotation, t);
            result.scale = ash::Lerp(scale, other.scale, t);
            return result;
        }

        Transform3D operator*(const Transform3D &other) const {
            return Combine(other);
        }

        bool operator==(const Transform3D &other) const {
            return ApproxEqual(position, other.position) &&
                   ApproxEqual(rotation.x, other.rotation.x) &&
                   ApproxEqual(rotation.y, other.rotation.y) &&
                   ApproxEqual(rotation.z, other.rotation.z) &&
                   ApproxEqual(rotation.w, other.rotation.w) &&
                   ApproxEqual(scale, other.scale);
        }

        bool operator!=(const Transform3D &other) const {
            return !(*this == other);
        }

        static const Transform3D Identity;
    };
}

#endif // ASHEN_TRANSFORM_H
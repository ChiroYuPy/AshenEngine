#ifndef ASHEN_TRANSFORM_H
#define ASHEN_TRANSFORM_H

#include "Ashen/Math/Math.h"
#include <glm/gtc/quaternion.hpp>

namespace ash {
    class Transform {
    public:
        // ===== Constructeurs =====
        Transform()
            : m_Position(0.0f)
              , m_Rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
              , m_Scale(1.0f)
              , m_Dirty(true) {
        }

        explicit Transform(const Vec3 &position)
            : m_Position(position)
              , m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
              , m_Scale(1.0f)
              , m_Dirty(true) {
        }

        Transform(const Vec3 &position, const Quat &rotation)
            : m_Position(position)
              , m_Rotation(rotation)
              , m_Scale(1.0f)
              , m_Dirty(true) {
        }

        Transform(const Vec3 &position, const Quat &rotation, const Vec3 &scale)
            : m_Position(position)
              , m_Rotation(rotation)
              , m_Scale(scale)
              , m_Dirty(true) {
        }

        // ===== Position =====
        void SetPosition(const Vec3 &position) {
            m_Position = position;
            m_Dirty = true;
        }

        void SetPosition(float x, float y, float z) {
            SetPosition(Vec3(x, y, z));
        }

        const Vec3 &GetPosition() const { return m_Position; }

        void Translate(const Vec3 &offset) {
            m_Position += offset;
            m_Dirty = true;
        }

        void TranslateLocal(const Vec3 &offset) {
            m_Position += m_Rotation * offset;
            m_Dirty = true;
        }

        // ===== Rotation =====
        void SetRotation(const Quat &rotation) {
            m_Rotation = rotation;
            m_Dirty = true;
        }

        void SetRotation(const Vec3 &eulerAngles) {
            m_Rotation = Quat(eulerAngles);
            m_Dirty = true;
        }

        void SetRotation(float pitch, float yaw, float roll) {
            SetRotation(Vec3(pitch, yaw, roll));
        }

        const Quat &GetRotation() const { return m_Rotation; }

        Vec3 GetEulerAngles() const {
            return glm::eulerAngles(m_Rotation);
        }

        void Rotate(const Quat &rotation) {
            m_Rotation = rotation * m_Rotation;
            m_Dirty = true;
        }

        void Rotate(const Vec3 &axis, float angle) {
            Rotate(glm::angleAxis(angle, axis));
        }

        void RotateAround(const Vec3 &point, const Vec3 &axis, float angle) {
            const Vec3 offset = m_Position - point;
            const Quat rotation = glm::angleAxis(angle, axis);
            m_Position = point + rotation * offset;
            m_Rotation = rotation * m_Rotation;
            m_Dirty = true;
        }

        void LookAt(const Vec3 &target, const Vec3 &up = Vec3(0, 1, 0)) {
            const Vec3 forward = Normalize(target - m_Position);
            m_Rotation = glm::quatLookAt(forward, up);
            m_Dirty = true;
        }

        // ===== Scale =====
        void SetScale(const Vec3 &scale) {
            m_Scale = scale;
            m_Dirty = true;
        }

        void SetScale(float uniformScale) {
            SetScale(Vec3(uniformScale));
        }

        void SetScale(float x, float y, float z) {
            SetScale(Vec3(x, y, z));
        }

        const Vec3 &GetScale() const { return m_Scale; }

        void Scale(const Vec3 &scale) {
            m_Scale *= scale;
            m_Dirty = true;
        }

        void Scale(float uniformScale) {
            Scale(Vec3(uniformScale));
        }

        // ===== Direction Vectors =====
        Vec3 Forward() const {
            return Normalize(m_Rotation * Vec3(0, 0, -1));
        }

        Vec3 Back() const {
            return -Forward();
        }

        Vec3 Right() const {
            return Normalize(m_Rotation * Vec3(1, 0, 0));
        }

        Vec3 Left() const {
            return -Right();
        }

        Vec3 Up() const {
            return Normalize(m_Rotation * Vec3(0, 1, 0));
        }

        Vec3 Down() const {
            return -Up();
        }

        // ===== Matrix Operations =====
        const Mat4 &GetMatrix() const {
            if (m_Dirty) {
                UpdateMatrix();
            }
            return m_Matrix;
        }

        Mat4 GetInverseMatrix() const {
            return glm::inverse(GetMatrix());
        }

        void SetFromMatrix(const Mat4 &matrix) {
            // Extraire la position
            m_Position = Vec3(matrix[3]);

            // Extraire l'échelle
            m_Scale.x = glm::length(Vec3(matrix[0]));
            m_Scale.y = glm::length(Vec3(matrix[1]));
            m_Scale.z = glm::length(Vec3(matrix[2]));

            // Extraire la rotation (sans shear)
            Mat3 rotMat;
            rotMat[0] = Vec3(matrix[0]) / m_Scale.x;
            rotMat[1] = Vec3(matrix[1]) / m_Scale.y;
            rotMat[2] = Vec3(matrix[2]) / m_Scale.z;
            m_Rotation = glm::quat_cast(rotMat);

            m_Dirty = true;
        }

        // ===== Transformations =====
        Vec3 TransformPoint(const Vec3 &point) const {
            return Vec3(GetMatrix() * Vec4(point, 1.0f));
        }

        Vec3 TransformDirection(const Vec3 &direction) const {
            return Normalize(m_Rotation * direction);
        }

        Vec3 InverseTransformPoint(const Vec3 &point) const {
            return Vec3(GetInverseMatrix() * Vec4(point, 1.0f));
        }

        Vec3 InverseTransformDirection(const Vec3 &direction) const {
            return Normalize(glm::inverse(m_Rotation) * direction);
        }

        // ===== Hierarchy Support =====
        Transform GetLocalToWorld(const Transform &parent) const {
            Transform result;
            result.m_Position = parent.TransformPoint(m_Position);
            result.m_Rotation = parent.m_Rotation * m_Rotation;
            result.m_Scale = parent.m_Scale * m_Scale;
            result.m_Dirty = true;
            return result;
        }

        Transform GetWorldToLocal(const Transform &parent) const {
            Transform result;
            result.m_Position = parent.InverseTransformPoint(m_Position);
            result.m_Rotation = glm::inverse(parent.m_Rotation) * m_Rotation;
            result.m_Scale = m_Scale / parent.m_Scale;
            result.m_Dirty = true;
            return result;
        }

        // ===== Interpolation =====
        static Transform Lerp(const Transform &a, const Transform &b, float t) {
            Transform result;
            result.m_Position = ash::Lerp(a.m_Position, b.m_Position, t);
            result.m_Rotation = glm::slerp(a.m_Rotation, b.m_Rotation, t);
            result.m_Scale = ash::Lerp(a.m_Scale, b.m_Scale, t);
            result.m_Dirty = true;
            return result;
        }

        // ===== Comparison =====
        bool operator==(const Transform &other) const {
            return ApproxEqual(m_Position, other.m_Position) &&
                   ApproxEqual(m_Scale, other.m_Scale) &&
                   ApproxEqual(m_Rotation.x, other.m_Rotation.x) &&
                   ApproxEqual(m_Rotation.y, other.m_Rotation.y) &&
                   ApproxEqual(m_Rotation.z, other.m_Rotation.z) &&
                   ApproxEqual(m_Rotation.w, other.m_Rotation.w);
        }

        bool operator!=(const Transform &other) const {
            return !(*this == other);
        }

        // ===== Reset =====
        void Reset() {
            m_Position = Vec3(0.0f);
            m_Rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
            m_Scale = Vec3(1.0f);
            m_Dirty = true;
        }

    private:
        void UpdateMatrix() const {
            m_Matrix = glm::translate(Mat4(1.0f), m_Position);
            m_Matrix *= glm::mat4_cast(m_Rotation);
            m_Matrix = glm::scale(m_Matrix, m_Scale);
            m_Dirty = false;
        }

        Vec3 m_Position;
        Quat m_Rotation;
        Vec3 m_Scale;

        mutable Mat4 m_Matrix;
        mutable bool m_Dirty;
    };
}

#endif // ASHEN_TRANSFORM_H
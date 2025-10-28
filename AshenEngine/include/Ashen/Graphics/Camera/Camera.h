#ifndef ASHEN_CAMERA_H
#define ASHEN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Ashen/Math/Math.h"

namespace ash {
    class Camera {
    public:
        virtual ~Camera() = default;

        [[nodiscard]] virtual const Mat4& GetViewMatrix() const { return m_ViewMatrix; }
        [[nodiscard]] virtual const Mat4& GetProjectionMatrix() const = 0;

        [[nodiscard]] const Vec3& GetPosition() const { return m_Position; }
        [[nodiscard]] const Vec3& GetFront() const { return m_Front; }
        [[nodiscard]] const Vec3& GetUp() const { return m_Up; }
        [[nodiscard]] const Vec3& GetRight() const { return m_Right; }

        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        void SetPosition(const Vec3& pos) {
            m_Position = pos;
            RecalculateViewMatrix();
        }

        void LookAt(const Vec3& target, const Vec3& up = {0,1,0}) {
            m_Front = glm::normalize(target - m_Position);
            m_Right = glm::normalize(glm::cross(m_Front, up));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));
            RecalculateViewMatrix();
        }

    protected:
        Camera() = default;

        virtual void RecalculateViewMatrix() {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }

        Vec3 m_Position{0.f, 0.f, 0.f};
        Vec3 m_Front{0.f, 0.f, -1.f};
        Vec3 m_Up{0.f, 1.f, 0.f};
        Vec3 m_Right{1.f, 0.f, 0.f};

        Mat4 m_ViewMatrix{1.f};
        Mat4 m_ProjectionMatrix{1.f};
    };

    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float fov, float aspect, float nearClip, float farClip);

        void SetFOV(float fov);
        void SetClip(float nearClip, float farClip);

        void SetRotation(float yaw, float pitch);
        void Rotate(float yawOffset, float pitchOffset);

        void OnResize(uint32_t width, uint32_t height) override;

        [[nodiscard]] const Mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }

    private:
        void RecalculateProjectionMatrix();
        void UpdateVectors();

        float m_FOV, m_Aspect, m_NearClip, m_FarClip;
        float m_Yaw = -90.f;
        float m_Pitch = 0.f;
    };

    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetBounds(float left, float right, float bottom, float top);
        void OnResize(uint32_t width, uint32_t height) override;

        [[nodiscard]] const Mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }

    private:
        void RecalculateProjectionMatrix();

        float m_Left, m_Right, m_Bottom, m_Top;
    };
}

#endif // ASHEN_CAMERA_H
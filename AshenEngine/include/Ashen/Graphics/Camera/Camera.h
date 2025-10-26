#ifndef ASHEN_CAMERA_H
#define ASHEN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Ashen/Core/Logger.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Camera {
    public:
        virtual ~Camera() = default;

        [[nodiscard]] Mat4 GetViewMatrix() const {
            return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }

        [[nodiscard]] virtual Mat4 GetProjectionMatrix() const = 0;

        [[nodiscard]] Vec3 GetPosition() const { return m_Position; }
        [[nodiscard]] Vec3 GetFront() const { return m_Front; }
        [[nodiscard]] Vec3 GetUp() const { return m_Up; }
        [[nodiscard]] Vec3 GetRight() const { return m_Right; }

        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        void SetPosition(const Vec3 &position) { m_Position = position; }
        void Move(const Vec3 &offset) { m_Position += offset; }

        void LookAt(const Vec3 &target, const Vec3 &worldUp = {0.0f, 1.0f, 0.0f}) {
            m_Front = glm::normalize(target - m_Position);
            m_Right = glm::normalize(glm::cross(m_Front, worldUp));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        }

    protected:
        Camera() = default;

        // Commun à toutes les caméras
        Vec3 m_Position{0.0f, 0.0f, 0.0f};
        Vec3 m_Front{0.0f, 0.0f, -1.0f};
        Vec3 m_Up{0.0f, 1.0f, 0.0f};
        Vec3 m_Right{1.0f, 0.0f, 0.0f};
        Vec3 m_WorldUp{0.0f, 1.0f, 0.0f};

        Mat4 m_ProjectionMatrix{1.0f};

        void UpdateVectors(const float yaw, const float pitch) {
            Vec3 front;
            front.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
            front.y = static_cast<float>(sin(glm::radians(pitch)));
            front.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
            m_Front = glm::normalize(front);
            m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        }
    };

    class PerspectiveCamera final : public Camera {
    public:
        explicit PerspectiveCamera(
            const float fov = 45.0f,
            const float aspectRatio = 16.0f / 9.0f,
            const float nearPlane = 0.1f,
            const float farPlane = 1000.0f
        )
            : m_Fov(fov), m_AspectRatio(aspectRatio),
              m_Near(nearPlane), m_Far(farPlane) {
            UpdateVectors(m_Yaw, m_Pitch);
            UpdateProjectionMatrix();
        }

        [[nodiscard]] Mat4 GetProjectionMatrix() const override {
            return m_ProjectionMatrix;
        }

        void OnResize(const uint32_t width, const uint32_t height) override {
            m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            UpdateProjectionMatrix();
        }

        void SetRotation(const float yaw, const float pitch) {
            m_Yaw = yaw;
            m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
            UpdateVectors(m_Yaw, m_Pitch);
        }

        void Rotate(const float yawOffset, const float pitchOffset) {
            m_Yaw += yawOffset;
            m_Pitch = glm::clamp(m_Pitch + pitchOffset, -89.0f, 89.0f);
            UpdateVectors(m_Yaw, m_Pitch);
        }

        void SetFov(const float fov) {
            m_Fov = glm::clamp(fov, 1.0f, 120.0f);
            UpdateProjectionMatrix();
        }

        void SetClipPlanes(const float nearPlane, const float farPlane) {
            m_Near = nearPlane;
            m_Far = farPlane;
            UpdateProjectionMatrix();
        }

        [[nodiscard]] float GetYaw() const { return m_Yaw; }
        [[nodiscard]] float GetPitch() const { return m_Pitch; }
        [[nodiscard]] float GetFov() const { return m_Fov; }
        [[nodiscard]] float GetAspectRatio() const { return m_AspectRatio; }
        [[nodiscard]] float GetNear() const { return m_Near; }
        [[nodiscard]] float GetFar() const { return m_Far; }

    private:
        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;
        float m_Fov;
        float m_AspectRatio;
        float m_Near;
        float m_Far;

        void UpdateProjectionMatrix() {
            m_ProjectionMatrix = glm::perspective(
                glm::radians(m_Fov),
                m_AspectRatio,
                m_Near,
                m_Far
            );
        }
    };

    class OrthographicCamera final : public Camera {
    public:
        explicit OrthographicCamera(
            const float left = -10.0f,
            const float right = 10.0f,
            const float bottom = -10.0f,
            const float top = 10.0f,
            const float nearPlane = -1.0f,
            const float farPlane = 1.0f
        )
            : m_Left(left), m_Right(right),
              m_Bottom(bottom), m_Top(top),
              m_Near(nearPlane), m_Far(farPlane) {
            UpdateProjectionMatrix();
        }

        [[nodiscard]] Mat4 GetProjectionMatrix() const override {
            return m_ProjectionMatrix;
        }

        void OnResize(const uint32_t width, const uint32_t height) override {
            const float aspect = static_cast<float>(width) / static_cast<float>(height);

            const float centerX = (m_Left + m_Right) * 0.5f;
            const float centerY = (m_Top + m_Bottom) * 0.5f;

            const float halfHeight = (m_Top - m_Bottom) * 0.5f;
            const float halfWidth = halfHeight * aspect;

            m_Left = centerX - halfWidth;
            m_Right = centerX + halfWidth;
            m_Bottom = centerY - halfHeight;
            m_Top = centerY + halfHeight;

            UpdateProjectionMatrix();
        }

    private:
        float m_Left, m_Right, m_Bottom, m_Top;
        float m_Near, m_Far;

        void UpdateProjectionMatrix() {
            m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
        }
    };

    class UICamera final : public Camera {
    public:
        explicit UICamera(const float nearPlane = -1.0f, const float farPlane = 1.0f)
            : m_Near(nearPlane), m_Far(farPlane),
              m_Left(), m_Right(),
              m_Bottom(), m_Top() {
        }

        [[nodiscard]] Mat4 GetProjectionMatrix() const override {
            return m_ProjectionMatrix;
        }

        void OnResize(const uint32_t width, const uint32_t height) override {
            UpdateProjectionMatrix(width, height);
        }

    private:
        float m_Near, m_Far;

        void UpdateProjectionMatrix(const uint32_t width, const uint32_t height) {
            m_Left = 0.0f;
            m_Right = static_cast<float>(width);
            m_Bottom = 0.0f;
            m_Top = static_cast<float>(height);

            m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
        }

        float m_Left, m_Right, m_Bottom, m_Top;
    };
}

#endif // ASHEN_CAMERA_H
#ifndef ASHEN_CAMERA_H
#define ASHEN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Ashen/math/Math.h"

namespace ash {
    class Camera {
    public:
        virtual ~Camera() = default;

        [[nodiscard]] virtual Mat4 GetViewMatrix() const = 0;

        [[nodiscard]] virtual Mat4 GetProjectionMatrix() const = 0;

        [[nodiscard]] virtual Vec3 GetPosition() const = 0;

        [[nodiscard]] virtual Vec3 GetFront() const = 0;

        [[nodiscard]] virtual Vec3 GetUp() const = 0;

        [[nodiscard]] virtual Vec3 GetRight() const = 0;

        [[nodiscard]] Mat4 GetViewProjectionMatrix() const {
            return GetProjectionMatrix() * GetViewMatrix();
        }
    };

    class PerspectiveCamera final : public Camera {
    public:
        explicit PerspectiveCamera(const float fov = 45.0f, const float aspectRatio = 16.0f / 9.0f,
                                   const float nearPlane = 0.1f, const float farPlane = 1000.0f)
            : m_Fov(fov), m_AspectRatio(aspectRatio), m_Near(nearPlane), m_Far(farPlane) {
            UpdateVectors();
            UpdateProjectionMatrix();
        }

        [[nodiscard]] Mat4 GetViewMatrix() const override {
            return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }

        [[nodiscard]] Mat4 GetProjectionMatrix() const override {
            return m_ProjectionMatrix;
        }

        [[nodiscard]] Vec3 GetPosition() const override { return m_Position; }
        [[nodiscard]] Vec3 GetFront() const override { return m_Front; }
        [[nodiscard]] Vec3 GetUp() const override { return m_Up; }
        [[nodiscard]] Vec3 GetRight() const override { return m_Right; }

        void SetPosition(const Vec3 &position) { m_Position = position; }

        void SetRotation(const float yaw, const float pitch) {
            m_Yaw = yaw;
            m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
            UpdateVectors();
        }

        void Move(const Vec3 &offset) { m_Position += offset; }

        void Rotate(const float yawOffset, const float pitchOffset) {
            m_Yaw += yawOffset;
            m_Pitch = glm::clamp(m_Pitch + pitchOffset, -89.0f, 89.0f);
            UpdateVectors();
        }

        void SetFov(const float fov) {
            m_Fov = glm::clamp(fov, 1.0f, 120.0f);
            UpdateProjectionMatrix();
        }

        void SetAspectRatio(const float ratio) {
            m_AspectRatio = ratio;
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
        Vec3 m_Position{0.0f, 0.0f, 3.0f};
        Vec3 m_Front{0.0f, 0.0f, -1.0f};
        Vec3 m_Up{0.0f, 1.0f, 0.0f};
        Vec3 m_Right{1.0f, 0.0f, 0.0f};
        Vec3 m_WorldUp{0.0f, 1.0f, 0.0f};

        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;
        float m_Fov;
        float m_AspectRatio;
        float m_Near;
        float m_Far;

        Mat4 m_ProjectionMatrix{1.0f};

        void UpdateVectors() {
            Vec3 front;
            front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            front.y = sin(glm::radians(m_Pitch));
            front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            m_Front = glm::normalize(front);

            m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        }

        void UpdateProjectionMatrix() {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
        }
    };

    class OrthographicCamera final : public Camera {
    public:
        explicit OrthographicCamera(const float left = -10.0f, const float right = 10.0f,
                                    const float bottom = -10.0f, const float top = 10.0f,
                                    const float nearPlane = -1.0f, const float farPlane = 1.0f)
            : m_LeftBound(left), m_RightBound(right),
              m_BottomBound(bottom), m_TopBound(top),
              m_Near(nearPlane), m_Far(farPlane) {
            UpdateProjectionMatrix();
        }

        [[nodiscard]] Mat4 GetViewMatrix() const override {
            return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }

        [[nodiscard]] Mat4 GetProjectionMatrix() const override {
            return m_ProjectionMatrix;
        }

        [[nodiscard]] Vec3 GetPosition() const override { return m_Position; }
        [[nodiscard]] Vec3 GetFront() const override { return m_Front; }
        [[nodiscard]] Vec3 GetUp() const override { return m_Up; }
        [[nodiscard]] Vec3 GetRight() const override { return m_Right; }

        void SetPosition(const Vec3 &position) { m_Position = position; }
        void Move(const Vec3 &offset) { m_Position += offset; }

        void SetBounds(const float left, const float right, const float bottom, const float top) {
            m_LeftBound = left;
            m_RightBound = right;
            m_BottomBound = bottom;
            m_TopBound = top;
            UpdateProjectionMatrix();
        }

    private:
        Vec3 m_Position{0.0f, 0.0f, 0.0f};
        Vec3 m_Front{0.0f, 0.0f, -1.0f};
        Vec3 m_Up{0.0f, 1.0f, 0.0f};
        Vec3 m_Right{1.0f, 0.0f, 0.0f};

        float m_LeftBound, m_RightBound, m_BottomBound, m_TopBound, m_Near, m_Far;
        Mat4 m_ProjectionMatrix{1.0f};

        void UpdateProjectionMatrix() {
            m_ProjectionMatrix = glm::ortho(m_LeftBound, m_RightBound, m_BottomBound, m_TopBound, m_Near, m_Far);
        }
    };
}

#endif // ASHEN_CAMERA_H
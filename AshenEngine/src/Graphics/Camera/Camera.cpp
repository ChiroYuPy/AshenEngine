#include "Ashen/Graphics/Camera/Camera.h"

namespace ash {
    PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float nearClip, const float farClip)
        : m_FOV(fov), m_Aspect(aspect), m_NearClip(nearClip), m_FarClip(farClip) {
        UpdateVectors();
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::SetFOV(const float fov) {
        m_FOV = glm::clamp(fov, 1.f, 120.f);
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::SetClip(const float nearClip, const float farClip) {
        m_NearClip = nearClip;
        m_FarClip = farClip;
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::OnResize(const uint32_t width, const uint32_t height) {
        m_Aspect = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::Rotate(const float yawOffset, const float pitchOffset) {
        m_Yaw += yawOffset;
        m_Pitch = glm::clamp(m_Pitch + pitchOffset, -89.f, 89.f);
        UpdateVectors();
    }

    void PerspectiveCamera::SetRotation(const float yaw, const float pitch) {
        m_Yaw = yaw;
        m_Pitch = glm::clamp(pitch, -89.f, 89.f);
        UpdateVectors();
    }

    void PerspectiveCamera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_NearClip, m_FarClip);
    }

    void PerspectiveCamera::UpdateVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, {0, 1, 0}));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));

        RecalculateViewMatrix();
    }

    OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom, const float top)
            : m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top) {
        RecalculateProjectionMatrix();
    }

    void OrthographicCamera::SetBounds(const float left, const float right, const float bottom, const float top) {
        m_Left = left; m_Right = right;
        m_Bottom = bottom; m_Top = top;
        RecalculateProjectionMatrix();
    }

    void OrthographicCamera::OnResize(const uint32_t width, const uint32_t height) {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const float centerX = (m_Left + m_Right) * 0.5f;
        const float halfHeight = (m_Top - m_Bottom) * 0.5f;
        const float halfWidth = halfHeight * aspect;

        SetBounds(centerX - halfWidth, centerX + halfWidth, m_Bottom, m_Top);
    }

    void OrthographicCamera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, -1.f, 1.f);
    }

}

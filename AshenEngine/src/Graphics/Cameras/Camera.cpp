#include "Ashen/Graphics/Cameras/Camera.h"

namespace ash {
    Mat4 Camera::GetViewMatrix() const {
        if (m_ViewDirty) {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
            m_ViewDirty = false;
        }
        return m_ViewMatrix;
    }

    const Mat4& Camera::GetViewProjectionMatrix() const {
        if (m_VPDirty || m_ViewDirty || m_ProjectionDirty) {
            m_ViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
            m_VPDirty = false;
            m_FrustumDirty = true;
        }
        return m_ViewProjectionMatrix;
    }

    const Frustum& Camera::GetViewFrustum() const {
        if (m_FrustumDirty) {
            m_Frustum.ExtractFromViewProjection(GetViewProjectionMatrix());
            m_FrustumDirty = false;
        }
        return m_Frustum;
    }

    void Camera::SetPosition(const Vec3& pos) {
        if (m_Position != pos) {
            m_Position = pos;
            MarkViewDirty();
        }
    }

    void Camera::LookAt(const Vec3& target, const Vec3& up) {
        m_Front = glm::normalize(target - m_Position);
        m_Right = glm::normalize(glm::cross(m_Front, up));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        MarkViewDirty();
    }

    void Camera::SetClipPlanes(const float nearPlane, const float farPlane) {
        if (m_NearPlane != nearPlane || m_FarPlane != farPlane) {
            m_NearPlane = nearPlane;
            m_FarPlane = farPlane;
            MarkProjectionDirty();
        }
    }

    void Camera::MarkViewDirty() const {
        m_ViewDirty = true;
        m_VPDirty = true;
        m_FrustumDirty = true;
    }

    void Camera::MarkProjectionDirty() const {
        m_ProjectionDirty = true;
        m_VPDirty = true;
        m_FrustumDirty = true;
    }

    // ===== PerspectiveCamera =====
    PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float nearClip, const float farClip)
        : m_FieldOfView(fov), m_AspectRatio(aspect) {
        m_NearPlane = nearClip;
        m_FarPlane = farClip;

        // Orientation initiale : regarder vers -Z (front par défaut)
        UpdateVectorsFromOrientation();
    }

    Mat4 PerspectiveCamera::GetProjectionMatrix() const {
        if (m_ProjectionDirty) {
            m_ProjectionMatrix = glm::perspective(
                glm::radians(m_FieldOfView),
                m_AspectRatio,
                m_NearPlane,
                m_FarPlane
            );
            m_ProjectionDirty = false;
        }
        return m_ProjectionMatrix;
    }

    void PerspectiveCamera::OnResize(const uint32_t width, const uint32_t height) {
        if (height > 0) {
            SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        }
    }

    void PerspectiveCamera::SetFieldOfView(const float fov) {
        if (m_FieldOfView != fov) {
            m_FieldOfView = fov;
            MarkProjectionDirty();
        }
    }

    void PerspectiveCamera::SetAspectRatio(const float aspect) {
        if (m_AspectRatio != aspect) {
            m_AspectRatio = aspect;
            MarkProjectionDirty();
        }
    }

    void PerspectiveCamera::SetOrientation(const Quaternion& orientation) {
        m_Orientation = glm::normalize(orientation);
        UpdateVectorsFromOrientation();
    }

    void PerspectiveCamera::UpdateVectorsFromOrientation() {
        // Extraire les vecteurs directionnels depuis le quaternion
        Mat3 rotationMatrix = glm::mat3_cast(m_Orientation);

        m_Front = -rotationMatrix[2];  // -Z en forward
        m_Right = rotationMatrix[0];   // X en right
        m_Up = rotationMatrix[1];      // Y en up

        MarkViewDirty();
    }

    // ===== OrthographicCamera =====
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top,
                                           float nearClip, float farClip)
        : m_OrthoBoundsLeft(left), m_OrthoBoundsRight(right),
          m_OrthoBoundsBottom(bottom), m_OrthoBoundsTop(top),
          m_OriginalWidth(right - left), m_OriginalHeight(top - bottom),
          m_OriginMode(OriginMode::BottomLeft) {
        m_NearPlane = nearClip;
        m_FarPlane = farClip;

        // Pour 2D, regarder vers -Z
        m_Front = Vec3(0.0f, 0.0f, -1.0f);
        m_Up = Vec3(0.0f, 1.0f, 0.0f);
        m_Right = Vec3(1.0f, 0.0f, 0.0f);
    }

    OrthographicCamera::OrthographicCamera(float width, float height, OriginMode mode)
        : m_OriginalWidth(width), m_OriginalHeight(height), m_OriginMode(mode) {

        if (mode == OriginMode::BottomLeft) {
            // (0,0) en bas à gauche - Pour UI
            m_OrthoBoundsLeft = 0.0f;
            m_OrthoBoundsRight = width;
            m_OrthoBoundsBottom = 0.0f;
            m_OrthoBoundsTop = height;
        } else {
            // (0,0) au centre - Pour jeux 2D
            m_OrthoBoundsLeft = -width * 0.5f;
            m_OrthoBoundsRight = width * 0.5f;
            m_OrthoBoundsBottom = -height * 0.5f;
            m_OrthoBoundsTop = height * 0.5f;
        }

        m_NearPlane = -1.0f;
        m_FarPlane = 1.0f;

        // Pour 2D, regarder vers -Z
        m_Front = Vec3(0.0f, 0.0f, -1.0f);
        m_Up = Vec3(0.0f, 1.0f, 0.0f);
        m_Right = Vec3(1.0f, 0.0f, 0.0f);
    }

    Mat4 OrthographicCamera::GetProjectionMatrix() const {
        if (m_ProjectionDirty) {
            // Appliquer le zoom
            const float halfWidth = (m_OrthoBoundsRight - m_OrthoBoundsLeft) * 0.5f / m_Zoom;
            const float halfHeight = (m_OrthoBoundsTop - m_OrthoBoundsBottom) * 0.5f / m_Zoom;
            const float centerX = (m_OrthoBoundsLeft + m_OrthoBoundsRight) * 0.5f;
            const float centerY = (m_OrthoBoundsBottom + m_OrthoBoundsTop) * 0.5f;

            m_ProjectionMatrix = glm::ortho(
                centerX - halfWidth,
                centerX + halfWidth,
                centerY - halfHeight,
                centerY + halfHeight,
                m_NearPlane,
                m_FarPlane
            );

            // Appliquer rotation 2D si nécessaire
            if (m_Rotation2D != 0.0f) {
                m_ProjectionMatrix = glm::rotate(
                    m_ProjectionMatrix,
                    glm::radians(m_Rotation2D),
                    Vec3(0.0f, 0.0f, 1.0f)
                );
            }

            m_ProjectionDirty = false;
        }
        return m_ProjectionMatrix;
    }

    void OrthographicCamera::OnResize(const uint32_t width, const uint32_t height) {
        const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        const float originalAspect = m_OriginalWidth / m_OriginalHeight;

        if (aspectRatio > originalAspect) {
            const float newWidth = m_OriginalHeight * aspectRatio;
            SetSize(newWidth, m_OriginalHeight);
        } else {
            const float newHeight = m_OriginalWidth / aspectRatio;
            SetSize(m_OriginalWidth, newHeight);
        }
    }

    void OrthographicCamera::SetBounds(const float left, const float right, const float bottom, const float top) {
        if (m_OrthoBoundsLeft != left || m_OrthoBoundsRight != right ||
            m_OrthoBoundsBottom != bottom || m_OrthoBoundsTop != top) {
            m_OrthoBoundsLeft = left;
            m_OrthoBoundsRight = right;
            m_OrthoBoundsBottom = bottom;
            m_OrthoBoundsTop = top;
            m_OriginalWidth = right - left;
            m_OriginalHeight = top - bottom;
            MarkProjectionDirty();
        }
    }

    void OrthographicCamera::SetSize(const float width, const float height) {
        if (m_OriginMode == OriginMode::BottomLeft) {
            SetBounds(0.0f, width, 0.0f, height);
        } else {
            SetBounds(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f);
        }
    }

    void OrthographicCamera::SetZoom(float zoom) {
        zoom = glm::max(zoom, 0.01f);
        if (m_Zoom != zoom) {
            m_Zoom = zoom;
            MarkProjectionDirty();
        }
    }

    void OrthographicCamera::SetRotation2D(const float rotation) {
        if (m_Rotation2D != rotation) {
            m_Rotation2D = rotation;
            MarkProjectionDirty();
        }
    }

    void OrthographicCamera::Rotate2D(const float rotationOffset) {
        SetRotation2D(m_Rotation2D + rotationOffset);
    }
}
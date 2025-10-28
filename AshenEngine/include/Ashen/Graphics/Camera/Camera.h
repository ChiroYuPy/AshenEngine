#ifndef ASHEN_CAMERA_H
#define ASHEN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Frustum.h"
#include "Viewport.h"
#include "Ashen/Math/Math.h"

namespace ash {

    class Camera {
    public:
        virtual ~Camera() = default;

        // Méthodes virtuelles pures
        virtual Mat4 GetProjectionMatrix() const = 0;
        virtual void OnResize(uint32_t width, uint32_t height) = 0;
        virtual void Update(float deltaTime) {}

        // View matrix
        Mat4 GetViewMatrix() const;

        // View-Projection combinée (cached)
        const Mat4& GetViewProjectionMatrix() const;

        // Frustum culling
        const Frustum& GetViewFrustum() const;

        // Position et orientation
        const Vec3& GetPosition() const { return m_Position; }
        const Vec3& GetFront() const { return m_Front; }
        const Vec3& GetUp() const { return m_Up; }
        const Vec3& GetRight() const { return m_Right; }

        void SetPosition(const Vec3& pos);

        // Look-At (pour caméras statiques ou cinématiques)
        void LookAt(const Vec3& target, const Vec3& up = {0, 1, 0});

        // Near/Far planes
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }
        void SetClipPlanes(float nearPlane, float farPlane);

        // Viewport
        const Viewport& GetViewport() const { return m_Viewport; }
        void SetViewport(const Viewport& viewport) { m_Viewport = viewport; }

    protected:
        Camera() = default;
        Camera(const Camera&) = default;
        Camera& operator=(const Camera&) = default;

        void MarkViewDirty() const;
        void MarkProjectionDirty() const;

        // Données de transformation
        Vec3 m_Position{0.0f, 0.0f, 0.0f};
        Vec3 m_Front{0.0f, 0.0f, -1.0f};
        Vec3 m_Up{0.0f, 1.0f, 0.0f};
        Vec3 m_Right{1.0f, 0.0f, 0.0f};

        float m_NearPlane = 0.1f;
        float m_FarPlane = 1000.0f;

        // Matrices avec dirty flags
        mutable Mat4 m_ViewMatrix{1.0f};
        mutable Mat4 m_ProjectionMatrix{1.0f};
        mutable Mat4 m_ViewProjectionMatrix{1.0f};

        mutable bool m_ViewDirty = true;
        mutable bool m_ProjectionDirty = true;
        mutable bool m_VPDirty = true;

        // Frustum et viewport
        mutable Frustum m_Frustum;
        mutable bool m_FrustumDirty = true;
        Viewport m_Viewport;
    };

    class PerspectiveCamera final : public Camera {
    public:
        PerspectiveCamera(float fov = 60.0f, float aspect = 16.0f / 9.0f,
                         float nearClip = 0.1f, float farClip = 1000.0f);

        Mat4 GetProjectionMatrix() const override;
        void OnResize(uint32_t width, uint32_t height) override;

        // Configuration perspective
        void SetFieldOfView(float fov);
        void SetAspectRatio(float aspect);

        float GetFieldOfView() const { return m_FieldOfView; }
        float GetAspectRatio() const { return m_AspectRatio; }

        // Orientation via quaternion (plus robuste que Euler)
        void SetOrientation(const Quaternion& orientation);
        const Quaternion& GetOrientation() const { return m_Orientation; }

    private:
        void UpdateVectorsFromOrientation();

        float m_FieldOfView;
        float m_AspectRatio;
        Quaternion m_Orientation{1.0f, 0.0f, 0.0f, 0.0f};  // Identité (w, x, y, z)
    };

    class OrthographicCamera final : public Camera {
    public:
        enum class OriginMode {
            BottomLeft,  // Pour UI : (0,0) en bas à gauche
            Center       // Pour jeux 2D : (0,0) au centre
        };

        // Constructeur avec bounds explicites
        OrthographicCamera(float left, float right, float bottom, float top,
                          float nearClip = -1.0f, float farClip = 1.0f);

        // Constructeur simplifié (pour UI ou jeux 2D)
        OrthographicCamera(float width, float height, OriginMode mode = OriginMode::BottomLeft);

        Mat4 GetProjectionMatrix() const override;
        void OnResize(uint32_t width, uint32_t height) override;

        // Configuration orthographique
        void SetBounds(float left, float right, float bottom, float top);
        void SetSize(float width, float height);

        // Zoom (pour 2D)
        void SetZoom(float zoom);
        float GetZoom() const { return m_Zoom; }

        // Rotation 2D (autour de l'axe Z)
        void SetRotation2D(float rotation);
        void Rotate2D(float rotationOffset);
        float GetRotation2D() const { return m_Rotation2D; }

        // Récupérer les dimensions
        float GetWidth() const { return m_OrthoBoundsRight - m_OrthoBoundsLeft; }
        float GetHeight() const { return m_OrthoBoundsTop - m_OrthoBoundsBottom; }
        OriginMode GetOriginMode() const { return m_OriginMode; }

    private:
        float m_OrthoBoundsLeft, m_OrthoBoundsRight;
        float m_OrthoBoundsBottom, m_OrthoBoundsTop;
        float m_Zoom = 1.0f;
        float m_Rotation2D = 0.0f;  // En degrés
        float m_OriginalWidth, m_OriginalHeight;
        OriginMode m_OriginMode;  // Mode d'origine
    };
}

#endif // ASHEN_CAMERA_H
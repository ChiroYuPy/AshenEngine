#ifndef ASHEN_CAMERA2DCONTROLLER_H
#define ASHEN_CAMERA2DCONTROLLER_H

#include "CameraController.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

namespace ash {
    class Camera2DController final : public CameraController {
    public:
        static Ref<Camera2DController> Create(OrthographicCamera& camera, float moveSpeed = 300.0f, float zoomSpeed = 0.1f);

        void Update(float deltaTime) override;
        void OnEvent(Event& event) override;

        void FollowTarget(const Vec2& targetPos, float smoothing = 0.1f);
        void StopFollowing() { m_HasTarget = false; }

        void SetBounds(const Vec2& min, const Vec2& max);
        void ClearBounds() { m_HasBounds = false; }

        void SetMoveSpeed(const float speed) { m_MoveSpeed = speed; }
        void SetZoomSpeed(const float speed) { m_ZoomSpeed = speed; }
        void SetZoomRange(float min, float max);

    private:
        explicit Camera2DController(OrthographicCamera& camera, float moveSpeed, float zoomSpeed);

        float m_MoveSpeed;
        float m_ZoomSpeed;
        float m_MinZoom = 0.1f;
        float m_MaxZoom = 10.0f;

        bool m_HasTarget = false;
        Vec2 m_TargetPosition{0.0f};
        float m_FollowSmoothing = 0.1f;

        bool m_HasBounds = false;
        Vec2 m_BoundsMin{0.0f};
        Vec2 m_BoundsMax{0.0f};

        bool m_LeftPressed = false;
        bool m_RightPressed = false;
        bool m_UpPressed = false;
        bool m_DownPressed = false;

        void ApplyBounds();
        void HandleKeyEvent(const KeyEvent& keyEvent);
        void HandleMouseScrolledEvent(const MouseScrolledEvent& mouseScrolled);
    };
}

#endif //ASHEN_CAMERA2DCONTROLLER_H
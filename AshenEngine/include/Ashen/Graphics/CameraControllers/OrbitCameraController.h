#ifndef ASHEN_ORBITCAMERACONTROLLER_H
#define ASHEN_ORBITCAMERACONTROLLER_H

#include "CameraController.h"
#include "Ashen/Events/MouseEvent.h"

namespace ash {
    class OrbitCameraController final : public CameraController {
    public:
        static Ref<OrbitCameraController> Create(
            Camera& camera,
            Vec3 target = {0, 0, 0},
            float distance = 10.0f);

        void Update(float deltaTime) override;
        void OnEvent(Event& event) override;

        void SetTarget(const Vec3& target) { m_Target = target; }
        void SetDistance(float distance);
        void SetDistanceRange(float min, float max);
        void SetSensitivity(const float sensitivity) { m_Sensitivity = sensitivity; }
        void SetZoomSpeed(const float speed) { m_ZoomSpeed = speed; }

        const Vec3& GetTarget() const { return m_Target; }
        float GetDistance() const { return m_Distance; }

    private:
        explicit OrbitCameraController(Camera& camera,
                                       Vec3 target,
                                       float distance);

        Vec3 m_Target;
        float m_Distance;
        float m_MinDistance = 1.0f;
        float m_MaxDistance = 100.0f;

        float m_Theta = 0.0f;
        float m_Phi = 45.0f;
        float m_Sensitivity = 0.5f;
        float m_ZoomSpeed = 2.0f;

        bool m_IsDragging = false;
        Vec2 m_LastMousePos{0.0f};

        void UpdateCameraPosition();
        void HandleMouseButtonPressedEvent(const MouseButtonPressedEvent &mouseButton);
        void HandleMouseButtonReleasedEvent(const MouseButtonReleasedEvent &mouseButton);
        void HandleMouseMovedEvent(const MouseMovedEvent& mouseMoved);
        void HandleMouseScrolledEvent(const MouseScrolledEvent& mouseScrolled);
    };
}

#endif //ASHEN_ORBITCAMERACONTROLLER_H
#ifndef ASHEN_FPSCAMERACONTROLLER_H
#define ASHEN_FPSCAMERACONTROLLER_H

#include "Ashen/Graphics/CameraControllers/CameraController.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

namespace ash {
    class FPSCameraController final : public CameraController {
    public:
        static Ref<FPSCameraController> Create(PerspectiveCamera &camera, float sensitivity = 0.1f,
                                               float moveSpeed = 5.0f);

        void Update(float deltaTime) override;

        void OnEvent(Event &event) override;

        void SetSensitivity(const float sensitivity) { m_Sensitivity = sensitivity; }
        void SetMoveSpeed(const float speed) { m_MoveSpeed = speed; }
        void SetSprintMultiplier(const float multiplier) { m_SprintMultiplier = multiplier; }

        float GetYaw() const { return m_Yaw; }
        float GetPitch() const { return m_Pitch; }

        void SetOrientation(float yaw, float pitch);

    private:
        explicit FPSCameraController(PerspectiveCamera &camera, float sensitivity, float moveSpeed);

        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;
        float m_Sensitivity;
        float m_MoveSpeed;
        float m_SprintMultiplier = 2.0f;

        bool m_ForwardPressed = false;
        bool m_BackwardPressed = false;
        bool m_LeftPressed = false;
        bool m_RightPressed = false;
        bool m_UpPressed = false;
        bool m_DownPressed = false;
        bool m_SprintPressed = false;

        bool m_FirstMouse = true;
        Vec2 m_LastMousePos{0.0f};

        void UpdateCameraOrientation() const;

        void HandleKeyPressed(const KeyPressedEvent &keyEvent);

        void HandleKeyReleased(const KeyReleasedEvent &keyEvent);

        void HandleMouseMovedEvent(const MouseMovedEvent &mouseMoved);
    };
}

#endif //ASHEN_FPSCAMERACONTROLLER_H

#include "../../../include/Ashen/Graphics/CameraControllers/FPSCameraController.h"

#include "Ashen/Events/EventDispatcher.h"

namespace ash {
        Ref<FPSCameraController> FPSCameraController::Create(PerspectiveCamera& camera, const float sensitivity, const float moveSpeed) {
        return Ref<FPSCameraController>(new FPSCameraController(camera, sensitivity, moveSpeed));
    }

    FPSCameraController::FPSCameraController(PerspectiveCamera& camera, const float sensitivity, const float moveSpeed)
        : CameraController(camera), m_Sensitivity(sensitivity), m_MoveSpeed(moveSpeed) {
        UpdateCameraOrientation();
    }

    void FPSCameraController::OnEvent(Event& event) {
            if (!m_Enabled) return;

            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e) {
                HandleKeyPressed(e);
                return false;
            });

            dispatcher.Dispatch<KeyReleasedEvent>([this](const KeyReleasedEvent& e) {
                HandleKeyReleased(e);
                return false;
            });

            dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent& e) {
                HandleMouseMovedEvent(e);
                return false;
            });
    }

    void FPSCameraController::HandleKeyPressed(const KeyPressedEvent& keyEvent) {
        switch (keyEvent.GetKeyCode()) {
            case Key::W: case Key::Up:
                m_ForwardPressed = true; break;
            case Key::S: case Key::Down:
                m_BackwardPressed = true; break;
            case Key::A: case Key::Left:
                m_LeftPressed = true; break;
            case Key::D: case Key::Right:
                m_RightPressed = true; break;
            case Key::Space:
                m_UpPressed = true; break;
            case Key::LeftControl:
                m_DownPressed = true; break;
            case Key::LeftShift:
                m_SprintPressed = true; break;
            default: break;
        }
    }

    void FPSCameraController::HandleKeyReleased(const KeyReleasedEvent& keyEvent) {
        switch (keyEvent.GetKeyCode()) {
            case Key::W: case Key::Up:
                m_ForwardPressed = false; break;
            case Key::S: case Key::Down:
                m_BackwardPressed = false; break;
            case Key::A: case Key::Left:
                m_LeftPressed = false; break;
            case Key::D: case Key::Right:
                m_RightPressed = false; break;
            case Key::Space:
                m_UpPressed = false; break;
            case Key::LeftControl:
                m_DownPressed = false; break;
            case Key::LeftShift:
                m_SprintPressed = false; break;
            default: break;
        }
    }

    void FPSCameraController::HandleMouseMovedEvent(const MouseMovedEvent& mouseMoved) {
        const Vec2 currentMousePos = {mouseMoved.GetX(), mouseMoved.GetY()};

        if (m_FirstMouse) {
            m_LastMousePos = currentMousePos;
            m_FirstMouse = false;
            return;
        }

        const Vec2 delta = currentMousePos - m_LastMousePos;
        m_LastMousePos = currentMousePos;

        m_Yaw -= delta.x * m_Sensitivity;
        m_Pitch -= delta.y * m_Sensitivity;

        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

        UpdateCameraOrientation();
    }

    void FPSCameraController::Update(const float deltaTime) {
        if (!m_Enabled) return;

        Vec3 movement{0.0f};
        if (m_ForwardPressed)  movement += m_Camera.GetFront();
        if (m_BackwardPressed) movement -= m_Camera.GetFront();
        if (m_LeftPressed)     movement -= m_Camera.GetRight();
        if (m_RightPressed)    movement += m_Camera.GetRight();
        if (m_UpPressed)       movement += Vec3(0, 1, 0);
        if (m_DownPressed)     movement -= Vec3(0, 1, 0);

        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement);

            const float speed = m_MoveSpeed * (m_SprintPressed ? m_SprintMultiplier : 1.0f);
            m_Camera.SetPosition(m_Camera.GetPosition() + movement * speed * deltaTime);
        }
    }

    void FPSCameraController::SetOrientation(const float yaw, const float pitch) {
        m_Yaw = yaw;
        m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
        UpdateCameraOrientation();
    }

    void FPSCameraController::UpdateCameraOrientation() {
        auto* perspCamera = dynamic_cast<PerspectiveCamera*>(&m_Camera);
        if (!perspCamera) return;

        const Quaternion qYaw = glm::angleAxis(glm::radians(m_Yaw), Vec3(0, 1, 0));
        const Quaternion qPitch = glm::angleAxis(glm::radians(m_Pitch), Vec3(1, 0, 0));

        const Quaternion orientation = qYaw * qPitch;
        perspCamera->SetOrientation(orientation);
    }
}

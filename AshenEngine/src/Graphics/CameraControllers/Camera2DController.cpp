#include "../../../include/Ashen/Graphics/CameraControllers/Camera2DController.h"

#include "Ashen/Events/EventDispatcher.h"

namespace ash {
        Ref<Camera2DController> Camera2DController::Create(OrthographicCamera& camera, const float moveSpeed, const float zoomSpeed) {
        return Ref<Camera2DController>(new Camera2DController(camera, moveSpeed, zoomSpeed));
    }

    Camera2DController::Camera2DController(OrthographicCamera& camera, const float moveSpeed, const float zoomSpeed)
        : CameraController(camera), m_MoveSpeed(moveSpeed), m_ZoomSpeed(zoomSpeed) {}

    void Camera2DController::OnEvent(Event& event) {
        if (!m_Enabled) return;

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e) {
            HandleKeyEvent(e);
            return false;
        });

        dispatcher.Dispatch<KeyReleasedEvent>([this](const KeyReleasedEvent& e) {
            HandleKeyEvent(e);
            return false;
        });

        dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& e) {
            HandleMouseScrolledEvent(e);
            return false;
        });
    }

    void Camera2DController::HandleKeyEvent(const KeyEvent& keyEvent) {
        const bool pressed = keyEvent.GetType() == KeyPressedEvent::GetStaticType();

        switch (keyEvent.GetKeyCode()) {
            case Key::A: case Key::Left:
                m_LeftPressed = pressed;
                break;
            case Key::D: case Key::Right:
                m_RightPressed = pressed;
                break;
            case Key::S: case Key::Down:
                m_DownPressed = pressed;
                break;
            case Key::W: case Key::Up:
                m_UpPressed = pressed;
                break;
            default: break;
        }
    }

    void Camera2DController::HandleMouseScrolledEvent(const MouseScrolledEvent& mouseScrolled) {
        auto* orthoCamera = dynamic_cast<OrthographicCamera*>(&m_Camera);
        if (!orthoCamera) return;

        const float currentZoom = orthoCamera->GetZoom();
        float newZoom = currentZoom + mouseScrolled.GetYOffset() * m_ZoomSpeed;
        newZoom = glm::clamp(newZoom, m_MinZoom, m_MaxZoom);
        orthoCamera->SetZoom(newZoom);
    }

    void Camera2DController::Update(const float deltaTime) {
        if (!m_Enabled) return;

        auto* orthoCamera = dynamic_cast<OrthographicCamera*>(&m_Camera);
        if (!orthoCamera) return;

        Vec3 position = m_Camera.GetPosition();

        // Suivi de cible avec lissage
        if (m_HasTarget) {
            const Vec2 currentPos(position.x, position.y);
            const Vec2 targetDelta = m_TargetPosition - currentPos;
            const Vec2 newPos = currentPos + targetDelta * m_FollowSmoothing;
            position.x = newPos.x;
            position.y = newPos.y;
        }

        // Déplacement manuel (seulement si pas de cible)
        if (!m_HasTarget) {
            Vec2 movement{0.0f};
            if (m_LeftPressed)  movement.x -= 1.0f;
            if (m_RightPressed) movement.x += 1.0f;
            if (m_DownPressed)  movement.y -= 1.0f;
            if (m_UpPressed)    movement.y += 1.0f;

            if (glm::length(movement) > 0.0f) {
                movement = glm::normalize(movement);

                // Adapter la vitesse au zoom (plus on zoom, plus on se déplace lentement)
                const float currentZoom = orthoCamera->GetZoom();
                const float adjustedSpeed = m_MoveSpeed / currentZoom;

                position.x += movement.x * adjustedSpeed * deltaTime;
                position.y += movement.y * adjustedSpeed * deltaTime;
            }
        }

        m_Camera.SetPosition(position);

        // Appliquer les limites de position
        if (m_HasBounds) {
            ApplyBounds();
        }
    }

    void Camera2DController::FollowTarget(const Vec2& targetPos, const float smoothing) {
        m_TargetPosition = targetPos;
        m_FollowSmoothing = glm::clamp(smoothing, 0.01f, 1.0f);
        m_HasTarget = true;
    }

    void Camera2DController::SetBounds(const Vec2& min, const Vec2& max) {
        m_BoundsMin = min;
        m_BoundsMax = max;
        m_HasBounds = true;
    }

    void Camera2DController::SetZoomRange(const float min, const float max) {
        m_MinZoom = min;
        m_MaxZoom = max;
    }

    void Camera2DController::ApplyBounds() {
        Vec3 position = m_Camera.GetPosition();
        position.x = glm::clamp(position.x, m_BoundsMin.x, m_BoundsMax.x);
        position.y = glm::clamp(position.y, m_BoundsMin.y, m_BoundsMax.y);
        m_Camera.SetPosition(position);
    }
}

#include "../../../include/Ashen/Graphics/CameraControllers/OrbitCameraController.h"

#include "Ashen/Core/Input.h"
#include "Ashen/Events/EventDispatcher.h"

namespace ash {
    Ref<OrbitCameraController> OrbitCameraController::Create(Camera& camera, const Vec3 target, const float distance) {
        return Ref<OrbitCameraController>(new OrbitCameraController(camera, target, distance));
    }

    OrbitCameraController::OrbitCameraController(Camera& camera, const Vec3 target, const float distance)
        : CameraController(camera), m_Target(target), m_Distance(distance) {
        UpdateCameraPosition();
    }
    
    void OrbitCameraController::OnEvent(Event& event) {
        if (!m_Enabled) return;

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
            HandleMouseButtonPressedEvent(e);
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent& e) {
            HandleMouseButtonReleasedEvent(e);
            return false;
        });

        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent& e) {
            HandleMouseMovedEvent(e);
            return false;
        });

        dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& e) {
            HandleMouseScrolledEvent(e);
            return false;
        });
    }
    
    void OrbitCameraController::HandleMouseButtonPressedEvent(const MouseButtonPressedEvent& mouseButton) {
        if (mouseButton.GetMouseButton() == MouseButton::ButtonRight) {
            m_IsDragging = true;
            m_LastMousePos = Input::GetMousePosition();
        }
    }

    void OrbitCameraController::HandleMouseButtonReleasedEvent(const MouseButtonReleasedEvent& mouseButton) {
        if (mouseButton.GetMouseButton() == MouseButton::ButtonRight) {
            m_IsDragging = false;
        }
    }

    void OrbitCameraController::HandleMouseMovedEvent(const MouseMovedEvent& mouseMoved) {
        if (!m_IsDragging) return;
        
        const Vec2 currentPos = {mouseMoved.GetX(), mouseMoved.GetY()};
        const Vec2 delta = currentPos - m_LastMousePos;
        m_LastMousePos = currentPos;
        
        m_Theta += delta.x * m_Sensitivity;
        m_Phi -= delta.y * m_Sensitivity;

        m_Phi = glm::clamp(m_Phi, 1.0f, 179.0f);
        
        UpdateCameraPosition();
    }
    
    void OrbitCameraController::HandleMouseScrolledEvent(const MouseScrolledEvent& mouseScrolled) {
        m_Distance -= mouseScrolled.GetYOffset() * m_ZoomSpeed;
        m_Distance = glm::clamp(m_Distance, m_MinDistance, m_MaxDistance);
        UpdateCameraPosition();
    }
    
    void OrbitCameraController::Update(float deltaTime) {
        // Pas de logique continue pour orbital
    }
    
    void OrbitCameraController::UpdateCameraPosition() {
        const float thetaRad = glm::radians(m_Theta);
        const float phiRad = glm::radians(m_Phi);

        Vec3 position;
        position.x = m_Target.x + m_Distance * sin(phiRad) * cos(thetaRad);
        position.y = m_Target.y + m_Distance * cos(phiRad);
        position.z = m_Target.z + m_Distance * sin(phiRad) * sin(thetaRad);
        
        m_Camera.SetPosition(position);
        m_Camera.LookAt(m_Target);
    }
    
    void OrbitCameraController::SetDistance(const float distance) {
        m_Distance = glm::clamp(distance, m_MinDistance, m_MaxDistance);
        UpdateCameraPosition();
    }
    
    void OrbitCameraController::SetDistanceRange(const float min, const float max) {
        m_MinDistance = min;
        m_MaxDistance = max;
        m_Distance = glm::clamp(m_Distance, m_MinDistance, m_MaxDistance);
    }
}
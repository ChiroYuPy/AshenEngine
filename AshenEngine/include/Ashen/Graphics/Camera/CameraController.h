#ifndef ASHEN_CAMERACONTROLLER_H
#define ASHEN_CAMERACONTROLLER_H

#include "Ashen/Core/Input.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Camera/Camera.h"

namespace ash {
    class CameraController {
    public:
        explicit CameraController(const Ref<PerspectiveCamera> &camera,
                                  const float moveSpeed = 5.0f,
                                  const float sensitivity = 0.1f)
            : m_camera(camera), m_moveSpeed(moveSpeed),
              m_mouseSensitivity(sensitivity), m_isActive(false) {
            // Calculer yaw et pitch depuis la direction actuelle de la caméra
            const Vec3 front = m_camera->GetFront();
            m_yaw = glm::degrees(atan2(front.z, front.x));
            m_pitch = glm::degrees(asin(front.y));

            m_lastMouseX = 0.0f;
            m_lastMouseY = 0.0f;
            m_firstMouse = true;
        }

        void OnUpdate(const float deltaTime) const {
            if (!m_isActive) return;
            // Récupération des vecteurs de direction
            const Vec3 cameraFront = m_camera->GetFront();
            const Vec3 cameraRight = m_camera->GetRight();
            const Vec3 cameraUp = m_camera->GetUp();

            Vec3 position = m_camera->GetPosition();
            const float velocity = m_moveSpeed * deltaTime;

            // Déplacements WASD
            if (Input::IsKeyPressed(KeyCode::W))
                position += cameraFront * velocity;
            if (Input::IsKeyPressed(KeyCode::S))
                position -= cameraFront * velocity;
            if (Input::IsKeyPressed(KeyCode::A))
                position -= cameraRight * velocity;
            if (Input::IsKeyPressed(KeyCode::D))
                position += cameraRight * velocity;
            if (Input::IsKeyPressed(KeyCode::Space))
                position += cameraUp * velocity;
            if (Input::IsKeyPressed(KeyCode::LeftShift))
                position -= cameraUp * velocity;

            // Déplacements verticaux Q/E
            if (Input::IsKeyPressed(KeyCode::Q))
                position.y -= velocity;
            if (Input::IsKeyPressed(KeyCode::E))
                position.y += velocity;

            m_camera->SetPosition(position);
        }

        void OnMouseMove(const float xpos, const float ypos) {
            if (!m_isActive) return;

            if (m_firstMouse) {
                m_lastMouseX = xpos;
                m_lastMouseY = ypos;
                m_firstMouse = false;
                return; // Sortir ici pour éviter le saut initial
            }

            float xoffset = xpos - m_lastMouseX;
            float yoffset = m_lastMouseY - ypos;
            m_lastMouseX = xpos;
            m_lastMouseY = ypos;

            xoffset *= m_mouseSensitivity;
            yoffset *= m_mouseSensitivity;

            m_yaw += xoffset;
            m_pitch += yoffset;

            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;

            UpdateCameraDirection();
        }

        void OnMouseScroll(const float yoffset) {
            if (!m_isActive) return;
            m_moveSpeed += yoffset * 0.5f;
            constexpr float minSpeed = 1.f / 16.f;
            constexpr float maxSpeed = 16.f;
            if (m_moveSpeed < minSpeed) m_moveSpeed = minSpeed;
            if (m_moveSpeed > maxSpeed) m_moveSpeed = maxSpeed;
        }

        void SetActive(const bool active) {
            m_isActive = active;
            if (active) {
                m_firstMouse = true;
            }
        }

        bool IsActive() const { return m_isActive; }

    private:
        void UpdateCameraDirection() const {
            Vec3 direction;
            direction.x = static_cast<float>(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
            direction.y = static_cast<float>(sin(glm::radians(m_pitch)));
            direction.z = static_cast<float>(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));

            const Vec3 target = m_camera->GetPosition() + glm::normalize(direction);
            m_camera->LookAt(target);
        }

        Ref<PerspectiveCamera> m_camera;
        float m_moveSpeed;
        float m_mouseSensitivity;
        float m_yaw;
        float m_pitch;
        float m_lastMouseX;
        float m_lastMouseY;
        bool m_firstMouse;
        bool m_isActive;
    };
}


#endif //ASHEN_CAMERACONTROLLER_H
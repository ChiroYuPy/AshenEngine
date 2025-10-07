#ifndef ASHEN_CAMERACONTROLLER_H
#define ASHEN_CAMERACONTROLLER_H

#include "Ashen/Core/Input.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Camera/Camera.h"

namespace ash {
    class CameraController {
    public:
        explicit CameraController(const Ref<PerspectiveCamera> &camera, const float moveSpeed = 5.0f, const float sensitivity = 0.1f)
            : m_camera(camera), m_moveSpeed(moveSpeed), m_mouseSensitivity(sensitivity) {
            m_yaw = -90.0f;
            m_pitch = 0.0f;
            m_lastMouseX = 0.0f;
            m_lastMouseY = 0.0f;
            m_firstMouse = true;
        }

        void OnUpdate(const float deltaTime) const {
            // Récupération des vecteurs de direction
            const Vec3 cameraFront = m_camera->GetFront();
            const Vec3 cameraRight = glm::normalize(glm::cross(cameraFront, Vec3(0, 1, 0)));

            Vec3 position = m_camera->GetPosition();
            const float velocity = m_moveSpeed * deltaTime;

            // Déplacements WASD
            if (Input::IsKeyPressed(Key::W))
                position += cameraFront * velocity;
            if (Input::IsKeyPressed(Key::S))
                position -= cameraFront * velocity;
            if (Input::IsKeyPressed(Key::A))
                position -= cameraRight * velocity;
            if (Input::IsKeyPressed(Key::D))
                position += cameraRight * velocity;

            // Déplacements verticaux Q/E
            if (Input::IsKeyPressed(Key::Q))
                position.y -= velocity;
            if (Input::IsKeyPressed(Key::E))
                position.y += velocity;

            m_camera->SetPosition(position);
        }

        void OnMouseMove(const float xpos, const float ypos) {
            if (m_firstMouse) {
                m_lastMouseX = xpos;
                m_lastMouseY = ypos;
                m_firstMouse = false;
            }

            float xoffset = xpos - m_lastMouseX;
            float yoffset = m_lastMouseY - ypos; // Inversé: y va de bas en haut
            m_lastMouseX = xpos;
            m_lastMouseY = ypos;

            xoffset *= m_mouseSensitivity;
            yoffset *= m_mouseSensitivity;

            m_yaw += xoffset;
            m_pitch += yoffset;

            // Contraintes pour éviter les flips
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;

            UpdateCameraDirection();
        }

        void OnMouseScroll(const float yoffset) {
            m_moveSpeed += yoffset * 0.5f;
            if (m_moveSpeed < 1.0f)
                m_moveSpeed = 1.0f;
            if (m_moveSpeed > 50.0f)
                m_moveSpeed = 50.0f;
        }

    private:
        void UpdateCameraDirection() const {
            Vec3 direction;
            direction.x =   static_cast<float>(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
            direction.y =   static_cast<float>(sin(glm::radians(m_pitch)));
            direction.z =   static_cast<float>(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));

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
    };
}


#endif //ASHEN_CAMERACONTROLLER_H
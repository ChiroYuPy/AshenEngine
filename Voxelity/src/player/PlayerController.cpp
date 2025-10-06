#include "Voxelity/player/PlayerController.h"

#include "Ashen/Core/Input.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Core/Logger.h"

namespace voxelity {
    PlayerController::PlayerController(std::shared_ptr<ash::PerspectiveCamera> camera)
        : m_camera(std::move(camera)), m_wantsJump(false), m_active(false) {
    }

    void PlayerController::update(const float deltaTime) {
        if (!m_active) {
            m_movementInput = glm::vec3(0.0f);
            m_wantsJump = false;
            return;
        }

        updateMovementInput();
        updateCameraRotation(deltaTime);
    }

    void PlayerController::handleEvent(ash::Event &event) {
        if (!m_active) return;

        ash::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<ash::KeyPressedEvent>([this](const ash::KeyPressedEvent &e) {
            if (e.GetKeyCode() == ash::Key::Space)
                m_wantsJump = true;

            return false;
        });
    }

    void PlayerController::updateMovementInput() {
        m_movementInput = glm::vec3(0.0f);
        m_wantsJump = false;

        // Calculer la direction de la caméra sur le plan horizontal
        glm::vec3 forward = m_camera->GetFront();
        forward.y = 0.0f;
        if (glm::length(forward) > 0.001f) {
            forward = glm::normalize(forward);
        }

        const glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

        // Calculer le mouvement basé sur les inputs
        glm::vec3 moveDir(0.0f);
        if (ash::Input::IsKeyPressed(ash::Key::W)) moveDir += forward;
        if (ash::Input::IsKeyPressed(ash::Key::S)) moveDir -= forward;
        if (ash::Input::IsKeyPressed(ash::Key::A)) moveDir -= right;
        if (ash::Input::IsKeyPressed(ash::Key::D)) moveDir += right;

        // Normaliser pour éviter le mouvement diagonal plus rapide
        if (glm::length(moveDir) > 0.001f) {
            moveDir = glm::normalize(moveDir);
        }

        // Déterminer la vitesse selon l'état (Minecraft style)
        float speed = m_settings.walkSpeed;
        if (ash::Input::IsKeyPressed(ash::Key::LeftControl)) {
            // Sprint : seulement si on avance (W)
            if (ash::Input::IsKeyPressed(ash::Key::W)) {
                speed = m_settings.sprintSpeed;
            }
        } else if (ash::Input::IsKeyPressed(ash::Key::LeftShift)) {
            speed = m_settings.crouchSpeed;
        }

        m_movementInput = moveDir * speed;

        // Saut
        if (ash::Input::IsKeyPressed(ash::Key::Space)) {
            m_wantsJump = true;
        }
    }

    void PlayerController::updateCameraRotation(const float deltaTime) const {
        const glm::vec2 mouseDelta = ash::Input::GetMouseDelta();
        const glm::vec2 offset = mouseDelta * m_settings.mouseSensitivity;

        if (glm::length(offset) > 0.001f) {
            m_camera->Rotate(offset.x, -offset.y);
        }
    }
}
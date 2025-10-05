#include "Voxelity/player/PlayerController.h"

#include "Ashen/core/Input.h"
#include "Ashen/events/KeyEvent.h"

#include "Ashen/core/Logger.h"

namespace voxelity {
    PlayerController::PlayerController(std::shared_ptr<pixl::PerspectiveCamera> camera)
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

    void PlayerController::handleEvent(pixl::Event &event) {
        if (!m_active) return;

        pixl::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<pixl::KeyPressedEvent>([this](const pixl::KeyPressedEvent &e) {
            if (e.GetKeyCode() == pixl::Key::Space)
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
        if (pixl::Input::IsKeyPressed(pixl::Key::W)) moveDir += forward;
        if (pixl::Input::IsKeyPressed(pixl::Key::S)) moveDir -= forward;
        if (pixl::Input::IsKeyPressed(pixl::Key::A)) moveDir -= right;
        if (pixl::Input::IsKeyPressed(pixl::Key::D)) moveDir += right;

        // Normaliser pour éviter le mouvement diagonal plus rapide
        if (glm::length(moveDir) > 0.001f) {
            moveDir = glm::normalize(moveDir);
        }

        // Déterminer la vitesse selon l'état
        float speed = m_settings.walkSpeed;
        if (pixl::Input::IsKeyPressed(pixl::Key::LeftControl)) {
            speed = m_settings.sprintSpeed;
        } else if (pixl::Input::IsKeyPressed(pixl::Key::LeftShift)) {
            speed = m_settings.crouchSpeed;
        }

        m_movementInput = moveDir * speed;

        // Saut
        if (pixl::Input::IsKeyPressed(pixl::Key::Space)) {
            m_wantsJump = true;
        }
    }

    void PlayerController::updateCameraRotation(const float deltaTime) const {
        const glm::vec2 mouseDelta = pixl::Input::GetMouseDelta();
        const glm::vec2 offset = mouseDelta * m_settings.mouseSensitivity;

        if (glm::length(offset) > 0.001f) {
            m_camera->Rotate(offset.x, -offset.y);
        }
    }
}

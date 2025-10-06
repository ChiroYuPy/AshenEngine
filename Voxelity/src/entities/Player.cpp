#include "Voxelity/entities/Player.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Ashen/core/Input.h"

namespace voxelity {
    Player::Player(std::shared_ptr<ash::PerspectiveCamera> camera)
        : m_camera(std::move(camera)) {
        boundingBoxSize = glm::vec3(0.6f, 1.8f, 0.6f);
        useGravity = true;
        hasCollisions = true;

        m_controller = std::make_unique<PlayerController>(m_camera);

        PhysicsConfig physicsConfig;
        physicsConfig.gravity = -32.0f;
        physicsConfig.terminalVelocity = -78.4f;
        physicsConfig.groundFriction = 0.546f;
        physicsConfig.airDrag = 0.98f;
        physicsConfig.horizontalAirDrag = 0.91f;
        m_physics = std::make_unique<PhysicsSystem>(physicsConfig);
    }

    void Player::update(const float deltaTime, const World &world) {
        if (!isActive) return;

        // Sauvegarder l'état actuel avant modification (pour interpolation)
        saveState();

        // 1. Gérer le mouvement basé sur les inputs
        handleMovement(deltaTime);

        // 2. Appliquer la physique
        if (m_isFlying) {
            position += velocity * deltaTime;
            onGround = false;
        } else {
            m_physics->step(*this, deltaTime, world);
        }
    }

    void Player::updateVisuals(const float alpha) {
        if (!isActive) return;

        // Mise à jour du contrôleur (rotation caméra, inputs)
        // Note: deltaTime n'est plus utilisé pour les visuels
        m_controller->update(0.0f);

        // Position de la caméra avec interpolation
        updateCameraPosition(alpha);
    }

    void Player::handleMovement(const float deltaTime) {
        const glm::vec3 input = m_controller->getMovementInput();

        if (m_isFlying) {
            float flySpeed = m_flySpeed;

            if (ash::Input::IsKeyPressed(ash::Key::LeftControl)) {
                flySpeed *= 2.0f;
            }

            velocity.x = input.x * flySpeed;
            velocity.z = input.z * flySpeed;

            velocity.y = 0.0f;
            if (ash::Input::IsKeyPressed(ash::Key::Space)) {
                velocity.y = flySpeed;
            }
            if (ash::Input::IsKeyPressed(ash::Key::LeftShift)) {
                velocity.y = -flySpeed;
            }
        } else {
            velocity.x = input.x;
            velocity.z = input.z;

            if (m_controller->wantsToJump() && onGround) {
                jump();
            }
        }
    }

    void Player::jump() {
        if (onGround) {
            velocity.y = m_jumpForce;
        }
    }

    void Player::updateCameraPosition(const float alpha) const {
        // Interpoler la position pour un rendu fluide
        const glm::vec3 interpolatedPos = getInterpolatedPosition(alpha);

        constexpr float eyeHeight = 0.85f;
        const float eyeY = interpolatedPos.y - (boundingBoxSize.y * 0.5f) + (boundingBoxSize.y * eyeHeight);
        const auto cameraPos = glm::vec3(interpolatedPos.x, eyeY, interpolatedPos.z);
        m_camera->SetPosition(cameraPos);
    }
}

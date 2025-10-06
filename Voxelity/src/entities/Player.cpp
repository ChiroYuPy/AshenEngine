#include "Voxelity/entities/Player.h"

#include "Voxelity/voxelWorld/world/World.h"
#include "Ashen/core/Input.h"

namespace voxelity {
    Player::Player(std::shared_ptr<ash::PerspectiveCamera> camera)
        : m_camera(std::move(camera)) {
        // Configuration de l'entité - dimensions standard Minecraft
        boundingBoxSize = glm::vec3(0.6f, 1.8f, 0.6f);
        useGravity = true;
        hasCollisions = true;

        // Initialisation du contrôleur et de la physique
        m_controller = std::make_unique<PlayerController>(m_camera);

        // Configuration physique Minecraft (exacte Java Edition)
        PhysicsConfig physicsConfig;
        physicsConfig.gravity = -32.0f;              // -0.08 blocks/tick
        physicsConfig.terminalVelocity = -78.4f;     // -3.92 blocks/tick
        physicsConfig.groundFriction = 0.546f;       // 0.91 * 0.6
        physicsConfig.airDrag = 0.98f;               // Vertical air drag
        physicsConfig.horizontalAirDrag = 0.91f;     // Horizontal air drag
        m_physics = std::make_unique<PhysicsSystem>(physicsConfig);
    }

    void Player::update(const float deltaTime, const World &world) {
        if (!isActive) return;

        // Fixed timestep : logique et physique seulement
        // 1. Gérer le mouvement basé sur les inputs
        handleMovement(deltaTime);

        // 2. Appliquer la physique (seulement si pas en mode vol)
        if (m_isFlying) {
            // En mode vol, pas de gravité ni de collisions
            position += velocity * deltaTime;
            onGround = false;
        } else {
            // Mode normal : physique complète
            m_physics->step(*this, deltaTime, world);
        }
    }

    void Player::updateVisuals(const float deltaTime) {
        if (!isActive) return;

        // Mise à jour chaque frame pour la fluidité
        // 1. Controller (rotation caméra, inputs)
        m_controller->update(deltaTime);

        // 2. Position de la caméra
        updateCameraPosition();
    }

    void Player::handleMovement(const float deltaTime) {
        const glm::vec3 input = m_controller->getMovementInput();

        if (m_isFlying) {
            // Mode vol Minecraft : mouvement horizontal plan + montée/descente
            float flySpeed = m_flySpeed;

            // Sprint en vol (comme Minecraft)
            if (ash::Input::IsKeyPressed(ash::Key::LeftControl)) {
                flySpeed *= 2.0f;
            }

            // Mouvement horizontal (WASD)
            velocity.x = input.x * flySpeed;
            velocity.z = input.z * flySpeed;

            // Montée/descente : Space pour monter, Shift pour descendre
            velocity.y = 0.0f;
            if (ash::Input::IsKeyPressed(ash::Key::Space)) {
                velocity.y = flySpeed;
            }
            if (ash::Input::IsKeyPressed(ash::Key::LeftShift)) {
                velocity.y = -flySpeed;
            }
        } else {
            // Mode normal : mouvement horizontal + gravité
            velocity.x = input.x;
            velocity.z = input.z;

            // Gérer le saut
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

    void Player::updateCameraPosition() const {
        // Placer la caméra à hauteur des yeux du joueur
        constexpr float eyeHeight = 0.85f; // 85% de la hauteur
        const float eyeY = position.y - (boundingBoxSize.y * 0.5f) + (boundingBoxSize.y * eyeHeight);
        const auto cameraPos = glm::vec3(position.x, eyeY, position.z);
        m_camera->SetPosition(cameraPos);
    }
}
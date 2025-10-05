#include "Voxelity/entities/Player.h"

#include "Voxelity/voxelWorld/world/World.h"

namespace voxelity {
    Player::Player(std::shared_ptr<ash::PerspectiveCamera> camera)
        : m_camera(std::move(camera)) {
        // Configuration de l'entité - dimensions standard Minecraft
        boundingBoxSize = glm::vec3(0.6f, 1.8f, 0.6f);
        useGravity = true;
        hasCollisions = true;

        // Initialisation du contrôleur et de la physique
        m_controller = std::make_unique<PlayerController>(m_camera);

        // Configuration physique réaliste
        PhysicsConfig physicsConfig;
        physicsConfig.gravity = -32.0f;
        physicsConfig.groundFriction = 0.6f;
        physicsConfig.airDrag = 0.02f;
        m_physics = std::make_unique<PhysicsSystem>(physicsConfig);
    }

    void Player::update(const float deltaTime, const World &world) {
        if (!isActive) return;

        // 1. Mettre à jour le contrôleur (inputs)
        m_controller->update(deltaTime);

        // 2. Gérer le mouvement basé sur les inputs
        handleMovement(deltaTime);

        // 3. Appliquer la physique (un seul appel qui gère tout)
        m_physics->step(*this, deltaTime, world);

        // 4. Mettre à jour la position de la caméra
        updateCameraPosition();
    }

    void Player::handleMovement(const float deltaTime) {
        const glm::vec3 input = m_controller->getMovementInput();

        // Appliquer le mouvement horizontal uniquement
        // La physique gère la gravité automatiquement
        velocity.x = input.x;
        velocity.z = input.z;

        // Gérer le saut
        if (m_controller->wantsToJump() && onGround) {
            jump();
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
#ifndef VOXELITY_PLAYER_H
#define VOXELITY_PLAYER_H

#include "Entity.h"
#include <memory>

#include "Ashen/core/Types.h"
#include "Ashen/renderer/Camera.h"
#include "Voxelity/player/PlayerController.h"
#include "Voxelity/systems/PhysicsSystem.h"

namespace voxelity {
    class Player final : public Entity {
    public:
        explicit Player(ash::Ref<ash::PerspectiveCamera> camera);

        void update(float deltaTime, const World &world) override;

        // Mise à jour visuelle (appelée chaque frame)
        void updateVisuals(float deltaTime);

        // Gestion du contrôleur
        PlayerController &getController() { return *m_controller; }
        const PlayerController &getController() const { return *m_controller; }

        // Caméra
        ash::Ref<ash::PerspectiveCamera> getCamera() const { return m_camera; }

        // Actions
        void jump();

        // Configuration
        void setJumpForce(const float force) { m_jumpForce = force; }
        float getJumpForce() const { return m_jumpForce; }

        // Mode vol
        void toggleFly() { m_isFlying = !m_isFlying; }
        bool isFlying() const { return m_isFlying; }

    private:
        ash::Ref<ash::PerspectiveCamera> m_camera;
        ash::Scope<PlayerController> m_controller;
        ash::Scope<PhysicsSystem> m_physics;

        float m_jumpForce = 10.0f;       // Jump velocity Minecraft (permet de sauter ~1.25 blocs)
        bool m_isFlying = false;
        float m_flySpeed = 10.92f;       // Fly speed Minecraft (10.92 blocks/s)

        void updateCameraPosition() const;

        void handleMovement(float deltaTime);
    };
}

#endif //VOXELITY_PLAYER_H
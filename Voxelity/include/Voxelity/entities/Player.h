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

    private:
        ash::Ref<ash::PerspectiveCamera> m_camera;
        ash::Scope<PlayerController> m_controller;
        ash::Scope<PhysicsSystem> m_physics;

        float m_jumpForce = 9.0f;

        void updateCameraPosition() const;

        void handleMovement(float deltaTime);
    };
}

#endif //VOXELITY_PLAYER_H
#ifndef VOXELITY_PLAYERCONTROLLER_H
#define VOXELITY_PLAYERCONTROLLER_H

#include <memory>

#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Events/Event.h"

namespace voxelity {
    class PlayerController {
    public:
        struct MovementSettings {
            float walkSpeed = 4.317f; // Minecraft walking speed (4.317 blocks/s)
            float sprintSpeed = 5.612f; // Minecraft sprinting speed (5.612 blocks/s)
            float crouchSpeed = 1.295f; // Minecraft sneaking speed (1.295 blocks/s)
            float jumpForce = 8.0f;
            float mouseSensitivity = 0.33f;
        };

        explicit PlayerController(ash::Ref<ash::PerspectiveCamera> camera);

        void update(float deltaTime);

        void handleEvent(ash::Event &event);

        // Récupérer le mouvement calculé
        glm::vec3 getMovementInput() const { return m_movementInput; }
        bool wantsToJump() const { return m_wantsJump; }

        void setActive(const bool active) { m_active = active; }
        bool isActive() const { return m_active; }

        void setSettings(const MovementSettings &settings) { m_settings = settings; }
        const MovementSettings &getSettings() const { return m_settings; }

        ash::Ref<ash::PerspectiveCamera> getCamera() const { return m_camera; }

    private:
        ash::Ref<ash::PerspectiveCamera> m_camera;
        MovementSettings m_settings;

        glm::vec3 m_movementInput{0.0f};
        bool m_wantsJump;
        bool m_active;

        void updateMovementInput();

        void updateCameraRotation(float deltaTime) const;
    };
}

#endif //VOXELITY_PLAYERCONTROLLER_H
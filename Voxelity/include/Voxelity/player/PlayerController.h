#ifndef VOXELITY_PLAYERCONTROLLER_H
#define VOXELITY_PLAYERCONTROLLER_H

#include <memory>

#include "Ashen/core/Types.h"
#include "Ashen/renderer/Camera.h"
#include "Ashen/events/Event.h"

namespace voxelity {
    class PlayerController {
    public:
        struct MovementSettings {
            float walkSpeed = 5.0f;
            float sprintSpeed = 10.0f;
            float crouchSpeed = 2.5f;
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
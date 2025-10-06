#ifndef VOXELITY_INPUTHANDLER_H
#define VOXELITY_INPUTHANDLER_H

#include "Ashen/Events/Event.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

namespace voxelity {
    class PlayerController;
    class WorldInteractor;
    class VoxelWorldLayer;

    // Classe dédiée à la gestion des inputs de l'application
    class InputHandler {
    public:
        InputHandler(VoxelWorldLayer &layer,
                     PlayerController &playerController,
                     WorldInteractor &worldInteractor);

        void handleEvent(ash::Event &event);

    private:
        VoxelWorldLayer &m_layer;
        PlayerController &m_playerController;
        WorldInteractor &m_worldInteractor;

        void handleKeyPress(const ash::KeyPressedEvent &event);

        void handleMouseButton(const ash::MouseButtonPressedEvent &event) const;

        void handleMouseScroll(const ash::MouseScrolledEvent &event) const;
    };
}

#endif //VOXELITY_INPUTHANDLER_H
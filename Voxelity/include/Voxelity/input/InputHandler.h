#ifndef VOXELITY_INPUTHANDLER_H
#define VOXELITY_INPUTHANDLER_H

#include "Ashen/events/Event.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/events/MouseEvent.h"

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

        void handleEvent(pixl::Event &event);

    private:
        VoxelWorldLayer &m_layer;
        PlayerController &m_playerController;
        WorldInteractor &m_worldInteractor;

        void handleKeyPress(const pixl::KeyPressedEvent &event);

        void handleMouseButton(const pixl::MouseButtonPressedEvent &event) const;

        void handleMouseScroll(const pixl::MouseScrolledEvent &event) const;
    };
}

#endif //VOXELITY_INPUTHANDLER_H
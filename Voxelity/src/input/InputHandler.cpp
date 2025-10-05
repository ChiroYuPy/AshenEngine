#include "Voxelity/input/InputHandler.h"
#include "Voxelity/voxelWorld/world/WorldInteractor.h"
#include "Voxelity/layers/VoxelWorldLayer.h"
#include "Voxelity/VoxelityApp.h"
#include "Ashen/core/Input.h"
#include "Ashen/core/Logger.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/events/MouseEvent.h"
#include "Voxelity/player/PlayerController.h"

namespace voxelity {
    InputHandler::InputHandler(VoxelWorldLayer &layer,
                               PlayerController &playerController,
                               WorldInteractor &worldInteractor)
        : m_layer(layer)
          , m_playerController(playerController)
          , m_worldInteractor(worldInteractor) {
    }

    void InputHandler::handleEvent(pixl::Event &event) {
        pixl::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<pixl::KeyPressedEvent>([this](const pixl::KeyPressedEvent &e) {
            handleKeyPress(e);
            return true;
        });

        dispatcher.Dispatch<pixl::MouseButtonPressedEvent>([this](const pixl::MouseButtonPressedEvent &e) {
            handleMouseButton(e);
            return true;
        });

        dispatcher.Dispatch<pixl::MouseScrolledEvent>([this](const pixl::MouseScrolledEvent &e) {
            handleMouseScroll(e);
            return true;
        });

        // Transmettre aussi au contrôleur du joueur
        m_playerController.handleEvent(event);
    }

    void InputHandler::handleKeyPress(const pixl::KeyPressedEvent &event) {
        // Échap : désactiver caméra ou quitter
        if (event.GetKeyCode() == pixl::Key::Escape) {
            if (m_playerController.isActive()) {
                pixl::Input::SetCursorMode(pixl::CursorMode::Normal);
                m_playerController.setActive(false);
            } else {
                VoxelityApp::Get().Stop();
            }
        }

        // Debug : ajuster l'espacement des chunks (si exposé)
        // if (event.GetKeyCode() == pixl::Key::Y) { ... }
        // if (event.GetKeyCode() == pixl::Key::U) { ... }
    }

    void InputHandler::handleMouseButton(const pixl::MouseButtonPressedEvent &event) const {
        const glm::vec3 cameraPos = m_playerController.getCamera()->GetPosition();
        const glm::vec3 cameraDir = m_playerController.getCamera()->GetFront();

        if (event.GetMouseButton() == pixl::Mouse::ButtonLeft) {
            if (m_playerController.isActive()) {
                // Casser un bloc
                if (m_worldInteractor.BreakBlock(cameraPos, cameraDir)) {
                    pixl::Logger::info() << "Block broken!";
                } else {
                    pixl::Logger::info() << "No block to break";
                }
            } else {
                // Activer le mode caméra
                pixl::Input::SetCursorMode(pixl::CursorMode::Disabled);
                m_playerController.setActive(true);
            }
        }

        if (event.GetMouseButton() == pixl::Mouse::ButtonRight && m_playerController.isActive()) {
            // Placer un bloc
            if (m_worldInteractor.PlaceBlock(cameraPos, cameraDir)) {
                pixl::Logger::info() << "Block placed!";
            } else {
                pixl::Logger::info() << "Cannot place block here";
            }
        }
    }

    void InputHandler::handleMouseScroll(const pixl::MouseScrolledEvent &event) const {
        static VoxelType voxelID = 0;
        voxelID += static_cast<VoxelType>(event.GetYOffset());
        m_worldInteractor.setSelectedVoxelID(voxelID);
        pixl::Logger::info() << "block selected: " + getDisplayName(voxelID);
    }
}

#include "Voxelity/input/InputHandler.h"

#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/EventDispatcher.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

#include "Voxelity/voxelWorld/world/WorldInteractor.h"
#include "Voxelity/VoxelityApp.h"
#include "Voxelity/entities/Player.h"
#include "Voxelity/player/PlayerController.h"
#include "Voxelity/layers/VoxelWorldLayer.h"

namespace voxelity {
    InputHandler::InputHandler(VoxelWorldLayer &layer,
                               PlayerController &playerController,
                               WorldInteractor &worldInteractor)
        : m_layer(layer)
          , m_playerController(playerController)
          , m_worldInteractor(worldInteractor) {
    }

    void InputHandler::handleEvent(ash::Event &event) {
        ash::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<ash::KeyPressedEvent>([this](const ash::KeyPressedEvent &e) {
            handleKeyPress(e);
            return true;
        });

        dispatcher.Dispatch<ash::MouseButtonPressedEvent>([this](const ash::MouseButtonPressedEvent &e) {
            handleMouseButton(e);
            return true;
        });

        dispatcher.Dispatch<ash::MouseScrolledEvent>([this](const ash::MouseScrolledEvent &e) {
            handleMouseScroll(e);
            return true;
        });

        // Transmettre aussi au contrôleur du joueur
        m_playerController.handleEvent(event);
    }

    void InputHandler::handleKeyPress(const ash::KeyPressedEvent &event) {
        // Échap : désactiver caméra ou quitter
        if (event.GetKeyCode() == ash::Key::Escape) {
            if (m_playerController.isActive()) {
                ash::Input::SetCursorMode(ash::CursorMode::Normal);
                m_playerController.setActive(false);
                // IMPORTANT : Reset le delta pour éviter les sauts
                ash::Input::ResetMouseDelta();
            } else {
                VoxelityApp::Get().Stop();
            }
        }

        // F : toggle mode vol
        if (event.GetKeyCode() == ash::Key::F && m_playerController.isActive()) {
            m_layer.getPlayer().toggleFly();
            const bool isFlying = m_layer.getPlayer().isFlying();
            ash::Logger::Info() << (isFlying ? "Flying mode enabled" : "Flying mode disabled");
        }
    }

    void InputHandler::handleMouseButton(const ash::MouseButtonPressedEvent &event) const {
        const glm::vec3 cameraPos = m_playerController.getCamera()->GetPosition();
        const glm::vec3 cameraDir = m_playerController.getCamera()->GetFront();

        if (event.GetMouseButton() == ash::MouseButton::Left) {
            if (m_playerController.isActive()) {
                // Casser un bloc
                if (m_worldInteractor.BreakBlock(cameraPos, cameraDir)) {
                    ash::Logger::Info() << "Block broken!";
                } else {
                    ash::Logger::Info() << "No block to break";
                }
            } else {
                // Activer le mode caméra
                ash::Input::SetCursorMode(ash::CursorMode::Captured);
                // IMPORTANT : Reset le delta pour éviter les sauts
                ash::Input::ResetMouseDelta();
                m_playerController.setActive(true);
            }
        }

        if (event.GetMouseButton() == ash::MouseButton::Right && m_playerController.isActive()) {
            // Placer un bloc
            if (m_worldInteractor.PlaceBlock(cameraPos, cameraDir)) {
                ash::Logger::Info() << "Block placed!";
            } else {
                ash::Logger::Info() << "Cannot place block here";
            }
        }
    }

    void InputHandler::handleMouseScroll(const ash::MouseScrolledEvent &event) const {
        static VoxelType voxelID = 0;
        voxelID += static_cast<VoxelType>(event.GetYOffset());
        m_worldInteractor.setSelectedVoxelID(voxelID);
        ash::Logger::Info() << "block selected: " + getDisplayName(voxelID);
    }
}
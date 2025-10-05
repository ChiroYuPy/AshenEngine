#include "Voxelity/layers/UILayer.h"

#include "Voxelity/VoxelityApp.h"
#include "Ashen/core/Logger.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer2D.h"

namespace voxelity {
    UILayer::UILayer() {
        m_UICamera = std::make_shared<ash::OrthographicCamera>();
        const ash::Window &window = VoxelityApp::Get().GetWindow();
        m_UICamera->SetBounds(0.0f, static_cast<float>(window.GetWidth()),
                              0.0f, static_cast<float>(window.GetHeight()));
    }

    void UILayer::OnRender() {
        ash::RenderCommand::EnableDepthTest(false);
        ash::RenderCommand::EnableBlending(true);

        const ash::Window &window = VoxelityApp::Get().GetWindow();
        const auto windowWidth = static_cast<float>(window.GetWidth());
        const auto windowHeight = static_cast<float>(window.GetHeight());

        ash::Renderer2D::BeginScene(*m_UICamera);

        // Dessin du crosshair au centre
        const ash::Vec2 center = {windowWidth / 2.0f, windowHeight / 2.0f};
        ash::Renderer2D::DrawLine(center - ash::Vec2{10, 0}, center + ash::Vec2{10, 0}, {1, 1, 1, 1});
        ash::Renderer2D::DrawLine(center - ash::Vec2{0, 10}, center + ash::Vec2{0, 10}, {1, 1, 1, 1});

        // Dessin de l'inventaire uniquement si ouvert
        if (m_ShowInventory) {
            constexpr int columns = 9;
            constexpr int rows = 6;
            constexpr float slotSize = 64.0f;
            constexpr float slotSpacing = 10.0f;

            constexpr float inventoryWidth = columns * slotSize + (columns - 1) * slotSpacing;
            constexpr float inventoryHeight = rows * slotSize + (rows - 1) * slotSpacing;

            // Coin supérieur gauche pour centrer l'inventaire
            const float startX = (windowWidth - inventoryWidth) / 2.0f;
            const float startY = (windowHeight - inventoryHeight) / 2.0f;

            // Fond semi-transparent
            ash::Renderer2D::DrawQuad({startX + inventoryWidth / 2.0f, startY + inventoryHeight / 2.0f},
                                      {inventoryWidth, inventoryHeight},
                                      {0, 0, 0, 0.5f});

            // Slots
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < columns; ++x) {
                    const float posX = startX + static_cast<float>(x) * (slotSize + slotSpacing) + slotSize / 2.0f;
                    const float posY = startY + static_cast<float>(y) * (slotSize + slotSpacing) + slotSize / 2.0f;
                    ash::Renderer2D::DrawQuad({posX, posY}, {slotSize, slotSize}, {0.5, 0.5, 0.5, 0.7});
                }
            }
        }

        ash::Renderer2D::EndScene();
    }

    void UILayer::OnEvent(ash::Event &event) {
        ash::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<ash::KeyPressedEvent>([this](const ash::KeyPressedEvent &e) {
            if (e.GetKeyCode() == ash::Key::I)
                m_ShowInventory = !m_ShowInventory;

            return false;
        });
    }
}
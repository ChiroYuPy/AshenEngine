#include "Voxelity/layers/UILayer.h"

#include "Voxelity/VoxelityApp.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer2D.h"
#include "Ashen/renderer/Renderer3D.h"
#include "Ashen/resources/ResourceManager.h"

namespace voxelity {
    UILayer::UILayer() : m_3DCrosshair(false) {
        m_UICamera = std::make_shared<ash::UICamera>();

        m_CrosshairTexture = ash::AssetLibrary::Textures().Load("textures/crosshairs/crosshair.png");
    }

    void UILayer::OnRender() {
        ash::RenderCommand::EnableDepthTest(false);
        ash::RenderCommand::EnableBlending(true);

        const ash::Window &window = VoxelityApp::Get().GetWindow();
        const auto windowWidth = static_cast<float>(window.GetWidth());
        const auto windowHeight = static_cast<float>(window.GetHeight());
        const ash::Vec2 center = {windowWidth / 2.0f, windowHeight / 2.0f};

        ash::Renderer2D::BeginScene(*m_UICamera);

        if (m_CrosshairTexture) {
            if (m_3DCrosshair) {

            } else {
                constexpr float crosshairSize = 64.0f;
                ash::Renderer2D::DrawQuad(center, {crosshairSize, crosshairSize}, m_CrosshairTexture, {1,1,1,1});
            }
        }

        ash::Renderer2D::EndScene();
    }

    void UILayer::OnEvent(ash::Event &event) {
        ash::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<ash::KeyPressedEvent>([this](const ash::KeyPressedEvent &e) {
            if (e.GetKeyCode() == ash::Key::F3)
                m_3DCrosshair = !m_3DCrosshair;
            return false;
        });

        dispatcher.Dispatch<ash::WindowResizeEvent>([this](const ash::WindowResizeEvent &e) {
            m_UICamera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });
    }
}

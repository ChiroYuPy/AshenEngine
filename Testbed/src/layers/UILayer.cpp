#include "layers/UILayer.h"

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Graphics/UI/UIWidgets.h"

namespace ash {
    void UILayer::OnAttach() {
        m_SceneTree = MakeOwn<SceneTree>();
        m_RootNode = m_SceneTree->GetRoot();

        const auto panel = MakeRef<Panel>();
        panel->SetPosition({100, 100});
        panel->SetSize({200, 200});
        panel->SetBgColor({1.f, 0.f, 0.f, 0.5f});
        m_RootNode->AddChild(panel);
    }

    void UILayer::OnUpdate(const float deltaTime) {

    }

    void UILayer::OnRender() {

    }

    void UILayer::OnEvent(Event &event) {

    }
}

#include "layers/SceneTestLayer.h"

#include "Ashen/Core/Codes.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/ECS/Component.h"

namespace ash {

    SceneTestLayer::SceneTestLayer()
    : Layer("SceneTestLayer"), m_Scene(MakeOwn<Scene>()) {}

    void SceneTestLayer::OnAttach() {

    }

    void SceneTestLayer::OnDetach() {
        m_Scene->Clear();
    }

    void SceneTestLayer::OnUpdate(const float deltaTime) {
        m_Scene->OnUpdate(deltaTime);

        auto entities = m_Scene->GetEntitiesWith<TransformComponent, SpriteRendererComponent>();
    }

    void SceneTestLayer::OnRender() {
        m_Scene->OnRender();
    }

    void SceneTestLayer::OnEvent(Event &event) {
        m_Scene->OnEvent(event);
    }
}

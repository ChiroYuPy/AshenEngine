#include "EditorLayer.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include <imgui.h>
#include <algorithm>

#include "Ashen/Scene/SceneTree.h"

namespace ash {

    EditorLayer::EditorLayer() : Layer() {
        m_ActiveScene = MakeRef<SceneTree>();
    }

    void EditorLayer::OnAttach() {
        Logger::Info("Editor Layer attached");

        const auto player = MakeRef<Node>("Player");
        const auto camera = MakeRef<Node>("Camera");
        const auto light = MakeRef<Node>("DirectionalLight");

        m_ActiveScene->GetRoot()->AddChild(player);
        m_ActiveScene->GetRoot()->AddChild(camera);
        m_ActiveScene->GetRoot()->AddChild(light);

        // Add child to player
        const auto mesh = MakeRef<Node>("Mesh");
        player->AddChild(mesh);
    }

    void EditorLayer::OnDetach() {
        Logger::Info("Editor Layer detached");
    }

    void EditorLayer::OnUpdate(const float ts) {

    }

    void EditorLayer::OnRender() {

    }

    void EditorLayer::OnEvent(Event& event) {

    }
}
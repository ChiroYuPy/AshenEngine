#include "layers/NodeGraphTestLayer.h"

#include "Ashen/Core/Application.h"

namespace ash {
    void NodeGraphTestLayer::OnAttach() {
        const Application& app = Application::Get();
        NodeGraph& ng = app.GetNodeGraph();
        ng.Clear();
        ng.SetRoot(MakeOwn<Node>("Root"));
        ng.GetRoot()->AddChild(MakeOwn<Node>("Child"));
    }

    void NodeGraphTestLayer::OnUpdate(const float deltaTime) {

    }

    void NodeGraphTestLayer::OnRender() {

    }

    void NodeGraphTestLayer::OnEvent(Event &event) {

    }
}

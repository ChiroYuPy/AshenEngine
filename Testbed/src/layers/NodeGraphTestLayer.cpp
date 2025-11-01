#include "layers/NodeGraphTestLayer.h"

#include "Ashen/Core/Application.h"
#include "Ashen/Nodes/Widgets.h"

namespace ash {
    void NodeGraphTestLayer::OnAttach() {
        const Application& app = Application::Get();
        const NodeGraph& ng = app.GetNodeGraph();

        ng.GetRoot()->AddChild(MakeOwn<Panel>("Child"));
    }

    void NodeGraphTestLayer::OnUpdate(const float deltaTime) {

    }

    void NodeGraphTestLayer::OnRender() {

    }

    void NodeGraphTestLayer::OnEvent(Event &event) {

    }
}

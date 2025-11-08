#include "layers/NodeGraphTestLayer.h"

#include "Ashen/Core/Application.h"
#include "Ashen/Nodes/Widgets.h"

namespace ash {
    void NodeGraphTestLayer::OnAttach() {
        const Application& app = Application::Get();
        const NodeGraph& ng = app.GetNodeGraph();

        Own<Panel> panel = MakeOwn<Panel>("Child");
        panel->background_color = {0.8f, 0.2f, 0.2f, 1.0f};
        panel->size = {100, 100};
        panel->position = {100, 100};

        ng.GetRoot()->AddChild(MovePtr(panel));
    }

    void NodeGraphTestLayer::OnUpdate(const float deltaTime) {

    }

    void NodeGraphTestLayer::OnRender() {

    }

    void NodeGraphTestLayer::OnEvent(Event &event) {

    }
}

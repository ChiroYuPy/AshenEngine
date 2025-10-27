#ifndef ASHEN_UILAYER_H
#define ASHEN_UILAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Scene/SceneTree.h"

namespace ash {
    class UILayer final : public Layer {
    public:
        void OnAttach() override;

        void OnUpdate(float deltaTime) override;

        void OnRender() override;

        void OnEvent(Event &event) override;

    private:
        Own<SceneTree> m_SceneTree;
        Ref<Node> m_RootNode;
    };
}

#endif //ASHEN_UILAYER_H
#ifndef ASHEN_EDITOR_LAYER_H
#define ASHEN_EDITOR_LAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Scene/Node.h"

namespace ash {

    class EditorLayer final : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnRender() override;
        void OnEvent(Event& event) override;

    private:
        Ref<SceneTree> m_ActiveScene;
    };

}

#endif // ASHEN_EDITOR_LAYER_H
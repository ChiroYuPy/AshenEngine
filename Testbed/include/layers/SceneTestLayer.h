#ifndef ASHEN_SCENETESTLAYER_H
#define ASHEN_SCENETESTLAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/ECS/Scene.h"

namespace ash {
    class SceneTestLayer final : public Layer {
    public:
        explicit SceneTestLayer();

        void OnAttach() override;

        void OnDetach() override;

        void OnUpdate(float deltaTime) override;

        void OnRender() override;

        void OnEvent(Event &event) override;

    private:
        Own<Scene> m_Scene;
    };
}

#endif //ASHEN_SCENETESTLAYER_H
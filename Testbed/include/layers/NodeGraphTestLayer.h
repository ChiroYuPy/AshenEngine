#ifndef ASHEN_NODEGRAPHTESTLAYER_H
#define ASHEN_NODEGRAPHTESTLAYER_H

#include "Ashen/Core/Layer.h"

namespace ash {
    class NodeGraphTestLayer final : public Layer {
        public:
            void OnAttach() override;

            void OnUpdate(float deltaTime) override;

            void OnRender() override;

            void OnEvent(Event &event) override;
        };
    }

#endif //ASHEN_NODEGRAPHTESTLAYER_H
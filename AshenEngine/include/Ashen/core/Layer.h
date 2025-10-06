#ifndef ASHEN_LAYER_H
#define ASHEN_LAYER_H

#include "Ashen/UI/Widget.h"
#include "Ashen/events/Event.h"

namespace ash {
    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {
        }

        virtual void OnDetach() {
        }

        virtual void OnUpdate(const float ts) {
        }

        virtual void OnRender() {
        }

        virtual void OnEvent(Event &event) {
        }
    };
}

#endif //ASHEN_LAYER_H
#ifndef ASHEN_LAYER_H
#define ASHEN_LAYER_H

#include "Ashen/Events/Event.h"

namespace ash {
    class Layer {
    public:
        explicit Layer(String name = "Layer") : m_Name(MovePtr(name)) {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float ts) {}
        virtual void OnRender() {}
        virtual void OnEvent(Event &event) {}

        String GetName() const { return m_Name; }

    protected:
        String m_Name;
    };
}

#endif //ASHEN_LAYER_H
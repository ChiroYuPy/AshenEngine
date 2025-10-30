#include "Ashen/Core/LayerStack.h"

#include <ranges>

namespace ash {
    LayerStack::~LayerStack() {
        Clear();
    }

    void LayerStack::PushLayer(Own<Layer> layer) {
        if (layer) {
            layer->OnAttach();
            m_Layers.push_back(MovePtr(layer));
        }
    }

    void LayerStack::PopLayer() {
        if (!m_Layers.empty()) {
            m_Layers.back()->OnDetach();
            m_Layers.pop_back();
        }
    }

    void LayerStack::Clear() {
        for (auto &layer: m_Layers)
            if (layer) layer->OnDetach();

        m_Layers.clear();
    }
}

#include "Ashen/Core/LayerStack.h"

#include <ranges>

namespace ash {
    LayerStack::~LayerStack() {
        Clear();
    }

    void LayerStack::PopLayer() {
        if (!m_Layers.empty()) {
            m_Layers.back()->OnDetach();
            m_Layers.pop_back();
        }
    }

    void LayerStack::Clear() {
        for (const auto &layer: m_Layers)
            layer->OnDetach();

        m_Layers.clear();
    }

    void LayerStack::OnEvent(Event &event) {
        for (const auto &m_Layer: std::ranges::reverse_view(m_Layers)) {
            m_Layer->OnEvent(event);
            if (event.handled) break;
        }
    }
}
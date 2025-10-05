#include "Ashen/core/LayerStack.h"
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
        for (const auto &layer: std::ranges::reverse_view(m_Layers)) {
            layer->OnEvent(event);
            if (event.Handled) break;
        }
    }
}

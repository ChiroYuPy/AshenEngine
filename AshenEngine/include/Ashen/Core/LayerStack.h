#ifndef ASHEN_LAYERSTACK_H
#define ASHEN_LAYERSTACK_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"

namespace ash {
    class LayerStack final {
    public:
        LayerStack() = default;
        ~LayerStack();

        LayerStack(const LayerStack&) = delete;
        LayerStack& operator=(const LayerStack&) = delete;

        void PushLayer(Own<Layer> layer);
        void PopLayer();
        void Clear();

        size_t Size() const noexcept { return m_Layers.size(); }
        bool Empty() const noexcept { return m_Layers.empty(); }

        auto begin() noexcept { return m_Layers.begin(); }
        auto end() noexcept { return m_Layers.end(); }
        auto begin() const noexcept { return m_Layers.begin(); }
        auto end() const noexcept { return m_Layers.end(); }

        auto rbegin() noexcept { return m_Layers.rbegin(); }
        auto rend() noexcept { return m_Layers.rend(); }
        auto rbegin() const noexcept { return m_Layers.rbegin(); }
        auto rend() const noexcept { return m_Layers.rend(); }

    private:
        Vector<Own<Layer>> m_Layers;
    };
}

#endif //ASHEN_LAYERSTACK_H
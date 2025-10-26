#ifndef ASHEN_LAYERSTACK_H
#define ASHEN_LAYERSTACK_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"

namespace ash {
    class LayerStack {
    public:
        LayerStack() = default;

        ~LayerStack();

        LayerStack(const LayerStack &) = delete;

        LayerStack &operator=(const LayerStack &) = delete;

        template<typename T, typename... Args>
            requires std::is_base_of_v<Layer, T>
        T *PushLayer(Args &&... args) {
            auto layer = MakeOwn<T>(std::forward<Args>(args)...);
            T *ptr = layer.get();
            layer->OnAttach();
            m_Layers.push_back(MovePtr(layer));
            return ptr;
        }

        void PopLayer();

        void Clear();

        void OnEvent(Event &event);

        [[nodiscard]] size_t Size() const { return m_Layers.size(); }
        [[nodiscard]] bool Empty() const { return m_Layers.empty(); }

        auto begin() { return m_Layers.begin(); }
        auto end() { return m_Layers.end(); }
        [[nodiscard]] auto begin() const { return m_Layers.begin(); }
        [[nodiscard]] auto end() const { return m_Layers.end(); }

        auto rbegin() { return m_Layers.rbegin(); }
        auto rend() { return m_Layers.rend(); }
        [[nodiscard]] auto rbegin() const { return m_Layers.rbegin(); }
        [[nodiscard]] auto rend() const { return m_Layers.rend(); }

    private:
        Vector<Own<Layer> > m_Layers;
    };
}

#endif //ASHEN_LAYERSTACK_H
#ifndef ASHEN_EVENTBUS_H
#define ASHEN_EVENTBUS_H

#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <ranges>

#include "Ashen/Events/Event.h"
#include "Ashen/Core/Types.h"

namespace ash {
    using ListenerID = size_t;

    class EventBus {
    public:
        using EventCallback = Function<bool(Event &)>;

        EventBus() = default;

        ~EventBus() = default;

        template<EventLike T>
        [[nodiscard]] ListenerID Subscribe(Function<bool(T &)> callback) {
            const EventType type = T::GetStaticType();
            const ListenerID id = m_NextID++;

            EventCallback wrapper = [callback = std::move(callback)](Event &e) -> bool {
                return callback(static_cast<T &>(e));
            };

            m_Listeners[type].emplace_back(id, std::move(wrapper));
            return id;
        }

        [[nodiscard]] ListenerID SubscribeToCategory(const EventCategory category, EventCallback callback) {
            const ListenerID id = m_NextID++;
            m_CategoryListeners[category].emplace_back(id, std::move(callback));
            return id;
        }

        void Unsubscribe(ListenerID id) {
            for (auto &listeners: m_Listeners | std::views::values)
                std::erase_if(listeners, [id](const auto &pair) { return pair.first == id; });

            for (auto &listeners: m_CategoryListeners | std::views::values)
                std::erase_if(listeners, [id](const auto &pair) { return pair.first == id; });
        }

        void Publish(Event &event) {
            if (auto it = m_Listeners.find(event.GetType()); it != m_Listeners.end())
                for (auto &callback: it->second | std::views::values)
                    if (callback(event)) {
                        event.handled = true;
                        break;
                    }


            for (auto &[category, listeners]: m_CategoryListeners)
                if (event.IsInCategory(category))
                    for (auto &callback: listeners | std::views::values)
                        if (callback(event)) {
                            event.handled = true;
                            return;
                        }
        }

        void Queue(Own<Event> event) {
            m_EventQueue.push_back(std::move(event));
        }

        void ProcessQueue() {
            for (auto &event: m_EventQueue)
                Publish(*event);

            m_EventQueue.clear();
        }

        void Clear() {
            m_Listeners.clear();
            m_CategoryListeners.clear();
            m_EventQueue.clear();
        }

        [[nodiscard]] size_t GetListenerCount() const {
            size_t count = 0;
            for (const auto &listeners: m_Listeners | std::views::values)
                count += listeners.size();

            for (const auto &listeners: m_CategoryListeners | std::views::values)
                count += listeners.size();

            return count;
        }

        [[nodiscard]] size_t GetQueueSize() const {
            return m_EventQueue.size();
        }

    private:
        using ListenerPair = Pair<ListenerID, EventCallback>;

        HashMap<EventType, Vector<ListenerPair> > m_Listeners;
        HashMap<EventCategory, Vector<ListenerPair> > m_CategoryListeners;
        Vector<Own<Event> > m_EventQueue;
        ListenerID m_NextID = 0;
    };

    class ScopedListener {
    public:
        ScopedListener(EventBus &bus, const ListenerID id) : m_Bus(&bus), m_ID(id) {
        }

        ~ScopedListener() {
            if (m_Bus) m_Bus->Unsubscribe(m_ID);
        }

        ScopedListener(const ScopedListener &) = delete;

        ScopedListener &operator=(const ScopedListener &) = delete;

        ScopedListener(ScopedListener &&other) noexcept
            : m_Bus(other.m_Bus), m_ID(other.m_ID) {
            other.m_Bus = nullptr;
        }

        ScopedListener &operator=(ScopedListener &&other) noexcept {
            if (this != &other) {
                if (m_Bus) m_Bus->Unsubscribe(m_ID);

                m_Bus = other.m_Bus;
                m_ID = other.m_ID;
                other.m_Bus = nullptr;
            }
            return *this;
        }

    private:
        EventBus *m_Bus;
        ListenerID m_ID;
    };

    template<EventLike T>
    [[nodiscard]] ScopedListener MakeScopedListener(
        EventBus & bus,
        Function < bool(T &) > callback
    ) {
        ListenerID id = bus.Subscribe < T > (std::move(callback));
        return {bus, id};
    }
} // namespace ash

#endif //ASHEN_EVENTBUS_H
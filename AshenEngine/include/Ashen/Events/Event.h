#ifndef ASHEN_EVENT_H
#define ASHEN_EVENT_H

#include <concepts>
#include <functional>
#include <string>
#include <string_view>

namespace ash {
    // =============================================================================
    // Event Types & Categories
    // =============================================================================

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum class EventCategory {
        None = 0,
        Application = 1 << 0,
        Input = 1 << 1,
        Keyboard = 1 << 2,
        Mouse = 1 << 3,
        MouseButton = 1 << 4
    };

    constexpr EventCategory operator|(EventCategory lhs, EventCategory rhs) noexcept {
        return static_cast<EventCategory>(
            static_cast<int>(lhs) | static_cast<int>(rhs)
        );
    }

    constexpr EventCategory operator&(EventCategory lhs, EventCategory rhs) noexcept {
        return static_cast<EventCategory>(
            static_cast<int>(lhs) & static_cast<int>(rhs)
        );
    }

    constexpr bool HasCategory(const EventCategory flags, const EventCategory category) noexcept {
        return static_cast<int>(flags & category) != 0;
    }

    // =============================================================================
    // Event Concepts
    // =============================================================================

    template<typename T>
    concept EventLike = requires(const T &event)
    {
        { T::GetStaticType() } -> std::same_as<EventType>;
        { event.GetType() } -> std::same_as<EventType>;
        { event.GetName() } -> std::convertible_to<std::string_view>;
        { event.GetCategories() } -> std::same_as<EventCategory>;
    };

    // =============================================================================
    // Base Event Class
    // =============================================================================

    class Event {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType GetType() const noexcept = 0;

        [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;

        [[nodiscard]] virtual EventCategory GetCategories() const noexcept = 0;

        [[nodiscard]] virtual std::string ToString() const { return std::string(GetName()); }

        [[nodiscard]] bool IsInCategory(const EventCategory category) const noexcept {
            return HasCategory(GetCategories(), category);
        }

        bool handled = false;
    };

    // =============================================================================
    // Event Dispatcher
    // =============================================================================

    class EventDispatcher {
    public:
        explicit EventDispatcher(Event &event) : m_Event(event) {
        }

        template<EventLike T>
        bool Dispatch(std::invocable<T &> auto &&callback) noexcept {
            if (m_Event.GetType() == T::GetStaticType()) {
                m_Event.handled |= std::invoke(
                    std::forward<decltype(callback)>(callback),
                    static_cast<T &>(m_Event)
                );
                return true;
            }
            return false;
        }

        template<EventLike T>
        bool DispatchVoid(std::invocable<T &> auto &&callback) noexcept {
            if (m_Event.GetType() == T::GetStaticType()) {
                std::invoke(
                    std::forward<decltype(callback)>(callback),
                    static_cast<T &>(m_Event)
                );
                return true;
            }
            return false;
        }

    private:
        Event &m_Event;
    };

    // =============================================================================
    // Output operator
    // =============================================================================

    inline std::ostream &operator<<(std::ostream &os, const Event &event) {
        return os << event.ToString();
    }
} // namespace ash

#endif //ASHEN_EVENT_H
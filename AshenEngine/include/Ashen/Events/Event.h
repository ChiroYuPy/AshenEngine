#ifndef ASHEN_EVENT_H
#define ASHEN_EVENT_H

#include <string_view>

#include "Ashen/Core/Types.h"

namespace ash {
    // =============================================================================
    // Event Types & Categories
    // =============================================================================

    enum class EventType : u8 {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum class EventCategory : u8 {
        None        = 0,
        Application = 1 << 0,
        Input       = 1 << 1,
        Keyboard    = 1 << 2,
        Mouse       = 1 << 3,
        MouseButton = 1 << 4,
    };

    class Event {
    public:
        virtual ~Event() = default;
        virtual EventType      GetType()     const noexcept = 0;
        virtual StringView GetName()   const noexcept = 0;
        virtual EventCategory  GetCategory() const noexcept = 0;
        bool Handled = false;

        bool IsInCategory(EventCategory category) const noexcept {
            return (static_cast<u16>(GetCategory()) & static_cast<uint16_t>(category)) != 0;
        }
    };

} // namespace ash

#endif //ASHEN_EVENT_H
#ifndef ASHEN_MOUSE_EVENT_H
#define ASHEN_MOUSE_EVENT_H

#include "Ashen/Events/Event.h"
#include "Ashen/Core/Codes.h"

namespace ash {
    class MouseMovedEvent final : public Event {
    public:
        MouseMovedEvent(const float x, const float y) : mX(x), mY(y) {}
        float GetX() const noexcept { return mX; }
        float GetY() const noexcept { return mY; }

        static EventType GetStaticType() noexcept { return EventType::MouseMoved; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "MouseMoved"; }
        EventCategory GetCategory() const noexcept override {
            return static_cast<EventCategory>(
                static_cast<uint16_t>(EventCategory::Mouse) | static_cast<uint16_t>(EventCategory::Input));
        }
    private:
        float mX, mY;
    };

    class MouseScrolledEvent final : public Event {
    public:
        MouseScrolledEvent(const float xOffset, const float yOffset) : mXOffset(xOffset), mYOffset(yOffset) {}
        float GetXOffset() const noexcept { return mXOffset; }
        float GetYOffset() const noexcept { return mYOffset; }

        static EventType GetStaticType() noexcept { return EventType::MouseScrolled; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "MouseScrolled"; }
        EventCategory GetCategory() const noexcept override {
            return static_cast<EventCategory>(
                static_cast<uint16_t>(EventCategory::Mouse) | static_cast<uint16_t>(EventCategory::Input));
        }
    private:
        float mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event {
    public:
        MouseButton GetMouseButton() const noexcept { return mButton; }
        EventCategory GetCategory() const noexcept override {
            return static_cast<EventCategory>(
                static_cast<uint16_t>(EventCategory::MouseButton) |
                static_cast<uint16_t>(EventCategory::Mouse) |
                static_cast<uint16_t>(EventCategory::Input));
        }
    protected:
        explicit MouseButtonEvent(const MouseButton button) : mButton(button) {}
        MouseButton mButton;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(const MouseButton button) : MouseButtonEvent(button) {}

        static EventType GetStaticType() noexcept { return EventType::MouseButtonPressed; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "MouseButtonPressed"; }
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(const MouseButton button) : MouseButtonEvent(button) {}

        static EventType GetStaticType() noexcept { return EventType::MouseButtonReleased; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "MouseButtonReleased"; }
    };
} // namespace ash

#endif //ASHEN_MOUSE_EVENT_H
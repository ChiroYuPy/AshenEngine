#ifndef ASHEN_MOUSEEVENT_H
#define ASHEN_MOUSEEVENT_H

#include <sstream>

#include "Ashen/Events/Event.h"
#include "Ashen/Core/MouseCodes.h"

namespace ash {
    class MouseMovedEvent final : public Event {
    public:
        MouseMovedEvent(const float x, const float y)
            : m_MouseX(x), m_MouseY(y) {
        }

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE (MouseMoved)
        EVENT_CLASS_CATEGORY (EventCategoryMouse
        |
        EventCategoryInput
        )

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent final : public Event {
    public:
        MouseScrolledEvent(const float xOffset, const float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {
        }

        float getXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << getXOffset() << ", " << GetYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE (MouseScrolled)
        EVENT_CLASS_CATEGORY (EventCategoryMouse
        |
        EventCategoryInput
        )

    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent : public Event {
    public:
        MouseCode GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY (EventCategoryMouse
        |
        EventCategoryInput
        |
        EventCategoryMouseButton
        )

    protected:
        explicit MouseButtonEvent(const MouseCode button)
            : m_Button(button) {
        }

        MouseCode m_Button;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(const MouseCode button)
            : MouseButtonEvent(button) {
        }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE (MouseButtonPressed)
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const MouseCode button)
            : MouseButtonEvent(button) {
        }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE (MouseButtonReleased)
    };
}

#endif //ASHEN_MOUSEEVENT_H
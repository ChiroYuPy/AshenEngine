#ifndef ASHEN_APPLICATIONEVENT_H
#define ASHEN_APPLICATIONEVENT_H

#include <sstream>

#include "Ashen/events/Event.h"

namespace ash {
    class WindowResizeEvent final : public Event {
    public:
        WindowResizeEvent(const unsigned int width, const unsigned int height)
            : m_Width(width), m_Height(height) {
        }

        [[nodiscard]] unsigned int GetWidth() const { return m_Width; }
        [[nodiscard]] unsigned int GetHeight() const { return m_Height; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE (WindowResize)
        EVENT_CLASS_CATEGORY (EventCategoryApplication)

    private:
        unsigned int m_Width, m_Height;
    };

    class WindowCloseEvent final : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE (WindowClose)
        EVENT_CLASS_CATEGORY (EventCategoryApplication)
    };

    class WindowFocusEvent final : public Event {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS_TYPE (WindowFocus)
        EVENT_CLASS_CATEGORY (EventCategoryApplication)
    };

    class WindowLostFocusEvent final : public Event {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE (WindowLostFocus)
        EVENT_CLASS_CATEGORY (EventCategoryApplication)
    };
}

#endif //ASHEN_APPLICATIONEVENT_H
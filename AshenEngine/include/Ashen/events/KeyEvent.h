#ifndef ASHEN_KEYEVENT_H
#define ASHEN_KEYEVENT_H

#include <sstream>

#include "Ashen/events/Event.h"
#include "Ashen/core/KeyCodes.h"

namespace pixl {
    class KeyEvent : public Event {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY (EventCategoryKeyboard
        |
        EventCategoryInput
        )

    protected:
        explicit KeyEvent(const KeyCode keycode)
            : m_KeyCode(keycode) {
        }

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent final : public KeyEvent {
    public:
        explicit KeyPressedEvent(const KeyCode keycode, const bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {
        }

        bool IsRepeat() const { return m_IsRepeat; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE (KeyPressed)

    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent final : public KeyEvent {
    public:
        explicit KeyReleasedEvent(const KeyCode keycode)
            : KeyEvent(keycode) {
        }

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE (KeyReleased)
    };

    class KeyTypedEvent final : public KeyEvent {
    public:
        explicit KeyTypedEvent(const KeyCode keycode)
            : KeyEvent(keycode) {
        }

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE (KeyTyped)
    };
}

#endif //ASHEN_KEYEVENT_H
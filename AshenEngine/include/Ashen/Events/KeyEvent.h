#ifndef ASHEN_KEY_EVENT_H
#define ASHEN_KEY_EVENT_H

#include "Ashen/Events/Event.h"
#include "Ashen/Core/Codes.h"

namespace ash {

    class KeyEvent : public Event {
    public:
        Key GetKeyCode() const noexcept { return mKeyCode; }
        EventCategory GetCategory() const noexcept override {
            return static_cast<EventCategory>(
                static_cast<uint16_t>(EventCategory::Keyboard) | static_cast<uint16_t>(EventCategory::Input));
        }
    protected:
        explicit KeyEvent(const Key keycode) : mKeyCode(keycode) {}
        Key mKeyCode;
    };

    class KeyPressedEvent final : public KeyEvent {
    public:
        KeyPressedEvent(const Key keycode, const bool isRepeat) : KeyEvent(keycode), mIsRepeat(isRepeat) {}

        static EventType GetStaticType() noexcept { return EventType::KeyPressed; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "KeyPressed"; }

        bool IsRepeat() const noexcept { return mIsRepeat; }
    private:
        bool mIsRepeat;
    };

    // De même pour KeyReleasedEvent et KeyTypedEvent
    class KeyReleasedEvent final : public KeyEvent {
    public:
        KeyReleasedEvent(const Key keycode) : KeyEvent(keycode) {}

        static EventType GetStaticType() noexcept { return EventType::KeyReleased; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "KeyReleased"; }
    };

    class KeyTypedEvent final : public KeyEvent {
    public:
        KeyTypedEvent(const Key keycode) : KeyEvent(keycode) {}

        static EventType GetStaticType() noexcept { return EventType::KeyTyped; }
        EventType GetType() const noexcept override { return GetStaticType(); }
        StringView GetName() const noexcept override { return "KeyTyped"; }
    };

} // namespace ash

#endif //ASHEN_KEY_EVENT_H
#ifndef ASHEN_KEYEVENT_H
#define ASHEN_KEYEVENT_H

#include <format>

#include "Ashen/Core/Codes.h"
#include "Ashen/Events/Event.h"

namespace ash {
    // =============================================================================
    // Base KeyEvent
    // =============================================================================

    class KeyEvent : public Event {
    public:
        [[nodiscard]] KeyCode GetKeyCode() const noexcept { return m_KeyCode; }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Keyboard | EventCategory::Input;
        }

    protected:
        explicit KeyEvent(const KeyCode keycode) noexcept
            : m_KeyCode(keycode) {
        }

        KeyCode m_KeyCode;
    };

    // =============================================================================
    // KeyPressedEvent
    // =============================================================================

    class KeyPressedEvent final : public KeyEvent {
    public:
        explicit KeyPressedEvent(const KeyCode keycode, const bool isRepeat = false) noexcept
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {
        }

        [[nodiscard]] bool IsRepeat() const noexcept { return m_IsRepeat; }

        [[nodiscard]] std::string ToString() const override {
            return std::format("KeyPressedEvent: {} (repeat={})",
                               static_cast<int>(m_KeyCode), m_IsRepeat);
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::KeyPressed;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "KeyPressed";
        }

    private:
        bool m_IsRepeat;
    };

    // =============================================================================
    // KeyReleasedEvent
    // =============================================================================

    class KeyReleasedEvent final : public KeyEvent {
    public:
        explicit KeyReleasedEvent(const KeyCode keycode) noexcept
            : KeyEvent(keycode) {
        }

        [[nodiscard]] std::string ToString() const override {
            return std::format("KeyReleasedEvent: {}", static_cast<int>(m_KeyCode));
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::KeyReleased;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "KeyReleased";
        }
    };

    // =============================================================================
    // KeyTypedEvent
    // =============================================================================

    class KeyTypedEvent final : public KeyEvent {
    public:
        explicit KeyTypedEvent(const KeyCode keycode) noexcept
            : KeyEvent(keycode) {
        }

        [[nodiscard]] std::string ToString() const override {
            return std::format("KeyTypedEvent: {}", static_cast<int>(m_KeyCode));
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::KeyTyped;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "KeyTyped";
        }
    };
} // namespace ash

#endif //ASHEN_KEYEVENT_H
#ifndef ASHEN_MOUSEEVENT_H
#define ASHEN_MOUSEEVENT_H

#include <format>
#include "Ashen/Events/Event.h"
#include "Ashen/Core/Types.h"

namespace ash {
    // =============================================================================
    // MouseMovedEvent
    // =============================================================================

    class MouseMovedEvent final : public Event {
    public:
        MouseMovedEvent(const float x, const float y) noexcept : m_X(x), m_Y(y) {
        }

        [[nodiscard]] float GetX() const noexcept { return m_X; }
        [[nodiscard]] float GetY() const noexcept { return m_Y; }

        [[nodiscard]] String ToString() const override {
            return std::format("MouseMovedEvent: {}, {}", m_X, m_Y);
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::MouseMoved;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "MouseMoved";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Mouse | EventCategory::Input;
        }

    private:
        float m_X, m_Y;
    };

    // =============================================================================
    // MouseScrolledEvent
    // =============================================================================

    class MouseScrolledEvent final : public Event {
    public:
        MouseScrolledEvent(const float xOffset, const float yOffset) noexcept
            : m_XOffset(xOffset), m_YOffset(yOffset) {
        }

        [[nodiscard]] float GetXOffset() const noexcept { return m_XOffset; }
        [[nodiscard]] float GetYOffset() const noexcept { return m_YOffset; }

        [[nodiscard]] String ToString() const override {
            return std::format("MouseScrolledEvent: {}, {}", m_XOffset, m_YOffset);
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::MouseScrolled;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "MouseScrolled";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Mouse | EventCategory::Input;
        }

    private:
        float m_XOffset, m_YOffset;
    };

    // =============================================================================
    // Base MouseButtonEvent
    // =============================================================================

    class MouseButtonEvent : public Event {
    public:
        [[nodiscard]] MouseButton GetButton() const noexcept { return m_Button; }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
        }

    protected:
        explicit MouseButtonEvent(const MouseButton button) noexcept : m_Button(button) {
        }

        MouseButton m_Button;
    };

    // =============================================================================
    // MouseButtonPressedEvent
    // =============================================================================

    class MouseButtonPressedEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(const MouseButton button) noexcept
            : MouseButtonEvent(button) {
        }

        [[nodiscard]] String ToString() const override {
            return std::format("MouseButtonPressedEvent: {}", static_cast<int>(m_Button));
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::MouseButtonPressed;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "MouseButtonPressed";
        }
    };

    // =============================================================================
    // MouseButtonReleasedEvent
    // =============================================================================

    class MouseButtonReleasedEvent final : public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const MouseButton button) noexcept
            : MouseButtonEvent(button) {
        }

        [[nodiscard]] String ToString() const override {
            return std::format("MouseButtonReleasedEvent: {}", static_cast<int>(m_Button));
        }

        static constexpr EventType GetStaticType() noexcept {
            return EventType::MouseButtonReleased;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "MouseButtonReleased";
        }
    };
} // namespace ash

#endif //ASHEN_MOUSEEVENT_H
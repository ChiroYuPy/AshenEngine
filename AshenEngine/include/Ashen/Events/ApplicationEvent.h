#ifndef ASHEN_APPLICATIONEVENT_H
#define ASHEN_APPLICATIONEVENT_H

#include <format>
#include "Ashen/Events/Event.h"

namespace ash {
    // =============================================================================
    // WindowResizeEvent
    // =============================================================================

    class WindowResizeEvent final : public Event {
    public:
        WindowResizeEvent(const unsigned int width, const unsigned int height) noexcept
            : m_Width(width), m_Height(height) {
        }

        [[nodiscard]] unsigned int GetWidth() const noexcept { return m_Width; }
        [[nodiscard]] unsigned int GetHeight() const noexcept { return m_Height; }

        [[nodiscard]] std::string ToString() const override {
            return std::format("WindowResizeEvent: {}x{}", m_Width, m_Height);
        }

        // Static type info
        static constexpr EventType GetStaticType() noexcept {
            return EventType::WindowResize;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "WindowResize";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Application;
        }

    private:
        unsigned int m_Width, m_Height;
    };

    // =============================================================================
    // WindowCloseEvent
    // =============================================================================

    class WindowCloseEvent final : public Event {
    public:
        WindowCloseEvent() = default;

        static constexpr EventType GetStaticType() noexcept {
            return EventType::WindowClose;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "WindowClose";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Application;
        }
    };

    // =============================================================================
    // WindowFocusEvent
    // =============================================================================

    class WindowFocusEvent final : public Event {
    public:
        WindowFocusEvent() = default;

        static constexpr EventType GetStaticType() noexcept {
            return EventType::WindowFocus;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "WindowFocus";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Application;
        }
    };

    // =============================================================================
    // WindowLostFocusEvent
    // =============================================================================

    class WindowLostFocusEvent final : public Event {
    public:
        WindowLostFocusEvent() = default;

        static constexpr EventType GetStaticType() noexcept {
            return EventType::WindowLostFocus;
        }

        [[nodiscard]] EventType GetType() const noexcept override {
            return GetStaticType();
        }

        [[nodiscard]] std::string_view GetName() const noexcept override {
            return "WindowLostFocus";
        }

        [[nodiscard]] EventCategory GetCategories() const noexcept override {
            return EventCategory::Application;
        }
    };
} // namespace ash

#endif //ASHEN_APPLICATIONEVENT_H
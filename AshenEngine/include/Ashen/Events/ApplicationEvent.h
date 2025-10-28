#ifndef ASHEN_APPLICATION_EVENT_H
#define ASHEN_APPLICATION_EVENT_H

#include "Event.h"

namespace ash {
    class WindowResizeEvent final : public Event {
    public:
        explicit WindowResizeEvent(const u32 width, const u32 height) : mWidth(width), mHeight(height) {}
        u32 GetWidth()  const noexcept { return mWidth; }
        u32 GetHeight() const noexcept { return mHeight; }

        static EventType GetStaticType() noexcept { return EventType::WindowResize; }
        EventType      GetType()     const noexcept override { return EventType::WindowResize; }
        StringView GetName()   const noexcept override { return "WindowResize"; }
        EventCategory  GetCategory() const noexcept override { return EventCategory::Application; }
    private:
        u32 mWidth, mHeight;
    };

    class WindowCloseEvent final : public Event {
    public:
        static EventType GetStaticType() noexcept { return EventType::WindowClose; }
        EventType      GetType()     const noexcept override { return EventType::WindowClose; }
        StringView GetName()   const noexcept override { return "WindowClose"; }
        EventCategory  GetCategory() const noexcept override { return EventCategory::Application; }
    };

    class WindowFocusEvent final : public Event {
    public:
        static EventType GetStaticType() noexcept { return EventType::WindowFocus; }
        EventType      GetType()     const noexcept override { return EventType::WindowFocus; }
        StringView GetName()   const noexcept override { return "WindowFocus"; }
        EventCategory  GetCategory() const noexcept override { return EventCategory::Application; }
    };

    class WindowLostFocusEvent final : public Event {
    public:
        static EventType GetStaticType() noexcept { return EventType::WindowLostFocus; }
        EventType      GetType()     const noexcept override { return EventType::WindowLostFocus; }
        StringView GetName()   const noexcept override { return "WindowLostFocus"; }
        EventCategory  GetCategory() const noexcept override { return EventCategory::Application; }
    };

    class WindowMovedEvent final : public Event {
    public:
        explicit WindowMovedEvent(const float x, const float y) : mX(x), mY(y) {}
        float GetX() const noexcept { return mX; }
        float GetY() const noexcept { return mY; }

        static EventType GetStaticType() noexcept { return EventType::WindowMoved; }
        EventType      GetType()     const noexcept override { return EventType::WindowMoved; }
        StringView GetName()   const noexcept override { return "WindowMoved"; }
        EventCategory  GetCategory() const noexcept override { return EventCategory::Application; }
    private:
        float mX, mY;
    };
} // namespace ash

#endif //ASHEN_APPLICATION_EVENT_H
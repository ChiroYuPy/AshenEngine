#ifndef ASHEN_WINDOW_H
#define ASHEN_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <glm/vec2.hpp>
#include "Ashen/events/Event.h"
#include "Ashen/math/Math.h"

namespace ash {
    struct WindowProperties {
        std::string Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizable = true;
        bool VSync = true;
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        explicit Window(const WindowProperties &specification = WindowProperties());

        Window(const Window &) = delete;

        Window &operator=(const Window &) = delete;

        Window(Window &&) noexcept;

        Window &operator=(Window &&) noexcept;

        ~Window();

        void Create();

        void Destroy();

        void SwapBuffers() const;

        void PollEvents() const;

        void SetEventCallback(const EventCallbackFn &callback) { m_Data.EventCallback = callback; }

        [[nodiscard]] Vec2 GetFramebufferSize() const;

        [[nodiscard]] bool ShouldClose() const;

        [[nodiscard]] std::string GetTitle() const { return m_Data.Title; }
        [[nodiscard]] UVec2 GetSizeU() const { return m_Data.Size; }
        [[nodiscard]] Vec2 GetSizeF() const { return {m_Data.Size}; }
        [[nodiscard]] uint32_t GetWidth() const { return m_Data.Size.x; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Data.Size.y; }

        [[nodiscard]] float GetAspectRatio() const {
            return static_cast<float>(m_Data.Size.x) / static_cast<float>(m_Data.Size.y);
        }

        [[nodiscard]] GLFWwindow *GetHandle() const { return m_Handle; }

    private:
        void SetupCallbacks() const;

        struct WindowData {
            std::string Title;
            UVec2 Size;
            bool VSync;
            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
        GLFWwindow *m_Handle = nullptr;
    };
}

#endif //ASHEN_WINDOW_H
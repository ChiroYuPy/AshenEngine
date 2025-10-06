#ifndef ASHEN_GLFWWINDOW_H
#define ASHEN_GLFWWINDOW_H

#include <functional>
#include <string>

#include "Ashen/Events/Event.h"
#include "Ashen/Math/Math.h"

struct GLFWwindow;

namespace ash {

    struct WindowProperties {
        std::string Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizable = true;
        bool VSync = true;
    };

    class Window final {
        using EventCallbackFn = std::function<void(Event&)>;

    public:
        explicit Window(const WindowProperties& props = WindowProperties());
        ~Window();

        void Create();
        void Destroy();
        void SwapBuffers() const;
        void PollEvents() const;
        void SetEventCallback(const EventCallbackFn& callback);

        [[nodiscard]] Vec2 GetFramebufferSize() const;
        [[nodiscard]] bool ShouldClose() const;
        [[nodiscard]] std::string GetTitle() const;
        [[nodiscard]] UVec2 GetSizeU() const;
        [[nodiscard]] Vec2 GetSizeF() const;
        [[nodiscard]] uint32_t GetWidth() const;
        [[nodiscard]] uint32_t GetHeight() const;
        [[nodiscard]] float GetAspectRatio() const;
        [[nodiscard]] void* GetHandle() const;

    private:
        void SetupCallbacks() const;

        struct WindowData {
            std::string Title;
            UVec2 Size;
            bool VSync;
            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
        GLFWwindow* m_Handle = nullptr;
    };

} // namespace ash

#endif // ASHEN_GLFWWINDOW_H

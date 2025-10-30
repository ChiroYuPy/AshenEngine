#ifndef ASHEN_GLFWWINDOW_H
#define ASHEN_GLFWWINDOW_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"
#include "Ashen/Events/Event.h"
#include "Ashen/GraphicsAPI/GraphicsContext.h"

struct GLFWwindow;

namespace ash {
    struct WindowProperties {
        String Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizable = true;
        bool VSync = true;
    };

    class Window final {
        using EventCallbackFn = Function<void(Event &)>;

    public:
        explicit Window(const WindowProperties &props = WindowProperties());

        ~Window();

        void Update() const;

        void PollEvents() const;

        void SetEventCallback(const EventCallbackFn &callback);

        [[nodiscard]] Vec2 GetFramebufferSize() const;

        [[nodiscard]] bool ShouldClose() const;

        [[nodiscard]] String GetTitle() const;

        [[nodiscard]] UVec2 GetSizeU() const;

        [[nodiscard]] Vec2 GetSizeF() const;

        [[nodiscard]] uint32_t GetWidth() const;

        [[nodiscard]] uint32_t GetHeight() const;

        [[nodiscard]] float GetAspectRatio() const;

        [[nodiscard]] void *GetHandle() const;

    private:
        void SetupCallbacks() const;

        void Create();

        void Destroy();

        GLFWwindow *m_Handle = nullptr;
        Own<GraphicsContext> m_Context;

        struct WindowData {
            String Title;
            UVec2 Size;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}

#endif // ASHEN_GLFWWINDOW_H
#include "Ashen/Core/Window.h"

#include <iostream>

#include <../../vendor/include/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

namespace ash {

    static uint8_t s_GLFWWindowCount = 0;

    static void GLFWErrorCallback(int error, const char* description) {
        Logger::Error("GLFW Error ({0}): {1}", error, description);
    }

    Window::Window(const WindowProperties &props) {
        m_Data.Title = props.Title;
        m_Data.Size.x = props.Width;
        m_Data.Size.y = props.Height;
        m_Data.VSync = props.VSync;

        Create();
    }

    Window::~Window() {
        Destroy();
    }

    void Window::Create() {
        if (s_GLFWWindowCount == 0) {
            if (!glfwInit())
                Logger::Error("Failed to initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        m_Handle = glfwCreateWindow(m_Data.Size.x, m_Data.Size.y, m_Data.Title.c_str(), nullptr, nullptr);
        ++s_GLFWWindowCount;

        m_Context = MakeOwn<GraphicsContext>(m_Handle);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Handle, &m_Data);
        glfwSwapInterval(m_Data.VSync ? 1 : 0); // Vsync

        SetupCallbacks();
    }

    void Window::Destroy() {
        if (!m_Handle) return;

        glfwDestroyWindow(m_Handle);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0)
            glfwTerminate();

        m_Handle = nullptr;
    }

    void* Window::GetHandle() const {
        return m_Handle;
    }

    void Window::SetupCallbacks() const {
        if (!glfwGetWindowUserPointer(m_Handle)) {
            Logger::Error("Window user pointer not set!");
            return;
        }

        // Window resize callback
        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow *window, const int width, const int height) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            WindowData &data = *static_cast<WindowData *>(up);
            data.Size.x = width;
            data.Size.y = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        // Window close callback
        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow *window) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        // Window focus callback
        glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow *window, const int focused) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            if (focused) {
                WindowFocusEvent event;
                data.EventCallback(event);
            } else {
                WindowLostFocusEvent event;
                data.EventCallback(event);
            }
        });

        // Key callback
        glfwSetKeyCallback(m_Handle, [](GLFWwindow *window, const int key, int scancode, const int action, int mods) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, false);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, true);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        // Char callback (for text input)
        glfwSetCharCallback(m_Handle, [](GLFWwindow *window, const unsigned int keycode) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        // Mouse button callback
        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow *window, const int button, const int action, int mods) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        // Mouse scroll callback
        glfwSetScrollCallback(m_Handle, [](GLFWwindow *window, const double xOffset, const double yOffset) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.EventCallback(event);
        });

        // Mouse position callback
        glfwSetCursorPosCallback(m_Handle, [](GLFWwindow *window, const double xPos, const double yPos) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            const WindowData &data = *static_cast<WindowData *>(up);

            MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.EventCallback(event);
        });

        // Framebuffer size callback (if you care about framebuffer vs window size)
        glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* window, const int width, const int height) {
            void* up = glfwGetWindowUserPointer(window);
            if (!up) return;
            WindowData &data = *static_cast<WindowData *>(up);

            // keep stored size in sync if you want, and emit event if needed
            data.Size.x = width;
            data.Size.y = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });
    }

    void Window::Update() const {
        m_Context->SwapBuffers();
    }

    void Window::PollEvents() const {
        glfwPollEvents();
    }

    void Window::SetEventCallback(const EventCallbackFn &callback) {
        m_Data.EventCallback = callback;
    }

    glm::vec2 Window::GetFramebufferSize() const {
        int width, height;
        glfwGetFramebufferSize(m_Handle, &width, &height);
        return {width, height};
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Handle) != 0;
    }

    std::string Window::GetTitle() const {
        return m_Data.Title;
    }

    UVec2 Window::GetSizeU() const {
        return {m_Data.Size.x, m_Data.Size.y};
    }

    Vec2 Window::GetSizeF() const {
        return {m_Data.Size.x, m_Data.Size.y};
    }

    uint32_t Window::GetWidth() const {
        return m_Data.Size.x;
    }

    uint32_t Window::GetHeight() const {
        return m_Data.Size.y;
    }

    float Window::GetAspectRatio() const {
        return static_cast<float>(m_Data.Size.x) / static_cast<float>(m_Data.Size.y);
    }
}

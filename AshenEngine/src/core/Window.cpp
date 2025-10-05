#include "Ashen/core/Window.h"

#include <iostream>

#include "Ashen/core/Logger.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/events/KeyEvent.h"
#include "Ashen/events/MouseEvent.h"

namespace ash {
    Window::Window(const WindowProperties &specification) {
        m_Data.Title = specification.Title;
        m_Data.Size.x = specification.Width;
        m_Data.Size.y = specification.Height;
        m_Data.VSync = specification.VSync;
    }

    Window::~Window() {
        Destroy();
    }

    void Window::Create() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        m_Handle = glfwCreateWindow(m_Data.Size.x, m_Data.Size.y,
                                    m_Data.Title.c_str(), nullptr, nullptr);

        if (!m_Handle) {
            std::cerr << "Failed to create GLFW window!\n";
            assert(false);
        }

        glfwMakeContextCurrent(m_Handle);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cerr << "Failed to initialize GLAD\n";
            assert(false);
        }

        glfwSwapInterval(m_Data.VSync ? 1 : 0);

        // Store window data in a GLFW user pointer for callbacks
        glfwSetWindowUserPointer(m_Handle, &m_Data);

        SetupCallbacks();
    }

    void Window::SetupCallbacks() const {
        // Window resize callback
        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow *window, const int width, const int height) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            data.Size.x = width;
            data.Size.y = height;

            Logger::info("window size: {}, {}", width, height);

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        // Window close callback
        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow *window) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        // Window focus callback
        glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow *window, const int focused) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

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
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

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
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        // Mouse button callback
        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow *window, const int button, const int action, int mods) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

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
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.EventCallback(event);
        });

        // Mouse position callback
        glfwSetCursorPosCallback(m_Handle, [](GLFWwindow *window, const double xPos, const double yPos) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.EventCallback(event);
        });
    }

    void Window::Destroy() {
        if (m_Handle) {
            glfwMakeContextCurrent(m_Handle);

            glfwSetWindowUserPointer(m_Handle, nullptr);

            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }
    }

    void Window::SwapBuffers() const {
        glfwSwapBuffers(m_Handle);
    }

    void Window::PollEvents() const {
        glfwPollEvents();
    }

    glm::vec2 Window::GetFramebufferSize() const {
        int width, height;
        glfwGetFramebufferSize(m_Handle, &width, &height);
        return {width, height};
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Handle) != 0;
    }
}
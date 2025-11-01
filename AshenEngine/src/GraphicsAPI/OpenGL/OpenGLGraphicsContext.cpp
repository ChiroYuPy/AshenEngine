#include "Ashen/GraphicsAPI/OpenGL/OpenGLGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Ashen/Core/Logger.h"

namespace ash {
    bool OpenGLGraphicsContext::s_GladInitialized = false;

    OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle) {
        if (!m_WindowHandle)
            Logger::Error("Window handle is null!");
    }

    void OpenGLGraphicsContext::Init() {
        glfwMakeContextCurrent(m_WindowHandle);

        if (!s_GladInitialized) {
            const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
            if (!status)
                Logger::Error("Failed to initialize GLAD!");

            Logger::Info("OpenGL Info:");
            Logger::Info() << "  Vendor: " << glGetString(GL_VENDOR);
            Logger::Info() << "  Renderer: " << glGetString(GL_RENDERER);
            Logger::Info() << "  Version: " << glGetString(GL_VERSION);

            if (!(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)))
                Logger::Error("Ashen requires at least OpenGL version 4.5!");

            s_GladInitialized = true;
        }
    }

    void OpenGLGraphicsContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLGraphicsContext::Terminate() {
        // not needed
    }
}
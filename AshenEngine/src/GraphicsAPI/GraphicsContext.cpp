#include "Ashen/GraphicsAPI/GraphicsContext.h"

#include <../../vendor/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include "Ashen/Core/Logger.h"

namespace ash {

    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        if (!m_WindowHandle)
            Logger::Error("Window handle is null!");
    }

    void GraphicsContext::Init() const {
        glfwMakeContextCurrent(m_WindowHandle);

        static bool s_GladInitialized = false;
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

    void GraphicsContext::SwapBuffers() const {
        glfwSwapBuffers(m_WindowHandle);
    }

}

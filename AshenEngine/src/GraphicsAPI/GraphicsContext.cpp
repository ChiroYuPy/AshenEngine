#include "Ashen/GraphicsAPI/GraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Ashen/Core/Logger.h"

namespace ash {

    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle) {
        if (!windowHandle) Logger::error("Window handle is not null!");
    }

    void GraphicsContext::Init() const {
        glfwMakeContextCurrent(m_WindowHandle);
        const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        if (!status) Logger::error("Failed to initialize Glad!");

        Logger::info("OpenGL Info:");
        Logger::info() << "  Vendor: {0}" << glGetString(GL_VENDOR);
        Logger::info() << "  Renderer: {0}" << glGetString(GL_RENDERER);
        Logger::info() << "  Version: {0}" << glGetString(GL_VERSION);

        if (!(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5))) Logger::error("Ashen requires at least OpenGL version 4.5!");
    }

    void GraphicsContext::SwapBuffers() const {
        glfwSwapBuffers(m_WindowHandle);
    }

}

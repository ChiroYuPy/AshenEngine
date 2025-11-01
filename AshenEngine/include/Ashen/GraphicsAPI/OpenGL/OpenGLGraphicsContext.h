#ifndef ASHEN_OPENGLGRAPHICSCONTEXT_H
#define ASHEN_OPENGLGRAPHICSCONTEXT_H

#include "Ashen/GraphicsAPI/GraphicsContext.h"

struct GLFWwindow;

namespace ash {
    class OpenGLGraphicsContext final : public GraphicsContext {
    public:
        explicit OpenGLGraphicsContext(GLFWwindow* windowHandle);
        ~OpenGLGraphicsContext() override = default;

        void Init() override;
        void SwapBuffers() override;
        void Terminate() override;

    private:
        GLFWwindow* m_WindowHandle;
        static bool s_GladInitialized;
    };
}

#endif // ASHEN_OPENGLGRAPHICSCONTEXT_H
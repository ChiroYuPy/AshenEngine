#ifndef ASHEN_GRAPHICSCONTEXT_H
#define ASHEN_GRAPHICSCONTEXT_H

struct GLFWwindow;

namespace ash {
    class GraphicsContext {
    public:
        explicit GraphicsContext(GLFWwindow *windowHandle);

        void Init() const;

        void SwapBuffers() const;

    private:
        GLFWwindow *m_WindowHandle;
    };
}

#endif //ASHEN_GRAPHICSCONTEXT_H
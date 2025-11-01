#ifndef ASHEN_GRAPHICSCONTEXT_H
#define ASHEN_GRAPHICSCONTEXT_H

#include "Ashen/Core/Types.h"

struct GLFWwindow;

namespace ash {
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Terminate() = 0;

        static Own<GraphicsContext> Create(GLFWwindow* windowHandle);
    };
}

#endif //ASHEN_GRAPHICSCONTEXT_H
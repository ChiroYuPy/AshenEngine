#include "Ashen/GraphicsAPI/GraphicsContext.h"

#include "Ashen/Core/Logger.h"
#include "Ashen/GraphicsAPI/RendererAPI.h"
#include "Ashen/GraphicsAPI/OpenGL/OpenGLGraphicsContext.h"

namespace ash {
    Own<GraphicsContext> GraphicsContext::Create(GLFWwindow* windowHandle) {
        if (!windowHandle) {
            Logger::Error("Cannot create GraphicsContext: window handle is null!");
            return nullptr;
        }

        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL: {
                Logger::Info("Creating OpenGL Graphics Context");
                return MakeOwn<OpenGLGraphicsContext>(windowHandle);
            }

            case RendererAPI::API::None:
            default: {
                Logger::Error("Invalid Graphics API!");
                return nullptr;
            }
        }
    }
}

#ifndef ASHEN_IMGUI_CONTEXT_H
#define ASHEN_IMGUI_CONTEXT_H

struct GLFWwindow;

namespace ash {
    class ImGuiContext {
    public:
        static void Init(GLFWwindow* window);
        static void Shutdown();
        
        static void BeginFrame();
        static void EndFrame();
        static void Render();
        
        static bool IsInitialized();
        
    private:
        static bool s_Initialized;
    };
}

#endif //ASHEN_IMGUI_CONTEXT_H
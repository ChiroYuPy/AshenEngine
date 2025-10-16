#include "ImGUIContext.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Ashen/Core/Logger.h"

namespace ash {
    bool ImGuiContext::s_Initialized = false;

    void ImGuiContext::Init(GLFWwindow* window) {
        if (s_Initialized) {
            Logger::Warn("ImGui already initialized");
            return;
        }

        if (!window) {
            Logger::Error("Cannot initialize ImGui: window is null");
            return;
        }

        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        const char* glsl_version = "#version 410";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        s_Initialized = true;
        Logger::Info("ImGui initialized successfully");
    }

    void ImGuiContext::Shutdown() {
        if (!s_Initialized) {
            return;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        s_Initialized = false;
        Logger::Info("ImGui shutdown");
    }

    void ImGuiContext::BeginFrame() {
        if (!s_Initialized) {
            Logger::Error("ImGui not initialized - call Init() first");
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiContext::EndFrame() {
        if (!s_Initialized) return;

        ImGui::Render();
        int display_w, display_h;
        // Vous devez passer la fenêtre pour avoir les dimensions
        // Pour l'instant, on utilise les dimensions du framebuffer
    }

    void ImGuiContext::Render() {
        if (!s_Initialized) return;

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool ImGuiContext::IsInitialized() {
        return s_Initialized;
    }
}
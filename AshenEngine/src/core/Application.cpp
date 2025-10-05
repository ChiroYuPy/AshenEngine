#include "Ashen/core/Application.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <utility>

#include "Ashen/core/Input.h"
#include "Ashen/core/Logger.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/renderer/GLUtils.h"
#include "Ashen/renderer/Renderer.h"
#include "Ashen/resources/ResourceManager.h"

namespace ash {
    Application *Application::s_Instance = nullptr;

    Application::Application(ApplicationSettings settings)
        : m_Settings(std::move(settings)), m_Running(false) {
        if (s_Instance) {
            Logger::error("Application already exists!");
            return;
        }
        s_Instance = this;

        InitializeGLFW();
        InitializeResourceSystem();
        InitializeWindow();
        InitializeRenderer();
        InitializeInput();
    }

    Application::~Application() {
        Shutdown();
    }

    void Application::InitializeGLFW() {
        if (!glfwInit()) {
            Logger::error("Failed to initialize GLFW");
            throw std::runtime_error("GLFW initialization failed");
        }
    }

    void Application::InitializeResourceSystem() {
        ResourcePaths::Instance().SetWorkingDirectory(m_Settings.ResourceDirectory);
        AssetLibrary::Initialize();
    }

    void Application::InitializeWindow() {
        WindowProperties windowProperties;
        windowProperties.Title = m_Settings.Name + " v" + m_Settings.Version;

        m_Window = std::make_unique<Window>(windowProperties);
        m_Window->Create();

        m_Window->SetEventCallback([this](Event &e) {
            Input::OnEvent(e);
            OnEvent(e);
        });
    }

    void Application::InitializeRenderer() {
        InitOpenGLDebugMessageCallback();
        Renderer::Init();
    }

    void Application::InitializeInput() {
        Input::Init(*m_Window);
    }

    void Application::Shutdown() {
        AssetLibrary::ClearAll();
        Renderer::Shutdown();

        if (m_Window) {
            m_Window->Destroy();
        }

        glfwTerminate();
    }

    void Application::Run() {
        m_Running = true;
        float lastFrameTime = GetTime();

        while (m_Running && !m_Window->ShouldClose()) {
            const float time = GetTime();
            const float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            Tick(deltaTime);
        }
    }

    void Application::Tick(float deltaTime) {
        ProcessEvents();
        Update(deltaTime);
        Render();
        m_Window->SwapBuffers();
    }

    void Application::ProcessEvents() const {
        m_Window->PollEvents();
    }

    void Application::Update(float deltaTime) const {
        Input::Update();
        UpdateLayers(deltaTime);
    }

    void Application::Render() const {
        Renderer::BeginFrame();
        RenderLayers();
        Renderer::EndFrame();
    }

    void Application::Stop() {
        m_Running = false;
    }

    Vec2 Application::GetFramebufferSize() const {
        return m_Window->GetFramebufferSize();
    }

    Application &Application::Get() {
        return *s_Instance;
    }

    float Application::GetTime() {
        return static_cast<float>(glfwGetTime());
    }

    void Application::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &) {
            Stop();
            return true;
        });

        dispatcher.Dispatch<WindowResizeEvent>([](const WindowResizeEvent &e) {
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
            return false;
        });

        m_LayerStack.OnEvent(event);
    }

    void Application::UpdateLayers(const float ts) const {
        for (auto &layer: m_LayerStack) {
            layer->OnUpdate(ts);
        }
    }

    void Application::RenderLayers() const {
        for (auto &layer: m_LayerStack) {
            layer->OnRender();
        }
    }
}

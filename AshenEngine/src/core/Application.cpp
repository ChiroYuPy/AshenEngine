#include "Ashen/core/Application.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <utility>

#include "Ashen/core/Input.h"
#include "Ashen/core/Logger.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/renderer/GLUtils.h"
#include "Ashen/renderer/Renderer.h"
#include "Ashen/resources/ResourceSystem.h"

namespace pixl {
    Application *Application::s_Instance = nullptr;

    Application::Application(ApplicationSettings settings)
        : m_Settings(std::move(settings)), m_Running(true) {
        if (s_Instance) {
            Logger::error("Application already exists!");
            return;
        }
        s_Instance = this;

        if (!glfwInit()) {
            Logger::error("Failed to initialize GLFW");
            return;
        }

        ResourcePaths::Instance().SetWorkingDirectory("resources");
        AssetLibrary::Initialize();

        WindowProperties windowProperties;
        windowProperties.Title = m_Settings.Name + " v" + m_Settings.Version;
        m_Window = MakeScope<Window>(windowProperties);
        m_Window->Create();

        InitOpenGLDebugMessageCallback();
        Renderer::Init();
        Input::Init(*m_Window);

        m_Window->SetEventCallback([this](Event &e) {
            Input::OnEvent(e);
            OnEvent(e);
        });
    }

    Application::~Application() {
        Renderer::Shutdown();
        m_Window->Destroy();
        glfwTerminate();
    }

    void Application::Run() const {
        float lastFrameTime = GetTime();

        while (m_Running && !m_Window->ShouldClose()) {
            const float time = GetTime();
            const float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            m_Window->PollEvents();

            Input::Update();

            m_LayerStack.OnUpdate(deltaTime);

            Renderer::BeginFrame();
            m_LayerStack.OnRender();
            Renderer::EndFrame();

            m_Window->SwapBuffers();
        }
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
}

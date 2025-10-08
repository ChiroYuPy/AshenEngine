#include "Ashen/Core/Application.h"

#include <iostream>
#include <utility>

#include "Ashen/Core/Window.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Graphics/Rendering/Renderer.h"
#include "Ashen/Resources/ResourceManager.h"

#include <GLFW/glfw3.h>

namespace ash {
    Application::Application(ApplicationSettings settings)
        : m_Settings(std::move(settings)), m_Running(false) {
        if (s_Instance) {
            Logger::error("Application already exists!");
            return;
        }
        s_Instance = this;

        Logger::info() << "Application started: " << m_Settings.Name << " v" << m_Settings.Version;

        WindowProperties windowProperties;
        windowProperties.Title = m_Settings.Name + " v" + m_Settings.Version;
        m_Window = MakeScope<Window>(windowProperties);

        m_Window->SetEventCallback([this](Event &e) {
            Input::OnEvent(e);
            OnEvent(e);
        });

        ResourcePaths::Instance().SetWorkingDirectory(m_Settings.ResourceDirectory);
        AssetLibrary::Initialize();
        AssetLibrary::PreloadCommon();

        Renderer::Init();

        Input::Init(*m_Window);
    }

    Application::~Application() {
        Shutdown();
    }

    void Application::Run() {
        m_Running = true;
        float lastFrameTime = GetTime();

        SendDefaultEvents();

        while (m_Running && !m_Window->ShouldClose()) {
            const float time = GetTime();
            const float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            Tick(deltaTime);
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

    void Application::SendDefaultEvents() {
        WindowResizeEvent event(m_Window->GetWidth(), m_Window->GetHeight());
        OnEvent(event);
    }

    void Application::Shutdown() {
        m_LayerStack.Clear();
        AssetLibrary::ClearAll();
        Renderer::Shutdown();
    }

    void Application::Tick(const float deltaTime) const {
        m_Window->PollEvents();
        Update(deltaTime);
        Render();
        m_Window->Update();
    }

    void Application::Update(const float deltaTime) const {
        Input::Update();
        UpdateLayers(deltaTime);
    }

    void Application::Render() const {
        Renderer::BeginFrame();
        RenderLayers();
        Renderer::EndFrame();
    }

    void Application::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([](const WindowResizeEvent &e) {
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
            return false;
        });

        m_LayerStack.OnEvent(event);

        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &) {
            Stop();
            return true;
        });
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

    Application *Application::s_Instance = nullptr;
}
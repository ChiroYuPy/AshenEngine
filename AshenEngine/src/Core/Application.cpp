#include "Ashen/Core/Application.h"

#include <ranges>
#include <utility>

#include "Ashen/Core/Window.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Graphics/Rendering/Renderer.h"
#include "Ashen/Resources/ResourceManager.h"
#include "Ashen/Audio/AudioManager.h"
#include "Ashen/Events/EventDispatcher.h"
#include "Ashen/Core/Platform.h"

#include <GLFW/glfw3.h>

namespace ash {
    Application::Application(ApplicationSettings settings)
        : m_Settings(MovePtr(settings)), m_Running(false) {
        Logger::Get().SetMinLevel(m_Settings.MinLogLevel);

        if (s_Instance) {
            Logger::Error("Application already exists!");
            return;
        }
        s_Instance = this;

        WindowProperties windowProperties;
        windowProperties.Title = m_Settings.Name + " v" + m_Settings.Version;
        m_Window = MakeOwn<Window>(windowProperties);

        m_Window->SetEventCallback([this](Event &e) {
            Input::OnEvent(e);
            OnEvent(e);
        });

        m_NodeGraph = MakeOwn<NodeGraph>();

        ResourcePaths::Instance().SetWorkingDirectory(m_Settings.ResourceDirectory);
        AssetLibrary::Initialize();
        AssetLibrary::PreloadCommon();

        Renderer::Init();
        Input::Init(*m_Window);

        AudioManager::Get().Initialize(AudioDevice::Backend::MiniAudio);

        Logger::Info() << "Application started: " << m_Settings.Name << " v" << m_Settings.Version;
        Logger::Debug() << "=== System Info ===";
        Logger::Debug() << "Platform:          " << Platform::GetPlatformName();
        Logger::Debug() << "Architecture:      " << (Platform::Is64Bit() ? "64-bit" : "32-bit");
        Logger::Debug() << "CPU:               " << Platform::GetCPUName();
        Logger::Debug() << "Cores:             " << Platform::GetCPUCoreCount();
        Logger::Debug() << "RAM:               " << Platform::GetTotalRAM() / (1024 * 1024) << " MB";
        Logger::Debug() << "GPU:               " << Platform::GetGPUName() << " (Vendor: " << Platform::GetGPUVendor()
                << ")";
        Logger::Debug() << "Endianness:        " << (Platform::GetEndianness() == Platform::Endianness::Little
                                                         ? "Little"
                                                         : "Big");
        Logger::Debug() << "===================";
    }

    Application::~Application() {
        Shutdown();
    }

    void Application::Run() {
        m_Running = true;
        float lastFrameTime = GetTime();

        m_NodeGraph->Ready();

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

    void Application::PushLayer(Own<Layer> layer) {
        m_LayerStack.PushLayer(MovePtr(layer));
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
        Logger::Info("Shutting down application...");

        if (m_NodeGraph) {
            m_NodeGraph->Clear();
        }

        m_LayerStack.Clear();

        AudioManager::Get().Shutdown();

        AssetLibrary::ClearAll();

        Input::Shutdown();

        Renderer::Shutdown();

        Logger::Info("Application shut down successfully");
    }

    void Application::Tick(const float deltaTime) const {
        m_Window->PollEvents();

        Update(deltaTime);

        Render();

        m_Window->Update();
    }

    void Application::Update(const float deltaTime) const {
        Input::Update();

        AudioManager::Get().Update();

        if (m_NodeGraph)
            m_NodeGraph->Process(deltaTime);

        UpdateLayers(deltaTime);
    }

    void Application::Render() const {
        Renderer::BeginFrame();

        if (m_NodeGraph)
            m_NodeGraph->Draw();

        RenderLayers();

        Renderer::EndFrame();
    }

    void Application::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([](const WindowResizeEvent &e) -> bool {
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
            return false;
        });

        if (m_NodeGraph && !event.IsHandled())
            m_NodeGraph->DispatchEvent(event);

        if (!event.IsHandled())
            HandleEventsLayers(event);

        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &) -> bool {
            Stop();
            return true;
        });
    }

    void Application::UpdateLayers(const float ts) const {
        for (auto &layerPtr: m_LayerStack)
            if (layerPtr)
                layerPtr->OnUpdate(ts);
    }

    void Application::RenderLayers() const {
        for (auto &layerPtr: m_LayerStack)
            if (layerPtr)
                layerPtr->OnRender();
    }

    void Application::HandleEventsLayers(Event &event) const {
        for (auto &layerPtr: std::ranges::reverse_view(m_LayerStack)) {
            if (layerPtr && !event.IsHandled())
                layerPtr->OnEvent(event);

            if (event.IsHandled())
                break;
        }
    }

    Application *Application::s_Instance = nullptr;
}

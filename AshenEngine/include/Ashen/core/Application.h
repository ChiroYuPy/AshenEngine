#ifndef ASHEN_APPLICATION_H
#define ASHEN_APPLICATION_H

#include <string>

#include "Layer.h"
#include "LayerStack.h"
#include "Window.h"
#include "Ashen/math/Math.h"

namespace ash {
    struct ApplicationCommandLineArgs {
        int Count = 0;
        char **Args = nullptr;

        const char *operator[](const int index) const {
            return Args[index];
        }
    };

    struct ApplicationSettings {
        std::string Name = "Pixl Application";
        std::string Version = "0.0.1";
        std::string ResourceDirectory = "resources";
        ApplicationCommandLineArgs CommandLineArgs;
    };

    class Application {
    public:
        explicit Application(ApplicationSettings settings);

        ~Application();

        Application(const Application &) = delete;

        Application &operator=(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(Application &&) = delete;

        void Run();

        void Stop();

        template<typename T, typename... Args>
            requires std::is_base_of_v<Layer, T>
        T *PushLayer(Args &&... args) {
            return m_LayerStack.PushLayer<T>(std::forward<Args>(args)...);
        }

        void PopLayer() { m_LayerStack.PopLayer(); }

        [[nodiscard]] Window &GetWindow() const { return *m_Window; }

        [[nodiscard]] Vec2 GetFramebufferSize() const;

        static Application &Get();

        static float GetTime();

    private:
        void InitializeGLFW() const;

        void InitializeResourceSystem() const;

        void InitializeWindow();

        void InitializeRenderer() const;

        void InitializeInput() const;

        void Shutdown() const;

        void Tick(float deltaTime) const;

        void ProcessEvents() const;

        void Update(float deltaTime) const;

        void Render() const;

        void OnEvent(Event &event);

        void UpdateLayers(float ts) const;

        void RenderLayers() const;

        ApplicationSettings m_Settings;
        LayerStack m_LayerStack;
        std::unique_ptr<Window> m_Window;
        bool m_Running;

        static Application *s_Instance;
    };

    Application *CreateApplication(ApplicationCommandLineArgs args);
}

#endif //ASHEN_APPLICATION_H

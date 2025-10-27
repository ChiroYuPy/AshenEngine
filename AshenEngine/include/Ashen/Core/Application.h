#ifndef ASHEN_APPLICATION_H
#define ASHEN_APPLICATION_H

#include "Logger.h"
#include "Ashen/Core/Layer.h"
#include "Ashen/Core/LayerStack.h"
#include "Ashen/Core/Window.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    struct ApplicationCommandLineArgs {
        int Count = 0;
        char **Args = nullptr;

        const char *operator[](const int index) const {
            return Args[index];
        }
    };

    struct ApplicationSettings {
        String Name = "Ashen Application";
        String Version = "0.0.0";
        String ResourceDirectory = "resources";
        ApplicationCommandLineArgs CommandLineArgs;
        LogLevel MinLogLevel = LogLevel::Info;
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

        void PushLayer(Own<Layer> layer);

        void PopLayer() { m_LayerStack.PopLayer(); }

        [[nodiscard]] Window &GetWindow() const { return *m_Window; }

        [[nodiscard]] Vec2 GetFramebufferSize() const;

        static Application &Get();

        static float GetTime();

    private:
        void SendDefaultEvents();

        void Shutdown();

        void Tick(float deltaTime) const;

        void Update(float deltaTime) const;

        void Render() const;

        void OnEvent(Event &event);

        void UpdateLayers(float ts) const;

        void RenderLayers() const;

        ApplicationSettings m_Settings;
        LayerStack m_LayerStack;
        Own<Window> m_Window;
        bool m_Running;

        static Application *s_Instance;
    };

    Application *CreateApplication(ApplicationCommandLineArgs args);
}

#endif //ASHEN_APPLICATION_H
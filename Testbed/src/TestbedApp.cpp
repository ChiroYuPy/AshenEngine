#include "TestbedApp.h"

#include "layers/GameLayer.h"
#include "layers/TestLayer.h"
#include "layers/UILayer.h"

namespace ash {
    TestbedApp::TestbedApp(const ApplicationSettings &settings)
        : Application(settings) {
        PushLayer(MakeOwn<GameLayer>());
        // PushLayer(MakeOwn<TestLayer>());
    }
}

ash::Application *ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Ashen Testbed - Shader Showcase";
    settings.Version = "0.1.0";
    settings.CommandLineArgs = args;
    settings.MinLogLevel = LogLevel::Info;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"

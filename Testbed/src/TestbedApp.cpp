#include "TestbedApp.h"

namespace ash {
    TestbedApp::TestbedApp(const ApplicationSettings &settings) : Application(settings) {
        //TODO: push a layer
    }
}

ash::Application *ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Sandbox";
    settings.Version = "0.0.3";
    settings.CommandLineArgs = args;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"

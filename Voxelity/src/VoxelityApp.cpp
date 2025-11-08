#include "Voxelity/VoxelityApp.h"

#include "Voxelity/layers/UILayer.h"
#include "Voxelity/layers/VoxelWorldLayer.h"

namespace voxelity {
    VoxelityApp::VoxelityApp(const ash::ApplicationSettings &settings) : Application(settings) {
        PushLayer(ash::MakeOwn<VoxelWorldLayer>());
        PushLayer(ash::MakeOwn<UILayer>());
    }
}

ash::Application *ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Voxelity";
    settings.Version = "0.0.3";
    settings.CommandLineArgs = args;

    return new voxelity::VoxelityApp(settings);
}

#include "Ashen/Core/EntryPoint.h"
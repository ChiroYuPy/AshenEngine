#include "Voxelity/VoxelityApp.h"

#include "Voxelity/layers/UILayer.h"
#include "Voxelity/layers/VoxelWorldLayer.h"

namespace voxelity {
    VoxelityApp::VoxelityApp(const pixl::ApplicationSettings &settings) : Application(settings) {
        PushLayer<VoxelWorldLayer>();
        PushLayer<UILayer>();
    }
}

pixl::Application *pixl::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Sandbox";
    settings.Version = "0.0.3";
    settings.CommandLineArgs = args;

    return new voxelity::VoxelityApp(settings);
}

#include "Ashen/core/EntryPoint.h"
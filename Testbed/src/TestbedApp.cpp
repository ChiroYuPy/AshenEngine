#include "TestbedApp.h"

#include "layers/GameLayer.h"
#include "layers/NodeGraphTestLayer.h"
#include "layers/Renderer2DTestLayer.h"
#include "layers/FeaturesDemoLayer.h"
#include "layers/ImGuiDemoLayer.h"
#include "layers/PhysicsDemoLayer.h"

namespace ash {
    TestbedApp::TestbedApp(const ApplicationSettings &settings)
        : Application(settings) {
        // === PHYSICS DEMO ===
        // Demo des nouveaux systèmes: Physique, Particules
        PushLayer(MakeOwn<PhysicsDemoLayer>());

        // === IMGUI DEMO ===
        // Demo showing ImGui integration with Scene Hierarchy, Inspector, and Performance Monitor
        // PushLayer(MakeOwn<ImGuiDemoLayer>());

        // === DEMO DES NOUVELLES FONCTIONNALITÉS ===
        // Décommente cette ligne pour voir les nouvelles features !
        // PushLayer(MakeOwn<FeaturesDemoLayer>());

        // === DEMOS ORIGINALES ===
        // Commente la ligne au-dessus et décommente ci-dessous pour voir les demos originales
        // PushLayer(MakeOwn<GameLayer>());
        // PushLayer(MakeOwn<NodeGraphTestLayer>());
        // PushLayer(MakeOwn<Renderer2DTestLayer>());
    }
}

ash::Application *ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Ashen Engine - Physics Demo";
    settings.Version = "1.0.0";
    settings.CommandLineArgs = args;
    settings.MinLogLevel = LogLevel::Info;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"

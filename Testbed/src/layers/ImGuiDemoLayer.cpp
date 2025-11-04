#include "layers/ImGuiDemoLayer.h"

#include <imgui.h>
#include <Ashen/Core/Application.h>
#include <Ashen/Core/Logger.h>
#include <Ashen/Nodes/Node3D.h>
#include <Ashen/Graphics/Rendering/Renderer.h>
#include <Ashen/GraphicsAPI/RendererAPI.h>

#ifdef ASHEN_ENABLE_PROFILING
#include <Ashen/Utils/Profiler.h>
#endif

ImGuiDemoLayer::ImGuiDemoLayer()
    : Layer("ImGuiDemoLayer") {
}

ImGuiDemoLayer::~ImGuiDemoLayer() = default;

void ImGuiDemoLayer::OnAttach() {
    ASH_PROFILE_FUNCTION();

    // Create ImGui layer
    m_ImGuiLayer = ash::MakeOwn<ash::ImGuiLayer>();
    m_ImGuiLayer->OnAttach();

    // Setup 3D camera
    auto& window = ash::Application::Get().GetWindow();
    float aspectRatio = window.GetAspectRatio();
    m_Camera = ash::MakeOwn<ash::PerspectiveCamera>(60.0f, aspectRatio, 0.1f, 1000.0f);
    m_CameraController = ash::OrbitCameraController::Create(*m_Camera, ash::Vec3(0, 0, 0), 15.0f);

    // Setup scene
    SetupScene();

    ash::Logger::Info("ImGuiDemoLayer attached");
}

void ImGuiDemoLayer::OnDetach() {
    ASH_PROFILE_FUNCTION();

    m_ImGuiLayer->OnDetach();
    m_Scene.Clear();

    ash::Logger::Info("ImGuiDemoLayer detached");
}

void ImGuiDemoLayer::OnUpdate(float deltaTime) {
    ASH_PROFILE_FUNCTION();

    // Update camera
    m_CameraController->Update(deltaTime);

    // Update scene
    m_Scene.Process(deltaTime);
}

void ImGuiDemoLayer::OnRender() {
    ASH_PROFILE_FUNCTION();

    // Render scene
    m_Scene.Draw();

    // Render ImGui
    RenderImGui();
}

void ImGuiDemoLayer::RenderImGui() {
    ASH_PROFILE_FUNCTION();

    m_ImGuiLayer->Begin();

    // Render dockspace
    RenderDockspace();

    // Render all panels
    m_SceneHierarchy.Render(&m_Scene);
    m_Inspector.Render(m_SceneHierarchy.GetSelectedNode());
    m_PerformanceMonitor.Render();

    // Demo window
    ImGui::ShowDemoWindow();

    m_ImGuiLayer->End();
}

void ImGuiDemoLayer::OnEvent(ash::Event& event) {
    ASH_PROFILE_FUNCTION();

    m_ImGuiLayer->OnEvent(event);

    // Pass events to camera controller if ImGui didn't handle them
    if (!event.IsHandled()) {
        m_CameraController->OnEvent(event);
    }
}

void ImGuiDemoLayer::SetupScene() {
    ASH_PROFILE_FUNCTION();

    // Create root node
    auto root = ash::MakeOwn<ash::Node3D>("Root");

    // Create some child nodes to demonstrate hierarchy
    auto parent1 = ash::MakeOwn<ash::Node3D>("Parent 1");
    parent1->SetPosition(ash::Vec3(0, 0, 0));

    auto child1 = ash::MakeOwn<ash::Node3D>("Child 1.1");
    child1->SetPosition(ash::Vec3(2, 0, 0));

    auto child2 = ash::MakeOwn<ash::Node3D>("Child 1.2");
    child2->SetPosition(ash::Vec3(-2, 0, 0));

    auto grandchild = ash::MakeOwn<ash::Node3D>("Grandchild 1.1.1");
    grandchild->SetPosition(ash::Vec3(0, 2, 0));

    // Build hierarchy
    child1->AddChild(ash::MovePtr(grandchild));
    parent1->AddChild(ash::MovePtr(child1));
    parent1->AddChild(ash::MovePtr(child2));
    root->AddChild(ash::MovePtr(parent1));

    // Create another parent node
    auto parent2 = ash::MakeOwn<ash::Node3D>("Parent 2");
    parent2->SetPosition(ash::Vec3(5, 0, 0));
    parent2->SetScale(ash::Vec3(1.5f, 1.5f, 1.5f));

    auto child3 = ash::MakeOwn<ash::Node3D>("Child 2.1");
    child3->SetPosition(ash::Vec3(0, 0, 2));

    parent2->AddChild(ash::MovePtr(child3));
    root->AddChild(ash::MovePtr(parent2));

    // Add some standalone nodes
    auto standalone = ash::MakeOwn<ash::Node3D>("Standalone Node");
    standalone->SetPosition(ash::Vec3(0, 5, 0));
    root->AddChild(ash::MovePtr(standalone));

    // Set as scene root
    m_Scene.SetRoot(ash::MovePtr(root));
    m_Scene.Ready();

    ash::Logger::Info(ash::Format("Demo scene created with {} nodes", m_Scene.GetNodeCount()));
}

void ImGuiDemoLayer::RenderDockspace() {
    // Simple menu bar (docking not available in this ImGui version)
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                // Note: Application doesn't have a Close() method
                // This would need to be implemented
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::Text("Scene Hierarchy");
            ImGui::Text("Inspector");
            ImGui::Text("Performance Monitor");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::Text("AshenEngine ImGui Demo");
            ImGui::Separator();
            ImGui::TextDisabled("Controls:");
            ImGui::BulletText("Select nodes in Scene Hierarchy");
            ImGui::BulletText("Edit properties in Inspector");
            ImGui::BulletText("View performance in Monitor");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

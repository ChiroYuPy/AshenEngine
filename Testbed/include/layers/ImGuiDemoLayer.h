#ifndef TESTBED_IMGUIDEMOLAYER_H
#define TESTBED_IMGUIDEMOLAYER_H

#include <Ashen/Core/Layer.h>
#include <Ashen/Nodes/NodeGraph.h>
#include <Ashen/ImGui/ImGuiLayer.h>
#include <Ashen/ImGui/SceneHierarchyPanel.h>
#include <Ashen/ImGui/InspectorPanel.h>
#include <Ashen/ImGui/PerformanceMonitorPanel.h>
#include <Ashen/Graphics/Cameras/Camera.h>
#include <Ashen/Graphics/CameraControllers/OrbitCameraController.h>

/**
 * @brief Demo layer showing ImGui integration with Scene Hierarchy, Inspector, and Performance Monitor
 */
class ImGuiDemoLayer : public ash::Layer {
public:
    ImGuiDemoLayer();
    ~ImGuiDemoLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnEvent(ash::Event& event) override;

private:
    void SetupScene();
    void RenderImGui();
    void RenderDockspace();

    // Scene
    ash::NodeGraph m_Scene;

    // ImGui
    ash::Own<ash::ImGuiLayer> m_ImGuiLayer;
    ash::SceneHierarchyPanel m_SceneHierarchy;
    ash::InspectorPanel m_Inspector;
    ash::PerformanceMonitorPanel m_PerformanceMonitor;

    // Camera
    ash::Own<ash::PerspectiveCamera> m_Camera;
    ash::Ref<ash::OrbitCameraController> m_CameraController;
};

#endif // TESTBED_IMGUIDEMOLAYER_H

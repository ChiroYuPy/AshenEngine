#include "TestbedApp.h"

#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Camera/CameraController.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Resources/ResourceManager.h"

namespace ash {
    class SandboxLayer final : public Layer {
    public:
        SandboxLayer() {
            // Load mesh
            try {
                m_Mesh = AssetLibrary::Meshes().Load("sofa/sofa");
            } catch (const std::exception& e) {
                Logger::error() << "Failed to load mesh: " << e.what();
                m_Mesh = AssetLibrary::Meshes().GetCube(); // Fallback
            }

            m_CubeMesh = AssetLibrary::Meshes().GetCube();

            // Setup camera
            m_Camera = MakeRef<PerspectiveCamera>();
            m_Camera->SetPosition({5, 5, 5});
            m_Camera->LookAt({0, 0, 0});
            m_CameraController = MakeRef<CameraController>(m_Camera, 5.0f, 0.1f);

            // Create material
            m_Material = AssetLibrary::Materials().CreatePBR(
                "main_material",
                Vec3(0.8f, 0.4f, 0.3f), // red albedo
                0.0f,  // non-metallic
                0.5f   // medium roughness
            );

            m_CubeMaterial = AssetLibrary::Materials().CreatePBR(
                "cube_material",
                Vec3(0.2f, 0.6f, 0.8f), // blue albedo
                0.0f,
                0.5f
            );

            // Setup lights - simplified to 4 lights
            SetupLights();
        }

        void OnUpdate(float deltaTime) override {
            m_CameraController->OnUpdate(deltaTime);
        }

        void OnRender() override {
            // Begin scene
            Renderer3D::BeginScene(*m_Camera);

            // Setup lighting
            Renderer3D::ClearLights();
            for (size_t i = 0; i < m_LightPositions.size(); ++i) {
                PointLight light;
                light.position = m_LightPositions[i];
                light.color = m_LightColors[i];
                light.intensity = 1.0f;
                Renderer3D::AddPointLight(light);
            }

            // Render main mesh
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0));
            Renderer3D::Submit(m_Mesh, m_Material, transform);

            // Render cube
            transform = glm::translate(Mat4(1.0f), Vec3(-3, 0, 0));
            Renderer3D::Submit(m_CubeMesh, m_CubeMaterial, transform);

            // End scene
            Renderer3D::EndScene();

            // Optional: Display stats
            const auto& stats = Renderer3D::GetStats();
            if (m_ShowStats) {
                Logger::info() << "Draw Calls: " << stats.drawCalls
                              << " | Triangles: " << stats.triangles
                              << " | Vertices: " << stats.vertices;
            }
        }

        void OnEvent(Event& event) override {
            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e) {
                m_Camera->OnResize(e.GetWidth(), e.GetHeight());
                return false;
            });

            dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
                if (e.GetMouseButton() == Mouse::ButtonLeft) {
                    m_CameraController->SetActive(true);
                    Input::SetCursorMode(CursorMode::Disabled);
                    return true;
                }
                return false;
            });

            dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e) {
                if (e.GetKeyCode() == Key::Escape) {
                    m_CameraController->SetActive(false);
                    Input::SetCursorMode(CursorMode::Normal);
                    return true;
                }
                if (e.GetKeyCode() == Key::F1) {
                    m_ShowStats = !m_ShowStats;
                    return true;
                }
                if (e.GetKeyCode() == Key::F2) {
                    m_Wireframe = !m_Wireframe;
                    Renderer3D::SetWireframeMode(m_Wireframe);
                    return true;
                }
                return false;
            });

            dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent& e) {
                m_CameraController->OnMouseMove(e.GetX(), e.GetY());
                return false;
            });

            dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& e) {
                m_CameraController->OnMouseScroll(e.GetYOffset());
                return false;
            });
        }

    private:
        void SetupLights() {
            // 4 lights in a square pattern
            m_LightPositions = {
                Vec3(5.0f, 5.0f, 5.0f),
                Vec3(-5.0f, 5.0f, 5.0f),
                Vec3(-5.0f, 5.0f, -5.0f),
                Vec3(5.0f, 5.0f, -5.0f)
            };

            m_LightColors = {
                Vec3(300.0f, 100.0f, 100.0f),  // Red
                Vec3(100.0f, 300.0f, 100.0f),  // Green
                Vec3(100.0f, 100.0f, 300.0f),  // Blue
                Vec3(200.0f, 200.0f, 200.0f)   // White
            };
        }

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<PerspectiveCamera> m_Camera;

        // Meshes
        Ref<Mesh> m_Mesh;
        Ref<Mesh> m_CubeMesh;

        // Materials
        Ref<PBRMaterial> m_Material;
        Ref<PBRMaterial> m_CubeMaterial;

        // Lighting
        std::array<Vec3, 4> m_LightPositions;
        std::array<Vec3, 4> m_LightColors;

        // Debug
        bool m_ShowStats = false;
        bool m_Wireframe = false;
    };

    TestbedApp::TestbedApp(const ApplicationSettings& settings)
        : Application(settings) {
        PushLayer<SandboxLayer>();
    }
}

ash::Application* ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Sandbox";
    settings.Version = "0.1.0";
    settings.CommandLineArgs = args;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"
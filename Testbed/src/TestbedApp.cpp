#include "TestbedApp.h"

#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Camera/CameraController.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Resources/ResourceManager.h"

namespace ash {
    class SandboxLayer final : public Layer {
    public:
        SandboxLayer() {
            SetupMeshes();
            SetupCamera();
            SetupMaterials();
            SetupLights();
        }

        void OnUpdate(float deltaTime) override {
            m_CameraController->OnUpdate(deltaTime);

            // Animate objects
            m_Time += deltaTime;
            m_CubeRotation += deltaTime * 45.0f; // 45 degrees per second
        }

        void OnRender() override {
            // 3D Scene
            Render3DScene();

            // 2D Overlay (UI/HUD)
            if (m_ShowUI) {
                Render2DOverlay();
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
                if (e.GetKeyCode() == Key::F3) {
                    m_ShowUI = !m_ShowUI;
                    return true;
                }
                if (e.GetKeyCode() == Key::D0) {
                    m_CurrentScene = 0; // Basic materials
                    return true;
                }
                if (e.GetKeyCode() == Key::D1) {
                    m_CurrentScene = 1; // Multiple objects
                    return true;
                }
                if (e.GetKeyCode() == Key::D2) {
                    m_CurrentScene = 2; // Lighting demo
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
        void SetupMeshes() {
            // Load custom mesh or use primitives
            try {
                m_CustomMesh = AssetLibrary::Meshes().Load("sofa/sofa");
            } catch (const std::exception& e) {
                Logger::Warn() << "Could not load custom mesh: " << e.what();
                m_CustomMesh = AssetLibrary::Meshes().GetCube();
            }

            // Get primitive meshes
            m_CubeMesh = AssetLibrary::Meshes().GetCube();
            m_SphereMesh = AssetLibrary::Meshes().GetSphere();
            m_PlaneMesh = AssetLibrary::Meshes().GetPlane();
        }

        void SetupCamera() {
            m_Camera = MakeRef<PerspectiveCamera>();
            m_Camera->SetPosition({8, 6, 8});
            m_Camera->LookAt({0, 0, 0});
            m_CameraController = MakeRef<CameraController>(m_Camera, 5.0f, 0.1f);
        }

        void SetupMaterials() {
            // Spatial materials (3D with lighting)
            m_RedMaterial = AssetLibrary::Materials().CreateSpatial(
                "red_spatial",
                Vec4(0.8f, 0.2f, 0.2f, 1.0f), // Red albedo
                0.0f,  // Non-metallic
                0.6f,  // Medium roughness
                0.5f   // Medium specular
            );

            m_BlueMaterial = AssetLibrary::Materials().CreateSpatial(
                "blue_spatial",
                Vec4(0.2f, 0.4f, 0.8f, 1.0f), // Blue albedo
                0.0f,
                0.4f,
                0.5f
            );

            m_GreenMaterial = AssetLibrary::Materials().CreateSpatial(
                "green_spatial",
                Vec4(0.2f, 0.8f, 0.3f, 1.0f), // Green albedo
                0.0f,
                0.5f,
                0.5f
            );

            // Metallic material
            m_MetallicMaterial = AssetLibrary::Materials().CreateSpatial(
                "metallic_spatial",
                Vec4(0.9f, 0.9f, 0.9f, 1.0f), // Silver-ish
                0.9f,  // Very metallic
                0.2f,  // Low roughness (shiny)
                0.5f
            );

            // Rough material
            m_RoughMaterial = AssetLibrary::Materials().CreateSpatial(
                "rough_spatial",
                Vec4(0.6f, 0.5f, 0.4f, 1.0f), // Brown-ish
                0.0f,
                0.95f, // Very rough
                0.3f
            );

            // Unlit material (no lighting)
            m_UnlitMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_spatial",
                Vec4(1.0f, 0.8f, 0.2f, 1.0f) // Bright yellow
            );

            // Ground plane material
            m_GroundMaterial = AssetLibrary::Materials().CreateSpatial(
                "ground_spatial",
                Vec4(0.3f, 0.3f, 0.3f, 1.0f),
                0.0f,
                0.9f,
                0.2f
            );
        }

        void SetupLights() {
            // Main directional light (sun)
            m_DirectionalLight.direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
            m_DirectionalLight.color = Vec3(1.0f, 0.95f, 0.9f);
            m_DirectionalLight.intensity = 1.0f;

            // Point lights for dynamic lighting
            m_PointLights = {
                PointLight{Vec3(5.0f, 3.0f, 5.0f), Vec3(1.0f, 0.2f, 0.2f), 50.0f},  // Red
                PointLight{Vec3(-5.0f, 3.0f, 5.0f), Vec3(0.2f, 1.0f, 0.2f), 50.0f}, // Green
                PointLight{Vec3(-5.0f, 3.0f, -5.0f), Vec3(0.2f, 0.2f, 1.0f), 50.0f}, // Blue
                PointLight{Vec3(5.0f, 3.0f, -5.0f), Vec3(1.0f, 1.0f, 1.0f), 30.0f}   // White
            };
        }

        void Render3DScene() {
            Renderer3D::BeginScene(*m_Camera);

            // Setup lighting
            Renderer3D::SetDirectionalLight(m_DirectionalLight);
            Renderer3D::ClearLights();

            for (const auto& light : m_PointLights) {
                Renderer3D::AddPointLight(light);
            }

            Renderer3D::SetAmbientLight(Vec3(0.05f));

            // Render based on current scene
            switch (m_CurrentScene) {
                case 0: RenderBasicMaterialsScene(); break;
                case 1: RenderMultipleObjectsScene(); break;
                case 2: RenderLightingDemoScene(); break;
            }

            // Render ground plane
            Mat4 groundTransform = glm::translate(Mat4(1.0f), Vec3(0, -1, 0))
                                 * glm::scale(Mat4(1.0f), Vec3(20, 1, 20));
            Renderer3D::Submit(m_PlaneMesh, m_GroundMaterial, groundTransform);

            Renderer3D::EndScene();

            // Display stats
            if (m_ShowStats) {
                const auto& stats = Renderer3D::GetStats();
                Logger::Info() << "Scene " << m_CurrentScene + 1
                              << " | Draw Calls: " << stats.drawCalls
                              << " | Triangles: " << stats.triangles
                              << " | Vertices: " << stats.vertices;
            }
        }

        void RenderBasicMaterialsScene() {
            // Row of cubes with different materials
            float spacing = 3.0f;

            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-6, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_RedMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(-3, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_BlueMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_GreenMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(3, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_MetallicMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(6, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_RoughMaterial, transform);
        }

        void RenderMultipleObjectsScene() {
            // Central custom mesh
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.5f), Vec3(0, 1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(2.0f));
            Renderer3D::Submit(m_CustomMesh, m_BlueMaterial, transform);

            // Orbiting spheres
            for (int i = 0; i < 4; ++i) {
                float angle = m_Time + (i * glm::two_pi<float>() / 4.0f);
                float radius = 5.0f;
                Vec3 position(
                    std::cos(angle) * radius,
                    std::sin(m_Time * 2.0f + i) * 0.5f + 1.5f,
                    std::sin(angle) * radius
                );

                transform = glm::translate(Mat4(1.0f), position)
                          * glm::scale(Mat4(1.0f), Vec3(0.5f));

                // Alternate materials
                auto material = (i % 2 == 0) ? m_RedMaterial : m_GreenMaterial;
                Renderer3D::Submit(m_SphereMesh, material, transform);
            }

            // Unlit cube above (always bright)
            transform = glm::translate(Mat4(1.0f), Vec3(0, 4, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 2.0f), Vec3(1, 1, 0))
                      * glm::scale(Mat4(1.0f), Vec3(0.8f));
            Renderer3D::Submit(m_CubeMesh, m_UnlitMaterial, transform);
        }

        void RenderLightingDemoScene() {
            // Central sphere to show lighting
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0))
                           * glm::scale(Mat4(1.0f), Vec3(2.0f));
            Renderer3D::Submit(m_SphereMesh, m_MetallicMaterial, transform);

            // Small cubes at light positions
            for (size_t i = 0; i < m_PointLights.size(); ++i) {
                transform = glm::translate(Mat4(1.0f), m_PointLights[i].position)
                          * glm::scale(Mat4(1.0f), Vec3(0.3f));
                Renderer3D::Submit(m_CubeMesh, m_UnlitMaterial, transform);
            }

            // Ring of objects to show light falloff
            const int objectCount = 8;
            const float ringRadius = 6.0f;

            for (int i = 0; i < objectCount; ++i) {
                float angle = (i * glm::two_pi<float>()) / objectCount;
                Vec3 position(
                    std::cos(angle) * ringRadius,
                    0,
                    std::sin(angle) * ringRadius
                );

                transform = glm::translate(Mat4(1.0f), position)
                          * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation + i * 45.0f), Vec3(0, 1, 0));

                // Vary roughness around the ring
                float roughness = static_cast<float>(i) / objectCount;

                if (i % 2 == 0) {
                    Renderer3D::Submit(m_CubeMesh, m_BlueMaterial, transform);
                } else {
                    Renderer3D::Submit(m_SphereMesh, m_RedMaterial, transform);
                }
            }
        }

        void Render2DOverlay() {
            Renderer2D::BeginScene(*m_Camera);

            // Draw FPS counter
            const float padding = 10.0f;
            const float boxWidth = 200.0f;
            const float boxHeight = 100.0f;

            // Semi-transparent background
            Vec2 screenSize = Application::Get().GetWindow().GetSizeF();
            Renderer2D::DrawQuad(
                Vec2(padding + boxWidth * 0.5f, screenSize.y - padding - boxHeight * 0.5f),
                Vec2(boxWidth, boxHeight),
                Vec4(0.0f, 0.0f, 0.0f, 0.5f)
            );

            // Info text would go here (needs text rendering)
            // For now, just draw colored indicators
            float indicatorSize = 10.0f;
            float x = padding + 10.0f;
            float y = screenSize.y - padding - 20.0f;

            // Scene indicator
            for (int i = 0; i < 3; ++i) {
                Vec4 color = (i == m_CurrentScene) ? Vec4(0.2f, 1.0f, 0.3f, 1.0f) : Vec4(0.3f, 0.3f, 0.3f, 1.0f);
                Renderer2D::DrawQuad(
                    Vec2(x + i * (indicatorSize + 5.0f), y),
                    Vec2(indicatorSize, indicatorSize),
                    color
                );
            }

            // Stats indicator
            if (m_ShowStats) {
                Renderer2D::DrawCircle(
                    Vec2(x, y - 30.0f),
                    8.0f,
                    Vec4(1.0f, 0.8f, 0.2f, 1.0f)
                );
            }

            // Wireframe indicator
            if (m_Wireframe) {
                Renderer2D::DrawRect(
                    Vec2(x + 20.0f, y - 30.0f),
                    Vec2(16.0f, 16.0f),
                    Vec4(1.0f, 0.2f, 0.2f, 1.0f)
                );
            }

            Renderer2D::EndScene();
        }

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<PerspectiveCamera> m_Camera;

        // Meshes
        Ref<Mesh> m_CustomMesh;
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;
        Ref<Mesh> m_PlaneMesh;

        // Materials (Godot-style naming)
        Ref<SpatialMaterial> m_RedMaterial;
        Ref<SpatialMaterial> m_BlueMaterial;
        Ref<SpatialMaterial> m_GreenMaterial;
        Ref<SpatialMaterial> m_MetallicMaterial;
        Ref<SpatialMaterial> m_RoughMaterial;
        Ref<SpatialMaterial> m_UnlitMaterial;
        Ref<SpatialMaterial> m_GroundMaterial;

        // Lighting
        DirectionalLight m_DirectionalLight;
        std::vector<PointLight> m_PointLights;

        // State
        int m_CurrentScene = 0;
        bool m_ShowStats = false;
        bool m_ShowUI = true;
        bool m_Wireframe = false;
        float m_Time = 0.0f;
        float m_CubeRotation = 0.0f;
    };

    TestbedApp::TestbedApp(const ApplicationSettings& settings)
        : Application(settings) {
        PushLayer<SandboxLayer>();
    }
}

ash::Application* ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Ashen Testbed";
    settings.Version = "0.1.0";
    settings.CommandLineArgs = args;

    Logger::Get().SetMinLevel(LogLevel::Warn);

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"
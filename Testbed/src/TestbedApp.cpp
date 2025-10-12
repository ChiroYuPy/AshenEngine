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
            SetupMeshes();
            SetupCamera();
            SetupMaterials();
            SetupLights();
        }

        void OnUpdate(float deltaTime) override {
            m_CameraController->OnUpdate(deltaTime);
            m_Time += deltaTime;
        }

        void OnRender() override {
            Renderer3D::BeginScene(*m_Camera);
            Renderer3D::SetDirectionalLight(m_DirectionalLight);
            Renderer3D::ClearLights();
            Renderer3D::AddPointLight(m_PointLight);
            Renderer3D::SetAmbientLight(Vec3(0.1f));

            // Mesh statiques au centre (ne bougent pas)
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-2, 0, 0));
            Renderer3D::Submit(m_CustomMesh, m_PBRMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(2, 0, 0));
            Renderer3D::Submit(m_CustomMesh, m_ToonMaterial, transform);

            // Cubes statiques un peu plus loin
            transform = glm::translate(Mat4(1.0f), Vec3(-4, 0, -6));
            Renderer3D::Submit(m_CubeMesh, m_PBRMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(4, 0, -6));
            Renderer3D::Submit(m_CubeMesh, m_ToonMaterial, transform);

            // Balles qui bougent (3 balles en orbite)
            for (int i = 0; i < 3; ++i) {
                const float angle = m_Time + (i * glm::two_pi<float>() / 3.0f);
                Vec3 pos(std::cos(angle) * 5.0f,
                         std::sin(m_Time * 2.0f + i) * 0.5f + 1.0f,
                         std::sin(angle) * 5.0f);
                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::scale(Mat4(1.0f), Vec3(0.5f));

                // CORRECTION: Utiliser le type de base Material
                Ref<Material> mat = (i == 0) ? m_PBRMaterial : m_ToonMaterial;
                Renderer3D::Submit(m_SphereMesh, mat, transform);
            }

            // Sol
            transform = glm::translate(Mat4(1.0f), Vec3(0, -1, 0))
                      * glm::scale(Mat4(1.0f), Vec3(20, 1, 20));
            Renderer3D::Submit(m_PlaneMesh, m_PBRMaterial, transform);

            Renderer3D::EndScene();
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
            try {
                m_CustomMesh = AssetLibrary::Meshes().Load("sofa/sofa");
            } catch (const std::exception& e) {
                Logger::Warn() << "Could not load custom mesh: " << e.what();
                m_CustomMesh = AssetLibrary::Meshes().GetCube();
            }

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
            // Un seul matériau PBR (Spatial)
            m_PBRMaterial = AssetLibrary::Materials().CreateSpatial(
                "pbr_material",
                Vec4(0.6f, 0.4f, 0.8f, 1.0f),
                0.3f, 0.5f, 0.5f
            );

            // Un seul matériau Toon
            m_ToonMaterial = AssetLibrary::Materials().CreateToon(
                "toon_material",
                Vec4(0.2f, 0.8f, 0.4f, 1.0f),
                4, 0.7f
            );
        }

        void SetupLights() {
            m_DirectionalLight.direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
            m_DirectionalLight.color = Vec3(1.0f, 0.95f, 0.9f);
            m_DirectionalLight.intensity = 1.0f;

            m_PointLight = PointLight{
                Vec3(5.0f, 3.0f, 5.0f),
                Vec3(1.0f, 0.8f, 0.6f),
                50.0f
            };
        }

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<PerspectiveCamera> m_Camera;

        // Meshes
        Ref<Mesh> m_CustomMesh;
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;
        Ref<Mesh> m_PlaneMesh;

        Ref<Material> m_PBRMaterial;
        Ref<Material> m_ToonMaterial;

        // Lighting
        DirectionalLight m_DirectionalLight;
        PointLight m_PointLight;

        // State
        float m_Time = 0.0f;
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

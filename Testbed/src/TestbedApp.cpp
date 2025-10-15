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

        void OnUpdate(const float deltaTime) override {
            m_CameraController->OnUpdate(deltaTime);
            m_Time += deltaTime;
        }

        void OnRender() override {
            Renderer3D::BeginScene(*m_Camera);
            Renderer3D::SetDirectionalLight(m_DirectionalLight);
            Renderer3D::ClearLights();

            // Ajouter les point lights
            for (const auto& light : m_PointLights) {
                Renderer3D::AddPointLight(light);
            }

            Renderer3D::SetAmbientLight(Vec3(0.05f));

            // ========== Zone 1: Shader Spatial (PBR) - Gauche ==========
            RenderSpatialZone();

            // ========== Zone 2: Shader Toon - Centre ==========
            RenderToonZone();

            // ========== Zone 3: Shader Unlit - Droite ==========
            RenderUnlitZone();

            // ========== Sol ==========
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, -1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(60, 1, 30));
            Renderer3D::Submit(m_PlaneMesh, m_GroundMaterial, transform);

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
                if (e.GetKeyCode() == Key::F1) {
                    m_ShowWireframe = !m_ShowWireframe;
                    Renderer3D::SetWireframeMode(m_ShowWireframe);
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
        // ========== ZONE 1: SPATIAL (PBR) ==========
        void RenderSpatialZone() {
            const float zoneX = -20.0f;  // Position X de la zone

            // Sofa central (10 unités de long)
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CustomMesh, m_SpatialMaterial, transform);

            // Cubes statiques (4 coins autour du sofa)
            const float cubeOffset = 8.0f;

            // Cube avant-gauche (métal)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_MetallicMaterial, transform);

            // Cube avant-droite (rugueux)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_RoughMaterial, transform);

            // Cube arrière-gauche (rouge)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_RedMaterial, transform);

            // Cube arrière-droite (bleu)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_BlueMaterial, transform);

            // Sphères en orbite (3 sphères)
            constexpr int nbSphere = 32;
            for (int i = 0; i < nbSphere; ++i) {
                const float angle = m_Time * 0.5f + (i * glm::two_pi<float>() / static_cast<float>(nbSphere));
                const float radius = 6.0f;
                Vec3 pos(
                    zoneX + std::cos(angle) * radius,
                    std::sin(m_Time * 2.0f + i) * 0.8f + 1.5f,
                    std::sin(angle) * radius
                );

                Mat4 transform = glm::translate(Mat4(1.0f), pos)
                                * glm::scale(Mat4(1.0f), Vec3(0.6f));

                Ref<Material> mat;
                switch (i % 3) {
                    case 0: mat = m_SpatialMaterial; break;
                    case 1: mat = m_MetallicMaterial; break;
                    case 2: mat = m_RoughMaterial; break;
                }

                Renderer3D::Submit(m_SphereMesh, mat, transform);
            }
        }

        // ========== ZONE 2: TOON ==========
        void RenderToonZone() {
            const float zoneX = 0.0f;  // Centre

            // Sofa central (10 unités de long)
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CustomMesh, m_ToonMaterial, transform);

            // Cubes statiques (4 coins) - Différents niveaux de toon
            const float cubeOffset = 8.0f;

            // Cube avant-gauche (Toon 3 niveaux)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_ToonMaterial, transform);

            // Cube avant-droite (Toon 4 niveaux)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_ToonMaterial2, transform);

            // Cube arrière-gauche (Toon 5 niveaux)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_ToonMaterial3, transform);

            // Cube arrière-droite (Toon rouge)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_ToonRedMaterial, transform);

            // Sphères en orbite inversée (3 sphères)
            for (int i = 0; i < 3; ++i) {
                const float angle = -m_Time * 0.7f + (i * glm::two_pi<float>() / 3.0f);
                const float radius = 6.0f;
                Vec3 pos(
                    zoneX + std::cos(angle) * radius,
                    std::abs(std::sin(m_Time * 1.5f + i)) * 1.2f + 1.0f,
                    std::sin(angle) * radius
                );

                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::scale(Mat4(1.0f), Vec3(0.6f));

                Ref<Material> mat = (i % 2 == 0) ? m_ToonMaterial : m_ToonRedMaterial;
                Renderer3D::Submit(m_SphereMesh, mat, transform);
            }
        }

        // ========== ZONE 3: UNLIT ==========
        void RenderUnlitZone() {
            const float zoneX = 20.0f;  // Droite

            // Sofa central (10 unités de long)
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CustomMesh, m_UnlitMaterial, transform);

            // Cubes statiques (4 coins) - Différentes couleurs vives
            const float cubeOffset = 8.0f;

            // Cube avant-gauche (Jaune)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_UnlitYellowMaterial, transform);

            // Cube avant-droite (Cyan)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_UnlitCyanMaterial, transform);

            // Cube arrière-gauche (Magenta)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_UnlitMagentaMaterial, transform);

            // Cube arrière-droite (Orange)
            transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
            Renderer3D::Submit(m_CubeMesh, m_UnlitOrangeMaterial, transform);

            // Sphères oscillantes (3 sphères qui montent/descendent)
            for (int i = 0; i < 3; ++i) {
                const float heightOffset = std::sin(m_Time * 2.0f + i * 1.0f) * 2.0f;
                Vec3 pos(
                    zoneX + (i - 1) * 3.0f,  // Alignées horizontalement
                    heightOffset + 2.0f,
                    -3.0f
                );

                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::rotate(Mat4(1.0f), m_Time + i, Vec3(1, 1, 0))
                          * glm::scale(Mat4(1.0f), Vec3(0.6f));

                Ref<Material> mat = (i == 0) ? m_UnlitMaterial :
                                    (i == 1) ? m_UnlitYellowMaterial : m_UnlitCyanMaterial;
                Renderer3D::Submit(m_SphereMesh, mat, transform);
            }
        }

        // ========== SETUP ==========

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
            // Position pour voir les 3 zones
            m_Camera->SetPosition({0, 15, 25});
            m_Camera->LookAt({0, 0, 0});
            m_CameraController = MakeRef<CameraController>(m_Camera, 8.0f, 0.1f);
        }

        void SetupMaterials() {
            // ========== ZONE 1: MATÉRIAUX SPATIAL (PBR) ==========

            // Matériau principal
            m_SpatialMaterial = AssetLibrary::Materials().CreateSpatial(
                "spatial_main",
                Vec4(0.7f, 0.5f, 0.8f, 1.0f),  // Violet
                0.2f,  // Peu métallique
                0.5f,  // Rugosité moyenne
                0.6f
            );

            // Matériau métallique
            m_MetallicMaterial = AssetLibrary::Materials().CreateSpatial(
                "spatial_metallic",
                Vec4(0.8f, 0.8f, 0.8f, 1.0f),  // Gris
                0.9f,  // Très métallique
                0.2f,  // Peu rugueux (brillant)
                0.8f
            );

            // Matériau rugueux
            m_RoughMaterial = AssetLibrary::Materials().CreateSpatial(
                "spatial_rough",
                Vec4(0.6f, 0.5f, 0.4f, 1.0f),  // Brun
                0.0f,  // Pas métallique
                0.9f,  // Très rugueux
                0.3f
            );

            // Matériaux colorés
            m_RedMaterial = AssetLibrary::Materials().CreateSpatial(
                "spatial_red",
                Vec4(0.9f, 0.2f, 0.2f, 1.0f),
                0.0f, 0.6f, 0.5f
            );

            m_BlueMaterial = AssetLibrary::Materials().CreateSpatial(
                "spatial_blue",
                Vec4(0.2f, 0.4f, 0.9f, 1.0f),
                0.0f, 0.5f, 0.5f
            );

            // ========== ZONE 2: MATÉRIAUX TOON ==========

            // Toon vert (3 niveaux)
            m_ToonMaterial = AssetLibrary::Materials().CreateToon(
                "toon_green",
                Vec4(0.3f, 0.9f, 0.4f, 1.0f),
                3,      // 3 niveaux
                0.7f    // Rim amount
            );

            // Toon bleu (4 niveaux)
            m_ToonMaterial2 = AssetLibrary::Materials().CreateToon(
                "toon_blue",
                Vec4(0.3f, 0.5f, 0.9f, 1.0f),
                4,      // 4 niveaux
                0.8f
            );

            // Toon cyan (5 niveaux)
            m_ToonMaterial3 = AssetLibrary::Materials().CreateToon(
                "toon_cyan",
                Vec4(0.2f, 0.8f, 0.9f, 1.0f),
                5,      // 5 niveaux (plus lisse)
                0.6f
            );

            // Toon rouge
            m_ToonRedMaterial = AssetLibrary::Materials().CreateToon(
                "toon_red",
                Vec4(0.9f, 0.3f, 0.3f, 1.0f),
                3,
                0.7f
            );

            // ========== ZONE 3: MATÉRIAUX UNLIT ==========

            m_UnlitMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_white",
                Vec4(1.0f, 1.0f, 1.0f, 1.0f)
            );

            m_UnlitYellowMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_yellow",
                Vec4(1.0f, 0.9f, 0.2f, 1.0f)
            );

            m_UnlitCyanMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_cyan",
                Vec4(0.2f, 0.9f, 0.9f, 1.0f)
            );

            m_UnlitMagentaMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_magenta",
                Vec4(0.9f, 0.2f, 0.9f, 1.0f)
            );

            m_UnlitOrangeMaterial = AssetLibrary::Materials().CreateSpatialUnlit(
                "unlit_orange",
                Vec4(1.0f, 0.5f, 0.1f, 1.0f)
            );

            // ========== SOL ==========

            m_GroundMaterial = AssetLibrary::Materials().CreateSpatial(
                "ground",
                Vec4(0.25f, 0.25f, 0.25f, 1.0f),
                0.0f, 0.9f, 0.2f
            );
        }

        void SetupLights() {
            // Lumière directionnelle principale (soleil)
            m_DirectionalLight.direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
            m_DirectionalLight.color = Vec3(1.0f, 0.95f, 0.9f);
            m_DirectionalLight.intensity = 1.0f;

            // Point lights pour chaque zone
            m_PointLights = {
                // Zone Spatial (gauche) - Lumière bleue
                PointLight{
                    Vec3(-20.0f, 4.0f, 0.0f),
                    Vec3(0.3f, 0.5f, 1.0f),
                    60.0f
                },
                // Zone Toon (centre) - Lumière verte
                PointLight{
                    Vec3(0.0f, 4.0f, 0.0f),
                    Vec3(0.3f, 1.0f, 0.5f),
                    60.0f
                },
                // Zone Unlit (droite) - Lumière orange
                PointLight{
                    Vec3(20.0f, 4.0f, 0.0f),
                    Vec3(1.0f, 0.6f, 0.2f),
                    60.0f
                }
            };
        }

        // ========== MEMBRES ==========

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<PerspectiveCamera> m_Camera;

        // Meshes
        Ref<Mesh> m_CustomMesh;
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;
        Ref<Mesh> m_PlaneMesh;

        // Matériaux SPATIAL (PBR) - Zone 1
        Ref<Material> m_SpatialMaterial;
        Ref<Material> m_MetallicMaterial;
        Ref<Material> m_RoughMaterial;
        Ref<Material> m_RedMaterial;
        Ref<Material> m_BlueMaterial;

        // Matériaux TOON - Zone 2
        Ref<Material> m_ToonMaterial;
        Ref<Material> m_ToonMaterial2;
        Ref<Material> m_ToonMaterial3;
        Ref<Material> m_ToonRedMaterial;

        // Matériaux UNLIT - Zone 3
        Ref<Material> m_UnlitMaterial;
        Ref<Material> m_UnlitYellowMaterial;
        Ref<Material> m_UnlitCyanMaterial;
        Ref<Material> m_UnlitMagentaMaterial;
        Ref<Material> m_UnlitOrangeMaterial;

        // Sol
        Ref<Material> m_GroundMaterial;

        // Lighting
        DirectionalLight m_DirectionalLight;
        Vector<PointLight> m_PointLights;

        // State
        float m_Time = 0.0f;
        bool m_ShowWireframe = false;
    };

    TestbedApp::TestbedApp(const ApplicationSettings& settings)
        : Application(settings) {
        PushLayer<SandboxLayer>();
    }
}

ash::Application* ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Ashen Testbed - Shader Showcase";
    settings.Version = "0.1.0";
    settings.CommandLineArgs = args;

    Logger::Get().SetMinLevel(LogLevel::Warn);

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"
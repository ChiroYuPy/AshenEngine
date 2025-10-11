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
                if (e.GetKeyCode() == Key::D3) {
                    m_CurrentScene = 3; // Toon scene
                    return true;
                }
                if (e.GetKeyCode() == Key::D4) {
                    m_CurrentScene = 4; // Mixed scene (Toon + Spatial)
                    return true;
                }
                if (e.GetKeyCode() == Key::D5) {
                    m_CurrentScene = 5; // Mixed Materials
                    return true;
                }
                if (e.GetKeyCode() == Key::D6) {
                    m_CurrentScene = 6; // Performance Test
                    return true;
                }
                if (e.GetKeyCode() == Key::D7) {
                    m_CurrentScene = 7; // Complete Demo
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
            // Materials Spatial existants...
            m_RedMaterial = AssetLibrary::Materials().CreateSpatial(
                "red_spatial",
                Vec4(0.8f, 0.2f, 0.2f, 1.0f),
                0.0f, 0.6f, 0.5f
            );

            m_BlueMaterial = AssetLibrary::Materials().CreateSpatial(
                "blue_spatial",
                Vec4(0.2f, 0.4f, 0.8f, 1.0f),
                0.0f, 0.4f, 0.5f
            );

            m_GreenMaterial = AssetLibrary::Materials().CreateSpatial(
                "green_spatial",
                Vec4(0.2f, 0.8f, 0.3f, 1.0f),
                0.0f, 0.5f, 0.5f
            );

            // NOUVEAUX: Materials Toon
            m_ToonRedMaterial = AssetLibrary::Materials().CreateToon(
                "toon_red",
                Vec4(0.9f, 0.2f, 0.2f, 1.0f),
                3,      // 3 niveaux de shading
                0.716f  // Rim light amount
            );

            m_ToonBlueMaterial = AssetLibrary::Materials().CreateToon(
                "toon_blue",
                Vec4(0.2f, 0.4f, 0.9f, 1.0f),
                4,      // 4 niveaux de shading
                0.8f    // Plus de rim light
            );

            m_ToonGreenMaterial = AssetLibrary::Materials().CreateToon(
                "toon_green",
                Vec4(0.2f, 0.9f, 0.3f, 1.0f),
                5,      // 5 niveaux (plus lisse)
                0.5f
            );

            // Personnaliser les paramètres du shader toon
            const auto toonMat = m_ToonRedMaterial;
            toonMat->SetOutlineColor(Vec3(0.1f, 0.0f, 0.0f));  // Outline rouge foncé
            toonMat->SetOutlineThickness(0.05f);
            toonMat->SetSpecularGlossiness(64.0f);  // Specular plus net
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

        void RenderMaterialShowcaseScene() const {
            // std::array pour les matériaux Spatial
            const std::array<Ref<SpatialMaterial>, 5> materials = {
                m_RedMaterial, m_BlueMaterial, m_GreenMaterial,
                m_MetallicMaterial, m_RoughMaterial
            };

            // Cubes Spatial en ligne
            for (int i = 0; i < 5; ++i) {
                Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-6.0f + i * 3.0f, 0, -2))
                               * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
                Renderer3D::Submit(m_CubeMesh, materials[i], transform);
            }

            // std::array pour les matériaux Toon
            const std::array<Ref<ToonMaterial>, 3> toonMats = {
                m_ToonRedMaterial, m_ToonBlueMaterial, m_ToonGreenMaterial
            };

            for (int i = 0; i < 3; ++i) {
                Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-3.0f + i * 3.0f, 0, 2))
                               * glm::scale(Mat4(1.0f), Vec3(0.8f));
                Renderer3D::Submit(m_SphereMesh, toonMats[i], transform);
            }
        }

        void RenderPrimitiveMeshesScene() const {
            // Custom mesh au centre
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 1, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.5f), Vec3(0, 1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(1.5f));
            Renderer3D::Submit(m_CustomMesh, m_BlueMaterial, transform);

            // Primitives et matériaux en cercle
            const std::array<Ref<Mesh>, 4> meshes = {
                m_CubeMesh, m_SphereMesh, m_CubeMesh, m_SphereMesh
            };

            const std::array<Ref<Material>, 4> materials = {
                m_RedMaterial, m_GreenMaterial, m_ToonBlueMaterial, m_ToonRedMaterial
            };

            for (int i = 0; i < 4; ++i) {
                const float angle = i * glm::half_pi<float>();
                Vec3 pos(std::cos(angle) * 5.0f, 0, std::sin(angle) * 5.0f);
                transform = glm::translate(Mat4(1.0f), pos);
                Renderer3D::Submit(meshes[i], materials[i], transform);
            }
        }

        void RenderMotionTransformScene() const {
            // Mesh central rotatif
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 1, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(1.2f));
            Renderer3D::Submit(m_CustomMesh, m_MetallicMaterial, transform);

            // Sphères en orbite avec bobbing
            for (int i = 0; i < 4; ++i) {
                const float angle = m_Time * 0.5f + (i * glm::half_pi<float>());
                Vec3 pos(std::cos(angle) * 4.0f,
                         std::sin(m_Time * 2.0f + i) * 0.8f + 1.5f,
                         std::sin(angle) * 4.0f);
                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::scale(Mat4(1.0f), Vec3(0.5f));
                Renderer3D::Submit(m_SphereMesh, m_ToonBlueMaterial, transform);
            }

            // Cubes oscillants
            for (int i = 0; i < 3; ++i) {
                const float speed = 1.0f + i * 0.5f;
                const float height = std::sin(m_Time * speed) * 2.0f + 2.0f;
                transform = glm::translate(Mat4(1.0f), Vec3(-4.0f + i * 4.0f, height, -5))
                          * glm::rotate(Mat4(1.0f), glm::radians(m_Time * 45.0f), Vec3(1, 0, 1));
                Renderer3D::Submit(m_CubeMesh, m_GreenMaterial, transform);
            }
        }

        void RenderPerformanceTestScene() {
            // Grille 10x10 de cubes
            for (int x = 0; x < 10; ++x) {
                for (int z = 0; z < 10; ++z) {
                    Vec3 pos(-15.0f + x * 3.0f, 0, -15.0f + z * 3.0f);
                    Mat4 transform = glm::translate(Mat4(1.0f), pos)
                                   * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation + x * 10.0f), Vec3(0, 1, 0));

                    auto mat = ((x + z) % 2 == 0) ? m_RedMaterial : m_BlueMaterial;
                    Renderer3D::Submit(m_CubeMesh, mat, transform);
                }
            }

            m_ShowStats = true; // Force affichage stats
        }

        void RenderShowcaseScene() const {
            // Composition artistique complète
            // Centre: custom mesh avec lumière dramatique
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 1.5, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.3f), Vec3(0, 1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(2.0f));
            Renderer3D::Submit(m_CustomMesh, m_ToonBlueMaterial, transform);

            // Satellites toon
            for (int i = 0; i < 6; ++i) {
                const float angle = m_Time * 0.2f + (i * glm::two_pi<float>() / 6.0f);
                Vec3 pos(std::cos(angle) * 6.0f, std::sin(i) * 1.5f + 1.0f, std::sin(angle) * 6.0f);
                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::scale(Mat4(1.0f), Vec3(0.6f));
                auto mat = (i % 3 == 0) ? m_ToonRedMaterial :
                           (i % 3 == 1) ? m_ToonGreenMaterial : m_ToonBlueMaterial;
                Renderer3D::Submit(m_SphereMesh, mat, transform);
            }

            // Objets décoratifs PBR
            for (int i = 0; i < 4; ++i) {
                Vec3 pos((i % 2) * 8.0f - 4.0f, 0.5f, (i / 2) * 8.0f - 4.0f);
                transform = glm::translate(Mat4(1.0f), pos)
                          * glm::rotate(Mat4(1.0f), glm::radians(m_Time * 20.0f), Vec3(0, 1, 0));
                Renderer3D::Submit(m_CubeMesh, m_MetallicMaterial, transform);
            }
        }

        void Render3DScene() {
            Renderer3D::BeginScene(*m_Camera);
            Renderer3D::SetDirectionalLight(m_DirectionalLight);
            Renderer3D::ClearLights();

            for (const auto& light : m_PointLights) {
                Renderer3D::AddPointLight(light);
            }

            Renderer3D::SetAmbientLight(Vec3(0.05f));

            switch (m_CurrentScene) {
                case 0: RenderMaterialShowcaseScene(); break;
                case 1: RenderPrimitiveMeshesScene(); break;
                case 2: RenderLightingDemoScene(); break; // Garder l'existante
                case 3: RenderMotionTransformScene(); break;
                case 4: RenderToonScene(); break; // Garder l'existante mais renommer
                case 5: RenderMixedScene(); break; // Garder l'existante
                case 6: RenderPerformanceTestScene(); break;
                case 7: RenderShowcaseScene(); break;
            }

            // Ground plane
            const Mat4 groundTransform = glm::translate(Mat4(1.0f), Vec3(0, -1, 0))
                                 * glm::scale(Mat4(1.0f), Vec3(30, 1, 30));
            Renderer3D::Submit(m_PlaneMesh, m_GroundMaterial, groundTransform);

            Renderer3D::EndScene();
        }

        void RenderToonScene() const {
            // Rangée de cubes avec shader toon
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-4, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_ToonRedMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_ToonBlueMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(4, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_ToonGreenMaterial, transform);

            // Sphères en orbite avec shader toon
            for (int i = 0; i < 4; ++i) {
                const float angle = m_Time + (i * glm::two_pi<float>() / 4.0f);
                const float radius = 5.0f;
                Vec3 position(
                    std::cos(angle) * radius,
                    std::sin(m_Time * 2.0f + i) * 0.5f + 1.5f,
                    std::sin(angle) * radius
                );

                transform = glm::translate(Mat4(1.0f), position)
                          * glm::scale(Mat4(1.0f), Vec3(0.8f));

                auto material = (i % 3 == 0) ? m_ToonRedMaterial :
                               (i % 3 == 1) ? m_ToonBlueMaterial :
                                              m_ToonGreenMaterial;

                Renderer3D::Submit(m_SphereMesh, material, transform);
            }

            // Grand objet central pour bien voir les effets
            transform = glm::translate(Mat4(1.0f), Vec3(0, 2, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.5f), Vec3(0, 1, 0))
                      * glm::scale(Mat4(1.0f), Vec3(1.5f));
            Renderer3D::Submit(m_CustomMesh, m_ToonBlueMaterial, transform);
        }

        void RenderMixedScene() const {
            // Objets Spatial (réalistes)
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(-6, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_MetallicMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(-3, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_SphereMesh, m_BlueMaterial, transform);

            // Objets Toon (stylisés)
            transform = glm::translate(Mat4(1.0f), Vec3(3, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_CubeMesh, m_ToonRedMaterial, transform);

            transform = glm::translate(Mat4(1.0f), Vec3(6, 0, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation), Vec3(0, 1, 0));
            Renderer3D::Submit(m_SphereMesh, m_ToonBlueMaterial, transform);

            // Objet central hybride
            transform = glm::translate(Mat4(1.0f), Vec3(0, 1, 0))
                      * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.7f), Vec3(1, 1, 0))
                      * glm::scale(Mat4(1.0f), Vec3(1.2f));
            Renderer3D::Submit(m_CustomMesh, m_ToonGreenMaterial, transform);
        }

        void RenderBasicMaterialsScene() const {
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

        void RenderMultipleObjectsScene() const {
            // Central custom mesh
            Mat4 transform = glm::translate(Mat4(1.0f), Vec3(0, 0, 0))
                           * glm::rotate(Mat4(1.0f), glm::radians(m_CubeRotation * 0.5f), Vec3(0, 1, 0))
                           * glm::scale(Mat4(1.0f), Vec3(2.0f));
            Renderer3D::Submit(m_CustomMesh, m_BlueMaterial, transform);

            // Orbiting spheres
            for (int i = 0; i < 4; ++i) {
                const float angle = m_Time + (i * glm::two_pi<float>() / 4.0f);
                const float radius = 5.0f;
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

        void RenderLightingDemoScene() const {
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
            constexpr int objectCount = 8;
            constexpr float ringRadius = 6.0f;

            for (int i = 0; i < objectCount; ++i) {
                const float angle = (i * glm::two_pi<float>()) / objectCount;
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

        void Render2DOverlay() const {
            Renderer2D::BeginScene(*m_Camera);

            // Draw FPS counter
            const float padding = 10.0f;
            const float boxWidth = 200.0f;
            const float boxHeight = 100.0f;

            // Semi-transparent background
            const Vec2 screenSize = Application::Get().GetWindow().GetSizeF();
            Renderer2D::DrawQuad(
                Vec2(padding + boxWidth * 0.5f, screenSize.y - padding - boxHeight * 0.5f),
                Vec2(boxWidth, boxHeight),
                Vec4(0.0f, 0.0f, 0.0f, 0.5f)
            );

            // Info text would go here (needs text rendering)
            // For now, just draw colored indicators
            const float indicatorSize = 10.0f;
            const float x = padding + 10.0f;
            const float y = screenSize.y - padding - 20.0f;

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
        Ref<ToonMaterial> m_ToonRedMaterial;
        Ref<ToonMaterial> m_ToonBlueMaterial;
        Ref<ToonMaterial> m_ToonGreenMaterial;

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
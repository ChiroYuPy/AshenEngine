#include "layers/GameLayer.h"

#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Resources/ResourceManager.h"
#include "Ashen/Audio/AudioManager.h"
#include "Ashen/Core/Application.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Events/EventDispatcher.h"
#include "Ashen/Graphics/CameraControllers/FPSCameraController.h"
#include "Ashen/Graphics/CameraControllers/OrbitCameraController.h"

namespace ash {
    void GameLayer::OnAttach() {
        SetupMeshes();
        SetupCamera();
        SetupMaterials();
        SetupLights();
    }

    void GameLayer::OnUpdate(const float deltaTime) {
        m_CameraController->Update(deltaTime);
        m_Time += deltaTime;

        AudioManager::Get().SetListenerPosition(m_Camera->GetPosition());
        AudioManager::Get().SetListenerOrientation(m_Camera->GetFront(), m_Camera->GetUp());
    }

    void GameLayer::OnRender() {
        Renderer3D::BeginScene(*m_Camera);
        Renderer3D::SetDirectionalLight(m_DirectionalLight);
        Renderer3D::ClearLights();

        for (const auto &light: m_PointLights)
            Renderer3D::AddPointLight(light);

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

    void GameLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent &e) {
            if (e.GetKeyCode() == Key::Escape) {
                if (m_CameraController->IsEnabled()) {
                    m_CameraController->SetEnabled(false);
                    Input::SetCursorMode(CursorMode::Normal);
                } else Application::Get().Stop();
                return true;
            }
            if (e.GetKeyCode() == Key::P) {
                AudioManager::Get().PlaySound("resources/sounds/sound.mp3", 1.f, AudioCategory::Ambient);
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &e) {
            if (e.GetMouseButton() == MouseButton::Left) {
                if (!m_CameraController->IsEnabled()) {
                    m_CameraController->SetEnabled(true);
                    Input::SetCursorMode(CursorMode::Captured);
                }
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent &e) {
            m_Camera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });

        m_CameraController->OnEvent(event);
    }

    void GameLayer::RenderSpatialZone() const {
        constexpr float zoneX = -20.0f;

        Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                         * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
        Renderer3D::Submit(m_CustomMesh, m_SpatialMaterial, transform);

        constexpr float cubeOffset = 8.0f;

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_MetallicMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_RoughMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_RedMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_BlueMaterial, transform);

        constexpr int nbSphere = 32;
        for (int i = 0; i < nbSphere; ++i) {
            const float angle = m_Time * 0.5f + i * glm::two_pi<float>() / static_cast<float>(nbSphere);
            constexpr float radius = 6.0f;
            Vec3 pos(
                zoneX + std::cos(angle) * radius,
                std::sin(m_Time * 2.0f + i) * 0.8f + 1.5f,
                std::sin(angle) * radius
            );

            Mat4 modelTransform = glm::translate(Mat4(1.0f), pos)
                                  * glm::scale(Mat4(1.0f), Vec3(0.6f));

            static const Ref<Material> materials[] = {
                m_SpatialMaterial,
                m_MetallicMaterial,
                m_RoughMaterial
            };

            Ref<Material> mat = materials[i % 3];
            Renderer3D::Submit(m_SphereMesh, mat, modelTransform);
        }
    }

    void GameLayer::RenderToonZone() const {
        constexpr float zoneX = 0.0f;

        Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                         * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
        Renderer3D::Submit(m_CustomMesh, m_ToonMaterial, transform);

        constexpr float cubeOffset = 8.0f;

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_ToonMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_ToonMaterial2, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_ToonMaterial3, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_ToonRedMaterial, transform);

        for (int i = 0; i < 3; ++i) {
            const float angle = -m_Time * 0.7f + i * glm::two_pi<float>() / 3.0f;
            constexpr float radius = 6.0f;
            Vec3 pos(
                zoneX + std::cos(angle) * radius,
                std::abs(std::sin(m_Time * 1.5f + i)) * 1.2f + 1.0f,
                std::sin(angle) * radius
            );

            transform = glm::translate(Mat4(1.0f), pos)
                        * glm::scale(Mat4(1.0f), Vec3(0.6f));

            Ref<Material> mat = i % 2 == 0 ? m_ToonMaterial : m_ToonRedMaterial;
            Renderer3D::Submit(m_SphereMesh, mat, transform);
        }
    }

    void GameLayer::RenderUnlitZone() const {
        constexpr float zoneX = 20.0f;

        Mat4 transform = glm::translate(Mat4(1.0f), Vec3(zoneX, 0, 0))
                         * glm::rotate(Mat4(1.0f), glm::radians(-45.0f), Vec3(0, 1, 0));
        Renderer3D::Submit(m_CustomMesh, m_UnlitMaterial, transform);

        constexpr float cubeOffset = 8.0f;

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_UnlitYellowMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_UnlitCyanMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX - cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_UnlitMagentaMaterial, transform);

        transform = glm::translate(Mat4(1.0f), Vec3(zoneX + cubeOffset, 0, -cubeOffset));
        Renderer3D::Submit(m_CubeMesh, m_UnlitOrangeMaterial, transform);

        for (int i = 0; i < 3; ++i) {
            const float heightOffset = std::sin(m_Time * 2.0f + i * 1.0f) * 2.0f;
            Vec3 pos(
                zoneX + (i - 1) * 3.0f,
                heightOffset + 2.0f,
                -3.0f
            );

            transform = glm::translate(Mat4(1.0f), pos)
                        * glm::rotate(Mat4(1.0f), m_Time + i, Vec3(1, 1, 0))
                        * glm::scale(Mat4(1.0f), Vec3(0.6f));

            Ref<Material> mat = i == 0 ? m_UnlitMaterial : i == 1 ? m_UnlitYellowMaterial : m_UnlitCyanMaterial;
            Renderer3D::Submit(m_SphereMesh, mat, transform);
        }
    }

    void GameLayer::SetupMeshes() {
        try {
            m_CustomMesh = AssetLibrary::Meshes().Load("plant/plant");
        } catch (const std::exception &e) {
            Logger::Warn() << "Could not load custom mesh: " << e.what();
            m_CustomMesh = AssetLibrary::Meshes().GetCube();
        }

        m_CubeMesh = AssetLibrary::Meshes().GetCube();
        m_SphereMesh = AssetLibrary::Meshes().GetSphere();
        m_PlaneMesh = AssetLibrary::Meshes().GetPlane();
    }

    void GameLayer::SetupCamera() {
        const auto perspectiveCamera = MakeRef<PerspectiveCamera>(60.f, 1.f, 0.1f, 1000.f);
        m_Camera = perspectiveCamera;

        m_CameraController = FPSCameraController::Create(*perspectiveCamera, 0.1f, 8.f); // need perspectiveCamera specifically
        m_CameraController = OrbitCameraController::Create(*m_Camera);

        m_Camera->SetPosition({0, 15, 25});
        m_Camera->LookAt({0, 0, 0});
    }

    void GameLayer::SetupMaterials() {
        m_SpatialMaterial = AssetLibrary::Materials().CreateSpatial(
            "spatial_main",
            Vec4(0.7f, 0.5f, 0.8f, 1.0f),
            0.2f, 0.5f, 0.6f
        );

        m_MetallicMaterial = AssetLibrary::Materials().CreateSpatial(
            "spatial_metallic",
            Vec4(0.8f, 0.8f, 0.8f, 1.0f),
            0.9f, 0.2f, 0.8f
        );

        m_RoughMaterial = AssetLibrary::Materials().CreateSpatial(
            "spatial_rough",
            Vec4(0.6f, 0.5f, 0.4f, 1.0f),
            0.0f, 0.9f, 0.3f
        );

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

        m_ToonMaterial = AssetLibrary::Materials().CreateToon(
            "toon_green",
            Vec4(0.3f, 0.9f, 0.4f, 1.0f),
            3, 0.7f
        );

        m_ToonMaterial2 = AssetLibrary::Materials().CreateToon(
            "toon_blue",
            Vec4(0.3f, 0.5f, 0.9f, 1.0f),
            4, 0.8f
        );

        m_ToonMaterial3 = AssetLibrary::Materials().CreateToon(
            "toon_cyan",
            Vec4(0.2f, 0.8f, 0.9f, 1.0f),
            5, 0.6f
        );

        m_ToonRedMaterial = AssetLibrary::Materials().CreateToon(
            "toon_red",
            Vec4(0.9f, 0.3f, 0.3f, 1.0f),
            3, 0.7f
        );

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

        m_GroundMaterial = AssetLibrary::Materials().CreateSpatial(
            "ground",
            Vec4(0.25f, 0.25f, 0.25f, 1.0f),
            0.0f, 0.9f, 0.2f
        );
    }

    void GameLayer::SetupLights() {
        m_DirectionalLight.direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
        m_DirectionalLight.color = Vec3(1.0f, 0.95f, 0.9f);
        m_DirectionalLight.intensity = 1.0f;

        m_PointLights = {
            PointLight{Vec3(-20.0f, 4.0f, 0.0f), Vec3(0.3f, 0.5f, 1.0f), 60.0f},
            PointLight{Vec3(0.0f, 4.0f, 0.0f), Vec3(0.3f, 1.0f, 0.5f), 60.0f},
            PointLight{Vec3(20.0f, 4.0f, 0.0f), Vec3(1.0f, 0.6f, 0.2f), 60.0f}
        };
    }
}

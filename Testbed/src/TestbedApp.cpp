#include "TestbedApp.h"

#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Camera/CameraController.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Resources/ResourceManager.h"

namespace ash {
    class SandboxLayer final : public Layer {
    public:
        static constexpr int MAX_LIGHTS = 4;

        SandboxLayer() {
            m_Mesh = AssetLibrary::Meshes().Load("sofa/sofa");

            m_Camera = MakeRef<PerspectiveCamera>();
            m_Camera->SetPosition({10, 10, 10});
            m_Camera->LookAt({0, 0, 0});
            m_CameraController = MakeRef<CameraController>(m_Camera, 5.0f, 0.1f);

            m_Material = AssetLibrary::Materials().CreatePBR(
                "red_material",
                Vec3(0.8f, 0.4f, 0.3f), // albedo
                0.0f, // metallic
                0.5f  // roughness
            );

            // Initialisation des lumières
            m_LightPositions = {
                Vec3(10.0f, 10.0f, 10.0f),
                Vec3(-10.0f, 10.0f, 10.0f),
                Vec3(0.0f, 10.0f, -10.0f),
                Vec3(0.0f, -10.0f, 10.0f)
            };

            m_LightColors = {
                Vec3(300.0f, 100.0f, 100.0f),
                Vec3(100.0f, 300.0f, 100.0f),
                Vec3(100.0f, 100.0f, 300.0f),
                Vec3(200.0f, 200.0f, 200.0f)
            };
        }

        void OnUpdate(const float deltaTime) override {
            m_CameraController->OnUpdate(deltaTime);
        }

        void OnRender() override {
            const auto shader = m_Material->GetShader();
            shader->Bind();

            const Mat4 model = glm::mat4(1.0f);
            const Mat4 view = m_Camera->GetViewMatrix();
            const Mat4 proj = m_Camera->GetProjectionMatrix();

            shader->SetMat4("u_Model", model);
            shader->SetMat4("u_View", view);
            shader->SetMat4("u_Proj", proj);

            const Mat3 normalMatrix = glm::transpose(glm::inverse(Mat3(model)));
            shader->SetMat3("u_NormalMatrix", normalMatrix);
            shader->SetVec3("u_ViewPos", m_Camera->GetPosition());

            // Envoi de toutes les lumières
            for (int i = 0; i < MAX_LIGHTS; ++i) {
                shader->SetVec3("u_LightPositions[" + std::to_string(i) + "]", m_LightPositions[i]);
                shader->SetVec3("u_LightColors[" + std::to_string(i) + "]", m_LightColors[i]);
            }

            m_Material->Bind();
            m_Mesh->Draw();
        }

        void OnEvent(Event &event) override {
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
        Ref<CameraController> m_CameraController;
        Ref<PerspectiveCamera> m_Camera;
        Ref<Mesh> m_Mesh;
        Ref<PBRMaterial> m_Material;

        std::array<Vec3, MAX_LIGHTS> m_LightPositions;
        std::array<Vec3, MAX_LIGHTS> m_LightColors;
    };

    TestbedApp::TestbedApp(const ApplicationSettings &settings)
        : Application(settings) {
        PushLayer<SandboxLayer>();
    }
}

ash::Application* ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Sandbox";
    settings.Version = "0.0.0";
    settings.CommandLineArgs = args;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"

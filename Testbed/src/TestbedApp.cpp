#include "TestbedApp.h"

#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Camera/CameraController.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Resources/ResourceManager.h"

namespace ash {
    class SandboxLayer final : public Layer {
    public:
        SandboxLayer() {
            m_camera = MakeRef<UICamera>();
            m_cubeMesh = AssetLibrary::Meshes().Load("sofa/sofa");

            m_pbrShader = AssetLibrary::Shaders().Get("pbr");
            m_plasticMaterial = AssetLibrary::Materials().CreatePBR("plastic_mat", "pbr");
            m_plasticMaterial->SetAlbedo(Vec3(0.2f, 0.5f, 0.8f));
            m_plasticMaterial->SetMetallic(1.f);
            m_plasticMaterial->SetRoughness(1.f);

            m_perspectiveCamera = MakeRef<PerspectiveCamera>();
            m_perspectiveCamera->SetPosition({10, 10, 10});
            m_perspectiveCamera->LookAt({0, 0, 0});

            // Créer le contrôleur de caméra
            m_cameraController = MakeRef<CameraController>(m_perspectiveCamera, 5.0f, 0.1f);

            // Initialiser le temps
            m_lastFrameTime = 0.0f;
        }

        void OnUpdate(const float deltaTime) override {
            m_cameraController->OnUpdate(deltaTime);
        }

        static void RenderObject(const Mesh& mesh, const Material& material,
                         const Mat4& model, const Mat4& view, const Mat4& projection) {
            const auto& shader = material.GetShader();
            if (!shader) return;

            shader->Bind();
            shader->SetMat4("u_Model", model);
            shader->SetMat4("u_View", view);
            shader->SetMat4("u_Proj", projection);

            // Normal matrix for lighting
            const Mat3 normalMatrix = glm::transpose(glm::inverse(Mat3(model)));
            shader->SetMat3("normalMatrix", normalMatrix);

            material.Bind();
            mesh.Draw();
            material.Unbind();
        }

        void OnRender() override {
            Renderer2D::BeginScene(*m_camera);
            Renderer2D::DrawQuad({50, 50}, {100, 100}, {1, 0, 0, 1});
            Renderer2D::EndScene();

            const auto view = m_perspectiveCamera->GetViewMatrix();
            const auto projection = m_perspectiveCamera->GetProjectionMatrix();

            RenderObject(*m_cubeMesh, *m_plasticMaterial,
                         glm::translate(Mat4(1.0f), Vec3(0.0f, 0.0f, 0.0f)),
                         view, projection);
        }

        void OnEvent(Event &event) override {
            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent &e) {
                m_camera->OnResize(e.GetWidth(), e.GetHeight());
                m_perspectiveCamera->OnResize(e.GetWidth(), e.GetHeight());
                return false;
            });

            dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &e) {
                m_cameraController->OnMouseMove(e.GetX(), e.GetY());
                return false;
            });

            dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent &e) {
                m_cameraController->OnMouseScroll(e.GetYOffset());
                return false;
            });
        }

        void OnDetach() override {
            m_cameraController.reset();
            m_perspectiveCamera.reset();
            m_plasticMaterial.reset();
            m_pbrShader.reset();
            m_cubeMesh.reset();
            m_camera.reset();
        }

    private:
        Ref<UICamera> m_camera;
        Ref<Mesh> m_cubeMesh;
        Ref<ShaderProgram> m_pbrShader;
        Ref<PBRMaterial> m_plasticMaterial;
        Ref<PerspectiveCamera> m_perspectiveCamera;
        Ref<CameraController> m_cameraController;
        float m_lastFrameTime;
    };

    TestbedApp::TestbedApp(const ApplicationSettings &settings) : Application(settings) {
        PushLayer<SandboxLayer>();
    }
}

ash::Application *ash::CreateApplication(const ApplicationCommandLineArgs args) {
    ApplicationSettings settings;
    settings.Name = "Sandbox";
    settings.Version = "0.0.0";
    settings.CommandLineArgs = args;

    return new TestbedApp(settings);
}

#include "Ashen/Core/EntryPoint.h"

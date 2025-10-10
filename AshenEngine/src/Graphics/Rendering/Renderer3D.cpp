#include "Ashen/Graphics/Rendering/Renderer3D.h"

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Rendering/DebugRenderer.h"
#include "Ashen/GraphicsAPI/RenderCommand.h"

namespace ash {

    struct Renderer3D::SceneData {
        // Camera
        const Camera* camera = nullptr;
        Mat4 viewMatrix{};
        Mat4 projectionMatrix{};
        Mat4 viewProjectionMatrix{};
        Vec3 cameraPosition{};

        // Render queue
        std::vector<RenderObject> renderQueue;

        // Lighting
        DirectionalLight directionalLight;
        std::vector<PointLight> pointLights;
        bool hasDirectionalLight = false;

        // Environment
        SceneEnvironment environment;

        // Stats
        RenderStats stats;

        void Reset() {
            camera = nullptr;
            renderQueue.clear();
            pointLights.clear();
            hasDirectionalLight = false;
            stats.Reset();
        }
    };

    std::unique_ptr<Renderer3D::SceneData> Renderer3D::s_Data = nullptr;

    void Renderer3D::Init() {
        s_Data = std::make_unique<SceneData>();
        DebugRenderer::Init();
        Logger::info("Renderer3D initialized");
    }

    void Renderer3D::Shutdown() {
        DebugRenderer::Shutdown();
        if (s_Data)
            s_Data.reset();

        Logger::info("Renderer3D shutdown");
    }

    void Renderer3D::BeginScene(const Camera& camera) {
        s_Data->Reset();

        s_Data->camera = &camera;
        s_Data->viewMatrix = camera.GetViewMatrix();
        s_Data->projectionMatrix = camera.GetProjectionMatrix();
        s_Data->viewProjectionMatrix = s_Data->projectionMatrix * s_Data->viewMatrix;
        s_Data->cameraPosition = camera.GetPosition();

        // Setup render state
        RenderCommand::EnableDepthTest(true);
        RenderCommand::SetDepthFunc(DepthFunc::Less);
        RenderCommand::EnableCulling(true);
        RenderCommand::SetCullFace(CullFaceMode::Back);
        RenderCommand::SetFrontFace(FrontFace::CounterClockwise);
    }

    void Renderer3D::EndScene() {
        FlushRenderQueue();
    }

    void Renderer3D::Submit(const RenderObject& object) {
        s_Data->renderQueue.push_back(object);
    }

    void Renderer3D::Submit(
        const std::shared_ptr<Mesh>& mesh,
        const std::shared_ptr<Material>& material,
        const Mat4& transform
    ) {
        RenderObject obj;
        obj.mesh = mesh;
        obj.material = material;
        obj.transform = transform;
        Submit(obj);
    }

    void Renderer3D::DrawImmediate(
        const std::shared_ptr<Mesh>& mesh,
        const std::shared_ptr<Material>& material,
        const Mat4& transform
    ) {
        if (!mesh || !material) return;

        const auto shader = material->GetShader();
        if (!shader) return;

        shader->Bind();

        // Setup matrices
        shader->SetMat4("u_Model", transform);
        shader->SetMat4("u_View", s_Data->viewMatrix);
        shader->SetMat4("u_Proj", s_Data->projectionMatrix);

        const Mat3 normalMatrix = glm::transpose(glm::inverse(Mat3(transform)));
        shader->SetMat3("u_NormalMatrix", normalMatrix);
        shader->SetVec3("u_ViewPos", s_Data->cameraPosition);

        // Setup lighting
        SetupLighting(shader);

        // Apply material
        material->Bind();

        // Draw
        mesh->Draw();

        s_Data->stats.drawCalls++;
        s_Data->stats.triangles += mesh->GetIndexCount() / 3;
        s_Data->stats.vertices += mesh->GetVertexCount();
    }

    void Renderer3D::SetDirectionalLight(const DirectionalLight& light) {
        s_Data->directionalLight = light;
        s_Data->hasDirectionalLight = true;
    }

    void Renderer3D::AddPointLight(const PointLight& light) {
        constexpr size_t MAX_POINT_LIGHTS = 16;
        if (s_Data->pointLights.size() < MAX_POINT_LIGHTS)
            s_Data->pointLights.push_back(light);
        else
            Logger::warn("Maximum number of point lights reached (16)");
    }

    void Renderer3D::ClearLights() {
        s_Data->hasDirectionalLight = false;
        s_Data->pointLights.clear();
    }

    void Renderer3D::SetEnvironment(const SceneEnvironment& env) {
        s_Data->environment = env;
    }

    void Renderer3D::SetSkybox(const std::shared_ptr<TextureCubeMap>& skybox) {
        s_Data->environment.skybox = skybox;
    }

    void Renderer3D::SetAmbientLight(const Vec3& color) {
        s_Data->environment.ambientColor = color;
    }

    const RenderStats& Renderer3D::GetStats() {
        return s_Data->stats;
    }

    void Renderer3D::ResetStats() {
        s_Data->stats.Reset();
    }

    void Renderer3D::SetWireframeMode(const bool enabled) {
        RenderCommand::SetPolygonMode(CullFaceMode::FrontAndBack, enabled ? PolygonMode::Line : PolygonMode::Fill);
    }

    void Renderer3D::FlushRenderQueue() {
        for (const auto&[mesh, material, transform] : s_Data->renderQueue)
            DrawImmediate(mesh, material, transform);

        s_Data->renderQueue.clear();
    }

    void Renderer3D::SetupLighting(const std::shared_ptr<ShaderProgram>& shader) {
        // Directional light
        if (s_Data->hasDirectionalLight) {
            shader->SetVec3("u_DirectionalLight.direction", s_Data->directionalLight.direction);
            shader->SetVec3("u_DirectionalLight.color", s_Data->directionalLight.color);
            shader->SetFloat("u_DirectionalLight.intensity", s_Data->directionalLight.intensity);
            shader->SetBool("u_HasDirectionalLight", true);
        } else {
            shader->SetBool("u_HasDirectionalLight", false);
        }

        // Point lights (max 16)
        const int pointLightCount = static_cast<int>(s_Data->pointLights.size());
        shader->SetInt("u_PointLightCount", pointLightCount);

        for (int i = 0; i < pointLightCount && i < 16; ++i) {
            const auto&[position, color, intensity] = s_Data->pointLights[i];
            shader->SetVec3("u_LightPositions[" + std::to_string(i) + "]", position);
            shader->SetVec3("u_LightColors[" + std::to_string(i) + "]",
                          color * intensity);
        }

        // Ambient light
        shader->SetVec3("u_AmbientLight", s_Data->environment.ambientColor);
    }
}
#include "Ashen/Graphics/Rendering/Renderer3D.h"

#include "Ashen/Core/Logger.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/GraphicsAPI/RenderCommand.h"

namespace ash {
    struct Renderer3D::SceneData {
        // Camera
        const Camera *camera = nullptr;
        Mat4 viewMatrix{};
        Mat4 projectionMatrix{};
        Mat4 viewProjectionMatrix{};
        Vec3 cameraPosition{};

        // Render queue
        Vector<RenderObject> renderQueue;

        // Lighting
        DirectionalLight directionalLight;
        Vector<PointLight> pointLights;
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
        Logger::Info("Renderer3D initialized");
    }

    void Renderer3D::Shutdown() {
        if (s_Data)
            s_Data.reset();

        Logger::Info("Renderer3D shutdown");
    }

    void Renderer3D::BeginScene(const Camera &camera) {
        s_Data->Reset();

        s_Data->camera = &camera;
        s_Data->viewMatrix = camera.GetViewMatrix();
        s_Data->projectionMatrix = camera.GetProjectionMatrix();
        s_Data->viewProjectionMatrix = s_Data->projectionMatrix * s_Data->viewMatrix;
        s_Data->cameraPosition = camera.GetPosition();

        // Setup render state
        RenderCommand::EnableDepthTest();
        RenderCommand::SetDepthFunc(DepthFunc::Less);
        RenderCommand::EnableCulling();
        RenderCommand::SetCullFace(CullFaceMode::Back);
        RenderCommand::SetFrontFace(FrontFace::CounterClockwise);
    }

    void Renderer3D::EndScene() {
        FlushRenderQueue();
    }

    void Renderer3D::Submit(const RenderObject &object) {
        s_Data->renderQueue.push_back(object);
    }

    void Renderer3D::Submit(
        const std::shared_ptr<Mesh> &mesh,
        const std::shared_ptr<Material> &material,
        const Mat4 &transform
    ) {
        RenderObject obj;
        obj.mesh = mesh;
        obj.material = material;
        obj.transform = transform;
        Submit(obj);
    }

    void Renderer3D::DrawImmediate(
        const std::shared_ptr<Mesh> &mesh,
        const std::shared_ptr<Material> &material,
        const Mat4 &transform
    ) {
        if (!mesh || !material) return;

        const auto shader = material->GetShader();
        if (!shader) return;

        shader->Bind();

        // Setup transformation matrices (tous les shaders en ont besoin)
        shader->SetMat4("u_Model", transform);
        shader->SetMat4("u_View", s_Data->viewMatrix);
        shader->SetMat4("u_Proj", s_Data->projectionMatrix);

        // CORRECTION: Vérifier si le shader a besoin de lighting
        // On vérifie la présence d'un uniform clé au lieu de tous les tester
        const bool needsLighting = shader->HasUniform("u_CameraPos");

        if (needsLighting)
            SetupLighting(shader);

        // Apply material properties
        material->Apply();

        // Draw mesh
        MeshRenderer::Draw(*mesh);

        // Update stats
        s_Data->stats.drawCalls++;
        s_Data->stats.triangles += mesh->GetIndexCount() / 3;
        s_Data->stats.vertices += mesh->GetVertexCount();

        shader->Unbind();
    }

    void Renderer3D::SetDirectionalLight(const DirectionalLight &light) {
        s_Data->directionalLight = light;
        s_Data->hasDirectionalLight = true;
    }

    void Renderer3D::AddPointLight(const PointLight &light) {
        constexpr size_t MAX_POINT_LIGHTS = 4;
        if (s_Data->pointLights.size() < MAX_POINT_LIGHTS)
            s_Data->pointLights.push_back(light);
        else
            Logger::Warn("Maximum number of point lights reached (4)");
    }

    void Renderer3D::ClearLights() {
        s_Data->hasDirectionalLight = false;
        s_Data->pointLights.clear();
    }

    void Renderer3D::SetEnvironment(const SceneEnvironment &env) {
        s_Data->environment = env;
    }

    void Renderer3D::SetSkybox(const std::shared_ptr<TextureCubeMap> &skybox) {
        s_Data->environment.skybox = skybox;
    }

    void Renderer3D::SetAmbientLight(const Vec3 &color) {
        s_Data->environment.ambientColor = color;
    }

    const RenderStats &Renderer3D::GetStats() {
        return s_Data->stats;
    }

    void Renderer3D::ResetStats() {
        s_Data->stats.Reset();
    }

    void Renderer3D::SetWireframeMode(const bool enabled) {
        RenderCommand::SetPolygonMode(CullFaceMode::FrontAndBack, enabled ? PolygonMode::Line : PolygonMode::Fill);
    }

    void Renderer3D::FlushRenderQueue() {
        for (const auto &[mesh, material, transform]: s_Data->renderQueue)
            DrawImmediate(mesh, material, transform);

        s_Data->renderQueue.clear();
    }

    void Renderer3D::SetupLighting(const std::shared_ptr<ShaderProgram> &shader) {
        // CORRECTION: Ne set les uniforms QUE s'ils existent dans le shader

        // Camera position (utilisé par PBR et Toon)
        if (shader->HasUniform("u_CameraPos"))
            shader->SetVec3("u_CameraPos", s_Data->cameraPosition);

        // Ambient light
        if (shader->HasUniform("u_AmbientLight"))
            shader->SetVec3("u_AmbientLight", s_Data->environment.ambientColor);

        // Directional light
        if (shader->HasUniform("u_LightDirection")) {
            if (s_Data->hasDirectionalLight) {
                shader->SetVec3("u_LightDirection", s_Data->directionalLight.direction);
                shader->SetVec3("u_LightColor", s_Data->directionalLight.color);
                shader->SetFloat("u_LightEnergy", s_Data->directionalLight.intensity);
            } else {
                // Default sun-like light
                shader->SetVec3("u_LightDirection", Vec3(-0.5f, -1.0f, -0.3f));
                shader->SetVec3("u_LightColor", Vec3(1.0f));
                shader->SetFloat("u_LightEnergy", 1.0f);
            }
        }

        // Point lights (seulement pour les shaders PBR qui les supportent)
        if (shader->HasUniform("u_PointLightCount")) {
            const int pointLightCount = std::min(static_cast<int>(s_Data->pointLights.size()), 4);
            shader->SetInt("u_PointLightCount", pointLightCount);

            for (int i = 0; i < pointLightCount; ++i) {
                const auto &light = s_Data->pointLights[i];

                // Construire les noms d'uniforms une seule fois
                const std::string posUniform = "u_PointLightPositions[" + std::to_string(i) + "]";
                const std::string colorUniform = "u_PointLightColors[" + std::to_string(i) + "]";
                const std::string energyUniform = "u_PointLightEnergies[" + std::to_string(i) + "]";
                const std::string rangeUniform = "u_PointLightRanges[" + std::to_string(i) + "]";

                // Set seulement si l'uniform existe
                if (shader->HasUniform(posUniform)) {
                    shader->SetVec3(posUniform, light.position);
                    shader->SetVec3(colorUniform, light.color);
                    shader->SetFloat(energyUniform, light.intensity);

                    // Calculate range from intensity
                    const float range = std::sqrt(light.intensity) * 10.0f;
                    shader->SetFloat(rangeUniform, range);
                }
            }
        }
    }
}
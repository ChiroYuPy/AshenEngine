#include "Voxelity/layers/VoxelWorldLayer.h"

#include <format>
#include "Voxelity/entities/Player.h"
#include "Ashen/core/Input.h"
#include "Ashen/core/Logger.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer.h"
#include "Ashen/renderer/Renderer2D.h"
#include "Ashen/resources/ResourceManager.h"

namespace voxelity {
    VoxelWorldLayer::VoxelWorldLayer() {
        setupCamera();
        setupShader();
        setupWorld();
        setupPlayer();
        setupWorldInteractor();
        setupSkybox();
        setupInputHandler();
    }

    VoxelWorldLayer::~VoxelWorldLayer() {
        m_shader.reset();
        m_camera.reset();
        m_world.reset();
    }

    void VoxelWorldLayer::OnEvent(ash::Event &event) {
        if (m_inputHandler) {
            m_inputHandler->handleEvent(event);
        }
    }

    void VoxelWorldLayer::OnUpdate(const float ts) {
        if (!m_player || !m_world || !m_worldRenderer) return;

        m_player->update(ts, *m_world);

        /*
        m_fpsTimer += ts;
        m_frameCount++;

        if (m_fpsTimer >= 1.0f) {
            pixl::Logger::info() << std::format("FPS: {}", m_frameCount);
            m_fpsTimer = 0.0f;
            m_frameCount = 0;
        }
        */

        // Mise à jour du bloc ciblé
        const glm::vec3 cameraPos = m_camera->GetPosition();
        const glm::vec3 cameraDir = m_camera->GetFront();

        const auto targetedBlock = m_worldInteractor->getTargetedBlock(cameraPos, cameraDir);
        if (targetedBlock) {
            m_targetedBlockPos = targetedBlock->blockPos;
            m_hasTargetedBlock = true;
        } else {
            m_hasTargetedBlock = false;
        }
    }

    void VoxelWorldLayer::OnRender() {
        if (!m_camera || !m_worldRenderer) return;

        // Configuration OpenGL spécifique à cette scène 3D
        ash::RenderCommand::EnableDepthTest(true);
        ash::RenderCommand::SetDepthFunc(ash::RenderCommand::DepthFunc::Less);

        // 1. Skybox (rendu en premier avec depth trick)
        renderSkybox();

        // 2. Monde 3D
        renderWorld();
    }

    void VoxelWorldLayer::setupCamera() {
        m_camera = ash::MakeRef<ash::PerspectiveCamera>();
        m_camera->SetPosition(World::toWorldPos({-RENDER_DISTANCE - 1, RENDER_HEIGHT, -RENDER_DISTANCE - 1}));
        m_camera->SetFov(70.f);
        m_camera->SetRotation(45.f, 0.f);

        // Caméra orthographique pour l'UI 2D
        m_orthoCam = ash::MakeRef<ash::OrthographicCamera>(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
    }

    void VoxelWorldLayer::setupShader() {
        m_shader = ash::AssetLibrary::Shaders().Get("shaders/chunk");
    }

    void VoxelWorldLayer::setupWorld() {
        m_world = ash::MakeScope<World>();
        m_worldRenderer = ash::MakeScope<WorldRenderer>(*m_world, *m_camera, *m_shader);

        m_world->generateArea(
            {-RENDER_DISTANCE, -RENDER_HEIGHT, -RENDER_DISTANCE},
            {RENDER_DISTANCE, RENDER_HEIGHT, RENDER_DISTANCE}
        );
        m_worldRenderer->buildAll();
    }

    void VoxelWorldLayer::setupPlayer() {
        m_player = ash::MakeScope<Player>(m_camera);
        m_player->position = World::toWorldPos({0, 1, 0}, {0, 0, 0});
        m_player->velocity = {0.0f, 0.0f, 0.0f};
    }

    void VoxelWorldLayer::setupWorldInteractor() {
        m_worldInteractor = ash::MakeScope<WorldInteractor>(*m_world, *m_worldRenderer);
        m_worldInteractor->setMaxReach(64.0f);
        m_worldInteractor->setSelectedVoxelID(1);
    }

    void VoxelWorldLayer::setupSkybox() {
        m_skyboxShader = ash::AssetLibrary::Shaders().Get("shaders/mountain_skybox");

        std::array<float, 36 * 3> skyboxVertices = {
            -1, 1, -1, -1, -1, -1, 1, -1, -1,
            1, -1, -1, 1, 1, -1, -1, 1, -1,
            -1, -1, 1, -1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, -1, 1, -1, -1, 1,
            -1, 1, -1, -1, 1, 1, -1, -1, 1,
            -1, -1, 1, -1, -1, -1, -1, 1, -1,
            1, 1, 1, 1, 1, -1, 1, -1, -1,
            1, -1, -1, 1, -1, 1, 1, 1, 1,
            -1, 1, 1, -1, 1, -1, 1, 1, -1,
            1, 1, -1, 1, 1, 1, -1, 1, 1,
            -1, -1, -1, -1, -1, 1, 1, -1, 1,
            1, -1, 1, 1, -1, -1, -1, -1, -1
        };

        m_skyboxVAO = ash::MakeRef<ash::VertexArray>();
        m_skyboxVBO = ash::MakeRef<ash::VertexBuffer>();
        m_skyboxVBO->SetData<float>(skyboxVertices, ash::BufferUsage::Static);

        ash::VertexBufferLayout layout(sizeof(glm::vec3));
        layout.AddAttribute<glm::vec3>(0, 0);
        m_skyboxVAO->AddVertexBuffer(m_skyboxVBO, layout);

        const std::array<std::string, 6> faces = {
            "resources/textures/mountain_skybox/right.jpg",
            "resources/textures/mountain_skybox/left.jpg",
            "resources/textures/mountain_skybox/top.jpg",
            "resources/textures/mountain_skybox/bottom.jpg",
            "resources/textures/mountain_skybox/front.jpg",
            "resources/textures/mountain_skybox/back.jpg"
        };
        m_skyboxTexture = ash::MakeRef<ash::TextureCubeMap>(ash::LoadCubeMap(faces));

        m_skyboxShader->Bind();
        m_skyboxShader->SetInt("skybox", 0);
    }

    void VoxelWorldLayer::setupInputHandler() {
        m_inputHandler = ash::MakeScope<InputHandler>(
            *this,
            m_player->getController(),
            *m_worldInteractor
        );
    }

    void VoxelWorldLayer::renderSkybox() const {
        // Trick pour dessiner la skybox en dernier mais qu'elle apparaisse derrière tout
        ash::RenderCommand::SetDepthWrite(false);
        ash::RenderCommand::SetDepthFunc(ash::RenderCommand::DepthFunc::LessEqual);

        m_skyboxShader->Bind();
        const auto view = glm::mat4(glm::mat3(m_camera->GetViewMatrix()));
        const glm::mat4 projection = m_camera->GetProjectionMatrix();
        m_skyboxShader->SetMat4("uView", view);
        m_skyboxShader->SetMat4("uProjection", projection);

        glActiveTexture(GL_TEXTURE0);
        m_skyboxTexture->Bind();

        ash::Renderer::DrawArrays(*m_skyboxVAO, 36);

        // Restaurer état normal
        ash::RenderCommand::SetDepthWrite(true);
        ash::RenderCommand::SetDepthFunc(ash::RenderCommand::DepthFunc::Less);
    }

    void VoxelWorldLayer::renderWorld() const {
        m_worldRenderer->renderAll();
    }
}

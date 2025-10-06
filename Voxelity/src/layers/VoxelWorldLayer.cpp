#include "Voxelity/layers/VoxelWorldLayer.h"
#include "Voxelity/entities/Player.h"
#include "Voxelity/voxelWorld/generation/NaturalTerrainGenerator.h"
#include "Ashen/core/Input.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer.h"
#include "Ashen/resources/ResourceManager.h"

namespace voxelity {
    VoxelWorldLayer::VoxelWorldLayer() {
        setupCamera();
        setupShader();
        setupWorld();
        setupEntityManager();
        setupPlayer();
        setupWorldInteractor();
        setupSkybox();
        setupInputHandler();

        m_world->updateLoadedChunks(m_player->position, m_config.renderDistance);
    }

    VoxelWorldLayer::~VoxelWorldLayer() {
        m_shader.reset();
        m_camera.reset();
        m_world.reset();
    }

    void VoxelWorldLayer::OnEvent(ash::Event& event) {
        if (m_inputHandler)
            m_inputHandler->handleEvent(event);

        ash::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<ash::WindowResizeEvent>([this](const ash::WindowResizeEvent& e) {
            m_camera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });
    }

    void VoxelWorldLayer::OnUpdate(const float ts) {
        if (!m_player || !m_world) return;

        // 1. Mise à jour du chargement des chunks
        m_world->updateLoadedChunks(m_player->position, m_config.renderDistance);
        m_world->processChunkLoading();

        // 2. CRUCIAL: Construction des meshes (séparé du chargement)
        m_world->processMeshBuilding();

        // 3. Mise à jour des entités
        m_entityManager->updateAll(ts, *m_world);

        // Debug: afficher les statistiques toutes les 60 frames
        static int frameCount = 0;
        if (++frameCount % 60 == 0) {
            ash::Logger::info() << "Chunks: " << m_world->getLoadedChunkCount()
                               << " | Pending Load: " << m_world->getPendingLoadCount()
                               << " | Pending Mesh: " << m_world->getPendingMeshCount();
        }
    }

    void VoxelWorldLayer::OnRender() {
        if (!m_camera || !m_worldRenderer) return;

        ash::RenderCommand::EnableDepthTest(true);
        ash::RenderCommand::SetDepthFunc(ash::RenderCommand::DepthFunc::Less);

        renderSkybox();
        renderWorld();
    }

    void VoxelWorldLayer::setRenderDistance(const int distance) {
        m_config.renderDistance = distance;
    }

    void VoxelWorldLayer::setupCamera() {
        m_camera = ash::MakeRef<ash::PerspectiveCamera>();
        m_camera->SetPosition({0, 70, 0});
        m_camera->SetFov(70.f);
        m_camera->SetRotation(45.f, 0.f);
    }

    void VoxelWorldLayer::setupShader() {
        m_shader = ash::AssetLibrary::Shaders().Get("shaders/chunk");
    }

    void VoxelWorldLayer::setupWorld() {
        auto generator = std::make_unique<NaturalTerrainGenerator>(0);
        m_world = std::make_unique<World>(std::move(generator));
        m_worldRenderer = std::make_unique<WorldRenderer>(*m_world, *m_camera, *m_shader);
    }

    void VoxelWorldLayer::setupEntityManager() {
        m_entityManager = std::make_unique<EntityManager>();
    }

    void VoxelWorldLayer::setupPlayer() {
        m_player = m_entityManager->createEntity<Player>(m_camera);
        m_player->position = {0, 70, 0};
        m_player->velocity = {0.0f, 0.0f, 0.0f};
    }

    void VoxelWorldLayer::setupWorldInteractor() {
        m_worldInteractor = std::make_unique<WorldInteractor>(*m_world, *m_worldRenderer);
        m_worldInteractor->setMaxReach(64.0f);
        m_worldInteractor->setSelectedVoxelID(VoxelID::DIRT);
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
        m_inputHandler = std::make_unique<InputHandler>(
            *this,
            m_player->getController(),
            *m_worldInteractor
        );
    }

    void VoxelWorldLayer::renderSkybox() const {
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

        ash::RenderCommand::SetDepthWrite(true);
        ash::RenderCommand::SetDepthFunc(ash::RenderCommand::DepthFunc::Less);
    }

    void VoxelWorldLayer::renderWorld() const {
        m_worldRenderer->render();
    }
}
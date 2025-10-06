#include "Voxelity/layers/VoxelWorldLayer.h"
#include "Voxelity/entities/Player.h"
#include "Voxelity/voxelWorld/generation/NaturalTerrainGenerator.h"
#include "Ashen/core/Input.h"
#include "Ashen/events/ApplicationEvent.h"
#include "Ashen/renderer/RenderCommand.h"
#include "Ashen/renderer/Renderer.h"
#include "Ashen/resources/ResourceManager.h"
#include "Voxelity/voxelWorld/generation/FlatTerrainGenerator.h"

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

        // Fixed timestep loop (Minecraft-style) pour la physique
        m_tickAccumulator += ts;

        // Limiter pour éviter spiral of death
        if (m_tickAccumulator > m_config.fixedDeltaTime * m_config.maxTicksPerFrame) {
            m_tickAccumulator = m_config.fixedDeltaTime * m_config.maxTicksPerFrame;
        }

        // Exécuter les ticks avec delta fixe (physique/logique)
        int ticksExecuted = 0;
        while (m_tickAccumulator >= m_config.fixedDeltaTime && ticksExecuted < m_config.maxTicksPerFrame) {
            // Mise à jour des entités (physique, logique) avec fixed timestep
            m_entityManager->updateAll(m_config.fixedDeltaTime, *m_world);

            m_tickAccumulator -= m_config.fixedDeltaTime;
            ticksExecuted++;
        }

        // Mise à jour VISUELLE du joueur (chaque frame pour fluidité)
        m_player->updateVisuals(ts);

        // Mise à jour du chargement des chunks (chaque frame)
        m_world->updateLoadedChunks(m_player->position, m_config.renderDistance);
        m_world->processChunkLoading();

        // Construction des meshes (chaque frame)
        m_world->processMeshBuilding();

        // Debug: afficher les statistiques
        static int frameCount = 0;
        if (++frameCount % 120 == 0) {
            ash::Logger::info() << "Chunks: " << m_world->getLoadedChunkCount()
                               << " | Pending Load: " << m_world->getPendingLoadCount()
                               << " | Pending Mesh: " << m_world->getPendingMeshCount()
                               << " | Ticks: " << ticksExecuted;
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
        auto shader = ash::AssetLibrary::Shaders().Get("shaders/mountain_skybox");

        const std::array<std::string, 6> faces = {
            "resources/textures/mountain_skybox/right.jpg",
            "resources/textures/mountain_skybox/left.jpg",
            "resources/textures/mountain_skybox/top.jpg",
            "resources/textures/mountain_skybox/bottom.jpg",
            "resources/textures/mountain_skybox/front.jpg",
            "resources/textures/mountain_skybox/back.jpg"
        };

        m_skybox = ash::MakeRef<ash::SkyboxCubeMap>(faces, shader);
    }

    void VoxelWorldLayer::setupInputHandler() {
        m_inputHandler = std::make_unique<InputHandler>(
            *this,
            m_player->getController(),
            *m_worldInteractor
        );
    }

    void VoxelWorldLayer::renderSkybox() const {
        if (m_skybox)
            m_skybox->Render(m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix());
    }

    void VoxelWorldLayer::renderWorld() const {
        if (m_worldRenderer)
            m_worldRenderer->render();
    }
}

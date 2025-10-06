#ifndef VOXELITY_WORLDRENDERLAYER_H
#define VOXELITY_WORLDRENDERLAYER_H

#include "Ashen/renderer/Camera.h"
#include "Ashen/core/Types.h"
#include "Voxelity/VoxelityApp.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/world/WorldInteractor.h"
#include "Voxelity/voxelWorld/world/WorldRenderer.h"
#include "Voxelity/input/InputHandler.h"
#include "Voxelity/entities/EntityManager.h"

namespace voxelity {
    class Player;

    struct WorldConfig {
        int renderDistance = 4;
        int renderHeight = 2;
    };

    class VoxelWorldLayer final : public ash::Layer {
    public:
        VoxelWorldLayer();
        ~VoxelWorldLayer() override;

        void OnEvent(ash::Event& event) override;
        void OnUpdate(float ts) override;
        void OnRender() override;

        // Configuration
        void setRenderDistance(int distance);
        int getRenderDistance() const { return m_config.renderDistance; }

        // Accès pour InputHandler
        World& getWorld() const { return *m_world; }
        WorldRenderer& getWorldRenderer() const { return *m_worldRenderer; }
        WorldInteractor& getWorldInteractor() const { return *m_worldInteractor; }
        Player& getPlayer() const { return *m_player; }
        EntityManager& getEntityManager() const { return *m_entityManager; }

    private:
        WorldConfig m_config;

        // Systèmes principaux
        ash::Scope<World> m_world;
        ash::Scope<WorldRenderer> m_worldRenderer;
        ash::Scope<WorldInteractor> m_worldInteractor;
        ash::Scope<EntityManager> m_entityManager;
        Player* m_player; // Géré par EntityManager
        ash::Scope<InputHandler> m_inputHandler;

        // Caméra et shaders
        ash::Ref<ash::PerspectiveCamera> m_camera;
        ash::Ref<ash::ShaderProgram> m_shader;

        // Skybox
        ash::Ref<ash::ShaderProgram> m_skyboxShader;
        ash::Ref<ash::VertexArray> m_skyboxVAO;
        ash::Ref<ash::VertexBuffer> m_skyboxVBO;
        ash::Ref<ash::TextureCubeMap> m_skyboxTexture;

        // Initialisation
        void setupCamera();
        void setupShader();
        void setupWorld();
        void setupPlayer();
        void setupEntityManager();
        void setupWorldInteractor();
        void setupSkybox();
        void setupInputHandler();

        // Rendu
        void renderSkybox() const;
        void renderWorld() const;
    };
}

#endif
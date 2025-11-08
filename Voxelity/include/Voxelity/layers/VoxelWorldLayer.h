#ifndef VOXELITY_WORLDRENDERLAYER_H
#define VOXELITY_WORLDRENDERLAYER_H

#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Core/Types.h"

#include "Voxelity/VoxelityApp.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/world/WorldInteractor.h"
#include "Voxelity/voxelWorld/render/WorldRenderer.h"
#include "Voxelity/input/InputHandler.h"
#include "Voxelity/entities/EntityManager.h"

namespace voxelity {
    class Player;

    struct WorldConfig {
        int renderDistance = 8;
        int renderHeight = 2;

        // Fixed timestep Minecraft-style (20 ticks/second)
        float tickRate = 20.0f; // 20 TPS comme Minecraft
        float fixedDeltaTime = 1.0f / 20.0f; // 0.05s par tick
        int maxTicksPerFrame = 10; // Limite pour éviter spiral of death
    };

    class VoxelWorldLayer final : public ash::Layer {
    public:
        VoxelWorldLayer();

        ~VoxelWorldLayer() override;

        void OnEvent(ash::Event &event) override;

        void OnUpdate(float ts) override;

        void OnRender() override;

        // Configuration
        void setRenderDistance(int distance);

        int getRenderDistance() const { return m_config.renderDistance; }

        // Accès pour InputHandler
        World &getWorld() const { return *m_world; }
        WorldRenderer &getWorldRenderer() const { return *m_worldRenderer; }
        WorldInteractor &getWorldInteractor() const { return *m_worldInteractor; }
        Player &getPlayer() const { return *m_player; }
        EntityManager &getEntityManager() const { return *m_entityManager; }

    private:
        WorldConfig m_config;

        // Fixed timestep accumulator
        float m_tickAccumulator = 0.0f;

        // Systèmes principaux
        ash::Own<World> m_world;
        ash::Own<WorldRenderer> m_worldRenderer;
        ash::Own<WorldInteractor> m_worldInteractor;
        ash::Own<EntityManager> m_entityManager;
        Player *m_player; // Géré par EntityManager
        ash::Own<InputHandler> m_inputHandler;

        // Caméra et shaders
        ash::Ref<ash::PerspectiveCamera> m_camera;
        ash::Ref<ash::ShaderProgram> m_shader;

        // Skybox
        // ash::Ref<ash::SkyboxCubeMap> m_skybox;

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
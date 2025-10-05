#ifndef VOXELITY_WORLDRENDERLAYER_H
#define VOXELITY_WORLDRENDERLAYER_H

#include "Ashen/renderer/Camera.h"
#include "Ashen/core/Types.h"

#include "Voxelity/VoxelityApp.h"
#include "Voxelity/voxelWorld/world/World.h"
#include "Voxelity/voxelWorld/world/WorldInteractor.h"
#include "Voxelity/voxelWorld/world/WorldRenderer.h"
#include "Voxelity/input/InputHandler.h"

namespace voxelity {
    class Player;

    constexpr unsigned int RENDER_DISTANCE = 2;
    constexpr unsigned int RENDER_HEIGHT = 2;

    class VoxelWorldLayer final : public pixl::Layer {
    public:
        VoxelWorldLayer();

        ~VoxelWorldLayer() override;

        void OnEvent(pixl::Event &event) override;

        void OnUpdate(float ts) override;

        void OnRender() override;

        // Accès pour InputHandler
        [[nodiscard]] World &getWorld() const { return *m_world; }
        [[nodiscard]] WorldRenderer &getWorldRenderer() const { return *m_worldRenderer; }
        [[nodiscard]] WorldInteractor &getWorldInteractor() const { return *m_worldInteractor; }
        [[nodiscard]] Player &getPlayer() const { return *m_player; }

    private:
        // Initialisation
        void setupCamera();

        void setupShader();

        void setupWorld();

        void setupPlayer();

        void setupWorldInteractor();

        void setupSkybox();

        void setupInputHandler();

        // Rendu
        void renderSkybox() const;

        void renderWorld() const;

        // Systèmes principaux
        pixl::Scope<World> m_world;
        pixl::Scope<WorldRenderer> m_worldRenderer;
        pixl::Scope<WorldInteractor> m_worldInteractor;
        pixl::Scope<Player> m_player;
        pixl::Scope<InputHandler> m_inputHandler;

        // Caméra et shader
        pixl::Ref<pixl::PerspectiveCamera> m_camera;
        pixl::Ref<pixl::ShaderProgram> m_shader;

        // Skybox
        pixl::Ref<pixl::ShaderProgram> m_skyboxShader;
        pixl::Ref<pixl::VertexArray> m_skyboxVAO;
        pixl::Ref<pixl::VertexBuffer> m_skyboxVBO;
        pixl::Ref<pixl::TextureCubeMap> m_skyboxTexture;

        // UI/Debug
        pixl::Ref<pixl::OrthographicCamera> m_orthoCam;
        glm::ivec3 m_targetedBlockPos{};
        bool m_hasTargetedBlock = false;

        // FPS Counter
        float m_fpsTimer = 0.0f;
        int m_frameCount = 0;
    };
}

#endif //VOXELITY_WORLDRENDERLAYER_H

#ifndef ASHEN_FEATURESDEMOLAYER_H
#define ASHEN_FEATURESDEMOLAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Graphics/CameraControllers/CameraController.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Nodes/NodeGraph.h"

namespace ash {
    /**
     * @brief Layer de démonstration des nouvelles fonctionnalités
     * - Shader Hot-Reload
     * - Performance Profiler
     * - Scene Serialization
     */
    class FeaturesDemoLayer final : public Layer {
    public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnEvent(Event& event) override;

    private:
        void CreateDemoScene();
        void SaveScene();
        void LoadScene();
        void PrintProfilerStats();
        void CreateCustomShader();

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<Camera> m_Camera;

        // Scene
        Own<NodeGraph> m_NodeGraph;

        // Meshes
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;

        // Materials
        Ref<Material> m_Material;

        // Shaders
        Ref<ShaderProgram> m_CustomShader;

        // State
        float m_Time = 0.0f;
        uint32_t m_FrameCount = 0;
        bool m_ShowHelp = true;
    };
}

#endif // ASHEN_FEATURESDEMOLAYER_H

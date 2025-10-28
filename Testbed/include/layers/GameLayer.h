#ifndef ASHEN_GAMELAYER_H
#define ASHEN_GAMELAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Camera/CameraController.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Graphics/Objects/Mesh.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"

namespace ash {
    class GameLayer final : public Layer {
    public:
        void OnAttach() override;

        void OnUpdate(float deltaTime) override;

        void OnRender() override;

        void OnEvent(Event &event) override;

    private:
        // ========== ZONE 1: SPATIAL (PBR) ==========
        void RenderSpatialZone() const;

        // ========== ZONE 2: TOON ==========
        void RenderToonZone() const;

        // ========== ZONE 3: UNLIT ==========
        void RenderUnlitZone() const;

        // ========== SETUP ==========
        void SetupMeshes();

        void SetupCamera();

        void SetupMaterials();

        void SetupLights();

        // ========== MEMBRES ==========

        // Camera
        Ref<CameraController> m_CameraController;
        Ref<Camera> m_Camera;

        // Meshes
        Ref<Mesh> m_CustomMesh;
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;
        Ref<Mesh> m_PlaneMesh;

        // Matériaux SPATIAL (PBR) - Zone 1
        Ref<Material> m_SpatialMaterial;
        Ref<Material> m_MetallicMaterial;
        Ref<Material> m_RoughMaterial;
        Ref<Material> m_RedMaterial;
        Ref<Material> m_BlueMaterial;

        // Matériaux TOON - Zone 2
        Ref<Material> m_ToonMaterial;
        Ref<Material> m_ToonMaterial2;
        Ref<Material> m_ToonMaterial3;
        Ref<Material> m_ToonRedMaterial;

        // Matériaux UNLIT - Zone 3
        Ref<Material> m_UnlitMaterial;
        Ref<Material> m_UnlitYellowMaterial;
        Ref<Material> m_UnlitCyanMaterial;
        Ref<Material> m_UnlitMagentaMaterial;
        Ref<Material> m_UnlitOrangeMaterial;

        // Sol
        Ref<Material> m_GroundMaterial;

        // Lighting
        DirectionalLight m_DirectionalLight;
        Vector<PointLight> m_PointLights;

        // State
        float m_Time = 0.0f;
        bool m_ShowWireframe = false;
    };
}

#endif //ASHEN_GAMELAYER_H

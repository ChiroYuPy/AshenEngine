#ifndef PHYSICS_DEMO_LAYER_H
#define PHYSICS_DEMO_LAYER_H

#include "Ashen/Core/Layer.h"
#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Graphics/CameraControllers/OrbitCameraController.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Graphics/Objects/Material.h"
#include "Ashen/Physics/Physics.h"
#include "Ashen/Particles/Particles.h"
#include "Ashen/Nodes/Node3D.h"

namespace ash {
    class PhysicsDemoLayer : public Layer {
    public:
        PhysicsDemoLayer() : Layer("PhysicsDemoLayer") {}

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnEvent(Event& event) override;

    private:
        // Camera
        Ref<PerspectiveCamera> m_Camera;
        Ref<OrbitCameraController> m_CameraController;

        // Physics - Order matters for destruction!
        Own<PhysicsWorld> m_PhysicsWorld;
        Vector<Own<Rigidbody>> m_Rigidbodies;      // Destroyed first (references Colliders and Nodes)
        Vector<Own<Collider>> m_Colliders;         // Destroyed second (references Nodes)
        Vector<Own<Node3D>> m_PhysicsNodes;        // Destroyed last

        // Particles
        Own<ParticleSystem> m_ParticleSystem;

        // Meshes
        Ref<Mesh> m_CubeMesh;
        Ref<Mesh> m_SphereMesh;
        Ref<Mesh> m_PlaneMesh;

        // Materials
        Ref<Material> m_CubeMaterial;
        Ref<Material> m_GroundMaterial;

        // Lights
        DirectionalLight m_DirectionalLight;
        Vector<PointLight> m_PointLights;

        float m_Time = 0.0f;
        float m_SpawnTimer = 0.0f;

        void SetupCamera();
        void SetupPhysics();
        void SetupParticles();
        void SetupMeshes();
        void SetupMaterials();
        void SetupLights();
        void SpawnPhysicsCube();
    };
}

#endif // PHYSICS_DEMO_LAYER_H
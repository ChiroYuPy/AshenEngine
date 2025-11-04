#include "layers/PhysicsDemoLayer.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Events/EventDispatcher.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Resources/ResourceManager.h"
#include "Ashen/Core/Application.h"
#include "Ashen/Math/Random.h"

namespace ash {
    void PhysicsDemoLayer::OnAttach() {
        SetupCamera();
        SetupMeshes();
        SetupMaterials();
        SetupLights();
        SetupPhysics();
        SetupParticles();

        Logger::Info() << "=== Physics Demo Layer ===";
        Logger::Info() << "Les cubes vont tomber avec la physique!";
        Logger::Info() << "Des particules apparaissent au centre!";
    }

    void PhysicsDemoLayer::OnDetach() {
        // Nettoyer dans le bon ordre pour éviter les problèmes de pointeurs pendants
        m_Rigidbodies.clear();
        m_Colliders.clear();
        m_PhysicsNodes.clear();
        m_PhysicsWorld.reset();
    }

    void PhysicsDemoLayer::OnUpdate(float deltaTime) {
        m_CameraController->Update(deltaTime);
        m_Time += deltaTime;
        m_SpawnTimer += deltaTime;

        // Faire tourner la lumière
        float angle = m_Time * 0.5f;
        m_PointLights[0].position = Vec3(Sin(angle) * 10.0f, 5.0f, Cos(angle) * 10.0f);

        // Spawner des cubes toutes les 2 secondes
        if (m_SpawnTimer > 2.0f) {
            m_SpawnTimer = 0.0f;
            SpawnPhysicsCube();
        }

        // Update physique
        m_PhysicsWorld->Step(deltaTime);

        // Update particules
        if (m_ParticleSystem) {
            m_ParticleSystem->Update(deltaTime);
        }

        // Supprimer les cubes qui sont tombés trop bas
        for (size_t i = 1; i < m_Rigidbodies.size(); ) {  // Start at 1 to skip ground
            if (m_Rigidbodies[i]->GetNode()->GetGlobalPosition().y < -20.0f) {
                // Retirer du monde physique avant de détruire
                m_PhysicsWorld->RemoveRigidbody(m_Rigidbodies[i].get());

                m_PhysicsNodes.erase(m_PhysicsNodes.begin() + i);
                m_Rigidbodies.erase(m_Rigidbodies.begin() + i);
                m_Colliders.erase(m_Colliders.begin() + i);
            } else {
                ++i;
            }
        }
    }

    void PhysicsDemoLayer::OnRender() {
        Renderer3D::BeginScene(*m_Camera);
        Renderer3D::SetDirectionalLight(m_DirectionalLight);
        Renderer3D::ClearLights();

        for (const auto& light : m_PointLights)
            Renderer3D::AddPointLight(light);

        Renderer3D::SetAmbientLight(Vec3(0.1f));

        // Dessiner le sol
        Mat4 groundTransform = glm::translate(Mat4(1.0f), Vec3(0, -5, 0))
                             * glm::scale(Mat4(1.0f), Vec3(20, 1, 20));
        Renderer3D::Submit(m_PlaneMesh, m_GroundMaterial, groundTransform);

        // Dessiner les cubes physiques (skip index 0 qui est le sol)
        for (size_t i = 1; i < m_Rigidbodies.size(); ++i) {
            Transform3D globalTransform = m_Rigidbodies[i]->GetNode()->GetGlobalTransform();
            Mat4 transform = globalTransform.ToMatrix();

            Renderer3D::Submit(m_CubeMesh, m_CubeMaterial, transform);
        }

        Renderer3D::EndScene();

        // Dessiner les particules
        if (m_ParticleSystem) {
            ParticleRenderer::RenderParticles(m_ParticleSystem.get(), *m_Camera);
        }
    }

    void PhysicsDemoLayer::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e) {
            if (e.GetKeyCode() == Key::Escape) {
                if (m_CameraController->IsEnabled()) {
                    m_CameraController->SetEnabled(false);
                    Input::SetCursorMode(CursorMode::Normal);
                } else {
                    Application::Get().Stop();
                }
                return true;
            }
            if (e.GetKeyCode() == Key::Space) {
                // Spawner un cube manuellement
                SpawnPhysicsCube();
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
            if (e.GetMouseButton() == MouseButton::Left) {
                if (!m_CameraController->IsEnabled()) {
                    m_CameraController->SetEnabled(true);
                    Input::SetCursorMode(CursorMode::Captured);
                }
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e) {
            m_Camera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });

        m_CameraController->OnEvent(event);
    }

    void PhysicsDemoLayer::SetupCamera() {
        m_Camera = MakeRef<PerspectiveCamera>(60.f, 1.f, 0.1f, 1000.f);
        m_CameraController = OrbitCameraController::Create(*m_Camera);

        m_Camera->SetPosition({15, 10, 15});
        m_Camera->LookAt({0, 0, 0});
    }

    void PhysicsDemoLayer::SetupPhysics() {
        m_PhysicsWorld = MakeOwn<PhysicsWorld>();
        m_PhysicsWorld->SetGravity(Vec3(0, -9.81f, 0));

        // Réserver de l'espace pour éviter les réallocations qui invalideraient les pointeurs
        m_PhysicsNodes.reserve(50);
        m_Rigidbodies.reserve(50);
        m_Colliders.reserve(50);

        // Créer un sol statique
        auto groundNode = MakeOwn<Node3D>();
        groundNode->SetPosition(Vec3(0, -5, 0));
        m_PhysicsNodes.push_back(std::move(groundNode));
        Node3D* groundNodePtr = m_PhysicsNodes.back().get();

        auto groundRb = MakeOwn<Rigidbody>(groundNodePtr);
        groundRb->SetBodyType(BodyType::Static);
        m_Rigidbodies.push_back(std::move(groundRb));
        Rigidbody* groundRbPtr = m_Rigidbodies.back().get();

        auto groundCollider = MakeOwn<BoxCollider>(Vec3(20, 1, 20));
        groundCollider->SetNode(groundNodePtr);
        groundCollider->SetRigidbody(groundRbPtr);
        m_Colliders.push_back(std::move(groundCollider));
        Collider* groundColliderPtr = m_Colliders.back().get();

        groundRbPtr->SetCollider(groundColliderPtr);
        m_PhysicsWorld->AddRigidbody(groundRbPtr);
        groundRbPtr->WakeUp();  // Ensure static bodies are awake

        Logger::Info() << "Monde physique créé avec gravité: " << m_PhysicsWorld->GetGravity().y;
    }

    void PhysicsDemoLayer::SetupParticles() {
        m_ParticleSystem = MakeOwn<ParticleSystem>();

        ParticleEmitterConfig config;
        config.shape = ParticleEmitterShape::Sphere;
        config.shapeRadius = 2.0f;
        config.emissionRate = 10.0f;
        config.maxParticles = 100;  // Réduit de 500 pour éviter buffer overflow

        config.startColor = Vec4(1.0f, 0.8f, 0.2f, 1.0f);  // Orange/jaune
        config.endColor = Vec4(1.0f, 0.2f, 0.1f, 0.0f);    // Rouge transparent

        config.startSizeMin = 0.3f;
        config.startSizeMax = 0.3f;
        config.endSizeMin = 0.1f;
        config.endSizeMax = 0.1f;
        config.lifetimeMin = 2.0f;
        config.lifetimeMax = 2.0f;

        config.startVelocityMin = Vec3(-1.0f, 1.0f, -1.0f) * 2.0f;
        config.startVelocityMax = Vec3(1.0f, 3.0f, 1.0f) * 2.0f;

        config.gravity = Vec3(0, -2.0f, 0);

        m_ParticleSystem->SetConfig(config);
        m_ParticleSystem->Play();

        Logger::Info() << "Système de particules créé!";
    }

    void PhysicsDemoLayer::SetupMeshes() {
        m_CubeMesh = AssetLibrary::Meshes().GetCube();
        m_SphereMesh = AssetLibrary::Meshes().GetSphere();
        m_PlaneMesh = AssetLibrary::Meshes().GetPlane();
    }

    void PhysicsDemoLayer::SetupMaterials() {
        m_CubeMaterial = AssetLibrary::Materials().CreateSpatial(
            "physics_cube",
            Vec4(0.8f, 0.3f, 0.3f, 1.0f),
            0.2f, 0.6f, 0.5f
        );

        m_GroundMaterial = AssetLibrary::Materials().CreateSpatial(
            "physics_ground",
            Vec4(0.3f, 0.3f, 0.3f, 1.0f),
            0.0f, 0.9f, 0.2f
        );
    }

    void PhysicsDemoLayer::SetupLights() {
        m_DirectionalLight.direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
        m_DirectionalLight.color = Vec3(1.0f, 0.95f, 0.9f);
        m_DirectionalLight.intensity = 0.8f;

        m_PointLights = {
            PointLight{Vec3(0.0f, 5.0f, 0.0f), Vec3(1.0f, 0.8f, 0.4f), 40.0f}
        };
    }

    void PhysicsDemoLayer::SpawnPhysicsCube() {
        // Position aléatoire en hauteur
        Random rng;
        Vec3 spawnPos(
            rng.Float(-5.0f, 5.0f),
            rng.Float(5.0f, 15.0f),
            rng.Float(-5.0f, 5.0f)
        );

        auto cubeNode = MakeOwn<Node3D>();
        cubeNode->SetPosition(spawnPos);
        m_PhysicsNodes.push_back(std::move(cubeNode));
        Node3D* cubeNodePtr = m_PhysicsNodes.back().get();

        auto rb = MakeOwn<Rigidbody>(cubeNodePtr);
        rb->SetBodyType(BodyType::Dynamic);
        rb->SetMass(1.0f);

        // Rotation aléatoire initiale
        Vec3 angularVel(
            rng.Float(-3.0f, 3.0f),
            rng.Float(-3.0f, 3.0f),
            rng.Float(-3.0f, 3.0f)
        );
        rb->SetAngularVelocity(angularVel);

        m_Rigidbodies.push_back(std::move(rb));
        Rigidbody* rbPtr = m_Rigidbodies.back().get();

        auto collider = MakeOwn<BoxCollider>(Vec3(1, 1, 1));
        collider->SetNode(cubeNodePtr);
        collider->SetRigidbody(rbPtr);
        collider->GetMaterial().SetBounciness(0.3f);
        collider->GetMaterial().SetFriction(0.5f);
        m_Colliders.push_back(std::move(collider));
        Collider* colliderPtr = m_Colliders.back().get();

        rbPtr->SetCollider(colliderPtr);
        m_PhysicsWorld->AddRigidbody(rbPtr);
        rbPtr->WakeUp();  // Ensure cube starts awake and receives gravity

        // Debug: vérifier l'état du rigidbody
        printf("DEBUG: Cube spawned - Sleeping=%d BodyType=%d UseGravity=%d World=%p\n",
               rbPtr->IsSleeping(), (int)rbPtr->GetBodyType(), rbPtr->UseGravity(), rbPtr->GetWorld());

        Logger::Info() << "Cube spawné à la position: " << spawnPos.x << ", " << spawnPos.y << ", " << spawnPos.z;
    }
}
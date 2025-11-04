#include "layers/FeaturesDemoLayer.h"

#include "Ashen/Core/Application.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/EventDispatcher.h"
#include "Ashen/Graphics/CameraControllers/OrbitCameraController.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Resources/ResourceManager.h"
#include "Ashen/Scene/SceneSerializer.h"
#include "Ashen/Utils/Profiler.h"

namespace ash {
    void FeaturesDemoLayer::OnAttach() {
        ASH_PROFILE_FUNCTION();

        Logger::Info("===========================================");
        Logger::Info("   ASHEN ENGINE - FEATURES DEMO");
        Logger::Info("===========================================");
        Logger::Info("");
        Logger::Info("Nouvelles fonctionnalités ajoutées :");
        Logger::Info("  1. Shader Hot-Reload");
        Logger::Info("  2. Performance Profiler");
        Logger::Info("  3. Scene Serialization");
        Logger::Info("");
        Logger::Info("Contrôles :");
        Logger::Info("  H - Afficher/masquer cette aide");
        Logger::Info("  S - Sauvegarder la scène (scenes/demo.json)");
        Logger::Info("  L - Charger la scène");
        Logger::Info("  P - Afficher les stats du profiler");
        Logger::Info("  R - Reset les stats du profiler");
        Logger::Info("  C - Créer un shader custom");
        Logger::Info("  ESC - Quitter");
        Logger::Info("===========================================");
        Logger::Info("");

        // Setup camera
        m_Camera = MakeRef<PerspectiveCamera>(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
        m_Camera->SetPosition(Vec3(0.0f, 5.0f, 15.0f));
        m_CameraController = OrbitCameraController::Create(*m_Camera, Vec3(0, 0, 0), 15.0f);
        m_CameraController->SetEnabled(true);

        // Load meshes
        m_CubeMesh = AssetLibrary::Meshes().GetCube();
        m_SphereMesh = AssetLibrary::Meshes().GetSphere();

        // Create material
        m_Material = AssetLibrary::Materials().CreateSpatial(
            "demo_material",
            Vec4(0.2f, 0.6f, 1.0f, 1.0f), // Albedo bleu
            0.3f,  // Metallic
            0.4f,  // Roughness
            0.5f   // Specular
        );

        // Create demo scene
        CreateDemoScene();

        Logger::Info("✓ Demo layer initialisé");
        Logger::Info("✓ Profiler activé (macros ASH_PROFILE_*)");
        Logger::Info("");
    }

    void FeaturesDemoLayer::OnDetach() {
        ASH_PROFILE_FUNCTION();

        Logger::Info("Demo layer détaché");
    }

    void FeaturesDemoLayer::OnUpdate(float deltaTime) {
        ASH_PROFILE_FUNCTION();

        {
            ASH_PROFILE_SCOPE("CameraUpdate");
            m_CameraController->Update(deltaTime);
        }

        {
            ASH_PROFILE_SCOPE("SceneUpdate");
            if (m_NodeGraph) {
                m_NodeGraph->Process(deltaTime);
            }
        }

        m_Time += deltaTime;
        m_FrameCount++;

        // Afficher les stats du profiler toutes les 120 frames
        if (m_FrameCount % 120 == 0) {
            PrintProfilerStats();
        }
    }

    void FeaturesDemoLayer::OnRender() {
        ASH_PROFILE_FUNCTION();

        {
            ASH_PROFILE_SCOPE("3DRendering");

            Renderer3D::BeginScene(*m_Camera);

            // Setup lighting
            DirectionalLight dirLight;
            dirLight.direction = Normalize(Vec3(1.0f, -1.0f, 1.0f));
            dirLight.color = Vec3(1.0f, 1.0f, 0.9f);
            dirLight.intensity = 1.0f;
            Renderer3D::SetDirectionalLight(dirLight);
            Renderer3D::SetAmbientLight(Vec3(0.1f));

            // Render rotating cubes
            for (int i = 0; i < 5; ++i) {
                float offset = i * 3.0f - 6.0f;
                Mat4 transform = glm::translate(Mat4(1.0f), Vec3(offset, 0.0f, 0.0f));
                transform = glm::rotate(transform, m_Time + i * 0.5f, Vec3(0, 1, 0));
                transform = glm::rotate(transform, m_Time * 0.5f, Vec3(1, 0, 0));

                Renderer3D::Submit(m_CubeMesh, m_Material, transform);
            }

            // Render spheres
            for (int i = 0; i < 3; ++i) {
                float offset = i * 4.0f - 4.0f;
                Mat4 transform = glm::translate(Mat4(1.0f), Vec3(offset, 3.0f, -5.0f));
                transform = glm::scale(transform, Vec3(1.5f));

                Renderer3D::Submit(m_SphereMesh, m_Material, transform);
            }

            Renderer3D::EndScene();
        }

        // Afficher l'aide si nécessaire
        if (m_ShowHelp && m_FrameCount % 300 == 0) {
            Logger::Info("Appuie sur 'H' pour afficher/masquer l'aide");
        }
    }

    void FeaturesDemoLayer::OnEvent(Event& event) {
        ASH_PROFILE_FUNCTION();

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e) {
            switch (e.GetKeyCode()) {
                case Key::Escape:
                    Application::Get().Stop();
                    return true;

                case Key::H:
                    m_ShowHelp = !m_ShowHelp;
                    if (m_ShowHelp) {
                        Logger::Info("=== Aide activée ===");
                    } else {
                        Logger::Info("=== Aide désactivée ===");
                    }
                    return true;

                case Key::S:
                    SaveScene();
                    return true;

                case Key::L:
                    LoadScene();
                    return true;

                case Key::P:
                    Logger::Info("=== Stats du Profiler (manuel) ===");
                    Profiler::Instance().PrintResults();
                    return true;

                case Key::R:
                    Logger::Info("=== Reset du Profiler ===");
                    Profiler::Instance().Reset();
                    return true;

                case Key::C:
                    CreateCustomShader();
                    return true;

                default:
                    return false;
            }
        });

        m_CameraController->OnEvent(event);
    }

    void FeaturesDemoLayer::CreateDemoScene() {
        ASH_PROFILE_FUNCTION();

        m_NodeGraph = MakeOwn<NodeGraph>();

        // Create root node
        auto root = MakeOwn<Node3D>("DemoRoot");

        // Create some child nodes
        for (int i = 0; i < 5; ++i) {
            auto child = MakeOwn<Node3D>("Cube_" + std::to_string(i));
            child->SetPosition(Vec3(i * 3.0f - 6.0f, 0.0f, 0.0f));
            child->AddToGroup("rotating_objects");
            root->AddChild(std::move(child));
        }

        // Create sphere nodes
        for (int i = 0; i < 3; ++i) {
            auto sphere = MakeOwn<Node3D>("Sphere_" + std::to_string(i));
            sphere->SetPosition(Vec3(i * 4.0f - 4.0f, 3.0f, -5.0f));
            sphere->SetScale(Vec3(1.5f));
            sphere->AddToGroup("spheres");
            root->AddChild(std::move(sphere));
        }

        m_NodeGraph->SetRoot(std::move(root));
        m_NodeGraph->Ready();

        Logger::Info("✓ Scène de démo créée avec " + std::to_string(m_NodeGraph->GetNodeCount()) + " nodes");
    }

    void FeaturesDemoLayer::SaveScene() {
        ASH_PROFILE_FUNCTION();

        Logger::Info("=== Sauvegarde de la scène ===");

        if (!m_NodeGraph) {
            Logger::Error("Aucune scène à sauvegarder!");
            return;
        }

        bool success = SceneSerializer::SaveToFile(*m_NodeGraph, "scenes/demo.json");

        if (success) {
            Logger::Info("✓ Scène sauvegardée dans 'scenes/demo.json'");
            Logger::Info("  Tu peux éditer ce fichier JSON manuellement!");
        } else {
            Logger::Error("✗ Échec de la sauvegarde");
        }
    }

    void FeaturesDemoLayer::LoadScene() {
        ASH_PROFILE_FUNCTION();

        Logger::Info("=== Chargement de la scène ===");

        auto root = SceneSerializer::LoadFromFile("scenes/demo.json");

        if (root) {
            m_NodeGraph = MakeOwn<NodeGraph>();
            m_NodeGraph->SetRoot(std::move(root));
            m_NodeGraph->Ready();

            Logger::Info("✓ Scène chargée depuis 'scenes/demo.json'");
            Logger::Info("  Nombre de nodes: " + std::to_string(m_NodeGraph->GetNodeCount()));
        } else {
            Logger::Error("✗ Échec du chargement");
            Logger::Warn("  Crée d'abord une scène avec 'S'");
        }
    }

    void FeaturesDemoLayer::PrintProfilerStats() {
        // Ne pas profiler cette fonction pour éviter la récursion
        auto results = Profiler::Instance().GetResults();

        if (results.empty()) {
            return;
        }

        Logger::Info("--- Profiler Stats (Frame " + std::to_string(m_FrameCount) + ") ---");

        // Afficher les 5 fonctions les plus coûteuses
        size_t count = 0;
        for (const auto& result : results) {
            if (count >= 5) break;

            Logger::Info("  " + result.name + ": " +
                        std::to_string(result.avgTime) + "ms avg, " +
                        std::to_string(result.callCount) + " calls");
            count++;
        }
    }

    void FeaturesDemoLayer::CreateCustomShader() {
        Logger::Info("=== Création d'un shader custom ===");
        Logger::Info("");
        Logger::Info("Pour tester le hot-reload des shaders :");
        Logger::Info("1. Crée deux fichiers :");
        Logger::Info("   - resources/shaders/custom.vert");
        Logger::Info("   - resources/shaders/custom.frag");
        Logger::Info("");
        Logger::Info("2. Charge le shader :");
        Logger::Info("   auto shader = AssetLibrary::Shaders().Get(\"shaders/custom\");");
        Logger::Info("");
        Logger::Info("3. Active le hot-reload :");
        Logger::Info("   AssetLibrary::Shaders().EnableHotReload(\"shaders/custom\");");
        Logger::Info("");
        Logger::Info("4. Modifie les fichiers .vert/.frag");
        Logger::Info("   -> Le shader sera rechargé automatiquement!");
        Logger::Info("");
        Logger::Info("Exemple de vertex shader minimal :");
        Logger::Info("---");
        Logger::Info("#version 330 core");
        Logger::Info("layout(location = 0) in vec3 a_Position;");
        Logger::Info("uniform mat4 u_ViewProjection;");
        Logger::Info("uniform mat4 u_Transform;");
        Logger::Info("void main() {");
        Logger::Info("    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);");
        Logger::Info("}");
        Logger::Info("---");
        Logger::Info("");
        Logger::Info("Exemple de fragment shader minimal :");
        Logger::Info("---");
        Logger::Info("#version 330 core");
        Logger::Info("out vec4 FragColor;");
        Logger::Info("void main() {");
        Logger::Info("    FragColor = vec4(1.0, 0.5, 0.2, 1.0); // Orange");
        Logger::Info("}");
        Logger::Info("---");
    }
}

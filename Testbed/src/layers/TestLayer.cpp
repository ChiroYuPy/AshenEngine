#include "layers/TestLayer.h"

#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"

namespace ash {
    void TestLayer::OnAttach() {
        // Créer une caméra orthographique
        mCamera = MakeRef<OrthographicCamera>(0.0f, 1280.0f, 0.0f, 720.0f);
        Logger::Info("TestLayer attached - Testing Renderer2D");
    }

    void TestLayer::OnUpdate(float deltaTime) {
        // Rien à faire pour le moment
    }

    void TestLayer::OnRender() {
        // IMPORTANT: N'appeler BeginScene qu'une seule fois
        Renderer2D::BeginScene(*mCamera);

        // Test 1: Rectangle rempli rouge
        Renderer2D::DrawQuad(Vec3(50, 50, 0), Vec2(100, 100), Vec4(1.0f, 0.0f, 0.0f, 1.0f));

        // Test 2: Rectangle rempli vert
        Renderer2D::DrawQuad(Vec3(200, 50, 0), Vec2(100, 100), Vec4(0.0f, 1.0f, 0.0f, 1.0f));

        // Test 3: Rectangle rempli bleu
        Renderer2D::DrawQuad(Vec3(350, 50, 0), Vec2(100, 100), Vec4(0.0f, 0.0f, 1.0f, 1.0f));

        // Test 4: Rectangle outline jaune
        Renderer2D::DrawRect(Vec3(500, 50, 0), Vec2(100, 100), Vec4(1.0f, 1.0f, 0.0f, 1.0f));

        // Test 5: Ligne rouge
        Renderer2D::DrawLine(Vec3(50, 200, 0), Vec3(650, 200, 0), Vec4(1.0f, 0.0f, 0.0f, 1.0f));

        // Test 6: Cercle rempli cyan
        Renderer2D::DrawFilledCircle(Vec3(150, 350, 0), 50.0f, Vec4(0.0f, 1.0f, 1.0f, 1.0f));

        // Test 7: Cercle outline magenta
        Renderer2D::DrawCircle(Vec3(300, 350, 0), 50.0f, Vec4(1.0f, 0.0f, 1.0f, 1.0f), 0.1f, 0.01f);

        // Test 8: Rectangle avec rotation
        Renderer2D::DrawRotatedQuad(Vec3(450, 350, 0), Vec2(80, 80), glm::radians(45.0f),
                                    Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        // Test 9: Polygone
        Vector<Vec3> polygonPoints = {
            Vec3(700, 300, 0),
            Vec3(750, 250, 0),
            Vec3(800, 300, 0),
            Vec3(780, 350, 0),
            Vec3(720, 350, 0)
        };
        Renderer2D::DrawPolygon(polygonPoints, Vec4(0.5f, 0.0f, 0.5f, 1.0f));

        // Test 10: Outline de polygone
        Vector<Vec3> outlinePoints = {
            Vec3(850, 300, 0),
            Vec3(900, 250, 0),
            Vec3(950, 300, 0),
            Vec3(930, 350, 0),
            Vec3(870, 350, 0)
        };
        Renderer2D::DrawPolygonOutline(outlinePoints, Vec4(0.0f, 1.0f, 0.5f, 1.0f));

        // Test 11: Plusieurs rectangles pour tester le batching
        for (int i = 0; i < 10; i++) {
            float x = 50.0f + i * 60.0f;
            float hue = i / 10.0f;
            Vec4 color(hue, 1.0f - hue, 0.5f, 1.0f);
            Renderer2D::DrawQuad(Vec3(x, 500, 0), Vec2(50, 50), color);
        }

        Renderer2D::EndScene();
    }

    void TestLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([&](const WindowResizeEvent &e) {
            mCamera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });
    }
} // namespace ash

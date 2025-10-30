#include "layers/Renderer2DTestLayer.h"

#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/EventDispatcher.h"

namespace ash {
    void Renderer2DTestLayer::OnAttach() {
        mCamera = MakeRef<OrthographicCamera>(0.0f, 1280.0f, 0.0f, 720.0f);
        Logger::Info("TestLayer attached - Testing Renderer2D");
    }

    void Renderer2DTestLayer::OnUpdate(float deltaTime) {

    }

    void Renderer2DTestLayer::OnRender() {
        Renderer2D::BeginScene(*mCamera);

        Renderer2D::DrawQuad(Vec3(50, 50, 0), Vec2(100, 100), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Renderer2D::DrawQuad(Vec3(200, 50, 0), Vec2(100, 100), Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        Renderer2D::DrawQuad(Vec3(350, 50, 0), Vec2(100, 100), Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        Renderer2D::DrawRect(Vec3(500, 50, 0), Vec2(100, 100), Vec4(1.0f, 1.0f, 0.0f, 1.0f));
        Renderer2D::DrawLine(Vec3(50, 200, 0), Vec3(650, 200, 0), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Renderer2D::DrawFilledCircle(Vec3(150, 350, 0), 50.0f, Vec4(0.0f, 1.0f, 1.0f, 1.0f));
        Renderer2D::DrawCircle(Vec3(300, 350, 0), 50.0f, Vec4(1.0f, 0.0f, 1.0f, 1.0f), 0.1f, 0.01f);
        Renderer2D::DrawRotatedQuad(Vec3(450, 350, 0), Vec2(80, 80), glm::radians(45.0f), Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        const Vector polygonPoints = {
            Vec3(700, 300, 0),
            Vec3(750, 250, 0),
            Vec3(800, 300, 0),
            Vec3(780, 350, 0),
            Vec3(720, 350, 0)
        };
        Renderer2D::DrawPolygon(polygonPoints, Vec4(0.5f, 0.0f, 0.5f, 1.0f));

        const Vector outlinePoints = {
            Vec3(850, 300, 0),
            Vec3(900, 250, 0),
            Vec3(950, 300, 0),
            Vec3(930, 350, 0),
            Vec3(870, 350, 0)
        };
        Renderer2D::DrawPolygonOutline(outlinePoints, Vec4(0.0f, 1.0f, 0.5f, 1.0f));

        for (int i = 0; i < 10; i++) {
            const float x = 50.0f + i * 60.0f;
            const float hue = i / 10.0f;
            Vec4 color(hue, 1.0f - hue, 0.5f, 1.0f);
            Renderer2D::DrawQuad(Vec3(x, 500, 0), Vec2(50, 50), color);
        }

        Renderer2D::EndScene();
    }

    void Renderer2DTestLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([&](const WindowResizeEvent &e) {
            mCamera->OnResize(e.GetWidth(), e.GetHeight());
            return false;
        });
    }
}

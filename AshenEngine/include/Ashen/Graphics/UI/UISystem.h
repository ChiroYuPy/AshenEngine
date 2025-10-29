#ifndef ASHEN_UI_SYSTEM_H
#define ASHEN_UI_SYSTEM_H

#include "Ashen/Scene/Control.h"
#include "Ashen/Scene/Node.h"
#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Math/BBox.h"
#include "Ashen/Events/Event.h"
#include <memory>

namespace ash {

    class UISystem {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
        static void Update(float delta);
        static void Render();

        static bool HandleEvent(Event& event);

        static void SetRoot(std::shared_ptr<Node> root);
        static std::shared_ptr<Node> GetRoot();

        static void SetCamera(std::shared_ptr<Camera> camera);
        static std::shared_ptr<Camera> GetCamera();

        static void SetScreenSize(const Vec2& size);
        static Vec2 GetScreenSize();

        static void SetFocusedControl(Control* control);
        static Control* GetFocusedControl();

        static Vec2 GetMousePosition();
        static Control* GetHoveredControl();

    private:
        struct Data {
            std::shared_ptr<Node> Root;
            std::shared_ptr<Camera> Camera;
            Vec2 ScreenSize{1280.0f, 720.0f};
            Vec2 MousePosition{0.0f};
            Control* FocusedControl{nullptr};
            Control* HoveredControl{nullptr};
            bool IsRendering{false};
        };

        static Data s_Data;

        static void ProcessNode(std::shared_ptr<Node> node, float delta);
        static void RenderNode(std::shared_ptr<Node> node);
        static void InputNode(std::shared_ptr<Node> node, Event& event);
        static void UpdateHoveredControl();
        static Control* FindControlAt(const Vec2& pos, std::shared_ptr<Node> node);
    };

} // namespace ash

#endif // ASHEN_UI_SYSTEM_H
#ifndef ASHEN_UI_SERVER_H
#define ASHEN_UI_SERVER_H

#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Scene/Control.h"
#include "Ashen/Scene/Node.h"

namespace ash {
    class UIServer {
    public:
        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void RenderTree(const Ref<Node> &root);

        static void SetCamera(const Ref<Camera> &camera);

        static Ref<Camera> GetCamera();

        static void SetScreenSize(const Vec2 &size);

        static Vec2 GetScreenSize();

        static void ProcessInput(Event &event);

        static Control *GetControlAtPosition(const Vec2 &pos, const Ref<Node> &root);

        static void SetFocusedControl(Control *control);

        static Control *GetFocusedControl();

        static Vec2 GetMousePosition();

        static Control *GetHoveredControl();

        static void SetTooltip(const String &text, const Vec2 &position);

        static void ClearTooltip();

        static void DrawControl(Control *control);

        static void DrawRect(const BBox2 &rect, const Vec4 &color);

        static void DrawRectOutline(const BBox2 &rect, const Vec4 &color, float width = 1.0f);

        static void DrawRoundedRect(const BBox2 &rect, const Vec4 &color, float radius);

        static void DrawTexture(const Ref<Texture2D> &texture, const BBox2 &rect, const Vec4 &modulate = Vec4(1.0f));

        static void DrawText(const String &text, const Vec2 &position, float fontSize, const Vec4 &color);

        static void PushClipRect(const BBox2 &rect);

        static void PopClipRect();

        static BBox2 GetCurrentClipRect();

        static bool IsClipping();

    private:
        struct UIData {
            Ref<Camera> Camera;
            Vec2 ScreenSize{1280.0f, 720.0f};
            Vec2 MousePosition{0.0f};

            Control *FocusedControl = nullptr;
            Control *HoveredControl = nullptr;

            String TooltipText;
            Vec2 TooltipPosition;
            float TooltipTimer = 0.0f;

            Vector<BBox2> ClipStack;
        };

        static UIData s_Data;

        static void RenderNode(const Ref<Node> &node);

        static void UpdateHoveredControl(const Ref<Node> &root);

        static void DrawTooltip();
    };
} // namespace ash

#endif // ASHEN_UI_SERVER_H
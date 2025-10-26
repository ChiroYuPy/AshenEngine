#ifndef ASHEN_UI_MANAGER_H
#define ASHEN_UI_MANAGER_H

#include "Widget.h"
#include "Ashen/Graphics/Camera/Camera.h"

namespace ash::UI {
    class UIManager {
    public:
        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void Update(float deltaTime);

        static void Render();

        static bool HandleEvent(Event &event);

        static void AddRootWidget(const Ref<Widget> &widget);

        static void RemoveRootWidget(const Ref<Widget> &widget);

        static void ClearRootWidgets();

        static void SetScreenSize(const Vec2 &size);

        static Vec2 GetScreenSize();

        static void SetCamera(Ref<Camera> camera);

        static Ref<Camera> GetCamera();

        static void SetFocusedWidget(Ref<Widget> widget);

        static Ref<Widget> GetFocusedWidget();

        static Ref<Widget> GetHoveredWidget();

    private:
        struct UIData {
            Vector<Ref<Widget> > RootWidgets;
            Ref<Camera> Camera;
            Vec2 ScreenSize{1280.0f, 720.0f};
            WeakRef<Widget> FocusedWidget;
            WeakRef<Widget> HoveredWidget;
            Vec2 MousePosition{0.0f};
        };

        static UIData s_Data;

        static Ref<Widget> FindWidgetAtPosition(const Vec2 &pos);

        static void UpdateHoveredWidget(const Vec2 &mousePos);
    };
}

#endif //ASHEN_UI_MANAGER_H
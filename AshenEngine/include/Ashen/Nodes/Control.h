#ifndef ASHEN_CONTROL_H
#define ASHEN_CONTROL_H

#include "CanvasItem.h"
#include "Ashen/Core/Codes.h"
#include "Ashen/Math/BBox.h"

namespace ash {
    class Control : public CanvasItem {
    public:
        explicit Control(String name = "Control");

        Vec2 size = Vec2(100.f, 100.f);
        Vec2 position = Vec2(0.f, 0.f);
        Vec2 min_size = Vec2(0.f, 0.f);
        Vec2 custom_minimum_size = Vec2(0.f, 0.f);

        Anchor anchor_preset = Anchor::TopLeft;
        Vec2 anchor_left_top = Vec2(0.f, 0.f);
        Vec2 anchor_right_bottom = Vec2(0.f, 0.f);
        Vec2 margin_left_top = Vec2(0.f, 0.f);
        Vec2 margin_right_bottom = Vec2(0.f, 0.f);

        bool focusable = false;
        bool focused = false;
        bool mouse_filter = true;

        String tooltip_text;

        BBox2 GetGlobalRect() const;

        BBox2 GetRect() const;

        Vec2 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec2 &pos);

        Vec2 GetSize() const;

        void SetSize(const Vec2 &s);

        void SetSize(const float w, const float h) { SetSize(Vec2(w, h)); }

        Vec2 GetMinimumSize() const;

        Vec2 GetCenter() const;

        bool HasPoint(const Vec2 &point) const;

        void GrabFocus();

        void ReleaseFocus();

        bool HasFocus() const;

        virtual void OnMouseEnter();

        virtual void OnMouseExit();

        virtual void OnMouseButton(MouseButton button, bool pressed, const Vec2 &pos);

        virtual void OnMouseMotion(const Vec2 &pos, const Vec2 &relative);

        virtual void OnMouseWheel(float delta_x, float delta_y);

        virtual void OnKey(bool pressed, int key_code);

        virtual void OnTextInput(const String &text);

        virtual void OnFocusEnter();

        virtual void OnFocusExit();

        virtual void OnResized();

        Vec2 ToLocal(const Vec2 &global_point) const;

        Vec2 ToGlobal(const Vec2 &local_point) const;

        void SetAnchors(float left, float top, float right, float bottom);

        void SetAnchorsPreset(Anchor preset);

    protected:
        virtual void UpdateLayout();
    };
}

#endif // ASHEN_CONTROL_H

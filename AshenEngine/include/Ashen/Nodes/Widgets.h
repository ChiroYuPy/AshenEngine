#ifndef ASHEN_WIDGETS_H
#define ASHEN_WIDGETS_H

#include "Control.h"

namespace ash {
    class Label : public Control {
    public:
        explicit Label(String name = "Label");

        String text;
        float font_size = 16.f;
        Vec4 font_color = Vec4(1.f, 1.f, 1.f, 1.f);
        bool autowrap = false;

        enum class Align { Left, Center, Right };

        Align horizontal_align = Align::Left;
        Align vertical_align = Align::Center;

        void SetText(const String &t);

        const String &GetText() const;

    protected:
        void UpdateMinimumSize();
    };

    class Button : public Control {
    public:
        explicit Button(String name = "Button");

        String text;
        bool pressed = false;
        bool toggle_mode = false;
        bool disabled = false;

        void SetText(const String &t);

        const String &GetText() const;

        void SetDisabled(bool d);

        bool IsDisabled() const;

        virtual void OnPressed();

        virtual void OnReleased();

        virtual void OnToggled(bool is_pressed);

        void OnMouseButton(MouseButton button, bool is_pressed, const Vec2 &pos) override;
    };

    class Panel : public Control {
    public:
        explicit Panel(String name = "Panel");

        Vec4 background_color = Vec4(0.2f, 0.2f, 0.2f, 1.f);
    };
}

#endif //ASHEN_WIDGETS_H

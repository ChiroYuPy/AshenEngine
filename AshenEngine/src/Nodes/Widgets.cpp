#include "Ashen/Nodes/Widgets.h"

namespace ash {
    Label::Label(String name) : Control(MovePtr(name)) {
        mouse_filter = false;
    }

    void Label::SetText(const String &t) {
        text = t;
        UpdateMinimumSize();
    }

    const String &Label::GetText() const { return text; }

    void Label::UpdateMinimumSize() {
        min_size = Vec2(text.length() * font_size * 0.6f, font_size + 4.f);
    }

    Button::Button(String name) : Control(MovePtr(name)) {
        focusable = true;
        size = Vec2(100.f, 30.f);
    }

    void Button::SetText(const String &t) { text = t; }

    const String &Button::GetText() const { return text; }

    void Button::SetDisabled(const bool d) { disabled = d; }

    bool Button::IsDisabled() const { return disabled; }

    void Button::OnPressed() {
    }

    void Button::OnReleased() {
    }

    void Button::OnToggled(bool is_pressed) {
    }

    void Button::OnMouseButton(const MouseButton button, const bool is_pressed, const Vec2 &pos) {
        if (disabled || button != MouseButton::Left) return;

        if (is_pressed) {
            pressed = true;
            GrabFocus();
            OnPressed();
        } else if (pressed) {
            if (toggle_mode) {
                pressed = !pressed;
                OnToggled(pressed);
            } else {
                pressed = false;
            }
            OnReleased();
        }
    }

    Panel::Panel(String name) : Control(MovePtr(name)) {
    }
}

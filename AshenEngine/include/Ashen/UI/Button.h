#ifndef ASHEN_UI_BUTTON_H
#define ASHEN_UI_BUTTON_H

#include "Ashen/Nodes/Control.h"
#include "Ashen/Math/Math.h"
#include <functional>

namespace ash {
    enum class ButtonState {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    class Button : public Control {
    public:
        explicit Button(const String& text = "Button");

        // Text
        String GetText() const { return m_Text; }
        void SetText(const String& text) { m_Text = text; }

        // Callbacks
        using ClickCallback = std::function<void()>;
        void SetOnClick(ClickCallback callback) { m_OnClick = callback; }

        using HoverCallback = std::function<void(bool)>;
        void SetOnHover(HoverCallback callback) { m_OnHover = callback; }

        // State
        ButtonState GetState() const { return m_State; }
        bool IsDisabled() const { return m_IsDisabled; }
        void SetDisabled(bool disabled);

        // Appearance
        Vec4 GetNormalColor() const { return m_NormalColor; }
        void SetNormalColor(const Vec4& color) { m_NormalColor = color; }

        Vec4 GetHoverColor() const { return m_HoverColor; }
        void SetHoverColor(const Vec4& color) { m_HoverColor = color; }

        Vec4 GetPressedColor() const { return m_PressedColor; }
        void SetPressedColor(const Vec4& color) { m_PressedColor = color; }

        Vec4 GetDisabledColor() const { return m_DisabledColor; }
        void SetDisabledColor(const Vec4& color) { m_DisabledColor = color; }

        Vec4 GetTextColor() const { return m_TextColor; }
        void SetTextColor(const Vec4& color) { m_TextColor = color; }

        float GetFontSize() const { return m_FontSize; }
        void SetFontSize(float size) { m_FontSize = Max(size, 1.0f); }

        // Events
        void OnMouseEnter() override;
        void OnMouseExit() override;
        void OnMouseButton(MouseButton button, bool pressed, const Vec2& pos) override;

        // Rendering
        void _Draw() override;

    private:
        String m_Text;
        ButtonState m_State = ButtonState::Normal;
        bool m_IsDisabled = false;

        Vec4 m_NormalColor = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
        Vec4 m_HoverColor = Vec4(0.3f, 0.3f, 0.3f, 1.0f);
        Vec4 m_PressedColor = Vec4(0.15f, 0.15f, 0.15f, 1.0f);
        Vec4 m_DisabledColor = Vec4(0.1f, 0.1f, 0.1f, 0.5f);
        Vec4 m_TextColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

        float m_FontSize = 16.0f;

        ClickCallback m_OnClick;
        HoverCallback m_OnHover;
    };
}

#endif // ASHEN_UI_BUTTON_H
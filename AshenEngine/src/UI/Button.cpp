#include "Ashen/UI/Button.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Button::Button(const String& text)
        : Control("Button")
        , m_Text(text) {
        focusable = true;
        size = Vec2(120.0f, 40.0f);
    }

    void Button::SetDisabled(bool disabled) {
        m_IsDisabled = disabled;
        m_State = disabled ? ButtonState::Disabled : ButtonState::Normal;
    }

    void Button::OnMouseEnter() {
        if (!m_IsDisabled) {
            m_State = ButtonState::Hovered;
            if (m_OnHover) m_OnHover(true);
        }
    }

    void Button::OnMouseExit() {
        if (!m_IsDisabled) {
            m_State = ButtonState::Normal;
            if (m_OnHover) m_OnHover(false);
        }
    }

    void Button::OnMouseButton(MouseButton button, bool pressed, const Vec2& pos) {
        if (m_IsDisabled) return;

        if (button == MouseButton::Left) {
            if (pressed) {
                m_State = ButtonState::Pressed;
            } else {
                m_State = ButtonState::Hovered;
                if (m_OnClick) m_OnClick();
            }
        }
    }

    void Button::_Draw() {
        Vec2 globalPos = GetGlobalPosition();
        Vec2 size = GetSize();

        // Choose color based on state
        Vec4 color = m_NormalColor;
        switch (m_State) {
            case ButtonState::Hovered: color = m_HoverColor; break;
            case ButtonState::Pressed: color = m_PressedColor; break;
            case ButtonState::Disabled: color = m_DisabledColor; break;
            default: break;
        }

        // Draw background
        Renderer2D::DrawQuad(globalPos + size * 0.5f, size, color);

        // Draw border
        Renderer2D::DrawRect(globalPos + size * 0.5f, size, Vec4(0.5f, 0.5f, 0.5f, 1.0f));

        // Draw text (simplified - actual text rendering would use a font system)
        // For now, we just indicate that text would be rendered here
    }
}
#include "Ashen/UI/Label.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"

namespace ash {
    Label::Label(const String& text)
        : Control("Label")
        , m_Text(text) {
        mouse_filter = false; // Labels don't need mouse events by default
        size = Vec2(100.0f, 20.0f);
    }

    void Label::SetText(const String& text) {
        m_Text = text;
        if (m_AutoResize) {
            size = CalculateTextSize();
        }
    }

    void Label::_Draw() {
        Vec2 textPos = GetTextPosition();

        // Draw text (simplified - actual text rendering would use a font system)
        // For now, we just draw a placeholder rectangle to show where text would be
        // Renderer2D::DrawText(m_Text, textPos, m_FontSize, m_TextColor);
    }

    Vec2 Label::CalculateTextSize() const {
        // Simplified - would actually calculate based on font metrics
        float width = m_Text.length() * m_FontSize * 0.6f;
        float height = m_FontSize * 1.2f;
        return Vec2(width, height);
    }

    Vec2 Label::GetTextPosition() const {
        Vec2 globalPos = GetGlobalPosition();
        Vec2 size = GetSize();
        Vec2 textSize = CalculateTextSize();

        Vec2 pos = globalPos;

        // Horizontal alignment
        switch (m_TextAlign) {
            case TextAlign::Center:
                pos.x += (size.x - textSize.x) * 0.5f;
                break;
            case TextAlign::Right:
                pos.x += size.x - textSize.x;
                break;
            default: break;
        }

        // Vertical alignment
        switch (m_VerticalAlign) {
            case VerticalAlign::Center:
                pos.y += (size.y - textSize.y) * 0.5f;
                break;
            case VerticalAlign::Bottom:
                pos.y += size.y - textSize.y;
                break;
            default: break;
        }

        return pos;
    }
}
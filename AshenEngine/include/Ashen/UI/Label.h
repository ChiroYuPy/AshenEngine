#ifndef ASHEN_UI_LABEL_H
#define ASHEN_UI_LABEL_H

#include "Ashen/Nodes/Control.h"
#include "Ashen/Math/Math.h"

namespace ash {
    enum class TextAlign {
        Left,
        Center,
        Right
    };

    enum class VerticalAlign {
        Top,
        Center,
        Bottom
    };

    class Label : public Control {
    public:
        explicit Label(const String& text = "");

        // Text
        String GetText() const { return m_Text; }
        void SetText(const String& text);

        // Appearance
        Vec4 GetTextColor() const { return m_TextColor; }
        void SetTextColor(const Vec4& color) { m_TextColor = color; }

        float GetFontSize() const { return m_FontSize; }
        void SetFontSize(float size) { m_FontSize = Max(size, 1.0f); }

        // Alignment
        TextAlign GetTextAlign() const { return m_TextAlign; }
        void SetTextAlign(TextAlign align) { m_TextAlign = align; }

        VerticalAlign GetVerticalAlign() const { return m_VerticalAlign; }
        void SetVerticalAlign(VerticalAlign align) { m_VerticalAlign = align; }

        // Word wrap
        bool GetWordWrap() const { return m_WordWrap; }
        void SetWordWrap(bool wrap) { m_WordWrap = wrap; }

        // Auto-resize
        bool GetAutoResize() const { return m_AutoResize; }
        void SetAutoResize(bool autoResize) { m_AutoResize = autoResize; }

        // Rendering
        void _Draw() override;

    private:
        String m_Text;
        Vec4 m_TextColor = Vec4(1.0f);
        float m_FontSize = 16.0f;
        TextAlign m_TextAlign = TextAlign::Left;
        VerticalAlign m_VerticalAlign = VerticalAlign::Top;
        bool m_WordWrap = false;
        bool m_AutoResize = false;

        Vec2 CalculateTextSize() const;
        Vec2 GetTextPosition() const;
    };
}

#endif // ASHEN_UI_LABEL_H
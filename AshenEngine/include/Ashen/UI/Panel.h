#ifndef ASHEN_UI_PANEL_H
#define ASHEN_UI_PANEL_H

#include "Ashen/Nodes/Control.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Panel : public Control {
    public:
        explicit Panel();

        // Appearance
        Vec4 GetBackgroundColor() const { return m_BackgroundColor; }
        void SetBackgroundColor(const Vec4& color) { m_BackgroundColor = color; }

        Vec4 GetBorderColor() const { return m_BorderColor; }
        void SetBorderColor(const Vec4& color) { m_BorderColor = color; }

        float GetBorderWidth() const { return m_BorderWidth; }
        void SetBorderWidth(float width) { m_BorderWidth = Max(width, 0.0f); }

        float GetCornerRadius() const { return m_CornerRadius; }
        void SetCornerRadius(float radius) { m_CornerRadius = Max(radius, 0.0f); }

        // Padding
        Vec4 GetPadding() const { return m_Padding; } // left, top, right, bottom
        void SetPadding(const Vec4& padding) { m_Padding = padding; }
        void SetPadding(float all) { m_Padding = Vec4(all); }

        // Rendering
        void _Draw() override;

    private:
        Vec4 m_BackgroundColor = Vec4(0.15f, 0.15f, 0.15f, 0.9f);
        Vec4 m_BorderColor = Vec4(0.3f, 0.3f, 0.3f, 1.0f);
        float m_BorderWidth = 1.0f;
        float m_CornerRadius = 0.0f;
        Vec4 m_Padding = Vec4(0.0f); // left, top, right, bottom
    };
}

#endif // ASHEN_UI_PANEL_H
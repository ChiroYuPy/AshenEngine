#include "Ashen/UI/Panel.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"

namespace ash {
    Panel::Panel()
        : Control("Panel") {
        size = Vec2(200.0f, 200.0f);
    }

    void Panel::_Draw() {
        Vec2 globalPos = GetGlobalPosition();
        Vec2 size = GetSize();
        Vec2 center = globalPos + size * 0.5f;

        // Draw background
        Renderer2D::DrawQuad(center, size, m_BackgroundColor);

        // Draw border if needed
        if (m_BorderWidth > 0.0f) {
            Renderer2D::DrawRect(center, size, m_BorderColor);
        }
    }
}
#include "Ashen/Graphics/UI/UIWidgets.h"
#include "Ashen/Graphics/UI/UISystem.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Core/Input.h"

namespace ash {

// ==================== Panel ====================
void Panel::DrawSelf() {
    BBox2 rect = GetRect();
    Vec2 pos = GetGlobalPosition();
    Vec2 size = rect.Size();

    // Background with rounded corners
    Renderer2D::DrawQuad(Vec3(pos.x, pos.y, 0.0f), size, m_BgColor);

    // Border
    if (m_BorderWidth > 0.0f) {
        Renderer2D::DrawRect(Vec3(pos.x, pos.y, 0.0f), size, m_BorderColor);
    }
}

// ==================== Label ====================
void Label::SetText(const String& text) {
    m_Text = text;
    m_MinSize = GetMinimumSize();
}

Vec2 Label::GetMinimumSize() const {
    float width = m_Text.length() * m_FontSize * 0.6f;
    float height = m_FontSize * 1.5f;
    return Vec2(width, height);
}

void Label::DrawSelf() {
    BBox2 rect = GetRect();
    Vec2 pos = GetGlobalPosition();
    Vec2 size = rect.Size();
    Vec2 textSize = GetMinimumSize();

    Vec2 textPos = pos;

    // Alignement
    switch (m_Alignment) {
        case 1: // Center
            textPos.x += (size.x - textSize.x) * 0.5f;
            break;
        case 2: // Right
            textPos.x += size.x - textSize.x;
            break;
        default: // Left
            break;
    }

    textPos.y += (size.y - textSize.y) * 0.5f;

    // Draw text background for visibility (placeholder until real text rendering)
    Renderer2D::DrawQuad(Vec3(textPos.x, textPos.y, 0.0f), textSize, m_TextColor * 0.3f);
}

// ==================== Button ====================
void Button::OnInput(Event& event) {
    EventDispatcher dispatcher(event);

    dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
        if (e.GetButton() == MouseButton::Left && IsHovered()) {
            m_Pressed = true;
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent& e) {
        if (e.GetButton() == MouseButton::Left && m_Pressed) {
            m_Pressed = false;
            if (IsHovered() && OnPressed) {
                OnPressed();
            }
            return true;
        }
        return false;
    });
}

void Button::DrawSelf() {
    BBox2 rect = GetRect();
    Vec2 pos = GetGlobalPosition();
    Vec2 size = rect.Size();

    Vec4 color = m_NormalColor;
    if (m_Pressed) color = m_PressedColor;
    else if (IsHovered()) color = m_HoverColor;

    // Button background
    Renderer2D::DrawQuad(Vec3(pos.x, pos.y, 0.0f), size, color);

    // Border
    Renderer2D::DrawRect(Vec3(pos.x, pos.y, 0.0f), size, Vec4(0.5f, 0.5f, 0.55f, 1.0f));

    // Text (simplified)
    if (!m_Text.empty()) {
        float textWidth = m_Text.length() * 14.0f * 0.6f;
        float textHeight = 14.0f;
        Vec2 textPos = pos + (size - Vec2(textWidth, textHeight)) * 0.5f;

        // Draw text placeholder
        Renderer2D::DrawQuad(Vec3(textPos.x, textPos.y, 0.0f),
                            Vec2(textWidth, textHeight),
                            Vec4(1.0f, 1.0f, 1.0f, 0.3f));
    }
}

// ==================== TextureRect ====================
void TextureRect::SetTexture(std::shared_ptr<Texture2D> texture) {
    m_Texture = texture;
    if (texture) {
        SetSize(Vec2(texture->GetWidth(), texture->GetHeight()));
    }
}

void TextureRect::DrawSelf() {
    if (!m_Texture) return;

    Vec2 pos = GetGlobalPosition();
    Vec2 size = GetSize();
    Renderer2D::DrawQuad(Vec3(pos.x, pos.y, 0.0f), size, m_Texture, m_Modulate);
}

// ==================== ProgressBar ====================
void ProgressBar::DrawSelf() {
    Vec2 pos = GetGlobalPosition();
    Vec2 size = GetSize();

    // Background
    Renderer2D::DrawQuad(Vec3(pos.x, pos.y, 0.0f), size, m_BgColor);

    // Fill
    float ratio = m_Value / 100.0f;
    Vec2 fillSize = Vec2(size.x * ratio, size.y);
    Renderer2D::DrawQuad(Vec3(pos.x, pos.y, 0.0f), fillSize, m_FillColor);

    // Border
    Renderer2D::DrawRect(Vec3(pos.x, pos.y, 0.0f), size, Vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Percentage text (placeholder)
    if (m_ShowPercentage) {
        String percentText = std::to_string(static_cast<int>(m_Value)) + "%";
        float textWidth = percentText.length() * 14.0f * 0.6f;
        Vec2 textPos = pos + (size - Vec2(textWidth, 14.0f)) * 0.5f;
        Renderer2D::DrawQuad(Vec3(textPos.x, textPos.y, 0.0f),
                            Vec2(textWidth, 14.0f),
                            Vec4(1.0f, 1.0f, 1.0f, 0.3f));
    }
}

// ==================== VBoxContainer ====================
void VBoxContainer::LayoutChildren() {
    Vec2 offset(0.0f);

    for (const auto& child : GetChildren()) {
        if (auto control = std::dynamic_pointer_cast<Control>(child)) {
            if (!control->IsVisible()) continue;

            Vec2 childSize = control->GetMinimumSize();
            childSize.x = glm::max(childSize.x, GetSize().x);

            control->SetPosition(offset);
            control->SetSize(childSize);

            offset.y += childSize.y + m_Spacing;
        }
    }
}

// ==================== HBoxContainer ====================
void HBoxContainer::LayoutChildren() {
    Vec2 offset(0.0f);

    for (const auto& child : GetChildren()) {
        if (auto control = std::dynamic_pointer_cast<Control>(child)) {
            if (!control->IsVisible()) continue;

            Vec2 childSize = control->GetMinimumSize();
            childSize.y = glm::max(childSize.y, GetSize().y);

            control->SetPosition(offset);
            control->SetSize(childSize);

            offset.x += childSize.x + m_Spacing;
        }
    }
}

// ==================== MarginContainer ====================
void MarginContainer::LayoutChildren() {
    if (GetChildren().empty()) return;

    if (auto child = std::dynamic_pointer_cast<Control>(GetChildren()[0])) {
        Vec2 pos(m_Margin, m_Margin);
        Vec2 size = GetSize() - Vec2(m_Margin * 2.0f);

        child->SetPosition(pos);
        child->SetSize(size);
    }
}

} // namespace ash
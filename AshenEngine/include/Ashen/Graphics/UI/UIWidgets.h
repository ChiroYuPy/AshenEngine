#ifndef ASHEN_UI_WIDGETS_H
#define ASHEN_UI_WIDGETS_H

#include "Ashen/GraphicsAPI/Texture.h"
#include "Ashen/Scene/Control.h"

namespace ash {

// ==================== Panel ====================
class Panel : public Control {
public:
    explicit Panel(const String& name = "Panel") : Control(name) {}

    void SetBgColor(const Vec4& color) { m_BgColor = color; }
    void SetBorderColor(const Vec4& color) { m_BorderColor = color; }
    void SetBorderWidth(float width) { m_BorderWidth = width; }
    void SetCornerRadius(float radius) { m_CornerRadius = radius; }

    void DrawSelf() override;

private:
    Vec4 m_BgColor{0.2f, 0.2f, 0.25f, 1.0f};
    Vec4 m_BorderColor{0.4f, 0.4f, 0.45f, 1.0f};
    float m_BorderWidth{1.0f};
    float m_CornerRadius{4.0f};
};

// ==================== Label ====================
class Label : public Control {
public:
    explicit Label(const String& name = "Label") : Control(name) {
        SetMouseFilter(false);
    }

    void SetText(const String& text);
    String GetText() const { return m_Text; }

    void SetFontSize(float size) { m_FontSize = size; }
    void SetTextColor(const Vec4& color) { m_TextColor = color; }
    void SetAlignment(int align) { m_Alignment = align; } // 0=left, 1=center, 2=right

    Vec2 GetMinimumSize() const override;
    void DrawSelf() override;

private:
    String m_Text;
    float m_FontSize{16.0f};
    Vec4 m_TextColor{1.0f};
    int m_Alignment{0};
};

// ==================== Button ====================
class Button : public Control {
public:
    explicit Button(const String& name = "Button") : Control(name) {}

    void SetText(const String& text) { m_Text = text; }
    String GetText() const { return m_Text; }

    void SetIcon(std::shared_ptr<Texture2D> icon) { m_Icon = icon; }

    bool IsPressed() const { return m_Pressed; }
    bool IsHovered() const { return m_IsHovered; }

    void OnInput(Event& event) override;
    void DrawSelf() override;

    std::function<void()> OnPressed;

private:
    String m_Text;
    std::shared_ptr<Texture2D> m_Icon;
    bool m_Pressed{false};

    Vec4 m_NormalColor{0.3f, 0.3f, 0.35f, 1.0f};
    Vec4 m_HoverColor{0.4f, 0.4f, 0.45f, 1.0f};
    Vec4 m_PressedColor{0.25f, 0.25f, 0.3f, 1.0f};
};

// ==================== TextureRect ====================
class TextureRect : public Control {
public:
    explicit TextureRect(const String& name = "TextureRect") : Control(name) {
        SetMouseFilter(false);
    }

    void SetTexture(std::shared_ptr<Texture2D> texture);
    void SetModulate(const Vec4& color) { m_Modulate = color; }

    void DrawSelf() override;

private:
    std::shared_ptr<Texture2D> m_Texture;
    Vec4 m_Modulate{1.0f};
};

// ==================== ProgressBar ====================
class ProgressBar : public Control {
public:
    explicit ProgressBar(const String& name = "ProgressBar") : Control(name) {
        SetMouseFilter(false);
    }

    void SetValue(float value) { m_Value = glm::clamp(value, 0.0f, 100.0f); }
    float GetValue() const { return m_Value; }

    void SetShowPercentage(bool show) { m_ShowPercentage = show; }

    Vec2 GetMinimumSize() const override { return Vec2(200.0f, 24.0f); }
    void DrawSelf() override;

private:
    float m_Value{0.0f};
    bool m_ShowPercentage{true};
    Vec4 m_BgColor{0.2f, 0.2f, 0.2f, 1.0f};
    Vec4 m_FillColor{0.3f, 0.6f, 0.9f, 1.0f};
};

// ==================== VBoxContainer ====================
class VBoxContainer : public Container {
public:
    explicit VBoxContainer(const String& name = "VBoxContainer") : Container(name) {}

    void SetSpacing(float spacing) { m_Spacing = spacing; }
    void LayoutChildren() override;

private:
    float m_Spacing{4.0f};
};

// ==================== HBoxContainer ====================
class HBoxContainer : public Container {
public:
    explicit HBoxContainer(const String& name = "HBoxContainer") : Container(name) {}

    void SetSpacing(float spacing) { m_Spacing = spacing; }
    void LayoutChildren() override;

private:
    float m_Spacing{4.0f};
};

// ==================== MarginContainer ====================
class MarginContainer : public Container {
public:
    explicit MarginContainer(const String& name = "MarginContainer") : Container(name) {}

    void SetMargin(float margin) { m_Margin = margin; }
    void LayoutChildren() override;

private:
    float m_Margin{8.0f};
};

} // namespace ash

#endif // ASHEN_UI_WIDGETS_H
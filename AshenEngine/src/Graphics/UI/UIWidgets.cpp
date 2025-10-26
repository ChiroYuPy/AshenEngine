#include "Ashen/Graphics/UI/UIWidgets.h"
#include "Ashen/Graphics/UI/UIServer.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Events/MouseEvent.h"
#include <algorithm>
#include <cctype>

namespace ash {
    // Helper function to convert string to uppercase
    static String ToUpperCase(const String &str) {
        String result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](const unsigned char c) { return std::toupper(c); });
        return result;
    }

    // ==================== Container ====================

    Container::Container(const String &name) : Control(name) {
    }

    void Container::Notification(int what) {
        if (m_PendingSort) {
            SortChildren();
            m_PendingSort = false;
        }
    }

    void Container::FitChildInRect(const ControlRef &child, const BBox2 &rect) {
        if (!child) return;
        child->SetPosition(rect.min);
        child->SetSize(rect.Size());
    }

    void Container::QueueSort() {
        m_PendingSort = true;
    }

    // ==================== Panel ====================

    Panel::Panel(const String &name) : Control(name) {
    }

    void Panel::_Draw() {
        const BBox2 rect = GetRect();

        // Background
        if (m_CornerRadius > 0.0f) {
            UIServer::DrawRoundedRect(rect, m_BgColor, m_CornerRadius);
        } else {
            UIServer::DrawRect(rect, m_BgColor);
        }

        // Border
        if (m_BorderWidth > 0.0f) {
            UIServer::DrawRectOutline(rect, m_BorderColor, m_BorderWidth);
        }
    }

    // ==================== Label ====================

    Label::Label(const String &name) : Control(name) {
        SetMouseFilter(false);
    }

    void Label::SetText(const String &text) {
        if (m_Text != text) {
            m_Text = text;
            UpdateMinimumSize();
        }
    }

    Vec2 Label::_GetMinimumSize() const {
        // TODO: Calculate based on actual text size
        const float width = m_Text.length() * m_FontSize * 0.6f;
        const float height = m_FontSize * 1.5f;
        return Vec2(width, height);
    }

    void Label::_Draw() {
        const BBox2 rect = GetRect();
        Vec2 textPos = rect.min;

        // Apply alignment
        const Vec2 textSize = _GetMinimumSize();

        switch (m_Align) {
            case Align::Center:
                textPos.x += (rect.Size().x - textSize.x) * 0.5f;
                break;
            case Align::Right:
                textPos.x += rect.Size().x - textSize.x;
                break;
            case Align::Left:
            default:
                break;
        }

        switch (m_VAlign) {
            case VAlign::Center:
                textPos.y += (rect.Size().y - textSize.y) * 0.5f;
                break;
            case VAlign::Bottom:
                textPos.y += rect.Size().y - textSize.y;
                break;
            case VAlign::Top:
            default:
                break;
        }

        const String displayText = m_Uppercase ? ToUpperCase(m_Text) : m_Text;
        UIServer::DrawText(displayText, textPos, m_FontSize, m_TextColor);
    }

    // ==================== BaseButton ====================

    BaseButton::BaseButton(const String &name) : Control(name) {
        SetFocusMode(FocusMode::Click);
    }

    void BaseButton::SetPressed(const bool pressed) {
        if (m_ToggleMode) {
            m_Pressed = pressed;
            Toggled(m_Pressed);
            if (OnToggled) OnToggled(m_Pressed);
        }
    }

    void BaseButton::_GuiInput(Event &event) {
        if (m_Disabled) return;

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &e) {
            if (e.GetButton() == MouseButton::Left && m_IsHovered) {
                if (m_ActionMode == ActionMode::Press) {
                    Pressed();
                    if (OnPressed) OnPressed();

                    if (m_ToggleMode) {
                        m_Pressed = !m_Pressed;
                        Toggled(m_Pressed);
                        if (OnToggled) OnToggled(m_Pressed);
                    }
                } else {
                    m_Pressed = true;
                    if (OnButtonDown) OnButtonDown();
                }
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent &e) {
            if (e.GetButton() == MouseButton::Left && m_Pressed) {
                if (m_ActionMode == ActionMode::Release && m_IsHovered) {
                    Pressed();
                    if (OnPressed) OnPressed();

                    if (m_ToggleMode) {
                        Toggled(m_Pressed);
                        if (OnToggled) OnToggled(m_Pressed);
                    } else {
                        m_Pressed = false;
                    }
                } else if (!m_ToggleMode) {
                    m_Pressed = false;
                }

                if (OnButtonUp) OnButtonUp();
                return true;
            }
            return false;
        });
    }

    // ==================== Button ====================

    Button::Button(const String &name) : BaseButton(name) {
    }

    Vec2 Button::_GetMinimumSize() const {
        Vec2 minSize(0.0f);

        if (!m_Text.empty()) {
            minSize.x = m_Text.length() * 14.0f * 0.6f + 20.0f;
            minSize.y = 14.0f * 1.5f + 10.0f;
        }

        if (m_Icon) {
            minSize.x += m_Icon->GetWidth() + 5.0f;
            minSize.y = glm::max(minSize.y, static_cast<float>(m_Icon->GetHeight()) + 10.0f);
        }

        return glm::max(minSize, Vec2(80.0f, 30.0f));
    }

    void Button::_Draw() {
        const BBox2 rect = GetRect();

        // Determine color
        Vec4 color = m_NormalColor;
        if (m_Disabled) {
            color = m_DisabledColor;
        } else if (m_Pressed) {
            color = m_PressedColor;
        } else if (m_IsHovered) {
            color = m_HoverColor;
        }

        // Draw background
        if (!m_Flat || m_IsHovered || m_Pressed) {
            UIServer::DrawRoundedRect(rect, color, 4.0f);
        }

        // Draw border
        if (!m_Flat) {
            UIServer::DrawRectOutline(rect, Vec4(0.6f, 0.6f, 0.6f, 1.0f), 1.0f);
        }

        // Draw icon and text
        Vec2 contentPos = rect.Center();
        float totalWidth = 0.0f;

        if (m_Icon) {
            totalWidth += m_Icon->GetWidth();
        }
        if (!m_Text.empty()) {
            if (m_Icon) totalWidth += 5.0f;
            totalWidth += m_Text.length() * 14.0f * 0.6f;
        }

        contentPos.x -= totalWidth * 0.5f;

        if (m_Icon) {
            const BBox2 iconRect = BBox2::FromCenterAndSize(
                Vec2(contentPos.x + m_Icon->GetWidth() * 0.5f, rect.Center().y),
                Vec2(m_Icon->GetWidth(), m_Icon->GetHeight())
            );
            UIServer::DrawTexture(m_Icon, iconRect);
            contentPos.x += m_Icon->GetWidth() + 5.0f;
        }

        if (!m_Text.empty()) {
            UIServer::DrawText(m_Text, Vec2(contentPos.x, contentPos.y - 7.0f), 14.0f, Vec4(1.0f));
        }
    }

    // ==================== TextureRect ====================

    TextureRect::TextureRect(const String &name) : Control(name) {
        SetMouseFilter(false);
    }

    void TextureRect::SetTexture(const Ref<Texture2D> &texture) {
        m_Texture = texture;
        if (texture) {
            SetSize(Vec2(texture->GetWidth(), texture->GetHeight()));
        }
    }

    void TextureRect::_Draw() {
        if (!m_Texture) return;

        BBox2 rect = GetRect();
        BBox2 drawRect = rect;

        // Apply stretch mode
        switch (m_StretchMode) {
            case StretchMode::Scale:
                // Use full rect
                break;

            case StretchMode::Keep:
                drawRect = BBox2::FromCenterAndSize(
                    rect.Center(),
                    Vec2(m_Texture->GetWidth(), m_Texture->GetHeight())
                );
                break;

            case StretchMode::KeepCentered:
                drawRect = BBox2::FromCenterAndSize(
                    rect.Center(),
                    Vec2(m_Texture->GetWidth(), m_Texture->GetHeight())
                );
                break;

            case StretchMode::KeepAspect:
            case StretchMode::KeepAspectCentered: {
                float texAspect = static_cast<float>(m_Texture->GetWidth()) / m_Texture->GetHeight();
                float rectAspect = rect.Size().x / rect.Size().y;

                Vec2 drawSize;
                if (texAspect > rectAspect) {
                    drawSize.x = rect.Size().x;
                    drawSize.y = rect.Size().x / texAspect;
                } else {
                    drawSize.y = rect.Size().y;
                    drawSize.x = rect.Size().y * texAspect;
                }

                drawRect = BBox2::FromCenterAndSize(rect.Center(), drawSize);
                break;
            }

            case StretchMode::KeepAspectCovered: {
                float texAspect = static_cast<float>(m_Texture->GetWidth()) / m_Texture->GetHeight();
                float rectAspect = rect.Size().x / rect.Size().y;

                Vec2 drawSize;
                if (texAspect < rectAspect) {
                    drawSize.x = rect.Size().x;
                    drawSize.y = rect.Size().x / texAspect;
                } else {
                    drawSize.y = rect.Size().y;
                    drawSize.x = rect.Size().y * texAspect;
                }

                drawRect = BBox2::FromCenterAndSize(rect.Center(), drawSize);
                break;
            }

            case StretchMode::Tile:
                // TODO: Implement tiling
                break;
        }

        UIServer::DrawTexture(m_Texture, drawRect, m_Modulate);
    }

    // ==================== Range ====================

    Range::Range(const String &name) : Control(name) {
    }

    void Range::SetValue(const float value) {
        float newValue = value;

        if (!m_AllowGreater) {
            newValue = glm::min(newValue, m_Max);
        }
        if (!m_AllowLesser) {
            newValue = glm::max(newValue, m_Min);
        }

        if (m_Step > 0.0f) {
            newValue = glm::round(newValue / m_Step) * m_Step;
        }

        if (m_Rounded) {
            newValue = glm::round(newValue);
        }

        if (m_Value != newValue) {
            m_Value = newValue;
            ValueChanged();
        }
    }

    void Range::SetRatio(const float ratio) {
        SetValue(m_Min + ratio * (m_Max - m_Min));
    }

    float Range::GetRatio() const {
        if (m_Max == m_Min) return 0.0f;
        return (m_Value - m_Min) / (m_Max - m_Min);
    }

    void Range::ValueChanged() {
        if (OnValueChanged) OnValueChanged(m_Value);
        if (OnChanged) OnChanged();
    }

    // ==================== Slider ====================

    Slider::Slider(const String &name) : Range(name) {
    }

    void Slider::_GuiInput(Event &event) {
        if (!m_Editable) return;

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &e) {
            if (e.GetButton() == MouseButton::Left && m_IsHovered) {
                m_Dragging = true;
                GrabFocus();

                const BBox2 rect = GetRect();
                const float ratio = (Input::GetMousePosition().x - rect.min.x) / rect.Size().x;
                SetRatio(glm::clamp(ratio, 0.0f, 1.0f));
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent &e) {
            if (e.GetButton() == MouseButton::Left && m_Dragging) {
                m_Dragging = false;
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent &e) {
            if (m_Dragging) {
                const BBox2 rect = GetRect();
                const float ratio = (e.GetX() - rect.min.x) / rect.Size().x;
                SetRatio(glm::clamp(ratio, 0.0f, 1.0f));
                return true;
            }
            return false;
        });
    }

    void Slider::_Draw() {
        BBox2 rect = GetRect();

        // Background track
        UIServer::DrawRect(rect, Vec4(0.2f, 0.2f, 0.2f, 1.0f));

        // Fill
        float ratio = GetRatio();
        BBox2 fillRect = rect;
        fillRect.max.x = fillRect.min.x + rect.Size().x * ratio;
        UIServer::DrawRect(fillRect, Vec4(0.4f, 0.6f, 0.8f, 1.0f));

        // Handle
        float handleX = rect.min.x + rect.Size().x * ratio;
        Vec2 handlePos(handleX, rect.Center().y);
        Vec2 handleSize(10.0f, rect.Size().y + 4.0f);

        BBox2 handleRect = BBox2::FromCenterAndSize(handlePos, handleSize);
        Vec4 handleColor = m_Dragging ? Vec4(0.9f, 0.9f, 0.9f, 1.0f) : Vec4(0.7f, 0.7f, 0.7f, 1.0f);
        UIServer::DrawRect(handleRect, handleColor);

        // Ticks
        if (m_TickCount > 0) {
            for (int i = 0; i <= m_TickCount; i++) {
                float t = static_cast<float>(i) / m_TickCount;
                float x = rect.min.x + rect.Size().x * t;

                Vec2 p0(x, rect.min.y);
                Vec2 p1(x, rect.max.y);
                Renderer2D::DrawLine(p0, p1, Vec4(0.5f, 0.5f, 0.5f, 1.0f));
            }
        }
    }

    // ==================== HSlider / VSlider ====================

    HSlider::HSlider(const String &name) : Slider(name) {
    }

    VSlider::VSlider(const String &name) : Slider(name) {
    }

    // ==================== ProgressBar ====================

    ProgressBar::ProgressBar(const String &name) : Range(name) {
        SetMouseFilter(false);
        SetMin(0.0f);
        SetMax(100.0f);
        SetValue(0.0f);
    }

    Vec2 ProgressBar::_GetMinimumSize() const {
        return Vec2(200.0f, 24.0f);
    }

    void ProgressBar::_Draw() {
        const BBox2 rect = GetRect();

        // Background
        UIServer::DrawRect(rect, m_BgColor);

        // Progress fill
        const float ratio = GetRatio();
        BBox2 fillRect = rect;
        fillRect.max.x = fillRect.min.x + rect.Size().x * ratio;
        UIServer::DrawRect(fillRect, m_FgColor);

        // Border
        UIServer::DrawRectOutline(rect, Vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f);

        // Percentage text
        if (m_PercentVisible) {
            const String percentText = std::to_string(static_cast<int>(ratio * 100.0f)) + "%";
            const Vec2 textPos = rect.Center() - Vec2(15.0f, 7.0f);
            UIServer::DrawText(percentText, textPos, 14.0f, Vec4(1.0f));
        }
    }

    // ==================== Separator ====================

    Separator::Separator(const String &name) : Control(name) {
        SetMouseFilter(false);
    }

    void Separator::_Draw() {
        const BBox2 rect = GetRect();
        UIServer::DrawRect(rect, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
    }

    HSeparator::HSeparator(const String &name) : Separator(name) {
    }

    VSeparator::VSeparator(const String &name) : Separator(name) {
    }
} // namespace ash
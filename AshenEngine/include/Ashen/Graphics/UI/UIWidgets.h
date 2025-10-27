#ifndef ASHEN_UI_WIDGETS_H
#define ASHEN_UI_WIDGETS_H

#include "Ashen/Scene/Control.h"
#include "Ashen/GraphicsAPI/Texture.h"

namespace ash {
    // ==================== Container ====================
    class Container : public Control {
    public:
        explicit Container(const String &name = "Container");

        void Notification(int what) override;

        void _GuiInput(Event &event) override {
        }

        void FitChildInRect(const ControlRef &child, const BBox2 &rect);

        void QueueSort();

    protected:
        virtual void SortChildren() {
        }

        bool m_PendingSort = false;
    };

    // ==================== Panel ====================
    class Panel : public Control {
    public:
        explicit Panel(const String &name = "Panel");

        void SetBgColor(const Vec4 &color) { m_BgColor = color; }
        Vec4 GetBgColor() const { return m_BgColor; }

        void SetBorderColor(const Vec4 &color) { m_BorderColor = color; }
        void SetBorderWidth(const float width) { m_BorderWidth = width; }
        void SetCornerRadius(const float radius) { m_CornerRadius = radius; }

        void _Draw() override;

    private:
        Vec4 m_BgColor{0.2f, 0.2f, 0.2f, 0.9f};
        Vec4 m_BorderColor{0.4f, 0.4f, 0.4f, 1.0f};
        float m_BorderWidth = 0.0f;
        float m_CornerRadius = 0.0f;
    };

    // ==================== Label ====================
    class Label : public Control {
    public:
        enum class Align { Left, Center, Right };

        enum class VAlign { Top, Center, Bottom };

        explicit Label(const String &name = "Label");

        void SetText(const String &text);

        String GetText() const { return m_Text; }

        void SetAlign(const Align align) { m_Align = align; }
        void SetVAlign(const VAlign valign) { m_VAlign = valign; }

        void SetFontSize(const float size) { m_FontSize = size; }
        float GetFontSize() const { return m_FontSize; }

        void SetTextColor(const Vec4 &color) { m_TextColor = color; }
        Vec4 GetTextColor() const { return m_TextColor; }

        void SetAutowrap(const bool enabled) { m_Autowrap = enabled; }
        void SetUppercase(const bool enabled) { m_Uppercase = enabled; }

        Vec2 _GetMinimumSize() const override;

        void _Draw() override;

    private:
        String m_Text;
        Align m_Align = Align::Left;
        VAlign m_VAlign = VAlign::Top;
        float m_FontSize = 14.0f;
        Vec4 m_TextColor{1.0f};
        bool m_Autowrap = false;
        bool m_Uppercase = false;
    };

    // ==================== Button ====================
    class BaseButton : public Control {
    public:
        enum class ActionMode { Press, Release };

        explicit BaseButton(const String &name = "BaseButton");

        void SetToggleMode(const bool enabled) { m_ToggleMode = enabled; }
        bool GetToggleMode() const { return m_ToggleMode; }

        void SetPressed(bool pressed);

        bool IsPressed() const { return m_Pressed; }
        bool IsHovered() const { return m_IsHovered; }

        void SetActionMode(const ActionMode mode) { m_ActionMode = mode; }
        void SetDisabled(const bool disabled) { m_Disabled = disabled; }
        bool IsDisabled() const { return m_Disabled; }

        void _GuiInput(Event &event) override;

        Function<void()> OnPressed;
        Function<void()> OnButtonDown;
        Function<void()> OnButtonUp;
        Function<void(bool)> OnToggled;

    protected:
        virtual void Pressed() {
        }

        virtual void Toggled(bool pressed) {
        }

        bool m_ToggleMode = false;
        bool m_Pressed = false;
        bool m_Disabled = false;
        ActionMode m_ActionMode = ActionMode::Release;
    };

    class Button : public BaseButton {
    public:
        explicit Button(const String &name = "Button");

        void SetText(const String &text) { m_Text = text; }
        String GetText() const { return m_Text; }

        void SetIcon(const Ref<Texture2D> &icon) { m_Icon = icon; }
        void SetFlat(const bool flat) { m_Flat = flat; }

        Vec2 _GetMinimumSize() const override;

        void _Draw() override;

    private:
        String m_Text;
        Ref<Texture2D> m_Icon;
        bool m_Flat = false;

        Vec4 m_NormalColor{0.3f, 0.3f, 0.3f, 1.0f};
        Vec4 m_HoverColor{0.4f, 0.4f, 0.4f, 1.0f};
        Vec4 m_PressedColor{0.2f, 0.2f, 0.2f, 1.0f};
        Vec4 m_DisabledColor{0.2f, 0.2f, 0.2f, 0.5f};
    };

    // ==================== TextureRect ====================
    class TextureRect : public Control {
    public:
        enum class StretchMode { Scale, Tile, Keep, KeepCentered, KeepAspect, KeepAspectCentered, KeepAspectCovered };

        explicit TextureRect(const String &name = "TextureRect");

        void SetTexture(const Ref<Texture2D> &texture);

        Ref<Texture2D> GetTexture() const { return m_Texture; }

        void SetStretchMode(const StretchMode mode) { m_StretchMode = mode; }
        void SetFlipH(const bool flip) { m_FlipH = flip; }
        void SetFlipV(const bool flip) { m_FlipV = flip; }
        void SetModulate(const Vec4 &color) { m_Modulate = color; }

        void _Draw() override;

    private:
        Ref<Texture2D> m_Texture;
        StretchMode m_StretchMode = StretchMode::Scale;
        Vec4 m_Modulate{1.0f};
        bool m_FlipH = false;
        bool m_FlipV = false;
    };

    // ==================== Range ====================
    class Range : public Control {
    public:
        explicit Range(const String &name = "Range");

        void SetValue(float value);

        float GetValue() const { return m_Value; }

        void SetMin(const float min) {
            m_Min = min;
            ValueChanged();
        }

        void SetMax(const float max) {
            m_Max = max;
            ValueChanged();
        }

        void SetStep(const float step) { m_Step = step; }
        void SetPage(const float page) { m_Page = page; }

        float GetMin() const { return m_Min; }
        float GetMax() const { return m_Max; }
        float GetStep() const { return m_Step; }
        float GetPage() const { return m_Page; }

        void SetRatio(float ratio);

        float GetRatio() const;

        void SetExpEdit(const bool enabled) { m_ExpEdit = enabled; }
        void SetRounded(const bool rounded) { m_Rounded = rounded; }
        void SetAllowGreater(const bool allow) { m_AllowGreater = allow; }
        void SetAllowLesser(const bool allow) { m_AllowLesser = allow; }

        Function<void(float)> OnValueChanged;
        Function<void()> OnChanged;

    protected:
        virtual void ValueChanged();

        float m_Value = 0.0f;
        float m_Min = 0.0f;
        float m_Max = 100.0f;
        float m_Step = 1.0f;
        float m_Page = 0.0f;

        bool m_ExpEdit = false;
        bool m_Rounded = false;
        bool m_AllowGreater = false;
        bool m_AllowLesser = false;
    };

    // ==================== Slider ====================
    class Slider : public Range {
    public:
        explicit Slider(const String &name = "Slider");

        void SetEditable(const bool editable) { m_Editable = editable; }
        void SetScrollable(const bool scrollable) { m_Scrollable = scrollable; }
        void SetTickCount(const int count) { m_TickCount = count; }
        void SetTicksOnBorders(const bool enabled) { m_TicksOnBorders = enabled; }

        void _GuiInput(Event &event) override;

        void _Draw() override;

    protected:
        bool m_Editable = true;
        bool m_Scrollable = true;
        bool m_Dragging = false;
        int m_TickCount = 0;
        bool m_TicksOnBorders = false;
    };

    class HSlider : public Slider {
    public:
        explicit HSlider(const String &name = "HSlider");

        Vec2 _GetMinimumSize() const override { return {200.0f, 20.0f}; }
    };

    class VSlider : public Slider {
    public:
        explicit VSlider(const String &name = "VSlider");

        Vec2 _GetMinimumSize() const override { return {20.0f, 200.0f}; }
    };

    // ==================== ProgressBar ====================
    class ProgressBar : public Range {
    public:
        explicit ProgressBar(const String &name = "ProgressBar");

        void SetPercentVisible(const bool visible) { m_PercentVisible = visible; }

        Vec2 _GetMinimumSize() const override;

        void _Draw() override;

    private:
        bool m_PercentVisible = true;
        Vec4 m_BgColor{0.2f, 0.2f, 0.2f, 1.0f};
        Vec4 m_FgColor{0.4f, 0.7f, 1.0f, 1.0f};
    };

    // ==================== Separator ====================
    class Separator : public Control {
    public:
        explicit Separator(const String &name = "Separator");

        void _Draw() override;
    };

    class HSeparator : public Separator {
    public:
        explicit HSeparator(const String &name = "HSeparator");

        Vec2 _GetMinimumSize() const override { return {0.0f, 4.0f}; }
    };

    class VSeparator : public Separator {
    public:
        explicit VSeparator(const String &name = "VSeparator");

        Vec2 _GetMinimumSize() const override { return {4.0f, 0.0f}; }
    };
} // namespace ash

#endif // ASHEN_UI_WIDGETS_H
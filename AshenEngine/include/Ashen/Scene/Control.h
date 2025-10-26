#ifndef ASHEN_CONTROL_H
#define ASHEN_CONTROL_H

#include "Ashen/Scene/Node.h"
#include "Ashen/Math/BBox.h"

namespace ash {
    // Anchor presets for UI positioning
    enum class LayoutPreset {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight,
        LeftWide, RightWide, TopWide, BottomWide,
        VCenterWide, HCenterWide,
        FullRect
    };

    // Size flags for containers
    enum class SizeFlags {
        Fill = 1 << 0,
        Expand = 1 << 1,
        ShrinkCenter = 1 << 2,
        ShrinkEnd = 1 << 3
    };

    inline SizeFlags operator|(SizeFlags a, SizeFlags b) {
        return static_cast<SizeFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    // Mouse cursor shapes
    enum class CursorShape {
        Arrow, IBeam, PointingHand, Cross, Wait,
        Busy, Drag, CanDrop, Forbidden,
        VSize, HSize, BDiagSize, FDiagSize,
        Move, VSplit, HSplit, Help
    };

    // Focus mode
    enum class FocusMode {
        None, Click, All
    };

    // Base class for all UI controls
    class Control : public Node2D {
    public:
        using ControlRef = Ref<Control>;

        explicit Control(const String &name = "Control");

        ~Control() override = default;

        // Rect and positioning
        void SetPosition(const Vec2 &position) override;

        void SetSize(const Vec2 &size);

        void SetRect(const Vec2 &position, const Vec2 &size);

        void SetGlobalPosition(const Vec2 &position);

        Vec2 GetSize() const { return m_Size; }

        Vec2 GetMinimumSize() const;

        Vec2 GetCombinedMinimumSize() const;

        BBox2 GetRect() const;

        BBox2 GetGlobalRect() const;

        // Anchors and margins
        void SetAnchor(float left, float top, float right, float bottom, bool keepMargins = true);

        void SetAnchorPreset(LayoutPreset preset, bool keepMargins = true);

        void SetAnchorsAndMarginsPreset(LayoutPreset preset);

        Vec4 GetAnchors() const { return m_Anchors; }
        Vec4 GetMargins() const { return m_Margins; }

        void SetMargins(const Vec4 &margins);

        // Size constraints
        void SetCustomMinimumSize(const Vec2 &size) {
            m_CustomMinSize = size;
            SizeChanged();
        }

        Vec2 GetCustomMinimumSize() const { return m_CustomMinSize; }

        // Layout
        void SetHSizeFlags(const SizeFlags flags) {
            m_HSizeFlags = flags;
            SizeChanged();
        }

        void SetVSizeFlags(const SizeFlags flags) {
            m_VSizeFlags = flags;
            SizeChanged();
        }

        SizeFlags GetHSizeFlags() const { return m_HSizeFlags; }
        SizeFlags GetVSizeFlags() const { return m_VSizeFlags; }

        void SetStretchRatio(const float ratio) { m_StretchRatio = ratio; }
        float GetStretchRatio() const { return m_StretchRatio; }

        // Input and focus
        void SetFocusMode(const FocusMode mode) { m_FocusMode = mode; }
        FocusMode GetFocusMode() const { return m_FocusMode; }

        bool HasFocus() const;

        void GrabFocus();

        void ReleaseFocus();

        void SetMouseFilter(const bool enabled) { m_MouseFilterEnabled = enabled; }
        bool GetMouseFilter() const { return m_MouseFilterEnabled; }
        void SetMouseDefaultCursorShape(const CursorShape shape) { m_DefaultCursorShape = shape; }
        CursorShape GetMouseDefaultCursorShape() const { return m_DefaultCursorShape; }

        // Clipping and visibility
        void SetClipContents(const bool enabled) { m_ClipContents = enabled; }
        bool GetClipContents() const { return m_ClipContents; }

        // Virtual methods for derived classes
        virtual Vec2 _GetMinimumSize() const { return m_CustomMinSize; }

        virtual void _GuiInput(Event &event) {
        }

        virtual void _Draw() {
        }

        // Rendering
        void OnProcess(float delta) override;

        void OnInput(Event &event) override;

        // Signals (callbacks)
        Function<void()> OnResized;
        Function<void()> OnMinimumSizeChanged;
        Function<void()> OnFocusEntered;
        Function<void()> OnFocusExited;
        Function<void()> OnMouseEntered;
        Function<void()> OnMouseExited;
        Function<void(Event &)> OnGuiInput;

    protected:
        void SizeChanged() const;

        void UpdateMinimumSize() const;

        virtual void Notification(int what);

        bool HasPoint(const Vec2 &point) const;

        Vec2 m_Size{100.0f, 100.0f};
        Vec2 m_CustomMinSize{0.0f};
        Vec4 m_Anchors{0.0f}; // left, top, right, bottom (0-1)
        Vec4 m_Margins{0.0f}; // left, top, right, bottom

        SizeFlags m_HSizeFlags = SizeFlags::Fill;
        SizeFlags m_VSizeFlags = SizeFlags::Fill;
        float m_StretchRatio = 1.0f;

        FocusMode m_FocusMode = FocusMode::None;
        bool m_MouseFilterEnabled = true;
        bool m_ClipContents = false;
        CursorShape m_DefaultCursorShape = CursorShape::Arrow;

        bool m_IsHovered = false;
        bool m_HasFocus = false;

        friend class UIServer;
        friend class SceneTree;
    };
} // namespace ash

#endif // ASHEN_CONTROL_H
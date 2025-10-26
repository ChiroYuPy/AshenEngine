#ifndef ASHEN_UI_CONTAINERS_H
#define ASHEN_UI_CONTAINERS_H

#include "UIWidgets.h"

namespace ash {
    // ==================== BoxContainer ====================
    class BoxContainer : public Container {
    public:
        enum class AlignMode { Begin, Center, End };

        explicit BoxContainer(const String &name = "BoxContainer", bool vertical = false);

        void AddSpacer(bool begin);

        void SetAlignment(const AlignMode mode) {
            m_Alignment = mode;
            QueueSort();
        }

        AlignMode GetAlignment() const { return m_Alignment; }

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        virtual bool IsVertical() const { return m_Vertical; }

        bool m_Vertical;
        AlignMode m_Alignment = AlignMode::Begin;
    };

    class HBoxContainer : public BoxContainer {
    public:
        explicit HBoxContainer(const String &name = "HBoxContainer")
            : BoxContainer(name, false) {
        }
    };

    class VBoxContainer : public BoxContainer {
    public:
        explicit VBoxContainer(const String &name = "VBoxContainer")
            : BoxContainer(name, true) {
        }
    };

    // ==================== GridContainer ====================
    class GridContainer : public Container {
    public:
        explicit GridContainer(const String &name = "GridContainer");

        void SetColumns(int columns);

        int GetColumns() const { return m_Columns; }

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        int m_Columns = 1;
    };

    // ==================== SplitContainer ====================
    class SplitContainer : public Container {
    public:
        enum class DraggerVisibility { Visible, Hidden, HiddenCollapsed };

        explicit SplitContainer(const String &name = "SplitContainer", bool vertical = false);

        void SetSplitOffset(int offset);

        int GetSplitOffset() const { return m_SplitOffset; }

        void SetCollapsed(const bool collapsed) { m_Collapsed = collapsed; }
        bool IsCollapsed() const { return m_Collapsed; }

        void SetDraggerVisibility(const DraggerVisibility vis) { m_DraggerVisibility = vis; }

        void _GuiInput(Event &event) override;

        void _Draw() override;

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        virtual bool IsVertical() const { return m_Vertical; }

        bool m_Vertical;
        int m_SplitOffset = 0;
        int m_MinimumSplitOffset = 0;
        bool m_Collapsed = false;
        bool m_Dragging = false;
        DraggerVisibility m_DraggerVisibility = DraggerVisibility::Visible;
    };

    class HSplitContainer : public SplitContainer {
    public:
        explicit HSplitContainer(const String &name = "HSplitContainer")
            : SplitContainer(name, false) {
        }
    };

    class VSplitContainer : public SplitContainer {
    public:
        explicit VSplitContainer(const String &name = "VSplitContainer")
            : SplitContainer(name, true) {
        }
    };

    // ==================== ScrollContainer ====================
    class ScrollContainer : public Container {
    public:
        enum class ScrollMode { Disabled, Auto, AlwaysShow, AlwaysHide };

        explicit ScrollContainer(const String &name = "ScrollContainer");

        void SetHScrollMode(const ScrollMode mode) { m_HScrollMode = mode; }
        void SetVScrollMode(const ScrollMode mode) { m_VScrollMode = mode; }

        void SetHScroll(int value);

        void SetVScroll(int value);

        int GetHScroll() const { return m_HScroll; }
        int GetVScroll() const { return m_VScroll; }

        void SetFollowFocus(const bool enabled) { m_FollowFocus = enabled; }

        void _GuiInput(Event &event) override;

        void _Draw() override;

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        void UpdateScrollbars();

        ScrollMode m_HScrollMode = ScrollMode::Auto;
        ScrollMode m_VScrollMode = ScrollMode::Auto;
        int m_HScroll = 0;
        int m_VScroll = 0;
        bool m_FollowFocus = false;

        Vec2 m_ScrollbarSize{12.0f};
        bool m_HScrollbarVisible = false;
        bool m_VScrollbarVisible = false;
    };

    // ==================== PanelContainer ====================
    class PanelContainer : public Container {
    public:
        explicit PanelContainer(const String &name = "PanelContainer");

        void _Draw() override;

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        Vec4 m_StyleBgColor{0.2f, 0.2f, 0.2f, 0.9f};
        Vec4 m_StyleBorderColor{0.4f, 0.4f, 0.4f, 1.0f};
        float m_StyleBorderWidth = 1.0f;
        Vec4 m_StylePadding{8.0f};
    };

    // ==================== MarginContainer ====================
    class MarginContainer : public Container {
    public:
        explicit MarginContainer(const String &name = "MarginContainer");

        void SetMarginLeft(const int margin) {
            m_MarginLeft = margin;
            QueueSort();
        }

        void SetMarginTop(const int margin) {
            m_MarginTop = margin;
            QueueSort();
        }

        void SetMarginRight(const int margin) {
            m_MarginRight = margin;
            QueueSort();
        }

        void SetMarginBottom(const int margin) {
            m_MarginBottom = margin;
            QueueSort();
        }

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        int m_MarginLeft = 0;
        int m_MarginTop = 0;
        int m_MarginRight = 0;
        int m_MarginBottom = 0;
    };

    // ==================== CenterContainer ====================
    class CenterContainer : public Container {
    public:
        explicit CenterContainer(const String &name = "CenterContainer");

        void SetUseTopLeft(const bool enabled) {
            m_UseTopLeft = enabled;
            QueueSort();
        }

        bool IsUsingTopLeft() const { return m_UseTopLeft; }

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        bool m_UseTopLeft = false;
    };

    // ==================== AspectRatioContainer ====================
    class AspectRatioContainer : public Container {
    public:
        enum class StretchMode { WidthControlsHeight, HeightControlsWidth, Fit, Cover };

        enum class AlignMode { Begin, Center, End };

        explicit AspectRatioContainer(const String &name = "AspectRatioContainer");

        void SetRatio(const float ratio) {
            m_Ratio = ratio;
            QueueSort();
        }

        float GetRatio() const { return m_Ratio; }

        void SetStretchMode(const StretchMode mode) {
            m_StretchMode = mode;
            QueueSort();
        }

        void SetAlignmentHorizontal(const AlignMode mode) {
            m_AlignH = mode;
            QueueSort();
        }

        void SetAlignmentVertical(const AlignMode mode) {
            m_AlignV = mode;
            QueueSort();
        }

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        float m_Ratio = 1.0f;
        StretchMode m_StretchMode = StretchMode::Fit;
        AlignMode m_AlignH = AlignMode::Center;
        AlignMode m_AlignV = AlignMode::Center;
    };

    // ==================== TabContainer ====================
    class TabContainer : public Container {
    public:
        enum class TabAlign { Left, Center, Right };

        explicit TabContainer(const String &name = "TabContainer");

        int GetTabCount() const;

        void SetCurrentTab(int tab);

        int GetCurrentTab() const { return m_CurrentTab; }

        void SetTabAlign(const TabAlign align) { m_TabAlign = align; }
        void SetTabsVisible(const bool visible) { m_TabsVisible = visible; }
        void SetAllTabsInFront(const bool enabled) { m_AllTabsInFront = enabled; }

        String GetTabTitle(int tab) const;

        void SetTabTitle(int tab, const String &title);

        void SetTabIcon(int tab, const Ref<Texture2D> &icon);

        void SetTabDisabled(int tab, bool disabled);

        void SetTabHidden(int tab, bool hidden);

        ControlRef GetTabControl(int tab) const;

        ControlRef GetCurrentTabControl() const;

        void _GuiInput(Event &event) override;

        void _Draw() override;

        Vec2 _GetMinimumSize() const override;

        Function<void(int)> OnTabChanged;
        Function<void(int)> OnTabSelected;

    protected:
        void SortChildren() override;

        int GetTabFromPosition(const Vec2 &pos) const;

        int m_CurrentTab = 0;
        TabAlign m_TabAlign = TabAlign::Center;
        bool m_TabsVisible = true;
        bool m_AllTabsInFront = false;
        float m_TabHeight = 32.0f;

        struct TabData {
            String title;
            Ref<Texture2D> icon;
            bool disabled = false;
            bool hidden = false;
        };

        Vector<TabData> m_Tabs;
    };

    // ==================== FlowContainer ====================
    class FlowContainer : public Container {
    public:
        explicit FlowContainer(const String &name = "FlowContainer", bool vertical = false);

        Vec2 _GetMinimumSize() const override;

    protected:
        void SortChildren() override;

        virtual bool IsVertical() const { return m_Vertical; }

        bool m_Vertical;
    };

    class HFlowContainer : public FlowContainer {
    public:
        explicit HFlowContainer(const String &name = "HFlowContainer")
            : FlowContainer(name, false) {
        }
    };

    class VFlowContainer : public FlowContainer {
    public:
        explicit VFlowContainer(const String &name = "VFlowContainer")
            : FlowContainer(name, true) {
        }
    };
} // namespace ash

#endif // ASHEN_UI_CONTAINERS_H
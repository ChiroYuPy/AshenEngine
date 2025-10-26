#include "Ashen/Graphics/UI/UIContainers.h"

#include "Ashen/Core/Input.h"
#include "Ashen/Events/MouseEvent.h"
#include "Ashen/Graphics/UI/UIServer.h"

namespace ash {
    // ==================== BoxContainer ====================

    BoxContainer::BoxContainer(const String &name, const bool vertical)
        : Container(name), m_Vertical(vertical) {
    }

    void BoxContainer::AddSpacer(const bool begin) {
        const auto spacer = MakeRef<Control>("Spacer");
        spacer->SetHSizeFlags(SizeFlags::Expand | SizeFlags::Fill);
        spacer->SetVSizeFlags(SizeFlags::Expand | SizeFlags::Fill);

        if (begin) {
            // Add at beginning - would need to implement insertion
        } else {
            AddChild(spacer);
        }
    }

    Vec2 BoxContainer::_GetMinimumSize() const {
        Vec2 minSize(0.0f);
        int count = 0;

        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const Vec2 childMinSize = control->GetCombinedMinimumSize();

            if (m_Vertical) {
                minSize.x = glm::max(minSize.x, childMinSize.x);
                minSize.y += childMinSize.y;
            } else {
                minSize.x += childMinSize.x;
                minSize.y = glm::max(minSize.y, childMinSize.y);
            }
            count++;
        }

        // Add spacing between children
        if (count > 1) {
            const float spacing = 4.0f;
            if (m_Vertical) {
                minSize.y += spacing * (count - 1);
            } else {
                minSize.x += spacing * (count - 1);
            }
        }

        return minSize;
    }

    void BoxContainer::SortChildren() {
        const Vec2 size = GetSize();
        Vec2 offset(0.0f);
        const float spacing = 4.0f;

        // Calculate total size and expansion
        float totalMinSize = 0.0f;
        float totalExpand = 0.0f;
        int visibleCount = 0;

        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const Vec2 childMinSize = control->GetCombinedMinimumSize();
            if (m_Vertical) {
                totalMinSize += childMinSize.y;
                if (static_cast<int>(control->GetVSizeFlags()) & static_cast<int>(SizeFlags::Expand)) {
                    totalExpand += control->GetStretchRatio();
                }
            } else {
                totalMinSize += childMinSize.x;
                if (static_cast<int>(control->GetHSizeFlags()) & static_cast<int>(SizeFlags::Expand)) {
                    totalExpand += control->GetStretchRatio();
                }
            }
            visibleCount++;
        }

        if (visibleCount > 1) {
            totalMinSize += spacing * (visibleCount - 1);
        }

        // Calculate extra space
        float extraSpace = (m_Vertical ? size.y : size.x) - totalMinSize;
        if (extraSpace < 0.0f) extraSpace = 0.0f;

        // Apply alignment if no expansion
        if (totalExpand == 0.0f) {
            switch (m_Alignment) {
                case AlignMode::Center:
                    if (m_Vertical) offset.y = extraSpace * 0.5f;
                    else offset.x = extraSpace * 0.5f;
                    break;
                case AlignMode::End:
                    if (m_Vertical) offset.y = extraSpace;
                    else offset.x = extraSpace;
                    break;
                case AlignMode::Begin:
                default:
                    break;
            }
        }

        // Layout children
        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const Vec2 childMinSize = control->GetCombinedMinimumSize();
            Vec2 childSize = childMinSize;

            // Calculate child size with expansion
            if (m_Vertical) {
                childSize.x = size.x;
                if (totalExpand > 0.0f &&
                    (static_cast<int>(control->GetVSizeFlags()) & static_cast<int>(SizeFlags::Expand))) {
                    childSize.y += extraSpace * (control->GetStretchRatio() / totalExpand);
                }
            } else {
                childSize.y = size.y;
                if (totalExpand > 0.0f &&
                    (static_cast<int>(control->GetHSizeFlags()) & static_cast<int>(SizeFlags::Expand))) {
                    childSize.x += extraSpace * (control->GetStretchRatio() / totalExpand);
                }
            }

            // Apply Fill flag
            if (m_Vertical) {
                if (!(static_cast<int>(control->GetHSizeFlags()) & static_cast<int>(SizeFlags::Fill))) {
                    childSize.x = childMinSize.x;
                }
            } else {
                if (!(static_cast<int>(control->GetVSizeFlags()) & static_cast<int>(SizeFlags::Fill))) {
                    childSize.y = childMinSize.y;
                }
            }

            control->SetPosition(offset);
            control->SetSize(childSize);

            if (m_Vertical) {
                offset.y += childSize.y + spacing;
            } else {
                offset.x += childSize.x + spacing;
            }
        }
    }

    // ==================== GridContainer ====================

    GridContainer::GridContainer(const String &name) : Container(name) {
    }

    void GridContainer::SetColumns(int columns) {
        if (columns < 1) columns = 1;
        m_Columns = columns;
        QueueSort();
    }

    Vec2 GridContainer::_GetMinimumSize() const {
        if (m_Columns <= 0) return Vec2(0.0f);

        Vec2 cellSize(0.0f);
        int visibleCount = 0;

        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const Vec2 childMinSize = control->GetCombinedMinimumSize();
            cellSize.x = glm::max(cellSize.x, childMinSize.x);
            cellSize.y = glm::max(cellSize.y, childMinSize.y);
            visibleCount++;
        }

        const int rows = (visibleCount + m_Columns - 1) / m_Columns;
        const float spacing = 4.0f;

        Vec2 minSize;
        minSize.x = cellSize.x * m_Columns + spacing * (m_Columns - 1);
        minSize.y = cellSize.y * rows + spacing * (rows - 1);

        return minSize;
    }

    void GridContainer::SortChildren() {
        const Vec2 size = GetSize();
        const float spacing = 4.0f;

        // Calculate cell size
        int visibleCount = 0;
        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (control && control->IsVisible()) visibleCount++;
        }

        if (visibleCount == 0) return;

        const int rows = (visibleCount + m_Columns - 1) / m_Columns;
        Vec2 cellSize;
        cellSize.x = (size.x - spacing * (m_Columns - 1)) / m_Columns;
        cellSize.y = (size.y - spacing * (rows - 1)) / rows;

        // Layout children
        int index = 0;
        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const int row = index / m_Columns;
            const int col = index % m_Columns;

            Vec2 pos;
            pos.x = col * (cellSize.x + spacing);
            pos.y = row * (cellSize.y + spacing);

            control->SetPosition(pos);
            control->SetSize(cellSize);

            index++;
        }
    }

    // ==================== SplitContainer ====================

    SplitContainer::SplitContainer(const String &name, const bool vertical)
        : Container(name), m_Vertical(vertical) {
    }

    void SplitContainer::SetSplitOffset(const int offset) {
        m_SplitOffset = offset;
        QueueSort();
    }

    Vec2 SplitContainer::_GetMinimumSize() const {
        Vec2 minSize(0.0f);

        const auto children = GetChildren();
        if (children.size() >= 2) {
            const auto first = std::dynamic_pointer_cast<Control>(children[0]);
            const auto second = std::dynamic_pointer_cast<Control>(children[1]);

            if (first && second) {
                const Vec2 firstMin = first->GetCombinedMinimumSize();
                const Vec2 secondMin = second->GetCombinedMinimumSize();

                if (m_Vertical) {
                    minSize.x = glm::max(firstMin.x, secondMin.x);
                    minSize.y = firstMin.y + secondMin.y + 8.0f; // Dragger size
                } else {
                    minSize.x = firstMin.x + secondMin.x + 8.0f;
                    minSize.y = glm::max(firstMin.y, secondMin.y);
                }
            }
        }

        return minSize;
    }

    void SplitContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.size() < 2) return;

        const auto first = std::dynamic_pointer_cast<Control>(children[0]);
        const auto second = std::dynamic_pointer_cast<Control>(children[1]);
        if (!first || !second) return;

        const Vec2 size = GetSize();
        const float draggerSize = 8.0f;

        int splitPos = m_SplitOffset;
        if (splitPos < 0) {
            splitPos = (m_Vertical ? size.y : size.x) + splitPos;
        }

        splitPos = glm::clamp(splitPos, m_MinimumSplitOffset,
                              static_cast<int>((m_Vertical ? size.y : size.x) - draggerSize - m_MinimumSplitOffset));

        if (m_Collapsed) {
            first->SetVisible(false);
            second->SetPosition(Vec2(0.0f));
            second->SetSize(size);
        } else {
            first->SetVisible(true);

            if (m_Vertical) {
                first->SetPosition(Vec2(0.0f));
                first->SetSize(Vec2(size.x, splitPos));

                second->SetPosition(Vec2(0.0f, splitPos + draggerSize));
                second->SetSize(Vec2(size.x, size.y - splitPos - draggerSize));
            } else {
                first->SetPosition(Vec2(0.0f));
                first->SetSize(Vec2(splitPos, size.y));

                second->SetPosition(Vec2(splitPos + draggerSize, 0.0f));
                second->SetSize(Vec2(size.x - splitPos - draggerSize, size.y));
            }
        }
    }

    void SplitContainer::_GuiInput(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &e) {
            if (e.GetButton() == MouseButton::Left) {
                const Vec2 mousePos = Input::GetMousePosition();
                const BBox2 rect = GetGlobalRect();

                const float draggerPos = m_Vertical ? rect.min.y + m_SplitOffset : rect.min.x + m_SplitOffset;
                const float draggerSize = 8.0f;

                const bool inDragger = m_Vertical
                                           ? (mousePos.y >= draggerPos && mousePos.y <= draggerPos + draggerSize)
                                           : (mousePos.x >= draggerPos && mousePos.x <= draggerPos + draggerSize);

                if (inDragger) {
                    m_Dragging = true;
                    return true;
                }
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
                const BBox2 rect = GetGlobalRect();
                const int newOffset = m_Vertical
                                          ? static_cast<int>(e.GetY() - rect.min.y)
                                          : static_cast<int>(e.GetX() - rect.min.x);

                SetSplitOffset(newOffset);
                return true;
            }
            return false;
        });
    }

    void SplitContainer::_Draw() {
        if (m_DraggerVisibility == DraggerVisibility::Hidden ||
            m_DraggerVisibility == DraggerVisibility::HiddenCollapsed) {
            return;
        }

        const BBox2 rect = GetRect();
        const float draggerSize = 8.0f;

        BBox2 draggerRect;
        if (m_Vertical) {
            draggerRect = BBox2(
                Vec2(rect.min.x, rect.min.y + m_SplitOffset),
                Vec2(rect.max.x, rect.min.y + m_SplitOffset + draggerSize)
            );
        } else {
            draggerRect = BBox2(
                Vec2(rect.min.x + m_SplitOffset, rect.min.y),
                Vec2(rect.min.x + m_SplitOffset + draggerSize, rect.max.y)
            );
        }

        const Vec4 draggerColor = m_Dragging ? Vec4(0.5f, 0.5f, 0.5f, 1.0f) : Vec4(0.3f, 0.3f, 0.3f, 1.0f);
        UIServer::DrawRect(draggerRect, draggerColor);
    }

    // ==================== ScrollContainer ====================

    ScrollContainer::ScrollContainer(const String &name) : Container(name) {
        SetClipContents(true);
    }

    void ScrollContainer::SetHScroll(const int value) {
        m_HScroll = value;
        QueueSort();
    }

    void ScrollContainer::SetVScroll(const int value) {
        m_VScroll = value;
        QueueSort();
    }

    Vec2 ScrollContainer::_GetMinimumSize() const {
        return Vec2(100.0f, 100.0f);
    }

    void ScrollContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.empty()) return;

        const auto content = std::dynamic_pointer_cast<Control>(children[0]);
        if (!content) return;

        const Vec2 size = GetSize();
        const Vec2 contentSize = content->GetCombinedMinimumSize();

        // Determine if scrollbars are needed
        m_HScrollbarVisible = (m_HScrollMode == ScrollMode::AlwaysShow) ||
                              (m_HScrollMode == ScrollMode::Auto && contentSize.x > size.x);
        m_VScrollbarVisible = (m_VScrollMode == ScrollMode::AlwaysShow) ||
                              (m_VScrollMode == ScrollMode::Auto && contentSize.y > size.y);

        // Adjust available size for scrollbars
        Vec2 availableSize = size;
        if (m_VScrollbarVisible) availableSize.x -= m_ScrollbarSize.x;
        if (m_HScrollbarVisible) availableSize.y -= m_ScrollbarSize.y;

        // Position content
        const Vec2 contentPos(-m_HScroll, -m_VScroll);
        content->SetPosition(contentPos);
        content->SetSize(glm::max(contentSize, availableSize));
    }

    void ScrollContainer::_GuiInput(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent &e) {
            if (m_IsHovered) {
                if (m_VScrollbarVisible) {
                    m_VScroll -= static_cast<int>(e.GetYOffset() * 20.0f);
                    m_VScroll = glm::max(0, m_VScroll);
                    QueueSort();
                    return true;
                }
            }
            return false;
        });
    }

    void ScrollContainer::_Draw() {
        // Draw scrollbars
        if (m_VScrollbarVisible) {
            const Vec2 size = GetSize();
            const BBox2 scrollbarRect(
                Vec2(size.x - m_ScrollbarSize.x, 0.0f),
                Vec2(size.x, size.y)
            );
            UIServer::DrawRect(scrollbarRect, Vec4(0.2f, 0.2f, 0.2f, 0.8f));
        }

        if (m_HScrollbarVisible) {
            const Vec2 size = GetSize();
            const BBox2 scrollbarRect(
                Vec2(0.0f, size.y - m_ScrollbarSize.y),
                Vec2(size.x, size.y)
            );
            UIServer::DrawRect(scrollbarRect, Vec4(0.2f, 0.2f, 0.2f, 0.8f));
        }
    }

    void ScrollContainer::UpdateScrollbars() {
        // TODO: Calculate scrollbar thumb positions
    }

    // ==================== PanelContainer ====================

    PanelContainer::PanelContainer(const String &name) : Container(name) {
    }

    Vec2 PanelContainer::_GetMinimumSize() const {
        Vec2 minSize = m_StylePadding.x + m_StylePadding.z + Vec2(0.0f, m_StylePadding.y + m_StylePadding.w);

        const auto children = GetChildren();
        if (!children.empty()) {
            const auto child = std::dynamic_pointer_cast<Control>(children[0]);
            if (child) {
                minSize += child->GetCombinedMinimumSize();
            }
        }

        return minSize;
    }

    void PanelContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.empty()) return;

        const auto child = std::dynamic_pointer_cast<Control>(children[0]);
        if (!child) return;

        const Vec2 size = GetSize();
        const Vec2 contentPos(m_StylePadding.x, m_StylePadding.y);
        const Vec2 contentSize(
            size.x - m_StylePadding.x - m_StylePadding.z,
            size.y - m_StylePadding.y - m_StylePadding.w
        );

        child->SetPosition(contentPos);
        child->SetSize(contentSize);
    }

    void PanelContainer::_Draw() {
        const BBox2 rect = GetRect();

        // Background
        UIServer::DrawRect(rect, m_StyleBgColor);

        // Border
        if (m_StyleBorderWidth > 0.0f) {
            UIServer::DrawRectOutline(rect, m_StyleBorderColor, m_StyleBorderWidth);
        }
    }

    // ==================== MarginContainer ====================

    MarginContainer::MarginContainer(const String &name) : Container(name) {
    }

    Vec2 MarginContainer::_GetMinimumSize() const {
        Vec2 minSize(m_MarginLeft + m_MarginRight, m_MarginTop + m_MarginBottom);

        const auto children = GetChildren();
        if (!children.empty()) {
            const auto child = std::dynamic_pointer_cast<Control>(children[0]);
            if (child) {
                minSize += child->GetCombinedMinimumSize();
            }
        }

        return minSize;
    }

    void MarginContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.empty()) return;

        const auto child = std::dynamic_pointer_cast<Control>(children[0]);
        if (!child) return;

        const Vec2 size = GetSize();
        const Vec2 contentPos(m_MarginLeft, m_MarginTop);
        const Vec2 contentSize(
            size.x - m_MarginLeft - m_MarginRight,
            size.y - m_MarginTop - m_MarginBottom
        );

        child->SetPosition(contentPos);
        child->SetSize(contentSize);
    }

    // ==================== CenterContainer ====================

    CenterContainer::CenterContainer(const String &name) : Container(name) {
    }

    Vec2 CenterContainer::_GetMinimumSize() const {
        const auto children = GetChildren();
        if (!children.empty()) {
            const auto child = std::dynamic_pointer_cast<Control>(children[0]);
            if (child) {
                return child->GetCombinedMinimumSize();
            }
        }
        return Vec2(0.0f);
    }

    void CenterContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.empty()) return;

        const auto child = std::dynamic_pointer_cast<Control>(children[0]);
        if (!child) return;

        const Vec2 size = GetSize();
        const Vec2 childSize = child->GetCombinedMinimumSize();

        Vec2 pos;
        if (m_UseTopLeft) {
            pos = Vec2(0.0f);
        } else {
            pos = (size - childSize) * 0.5f;
        }

        child->SetPosition(pos);
        child->SetSize(childSize);
    }

    // ==================== AspectRatioContainer ====================

    AspectRatioContainer::AspectRatioContainer(const String &name) : Container(name) {
    }

    Vec2 AspectRatioContainer::_GetMinimumSize() const {
        return Vec2(100.0f * m_Ratio, 100.0f);
    }

    void AspectRatioContainer::SortChildren() {
        const auto children = GetChildren();
        if (children.empty()) return;

        const auto child = std::dynamic_pointer_cast<Control>(children[0]);
        if (!child) return;

        const Vec2 size = GetSize();
        Vec2 childSize = size;

        // Calculate size based on stretch mode
        const float targetAspect = m_Ratio;
        const float currentAspect = size.x / size.y;

        switch (m_StretchMode) {
            case StretchMode::WidthControlsHeight:
                childSize.y = childSize.x / targetAspect;
                break;
            case StretchMode::HeightControlsWidth:
                childSize.x = childSize.y * targetAspect;
                break;
            case StretchMode::Fit:
                if (currentAspect > targetAspect) {
                    childSize.x = childSize.y * targetAspect;
                } else {
                    childSize.y = childSize.x / targetAspect;
                }
                break;
            case StretchMode::Cover:
                if (currentAspect < targetAspect) {
                    childSize.x = childSize.y * targetAspect;
                } else {
                    childSize.y = childSize.x / targetAspect;
                }
                break;
        }

        // Calculate position based on alignment
        Vec2 pos(0.0f);

        switch (m_AlignH) {
            case AlignMode::Center:
                pos.x = (size.x - childSize.x) * 0.5f;
                break;
            case AlignMode::End:
                pos.x = size.x - childSize.x;
                break;
            case AlignMode::Begin:
            default:
                break;
        }

        switch (m_AlignV) {
            case AlignMode::Center:
                pos.y = (size.y - childSize.y) * 0.5f;
                break;
            case AlignMode::End:
                pos.y = size.y - childSize.y;
                break;
            case AlignMode::Begin:
            default:
                break;
        }

        child->SetPosition(pos);
        child->SetSize(childSize);
    }

    // ==================== TabContainer ====================

    TabContainer::TabContainer(const String &name) : Container(name) {
    }

    int TabContainer::GetTabCount() const {
        int count = 0;
        for (const auto &child: GetChildren()) {
            if (std::dynamic_pointer_cast<Control>(child)) {
                count++;
            }
        }
        return count;
    }

    void TabContainer::SetCurrentTab(const int tab) {
        if (tab >= 0 && tab < GetTabCount()) {
            m_CurrentTab = tab;
            QueueSort();
            if (OnTabChanged) OnTabChanged(tab);
        }
    }

    String TabContainer::GetTabTitle(const int tab) const {
        if (tab >= 0 && tab < static_cast<int>(m_Tabs.size())) {
            return m_Tabs[tab].title;
        }
        return "";
    }

    void TabContainer::SetTabTitle(const int tab, const String &title) {
        while (static_cast<int>(m_Tabs.size()) <= tab) {
            m_Tabs.push_back(TabData());
        }
        m_Tabs[tab].title = title;
    }

    void TabContainer::SetTabIcon(const int tab, const Ref<Texture2D> &icon) {
        while (static_cast<int>(m_Tabs.size()) <= tab) {
            m_Tabs.push_back(TabData());
        }
        m_Tabs[tab].icon = icon;
    }

    void TabContainer::SetTabDisabled(const int tab, const bool disabled) {
        if (tab >= 0 && tab < static_cast<int>(m_Tabs.size())) {
            m_Tabs[tab].disabled = disabled;
        }
    }

    void TabContainer::SetTabHidden(const int tab, const bool hidden) {
        if (tab >= 0 && tab < static_cast<int>(m_Tabs.size())) {
            m_Tabs[tab].hidden = hidden;
        }
    }

    Control::ControlRef TabContainer::GetTabControl(const int tab) const {
        const auto children = GetChildren();
        if (tab >= 0 && tab < static_cast<int>(children.size())) {
            return std::dynamic_pointer_cast<Control>(children[tab]);
        }
        return nullptr;
    }

    Control::ControlRef TabContainer::GetCurrentTabControl() const {
        return GetTabControl(m_CurrentTab);
    }

    Vec2 TabContainer::_GetMinimumSize() const {
        Vec2 minSize(0.0f, m_TabHeight);

        for (const auto &child: GetChildren()) {
            auto control = std::dynamic_pointer_cast<Control>(child);
            if (control) {
                const Vec2 childMin = control->GetCombinedMinimumSize();
                minSize.x = glm::max(minSize.x, childMin.x);
                minSize.y = glm::max(minSize.y, childMin.y + m_TabHeight);
            }
        }

        return minSize;
    }

    void TabContainer::SortChildren() {
        const Vec2 size = GetSize();
        const Vec2 contentPos(0.0f, m_TabsVisible ? m_TabHeight : 0.0f);
        const Vec2 contentSize(size.x, size.y - contentPos.y);

        int index = 0;
        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control) continue;

            control->SetVisible(index == m_CurrentTab);
            control->SetPosition(contentPos);
            control->SetSize(contentSize);

            index++;
        }
    }

    void TabContainer::_GuiInput(Event &event) {
        if (!m_TabsVisible) return;

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &e) {
            if (e.GetButton() == MouseButton::Left) {
                const int tab = GetTabFromPosition(Input::GetMousePosition());
                if (tab >= 0 && tab < GetTabCount()) {
                    if (tab < static_cast<int>(m_Tabs.size()) && !m_Tabs[tab].disabled) {
                        SetCurrentTab(tab);
                        if (OnTabSelected) OnTabSelected(tab);
                        return true;
                    }
                }
            }
            return false;
        });
    }

    void TabContainer::_Draw() {
        if (!m_TabsVisible) return;

        BBox2 rect = GetRect();
        float tabWidth = rect.Size().x / glm::max(1, GetTabCount());

        // Draw tab bar background
        BBox2 tabBarRect(rect.min, Vec2(rect.max.x, rect.min.y + m_TabHeight));
        UIServer::DrawRect(tabBarRect, Vec4(0.15f, 0.15f, 0.18f, 1.0f));

        // Draw tabs
        int index = 0;
        for (const auto &child: GetChildren()) {
            if (!std::dynamic_pointer_cast<Control>(child)) continue;

            bool isHidden = index < static_cast<int>(m_Tabs.size()) && m_Tabs[index].hidden;
            if (isHidden) {
                index++;
                continue;
            }

            float tabX = rect.min.x + index * tabWidth;
            BBox2 tabRect(
                Vec2(tabX, rect.min.y),
                Vec2(tabX + tabWidth, rect.min.y + m_TabHeight)
            );

            Vec4 tabColor = (index == m_CurrentTab) ? Vec4(0.25f, 0.25f, 0.3f, 1.0f) : Vec4(0.18f, 0.18f, 0.22f, 1.0f);
            bool isDisabled = index < static_cast<int>(m_Tabs.size()) && m_Tabs[index].disabled;
            if (isDisabled) {
                tabColor *= 0.5f;
            }

            UIServer::DrawRect(tabRect, tabColor);
            UIServer::DrawRectOutline(tabRect, Vec4(0.3f, 0.3f, 0.35f, 1.0f), 1.0f);

            // Draw tab text
            String title = GetTabTitle(index);
            if (title.empty()) {
                title = "Tab " + std::to_string(index + 1);
            }

            Vec2 textPos(tabRect.Center().x - title.length() * 4.0f, tabRect.min.y + 8.0f);
            UIServer::DrawText(title, textPos, 14.0f, Vec4(1.0f));

            index++;
        }
    }

    int TabContainer::GetTabFromPosition(const Vec2 &pos) const {
        const BBox2 rect = GetGlobalRect();
        if (pos.y < rect.min.y || pos.y > rect.min.y + m_TabHeight) {
            return -1;
        }

        const float tabWidth = rect.Size().x / glm::max(1, GetTabCount());
        const int tab = static_cast<int>((pos.x - rect.min.x) / tabWidth);

        return glm::clamp(tab, 0, GetTabCount() - 1);
    }

    // ==================== FlowContainer ====================

    FlowContainer::FlowContainer(const String &name, const bool vertical)
        : Container(name), m_Vertical(vertical) {
    }

    Vec2 FlowContainer::_GetMinimumSize() const {
        Vec2 minSize(0.0f);

        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            const Vec2 childMinSize = control->GetCombinedMinimumSize();
            minSize.x = glm::max(minSize.x, childMinSize.x);
            minSize.y = glm::max(minSize.y, childMinSize.y);
        }

        return minSize;
    }

    void FlowContainer::SortChildren() {
        const Vec2 size = GetSize();
        Vec2 offset(0.0f);
        const float spacing = 4.0f;
        float lineHeight = 0.0f;

        for (const auto &child: GetChildren()) {
            const auto control = std::dynamic_pointer_cast<Control>(child);
            if (!control || !control->IsVisible()) continue;

            Vec2 childSize = control->GetCombinedMinimumSize();

            // Check if we need to wrap
            bool needsWrap = false;
            if (m_Vertical) {
                if (offset.y + childSize.y > size.y && offset.y > 0.0f) {
                    needsWrap = true;
                }
            } else {
                if (offset.x + childSize.x > size.x && offset.x > 0.0f) {
                    needsWrap = true;
                }
            }

            if (needsWrap) {
                if (m_Vertical) {
                    offset.x += lineHeight + spacing;
                    offset.y = 0.0f;
                    lineHeight = 0.0f;
                } else {
                    offset.y += lineHeight + spacing;
                    offset.x = 0.0f;
                    lineHeight = 0.0f;
                }
            }

            control->SetPosition(offset);
            control->SetSize(childSize);

            if (m_Vertical) {
                offset.y += childSize.y + spacing;
                lineHeight = glm::max(lineHeight, childSize.x);
            } else {
                offset.x += childSize.x + spacing;
                lineHeight = glm::max(lineHeight, childSize.y);
            }
        }
    }
} // namespace ash
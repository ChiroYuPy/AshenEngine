#include "Ashen/Scene/Control.h"
#include "Ashen/Scene/SceneTree.h"
#include "Ashen/Graphics/UI/UIServer.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Control::Control(const String &name) : Node2D(name) {
        m_Anchors = Vec4(0.0f); // Top-left by default
        m_Margins = Vec4(0.0f);
    }

    void Control::SetPosition(const Vec2 &position) {
        Node2D::SetPosition(position);
        SizeChanged();
    }

    void Control::SetSize(const Vec2 &size) {
        if (m_Size != size) {
            m_Size = size;
            SizeChanged();
        }
    }

    void Control::SetRect(const Vec2 &position, const Vec2 &size) {
        SetPosition(position);
        SetSize(size);
    }

    void Control::SetGlobalPosition(const Vec2 &position) {
        if (const auto parent = std::dynamic_pointer_cast < Control > (GetParent()))
            SetPosition(position - parent->GetGlobalPosition());
        else
            SetPosition(position);
    }

    Vec2 Control::GetMinimumSize() const {
        return _GetMinimumSize();
    }

    Vec2 Control::GetCombinedMinimumSize() const {
        Vec2 minSize = GetMinimumSize();

        // Add margins
        minSize.x += std::abs(m_Margins.x) + std::abs(m_Margins.z);
        minSize.y += std::abs(m_Margins.y) + std::abs(m_Margins.w);

        return minSize;
    }

    BBox2 Control::GetRect() const {
        return {m_Position, m_Position + m_Size};
    }

    BBox2 Control::GetGlobalRect() const {
        const Vec2 globalPos = GetGlobalPosition();
        return {globalPos, globalPos + m_Size};
    }

    void Control::SetAnchor(const float left, const float top, const float right, const float bottom,
                            const bool keepMargins) {
        if (!keepMargins) {
            m_Anchors = Vec4(left, top, right, bottom);
            return;
        }

        // Calculate new margins to keep position
        if (const auto parent = std::dynamic_pointer_cast < Control > (GetParent())) {
            const Vec2 parentSize = parent->GetSize();
            const Vec2 pos = GetPosition();
            const Vec2 size = GetSize();

            m_Margins.x = pos.x - left * parentSize.x;
            m_Margins.y = pos.y - top * parentSize.y;
            m_Margins.z = (left + size.x / parentSize.x) * parentSize.x - (pos.x + size.x);
            m_Margins.w = (top + size.y / parentSize.y) * parentSize.y - (pos.y + size.y);
        }

        m_Anchors = Vec4(left, top, right, bottom);
        SizeChanged();
    }

    void Control::SetAnchorPreset(const LayoutPreset preset, const bool keepMargins) {
        switch (preset) {
            case LayoutPreset::TopLeft:
                SetAnchor(0.0f, 0.0f, 0.0f, 0.0f, keepMargins);
                break;
            case LayoutPreset::TopCenter:
                SetAnchor(0.5f, 0.0f, 0.5f, 0.0f, keepMargins);
                break;
            case LayoutPreset::TopRight:
                SetAnchor(1.0f, 0.0f, 1.0f, 0.0f, keepMargins);
                break;
            case LayoutPreset::CenterLeft:
                SetAnchor(0.0f, 0.5f, 0.0f, 0.5f, keepMargins);
                break;
            case LayoutPreset::Center:
                SetAnchor(0.5f, 0.5f, 0.5f, 0.5f, keepMargins);
                break;
            case LayoutPreset::CenterRight:
                SetAnchor(1.0f, 0.5f, 1.0f, 0.5f, keepMargins);
                break;
            case LayoutPreset::BottomLeft:
                SetAnchor(0.0f, 1.0f, 0.0f, 1.0f, keepMargins);
                break;
            case LayoutPreset::BottomCenter:
                SetAnchor(0.5f, 1.0f, 0.5f, 1.0f, keepMargins);
                break;
            case LayoutPreset::BottomRight:
                SetAnchor(1.0f, 1.0f, 1.0f, 1.0f, keepMargins);
                break;
            case LayoutPreset::LeftWide:
                SetAnchor(0.0f, 0.0f, 0.0f, 1.0f, keepMargins);
                break;
            case LayoutPreset::RightWide:
                SetAnchor(1.0f, 0.0f, 1.0f, 1.0f, keepMargins);
                break;
            case LayoutPreset::TopWide:
                SetAnchor(0.0f, 0.0f, 1.0f, 0.0f, keepMargins);
                break;
            case LayoutPreset::BottomWide:
                SetAnchor(0.0f, 1.0f, 1.0f, 1.0f, keepMargins);
                break;
            case LayoutPreset::VCenterWide:
                SetAnchor(0.0f, 0.5f, 1.0f, 0.5f, keepMargins);
                break;
            case LayoutPreset::HCenterWide:
                SetAnchor(0.5f, 0.0f, 0.5f, 1.0f, keepMargins);
                break;
            case LayoutPreset::FullRect:
                SetAnchor(0.0f, 0.0f, 1.0f, 1.0f, keepMargins);
                break;
        }
    }

    void Control::SetAnchorsAndMarginsPreset(const LayoutPreset preset) {
        SetAnchorPreset(preset, false);

        // Set default margins based on preset
        switch (preset) {
            case LayoutPreset::FullRect:
                m_Margins = Vec4(0.0f);
                break;
            case LayoutPreset::TopLeft:
            case LayoutPreset::TopCenter:
            case LayoutPreset::TopRight:
            case LayoutPreset::CenterLeft:
            case LayoutPreset::Center:
            case LayoutPreset::CenterRight:
            case LayoutPreset::BottomLeft:
            case LayoutPreset::BottomCenter:
            case LayoutPreset::BottomRight:
                // Keep size, adjust position
                break;
            default:
                m_Margins = Vec4(10.0f);
                break;
        }

        SizeChanged();
    }

    void Control::SetMargins(const Vec4 &margins) {
        m_Margins = margins;
        SizeChanged();
    }

    bool Control::HasFocus() const {
        return m_HasFocus;
    }

    void Control::GrabFocus() {
        if (m_FocusMode != FocusMode::None) {
            UIServer::SetFocusedControl(this);
            m_HasFocus = true;
            if (OnFocusEntered) OnFocusEntered();
        }
    }

    void Control::ReleaseFocus() {
        if (m_HasFocus) {
            m_HasFocus = false;
            if (OnFocusExited) OnFocusExited();
        }
    }

    void Control::OnProcess(float delta) {
        // Update rect based on anchors and parent size
        if (const auto parent = std::dynamic_pointer_cast < Control > (GetParent())) {
            const Vec2 parentSize = parent->GetSize();

            Vec2 anchorPos;
            anchorPos.x = m_Anchors.x * parentSize.x + m_Margins.x;
            anchorPos.y = m_Anchors.y * parentSize.y + m_Margins.y;

            Vec2 anchorEnd;
            anchorEnd.x = m_Anchors.z * parentSize.x - m_Margins.z;
            anchorEnd.y = m_Anchors.w * parentSize.y - m_Margins.w;

            if (m_Anchors.x != m_Anchors.z || m_Anchors.y != m_Anchors.w) {
                SetPosition(anchorPos);
                SetSize(anchorEnd - anchorPos);
            } else
                SetPosition(anchorPos);
        }

        // Call virtual draw
        _Draw();
    }

    void Control::OnInput(Event &event) {
        if (m_MouseFilterEnabled) {
            _GuiInput(event);
            if (OnGuiInput) OnGuiInput(event);
        }
    }

    void Control::SizeChanged() const {
        if (OnResized) OnResized();
        UpdateMinimumSize();
    }

    void Control::UpdateMinimumSize() const {
        if (OnMinimumSizeChanged) OnMinimumSizeChanged();
    }

    void Control::Notification(int what) {
        //TODO Handle notifications
    }

    bool Control::HasPoint(const Vec2 &point) const {
        const BBox2 rect = GetGlobalRect();
        return rect.Contains(point);
    }
} // namespace ash
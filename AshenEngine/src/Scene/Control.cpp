#include "Ashen/Scene/Control.h"
#include "Ashen/Graphics/UI/UISystem.h"

namespace ash {

void Control::SetSize(const Vec2& size) {
    m_Size = glm::max(size, m_MinSize);
}

BBox2 Control::GetRect() const {
    return BBox2(m_Position, m_Position + m_Size);
}

BBox2 Control::GetGlobalRect() const {
    Vec2 globalPos = GetGlobalPosition();
    return BBox2(globalPos, globalPos + m_Size);
}

void Control::SetAnchor(Anchor anchor) {
    m_Anchor = anchor;

    // Mettre à jour la position en fonction du parent
    if (auto parent = std::dynamic_pointer_cast<Control>(GetParent())) {
        Vec2 parentSize = parent->GetSize();
        Vec2 pos = m_Position;

        switch (anchor) {
            case Anchor::TopLeft: pos = Vec2(m_Margins.x, m_Margins.y); break;
            case Anchor::TopCenter: pos = Vec2(parentSize.x * 0.5f - m_Size.x * 0.5f, m_Margins.y); break;
            case Anchor::TopRight: pos = Vec2(parentSize.x - m_Size.x - m_Margins.z, m_Margins.y); break;
            case Anchor::CenterLeft: pos = Vec2(m_Margins.x, parentSize.y * 0.5f - m_Size.y * 0.5f); break;
            case Anchor::Center: pos = (parentSize - m_Size) * 0.5f; break;
            case Anchor::CenterRight: pos = Vec2(parentSize.x - m_Size.x - m_Margins.z, parentSize.y * 0.5f - m_Size.y * 0.5f); break;
            case Anchor::BottomLeft: pos = Vec2(m_Margins.x, parentSize.y - m_Size.y - m_Margins.w); break;
            case Anchor::BottomCenter: pos = Vec2(parentSize.x * 0.5f - m_Size.x * 0.5f, parentSize.y - m_Size.y - m_Margins.w); break;
            case Anchor::BottomRight: pos = parentSize - m_Size - Vec2(m_Margins.z, m_Margins.w); break;
            case Anchor::Full:
                pos = Vec2(m_Margins.x, m_Margins.y);
                SetSize(Vec2(parentSize.x - m_Margins.x - m_Margins.z, parentSize.y - m_Margins.y - m_Margins.w));
                break;
        }

        SetPosition(pos);
    }
}

void Control::SetMargins(float left, float top, float right, float bottom) {
    m_Margins = Vec4(left, top, right, bottom);
    SetAnchor(m_Anchor); // Recalculer la position
}

void Control::GrabFocus() {
    UISystem::SetFocusedControl(this);
    m_HasFocus = true;
    if (OnFocusEntered) OnFocusEntered();
}

void Control::ReleaseFocus() {
    if (m_HasFocus) {
        m_HasFocus = false;
        if (OnFocusExited) OnFocusExited();
    }
}

void Control::OnDraw() {
    if (!IsVisibleInTree()) return;
    DrawSelf();
}

// Container
void Container::OnProcess(float delta) {
    LayoutChildren();
    Node::OnProcess(delta);
}

} // namespace ash
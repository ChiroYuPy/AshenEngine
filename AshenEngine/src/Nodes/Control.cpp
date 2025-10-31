#include "Ashen/Nodes/Control.h"

namespace ash {
    Control::Control(String name) : CanvasItem(MovePtr(name)) {
    }

    BBox2 Control::GetGlobalRect() const {
        const Vec2 global_pos = GetGlobalPosition();
        return BBox2(global_pos, global_pos + size);
    }

    BBox2 Control::GetRect() const {
        return BBox2(position, position + size);
    }

    Vec2 Control::GetGlobalPosition() const {
        if (m_Parent)
            if (const auto parent_ctrl = dynamic_cast<const Control *>(m_Parent))
                return position + parent_ctrl->GetGlobalPosition();

        return position;
    }

    void Control::SetGlobalPosition(const Vec2 &pos) {
        if (m_Parent) {
            if (const auto parent_ctrl = dynamic_cast<const Control *>(m_Parent)) {
                position = pos - parent_ctrl->GetGlobalPosition();
                return;
            }
        }
        position = pos;
    }

    Vec2 Control::GetSize() const { return size; }

    void Control::SetSize(const Vec2 &s) {
        Vec2 new_size = s;
        new_size.x = Max(new_size.x, GetMinimumSize().x);
        new_size.y = Max(new_size.y, GetMinimumSize().y);

        if (ApproxEqual(size, new_size)) return;

        size = new_size;
        OnResized();
    }

    Vec2 Control::GetMinimumSize() const {
        return Vec2(
            Max(min_size.x, custom_minimum_size.x),
            Max(min_size.y, custom_minimum_size.y)
        );
    }

    Vec2 Control::GetCenter() const {
        return position + size * 0.5f;
    }

    bool Control::HasPoint(const Vec2 &point) const {
        return GetGlobalRect().Contains(point);
    }

    void Control::GrabFocus() {
        if (!focusable) return;
        focused = true;
        OnFocusEnter();
    }

    void Control::ReleaseFocus() {
        if (focused) {
            focused = false;
            OnFocusExit();
        }
    }

    bool Control::HasFocus() const { return focused; }

    void Control::OnMouseEnter() {
    }

    void Control::OnMouseExit() {
    }

    void Control::OnMouseButton(MouseButton button, bool pressed, const Vec2 &pos) {
    }

    void Control::OnMouseMotion(const Vec2 &pos, const Vec2 &relative) {
    }

    void Control::OnMouseWheel(float delta_x, float delta_y) {
    }

    void Control::OnKey(bool pressed, int key_code) {
    }

    void Control::OnTextInput(const String &text) {
    }

    void Control::OnFocusEnter() {
    }

    void Control::OnFocusExit() {
    }

    void Control::OnResized() {
    }

    Vec2 Control::ToLocal(const Vec2 &global_point) const {
        return global_point - GetGlobalPosition();
    }

    Vec2 Control::ToGlobal(const Vec2 &local_point) const {
        return local_point + GetGlobalPosition();
    }

    void Control::SetAnchors(const float left, const float top, const float right, const float bottom) {
        anchor_left_top = Vec2(left, top);
        anchor_right_bottom = Vec2(right, bottom);
        UpdateLayout();
    }

    void Control::SetAnchorsPreset(const Anchor preset) {
        anchor_preset = preset;
        switch (preset) {
            case Anchor::TopLeft: SetAnchors(0.f, 0.f, 0.f, 0.f);
                break;
            case Anchor::TopCenter: SetAnchors(0.5f, 0.f, 0.5f, 0.f);
                break;
            case Anchor::TopRight: SetAnchors(1.f, 0.f, 1.f, 0.f);
                break;
            case Anchor::CenterLeft: SetAnchors(0.f, 0.5f, 0.f, 0.5f);
                break;
            case Anchor::Center: SetAnchors(0.5f, 0.5f, 0.5f, 0.5f);
                break;
            case Anchor::CenterRight: SetAnchors(1.f, 0.5f, 1.f, 0.5f);
                break;
            case Anchor::BottomLeft: SetAnchors(0.f, 1.f, 0.f, 1.f);
                break;
            case Anchor::BottomCenter: SetAnchors(0.5f, 1.f, 0.5f, 1.f);
                break;
            case Anchor::BottomRight: SetAnchors(1.f, 1.f, 1.f, 1.f);
                break;
        }
    }

    void Control::UpdateLayout() {
        if (!m_Parent) return;

        const auto parent_ctrl = dynamic_cast<Control *>(m_Parent);
        if (!parent_ctrl) return;

        const Vec2 parent_size = parent_ctrl->GetSize();
        const Vec2 anchor_pos_lt = parent_size * anchor_left_top;
        const Vec2 anchor_pos_rb = parent_size * anchor_right_bottom;

        position = anchor_pos_lt + margin_left_top;
        size = anchor_pos_rb + margin_right_bottom - position;
        size = Vec2(Max(size.x, 0.f), Max(size.y, 0.f));
    }
}

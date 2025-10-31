#include "Ashen/Nodes/Node.h"

namespace ash {
    Node::Node(String name): m_Name(MovePtr(name)) {
    }

    Node * Node::GetParent() const { return m_Parent; }

    const Vector<Own<Node>> & Node::GetChildren() const { return m_Children; }

    Node * Node::GetChild(const size_t index) const {
        return index < m_Children.size() ? m_Children[index].get() : nullptr;
    }

    Node * Node::FindChild(const String &name, const bool recursive) const {
        for (const auto &child: m_Children) {
            if (child->GetName() == name) return child.get();
            if (recursive)
                if (const auto found = child->FindChild(name, true)) return found;
        }
        return nullptr;
    }

    size_t Node::GetChildCount() const { return m_Children.size(); }

    void Node::AddChild(Own<Node> child) {
        if (!child) {
            Logger::Warn("Tentative d'ajout d'un enfant null");
            return;
        }

        child->m_Parent = this;

        if (m_InsideTree) {
            child->_EnterTree();
            child->_Ready();
        }

        m_Children.push_back(MovePtr(child));
    }

    void Node::RemoveChild(Node *child) {
        if (!child) return;

        const auto it = std::ranges::find_if(m_Children,
                                             [child](const Own<Node> &ptr) { return ptr.get() == child; });

        if (it != m_Children.end()) {
            if ((*it)->m_InsideTree)
                (*it)->_ExitTree();

            (*it)->m_Parent = nullptr;
            m_Children.erase(it);
        }
    }

    void Node::RemoveFromParent() {
        if (m_Parent)
            m_Parent->RemoveChild(this);
    }

    const String & Node::GetName() const { return m_Name; }

    void Node::SetName(const String &name) { m_Name = name; }

    String Node::GetPath() const {
        if (!m_Parent) return "/" + m_Name;
        return m_Parent->GetPath() + "/" + m_Name;
    }

    void Node::AddToGroup(const String &group) { m_Groups.insert(group); }

    void Node::RemoveFromGroup(const String &group) { m_Groups.erase(group); }

    bool Node::IsInGroup(const String &group) const { return m_Groups.contains(group); }

    const Set<String> & Node::GetGroups() const { return m_Groups; }

    bool Node::IsInsideTree() const { return m_InsideTree; }

    void Node::SetProcessMode(const bool enabled) { m_ProcessEnabled = enabled; }

    bool Node::IsProcessing() const { return m_ProcessEnabled; }

    void Node::_Ready() {
        for (const auto &child: m_Children)
            if (child) child->_Ready();
    }

    void Node::_Process(const float delta) {
        if (!m_ProcessEnabled) return;

        for (const auto &child: m_Children)
            if (child) child->_Process(delta);
    }

    void Node::_PhysicsProcess(const float delta) {
        if (!m_ProcessEnabled) return;

        for (const auto &child: m_Children)
            if (child) child->_PhysicsProcess(delta);
    }

    void Node::_Draw() {
        for (const auto &child: m_Children)
            if (child) child->_Draw();
    }

    void Node::_DispatchEvent(Event &event) {
        if (event.IsHandled()) return;

        _OnEvent(event);

        if (!event.IsHandled()) {
            for (const auto &child: m_Children) {
                if (child) {
                    child->_DispatchEvent(event);
                    if (event.IsHandled()) break;
                }
            }
        }
    }

    void Node::_OnEvent(Event &event) {

    }

    void Node::_EnterTree() {
        m_InsideTree = true;
        for (const auto &child: m_Children)
            if (child) child->_EnterTree();
    }

    void Node::_ExitTree() {
        m_InsideTree = false;
        for (const auto &child: m_Children)
            if (child) child->_ExitTree();
    }

    CanvasItem::CanvasItem(String name): Node(MovePtr(name)) {}

    void CanvasItem::_Draw() {
        if (!visible) return;
        Node::_Draw();
    }

    void CanvasItem::Hide() { visible = false; }

    void CanvasItem::Show() { visible = true; }

    bool CanvasItem::IsVisible() const { return visible; }

    bool CanvasItem::IsVisibleInTree() const {
        if (!visible) return false;
        if (!m_Parent) return visible;
        if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
            return parent->IsVisibleInTree();

        return visible;
    }

    int CanvasItem::GetZIndex() const { return z_index; }

    void CanvasItem::SetZIndex(const int z) { z_index = z; }

    int CanvasItem::GetGlobalZIndex() const {
        if (!z_as_relative || !m_Parent) return z_index;
        if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
            return parent->GetGlobalZIndex() + z_index;

        return z_index;
    }

    Node2D::Node2D(String name): CanvasItem(MovePtr(name)) {
    }

    Transform2D Node2D::GetGlobalTransform() const {
        if (m_Parent)
            if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent))
                return local_transform.Combine(parent2D->GetGlobalTransform());

        return local_transform;
    }

    void Node2D::SetGlobalTransform(const Transform2D &transform) {
        if (m_Parent) {
            if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent)) {
                local_transform = parent2D->GetGlobalTransform().Inverse() * transform;
                return;
            }
        }
        local_transform = transform;
    }

    Vec2 Node2D::GetPosition() const { return local_transform.position; }

    void Node2D::SetPosition(const Vec2 &pos) { local_transform.position = pos; }

    void Node2D::SetPosition(const float x, const float y) { SetPosition(Vec2(x, y)); }

    Vec2 Node2D::GetGlobalPosition() const { return GetGlobalTransform().position; }

    void Node2D::SetGlobalPosition(const Vec2 &pos) {
        auto t = GetGlobalTransform();
        t.position = pos;
        SetGlobalTransform(t);
    }

    float Node2D::GetRotation() const { return local_transform.rotation; }

    void Node2D::SetRotation(const float radians) { local_transform.rotation = radians; }

    void Node2D::SetRotationDegrees(const float degrees) { SetRotation(ToRadians(degrees)); }

    float Node2D::GetRotationDegrees() const { return ToDegrees(GetRotation()); }

    float Node2D::GetGlobalRotation() const { return GetGlobalTransform().rotation; }

    Vec2 Node2D::GetScale() const { return local_transform.scale; }

    void Node2D::SetScale(const Vec2 &scl) { local_transform.scale = scl; }

    void Node2D::SetScale(const float sx, const float sy) { SetScale(Vec2(sx, sy)); }

    void Node2D::Translate(const Vec2 &offset) { local_transform.position += offset; }

    void Node2D::Rotate(const float radians) { local_transform.rotation += radians; }

    void Node2D::ScaleBy(const float factor) { local_transform.scale *= factor; }

    Vec2 Node2D::GetRight() const { return GetGlobalTransform().TransformDirection(Vec2(1.f, 0.f)); }

    Vec2 Node2D::GetUp() const { return GetGlobalTransform().TransformDirection(Vec2(0.f, -1.f)); }

    Vec2 Node2D::ToLocal(const Vec2 &global_point) const {
        return GetGlobalTransform().Inverse().TransformPoint(global_point);
    }

    Vec2 Node2D::ToGlobal(const Vec2 &local_point) const {
        return GetGlobalTransform().TransformPoint(local_point);
    }

    Control::Control(String name): CanvasItem(MovePtr(name)) {
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
        _OnResized();
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
        _OnFocusEnter();
    }

    void Control::ReleaseFocus() {
        if (focused) {
            focused = false;
            _OnFocusExit();
        }
    }

    bool Control::HasFocus() const { return focused; }

    void Control::_OnMouseEnter() {}

    void Control::_OnMouseExit() {}

    void Control::_OnMouseButton(MouseButton button, bool pressed, const Vec2 &pos) {}

    void Control::_OnMouseMotion(const Vec2 &pos, const Vec2 &relative) {}

    void Control::_OnMouseWheel(float delta_x, float delta_y) {}

    void Control::_OnKey(bool pressed, int key_code) {}

    void Control::_OnTextInput(const String &text) {}

    void Control::_OnFocusEnter() {}

    void Control::_OnFocusExit() {}

    void Control::_OnResized() {}

    Vec2 Control::ToLocal(const Vec2 &global_point) const {
        return global_point - GetGlobalPosition();
    }

    Vec2 Control::ToGlobal(const Vec2 &local_point) const {
        return local_point + GetGlobalPosition();
    }

    void Control::SetAnchors(const float left, const float top, const float right, const float bottom) {
        anchor_left_top = Vec2(left, top);
        anchor_right_bottom = Vec2(right, bottom);
        _UpdateLayout();
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

    void Control::_UpdateLayout() {
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

    Container::Container(String name): Control(MovePtr(name)) {}

    void Container::_UpdateChildrenLayout() {
        for (const auto &child: m_Children)
            if (const auto ctrl = dynamic_cast<Container *>(child.get()))
                ctrl->_UpdateChildrenLayout();
    }

    void Container::_OnResized() {
        _UpdateChildrenLayout();
    }

    VBoxContainer::VBoxContainer(String name): Container(MovePtr(name)) {
    }

    void VBoxContainer::_UpdateChildrenLayout() {
        float y_offset = 0.f;

        for (const auto &child: m_Children) {
            const auto ctrl = dynamic_cast<Control *>(child.get());
            if (!ctrl || !ctrl->visible) continue;

            ctrl->position = Vec2(0.f, y_offset);
            Vec2 child_size = ctrl->GetSize();
            child_size.x = size.x;
            ctrl->SetSize(child_size);

            y_offset += child_size.y + separation;
        }
    }

    HBoxContainer::HBoxContainer(String name): Container(MovePtr(name)) {
    }

    void HBoxContainer::_UpdateChildrenLayout() {
        float x_offset = 0.f;

        for (const auto &child: m_Children) {
            const auto ctrl = dynamic_cast<Control *>(child.get());
            if (!ctrl || !ctrl->visible) continue;

            ctrl->position = Vec2(x_offset, 0.f);
            Vec2 child_size = ctrl->GetSize();
            child_size.y = size.y;
            ctrl->SetSize(child_size);

            x_offset += child_size.x + separation;
        }
    }

    Label::Label(String name): Control(MovePtr(name)) {
        mouse_filter = false;
    }

    void Label::SetText(const String &t) {
        text = t;
        _UpdateMinimumSize();
    }

    const String & Label::GetText() const { return text; }

    void Label::_UpdateMinimumSize() {
        min_size = Vec2(text.length() * font_size * 0.6f, font_size + 4.f);
    }

    Button::Button(String name): Control(MovePtr(name)) {
        focusable = true;
        size = Vec2(100.f, 30.f);
    }

    void Button::SetText(const String &t) { text = t; }

    const String & Button::GetText() const { return text; }

    void Button::SetDisabled(const bool d) { disabled = d; }

    bool Button::IsDisabled() const { return disabled; }

    void Button::_OnPressed() {
    }

    void Button::_OnReleased() {
    }

    void Button::_OnToggled(bool is_pressed) {
    }

    void Button::_OnMouseButton(const MouseButton button, const bool is_pressed, const Vec2 &pos) {
        if (disabled || button != MouseButton::Left) return;

        if (is_pressed) {
            pressed = true;
            GrabFocus();
            _OnPressed();
        } else if (pressed) {
            if (toggle_mode) {
                pressed = !pressed;
                _OnToggled(pressed);
            } else {
                pressed = false;
            }
            _OnReleased();
        }
    }

    Panel::Panel(String name): Control(MovePtr(name)) {
    }

    Node3D::Node3D(String name): Node(MovePtr(name)) {
    }

    Transform3D Node3D::GetGlobalTransform() const {
        if (m_Parent)
            if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent))
                return local_transform.Combine(parent3D->GetGlobalTransform());

        return local_transform;
    }

    void Node3D::SetGlobalTransform(const Transform3D &transform) {
        if (m_Parent) {
            if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent)) {
                local_transform = parent3D->GetGlobalTransform().Inverse() * transform;
                return;
            }
        }
        local_transform = transform;
    }

    Vec3 Node3D::GetPosition() const { return local_transform.position; }

    void Node3D::SetPosition(const Vec3 &pos) { local_transform.position = pos; }

    Quaternion Node3D::GetRotation() const { return local_transform.rotation; }

    void Node3D::SetRotation(const Quaternion &rot) { local_transform.rotation = rot; }

    Vec3 Node3D::GetScale() const { return local_transform.scale; }

    void Node3D::SetScale(const Vec3 &scl) { local_transform.scale = scl; }

    Vec3 Node3D::GetGlobalPosition() const { return GetGlobalTransform().position; }

    void Node3D::SetGlobalPosition(const Vec3 &pos) {
        auto t = GetGlobalTransform();
        t.position = pos;
        SetGlobalTransform(t);
    }

    Vec3 Node3D::GetRight() const { return GetGlobalTransform().GetRight(); }

    Vec3 Node3D::GetUp() const { return GetGlobalTransform().GetUp(); }

    Vec3 Node3D::GetForward() const { return GetGlobalTransform().GetForward(); }

    Vec3 Node3D::ToLocal(const Vec3 &global_point) const {
        return GetGlobalTransform().Inverse().TransformPoint(global_point);
    }

    Vec3 Node3D::ToGlobal(const Vec3 &local_point) const {
        return GetGlobalTransform().TransformPoint(local_point);
    }
}

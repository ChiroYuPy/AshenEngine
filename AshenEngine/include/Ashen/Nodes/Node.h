#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Ashen/Core/Codes.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"
#include "Ashen/Math/Math.h"
#include "Ashen/Math/Transform.h"
#include "Ashen/Math/BBox.h"

namespace ash {
    enum class Anchor {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight
    };

    class Node {
    public:
        explicit Node(String name = "Node") : m_Name(MovePtr(name)) {
        }

        virtual ~Node() = default;

        Node(const Node &) = delete;

        Node &operator=(const Node &) = delete;

        Node(Node &&) = default;

        Node &operator=(Node &&) = default;

        Node *GetParent() const { return m_Parent; }
        const Vector<Own<Node> > &GetChildren() const { return m_Children; }

        Node *GetChild(const size_t index) const {
            return index < m_Children.size() ? m_Children[index].get() : nullptr;
        }

        Node *FindChild(const String &name, const bool recursive = false) const {
            for (const auto &child: m_Children) {
                if (child->GetName() == name) return child.get();
                if (recursive)
                    if (const auto found = child->FindChild(name, true)) return found;
            }
            return nullptr;
        }

        size_t GetChildCount() const { return m_Children.size(); }

        void AddChild(Own<Node> child) {
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

        void RemoveChild(Node *child) {
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

        void RemoveFromParent() {
            if (m_Parent)
                m_Parent->RemoveChild(this);
        }

        const String &GetName() const { return m_Name; }
        void SetName(const String &name) { m_Name = name; }

        String GetPath() const {
            if (!m_Parent) return "/" + m_Name;
            return m_Parent->GetPath() + "/" + m_Name;
        }

        void AddToGroup(const String &group) { m_Groups.insert(group); }
        void RemoveFromGroup(const String &group) { m_Groups.erase(group); }
        bool IsInGroup(const String &group) const { return m_Groups.contains(group); }
        const Set<String> &GetGroups() const { return m_Groups; }

        bool IsInsideTree() const { return m_InsideTree; }

        void SetProcessMode(const bool enabled) { m_ProcessEnabled = enabled; }
        bool IsProcessing() const { return m_ProcessEnabled; }

        virtual void _Ready() {
            for (const auto &child: m_Children)
                if (child) child->_Ready();
        }

        virtual void _Process(const float delta) {
            if (!m_ProcessEnabled) return;

            for (const auto &child: m_Children)
                if (child) child->_Process(delta);
        }

        virtual void _PhysicsProcess(const float delta) {
            if (!m_ProcessEnabled) return;

            for (const auto &child: m_Children)
                if (child) child->_PhysicsProcess(delta);
        }

        virtual void _Draw() {
            for (const auto &child: m_Children)
                if (child) child->_Draw();
        }

        virtual void _DispatchEvent(Event &event) {
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

        virtual void _OnEvent(Event &event) {

        }

        virtual void _EnterTree() {
            m_InsideTree = true;
            for (const auto &child: m_Children)
                if (child) child->_EnterTree();
        }

        virtual void _ExitTree() {
            m_InsideTree = false;
            for (const auto &child: m_Children)
                if (child) child->_ExitTree();
        }

    protected:
        Node *m_Parent = nullptr;
        Vector<Own<Node> > m_Children;
        String m_Name;
        Set<String> m_Groups;
        bool m_InsideTree = false;
        bool m_ProcessEnabled = true;
    };

    class CanvasItem : public Node {
    public:
        explicit CanvasItem(String name = "CanvasItem") : Node(MovePtr(name)) {}

        bool visible = true;
        int z_index = 0;
        bool z_as_relative = true;
        float modulate_alpha = 1.f;
        Vec4 modulate_color = Vec4(1.f, 1.f, 1.f, 1.f);

        void _Draw() override {
            if (!visible) return;
            Node::_Draw();
        }

        void Hide() { visible = false; }
        void Show() { visible = true; }
        bool IsVisible() const { return visible; }

        bool IsVisibleInTree() const {
            if (!visible) return false;
            if (!m_Parent) return visible;
            if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
                return parent->IsVisibleInTree();

            return visible;
        }

        int GetZIndex() const { return z_index; }
        void SetZIndex(const int z) { z_index = z; }

        int GetGlobalZIndex() const {
            if (!z_as_relative || !m_Parent) return z_index;
            if (const auto parent = dynamic_cast<const CanvasItem *>(m_Parent))
                return parent->GetGlobalZIndex() + z_index;

            return z_index;
        }
    };

    class Node2D : public CanvasItem {
    public:
        explicit Node2D(String name = "Node2D") : CanvasItem(MovePtr(name)) {
        }

        Transform2D local_transform;

        Transform2D GetGlobalTransform() const {
            if (m_Parent)
                if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent))
                    return local_transform.Combine(parent2D->GetGlobalTransform());

            return local_transform;
        }

        void SetGlobalTransform(const Transform2D &transform) {
            if (m_Parent) {
                if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent)) {
                    local_transform = parent2D->GetGlobalTransform().Inverse() * transform;
                    return;
                }
            }
            local_transform = transform;
        }

        Vec2 GetPosition() const { return local_transform.position; }
        void SetPosition(const Vec2 &pos) { local_transform.position = pos; }
        void SetPosition(const float x, const float y) { SetPosition(Vec2(x, y)); }

        Vec2 GetGlobalPosition() const { return GetGlobalTransform().position; }

        void SetGlobalPosition(const Vec2 &pos) {
            auto t = GetGlobalTransform();
            t.position = pos;
            SetGlobalTransform(t);
        }

        float GetRotation() const { return local_transform.rotation; }
        void SetRotation(const float radians) { local_transform.rotation = radians; }
        void SetRotationDegrees(const float degrees) { SetRotation(ToRadians(degrees)); }
        float GetRotationDegrees() const { return ToDegrees(GetRotation()); }
        float GetGlobalRotation() const { return GetGlobalTransform().rotation; }

        Vec2 GetScale() const { return local_transform.scale; }
        void SetScale(const Vec2 &scl) { local_transform.scale = scl; }
        void SetScale(const float sx, const float sy) { SetScale(Vec2(sx, sy)); }

        void Translate(const Vec2 &offset) { local_transform.position += offset; }
        void Rotate(const float radians) { local_transform.rotation += radians; }
        void ScaleBy(const float factor) { local_transform.scale *= factor; }

        Vec2 GetRight() const { return GetGlobalTransform().TransformDirection(Vec2(1.f, 0.f)); }
        Vec2 GetUp() const { return GetGlobalTransform().TransformDirection(Vec2(0.f, -1.f)); }

        Vec2 ToLocal(const Vec2 &global_point) const {
            return GetGlobalTransform().Inverse().TransformPoint(global_point);
        }

        Vec2 ToGlobal(const Vec2 &local_point) const {
            return GetGlobalTransform().TransformPoint(local_point);
        }
    };

    class Control : public CanvasItem {
    public:
        explicit Control(String name = "Control") : CanvasItem(MovePtr(name)) {
        }

        Vec2 size = Vec2(100.f, 100.f);
        Vec2 position = Vec2(0.f, 0.f);
        Vec2 min_size = Vec2(0.f, 0.f);
        Vec2 custom_minimum_size = Vec2(0.f, 0.f);

        Anchor anchor_preset = Anchor::TopLeft;
        Vec2 anchor_left_top = Vec2(0.f, 0.f);
        Vec2 anchor_right_bottom = Vec2(0.f, 0.f);
        Vec2 margin_left_top = Vec2(0.f, 0.f);
        Vec2 margin_right_bottom = Vec2(0.f, 0.f);

        bool focusable = false;
        bool focused = false;
        bool mouse_filter = true;

        String tooltip_text;

        BBox2 GetGlobalRect() const {
            const Vec2 global_pos = GetGlobalPosition();
            return BBox2(global_pos, global_pos + size);
        }

        BBox2 GetRect() const {
            return BBox2(position, position + size);
        }

        Vec2 GetGlobalPosition() const {
            if (m_Parent)
                if (const auto parent_ctrl = dynamic_cast<const Control *>(m_Parent))
                    return position + parent_ctrl->GetGlobalPosition();

            return position;
        }

        void SetGlobalPosition(const Vec2 &pos) {
            if (m_Parent) {
                if (const auto parent_ctrl = dynamic_cast<const Control *>(m_Parent)) {
                    position = pos - parent_ctrl->GetGlobalPosition();
                    return;
                }
            }
            position = pos;
        }

        Vec2 GetSize() const { return size; }

        void SetSize(const Vec2 &s) {
            Vec2 new_size = s;
            new_size.x = Max(new_size.x, GetMinimumSize().x);
            new_size.y = Max(new_size.y, GetMinimumSize().y);

            if (ApproxEqual(size, new_size)) return;

            size = new_size;
            _OnResized();
        }

        void SetSize(const float w, const float h) { SetSize(Vec2(w, h)); }

        Vec2 GetMinimumSize() const {
            return Vec2(
                Max(min_size.x, custom_minimum_size.x),
                Max(min_size.y, custom_minimum_size.y)
            );
        }

        Vec2 GetCenter() const {
            return position + size * 0.5f;
        }

        bool HasPoint(const Vec2 &point) const {
            return GetGlobalRect().Contains(point);
        }

        void GrabFocus() {
            if (!focusable) return;
            focused = true;
            _OnFocusEnter();
        }

        void ReleaseFocus() {
            if (focused) {
                focused = false;
                _OnFocusExit();
            }
        }

        bool HasFocus() const { return focused; }

        virtual void _OnMouseEnter() {}

        virtual void _OnMouseExit() {}

        virtual void _OnMouseButton(MouseButton button, bool pressed, const Vec2 &pos) {}

        virtual void _OnMouseMotion(const Vec2 &pos, const Vec2 &relative) {}

        virtual void _OnMouseWheel(float delta_x, float delta_y) {}

        virtual void _OnKey(bool pressed, int key_code) {}

        virtual void _OnTextInput(const String &text) {}

        virtual void _OnFocusEnter() {}

        virtual void _OnFocusExit() {}

        virtual void _OnResized() {}

        Vec2 ToLocal(const Vec2 &global_point) const {
            return global_point - GetGlobalPosition();
        }

        Vec2 ToGlobal(const Vec2 &local_point) const {
            return local_point + GetGlobalPosition();
        }

        void SetAnchors(const float left, const float top, const float right, const float bottom) {
            anchor_left_top = Vec2(left, top);
            anchor_right_bottom = Vec2(right, bottom);
            _UpdateLayout();
        }

        void SetAnchorsPreset(const Anchor preset) {
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

    protected:
        virtual void _UpdateLayout() {
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
    };

    class Container : public Control {
    public:
        explicit Container(String name = "Container") : Control(MovePtr(name)) {}

        virtual void _UpdateChildrenLayout() {
            for (const auto &child: m_Children)
                if (const auto ctrl = dynamic_cast<Container *>(child.get()))
                    ctrl->_UpdateChildrenLayout();
        }

        void _OnResized() override {
            _UpdateChildrenLayout();
        }
    };

    class VBoxContainer : public Container {
    public:
        explicit VBoxContainer(String name = "VBoxContainer") : Container(MovePtr(name)) {
        }

        float separation = 4.f;

        void _UpdateChildrenLayout() override {
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
    };

    class HBoxContainer : public Container {
    public:
        explicit HBoxContainer(String name = "HBoxContainer") : Container(MovePtr(name)) {
        }

        float separation = 4.f;

        void _UpdateChildrenLayout() override {
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
    };

    class Label : public Control {
    public:
        explicit Label(String name = "Label") : Control(MovePtr(name)) {
            mouse_filter = false;
        }

        String text;
        float font_size = 16.f;
        Vec4 font_color = Vec4(1.f, 1.f, 1.f, 1.f);
        bool autowrap = false;

        enum class Align { Left, Center, Right };

        Align horizontal_align = Align::Left;
        Align vertical_align = Align::Center;

        void SetText(const String &t) {
            text = t;
            _UpdateMinimumSize();
        }

        const String &GetText() const { return text; }

    protected:
        void _UpdateMinimumSize() {
            min_size = Vec2(text.length() * font_size * 0.6f, font_size + 4.f);
        }
    };

    class Button : public Control {
    public:
        explicit Button(String name = "Button") : Control(MovePtr(name)) {
            focusable = true;
            size = Vec2(100.f, 30.f);
        }

        String text;
        bool pressed = false;
        bool toggle_mode = false;
        bool disabled = false;

        void SetText(const String &t) { text = t; }
        const String &GetText() const { return text; }

        void SetDisabled(const bool d) { disabled = d; }
        bool IsDisabled() const { return disabled; }

        virtual void _OnPressed() {
        }

        virtual void _OnReleased() {
        }

        virtual void _OnToggled(bool is_pressed) {
        }

        void _OnMouseButton(const MouseButton button, const bool is_pressed, const Vec2 &pos) override {
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
    };

    class Panel : public Control {
    public:
        explicit Panel(String name = "Panel") : Control(MovePtr(name)) {
        }

        Vec4 background_color = Vec4(0.2f, 0.2f, 0.2f, 1.f);
    };

    class Node3D : public Node {
    public:
        explicit Node3D(String name = "Node3D") : Node(MovePtr(name)) {
        }

        Transform3D local_transform;

        Transform3D GetGlobalTransform() const {
            if (m_Parent)
                if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent))
                    return local_transform.Combine(parent3D->GetGlobalTransform());

            return local_transform;
        }

        void SetGlobalTransform(const Transform3D &transform) {
            if (m_Parent) {
                if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent)) {
                    local_transform = parent3D->GetGlobalTransform().Inverse() * transform;
                    return;
                }
            }
            local_transform = transform;
        }

        Vec3 GetPosition() const { return local_transform.position; }
        void SetPosition(const Vec3 &pos) { local_transform.position = pos; }

        Quaternion GetRotation() const { return local_transform.rotation; }
        void SetRotation(const Quaternion &rot) { local_transform.rotation = rot; }

        Vec3 GetScale() const { return local_transform.scale; }
        void SetScale(const Vec3 &scl) { local_transform.scale = scl; }

        Vec3 GetGlobalPosition() const { return GetGlobalTransform().position; }

        void SetGlobalPosition(const Vec3 &pos) {
            auto t = GetGlobalTransform();
            t.position = pos;
            SetGlobalTransform(t);
        }

        Vec3 GetRight() const { return GetGlobalTransform().GetRight(); }
        Vec3 GetUp() const { return GetGlobalTransform().GetUp(); }
        Vec3 GetForward() const { return GetGlobalTransform().GetForward(); }

        Vec3 ToLocal(const Vec3 &global_point) const {
            return GetGlobalTransform().Inverse().TransformPoint(global_point);
        }

        Vec3 ToGlobal(const Vec3 &local_point) const {
            return GetGlobalTransform().TransformPoint(local_point);
        }
    };
}

#endif // ASHEN_NODE_H
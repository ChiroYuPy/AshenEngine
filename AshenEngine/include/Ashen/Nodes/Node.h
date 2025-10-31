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
        explicit Node(String name = "Node");

        virtual ~Node() = default;

        Node(const Node &) = delete;

        Node &operator=(const Node &) = delete;

        Node(Node &&) = default;

        Node &operator=(Node &&) = default;

        Node *GetParent() const;

        const Vector<Own<Node> > &GetChildren() const;

        Node *GetChild(size_t index) const;

        Node *FindChild(const String &name, bool recursive = false) const;

        size_t GetChildCount() const;

        void AddChild(Own<Node> child);

        void RemoveChild(Node *child);

        void RemoveFromParent();

        const String &GetName() const;

        void SetName(const String &name);

        String GetPath() const;

        void AddToGroup(const String &group);

        void RemoveFromGroup(const String &group);

        bool IsInGroup(const String &group) const;

        const Set<String> &GetGroups() const;

        bool IsInsideTree() const;

        void SetProcessMode(bool enabled);

        bool IsProcessing() const;

        virtual void _Ready();

        virtual void _Process(float delta);

        virtual void _PhysicsProcess(float delta);

        virtual void _Draw();

        virtual void _DispatchEvent(Event &event);

        virtual void _OnEvent(Event &event);

        virtual void _EnterTree();

        virtual void _ExitTree();

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
        explicit CanvasItem(String name = "CanvasItem");

        bool visible = true;
        int z_index = 0;
        bool z_as_relative = true;
        float modulate_alpha = 1.f;
        Vec4 modulate_color = Vec4(1.f, 1.f, 1.f, 1.f);

        void _Draw() override;

        void Hide();

        void Show();

        bool IsVisible() const;

        bool IsVisibleInTree() const;

        int GetZIndex() const;

        void SetZIndex(int z);

        int GetGlobalZIndex() const;
    };

    class Node2D : public CanvasItem {
    public:
        explicit Node2D(String name = "Node2D");

        Transform2D local_transform;

        Transform2D GetGlobalTransform() const;

        void SetGlobalTransform(const Transform2D &transform);

        Vec2 GetPosition() const;

        void SetPosition(const Vec2 &pos);

        void SetPosition(float x, float y);

        Vec2 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec2 &pos);

        float GetRotation() const;

        void SetRotation(float radians);

        void SetRotationDegrees(float degrees);

        float GetRotationDegrees() const;

        float GetGlobalRotation() const;

        Vec2 GetScale() const;

        void SetScale(const Vec2 &scl);

        void SetScale(float sx, float sy);

        void Translate(const Vec2 &offset);

        void Rotate(float radians);

        void ScaleBy(float factor);

        Vec2 GetRight() const;

        Vec2 GetUp() const;

        Vec2 ToLocal(const Vec2 &global_point) const;

        Vec2 ToGlobal(const Vec2 &local_point) const;
    };

    class Control : public CanvasItem {
    public:
        explicit Control(String name = "Control");

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

        BBox2 GetGlobalRect() const;

        BBox2 GetRect() const;

        Vec2 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec2 &pos);

        Vec2 GetSize() const;

        void SetSize(const Vec2 &s);

        void SetSize(const float w, const float h) { SetSize(Vec2(w, h)); }

        Vec2 GetMinimumSize() const;

        Vec2 GetCenter() const;

        bool HasPoint(const Vec2 &point) const;

        void GrabFocus();

        void ReleaseFocus();

        bool HasFocus() const;

        virtual void _OnMouseEnter();

        virtual void _OnMouseExit();

        virtual void _OnMouseButton(MouseButton button, bool pressed, const Vec2 &pos);

        virtual void _OnMouseMotion(const Vec2 &pos, const Vec2 &relative);

        virtual void _OnMouseWheel(float delta_x, float delta_y);

        virtual void _OnKey(bool pressed, int key_code);

        virtual void _OnTextInput(const String &text);

        virtual void _OnFocusEnter();

        virtual void _OnFocusExit();

        virtual void _OnResized();

        Vec2 ToLocal(const Vec2 &global_point) const;

        Vec2 ToGlobal(const Vec2 &local_point) const;

        void SetAnchors(float left, float top, float right, float bottom);

        void SetAnchorsPreset(Anchor preset);

    protected:
        virtual void _UpdateLayout();
    };

    class Container : public Control {
    public:
        explicit Container(String name = "Container");

        virtual void _UpdateChildrenLayout();

        void _OnResized() override;
    };

    class VBoxContainer : public Container {
    public:
        explicit VBoxContainer(String name = "VBoxContainer");

        float separation = 4.f;

        void _UpdateChildrenLayout() override;
    };

    class HBoxContainer : public Container {
    public:
        explicit HBoxContainer(String name = "HBoxContainer");

        float separation = 4.f;

        void _UpdateChildrenLayout() override;
    };

    class Label : public Control {
    public:
        explicit Label(String name = "Label");

        String text;
        float font_size = 16.f;
        Vec4 font_color = Vec4(1.f, 1.f, 1.f, 1.f);
        bool autowrap = false;

        enum class Align { Left, Center, Right };

        Align horizontal_align = Align::Left;
        Align vertical_align = Align::Center;

        void SetText(const String &t);

        const String &GetText() const;

    protected:
        void _UpdateMinimumSize();
    };

    class Button : public Control {
    public:
        explicit Button(String name = "Button");

        String text;
        bool pressed = false;
        bool toggle_mode = false;
        bool disabled = false;

        void SetText(const String &t);

        const String &GetText() const;

        void SetDisabled(bool d);

        bool IsDisabled() const;

        virtual void _OnPressed();

        virtual void _OnReleased();

        virtual void _OnToggled(bool is_pressed);

        void _OnMouseButton(MouseButton button, bool is_pressed, const Vec2 &pos) override;
    };

    class Panel : public Control {
    public:
        explicit Panel(String name = "Panel");

        Vec4 background_color = Vec4(0.2f, 0.2f, 0.2f, 1.f);
    };

    class Node3D : public Node {
    public:
        explicit Node3D(String name = "Node3D");

        Transform3D local_transform;

        Transform3D GetGlobalTransform() const;

        void SetGlobalTransform(const Transform3D &transform);

        Vec3 GetPosition() const;

        void SetPosition(const Vec3 &pos);

        Quaternion GetRotation() const;

        void SetRotation(const Quaternion &rot);

        Vec3 GetScale() const;

        void SetScale(const Vec3 &scl);

        Vec3 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec3 &pos);

        Vec3 GetRight() const;

        Vec3 GetUp() const;

        Vec3 GetForward() const;

        Vec3 ToLocal(const Vec3 &global_point) const;

        Vec3 ToGlobal(const Vec3 &local_point) const;
    };
}

#endif // ASHEN_NODE_H
#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Ashen/Core/Types.h"
#include "Ashen/Core/UUID.h"
#include "Ashen/Math/Transform.h"
#include "Ashen/Events/Event.h"

namespace ash {
    // Base class for all nodes in the scene tree
    class Node : public std::enable_shared_from_this<Node> {
    public:
        using NodeRef = Ref<Node>;

        explicit Node(const String &name = "Node");

        virtual ~Node() = default;

        // Lifecycle methods
        virtual void OnReady() {
        }

        virtual void OnEnterTree() {
        }

        virtual void OnExitTree() {
        }

        virtual void OnProcess(float delta) {
        }

        virtual void OnPhysicsProcess(float delta) {
        }

        virtual void OnInput(Event &event) {
        }

        // Node tree management
        void AddChild(const NodeRef &child);

        void RemoveChild(const NodeRef &child);

        void RemoveFromParent();

        NodeRef GetChild(size_t index) const;

        NodeRef GetChild(const String &name) const;

        NodeRef FindChild(const String &name, bool recursive = true) const;

        Vector<NodeRef> GetChildren() const { return m_Children; }
        NodeRef GetParent() const { return m_Parent.lock(); }

        NodeRef GetRoot() const;

        size_t GetChildCount() const { return m_Children.size(); }

        int GetIndex() const;

        bool IsAncestorOf(const NodeRef &node) const;

        bool IsInsideTree() const { return m_IsInsideTree; }

        // Node path and identification
        String GetName() const { return m_Name; }
        void SetName(const String &name) { m_Name = name; }

        String GetPath() const;

        UUID GetUUID() const { return m_UUID; }

        // Processing control
        void SetProcessMode(const bool enabled) { m_ProcessEnabled = enabled; }
        bool IsProcessing() const { return m_ProcessEnabled; }
        void SetPhysicsProcessMode(const bool enabled) { m_PhysicsProcessEnabled = enabled; }
        bool IsPhysicsProcessing() const { return m_PhysicsProcessEnabled; }

        // Visibility and activity
        void SetVisible(bool visible);

        bool IsVisible() const { return m_Visible; }

        bool IsVisibleInTree() const;

        // Tree traversal
        void PropagateCall(const String &method, const Function<void(NodeRef)> &callback);

        void QueueFree(); // Mark for deletion at end of frame

        // Debug
        void PrintTree(int indent = 0) const;

    protected:
        // Internal update methods (called by scene tree)
        void _Process(float delta);

        void _PhysicsProcess(float delta);

        void _Input(Event &event);

        void _Ready();

        void _EnterTree();

        void _ExitTree();

        UUID m_UUID;
        String m_Name;
        WeakRef<Node> m_Parent;
        Vector<NodeRef> m_Children;

        bool m_IsInsideTree = false;
        bool m_IsReady = false;
        bool m_ProcessEnabled = true;
        bool m_PhysicsProcessEnabled = true;
        bool m_Visible = true;
        bool m_QueuedForDeletion = false;

        friend class SceneTree;
    };

    // 2D Node with transform
    class Node2D : public Node {
    public:
        explicit Node2D(const String &name = "Node2D");

        // Transform
        virtual void SetPosition(const Vec2 &position);

        Vec2 GetPosition() const { return m_Position; }

        Vec2 GetGlobalPosition() const;

        void SetRotation(float rotation);

        float GetRotation() const { return m_Rotation; }

        float GetGlobalRotation() const;

        void SetScale(const Vec2 &scale);

        Vec2 GetScale() const { return m_Scale; }

        Vec2 GetGlobalScale() const;

        void SetZIndex(const int zIndex) { m_ZIndex = zIndex; }
        int GetZIndex() const { return m_ZIndex; }

        // Transform utilities
        Mat4 GetTransform() const;

        Mat4 GetGlobalTransform() const;

        void LookAt(const Vec2 &target);

        Vec2 ToLocal(const Vec2 &globalPoint) const;

        Vec2 ToGlobal(const Vec2 &localPoint) const;

    protected:
        void UpdateTransform() const;

        Vec2 m_Position{0.0f};
        float m_Rotation = 0.0f;
        Vec2 m_Scale{1.0f};
        int m_ZIndex = 0;

        mutable Mat4 m_Transform{1.0f};
        mutable bool m_TransformDirty = true;
    };

    // 3D Node with transform
    class Node3D : public Node {
    public:
        explicit Node3D(const String &name = "Node3D");

        // Transform
        void SetPosition(const Vec3 &position);

        Vec3 GetPosition() const { return m_Position; }

        Vec3 GetGlobalPosition() const;

        void SetRotation(const Vec3 &rotation);

        Vec3 GetRotation() const { return m_Rotation; }

        Vec3 GetGlobalRotation() const;

        void SetScale(const Vec3 &scale);

        Vec3 GetScale() const { return m_Scale; }

        Vec3 GetGlobalScale() const;

        // Quaternion rotation
        void SetQuaternion(const Quat &quat);

        Quat GetQuaternion() const;

        // Transform utilities
        Mat4 GetTransform() const;

        Mat4 GetGlobalTransform() const;

        void LookAt(const Vec3 &target, const Vec3 &up = Vec3(0, 1, 0));

        Vec3 ToLocal(const Vec3 &globalPoint) const;

        Vec3 ToGlobal(const Vec3 &localPoint) const;

        // Direction vectors
        Vec3 GetForward() const;

        Vec3 GetRight() const;

        Vec3 GetUp() const;

    protected:
        void UpdateTransform() const;

        Vec3 m_Position{0.0f};
        Vec3 m_Rotation{0.0f}; // Euler angles
        Vec3 m_Scale{1.0f};

        mutable Mat4 m_Transform{1.0f};
        mutable bool m_TransformDirty = true;
    };
} // namespace ash

#endif // ASHEN_NODE_H
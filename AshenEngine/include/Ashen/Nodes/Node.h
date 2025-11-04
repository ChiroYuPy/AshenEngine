#ifndef ASHEN_NODE_H
#define ASHEN_NODE_H

#include "Ashen/Core/Logger.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"

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

        // Lifecycle methods (DO NOT override these directly - use _Ready, _Process, etc.)
        virtual void Ready();

        virtual void Process(float delta);

        virtual void PhysicsProcess(float delta);

        virtual void Draw();

        virtual void DispatchEvent(Event &event);

        virtual void OnEvent(Event &event);

        virtual void EnterTree();

        virtual void ExitTree();

    protected:
        // Override these methods in derived classes for custom behavior
        virtual void _Ready() {}

        virtual void _Process(float delta) {}

        virtual void _PhysicsProcess(float delta) {}

        virtual void _Draw() {}

        Node *m_Parent = nullptr;
        Vector<Own<Node> > m_Children;
        String m_Name;
        Set<String> m_Groups;
        bool m_InsideTree = false;
        bool m_ProcessEnabled = true;
    };
}

#endif //ASHEN_NODE_H

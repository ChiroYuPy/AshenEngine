#ifndef ASHEN_NODEGRAPH_H
#define ASHEN_NODEGRAPH_H

#include "Node.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"

namespace ash {
    class NodeGraph {
    public:
        NodeGraph();

        ~NodeGraph();

        Node *GetRoot() const;

        void SetRoot(Own<Node> newRoot);

        void Ready();

        void Process(float deltaTime) const;

        void PhysicsProcess(float deltaTime) const;

        void Draw() const;

        void DispatchEvent(Event &event) const;

        Node *FindNode(const String &name, bool recursive = true) const;

        Vector<Node *> GetNodesInGroup(const String &group) const;

        template<typename T>
        Vector<T *> GetNodesOfType() const;

        void Clear();

        bool IsReady() const;

        size_t GetNodeCount() const;

    private:
        static void CollectNodesInGroup(Node *node, const String &group, Vector<Node *> &result);

        template<typename T>
        void CollectNodesOfType(Node *node, Vector<T *> &result) const;

        static size_t CountNodes(const Node *node);

        Own<Node> m_RootNode;
        bool m_IsReady = false;
    };

    template<typename T>
    Vector<T *> NodeGraph::GetNodesOfType() const {
        Vector<T *> nodes;
        if (m_RootNode)
            CollectNodesOfType<T>(m_RootNode.get(), nodes);

        return nodes;
    }

    template<typename T>
    void NodeGraph::CollectNodesOfType(Node *node, Vector<T *> &result) const {
        if (!node) return;

        if (auto typedNode = dynamic_cast<T *>(node))
            result.push_back(typedNode);

        for (size_t i = 0; i < node->GetChildCount(); ++i)
            if (auto child = node->GetChild(i))
                CollectNodesOfType<T>(child, result);
    }
}

#endif // ASHEN_NODEGRAPH_H
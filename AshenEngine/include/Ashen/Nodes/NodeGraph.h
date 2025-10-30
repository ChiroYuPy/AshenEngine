#ifndef ASHEN_NODEGRAPH_H
#define ASHEN_NODEGRAPH_H

#include "Node.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Events/Event.h"

namespace ash {
    class NodeGraph {
    public:
        NodeGraph() {
            m_RootNode = MakeOwn<Node>("Root");
            m_RootNode->_EnterTree();
        }

        ~NodeGraph() {
            if (m_RootNode)
                m_RootNode->_ExitTree();
        }

        Node *GetRoot() const {
            return m_RootNode.get();
        }

        void SetRoot(Own<Node> newRoot) {
            if (m_RootNode)
                m_RootNode->_ExitTree();

            m_RootNode = MovePtr(newRoot);
            if (m_RootNode) {
                m_RootNode->_EnterTree();
                m_RootNode->_Ready();
            }
        }

        void Ready() {
            if (m_RootNode && !m_IsReady) {
                m_RootNode->_Ready();
                m_IsReady = true;
            }
        }

        void Process(const float deltaTime) const {
            if (m_RootNode && m_IsReady)
                m_RootNode->_Process(deltaTime);
        }

        void PhysicsProcess(const float deltaTime) const {
            if (m_RootNode && m_IsReady)
                m_RootNode->_PhysicsProcess(deltaTime);
        }

        void Draw() const {
            if (m_RootNode && m_IsReady)
                m_RootNode->_Draw();
        }

        void DispatchEvent(Event &event) const {
            if (m_RootNode && m_IsReady)
                m_RootNode->_DispatchEvent(event);
        }

        Node *FindNode(const String &name, const bool recursive = true) const {
            if (!m_RootNode) return nullptr;

            if (m_RootNode->GetName() == name)
                return m_RootNode.get();

            return m_RootNode->FindChild(name, recursive);
        }

        Vector<Node *> GetNodesInGroup(const String &group) const {
            Vector<Node *> nodes;
            if (m_RootNode)
                CollectNodesInGroup(m_RootNode.get(), group, nodes);

            return nodes;
        }

        template<typename T>
        Vector<T *> GetNodesOfType() const {
            Vector<T *> nodes;
            if (m_RootNode)
                CollectNodesOfType<T>(m_RootNode.get(), nodes);

            return nodes;
        }

        void Clear() {
            if (m_RootNode) {
                m_RootNode->_ExitTree();
                m_RootNode.reset();
            }
            m_IsReady = false;
        }

        bool IsReady() const { return m_IsReady; }

        size_t GetNodeCount() const {
            return m_RootNode ? CountNodes(m_RootNode.get()) : 0;
        }

    private:
        static void CollectNodesInGroup(Node *node, const String &group, Vector<Node *> &result) {
            if (!node) return;

            if (node->IsInGroup(group))
                result.push_back(node);

            for (size_t i = 0; i < node->GetChildCount(); ++i)
                if (const auto child = node->GetChild(i))
                    CollectNodesInGroup(child, group, result);
        }

        template<typename T>
        void CollectNodesOfType(Node *node, Vector<T *> &result) const {
            if (!node) return;

            if (auto typedNode = dynamic_cast<T *>(node))
                result.push_back(typedNode);

            for (size_t i = 0; i < node->GetChildCount(); ++i)
                if (auto child = node->GetChild(i))
                    CollectNodesOfType<T>(child, result);
        }

        static size_t CountNodes(const Node *node) {
            if (!node) return 0;

            size_t count = 1;
            for (size_t i = 0; i < node->GetChildCount(); ++i)
                if (const auto child = node->GetChild(i))
                    count += CountNodes(child);

            return count;
        }

        Own<Node> m_RootNode;
        bool m_IsReady = false;
    };
}

#endif // ASHEN_NODEGRAPH_H
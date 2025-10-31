#include "Ashen/Nodes/NodeGraph.h"

namespace ash {
    NodeGraph::NodeGraph() {
        m_RootNode = MakeOwn<Node>("Root");
        m_RootNode->_EnterTree();
    }

    NodeGraph::~NodeGraph() {
        if (m_RootNode)
            m_RootNode->_ExitTree();
    }

    Node * NodeGraph::GetRoot() const {
        return m_RootNode.get();
    }

    void NodeGraph::SetRoot(Own<Node> newRoot) {
        if (m_RootNode)
            m_RootNode->_ExitTree();

        m_RootNode = MovePtr(newRoot);
        if (m_RootNode) {
            m_RootNode->_EnterTree();
            m_RootNode->_Ready();
        }
    }

    void NodeGraph::Ready() {
        if (m_RootNode && !m_IsReady) {
            m_RootNode->_Ready();
            m_IsReady = true;
        }
    }

    void NodeGraph::Process(const float deltaTime) const {
        if (m_RootNode && m_IsReady)
            m_RootNode->_Process(deltaTime);
    }

    void NodeGraph::PhysicsProcess(const float deltaTime) const {
        if (m_RootNode && m_IsReady)
            m_RootNode->_PhysicsProcess(deltaTime);
    }

    void NodeGraph::Draw() const {
        if (m_RootNode && m_IsReady)
            m_RootNode->_Draw();
    }

    void NodeGraph::DispatchEvent(Event &event) const {
        if (m_RootNode && m_IsReady)
            m_RootNode->_DispatchEvent(event);
    }

    Node * NodeGraph::FindNode(const String &name, const bool recursive) const {
        if (!m_RootNode) return nullptr;

        if (m_RootNode->GetName() == name)
            return m_RootNode.get();

        return m_RootNode->FindChild(name, recursive);
    }

    Vector<Node *> NodeGraph::GetNodesInGroup(const String &group) const {
        Vector<Node *> nodes;
        if (m_RootNode)
            CollectNodesInGroup(m_RootNode.get(), group, nodes);

        return nodes;
    }

    void NodeGraph::Clear() {
        if (m_RootNode) {
            m_RootNode->_ExitTree();
            m_RootNode.reset();
        }
        m_IsReady = false;
    }

    bool NodeGraph::IsReady() const { return m_IsReady; }

    size_t NodeGraph::GetNodeCount() const {
        return m_RootNode ? CountNodes(m_RootNode.get()) : 0;
    }

    void NodeGraph::CollectNodesInGroup(Node *node, const String &group, Vector<Node *> &result) {
        if (!node) return;

        if (node->IsInGroup(group))
            result.push_back(node);

        for (size_t i = 0; i < node->GetChildCount(); ++i)
            if (const auto child = node->GetChild(i))
                CollectNodesInGroup(child, group, result);
    }

    size_t NodeGraph::CountNodes(const Node *node) {
        if (!node) return 0;

        size_t count = 1;
        for (size_t i = 0; i < node->GetChildCount(); ++i)
            if (const auto child = node->GetChild(i))
                count += CountNodes(child);

        return count;
    }
}

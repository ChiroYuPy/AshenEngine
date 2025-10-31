#include "Ashen/Nodes/Node.h"
#include "Ashen/Core/Logger.h"
#include <algorithm>

namespace ash {
    Node::Node(String name) : m_Name(MovePtr(name)) {
    }

    Node *Node::GetParent() const { return m_Parent; }
    const Vector<Own<Node> > &Node::GetChildren() const { return m_Children; }

    Node *Node::GetChild(size_t index) const {
        return index < m_Children.size() ? m_Children[index].get() : nullptr;
    }

    Node *Node::FindChild(const String &name, bool recursive) const {
        for (const auto &child: m_Children) {
            if (child->GetName() == name) return child.get();
            if (recursive) {
                if (auto found = child->FindChild(name, true)) return found;
            }
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
            child->EnterTree();
            child->Ready();
        }
        m_Children.push_back(MovePtr(child));
    }

    void Node::RemoveChild(Node *child) {
        if (!child) return;
        const auto it = std::ranges::find_if(m_Children, [child](const Own<Node> &ptr) { return ptr.get() == child; });
        if (it != m_Children.end()) {
            if ((*it)->m_InsideTree)
                (*it)->ExitTree();
            (*it)->m_Parent = nullptr;
            m_Children.erase(it);
        }
    }

    void Node::RemoveFromParent() {
        if (m_Parent) m_Parent->RemoveChild(this);
    }

    const String &Node::GetName() const { return m_Name; }
    void Node::SetName(const String &name) { m_Name = name; }

    String Node::GetPath() const {
        if (!m_Parent) return "/" + m_Name;
        return m_Parent->GetPath() + "/" + m_Name;
    }

    void Node::AddToGroup(const String &group) { m_Groups.insert(group); }
    void Node::RemoveFromGroup(const String &group) { m_Groups.erase(group); }
    bool Node::IsInGroup(const String &group) const { return m_Groups.contains(group); }
    const Set<String> &Node::GetGroups() const { return m_Groups; }
    bool Node::IsInsideTree() const { return m_InsideTree; }
    void Node::SetProcessMode(bool enabled) { m_ProcessEnabled = enabled; }
    bool Node::IsProcessing() const { return m_ProcessEnabled; }

    void Node::Ready() {
        for (const auto &child: m_Children)
            if (child) child->Ready();
    }

    void Node::Process(float delta) {
        if (!m_ProcessEnabled) return;
        for (const auto &child: m_Children)
            if (child) child->Process(delta);
    }

    void Node::PhysicsProcess(float delta) {
        if (!m_ProcessEnabled) return;
        for (const auto &child: m_Children)
            if (child) child->PhysicsProcess(delta);
    }

    void Node::Draw() {
        for (const auto &child: m_Children)
            if (child) child->Draw();
    }

    void Node::DispatchEvent(Event &event) {
        if (event.IsHandled()) return;
        OnEvent(event);
        if (!event.IsHandled()) {
            for (const auto &child: m_Children) {
                if (child) {
                    child->DispatchEvent(event);
                    if (event.IsHandled()) break;
                }
            }
        }
    }

    void Node::OnEvent(Event &event) {
    }

    void Node::EnterTree() {
        m_InsideTree = true;
        for (const auto &child: m_Children)
            if (child) child->EnterTree();
    }

    void Node::ExitTree() {
        m_InsideTree = false;
        for (const auto &child: m_Children)
            if (child) child->ExitTree();
    }
}

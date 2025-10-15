#include "Ashen/Scene/Node.h"
#include "Ashen/Scene/SceneTree.h"
#include <algorithm>

namespace ash {
    Node::Node(String nodeName) : name(std::move(nodeName)) {
        if (name.empty())
            name = Node::GetClassName();
    }

    void Node::AddChild(const Ref<Node> &child) {
        if (!child || child->parent) return;

        String uniqueName = child->name;
        int counter = 1;
        while (HasNode(uniqueName))
            uniqueName = child->name + std::to_string(counter++);

        child->name = uniqueName;
        child->parent = this;
        children.push_back(child);

        if (insideTree)
            AddChildToTree(child.get());
    }

    void Node::RemoveChild(Node* child) {
        if (!child || child->parent != this) return;

        if (child->insideTree)
            RemoveChildFromTree(child);

        auto it = std::ranges::find_if(children, [child](const Ref<Node>& c) {
            return c.get() == child;
        });

        if (it != children.end())
            children.erase(it);

        child->parent = nullptr;
    }

    Node* Node::GetParent() const {
        return parent;
    }

    Node* Node::GetChild(const int idx) const {
        if (idx >= 0 && idx < children.size())
            return children[idx].get();
        return nullptr;
    }

    int Node::GetChildCount() const {
        return static_cast<int>(children.size());
    }

    Vector<Ref<Node>> Node::GetChildren() const {
        return children;
    }

    Node* Node::GetNode(const String& path) const {
        if (path.empty()) return nullptr;

        if (path[0] == '/') {
            if (!tree) return nullptr;
            auto root = tree->GetRoot();
            if (!root) return nullptr;
            return root->GetNode(path.substr(1));
        }

        const size_t slashPos = path.find('/');
        const String first = slashPos == String::npos ? path : path.substr(0, slashPos);
        const String rest = slashPos == String::npos ? "" : path.substr(slashPos + 1);

        if (first == "..") {
            if (parent)
                return rest.empty() ? parent : parent->GetNode(rest);
            return nullptr;
        }

        if (first == ".")
            return rest.empty() ? const_cast<Node*>(this) : GetNode(rest);

        for (const auto& child : children)
            if (child->name == first)
                return rest.empty() ? child.get() : child->GetNode(rest);

        return nullptr;
    }

    Node* Node::FindChild(const String& pattern, const bool recursive) const {
        for (const auto& child : children) {
            if (child->name == pattern || pattern == "*")
                return child.get();

            if (recursive) {
                auto found = child->FindChild(pattern, recursive);
                if (found) return found;
            }
        }
        return nullptr;
    }

    void Node::SetName(const String& newName) { name = newName; }
    String Node::GetName() const { return name; }

    void Node::SetOwner(Node* newOwner) { owner = newOwner; }
    Node* Node::GetOwner() const { return owner; }

    bool Node::IsInsideTree() const { return insideTree; }
    SceneTree* Node::GetTree() const { return tree; }

    void Node::SetVisible(const bool v) { visible = v; }
    bool Node::IsVisible() const { return visible; }

    String Node::GetPath() const {
        if (!parent) return name;
        return parent->GetPath() + "/" + name;
    }

    bool Node::HasNode(const String& nodeName) const {
        return std::ranges::any_of(children, [&](const Ref<Node>& child) {
            return child->name == nodeName;
        });
    }

    void Node::AddChildToTree(Node* child) const {
        child->insideTree = true;
        child->tree = this->tree;
        for (const auto& grandchild : child->children)
            child->AddChildToTree(grandchild.get());
    }

    void Node::RemoveChildFromTree(Node* child) {
        for (const auto& grandchild : child->children)
            RemoveChildFromTree(grandchild.get());
        child->insideTree = false;
        child->tree = nullptr;
    }
}
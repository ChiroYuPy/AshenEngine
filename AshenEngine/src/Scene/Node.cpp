#include "Ashen/Scene/Node.h"

#include <algorithm>
#include <ranges>

namespace ash {
    uint32_t Node::s_NextInstanceID = 1;

    Node::Node(const std::string& name)
        : m_Name(name.empty() ? "Node" : name),
          m_InstanceID(s_NextInstanceID++) {}

    Node::~Node() {
        OnDestroy();
        for (const auto& comp : m_Components | std::views::values)
            comp->OnDestroy();
        m_Components.clear();
    }

    void Node::AddChild(const NodeRef& child) {
        if (!child || child->m_Parent != nullptr) return;  // FIX: vérifier nullptr, pas this
        AddChildInternal(child);
    }

    void Node::RemoveChild(Node* child) {
        if (!child || child->m_Parent != this) return;
        RemoveChildInternal(child);
    }

    Node* Node::GetChild(const int idx) const {
        return idx >= 0 && idx < static_cast<int>(m_Children.size()) ? m_Children[idx].get() : nullptr;
    }

    Node::NodeRef Node::FindChild(const std::string& name, const bool recursive) const {
        for (const auto& child : m_Children) {
            if (child->GetName() == name) return child;
            if (recursive) {
                auto found = child->FindChild(name, recursive);
                if (found) return found;
            }
        }
        return nullptr;
    }

    void Node::SetName(const std::string& name) {
        m_Name = name;
    }

    std::string Node::GetPath() const {
        if (!m_Parent) return "/" + m_Name;
        return m_Parent->GetPath() + "/" + m_Name;
    }

    void Node::SetActive(const bool active) {
        m_Active = active;
    }

    void Node::SetVisible(const bool visible) {
        m_Visible = visible;
    }

    Transform Node::GetGlobalTransform() const {
        UpdateCachedTransforms();
        return m_GlobalTransform;
    }

    Vec3 Node::GetGlobalPosition() const {
        return GetGlobalTransform().position;
    }

    Vec3 Node::GetGlobalRotation() const {
        return GetGlobalTransform().rotation;
    }

    Vec3 Node::GetGlobalScale() const {
        return GetGlobalTransform().scale;
    }

    void Node::SetGlobalPosition(const Vec3& pos) {
        if (m_Parent) {
            const auto parentGlobal = m_Parent->GetGlobalTransform();
            const auto parentInv = glm::inverse(parentGlobal.GetMatrix());
            const Vec4 localPos = parentInv * Vec4(pos, 1.0f);
            SetLocalPosition(Vec3(localPos));
        } else {
            SetLocalPosition(pos);
        }
    }

    Mat4 Node::GetWorldMatrix() const {
        UpdateCachedTransforms();
        return m_WorldMatrix;
    }

    void Node::RemoveComponent(const std::type_info& type) {
        const auto it = m_Components.find(&type);
        if (it != m_Components.end()) {
            it->second->OnDestroy();
            m_Components.erase(it);
        }
    }

    void Node::OnUpdate(const float ts) {
        for (const auto& comp : m_Components | std::views::values) {
            if (comp->IsActive())
                comp->OnUpdate(ts);
        }
    }

    void Node::OnRender() {
        for (const auto& comp : m_Components | std::views::values) {
            if (comp->IsActive())
                comp->OnRender();
        }
    }

    void Node::MarkTransformDirty() const {
        m_TransformDirty = true;
        for (const auto& child : m_Children)
            child->MarkTransformDirty();
    }

    void Node::UpdateCachedTransforms() const {
        if (!m_TransformDirty) return;

        if (m_Parent) {
            const auto parentMat = m_Parent->GetWorldMatrix();
            m_WorldMatrix = parentMat * m_LocalTransform.GetMatrix();
        } else {
            m_WorldMatrix = m_LocalTransform.GetMatrix();
        }

        m_GlobalTransform = m_LocalTransform;
        m_GlobalTransform.position = Vec3(m_WorldMatrix[3]);

        m_TransformDirty = false;
    }

    void Node::AddChildInternal(const NodeRef& child) {
        child->m_Parent = this;
        m_Children.push_back(child);
        if (m_Tree)
            child->SetTreeRecursive(m_Tree);

        child->MarkTransformDirty();
        child->OnCreate();
    }

    void Node::RemoveChildInternal(Node* child) {
        const auto it = std::ranges::find_if(m_Children, [child](const NodeRef& c) {
            return c.get() == child;
        });
        if (it != m_Children.end()) {
            child->OnDestroy();
            child->SetTreeRecursive(nullptr);
            child->m_Parent = nullptr;
            m_Children.erase(it);
        }
    }

    void Node::SetTreeRecursive(SceneTree* tree) {
        m_Tree = tree;
        for (const auto& child : m_Children)
            child->SetTreeRecursive(tree);
    }
}
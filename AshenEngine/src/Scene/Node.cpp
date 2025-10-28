
#include "Ashen/Scene/Node.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace ash {

void Node::AddChild(NodePtr child) {
    if (!child || child.get() == this) return;

    // Retirer de l'ancien parent
    if (auto oldParent = child->m_Parent.lock()) {
        oldParent->RemoveChild(child);
    }

    child->m_Parent = shared_from_this();
    m_Children.push_back(child);

    if (!child->m_IsReady) {
        child->OnReady();
        child->m_IsReady = true;
    }
}

void Node::RemoveChild(NodePtr child) {
    auto it = std::find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end()) {
        (*it)->m_Parent.reset();
        m_Children.erase(it);
    }
}

void Node::RemoveFromParent() {
    if (auto parent = m_Parent.lock()) {
        parent->RemoveChild(shared_from_this());
    }
}

Node::NodePtr Node::FindChild(const String& name, bool recursive) const {
    for (const auto& child : m_Children) {
        if (child->m_Name == name) return child;
        if (recursive) {
            if (auto found = child->FindChild(name, true)) {
                return found;
            }
        }
    }
    return nullptr;
}

bool Node::IsVisibleInTree() const {
    if (!m_Visible) return false;
    if (auto parent = m_Parent.lock()) {
        return parent->IsVisibleInTree();
    }
    return true;
}

Vec2 Node::GetGlobalPosition() const {
    Vec2 pos = m_Position;
    if (auto parent = m_Parent.lock()) {
        Vec4 globalPos = parent->GetGlobalTransform() * Vec4(m_Position.x, m_Position.y, 0.0f, 1.0f);
        return Vec2(globalPos.x, globalPos.y);
    }
    return pos;
}

Mat4 Node::GetTransform() const {
    if (m_TransformDirty) {
        m_Transform = glm::translate(Mat4(1.0f), Vec3(m_Position.x, m_Position.y, 0.0f))
                    * glm::rotate(Mat4(1.0f), m_Rotation, Vec3(0.0f, 0.0f, 1.0f))
                    * glm::scale(Mat4(1.0f), Vec3(m_Scale.x, m_Scale.y, 1.0f));
        m_TransformDirty = false;
    }
    return m_Transform;
}

Mat4 Node::GetGlobalTransform() const {
    Mat4 transform = GetTransform();
    if (auto parent = m_Parent.lock()) {
        return parent->GetGlobalTransform() * transform;
    }
    return transform;
}

} // namespace ash
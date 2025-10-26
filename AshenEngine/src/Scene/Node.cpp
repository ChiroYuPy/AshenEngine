#include "Ashen/Scene/Node.h"
#include "Ashen/Scene/SceneTree.h"
#include "Ashen/Core/Logger.h"
#include <algorithm>

namespace ash {
    // ==================== Node ====================

    Node::Node(const String &name)
        : m_UUID(), m_Name(name) {
    }

    void Node::AddChild(const NodeRef &child) {
        if (!child || child.get() == this) {
            Logger::Warn() << "Cannot add null or self as child";
            return;
        }

        // Remove from previous parent
        if (const auto prevParent = child->m_Parent.lock())
            prevParent->RemoveChild(child);

        child->m_Parent = shared_from_this();
        m_Children.push_back(child);

        // Notify scene tree
        if (m_IsInsideTree)
            child->_EnterTree();
    }

    void Node::RemoveChild(const NodeRef &child) {
        const auto it = std::ranges::find(m_Children, child);
        if (it != m_Children.end()) {
            if (child->m_IsInsideTree)
                child->_ExitTree();

            child->m_Parent.reset();
            m_Children.erase(it);
        }
    }

    void Node::RemoveFromParent() {
        if (const auto parent = m_Parent.lock())
            parent->RemoveChild(shared_from_this());
    }

    Node::NodeRef Node::GetChild(const size_t index) const {
        if (index < m_Children.size())
            return m_Children[index];

        return nullptr;
    }

    Node::NodeRef Node::GetChild(const String &name) const {
        for (const auto &child: m_Children)
            if (child->m_Name == name)
                return child;

        return nullptr;
    }

    Node::NodeRef Node::FindChild(const String &name, const bool recursive) const {
        for (const auto &child: m_Children) {
            if (child->m_Name == name)
                return child;

            if (recursive)
                if (auto found = child->FindChild(name, true))
                    return found;
        }
        return nullptr;
    }

    Node::NodeRef Node::GetRoot() const {
        NodeRef root = const_cast<Node *>(this)->shared_from_this();
        while (const auto parent = root->m_Parent.lock())
            root = parent;

        return root;
    }

    int Node::GetIndex() const {
        if (const auto parent = m_Parent.lock()) {
            const auto &siblings = parent->m_Children;
            const auto it = std::ranges::find_if(siblings, [this](const NodeRef &n) { return n.get() == this; });
            if (it != siblings.end())
                return static_cast<int>(std::distance(siblings.begin(), it));
        }
        return -1;
    }

    bool Node::IsAncestorOf(const NodeRef &node) const {
        if (!node) return false;
        auto current = node->m_Parent.lock();
        while (current) {
            if (current.get() == this) return true;
            current = current->m_Parent.lock();
        }
        return false;
    }

    String Node::GetPath() const {
        String path = m_Name;
        auto current = m_Parent.lock();
        while (current) {
            path = current->m_Name + "/" + path;
            current = current->m_Parent.lock();
        }
        return "/" + path;
    }

    void Node::SetVisible(const bool visible) {
        if (m_Visible != visible) {
            m_Visible = visible;
            for (const auto &child: m_Children)
                child->SetVisible(visible);
        }
    }

    bool Node::IsVisibleInTree() const {
        if (!m_Visible) return false;
        if (const auto parent = m_Parent.lock())
            return parent->IsVisibleInTree();

        return true;
    }

    void Node::PropagateCall(const String &method, const Function<void(NodeRef)> &callback) {
        callback(shared_from_this());
        for (const auto &child: m_Children)
            child->PropagateCall(method, callback);
    }

    void Node::QueueFree() {
        m_QueuedForDeletion = true;
    }

    void Node::PrintTree(const int indent) const {
        const String indentStr(indent * 2, ' ');
        printf("%s- %s [%s]\n", indentStr.c_str(), m_Name.c_str(),
               typeid(*this).name());

        for (const auto &child: m_Children)
            child->PrintTree(indent + 1);
    }

    void Node::_Process(const float delta) {
        if (!m_ProcessEnabled || !m_Visible) return;

        OnProcess(delta);

        for (const auto &child: m_Children)
            child->_Process(delta);
    }

    void Node::_PhysicsProcess(const float delta) {
        if (!m_PhysicsProcessEnabled || !m_Visible) return;

        OnPhysicsProcess(delta);

        for (const auto &child: m_Children)
            child->_PhysicsProcess(delta);
    }

    void Node::_Input(Event &event) {
        if (!m_Visible) return;

        OnInput(event);

        for (const auto &child: m_Children)
            child->_Input(event);
    }

    void Node::_Ready() {
        if (m_IsReady) return;

        OnReady();
        m_IsReady = true;

        for (const auto &child: m_Children)
            child->_Ready();
    }

    void Node::_EnterTree() {
        if (m_IsInsideTree) return;

        m_IsInsideTree = true;
        OnEnterTree();

        for (const auto &child: m_Children)
            child->_EnterTree();

        if (!m_IsReady)
            _Ready();
    }

    void Node::_ExitTree() {
        if (!m_IsInsideTree) return;

        for (const auto &child: m_Children)
            child->_ExitTree();

        OnExitTree();
        m_IsInsideTree = false;
    }

    Node2D::Node2D(const String &name) : Node(name) {
    }

    void Node2D::SetPosition(const Vec2 &position) {
        m_Position = position;
        m_TransformDirty = true;
    }

    Vec2 Node2D::GetGlobalPosition() const {
        if (const auto parent = std::dynamic_pointer_cast < Node2D > (m_Parent.lock()))
            return Vec2(parent->GetGlobalTransform() * Vec4(m_Position.x, m_Position.y, 0.0f, 1.0f));

        return m_Position;
    }

    void Node2D::SetRotation(const float rotation) {
        m_Rotation = rotation;
        m_TransformDirty = true;
    }

    float Node2D::GetGlobalRotation() const {
        float rot = m_Rotation;
        if (const auto parent = std::dynamic_pointer_cast < Node2D > (m_Parent.lock()))
            rot += parent->GetGlobalRotation();

        return rot;
    }

    void Node2D::SetScale(const Vec2 &scale) {
        m_Scale = scale;
        m_TransformDirty = true;
    }

    Vec2 Node2D::GetGlobalScale() const {
        Vec2 scale = m_Scale;
        if (const auto parent = std::dynamic_pointer_cast < Node2D > (m_Parent.lock())) {
            const Vec2 parentScale = parent->GetGlobalScale();
            scale.x *= parentScale.x;
            scale.y *= parentScale.y;
        }
        return scale;
    }

    Mat4 Node2D::GetTransform() const {
        if (m_TransformDirty)
            this->UpdateTransform();

        return m_Transform;
    }

    Mat4 Node2D::GetGlobalTransform() const {
        Mat4 transform = GetTransform();
        if (const auto parent = std::dynamic_pointer_cast < Node2D > (m_Parent.lock()))
            transform = parent->GetGlobalTransform() * transform;

        return transform;
    }

    void Node2D::UpdateTransform() const {
        m_Transform = glm::translate(Mat4(1.0f), Vec3(m_Position.x, m_Position.y, 0.0f))
                      * glm::rotate(Mat4(1.0f), m_Rotation, Vec3(0.0f, 0.0f, 1.0f))
                      * glm::scale(Mat4(1.0f), Vec3(m_Scale.x, m_Scale.y, 1.0f));
        m_TransformDirty = false;
    }

    void Node2D::LookAt(const Vec2 &target) {
        const Vec2 dir = target - m_Position;
        m_Rotation = std::atan2(dir.y, dir.x);
        m_TransformDirty = true;
    }

    Vec2 Node2D::ToLocal(const Vec2 &globalPoint) const {
        const Mat4 invTransform = glm::inverse(GetGlobalTransform());
        const Vec4 local = invTransform * Vec4(globalPoint.x, globalPoint.y, 0.0f, 1.0f);
        return Vec2(local.x, local.y);
    }

    Vec2 Node2D::ToGlobal(const Vec2 &localPoint) const {
        const Vec4 global = GetGlobalTransform() * Vec4(localPoint.x, localPoint.y, 0.0f, 1.0f);
        return Vec2(global.x, global.y);
    }

    // ==================== Node3D ====================

    Node3D::Node3D(const String &name) : Node(name) {
    }

    void Node3D::SetPosition(const Vec3 &position) {
        m_Position = position;
        m_TransformDirty = true;
    }

    Vec3 Node3D::GetGlobalPosition() const {
        if (const auto parent = std::dynamic_pointer_cast < Node3D > (m_Parent.lock()))
            return Vec3(parent->GetGlobalTransform() * Vec4(m_Position, 1.0f));

        return m_Position;
    }

    void Node3D::SetRotation(const Vec3 &rotation) {
        m_Rotation = rotation;
        m_TransformDirty = true;
    }

    Vec3 Node3D::GetGlobalRotation() const {
        Vec3 rot = m_Rotation;
        if (const auto parent = std::dynamic_pointer_cast < Node3D > (m_Parent.lock()))
            rot += parent->GetGlobalRotation();

        return rot;
    }

    void Node3D::SetScale(const Vec3 &scale) {
        m_Scale = scale;
        m_TransformDirty = true;
    }

    Vec3 Node3D::GetGlobalScale() const {
        Vec3 scale = m_Scale;
        if (const auto parent = std::dynamic_pointer_cast < Node3D > (m_Parent.lock()))
            scale *= parent->GetGlobalScale();

        return scale;
    }

    void Node3D::SetQuaternion(const Quat &quat) {
        m_Rotation = glm::eulerAngles(quat);
        m_TransformDirty = true;
    }

    Quat Node3D::GetQuaternion() const {
        return Quat(m_Rotation);
    }

    Mat4 Node3D::GetTransform() const {
        if (m_TransformDirty)
            this->UpdateTransform();

        return m_Transform;
    }

    Mat4 Node3D::GetGlobalTransform() const {
        Mat4 transform = GetTransform();
        if (const auto parent = std::dynamic_pointer_cast < Node3D > (m_Parent.lock()))
            transform = parent->GetGlobalTransform() * transform;

        return transform;
    }

    void Node3D::UpdateTransform() const {
        m_Transform = glm::translate(Mat4(1.0f), m_Position)
                      * glm::mat4_cast(Quat(m_Rotation))
                      * glm::scale(Mat4(1.0f), m_Scale);
        m_TransformDirty = false;
    }

    void Node3D::LookAt(const Vec3 &target, const Vec3 &up) {
        const Vec3 forward = glm::normalize(target - m_Position);
        const Vec3 right = glm::normalize(glm::cross(forward, up));
        const Vec3 newUp = glm::cross(right, forward);

        auto lookMatrix = Mat4(1.0f);
        lookMatrix[0] = Vec4(right, 0.0f);
        lookMatrix[1] = Vec4(newUp, 0.0f);
        lookMatrix[2] = Vec4(-forward, 0.0f);

        const Quat quat = glm::quat_cast(lookMatrix);
        m_Rotation = glm::eulerAngles(quat);
        m_TransformDirty = true;
    }

    Vec3 Node3D::ToLocal(const Vec3 &globalPoint) const {
        const Mat4 invTransform = glm::inverse(GetGlobalTransform());
        return Vec3(invTransform * Vec4(globalPoint, 1.0f));
    }

    Vec3 Node3D::ToGlobal(const Vec3 &localPoint) const {
        return Vec3(GetGlobalTransform() * Vec4(localPoint, 1.0f));
    }

    Vec3 Node3D::GetForward() const {
        return glm::normalize(Vec3(GetGlobalTransform() * Vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    }

    Vec3 Node3D::GetRight() const {
        return glm::normalize(Vec3(GetGlobalTransform() * Vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    }

    Vec3 Node3D::GetUp() const {
        return glm::normalize(Vec3(GetGlobalTransform() * Vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    }
} // namespace ash
#include "Ashen/Nodes/Node3D.h"

namespace ash {
    Node3D::Node3D(String name) : Node(MovePtr(name)) {
    }

    Transform3D Node3D::GetGlobalTransform() const {
        if (m_Parent)
            if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent))
                return local_transform.Combine(parent3D->GetGlobalTransform());

        return local_transform;
    }

    void Node3D::SetGlobalTransform(const Transform3D &transform) {
        if (m_Parent) {
            if (const auto parent3D = dynamic_cast<const Node3D *>(m_Parent)) {
                local_transform = parent3D->GetGlobalTransform().Inverse() * transform;
                return;
            }
        }
        local_transform = transform;
    }

    Vec3 Node3D::GetPosition() const { return local_transform.position; }

    void Node3D::SetPosition(const Vec3 &pos) { local_transform.position = pos; }

    Quaternion Node3D::GetRotation() const { return local_transform.rotation; }

    void Node3D::SetRotation(const Quaternion &rot) { local_transform.rotation = rot; }

    Vec3 Node3D::GetScale() const { return local_transform.scale; }

    void Node3D::SetScale(const Vec3 &scl) { local_transform.scale = scl; }

    Vec3 Node3D::GetGlobalPosition() const { return GetGlobalTransform().position; }

    void Node3D::SetGlobalPosition(const Vec3 &pos) {
        auto t = GetGlobalTransform();
        t.position = pos;
        SetGlobalTransform(t);
    }

    Vec3 Node3D::GetRight() const { return GetGlobalTransform().GetRight(); }

    Vec3 Node3D::GetUp() const { return GetGlobalTransform().GetUp(); }

    Vec3 Node3D::GetForward() const { return GetGlobalTransform().GetForward(); }

    Vec3 Node3D::ToLocal(const Vec3 &global_point) const {
        return GetGlobalTransform().Inverse().TransformPoint(global_point);
    }

    Vec3 Node3D::ToGlobal(const Vec3 &local_point) const {
        return GetGlobalTransform().TransformPoint(local_point);
    }
}

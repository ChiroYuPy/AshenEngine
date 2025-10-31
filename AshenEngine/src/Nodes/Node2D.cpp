#include "Ashen/Nodes/Node2D.h"
#include "Ashen/Math/Math.h"

namespace ash {
    Node2D::Node2D(String name) : CanvasItem(MovePtr(name)) {
    }

    Transform2D Node2D::GetGlobalTransform() const {
        if (m_Parent)
            if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent))
                return local_transform.Combine(parent2D->GetGlobalTransform());

        return local_transform;
    }

    void Node2D::SetGlobalTransform(const Transform2D &transform) {
        if (m_Parent) {
            if (const auto parent2D = dynamic_cast<const Node2D *>(m_Parent)) {
                local_transform = parent2D->GetGlobalTransform().Inverse() * transform;
                return;
            }
        }
        local_transform = transform;
    }

    Vec2 Node2D::GetPosition() const { return local_transform.position; }

    void Node2D::SetPosition(const Vec2 &pos) { local_transform.position = pos; }

    void Node2D::SetPosition(const float x, const float y) { SetPosition(Vec2(x, y)); }

    Vec2 Node2D::GetGlobalPosition() const { return GetGlobalTransform().position; }

    void Node2D::SetGlobalPosition(const Vec2 &pos) {
        auto t = GetGlobalTransform();
        t.position = pos;
        SetGlobalTransform(t);
    }

    float Node2D::GetRotation() const { return local_transform.rotation; }

    void Node2D::SetRotation(const float radians) { local_transform.rotation = radians; }

    void Node2D::SetRotationDegrees(const float degrees) { SetRotation(ToRadians(degrees)); }

    float Node2D::GetRotationDegrees() const { return ToDegrees(GetRotation()); }

    float Node2D::GetGlobalRotation() const { return GetGlobalTransform().rotation; }

    Vec2 Node2D::GetScale() const { return local_transform.scale; }

    void Node2D::SetScale(const Vec2 &scl) { local_transform.scale = scl; }

    void Node2D::SetScale(const float sx, const float sy) { SetScale(Vec2(sx, sy)); }

    void Node2D::Translate(const Vec2 &offset) { local_transform.position += offset; }

    void Node2D::Rotate(const float radians) { local_transform.rotation += radians; }

    void Node2D::ScaleBy(const float factor) { local_transform.scale *= factor; }

    Vec2 Node2D::GetRight() const { return GetGlobalTransform().TransformDirection(Vec2(1.f, 0.f)); }

    Vec2 Node2D::GetUp() const { return GetGlobalTransform().TransformDirection(Vec2(0.f, -1.f)); }

    Vec2 Node2D::ToLocal(const Vec2 &global_point) const {
        return GetGlobalTransform().Inverse().TransformPoint(global_point);
    }

    Vec2 Node2D::ToGlobal(const Vec2 &local_point) const {
        return GetGlobalTransform().TransformPoint(local_point);
    }
}

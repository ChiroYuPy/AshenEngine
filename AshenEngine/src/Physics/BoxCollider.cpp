#include "Ashen/Physics/BoxCollider.h"
#include "Ashen/Physics/SphereCollider.h"
#include "Ashen/Physics/CapsuleCollider.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Math.h"

namespace ash {
    BoxCollider::BoxCollider(const Vec3& size)
        : Collider(ColliderType::Box)
        , m_Size(size) {
    }

    BBox<3, float> BoxCollider::GetBounds() const {
        if (!m_Node) return BBox<3, float>();

        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        Vec3 halfExtents = GetHalfExtents();

        return BBox<3, float>(worldCenter - halfExtents, worldCenter + halfExtents);
    }

    bool BoxCollider::TestPoint(const Vec3& point) const {
        if (!m_Node) return false;

        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        Vec3 halfExtents = GetHalfExtents();

        Vec3 localPoint = point - worldCenter;

        return Abs(localPoint.x) <= halfExtents.x &&
               Abs(localPoint.y) <= halfExtents.y &&
               Abs(localPoint.z) <= halfExtents.z;
    }

    bool BoxCollider::Intersects(const Collider* other, ContactPoint& contact) const {
        switch (other->GetType()) {
            case ColliderType::Box:
                return IntersectsBox(static_cast<const BoxCollider*>(other), contact);
            case ColliderType::Sphere:
                return IntersectsSphere(static_cast<const SphereCollider*>(other), contact);
            case ColliderType::Capsule:
                return IntersectsCapsule(static_cast<const CapsuleCollider*>(other), contact);
            default:
                return false;
        }
    }

    bool BoxCollider::IntersectsBox(const BoxCollider* other, ContactPoint& contact) const {
        if (!m_Node || !other->GetNode()) return false;

        Vec3 centerA = m_Node->GetGlobalPosition() + m_Center;
        Vec3 centerB = other->GetNode()->GetGlobalPosition() + other->GetCenter();
        Vec3 halfA = GetHalfExtents();
        Vec3 halfB = other->GetHalfExtents();

        Vec3 delta = centerB - centerA;

        // AABB intersection test
        float overlapX = (halfA.x + halfB.x) - Abs(delta.x);
        float overlapY = (halfA.y + halfB.y) - Abs(delta.y);
        float overlapZ = (halfA.z + halfB.z) - Abs(delta.z);

        if (overlapX > 0 && overlapY > 0 && overlapZ > 0) {
            // Find axis of minimum penetration
            float minOverlap = overlapX;
            Vec3 normal = Vec3(delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);

            if (overlapY < minOverlap) {
                minOverlap = overlapY;
                normal = Vec3(0.0f, delta.y > 0 ? 1.0f : -1.0f, 0.0f);
            }

            if (overlapZ < minOverlap) {
                minOverlap = overlapZ;
                normal = Vec3(0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f);
            }

            contact.point = centerA + delta * 0.5f;
            contact.normal = normal;
            contact.penetration = minOverlap;
            contact.thisCollider = const_cast<BoxCollider*>(this);
            contact.otherCollider = const_cast<BoxCollider*>(other);
            return true;
        }

        return false;
    }

    bool BoxCollider::IntersectsSphere(const SphereCollider* other, ContactPoint& contact) const {
        if (!m_Node || !other->GetNode()) return false;

        Vec3 boxCenter = m_Node->GetGlobalPosition() + m_Center;
        Vec3 sphereCenter = other->GetNode()->GetGlobalPosition() + other->GetCenter();
        Vec3 halfExtents = GetHalfExtents();

        // Find closest point on box to sphere center
        Vec3 closest = sphereCenter - boxCenter;
        closest.x = Clamp(closest.x, -halfExtents.x, halfExtents.x);
        closest.y = Clamp(closest.y, -halfExtents.y, halfExtents.y);
        closest.z = Clamp(closest.z, -halfExtents.z, halfExtents.z);
        closest += boxCenter;

        Vec3 delta = sphereCenter - closest;
        float distanceSquared = LengthSqr(delta);
        float radius = other->GetRadius();

        if (distanceSquared < radius * radius) {
            float distance = Sqrt(distanceSquared);
            contact.point = closest;
            contact.normal = distance > 0.0001f ? delta / distance : Vec3(0.0f, 1.0f, 0.0f);
            contact.penetration = radius - distance;
            contact.thisCollider = const_cast<BoxCollider*>(this);
            contact.otherCollider = const_cast<SphereCollider*>(other);
            return true;
        }

        return false;
    }

    bool BoxCollider::IntersectsCapsule(const CapsuleCollider* other, ContactPoint& contact) const {
        // Simplified capsule-box collision (treat as sphere-box with line segment)
        // For a full implementation, you would test the line segment against the box
        return other->IntersectsBox(this, contact);
    }

    void BoxCollider::DebugDraw() const {
        // TODO: Implement debug drawing
    }
}
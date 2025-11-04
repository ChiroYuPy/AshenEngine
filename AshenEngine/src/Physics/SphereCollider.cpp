#include "Ashen/Physics/SphereCollider.h"
#include "Ashen/Physics/BoxCollider.h"
#include "Ashen/Physics/CapsuleCollider.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Math.h"

namespace ash {
    SphereCollider::SphereCollider(float radius)
        : Collider(ColliderType::Sphere)
        , m_Radius(Max(radius, 0.001f)) {
    }

    BBox<3, float> SphereCollider::GetBounds() const {
        if (!m_Node) return BBox<3, float>();

        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        Vec3 extents = Vec3(m_Radius);

        return BBox<3, float>(worldCenter - extents, worldCenter + extents);
    }

    bool SphereCollider::TestPoint(const Vec3& point) const {
        if (!m_Node) return false;

        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        float distanceSquared = LengthSqr(point - worldCenter);

        return distanceSquared <= m_Radius * m_Radius;
    }

    bool SphereCollider::Intersects(const Collider* other, ContactPoint& contact) const {
        switch (other->GetType()) {
            case ColliderType::Sphere:
                return IntersectsSphere(static_cast<const SphereCollider*>(other), contact);
            case ColliderType::Box:
                return IntersectsBox(static_cast<const BoxCollider*>(other), contact);
            case ColliderType::Capsule:
                return IntersectsCapsule(static_cast<const CapsuleCollider*>(other), contact);
            default:
                return false;
        }
    }

    bool SphereCollider::IntersectsSphere(const SphereCollider* other, ContactPoint& contact) const {
        if (!m_Node || !other->GetNode()) return false;

        Vec3 centerA = m_Node->GetGlobalPosition() + m_Center;
        Vec3 centerB = other->GetNode()->GetGlobalPosition() + other->GetCenter();

        Vec3 delta = centerB - centerA;
        float distanceSquared = LengthSqr(delta);
        float radiusSum = m_Radius + other->GetRadius();

        if (distanceSquared < radiusSum * radiusSum) {
            float distance = Sqrt(distanceSquared);
            Vec3 normal = distance > 0.0001f ? delta / distance : Vec3(0.0f, 1.0f, 0.0f);

            contact.point = centerA + normal * m_Radius;
            contact.normal = normal;
            contact.penetration = radiusSum - distance;
            contact.thisCollider = const_cast<SphereCollider*>(this);
            contact.otherCollider = const_cast<SphereCollider*>(other);
            return true;
        }

        return false;
    }

    bool SphereCollider::IntersectsBox(const BoxCollider* other, ContactPoint& contact) const {
        // Use box-sphere collision (already implemented)
        bool result = other->IntersectsSphere(this, contact);
        if (result) {
            // Flip normal since we're calling from sphere's perspective
            contact.normal = -contact.normal;
            contact.thisCollider = const_cast<SphereCollider*>(this);
            contact.otherCollider = const_cast<BoxCollider*>(other);
        }
        return result;
    }

    bool SphereCollider::IntersectsCapsule(const CapsuleCollider* other, ContactPoint& contact) const {
        return other->IntersectsSphere(this, contact);
    }

    void SphereCollider::DebugDraw() const {
        // TODO: Implement debug drawing
    }
}
#include "Ashen/Physics/CapsuleCollider.h"
#include "Ashen/Physics/SphereCollider.h"
#include "Ashen/Physics/BoxCollider.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Math.h"

namespace ash {
    CapsuleCollider::CapsuleCollider(float radius, float height)
        : Collider(ColliderType::Capsule)
        , m_Radius(Max(radius, 0.001f))
        , m_Height(Max(height, m_Radius * 2.0f)) {
    }

    Vec3 CapsuleCollider::GetTopSphereCenter() const {
        if (!m_Node) return Vec3(0.0f);

        float halfHeight = (m_Height - m_Radius * 2.0f) * 0.5f;
        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        return worldCenter + Vec3(0.0f, halfHeight, 0.0f);
    }

    Vec3 CapsuleCollider::GetBottomSphereCenter() const {
        if (!m_Node) return Vec3(0.0f);

        float halfHeight = (m_Height - m_Radius * 2.0f) * 0.5f;
        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        return worldCenter - Vec3(0.0f, halfHeight, 0.0f);
    }

    BBox<3, float> CapsuleCollider::GetBounds() const {
        if (!m_Node) return BBox<3, float>();

        Vec3 worldCenter = m_Node->GetGlobalPosition() + m_Center;
        float halfHeight = m_Height * 0.5f;
        Vec3 extents = Vec3(m_Radius, halfHeight, m_Radius);

        return BBox<3, float>(worldCenter - extents, worldCenter + extents);
    }

    bool CapsuleCollider::TestPoint(const Vec3& point) const {
        if (!m_Node) return false;

        Vec3 top = GetTopSphereCenter();
        Vec3 bottom = GetBottomSphereCenter();

        // Find closest point on line segment (capsule axis)
        Vec3 axis = top - bottom;
        float axisLength = Length(axis);

        if (axisLength < 0.0001f) {
            // Degenerate case: capsule is a sphere
            return LengthSqr(point - bottom) <= m_Radius * m_Radius;
        }

        Vec3 axisDir = axis / axisLength;
        Vec3 toPoint = point - bottom;
        float projection = Clamp(Dot(toPoint, axisDir), 0.0f, axisLength);
        Vec3 closestPoint = bottom + axisDir * projection;

        return LengthSqr(point - closestPoint) <= m_Radius * m_Radius;
    }

    bool CapsuleCollider::Intersects(const Collider* other, ContactPoint& contact) const {
        switch (other->GetType()) {
            case ColliderType::Capsule:
                return IntersectsCapsule(static_cast<const CapsuleCollider*>(other), contact);
            case ColliderType::Sphere:
                return IntersectsSphere(static_cast<const SphereCollider*>(other), contact);
            case ColliderType::Box:
                return IntersectsBox(static_cast<const BoxCollider*>(other), contact);
            default:
                return false;
        }
    }

    bool CapsuleCollider::IntersectsCapsule(const CapsuleCollider* other, ContactPoint& contact) const {
        if (!m_Node || !other->GetNode()) return false;

        Vec3 topA = GetTopSphereCenter();
        Vec3 bottomA = GetBottomSphereCenter();
        Vec3 topB = other->GetTopSphereCenter();
        Vec3 bottomB = other->GetBottomSphereCenter();

        // Find closest points between two line segments
        Vec3 axisA = topA - bottomA;
        Vec3 axisB = topB - bottomB;
        Vec3 delta = bottomB - bottomA;

        float a = Dot(axisA, axisA);
        float b = Dot(axisA, axisB);
        float c = Dot(axisB, axisB);
        float d = Dot(axisA, delta);
        float e = Dot(axisB, delta);

        float denom = a * c - b * b;
        float s = 0.0f, t = 0.0f;

        if (denom > 0.0001f) {
            s = Clamp((b * e - c * d) / denom, 0.0f, 1.0f);
            t = Clamp((a * e - b * d) / denom, 0.0f, 1.0f);
        }

        Vec3 closestA = bottomA + axisA * s;
        Vec3 closestB = bottomB + axisB * t;

        Vec3 separation = closestB - closestA;
        float distance = Length(separation);
        float radiusSum = m_Radius + other->GetRadius();

        if (distance < radiusSum) {
            Vec3 normal = distance > 0.0001f ? separation / distance : Vec3(0.0f, 1.0f, 0.0f);

            contact.point = closestA + normal * m_Radius;
            contact.normal = normal;
            contact.penetration = radiusSum - distance;
            contact.thisCollider = const_cast<CapsuleCollider*>(this);
            contact.otherCollider = const_cast<CapsuleCollider*>(other);
            return true;
        }

        return false;
    }

    bool CapsuleCollider::IntersectsSphere(const SphereCollider* other, ContactPoint& contact) const {
        if (!m_Node || !other->GetNode()) return false;

        Vec3 top = GetTopSphereCenter();
        Vec3 bottom = GetBottomSphereCenter();
        Vec3 sphereCenter = other->GetNode()->GetGlobalPosition() + other->GetCenter();

        // Find closest point on capsule axis to sphere center
        Vec3 axis = top - bottom;
        float axisLength = Length(axis);
        Vec3 axisDir = axisLength > 0.0001f ? axis / axisLength : Vec3(0.0f, 1.0f, 0.0f);

        Vec3 toSphere = sphereCenter - bottom;
        float projection = Clamp(Dot(toSphere, axisDir), 0.0f, axisLength);
        Vec3 closestPoint = bottom + axisDir * projection;

        Vec3 delta = sphereCenter - closestPoint;
        float distance = Length(delta);
        float radiusSum = m_Radius + other->GetRadius();

        if (distance < radiusSum) {
            Vec3 normal = distance > 0.0001f ? delta / distance : Vec3(0.0f, 1.0f, 0.0f);

            contact.point = closestPoint + normal * m_Radius;
            contact.normal = normal;
            contact.penetration = radiusSum - distance;
            contact.thisCollider = const_cast<CapsuleCollider*>(this);
            contact.otherCollider = const_cast<SphereCollider*>(other);
            return true;
        }

        return false;
    }

    bool CapsuleCollider::IntersectsBox(const BoxCollider* other, ContactPoint& contact) const {
        // Simplified: test capsule spheres against box
        // For full implementation, you would test line segment against box faces
        return other->IntersectsSphere(nullptr, contact);
    }

    void CapsuleCollider::DebugDraw() const {
        // TODO: Implement debug drawing
    }
}
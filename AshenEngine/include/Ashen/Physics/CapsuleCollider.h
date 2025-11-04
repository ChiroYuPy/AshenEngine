#ifndef ASHEN_CAPSULE_COLLIDER_H
#define ASHEN_CAPSULE_COLLIDER_H

#include "Collider.h"

namespace ash {
    class CapsuleCollider : public Collider {
    public:
        CapsuleCollider(float radius = 0.5f, float height = 2.0f);

        float GetRadius() const { return m_Radius; }
        void SetRadius(float radius) { m_Radius = Max(radius, 0.001f); }

        float GetHeight() const { return m_Height; }
        void SetHeight(float height) { m_Height = Max(height, m_Radius * 2.0f); }

        // Get the two sphere centers that define the capsule
        Vec3 GetTopSphereCenter() const;
        Vec3 GetBottomSphereCenter() const;

        BBox<3, float> GetBounds() const override;
        bool TestPoint(const Vec3& point) const override;
        bool Intersects(const Collider* other, ContactPoint& contact) const override;
        void DebugDraw() const override;

        bool IntersectsCapsule(const CapsuleCollider* other, ContactPoint& contact) const;
        bool IntersectsSphere(const class SphereCollider* other, ContactPoint& contact) const;
        bool IntersectsBox(const class BoxCollider* other, ContactPoint& contact) const;

    private:
        float m_Radius;
        float m_Height;
    };
}

#endif // ASHEN_CAPSULE_COLLIDER_H
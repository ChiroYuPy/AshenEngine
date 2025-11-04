#ifndef ASHEN_SPHERE_COLLIDER_H
#define ASHEN_SPHERE_COLLIDER_H

#include "Collider.h"

namespace ash {
    class SphereCollider : public Collider {
    public:
        SphereCollider(float radius = 0.5f);

        float GetRadius() const { return m_Radius; }
        void SetRadius(float radius) { m_Radius = Max(radius, 0.001f); }

        BBox<3, float> GetBounds() const override;
        bool TestPoint(const Vec3& point) const override;
        bool Intersects(const Collider* other, ContactPoint& contact) const override;
        void DebugDraw() const override;

        bool IntersectsSphere(const SphereCollider* other, ContactPoint& contact) const;
        bool IntersectsBox(const class BoxCollider* other, ContactPoint& contact) const;
        bool IntersectsCapsule(const class CapsuleCollider* other, ContactPoint& contact) const;

    private:
        float m_Radius;
    };
}

#endif // ASHEN_SPHERE_COLLIDER_H
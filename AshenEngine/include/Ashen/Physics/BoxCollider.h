#ifndef ASHEN_BOX_COLLIDER_H
#define ASHEN_BOX_COLLIDER_H

#include "Collider.h"

namespace ash {
    class BoxCollider : public Collider {
    public:
        BoxCollider(const Vec3& size = Vec3(1.0f));

        Vec3 GetSize() const { return m_Size; }
        void SetSize(const Vec3& size) { m_Size = size; }

        Vec3 GetHalfExtents() const { return m_Size * 0.5f; }

        BBox<3, float> GetBounds() const override;
        bool TestPoint(const Vec3& point) const override;
        bool Intersects(const Collider* other, ContactPoint& contact) const override;
        void DebugDraw() const override;

        bool IntersectsBox(const BoxCollider* other, ContactPoint& contact) const;
        bool IntersectsSphere(const class SphereCollider* other, ContactPoint& contact) const;
        bool IntersectsCapsule(const class CapsuleCollider* other, ContactPoint& contact) const;

    private:
        Vec3 m_Size;
    };
}

#endif // ASHEN_BOX_COLLIDER_H
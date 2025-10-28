#ifndef ASHEN_FRUSTUM_H
#define ASHEN_FRUSTUM_H
#include "Ashen/Math/Math.h"

namespace ash {

    struct Frustum {
        enum Plane { Left = 0, Right, Bottom, Top, Near, Far, Count };

        Vec4 planes[6];

        void ExtractFromViewProjection(const Mat4& vp);
        bool ContainsPoint(const Vec3& point) const;
        bool IntersectsSphere(const Vec3& center, float radius) const;
        bool IntersectsAABB(const Vec3& min, const Vec3& max) const;
    };

}

#endif //ASHEN_FRUSTUM_H
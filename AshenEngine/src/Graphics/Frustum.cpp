#include "../../include/Ashen/Graphics/Frustum.h"

#include <algorithm>

namespace ash {
    void Frustum::ExtractFromViewProjection(const Mat4& vp) {
        const float* m = glm::value_ptr(vp);

        planes[Left] = Vec4(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        planes[Right] = Vec4(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        planes[Bottom] = Vec4(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        planes[Top] = Vec4(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        planes[Near] = Vec4(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        planes[Far] = Vec4(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);

        for (auto& plane : planes) {
            const float length = glm::length(Vec3(plane));
            plane /= length;
        }
    }

    bool Frustum::ContainsPoint(const Vec3& point) const {
        return std::ranges::all_of(planes, [&](const Vec4& plane) {
            return glm::dot(Vec3(plane), point) + plane.w >= 0.0f;
        });
    }

    bool Frustum::IntersectsSphere(const Vec3& center, const float radius) const {
        return std::ranges::all_of(planes, [&](const Vec4& plane) {
            const float distance = glm::dot(Vec3(plane), center) + plane.w;
            return distance >= -radius;
        });
    }

    bool Frustum::IntersectsAABB(const Vec3& min, const Vec3& max) const {
        return std::ranges::all_of(planes, [&](const Vec4& plane) {
            const Vec3 positive(
                plane.x > 0 ? max.x : min.x,
                plane.y > 0 ? max.y : min.y,
                plane.z > 0 ? max.z : min.z
            );
            return glm::dot(Vec3(plane), positive) + plane.w >= 0.0f;
        });
    }
}

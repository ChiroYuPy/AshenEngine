#include "Ashen/Graphics/Camera/Frustum.h"

namespace ash {
    void Frustum::ExtractFromViewProjection(const Mat4& vp) {
        const float* m = glm::value_ptr(vp);

        planes[Left] = Vec4(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        planes[Right] = Vec4(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        planes[Bottom] = Vec4(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        planes[Top] = Vec4(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        planes[Near] = Vec4(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        planes[Far] = Vec4(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);

        for (int i = 0; i < Count; ++i) {
            const float length = glm::length(Vec3(planes[i]));
            planes[i] /= length;
        }
    }

    bool Frustum::ContainsPoint(const Vec3& point) const {
        for (int i = 0; i < Count; ++i) {
            if (glm::dot(Vec3(planes[i]), point) + planes[i].w < 0.0f)
                return false;
        }
        return true;
    }

    bool Frustum::IntersectsSphere(const Vec3& center, const float radius) const {
        for (int i = 0; i < Count; ++i) {
            const float distance = glm::dot(Vec3(planes[i]), center) + planes[i].w;
            if (distance < -radius)
                return false;
        }
        return true;
    }

    bool Frustum::IntersectsAABB(const Vec3& min, const Vec3& max) const {
        for (int i = 0; i < Count; ++i) {
            Vec3 positive(
                planes[i].x > 0 ? max.x : min.x,
                planes[i].y > 0 ? max.y : min.y,
                planes[i].z > 0 ? max.z : min.z
            );

            if (glm::dot(Vec3(planes[i]), positive) + planes[i].w < 0)
                return false;
        }
        return true;
    }
}

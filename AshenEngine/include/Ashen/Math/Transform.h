#ifndef ASHEN_TRANSFORM_H
#define ASHEN_TRANSFORM_H

#include "Ashen/Math/Math.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ash {
    class Transform {
    public:
        Vec3 position{0.0f};
        Vec3 rotation{0.0f};  // Radians (pitch, yaw, roll)
        Vec3 scale{1.0f};

        Transform() = default;
        explicit Transform(const Vec3& pos) : position(pos) {}

        [[nodiscard]] Mat4 GetMatrix() const {
            auto m = glm::identity<Mat4>();

            // Translation
            m = glm::translate(m, position);

            // Rotation (Euler angles to quaternion)
            const auto q = glm::quat(rotation);
            m = m * glm::toMat4(q);

            // Scale
            m = glm::scale(m, scale);

            return m;
        }

        [[nodiscard]] Vec3 Forward() const {
            const auto q = glm::quat(rotation);
            return glm::normalize(q * Vec3(0, 0, -1));
        }

        [[nodiscard]] Vec3 Right() const {
            const auto q = glm::quat(rotation);
            return glm::normalize(q * Vec3(1, 0, 0));
        }

        [[nodiscard]] Vec3 Up() const {
            const auto q = glm::quat(rotation);
            return glm::normalize(q * Vec3(0, 1, 0));
        }
    };
}

#endif //ASHEN_TRANSFORM_H
#ifndef ASHEN_COMPONENT_H
#define ASHEN_COMPONENT_H

#include "Ashen/Math/Math.h"
#include "Ashen/Core/Types.h"
#include "Ashen/GraphicsAPI/Texture.h"

namespace ash {

    struct Component final {
        ~Component() = default;
    };

    struct TransformComponent {
        Vec3 Position = Vec3(0.0f);
        Vec3 Rotation = Vec3(0.0f);
        Vec3 Scale = Vec3(1.0f);

        TransformComponent() = default;
        explicit TransformComponent(const Vec3& pos) : Position(pos) {}

        Mat4 GetTransform() const {
            const Mat4 rotation = glm::rotate(Mat4(1.0f), Rotation.x, Vec3(1, 0, 0))
                          * glm::rotate(Mat4(1.0f), Rotation.y, Vec3(0, 1, 0))
                          * glm::rotate(Mat4(1.0f), Rotation.z, Vec3(0, 0, 1));

            return glm::translate(Mat4(1.0f), Position)
                 * rotation
                 * glm::scale(Mat4(1.0f), Scale);
        }
    };

    struct TagComponent {
        String Tag;

        TagComponent() = default;
        explicit TagComponent(String tag) : Tag(MovePtr(tag)) {}
    };

    struct SpriteRendererComponent {
        Vec4 Color = Vec4(1.0f);
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        explicit SpriteRendererComponent(const Vec4& color) : Color(color) {}
    };

} // namespace ash

#endif // ASHEN_COMPONENT_H
#ifndef ASHEN_NODE2D_H
#define ASHEN_NODE2D_H

#include "CanvasItem.h"
#include "Ashen/Math/Transform.h"

namespace ash {
    class Node2D : public CanvasItem {
    public:
        explicit Node2D(String name = "Node2D");

        Transform2D local_transform;

        Transform2D GetGlobalTransform() const;

        void SetGlobalTransform(const Transform2D &transform);

        Vec2 GetPosition() const;

        void SetPosition(const Vec2 &pos);

        void SetPosition(float x, float y);

        Vec2 GetGlobalPosition() const;

        void SetGlobalPosition(const Vec2 &pos);

        float GetRotation() const;

        void SetRotation(float radians);

        void SetRotationDegrees(float degrees);

        float GetRotationDegrees() const;

        float GetGlobalRotation() const;

        Vec2 GetScale() const;

        void SetScale(const Vec2 &scl);

        void SetScale(float sx, float sy);

        void Translate(const Vec2 &offset);

        void Rotate(float radians);

        void ScaleBy(float factor);

        Vec2 GetRight() const;

        Vec2 GetUp() const;

        Vec2 ToLocal(const Vec2 &global_point) const;

        Vec2 ToGlobal(const Vec2 &local_point) const;
    };
}

#endif // ASHEN_NODE2D_H

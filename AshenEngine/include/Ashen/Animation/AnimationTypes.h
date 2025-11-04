#ifndef ASHEN_ANIMATION_TYPES_H
#define ASHEN_ANIMATION_TYPES_H

#include "Ashen/Core/Types.h"

namespace ash {
    enum class AnimationLoopMode {
        None,      // Play once
        Loop,      // Loop indefinitely
        PingPong   // Play forward then backward
    };

    enum class AnimationBlendMode {
        Blend,     // Blend with previous animation
        Additive   // Add to previous animation
    };

    enum class InterpolationMode {
        Linear,
        Cubic,
        Step       // No interpolation
    };

    enum class AnimationPropertyType {
        Position,
        Rotation,
        Scale,
        Color,
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    template<typename T>
    struct Keyframe {
        float time;
        T value;
        InterpolationMode interpolation = InterpolationMode::Linear;

        // Tangents for cubic interpolation
        T inTangent;
        T outTangent;

        Keyframe() : time(0.0f) {}
        Keyframe(float t, const T& v) : time(t), value(v) {}
    };

    struct AnimationEvent {
        float time;
        String name;
        String stringParameter;
        float floatParameter;
        int intParameter;
    };
}

#endif // ASHEN_ANIMATION_TYPES_H
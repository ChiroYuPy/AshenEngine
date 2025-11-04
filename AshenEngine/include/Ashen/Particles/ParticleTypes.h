#ifndef ASHEN_PARTICLE_TYPES_H
#define ASHEN_PARTICLE_TYPES_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    struct Particle {
        Vec3 position;
        Vec3 velocity;
        Vec3 acceleration;

        Vec4 color;
        Vec4 startColor;
        Vec4 endColor;

        float size;
        float startSize;
        float endSize;

        float rotation;
        float angularVelocity;

        float lifetime;
        float age;

        bool active;

        Particle()
            : position(0.0f)
            , velocity(0.0f)
            , acceleration(0.0f)
            , color(1.0f)
            , startColor(1.0f)
            , endColor(1.0f)
            , size(1.0f)
            , startSize(1.0f)
            , endSize(1.0f)
            , rotation(0.0f)
            , angularVelocity(0.0f)
            , lifetime(1.0f)
            , age(0.0f)
            , active(false) {
        }
    };

    enum class ParticleEmitterShape {
        Point,
        Sphere,
        Box,
        Cone,
        Circle
    };

    enum class ParticleSimulationSpace {
        Local,  // Particles move relative to emitter
        World   // Particles move in world space
    };

    struct ParticleEmitterConfig {
        // Emission
        float emissionRate = 10.0f;           // Particles per second
        int maxParticles = 100;
        float duration = 5.0f;                // -1 for infinite
        bool looping = true;
        float startDelay = 0.0f;

        // Lifetime
        float lifetimeMin = 1.0f;
        float lifetimeMax = 3.0f;

        // Velocity
        Vec3 startVelocityMin = Vec3(-1.0f, 1.0f, -1.0f);
        Vec3 startVelocityMax = Vec3(1.0f, 3.0f, 1.0f);

        // Size
        float startSizeMin = 0.1f;
        float startSizeMax = 0.5f;
        float endSizeMin = 0.0f;
        float endSizeMax = 0.1f;

        // Color
        Vec4 startColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Vec4 endColor = Vec4(1.0f, 1.0f, 1.0f, 0.0f);

        // Rotation
        float startRotationMin = 0.0f;
        float startRotationMax = 0.0f;
        float angularVelocityMin = 0.0f;
        float angularVelocityMax = 0.0f;

        // Gravity
        Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f);

        // Shape
        ParticleEmitterShape shape = ParticleEmitterShape::Sphere;
        float shapeRadius = 1.0f;
        Vec3 shapeSize = Vec3(1.0f);
        float coneAngle = 25.0f;

        // Simulation
        ParticleSimulationSpace simulationSpace = ParticleSimulationSpace::World;
    };
}

#endif // ASHEN_PARTICLE_TYPES_H

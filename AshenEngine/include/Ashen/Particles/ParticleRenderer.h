#ifndef ASHEN_PARTICLE_RENDERER_H
#define ASHEN_PARTICLE_RENDERER_H

#include "ParticleSystem.h"

namespace ash {
    class Camera;

    class ParticleRenderer {
    public:
        static void RenderParticles(ParticleSystem* system, const Camera& camera);
        static void RenderParticlesBatched(const Vector<ParticleSystem*>& systems, const Camera& camera);

    private:
        static Mat4 CalculateBillboardMatrix(const Vec3& position, const Vec3& cameraPos, const Vec3& cameraUp);
    };
}

#endif // ASHEN_PARTICLE_RENDERER_H
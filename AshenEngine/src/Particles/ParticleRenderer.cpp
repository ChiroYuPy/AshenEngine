#include "Ashen/Particles/ParticleRenderer.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Graphics/Cameras/Camera.h"
#include "Ashen/Math/Math.h"

namespace ash {
    void ParticleRenderer::RenderParticles(ParticleSystem* system, const Camera& camera) {
        if (!system) return;

        const auto& particles = system->GetParticles();
        Ref<Texture2D> texture = system->GetTexture();
        bool billboard = system->GetBillboard();

        Vec3 cameraPos = camera.GetPosition();
        Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f); // Could get from camera

        for (const auto& particle : particles) {
            if (!particle.active) continue;

            Vec3 position = particle.position;

            if (texture) {
                // With texture
                if (billboard) {
                    Renderer2D::DrawQuad(position, Vec2(particle.size), texture, particle.color);
                } else {
                    // Build transform matrix with rotation
                    Mat4 transform = Translate(Identity(), position);
                    transform = Rotate(transform, particle.rotation, Vec3(0.0f, 0.0f, 1.0f));
                    transform = Scale(transform, Vec3(particle.size, particle.size, 1.0f));
                    Renderer2D::DrawQuad(transform, texture, particle.color);
                }
            } else {
                // Without texture
                if (billboard) {
                    Renderer2D::DrawQuad(position, Vec2(particle.size), particle.color);
                } else {
                    // Build transform matrix with rotation
                    Mat4 transform = Translate(Identity(), position);
                    transform = Rotate(transform, particle.rotation, Vec3(0.0f, 0.0f, 1.0f));
                    transform = Scale(transform, Vec3(particle.size, particle.size, 1.0f));
                    Renderer2D::DrawQuad(transform, particle.color);
                }
            }
        }
    }

    void ParticleRenderer::RenderParticlesBatched(const Vector<ParticleSystem*>& systems, const Camera& camera) {
        for (auto* system : systems) {
            RenderParticles(system, camera);
        }
    }

    Mat4 ParticleRenderer::CalculateBillboardMatrix(const Vec3& position, const Vec3& cameraPos, const Vec3& cameraUp) {
        Vec3 forward = Normalize(cameraPos - position);
        Vec3 right = Normalize(Cross(cameraUp, forward));
        Vec3 up = Cross(forward, right);

        Mat4 matrix(1.0f);
        matrix[0] = Vec4(right.x, right.y, right.z, 0.0f);
        matrix[1] = Vec4(up.x, up.y, up.z, 0.0f);
        matrix[2] = Vec4(forward.x, forward.y, forward.z, 0.0f);
        matrix[3] = Vec4(position.x, position.y, position.z, 1.0f);

        return matrix;
    }
}
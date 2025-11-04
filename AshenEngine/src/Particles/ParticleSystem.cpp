#include "Ashen/Particles/ParticleSystem.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Math/Math.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    ParticleSystem::ParticleSystem(Node3D* node)
        : m_Node(node) {
        m_Particles.resize(m_Config.maxParticles);
    }

    ParticleSystem::~ParticleSystem() = default;

    void ParticleSystem::Play() {
        m_IsPlaying = true;
        m_IsPaused = false;
        m_Time = 0.0f;
    }

    void ParticleSystem::Pause() {
        m_IsPaused = true;
    }

    void ParticleSystem::Stop() {
        m_IsPlaying = false;
        m_IsPaused = false;
        m_Time = 0.0f;
        Clear();
    }

    void ParticleSystem::Clear() {
        for (auto& particle : m_Particles) {
            particle.active = false;
        }
    }

    void ParticleSystem::Emit(int count) {
        for (int i = 0; i < count; ++i) {
            EmitParticle();
        }
    }

    void ParticleSystem::SetMaxParticles(int max) {
        m_Config.maxParticles = Max(max, 1);
        m_Particles.resize(m_Config.maxParticles);
    }

    int ParticleSystem::GetActiveParticleCount() const {
        int count = 0;
        for (const auto& particle : m_Particles) {
            if (particle.active) count++;
        }
        return count;
    }

    void ParticleSystem::Update(float deltaTime) {
        if (!m_IsPlaying || m_IsPaused) return;

        m_Time += deltaTime;

        // Check if should stop (non-looping)
        if (m_Config.duration > 0.0f && !m_Config.looping && m_Time >= m_Config.duration) {
            m_IsPlaying = false;
            // Continue updating existing particles
        }

        // Emit particles
        if (m_IsPlaying && m_Time >= m_Config.startDelay) {
            if (m_Config.duration < 0.0f || m_Config.looping || m_Time < m_Config.duration) {
                m_EmissionAccumulator += m_Config.emissionRate * deltaTime;

                int particlesToEmit = static_cast<int>(m_EmissionAccumulator);
                m_EmissionAccumulator -= particlesToEmit;

                for (int i = 0; i < particlesToEmit; ++i) {
                    EmitParticle();
                }
            }
        }

        // Update particles
        for (auto& particle : m_Particles) {
            if (particle.active) {
                UpdateParticle(particle, deltaTime);
            }
        }
    }

    void ParticleSystem::EmitParticle() {
        // Find inactive particle
        for (auto& particle : m_Particles) {
            if (!particle.active) {
                particle.active = true;
                particle.age = 0.0f;

                // Lifetime
                particle.lifetime = m_Random.Float(m_Config.lifetimeMin, m_Config.lifetimeMax);

                // Position
                particle.position = GetEmissionPosition();

                // Velocity
                particle.velocity = GetEmissionVelocity();
                particle.acceleration = Vec3(0.0f);

                // Size
                particle.startSize = m_Random.Float(m_Config.startSizeMin, m_Config.startSizeMax);
                particle.endSize = m_Random.Float(m_Config.endSizeMin, m_Config.endSizeMax);
                particle.size = particle.startSize;

                // Color
                particle.startColor = m_Config.startColor;
                particle.endColor = m_Config.endColor;
                particle.color = particle.startColor;

                // Rotation
                particle.rotation = m_Random.Float(m_Config.startRotationMin, m_Config.startRotationMax);
                particle.angularVelocity = m_Random.Float(m_Config.angularVelocityMin, m_Config.angularVelocityMax);

                break;
            }
        }
    }

    void ParticleSystem::UpdateParticle(Particle& particle, float deltaTime) {
        particle.age += deltaTime;

        if (particle.age >= particle.lifetime) {
            particle.active = false;
            return;
        }

        // Calculate life ratio (0 to 1)
        float lifeRatio = particle.age / particle.lifetime;

        // Update velocity with gravity
        particle.velocity += m_Config.gravity * deltaTime;

        // Update position
        if (m_Config.simulationSpace == ParticleSimulationSpace::Local && m_Node) {
            // In local space, position is relative to node
            particle.position += particle.velocity * deltaTime;
        } else {
            // In world space
            particle.position += particle.velocity * deltaTime;
        }

        // Update size
        particle.size = Lerp(particle.startSize, particle.endSize, lifeRatio);

        // Update color
        particle.color.x = Lerp(particle.startColor.x, particle.endColor.x, lifeRatio);
        particle.color.y = Lerp(particle.startColor.y, particle.endColor.y, lifeRatio);
        particle.color.z = Lerp(particle.startColor.z, particle.endColor.z, lifeRatio);
        particle.color.w = Lerp(particle.startColor.w, particle.endColor.w, lifeRatio);

        // Update rotation
        particle.rotation += particle.angularVelocity * deltaTime;
    }

    Vec3 ParticleSystem::GetEmissionPosition() const {
        Vec3 basePos = m_Node ? m_Node->GetGlobalPosition() : Vec3(0.0f);

        switch (m_Config.shape) {
            case ParticleEmitterShape::Point:
                return basePos;

            case ParticleEmitterShape::Sphere: {
                // Generate random point in sphere using spherical coordinates
                float theta = m_Random.Float(0.0f, TWO_PI);
                float phi = m_Random.Float(0.0f, PI);
                float r = m_Random.Float(0.0f, m_Config.shapeRadius);
                Vec3 randomDir(
                    Sin(phi) * Cos(theta),
                    Sin(phi) * Sin(theta),
                    Cos(phi)
                );
                return basePos + randomDir * r;
            }

            case ParticleEmitterShape::Box: {
                Vec3 offset;
                offset.x = m_Random.Float(-m_Config.shapeSize.x * 0.5f, m_Config.shapeSize.x * 0.5f);
                offset.y = m_Random.Float(-m_Config.shapeSize.y * 0.5f, m_Config.shapeSize.y * 0.5f);
                offset.z = m_Random.Float(-m_Config.shapeSize.z * 0.5f, m_Config.shapeSize.z * 0.5f);
                return basePos + offset;
            }

            case ParticleEmitterShape::Circle: {
                float angle = m_Random.Float(0.0f, TWO_PI);
                float radius = m_Random.Float(0.0f, m_Config.shapeRadius);
                Vec3 offset(Cos(angle) * radius, 0.0f, Sin(angle) * radius);
                return basePos + offset;
            }

            case ParticleEmitterShape::Cone: {
                float angle = m_Random.Float(0.0f, TWO_PI);
                float coneAngleRad = ToRadians(m_Config.coneAngle);
                float height = m_Random.Float(0.0f, 1.0f);
                float radius = Tan(coneAngleRad) * height;

                Vec3 offset;
                offset.x = Cos(angle) * radius;
                offset.y = height;
                offset.z = Sin(angle) * radius;

                return basePos + offset;
            }

            default:
                return basePos;
        }
    }

    Vec3 ParticleSystem::GetEmissionVelocity() const {
        Vec3 velocity;
        velocity.x = m_Random.Float(m_Config.startVelocityMin.x, m_Config.startVelocityMax.x);
        velocity.y = m_Random.Float(m_Config.startVelocityMin.y, m_Config.startVelocityMax.y);
        velocity.z = m_Random.Float(m_Config.startVelocityMin.z, m_Config.startVelocityMax.z);
        return velocity;
    }
}
#ifndef ASHEN_PARTICLE_SYSTEM_H
#define ASHEN_PARTICLE_SYSTEM_H

#include "ParticleTypes.h"
#include "Ashen/Math/Random.h"

namespace ash {
    class Node3D;
    class Texture2D;

    class ParticleSystem {
    public:
        ParticleSystem(Node3D* node = nullptr);
        ~ParticleSystem();

        // Configuration
        ParticleEmitterConfig& GetConfig() { return m_Config; }
        const ParticleEmitterConfig& GetConfig() const { return m_Config; }
        void SetConfig(const ParticleEmitterConfig& config) { m_Config = config; }

        // Control
        void Play();
        void Pause();
        void Stop();
        void Clear();

        bool IsPlaying() const { return m_IsPlaying; }
        bool IsPaused() const { return m_IsPaused; }

        // Emission
        void Emit(int count);
        void SetEmissionRate(float rate) { m_Config.emissionRate = rate; }
        float GetEmissionRate() const { return m_Config.emissionRate; }

        // Particles
        const Vector<Particle>& GetParticles() const { return m_Particles; }
        int GetActiveParticleCount() const;
        int GetMaxParticles() const { return m_Config.maxParticles; }
        void SetMaxParticles(int max);

        // Update
        void Update(float deltaTime);

        // Rendering properties
        void SetTexture(Ref<Texture2D> texture) { m_Texture = texture; }
        Ref<Texture2D> GetTexture() const { return m_Texture; }

        bool GetBillboard() const { return m_Billboard; }
        void SetBillboard(bool billboard) { m_Billboard = billboard; }

        // Attached node
        Node3D* GetNode() const { return m_Node; }
        void SetNode(Node3D* node) { m_Node = node; }

    private:
        Node3D* m_Node;
        ParticleEmitterConfig m_Config;
        Vector<Particle> m_Particles;
        Ref<Texture2D> m_Texture;

        bool m_IsPlaying = false;
        bool m_IsPaused = false;
        float m_Time = 0.0f;
        float m_EmissionAccumulator = 0.0f;
        bool m_Billboard = true;

        Random m_Random;

        void EmitParticle();
        void UpdateParticle(Particle& particle, float deltaTime);
        Vec3 GetEmissionPosition() const;
        Vec3 GetEmissionVelocity() const;
    };
}

#endif // ASHEN_PARTICLE_SYSTEM_H
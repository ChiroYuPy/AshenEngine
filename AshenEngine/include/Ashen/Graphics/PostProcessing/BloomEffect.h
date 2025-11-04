#ifndef ASHEN_BLOOM_EFFECT_H
#define ASHEN_BLOOM_EFFECT_H

#include "PostProcessEffect.h"

namespace ash {
    class BloomEffect : public PostProcessEffect {
    public:
        BloomEffect();
        ~BloomEffect() override;

        void Apply(FrameBuffer* input, FrameBuffer* output) override;

        float GetThreshold() const { return m_Threshold; }
        void SetThreshold(float threshold) { m_Threshold = threshold; }

        float GetIntensity() const { return m_Intensity; }
        void SetIntensity(float intensity) { m_Intensity = intensity; }

        int GetIterations() const { return m_Iterations; }
        void SetIterations(int iterations) { m_Iterations = Math::Clamp(iterations, 1, 10); }

    private:
        float m_Threshold = 1.0f;
        float m_Intensity = 0.5f;
        int m_Iterations = 5;

        Own<ShaderProgram> m_BrightPassShader;
        Own<ShaderProgram> m_BlurShader;
        Own<ShaderProgram> m_CompositeShader;
        Vector<Own<FrameBuffer>> m_BlurBuffers;
    };
}

#endif // ASHEN_BLOOM_EFFECT_H
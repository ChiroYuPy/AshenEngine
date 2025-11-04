#ifndef ASHEN_TONEMAPPING_EFFECT_H
#define ASHEN_TONEMAPPING_EFFECT_H

#include "PostProcessEffect.h"

namespace ash {
    enum class TonemapOperator {
        None,
        Reinhard,
        Filmic,
        ACES,
        Uncharted2
    };

    class TonemappingEffect : public PostProcessEffect {
    public:
        TonemappingEffect();
        ~TonemappingEffect() override;

        void Apply(FrameBuffer* input, FrameBuffer* output) override;

        TonemapOperator GetOperator() const { return m_Operator; }
        void SetOperator(TonemapOperator op) { m_Operator = op; }

        float GetExposure() const { return m_Exposure; }
        void SetExposure(float exposure) { m_Exposure = Math::Max(exposure, 0.0f); }

        float GetGamma() const { return m_Gamma; }
        void SetGamma(float gamma) { m_Gamma = Math::Max(gamma, 0.1f); }

    private:
        TonemapOperator m_Operator = TonemapOperator::ACES;
        float m_Exposure = 1.0f;
        float m_Gamma = 2.2f;

        Own<ShaderProgram> m_TonemapShader;
    };
}

#endif // ASHEN_TONEMAPPING_EFFECT_H
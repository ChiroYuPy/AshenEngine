#ifndef ASHEN_POST_PROCESS_EFFECT_H
#define ASHEN_POST_PROCESS_EFFECT_H

#include "Ashen/Core/Types.h"

namespace ash {
    class FrameBuffer;
    class ShaderProgram;

    class PostProcessEffect {
    public:
        virtual ~PostProcessEffect() = default;

        virtual void Apply(FrameBuffer* input, FrameBuffer* output) = 0;

        bool IsEnabled() const { return m_Enabled; }
        void SetEnabled(bool enabled) { m_Enabled = enabled; }

    protected:
        bool m_Enabled = true;
    };
}

#endif // ASHEN_POST_PROCESS_EFFECT_H
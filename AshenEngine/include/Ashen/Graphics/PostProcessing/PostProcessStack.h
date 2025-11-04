#ifndef ASHEN_POST_PROCESS_STACK_H
#define ASHEN_POST_PROCESS_STACK_H

#include "PostProcessEffect.h"
#include "Ashen/Core/Types.h"

namespace ash {
    class FrameBuffer;

    class PostProcessStack {
    public:
        PostProcessStack();
        ~PostProcessStack();

        void AddEffect(Own<PostProcessEffect> effect);
        void RemoveEffect(size_t index);
        void Clear();

        void Process(FrameBuffer* input, FrameBuffer* output);

        const Vector<Own<PostProcessEffect>>& GetEffects() const { return m_Effects; }

        bool IsEnabled() const { return m_Enabled; }
        void SetEnabled(bool enabled) { m_Enabled = enabled; }

    private:
        Vector<Own<PostProcessEffect>> m_Effects;
        Vector<FrameBuffer*> m_PingPongBuffers;
        bool m_Enabled = true;
    };
}

#endif // ASHEN_POST_PROCESS_STACK_H
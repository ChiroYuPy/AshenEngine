#include "Ashen/Graphics/PostProcessing/PostProcessStack.h"

namespace ash {
    PostProcessStack::PostProcessStack() = default;

    PostProcessStack::~PostProcessStack() = default;

    void PostProcessStack::AddEffect(Own<PostProcessEffect> effect) {
        if (effect) {
            m_Effects.push_back(std::move(effect));
        }
    }

    void PostProcessStack::RemoveEffect(size_t index) {
        if (index < m_Effects.size()) {
            m_Effects.erase(m_Effects.begin() + index);
        }
    }

    void PostProcessStack::Clear() {
        m_Effects.clear();
    }

    void PostProcessStack::Process(FrameBuffer* input, FrameBuffer* output) {
        if (!m_Enabled || m_Effects.empty()) {
            // Just copy input to output
            return;
        }

        // Ensure we have ping-pong buffers
        if (m_PingPongBuffers.size() < 2) {
            // Create ping-pong buffers
            // Would actually create FrameBuffers here
        }

        FrameBuffer* current = input;
        FrameBuffer* next = nullptr;

        for (size_t i = 0; i < m_Effects.size(); ++i) {
            auto& effect = m_Effects[i];

            if (!effect->IsEnabled()) continue;

            // Determine next buffer
            if (i == m_Effects.size() - 1) {
                next = output;
            } else {
                next = m_PingPongBuffers[i % 2];
            }

            effect->Apply(current, next);
            current = next;
        }
    }
}
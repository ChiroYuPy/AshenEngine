#include "Ashen/Animation/Animator.h"
#include "Ashen/Nodes/Node.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    Animator::Animator(Node* node)
        : m_Node(node) {
    }

    Animator::~Animator() = default;

    void Animator::Play(const String& clipName) {
        auto it = m_Clips.find(clipName);
        if (it != m_Clips.end()) {
            Play(it->second);
        } else {
            Logger::Warn(Format("Animator: Animation clip '{}' not found", clipName));
        }
    }

    void Animator::Play(Ref<AnimationClip> clip) {
        if (!clip) return;

        if (m_CurrentState.clip != clip) {
            m_PreviousState = m_CurrentState;
            m_CurrentState.clip = clip;
            m_CurrentState.time = 0.0f;
            m_CurrentState.finished = false;
            m_LastEventTime = 0.0f;
        }

        m_CurrentState.playing = true;
        m_CrossFadeTime = 0.0f;
        m_CrossFadeDuration = 0.0f;
    }

    void Animator::Pause() {
        m_CurrentState.playing = false;
    }

    void Animator::Resume() {
        if (m_CurrentState.clip) {
            m_CurrentState.playing = true;
        }
    }

    void Animator::Stop() {
        m_CurrentState.playing = false;
        m_CurrentState.time = 0.0f;
        m_CurrentState.finished = false;
        m_CrossFadeTime = 0.0f;
    }

    void Animator::SetCurrentTime(float time) {
        if (m_CurrentState.clip) {
            float length = m_CurrentState.clip->GetLength();
            m_CurrentState.time = WrapTime(time, length, m_CurrentState.clip->GetLoopMode());
        }
    }

    void Animator::AddClip(Ref<AnimationClip> clip) {
        if (clip) {
            m_Clips[clip->GetName()] = clip;
        }
    }

    void Animator::RemoveClip(const String& name) {
        auto it = m_Clips.find(name);
        if (it != m_Clips.end()) {
            if (m_CurrentState.clip == it->second) {
                Stop();
            }
            m_Clips.erase(it);
        }
    }

    Ref<AnimationClip> Animator::GetClip(const String& name) const {
        auto it = m_Clips.find(name);
        return it != m_Clips.end() ? it->second : nullptr;
    }

    bool Animator::HasClip(const String& name) const {
        return m_Clips.find(name) != m_Clips.end();
    }

    void Animator::CrossFade(const String& clipName, float duration) {
        auto it = m_Clips.find(clipName);
        if (it != m_Clips.end()) {
            CrossFade(it->second, duration);
        }
    }

    void Animator::CrossFade(Ref<AnimationClip> clip, float duration) {
        if (!clip || clip == m_CurrentState.clip) return;

        m_PreviousState = m_CurrentState;
        m_CurrentState.clip = clip;
        m_CurrentState.time = 0.0f;
        m_CurrentState.playing = true;
        m_CurrentState.finished = false;

        m_CrossFadeTime = 0.0f;
        m_CrossFadeDuration = Max(duration, 0.0f);
        m_LastEventTime = 0.0f;
    }

    void Animator::Update(float deltaTime) {
        if (!m_CurrentState.clip) return;

        float scaledDelta = deltaTime * m_Speed;

        // Update cross-fade
        if (m_CrossFadeDuration > 0.0f && m_CrossFadeTime < m_CrossFadeDuration) {
            m_CrossFadeTime += deltaTime;

            float t = Clamp(m_CrossFadeTime / m_CrossFadeDuration, 0.0f, 1.0f);
            m_CurrentState.weight = t;
            m_PreviousState.weight = 1.0f - t;

            UpdateState(m_PreviousState, scaledDelta);
            UpdateState(m_CurrentState, scaledDelta);

            if (m_PreviousState.clip) {
                ApplyState(m_PreviousState);
            }
            ApplyState(m_CurrentState);

            if (m_CrossFadeTime >= m_CrossFadeDuration) {
                m_PreviousState.clip = nullptr;
                m_CurrentState.weight = 1.0f;
            }
        } else {
            UpdateState(m_CurrentState, scaledDelta);
            ApplyState(m_CurrentState);
        }
    }

    void Animator::UpdateState(AnimationState& state, float deltaTime) {
        if (!state.clip || !state.playing) return;

        float oldTime = state.time;
        state.time += deltaTime * state.speed;

        float length = state.clip->GetLength();
        AnimationLoopMode loopMode = state.clip->GetLoopMode();

        // Handle looping
        state.time = WrapTime(state.time, length, loopMode);

        // Check for finish
        if (loopMode == AnimationLoopMode::None && state.time >= length) {
            state.time = length;
            state.finished = true;
            state.playing = false;
        }

        // Process events
        ProcessEvents(state, oldTime, state.time);
    }

    void Animator::ApplyState(const AnimationState& state) {
        if (!state.clip || !m_Node) return;

        state.clip->Apply(m_Node, state.time, state.weight);
    }

    void Animator::ProcessEvents(const AnimationState& state, float oldTime, float newTime) {
        if (!state.clip || !m_EventCallback) return;

        Vector<AnimationEvent> events;

        // Handle wrapping
        if (newTime < oldTime) {
            // Wrapped around
            events = state.clip->GetEventsInRange(oldTime, state.clip->GetLength());
            auto startEvents = state.clip->GetEventsInRange(0.0f, newTime);
            events.insert(events.end(), startEvents.begin(), startEvents.end());
        } else {
            events = state.clip->GetEventsInRange(oldTime, newTime);
        }

        for (const auto& event : events) {
            m_EventCallback(event);
        }
    }

    float Animator::WrapTime(float time, float length, AnimationLoopMode loopMode) const {
        if (length <= 0.0f) return 0.0f;

        switch (loopMode) {
            case AnimationLoopMode::None:
                return Clamp(time, 0.0f, length);

            case AnimationLoopMode::Loop:
                while (time < 0.0f) time += length;
                while (time > length) time -= length;
                return time;

            case AnimationLoopMode::PingPong: {
                int cycles = static_cast<int>(time / length);
                float t = time - cycles * length;
                return (cycles % 2 == 0) ? t : length - t;
            }

            default:
                return Clamp(time, 0.0f, length);
        }
    }
}
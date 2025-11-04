#ifndef ASHEN_ANIMATOR_H
#define ASHEN_ANIMATOR_H

#include "AnimationClip.h"
#include "AnimationTypes.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Node;

    struct AnimationState {
        Ref<AnimationClip> clip;
        float time = 0.0f;
        float speed = 1.0f;
        float weight = 1.0f;
        bool playing = false;
        bool finished = false;

        AnimationState() = default;
        AnimationState(Ref<AnimationClip> c) : clip(c) {}
    };

    class Animator {
    public:
        Animator(Node* node);
        ~Animator();

        Node* GetNode() const { return m_Node; }

        // Animation control
        void Play(const String& clipName);
        void Play(Ref<AnimationClip> clip);
        void Pause();
        void Resume();
        void Stop();

        bool IsPlaying() const { return m_CurrentState.playing; }
        bool IsFinished() const { return m_CurrentState.finished; }

        // Animation properties
        float GetSpeed() const { return m_Speed; }
        void SetSpeed(float speed) { m_Speed = Max(speed, 0.0f); }

        float GetCurrentTime() const { return m_CurrentState.time; }
        void SetCurrentTime(float time);

        // Animation library
        void AddClip(Ref<AnimationClip> clip);
        void RemoveClip(const String& name);
        Ref<AnimationClip> GetClip(const String& name) const;
        bool HasClip(const String& name) const;

        const Map<String, Ref<AnimationClip>>& GetClips() const { return m_Clips; }

        // Cross-fading between animations
        void CrossFade(const String& clipName, float duration);
        void CrossFade(Ref<AnimationClip> clip, float duration);

        // Blending
        AnimationBlendMode GetBlendMode() const { return m_BlendMode; }
        void SetBlendMode(AnimationBlendMode mode) { m_BlendMode = mode; }

        // Update
        void Update(float deltaTime);

        // Events
        using AnimationEventCallback = std::function<void(const AnimationEvent&)>;
        void SetEventCallback(AnimationEventCallback callback) { m_EventCallback = callback; }

    private:
        Node* m_Node;
        Map<String, Ref<AnimationClip>> m_Clips;

        AnimationState m_CurrentState;
        AnimationState m_PreviousState;

        float m_Speed = 1.0f;
        float m_CrossFadeTime = 0.0f;
        float m_CrossFadeDuration = 0.0f;

        AnimationBlendMode m_BlendMode = AnimationBlendMode::Blend;
        AnimationEventCallback m_EventCallback;

        float m_LastEventTime = 0.0f;

        void UpdateState(AnimationState& state, float deltaTime);
        void ApplyState(const AnimationState& state);
        void ProcessEvents(const AnimationState& state, float oldTime, float newTime);
        float WrapTime(float time, float length, AnimationLoopMode loopMode) const;
    };
}

#endif // ASHEN_ANIMATOR_H
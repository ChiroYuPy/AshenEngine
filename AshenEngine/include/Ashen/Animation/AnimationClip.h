#ifndef ASHEN_ANIMATION_CLIP_H
#define ASHEN_ANIMATION_CLIP_H

#include "AnimationTypes.h"
#include "AnimationTrack.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    class Node;

    class AnimationClip {
    public:
        AnimationClip(const String& name = "Animation");
        ~AnimationClip();

        String GetName() const { return m_Name; }
        void SetName(const String& name) { m_Name = name; }

        float GetLength() const { return m_Length; }
        void SetLength(float length) { m_Length = Max(length, 0.001f); }

        AnimationLoopMode GetLoopMode() const { return m_LoopMode; }
        void SetLoopMode(AnimationLoopMode mode) { m_LoopMode = mode; }

        float GetFrameRate() const { return m_FrameRate; }
        void SetFrameRate(float fps) { m_FrameRate = Max(fps, 1.0f); }

        // Position tracks
        void AddPositionTrack(const String& nodePath);
        AnimationTrack<Vec3>* GetPositionTrack(const String& nodePath);
        const AnimationTrack<Vec3>* GetPositionTrack(const String& nodePath) const;

        // Rotation tracks
        void AddRotationTrack(const String& nodePath);
        AnimationTrack<Quaternion>* GetRotationTrack(const String& nodePath);
        const AnimationTrack<Quaternion>* GetRotationTrack(const String& nodePath) const;

        // Scale tracks
        void AddScaleTrack(const String& nodePath);
        AnimationTrack<Vec3>* GetScaleTrack(const String& nodePath);
        const AnimationTrack<Vec3>* GetScaleTrack(const String& nodePath) const;

        // Float tracks (for custom properties)
        void AddFloatTrack(const String& propertyPath);
        AnimationTrack<float>* GetFloatTrack(const String& propertyPath);
        const AnimationTrack<float>* GetFloatTrack(const String& propertyPath) const;

        // Events
        void AddEvent(const AnimationEvent& event);
        void AddEvent(float time, const String& name);
        void RemoveEvent(size_t index);
        const Vector<AnimationEvent>& GetEvents() const { return m_Events; }
        Vector<AnimationEvent> GetEventsInRange(float startTime, float endTime) const;

        // Evaluate animation at time
        void Apply(Node* root, float time, float weight = 1.0f) const;

        // Calculate actual length from tracks
        void RecalculateLength();

    private:
        String m_Name;
        float m_Length;
        AnimationLoopMode m_LoopMode;
        float m_FrameRate;

        Map<String, Own<AnimationTrack<Vec3>>> m_PositionTracks;
        Map<String, Own<AnimationTrack<Quaternion>>> m_RotationTracks;
        Map<String, Own<AnimationTrack<Vec3>>> m_ScaleTracks;
        Map<String, Own<AnimationTrack<float>>> m_FloatTracks;

        Vector<AnimationEvent> m_Events;

        Node* FindNode(Node* root, const String& path) const;
    };
}

#endif // ASHEN_ANIMATION_CLIP_H
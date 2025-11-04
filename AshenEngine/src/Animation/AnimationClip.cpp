#include "Ashen/Animation/AnimationClip.h"
#include "Ashen/Nodes/Node.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Core/Logger.h"
#include <algorithm>

namespace ash {
    AnimationClip::AnimationClip(const String& name)
        : m_Name(name)
        , m_Length(1.0f)
        , m_LoopMode(AnimationLoopMode::Loop)
        , m_FrameRate(30.0f) {
    }

    AnimationClip::~AnimationClip() = default;

    void AnimationClip::AddPositionTrack(const String& nodePath) {
        if (m_PositionTracks.find(nodePath) == m_PositionTracks.end()) {
            m_PositionTracks[nodePath] = MakeOwn<AnimationTrack<Vec3>>(
                nodePath, AnimationPropertyType::Position);
        }
    }

    AnimationTrack<Vec3>* AnimationClip::GetPositionTrack(const String& nodePath) {
        auto it = m_PositionTracks.find(nodePath);
        return it != m_PositionTracks.end() ? it->second.get() : nullptr;
    }

    const AnimationTrack<Vec3>* AnimationClip::GetPositionTrack(const String& nodePath) const {
        auto it = m_PositionTracks.find(nodePath);
        return it != m_PositionTracks.end() ? it->second.get() : nullptr;
    }

    void AnimationClip::AddRotationTrack(const String& nodePath) {
        if (m_RotationTracks.find(nodePath) == m_RotationTracks.end()) {
            m_RotationTracks[nodePath] = MakeOwn<AnimationTrack<Quaternion>>(
                nodePath, AnimationPropertyType::Rotation);
        }
    }

    AnimationTrack<Quaternion>* AnimationClip::GetRotationTrack(const String& nodePath) {
        auto it = m_RotationTracks.find(nodePath);
        return it != m_RotationTracks.end() ? it->second.get() : nullptr;
    }

    const AnimationTrack<Quaternion>* AnimationClip::GetRotationTrack(const String& nodePath) const {
        auto it = m_RotationTracks.find(nodePath);
        return it != m_RotationTracks.end() ? it->second.get() : nullptr;
    }

    void AnimationClip::AddScaleTrack(const String& nodePath) {
        if (m_ScaleTracks.find(nodePath) == m_ScaleTracks.end()) {
            m_ScaleTracks[nodePath] = MakeOwn<AnimationTrack<Vec3>>(
                nodePath, AnimationPropertyType::Scale);
        }
    }

    AnimationTrack<Vec3>* AnimationClip::GetScaleTrack(const String& nodePath) {
        auto it = m_ScaleTracks.find(nodePath);
        return it != m_ScaleTracks.end() ? it->second.get() : nullptr;
    }

    const AnimationTrack<Vec3>* AnimationClip::GetScaleTrack(const String& nodePath) const {
        auto it = m_ScaleTracks.find(nodePath);
        return it != m_ScaleTracks.end() ? it->second.get() : nullptr;
    }

    void AnimationClip::AddFloatTrack(const String& propertyPath) {
        if (m_FloatTracks.find(propertyPath) == m_FloatTracks.end()) {
            m_FloatTracks[propertyPath] = MakeOwn<AnimationTrack<float>>(
                propertyPath, AnimationPropertyType::Float);
        }
    }

    AnimationTrack<float>* AnimationClip::GetFloatTrack(const String& propertyPath) {
        auto it = m_FloatTracks.find(propertyPath);
        return it != m_FloatTracks.end() ? it->second.get() : nullptr;
    }

    const AnimationTrack<float>* AnimationClip::GetFloatTrack(const String& propertyPath) const {
        auto it = m_FloatTracks.find(propertyPath);
        return it != m_FloatTracks.end() ? it->second.get() : nullptr;
    }

    void AnimationClip::AddEvent(const AnimationEvent& event) {
        m_Events.push_back(event);
        std::sort(m_Events.begin(), m_Events.end(),
            [](const AnimationEvent& a, const AnimationEvent& b) {
                return a.time < b.time;
            });
    }

    void AnimationClip::AddEvent(float time, const String& name) {
        AnimationEvent event;
        event.time = time;
        event.name = name;
        AddEvent(event);
    }

    void AnimationClip::RemoveEvent(size_t index) {
        if (index < m_Events.size()) {
            m_Events.erase(m_Events.begin() + index);
        }
    }

    Vector<AnimationEvent> AnimationClip::GetEventsInRange(float startTime, float endTime) const {
        Vector<AnimationEvent> events;

        for (const auto& event : m_Events) {
            if (event.time >= startTime && event.time <= endTime) {
                events.push_back(event);
            }
        }

        return events;
    }

    void AnimationClip::Apply(Node* root, float time, float weight) const {
        if (!root || weight <= 0.0f) return;

        // Apply position tracks
        for (const auto& [path, track] : m_PositionTracks) {
            if (Node* node = FindNode(root, path)) {
                if (auto* node3D = dynamic_cast<Node3D*>(node)) {
                    Vec3 value = track->Evaluate(time);
                    if (weight >= 1.0f) {
                        node3D->SetPosition(value);
                    } else {
                        Vec3 current = node3D->GetPosition();
                        node3D->SetPosition(Lerp(current, value, weight));
                    }
                }
            }
        }

        // Apply rotation tracks
        for (const auto& [path, track] : m_RotationTracks) {
            if (Node* node = FindNode(root, path)) {
                if (auto* node3D = dynamic_cast<Node3D*>(node)) {
                    Quaternion value = track->Evaluate(time);
                    if (weight >= 1.0f) {
                        node3D->SetRotation(value);
                    } else {
                        Quaternion current = node3D->GetRotation();
                        node3D->SetRotation(QuatSlerp(current, value, weight));
                    }
                }
            }
        }

        // Apply scale tracks
        for (const auto& [path, track] : m_ScaleTracks) {
            if (Node* node = FindNode(root, path)) {
                if (auto* node3D = dynamic_cast<Node3D*>(node)) {
                    Vec3 value = track->Evaluate(time);
                    if (weight >= 1.0f) {
                        node3D->SetScale(value);
                    } else {
                        Vec3 current = node3D->GetScale();
                        node3D->SetScale(Lerp(current, value, weight));
                    }
                }
            }
        }
    }

    void AnimationClip::RecalculateLength() {
        float maxTime = 0.0f;

        for (const auto& [path, track] : m_PositionTracks) {
            maxTime = Max(maxTime, track->GetEndTime());
        }

        for (const auto& [path, track] : m_RotationTracks) {
            maxTime = Max(maxTime, track->GetEndTime());
        }

        for (const auto& [path, track] : m_ScaleTracks) {
            maxTime = Max(maxTime, track->GetEndTime());
        }

        for (const auto& [path, track] : m_FloatTracks) {
            maxTime = Max(maxTime, track->GetEndTime());
        }

        m_Length = Max(maxTime, 0.001f);
    }

    Node* AnimationClip::FindNode(Node* root, const String& path) const {
        if (!root) return nullptr;
        if (path.empty()) return root;

        // Simple path finding (e.g., "Parent/Child/GrandChild")
        Node* current = root;
        size_t start = 0;
        size_t end = path.find('/');

        while (end != String::npos) {
            String nodeName = path.substr(start, end - start);
            current = current->FindChild(nodeName);
            if (!current) return nullptr;

            start = end + 1;
            end = path.find('/', start);
        }

        if (start < path.length()) {
            String nodeName = path.substr(start);
            current = current->FindChild(nodeName);
        }

        return current;
    }
}
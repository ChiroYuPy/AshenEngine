#ifndef ASHEN_ANIMATION_TRACK_H
#define ASHEN_ANIMATION_TRACK_H

#include "AnimationTypes.h"
#include "Ashen/Math/Math.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

namespace ash {
    template<typename T>
    class AnimationTrack {
    public:
        AnimationTrack(const String& propertyPath, AnimationPropertyType type)
            : m_PropertyPath(propertyPath)
            , m_PropertyType(type) {
        }

        void AddKeyframe(const Keyframe<T>& keyframe) {
            m_Keyframes.push_back(keyframe);
            SortKeyframes();
        }

        void AddKeyframe(float time, const T& value, InterpolationMode interp = InterpolationMode::Linear) {
            Keyframe<T> kf(time, value);
            kf.interpolation = interp;
            AddKeyframe(kf);
        }

        void RemoveKeyframe(size_t index) {
            if (index < m_Keyframes.size()) {
                m_Keyframes.erase(m_Keyframes.begin() + index);
            }
        }

        void Clear() {
            m_Keyframes.clear();
        }

        const Vector<Keyframe<T>>& GetKeyframes() const { return m_Keyframes; }
        Vector<Keyframe<T>>& GetKeyframes() { return m_Keyframes; }

        String GetPropertyPath() const { return m_PropertyPath; }
        AnimationPropertyType GetPropertyType() const { return m_PropertyType; }

        bool IsEmpty() const { return m_Keyframes.empty(); }
        size_t GetKeyframeCount() const { return m_Keyframes.size(); }

        float GetStartTime() const {
            return m_Keyframes.empty() ? 0.0f : m_Keyframes.front().time;
        }

        float GetEndTime() const {
            return m_Keyframes.empty() ? 0.0f : m_Keyframes.back().time;
        }

        T Evaluate(float time) const {
            if (m_Keyframes.empty()) {
                return T();
            }

            if (m_Keyframes.size() == 1) {
                return m_Keyframes[0].value;
            }

            // Find surrounding keyframes
            size_t nextIndex = 0;
            for (size_t i = 0; i < m_Keyframes.size(); ++i) {
                if (m_Keyframes[i].time > time) {
                    nextIndex = i;
                    break;
                }
            }

            if (nextIndex == 0) {
                return m_Keyframes[0].value;
            }

            if (nextIndex >= m_Keyframes.size()) {
                return m_Keyframes.back().value;
            }

            size_t prevIndex = nextIndex - 1;
            const Keyframe<T>& prev = m_Keyframes[prevIndex];
            const Keyframe<T>& next = m_Keyframes[nextIndex];

            float t = (time - prev.time) / (next.time - prev.time);
            t = Clamp(t, 0.0f, 1.0f);

            return Interpolate(prev, next, t);
        }

    private:
        String m_PropertyPath;
        AnimationPropertyType m_PropertyType;
        Vector<Keyframe<T>> m_Keyframes;

        void SortKeyframes() {
            std::sort(m_Keyframes.begin(), m_Keyframes.end(),
                [](const Keyframe<T>& a, const Keyframe<T>& b) {
                    return a.time < b.time;
                });
        }

        T Interpolate(const Keyframe<T>& a, const Keyframe<T>& b, float t) const {
            switch (b.interpolation) {
                case InterpolationMode::Step:
                    return a.value;

                case InterpolationMode::Linear:
                    return Lerp(a.value, b.value, t);

                case InterpolationMode::Cubic: {
                    // Hermite interpolation
                    float t2 = t * t;
                    float t3 = t2 * t;

                    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
                    float h10 = t3 - 2.0f * t2 + t;
                    float h01 = -2.0f * t3 + 3.0f * t2;
                    float h11 = t3 - t2;

                    T outTangent = a.outTangent;
                    T inTangent = b.inTangent;

                    return a.value * h00 + outTangent * h10 + b.value * h01 + inTangent * h11;
                }

                default:
                    return Lerp(a.value, b.value, t);
            }
        }
    };

    // Specialization for Quaternion
    template<>
    inline Quaternion AnimationTrack<Quaternion>::Interpolate(
        const Keyframe<Quaternion>& a,
        const Keyframe<Quaternion>& b,
        float t) const {

        switch (b.interpolation) {
            case InterpolationMode::Step:
                return a.value;

            case InterpolationMode::Linear:
            case InterpolationMode::Cubic:  // Use slerp for quaternions in cubic mode too
                return QuatSlerp(a.value, b.value, t);

            default:
                return QuatSlerp(a.value, b.value, t);
        }
    }
}

#endif // ASHEN_ANIMATION_TRACK_H
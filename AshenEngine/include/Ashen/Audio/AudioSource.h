#ifndef ASHEN_AUDIOSOURCE_H
#define ASHEN_AUDIOSOURCE_H

#include "Ashen/Audio/AudioTypes.h"
#include "Ashen/Core/Types.h"

namespace ash {
    class AudioSource {
    public:
        virtual ~AudioSource() = default;

        virtual void Play() = 0;

        virtual void Pause() = 0;

        virtual void Stop() = 0;

        virtual void Resume() = 0;

        virtual AudioState GetState() const = 0;

        virtual bool IsPlaying() const = 0;

        virtual bool IsPaused() const = 0;

        virtual bool IsStopped() const = 0;

        virtual void SetPlaybackPosition(float seconds) = 0;

        virtual float GetPlaybackPosition() const = 0;

        virtual float GetDuration() const = 0;

        virtual void SetVolume(float volume) = 0;

        virtual float GetVolume() const = 0;

        virtual void SetPitch(float pitch) = 0;

        virtual float GetPitch() const = 0;

        virtual void SetLooping(bool loop) = 0;

        virtual bool IsLooping() const = 0;

        virtual void SetSpatial(bool spatial) = 0;

        virtual bool IsSpatial() const = 0;

        virtual void SetPosition(const Vec3 &position) = 0;

        virtual Vec3 GetPosition() const = 0;

        virtual void SetVelocity(const Vec3 &velocity) = 0;

        virtual Vec3 GetVelocity() const = 0;

        virtual void SetMinDistance(float distance) = 0;

        virtual float GetMinDistance() const = 0;

        virtual void SetMaxDistance(float distance) = 0;

        virtual float GetMaxDistance() const = 0;

        virtual void SetRolloff(float rolloff) = 0;

        virtual float GetRolloff() const = 0;

        virtual void SetCategory(AudioCategory category) = 0;

        virtual AudioCategory GetCategory() const = 0;

        virtual AudioSourceHandle GetHandle() const = 0;

        virtual AudioSourceType GetType() const = 0;
    };
}

#endif // ASHEN_AUDIOSOURCE_H
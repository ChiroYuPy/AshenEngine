#ifndef ASHEN_MINIAUDIOSOURCE_H
#define ASHEN_MINIAUDIOSOURCE_H

#include "Ashen/Audio/AudioSource.h"
#include "Ashen/Audio/AudioTypes.h"

typedef struct ma_sound ma_sound;
typedef struct ma_engine ma_engine;

namespace ash {
    class MiniAudioSource final : public AudioSource {
    public:
        MiniAudioSource(ma_engine *engine, const std::string &filepath, const AudioSourceConfig &config,
                        AudioSourceHandle handle);

        ~MiniAudioSource() override;

        void Play() override;

        void Pause() override;

        void Stop() override;

        void Resume() override;

        AudioState GetState() const override;

        bool IsPlaying() const override;

        bool IsPaused() const override;

        bool IsStopped() const override;

        void SetPlaybackPosition(float seconds) override;

        float GetPlaybackPosition() const override;

        float GetDuration() const override;

        void SetVolume(float volume) override;

        float GetVolume() const override;

        void SetPitch(float pitch) override;

        float GetPitch() const override;

        void SetLooping(bool loop) override;

        bool IsLooping() const override;

        void SetSpatial(bool spatial) override;

        bool IsSpatial() const override;

        void SetPosition(const Vec3 &position) override;

        Vec3 GetPosition() const override;

        void SetVelocity(const Vec3 &velocity) override;

        Vec3 GetVelocity() const override;

        void SetMinDistance(float distance) override;

        float GetMinDistance() const override;

        void SetMaxDistance(float distance) override;

        float GetMaxDistance() const override;

        void SetRolloff(float rolloff) override;

        float GetRolloff() const override;

        void SetCategory(AudioCategory category) override;

        AudioCategory GetCategory() const override;

        AudioSourceHandle GetHandle() const override { return m_Handle; }
        AudioSourceType GetType() const override { return m_Type; }

        ma_sound *GetMaSound() const { return m_Sound; }

    private:
        ma_engine *m_Engine;
        ma_sound *m_Sound;

        AudioSourceHandle m_Handle;
        AudioSourceType m_Type;
        AudioCategory m_Category;

        float m_Volume;
        float m_Pitch;
        bool m_Loop;
        bool m_Spatial;
        Vec3 m_Position;
        Vec3 m_Velocity;
        float m_MinDistance;
        float m_MaxDistance;
        float m_Rolloff;

        std::string m_Filepath;
    };
}

#endif // ASHEN_MINIAUDIOSOURCE_H
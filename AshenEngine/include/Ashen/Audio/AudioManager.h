#ifndef ASHEN_AUDIOMANAGER_H
#define ASHEN_AUDIOMANAGER_H

#include "Ashen/Audio/AudioDevice.h"
#include "Ashen/Audio/AudioSource.h"
#include "Ashen/Audio/AudioTypes.h"
#include "Ashen/Core/Types.h"

namespace ash {
    class AudioManager {
    public:
        AudioManager();

        ~AudioManager();

        AudioManager(const AudioManager &) = delete;

        AudioManager &operator=(const AudioManager &) = delete;

        bool Initialize(AudioDevice::Backend backend = AudioDevice::Backend::MiniAudio);

        void Shutdown();

        void PlaySound(const std::string &filepath, float volume = 1.0f, AudioCategory category = AudioCategory::SFX);

        void PlaySoundAtPosition(const std::string &filepath, const Vec3 &position, float volume = 1.0f,
                                 AudioCategory category = AudioCategory::SFX);

        void PlayMusic(const std::string &filepath, float volume = 1.0f, bool loop = true);

        void StopMusic();

        void PauseMusic() const;

        void ResumeMusic() const;

        void SetMusicVolume(float volume) const;

        bool IsMusicPlaying() const;

        Ref<AudioSource> CreateAudioSource(const std::string &filepath, const AudioSourceConfig &config);

        void DestroyAudioSource(AudioSourceHandle handle);

        Ref<AudioSource> GetAudioSource(AudioSourceHandle handle);

        void SetListenerPosition(const Vec3 &position) const;

        void SetListenerVelocity(const Vec3 &velocity) const;

        void SetListenerOrientation(const Vec3 &forward, const Vec3 &up) const;

        void SetMasterVolume(float volume) const;

        float GetMasterVolume() const;

        void SetCategoryVolume(AudioCategory category, float volume) const;

        float GetCategoryVolume(AudioCategory category) const;

        void MuteCategory(AudioCategory category, bool mute = true) const;

        void UnmuteCategory(AudioCategory category) const;

        bool IsCategoryMuted(AudioCategory category) const;

        void PauseAll() const;

        void ResumeAll() const;

        void StopAll();

        void Update();

        static AudioManager &Get();

        AudioDeviceInfo GetDeviceInfo() const;

    private:
        void CleanupFinishedSources();

        Own<AudioDevice> m_Device;
        Vector<Ref<AudioSource> > m_TempSources;
        HashMap<AudioSourceHandle, Ref<AudioSource> > m_Sources;
        Ref<AudioSource> m_CurrentMusic;
        AudioSourceHandle m_NextHandle = 1;

        static AudioManager *s_Instance;
    };
}

#endif // ASHEN_AUDIOMANAGER_H
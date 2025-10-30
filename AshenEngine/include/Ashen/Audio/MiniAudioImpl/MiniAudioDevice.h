#ifndef ASHEN_MINIAUDIODEVICE_H
#define ASHEN_MINIAUDIODEVICE_H

#include "Ashen/Audio/AudioDevice.h"
#include "Ashen/Audio/AudioTypes.h"
#include "Ashen/Audio/MiniAudioImpl/MiniAudioSource.h"

typedef ma_engine ma_engine;

namespace ash {
    class MiniAudioDevice final : public AudioDevice {
    public:
        MiniAudioDevice();

        ~MiniAudioDevice() override;

        bool Initialize() override;

        void Shutdown() override;

        bool IsInitialized() const override { return m_Initialized; }

        Ref<AudioSource> CreateSource(const std::string &filepath, const AudioSourceConfig &config) override;

        void DestroySource(AudioSourceHandle handle) override;

        void SetListenerPosition(const Vec3 &position) override;

        void SetListenerVelocity(const Vec3 &velocity) override;

        void SetListenerOrientation(const Vec3 &forward, const Vec3 &up) override;

        AudioListenerConfig GetListenerConfig() const override;

        void SetMasterVolume(float volume) override;

        float GetMasterVolume() const override;

        void SetCategoryVolume(AudioCategory category, float volume) override;

        float GetCategoryVolume(AudioCategory category) const override;

        void MuteCategory(AudioCategory category, bool mute) override;

        bool IsCategoryMuted(AudioCategory category) const override;

        void PauseAll() override;

        void ResumeAll() override;

        void StopAll() override;

        AudioDeviceInfo GetDeviceInfo() const override;

        Vector<AudioDeviceInfo> GetAvailableDevices() const override;

        bool SelectDevice(const std::string &deviceName) override;

        void Update() override;

    private:
        void ApplyCategoryVolumes();

        float CalculateEffectiveVolume(AudioCategory category) const;

        ma_engine *m_Engine;
        bool m_Initialized;

        AudioSourceHandle m_NextHandle;
        HashMap<AudioSourceHandle, Ref<MiniAudioSource> > m_Sources;

        AudioListenerConfig m_ListenerConfig;

        float m_MasterVolume;
        HashMap<AudioCategory, AudioCategoryVolume> m_CategoryVolumes;

        bool m_AllPaused;
    };
}

#endif // ASHEN_MINIAUDIODEVICE_H
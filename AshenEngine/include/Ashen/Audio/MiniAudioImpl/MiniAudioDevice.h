#ifndef ASHEN_MINIAUDIODEVICE_H
#define ASHEN_MINIAUDIODEVICE_H

#include "Ashen/Audio/AudioDevice.h"
#include "Ashen/Audio/AudioTypes.h"
#include "MiniAudioSource.h"

// Forward declaration
typedef struct ma_engine ma_engine;

namespace ash {

    class MiniAudioDevice : public AudioDevice {
    public:
        MiniAudioDevice();
        ~MiniAudioDevice() override;

        // Initialisation et destruction
        bool Initialize() override;
        void Shutdown() override;
        bool IsInitialized() const override { return m_Initialized; }

        // Création de sources audio
        Ref<AudioSource> CreateSource(const std::string& filepath, const AudioSourceConfig& config) override;
        void DestroySource(AudioSourceHandle handle) override;

        // Listener (écouteur)
        void SetListenerPosition(const Vec3& position) override;
        void SetListenerVelocity(const Vec3& velocity) override;
        void SetListenerOrientation(const Vec3& forward, const Vec3& up) override;
        AudioListenerConfig GetListenerConfig() const override;

        // Volume global
        void SetMasterVolume(float volume) override;
        float GetMasterVolume() const override;

        // Volumes par catégorie
        void SetCategoryVolume(AudioCategory category, float volume) override;
        float GetCategoryVolume(AudioCategory category) const override;
        void MuteCategory(AudioCategory category, bool mute) override;
        bool IsCategoryMuted(AudioCategory category) const override;

        // Pause globale
        void PauseAll() override;
        void ResumeAll() override;
        void StopAll() override;

        // Informations sur le périphérique
        AudioDeviceInfo GetDeviceInfo() const override;
        Vector<AudioDeviceInfo> GetAvailableDevices() const override;
        bool SelectDevice(const std::string& deviceName) override;

        // Update
        void Update() override;

    private:
        void ApplyCategoryVolumes();
        float CalculateEffectiveVolume(AudioCategory category) const;

        ma_engine* m_Engine;
        bool m_Initialized;

        AudioSourceHandle m_NextHandle;
        HashMap<AudioSourceHandle, Ref<MiniAudioSource>> m_Sources;

        // État du listener
        AudioListenerConfig m_ListenerConfig;

        // Volumes
        float m_MasterVolume;
        HashMap<AudioCategory, AudioCategoryVolume> m_CategoryVolumes;

        bool m_AllPaused;
    };

} // namespace ash

#endif // ASHEN_MINIAUDIODEVICE_H
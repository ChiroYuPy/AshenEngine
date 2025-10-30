#ifndef ASHEN_AUDIODEVICE_H
#define ASHEN_AUDIODEVICE_H

#include "Ashen/Audio/AudioTypes.h"
#include "Ashen/Audio/AudioSource.h"
#include "Ashen/Core/Types.h"

namespace ash {
    class AudioDevice {
    public:
        virtual ~AudioDevice() = default;

        virtual bool Initialize() = 0;

        virtual void Shutdown() = 0;

        virtual bool IsInitialized() const = 0;

        virtual Ref<AudioSource> CreateSource(const std::string &filepath, const AudioSourceConfig &config) = 0;

        virtual void DestroySource(AudioSourceHandle handle) = 0;

        virtual void SetListenerPosition(const Vec3 &position) = 0;

        virtual void SetListenerVelocity(const Vec3 &velocity) = 0;

        virtual void SetListenerOrientation(const Vec3 &forward, const Vec3 &up) = 0;

        virtual AudioListenerConfig GetListenerConfig() const = 0;

        virtual void SetMasterVolume(float volume) = 0;

        virtual float GetMasterVolume() const = 0;

        virtual void SetCategoryVolume(AudioCategory category, float volume) = 0;

        virtual float GetCategoryVolume(AudioCategory category) const = 0;

        virtual void MuteCategory(AudioCategory category, bool mute) = 0;

        virtual bool IsCategoryMuted(AudioCategory category) const = 0;

        virtual void PauseAll() = 0;

        virtual void ResumeAll() = 0;

        virtual void StopAll() = 0;

        virtual AudioDeviceInfo GetDeviceInfo() const = 0;

        virtual Vector<AudioDeviceInfo> GetAvailableDevices() const = 0;

        virtual bool SelectDevice(const std::string &deviceName) = 0;

        virtual void Update() = 0;

        enum class Backend {
            MiniAudio
        };

        static Own<AudioDevice> Create(Backend backend);
    };
}

#endif // ASHEN_AUDIODEVICE_H
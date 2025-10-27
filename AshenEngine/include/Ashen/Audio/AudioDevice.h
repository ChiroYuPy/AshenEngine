#ifndef ASHEN_AUDIODEVICE_H
#define ASHEN_AUDIODEVICE_H

#include "AudioTypes.h"
#include "AudioSource.h"
#include "Ashen/Core/Types.h"

namespace ash {

    // Interface abstraite pour le périphérique audio
    // Permet de changer d'implémentation (miniaudio, OpenAL, FMOD, etc.)
    class AudioDevice {
    public:
        virtual ~AudioDevice() = default;

        // Initialisation et destruction
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual bool IsInitialized() const = 0;

        // Création de sources audio
        virtual Ref<AudioSource> CreateSource(const std::string& filepath, const AudioSourceConfig& config) = 0;
        virtual void DestroySource(AudioSourceHandle handle) = 0;

        // Listener (écouteur - généralement la caméra)
        virtual void SetListenerPosition(const Vec3& position) = 0;
        virtual void SetListenerVelocity(const Vec3& velocity) = 0;
        virtual void SetListenerOrientation(const Vec3& forward, const Vec3& up) = 0;
        virtual AudioListenerConfig GetListenerConfig() const = 0;

        // Volume global
        virtual void SetMasterVolume(float volume) = 0;
        virtual float GetMasterVolume() const = 0;

        // Volumes par catégorie
        virtual void SetCategoryVolume(AudioCategory category, float volume) = 0;
        virtual float GetCategoryVolume(AudioCategory category) const = 0;
        virtual void MuteCategory(AudioCategory category, bool mute) = 0;
        virtual bool IsCategoryMuted(AudioCategory category) const = 0;

        // Pause globale (pause de jeu)
        virtual void PauseAll() = 0;
        virtual void ResumeAll() = 0;
        virtual void StopAll() = 0;

        // Informations sur le périphérique
        virtual AudioDeviceInfo GetDeviceInfo() const = 0;
        virtual Vector<AudioDeviceInfo> GetAvailableDevices() const = 0;
        virtual bool SelectDevice(const std::string& deviceName) = 0;

        // Update (à appeler chaque frame)
        virtual void Update() = 0;

        enum class Backend {
            MiniAudio
        };

        static Own<AudioDevice> Create(Backend backend);
    };

} // namespace ash

#endif // ASHEN_AUDIODEVICE_H
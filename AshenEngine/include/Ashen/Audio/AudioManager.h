#ifndef ASHEN_AUDIOMANAGER_H
#define ASHEN_AUDIOMANAGER_H

#include "AudioDevice.h"
#include "AudioSource.h"
#include "AudioTypes.h"
#include "Ashen/Core/Types.h"

namespace ash {

    // Gestionnaire audio de haut niveau - API simple pour l'utilisateur
    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        // Initialisation
        bool Initialize(AudioDevice::Backend backend = AudioDevice::Backend::MiniAudio);
        void Shutdown();

        // === API SIMPLE ===

        // Jouer un son rapidement (fire-and-forget)
        void PlaySound(const std::string& filepath, float volume = 1.0f, AudioCategory category = AudioCategory::SFX);
        void PlaySoundAtPosition(const std::string& filepath, const Vec3& position, float volume = 1.0f, AudioCategory category = AudioCategory::SFX);

        // Musique (une seule musique à la fois)
        void PlayMusic(const std::string& filepath, float volume = 1.0f, bool loop = true);
        void StopMusic();
        void PauseMusic();
        void ResumeMusic();
        void SetMusicVolume(float volume);
        bool IsMusicPlaying() const;

        // === API AVANCÉE ===

        // Créer et contrôler des sources manuellement
        Ref<AudioSource> CreateAudioSource(const std::string& filepath, const AudioSourceConfig& config);
        void DestroyAudioSource(AudioSourceHandle handle);
        Ref<AudioSource> GetAudioSource(AudioSourceHandle handle);

        // Listener (écouteur 3D)
        void SetListenerPosition(const Vec3& position);
        void SetListenerVelocity(const Vec3& velocity);
        void SetListenerOrientation(const Vec3& forward, const Vec3& up);

        // Volumes
        void SetMasterVolume(float volume);
        float GetMasterVolume() const;

        void SetCategoryVolume(AudioCategory category, float volume);
        float GetCategoryVolume(AudioCategory category) const;

        void MuteCategory(AudioCategory category, bool mute = true);
        void UnmuteCategory(AudioCategory category);
        bool IsCategoryMuted(AudioCategory category) const;

        // Contrôles globaux
        void PauseAll();
        void ResumeAll();
        void StopAll();

        // Update (à appeler chaque frame)
        void Update();

        // Singleton
        static AudioManager& Get();

        // Informations
        AudioDeviceInfo GetDeviceInfo() const;

    private:
        void CleanupFinishedSources();

        Own<AudioDevice> m_Device;

        // Sources temporaires (fire-and-forget)
        Vector<Ref<AudioSource>> m_TempSources;

        // Sources persistantes (contrôlées manuellement)
        HashMap<AudioSourceHandle, Ref<AudioSource>> m_Sources;

        // Musique actuelle
        Ref<AudioSource> m_CurrentMusic;

        AudioSourceHandle m_NextHandle = 1;

        static AudioManager* s_Instance;
    };

} // namespace ash

#endif // ASHEN_AUDIOMANAGER_H
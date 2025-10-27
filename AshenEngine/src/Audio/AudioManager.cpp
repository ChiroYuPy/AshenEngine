#include "Ashen/Audio/AudioManager.h"
#include "Ashen/Audio/MiniAudioImpl/MiniAudioDevice.h"
#include "Ashen/Core/Logger.h"

namespace ash {

    AudioManager* AudioManager::s_Instance = nullptr;

    AudioManager::AudioManager() {
        if (s_Instance) {
            Logger::Warn("AudioManager instance already exists!");
        }
        s_Instance = this;
    }

    AudioManager::~AudioManager() {
        Shutdown();
        s_Instance = nullptr;
    }

    bool AudioManager::Initialize(const AudioDevice::Backend backend) {
        Logger::Info("Initializing AudioManager...");

        m_Device = AudioDevice::Create(backend);
        if (!m_Device) {
            Logger::Error("Failed to create audio device");
            return false;
        }

        if (!m_Device->Initialize()) {
            Logger::Error("Failed to initialize audio device");
            m_Device.reset();
            return false;
        }

        Logger::Info("AudioManager initialized successfully");
        return true;
    }

    void AudioManager::Shutdown() {
        if (!m_Device) return;

        Logger::Info("Shutting down AudioManager...");

        // Arrêter toutes les sources
        StopAll();

        // Nettoyer les sources temporaires
        m_TempSources.clear();

        // Nettoyer les sources persistantes
        m_Sources.clear();

        // Nettoyer la musique
        m_CurrentMusic.reset();

        // Arrêter le device
        m_Device->Shutdown();
        m_Device.reset();

        Logger::Info("AudioManager shut down");
    }

    // === API SIMPLE ===

    void AudioManager::PlaySound(const std::string& filepath, const float volume, const AudioCategory category) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot play sound: audio device not initialized");
            return;
        }

        AudioSourceConfig config;
        config.Type = AudioSourceType::Static;
        config.Volume = volume;
        config.Loop = false;
        config.Spatial = false;

        auto source = m_Device->CreateSource(filepath, config);
        if (source) {
            source->SetCategory(category);
            source->Play();
            m_TempSources.push_back(source);
        }
    }

    void AudioManager::PlaySoundAtPosition(const std::string& filepath, const Vec3& position,
                                          const float volume, const AudioCategory category) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot play sound: audio device not initialized");
            return;
        }

        AudioSourceConfig config;
        config.Type = AudioSourceType::Static;
        config.Volume = volume;
        config.Loop = false;
        config.Spatial = true;
        config.Position = position;

        auto source = m_Device->CreateSource(filepath, config);
        if (source) {
            source->SetCategory(category);
            source->Play();
            m_TempSources.push_back(source);
        }
    }

    void AudioManager::PlayMusic(const std::string& filepath, const float volume, const bool loop) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot play music: audio device not initialized");
            return;
        }

        // Arrêter la musique actuelle si elle existe
        if (m_CurrentMusic) {
            m_CurrentMusic->Stop();
        }

        AudioSourceConfig config;
        config.Type = AudioSourceType::Streaming;
        config.Volume = volume;
        config.Loop = loop;
        config.Spatial = false;

        m_CurrentMusic = m_Device->CreateSource(filepath, config);
        if (m_CurrentMusic) {
            m_CurrentMusic->SetCategory(AudioCategory::Music);
            m_CurrentMusic->Play();
            Logger::Info("Playing music: {}", filepath);
        } else {
            Logger::Error("Failed to load music: {}", filepath);
        }
    }

    void AudioManager::StopMusic() {
        if (m_CurrentMusic) {
            m_CurrentMusic->Stop();
            m_CurrentMusic.reset();
        }
    }

    void AudioManager::PauseMusic() {
        if (m_CurrentMusic) {
            m_CurrentMusic->Pause();
        }
    }

    void AudioManager::ResumeMusic() {
        if (m_CurrentMusic) {
            m_CurrentMusic->Resume();
        }
    }

    void AudioManager::SetMusicVolume(const float volume) {
        if (m_CurrentMusic) {
            m_CurrentMusic->SetVolume(volume);
        }
    }

    bool AudioManager::IsMusicPlaying() const {
        return m_CurrentMusic && m_CurrentMusic->IsPlaying();
    }

    // === API AVANCÉE ===

    Ref<AudioSource> AudioManager::CreateAudioSource(const std::string& filepath, 
                                                     const AudioSourceConfig& config) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot create audio source: audio device not initialized");
            return nullptr;
        }

        auto source = m_Device->CreateSource(filepath, config);
        if (source) {
            m_Sources[source->GetHandle()] = source;
        }
        return source;
    }

    void AudioManager::DestroyAudioSource(const AudioSourceHandle handle) {
        auto it = m_Sources.find(handle);
        if (it != m_Sources.end()) {
            it->second->Stop();
            m_Device->DestroySource(handle);
            m_Sources.erase(it);
        }
    }

    Ref<AudioSource> AudioManager::GetAudioSource(const AudioSourceHandle handle) {
        auto it = m_Sources.find(handle);
        return (it != m_Sources.end()) ? it->second : nullptr;
    }

    // Listener

    void AudioManager::SetListenerPosition(const Vec3& position) {
        if (m_Device) {
            m_Device->SetListenerPosition(position);
        }
    }

    void AudioManager::SetListenerVelocity(const Vec3& velocity) {
        if (m_Device) {
            m_Device->SetListenerVelocity(velocity);
        }
    }

    void AudioManager::SetListenerOrientation(const Vec3& forward, const Vec3& up) {
        if (m_Device) {
            m_Device->SetListenerOrientation(forward, up);
        }
    }

    // Volumes

    void AudioManager::SetMasterVolume(const float volume) {
        if (m_Device) {
            m_Device->SetMasterVolume(volume);
        }
    }

    float AudioManager::GetMasterVolume() const {
        return m_Device ? m_Device->GetMasterVolume() : 1.0f;
    }

    void AudioManager::SetCategoryVolume(const AudioCategory category, const float volume) {
        if (m_Device) {
            m_Device->SetCategoryVolume(category, volume);
        }
    }

    float AudioManager::GetCategoryVolume(const AudioCategory category) const {
        return m_Device ? m_Device->GetCategoryVolume(category) : 1.0f;
    }

    void AudioManager::MuteCategory(const AudioCategory category, const bool mute) {
        if (m_Device) {
            m_Device->MuteCategory(category, mute);
        }
    }

    void AudioManager::UnmuteCategory(const AudioCategory category) {
        MuteCategory(category, false);
    }

    bool AudioManager::IsCategoryMuted(const AudioCategory category) const {
        return m_Device ? m_Device->IsCategoryMuted(category) : false;
    }

    // Contrôles globaux

    void AudioManager::PauseAll() {
        if (m_Device) {
            m_Device->PauseAll();
        }
    }

    void AudioManager::ResumeAll() {
        if (m_Device) {
            m_Device->ResumeAll();
        }
    }

    void AudioManager::StopAll() {
        if (m_Device) {
            m_Device->StopAll();
        }

        // Nettoyer aussi les sources temporaires
        m_TempSources.clear();
    }

    // Update

    void AudioManager::Update() {
        if (!m_Device) return;

        m_Device->Update();

        // Nettoyer les sources temporaires terminées
        CleanupFinishedSources();
    }

    void AudioManager::CleanupFinishedSources() {
        auto it = m_TempSources.begin();
        while (it != m_TempSources.end()) {
            if ((*it)->IsStopped() && !(*it)->IsLooping()) {
                it = m_TempSources.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Singleton

    AudioManager& AudioManager::Get() {
        if (!s_Instance) {
            Logger::Error("AudioManager not initialized!");
            static AudioManager dummy;
            return dummy;
        }
        return *s_Instance;
    }

    // Informations

    AudioDeviceInfo AudioManager::GetDeviceInfo() const {
        return m_Device ? m_Device->GetDeviceInfo() : AudioDeviceInfo{};
    }

    // Factory

    Own<AudioDevice> AudioDevice::Create(const Backend backend) {
        switch (backend) {
            case Backend::MiniAudio:
                return MakeOwn<MiniAudioDevice>();
            
            default:
                Logger::Error("Unknown audio backend");
                return nullptr;
        }
    }

} // namespace ash
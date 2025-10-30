#include "Ashen/Audio/AudioManager.h"
#include "Ashen/Audio/MiniAudioImpl/MiniAudioDevice.h"
#include "Ashen/Core/Logger.h"

namespace ash {
    AudioManager *AudioManager::s_Instance = nullptr;

    AudioManager::AudioManager() {
        if (s_Instance)
            Logger::Warn("AudioManager instance already exists!");

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

        StopAll();

        m_TempSources.clear();

        m_Sources.clear();

        m_CurrentMusic.reset();

        m_Device->Shutdown();
        m_Device.reset();

        Logger::Info("AudioManager shut down");
    }

    void AudioManager::PlaySound(const String &filepath, const float volume, const AudioCategory category) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot play sound: audio device not initialized");
            return;
        }

        AudioSourceConfig config;
        config.Type = AudioSourceType::Static;
        config.Volume = volume;
        config.Loop = false;
        config.Spatial = false;

        const auto source = m_Device->CreateSource(filepath, config);
        if (source) {
            source->SetCategory(category);
            source->Play();
            m_TempSources.push_back(source);
        }
    }

    void AudioManager::PlaySoundAtPosition(const String &filepath, const Vec3 &position,
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

        const auto source = m_Device->CreateSource(filepath, config);
        if (source) {
            source->SetCategory(category);
            source->Play();
            m_TempSources.push_back(source);
        }
    }

    void AudioManager::PlayMusic(const String &filepath, const float volume, const bool loop) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot play music: audio device not initialized");
            return;
        }

        if (m_CurrentMusic)
            m_CurrentMusic->Stop();

        AudioSourceConfig config;
        config.Type = AudioSourceType::Streaming;
        config.Volume = volume;
        config.Loop = loop;
        config.Spatial = false;

        m_CurrentMusic = m_Device->CreateSource(filepath, config);
        if (m_CurrentMusic) {
            m_CurrentMusic->SetCategory(AudioCategory::Music);
            m_CurrentMusic->Play();
            Logger::Info(Format("Playing music: {}", filepath));
        } else {
            Logger::Error(Format("Failed to load music: {}", filepath));
        }
    }

    void AudioManager::StopMusic() {
        if (m_CurrentMusic) {
            m_CurrentMusic->Stop();
            m_CurrentMusic.reset();
        }
    }

    void AudioManager::PauseMusic() const {
        if (m_CurrentMusic)
            m_CurrentMusic->Pause();
    }

    void AudioManager::ResumeMusic() const {
        if (m_CurrentMusic)
            m_CurrentMusic->Resume();
    }

    void AudioManager::SetMusicVolume(const float volume) const {
        if (m_CurrentMusic)
            m_CurrentMusic->SetVolume(volume);
    }

    bool AudioManager::IsMusicPlaying() const {
        return m_CurrentMusic && m_CurrentMusic->IsPlaying();
    }

    Ref<AudioSource> AudioManager::CreateAudioSource(const String &filepath,
                                                     const AudioSourceConfig &config) {
        if (!m_Device || !m_Device->IsInitialized()) {
            Logger::Warn("Cannot create audio source: audio device not initialized");
            return nullptr;
        }

        auto source = m_Device->CreateSource(filepath, config);
        if (source)
            m_Sources[source->GetHandle()] = source;

        return source;
    }

    void AudioManager::DestroyAudioSource(const AudioSourceHandle handle) {
        const auto it = m_Sources.find(handle);
        if (it != m_Sources.end()) {
            it->second->Stop();
            m_Device->DestroySource(handle);
            m_Sources.erase(it);
        }
    }

    Ref<AudioSource> AudioManager::GetAudioSource(const AudioSourceHandle handle) {
        const auto it = m_Sources.find(handle);
        return it != m_Sources.end() ? it->second : nullptr;
    }

    void AudioManager::SetListenerPosition(const Vec3 &position) const {
        if (m_Device)
            m_Device->SetListenerPosition(position);
    }

    void AudioManager::SetListenerVelocity(const Vec3 &velocity) const {
        if (m_Device)
            m_Device->SetListenerVelocity(velocity);
    }

    void AudioManager::SetListenerOrientation(const Vec3 &forward, const Vec3 &up) const {
        if (m_Device)
            m_Device->SetListenerOrientation(forward, up);
    }

    void AudioManager::SetMasterVolume(const float volume) const {
        if (m_Device)
            m_Device->SetMasterVolume(volume);
    }

    float AudioManager::GetMasterVolume() const {
        return m_Device ? m_Device->GetMasterVolume() : 1.0f;
    }

    void AudioManager::SetCategoryVolume(const AudioCategory category, const float volume) const {
        if (m_Device)
            m_Device->SetCategoryVolume(category, volume);
    }

    float AudioManager::GetCategoryVolume(const AudioCategory category) const {
        return m_Device ? m_Device->GetCategoryVolume(category) : 1.0f;
    }

    void AudioManager::MuteCategory(const AudioCategory category, const bool mute) const {
        if (m_Device)
            m_Device->MuteCategory(category, mute);
    }

    void AudioManager::UnmuteCategory(const AudioCategory category) const {
        MuteCategory(category, false);
    }

    bool AudioManager::IsCategoryMuted(const AudioCategory category) const {
        return m_Device ? m_Device->IsCategoryMuted(category) : false;
    }

    void AudioManager::PauseAll() const {
        if (m_Device)
            m_Device->PauseAll();
    }

    void AudioManager::ResumeAll() const {
        if (m_Device)
            m_Device->ResumeAll();
    }

    void AudioManager::StopAll() {
        if (m_Device)
            m_Device->StopAll();

        m_TempSources.clear();
    }

    void AudioManager::Update() {
        if (!m_Device) return;

        m_Device->Update();

        CleanupFinishedSources();
    }

    void AudioManager::CleanupFinishedSources() {
        auto it = m_TempSources.begin();
        while (it != m_TempSources.end()) {
            if ((*it)->IsStopped() && !(*it)->IsLooping())
                it = m_TempSources.erase(it);
            else ++it;
        }
    }

    AudioManager &AudioManager::Get() {
        if (!s_Instance) {
            Logger::Error("AudioManager not initialized!");
            static AudioManager dummy;
            return dummy;
        }
        return *s_Instance;
    }

    AudioDeviceInfo AudioManager::GetDeviceInfo() const {
        return m_Device ? m_Device->GetDeviceInfo() : AudioDeviceInfo{};
    }

    Own<AudioDevice> AudioDevice::Create(const Backend backend) {
        switch (backend) {
            case Backend::MiniAudio:
                return MakeOwn<MiniAudioDevice>();

            default:
                Logger::Error("Unknown audio backend");
                return nullptr;
        }
    }
}
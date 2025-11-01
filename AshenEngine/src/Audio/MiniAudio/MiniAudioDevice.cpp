#include "Ashen/Audio/MiniAudio/MiniAudioDevice.h"
#include "Ashen/Core/Logger.h"

#include <miniaudio.h>
#include <ranges>

namespace ash {
    MiniAudioDevice::MiniAudioDevice()
        : m_Engine(nullptr)
          , m_Initialized(false)
          , m_NextHandle(1)
          , m_MasterVolume(1.0f)
          , m_AllPaused(false) {
        m_CategoryVolumes[AudioCategory::Master] = {AudioCategory::Master, 1.0f, false};
        m_CategoryVolumes[AudioCategory::Music] = {AudioCategory::Music, 1.0f, false};
        m_CategoryVolumes[AudioCategory::SFX] = {AudioCategory::SFX, 1.0f, false};
        m_CategoryVolumes[AudioCategory::Ambient] = {AudioCategory::Ambient, 1.0f, false};
        m_CategoryVolumes[AudioCategory::Voice] = {AudioCategory::Voice, 1.0f, false};
        m_CategoryVolumes[AudioCategory::UI] = {AudioCategory::UI, 1.0f, false};
        m_CategoryVolumes[AudioCategory::Custom] = {AudioCategory::Custom, 1.0f, false};
    }

    MiniAudioDevice::~MiniAudioDevice() {
        Shutdown();
    }

    bool MiniAudioDevice::Initialize() {
        if (m_Initialized) {
            Logger::Warn("AudioDevice already initialized");
            return true;
        }

        m_Engine = new ma_engine;

        const ma_result result = ma_engine_init(nullptr, m_Engine);
        if (result != MA_SUCCESS) {
            Logger::Error("Failed to initialize miniaudio engine");
            delete m_Engine;
            m_Engine = nullptr;
            return false;
        }

        m_Initialized = true;
        Logger::Info("Audio system initialized with miniaudio");

        return true;
    }

    void MiniAudioDevice::Shutdown() {
        if (!m_Initialized) return;

        m_Sources.clear();

        if (m_Engine) {
            ma_engine_uninit(m_Engine);
            delete m_Engine;
            m_Engine = nullptr;
        }

        m_Initialized = false;
        Logger::Info("Audio system shut down");
    }

    Ref<AudioSource> MiniAudioDevice::CreateSource(const String &filepath, const AudioSourceConfig &config) {
        if (!m_Initialized) {
            Logger::Error("Cannot create audio source: device not initialized");
            return nullptr;
        }

        AudioSourceHandle handle = m_NextHandle++;
        auto source = MakeRef<MiniAudioSource>(m_Engine, filepath, config, handle);

        if (source && source->GetMaSound()) {
            m_Sources[handle] = source;
            return source;
        }

        Logger::Error(Format("Failed to create audio source from: {}", filepath));
        return nullptr;
    }

    void MiniAudioDevice::DestroySource(const AudioSourceHandle handle) {
        const auto it = m_Sources.find(handle);
        if (it != m_Sources.end())
            m_Sources.erase(it);
    }

    void MiniAudioDevice::SetListenerPosition(const Vec3 &position) {
        m_ListenerConfig.Position = position;
        if (m_Engine)
            ma_engine_listener_set_position(m_Engine, 0, position.x, position.y, position.z);
    }

    void MiniAudioDevice::SetListenerVelocity(const Vec3 &velocity) {
        m_ListenerConfig.Velocity = velocity;
        if (m_Engine)
            ma_engine_listener_set_velocity(m_Engine, 0, velocity.x, velocity.y, velocity.z);
    }

    void MiniAudioDevice::SetListenerOrientation(const Vec3 &forward, const Vec3 &up) {
        m_ListenerConfig.Forward = forward;
        m_ListenerConfig.Up = up;
        if (m_Engine) {
            ma_engine_listener_set_direction(m_Engine, 0, forward.x, forward.y, forward.z);
            ma_engine_listener_set_world_up(m_Engine, 0, up.x, up.y, up.z);
        }
    }

    AudioListenerConfig MiniAudioDevice::GetListenerConfig() const {
        return m_ListenerConfig;
    }

    void MiniAudioDevice::SetMasterVolume(const float volume) {
        m_MasterVolume = glm::clamp(volume, 0.0f, 1.0f);
        if (m_Engine)
            ma_engine_set_volume(m_Engine, m_MasterVolume);
    }

    float MiniAudioDevice::GetMasterVolume() const {
        return m_MasterVolume;
    }

    void MiniAudioDevice::SetCategoryVolume(const AudioCategory category, const float volume) {
        m_CategoryVolumes[category].Volume = glm::clamp(volume, 0.0f, 1.0f);
        ApplyCategoryVolumes();
    }

    float MiniAudioDevice::GetCategoryVolume(const AudioCategory category) const {
        const auto it = m_CategoryVolumes.find(category);
        return it != m_CategoryVolumes.end() ? it->second.Volume : 1.0f;
    }

    void MiniAudioDevice::MuteCategory(const AudioCategory category, const bool mute) {
        m_CategoryVolumes[category].Muted = mute;
        ApplyCategoryVolumes();
    }

    bool MiniAudioDevice::IsCategoryMuted(const AudioCategory category) const {
        const auto it = m_CategoryVolumes.find(category);
        return it != m_CategoryVolumes.end() ? it->second.Muted : false;
    }

    void MiniAudioDevice::PauseAll() {
        m_AllPaused = true;
        for (const auto &source: m_Sources | std::views::values) {
            if (source->IsPlaying())
                source->Pause();
        }
    }

    void MiniAudioDevice::ResumeAll() {
        m_AllPaused = false;
        for (const auto &source: m_Sources | std::views::values) {
            if (source->IsPaused())
                source->Resume();
        }
    }

    void MiniAudioDevice::StopAll() {
        for (const auto &source: m_Sources | std::views::values)
            source->Stop();
    }

    AudioDeviceInfo MiniAudioDevice::GetDeviceInfo() const {
        AudioDeviceInfo info;

        if (m_Engine) {
            ma_device *device = ma_engine_get_device(m_Engine);
            if (device) {
                info.Name = device->playback.name;
                info.SampleRate = device->sampleRate;
                info.Channels = device->playback.channels;
                info.IsDefault = true;
            }
        }

        return info;
    }

    Vector<AudioDeviceInfo> MiniAudioDevice::GetAvailableDevices() const {
        Vector<AudioDeviceInfo> devices;

        ma_context context;
        if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
            return devices;

        ma_device_info *pPlaybackInfos;
        ma_uint32 playbackCount;

        if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, nullptr, nullptr) == MA_SUCCESS) {
            for (ma_uint32 i = 0; i < playbackCount; ++i) {
                AudioDeviceInfo info;
                info.Name = pPlaybackInfos[i].name;
                info.IsDefault = pPlaybackInfos[i].isDefault;
                devices.push_back(info);
            }
        }

        ma_context_uninit(&context);
        return devices;
    }

    bool MiniAudioDevice::SelectDevice(const String &deviceName) {
        Logger::Warn("Device selection not implemented for miniaudio backend");
        return false;
    }

    void MiniAudioDevice::Update() {
        if (!m_Initialized) return;

        // Nettoyer les sources terminées
        Vector<AudioSourceHandle> toRemove;
        for (auto &[handle, source]: m_Sources)
            if (source->IsStopped() && !source->IsLooping())
                toRemove.push_back(handle);

        for (auto handle: toRemove)
            m_Sources.erase(handle);
    }

    void MiniAudioDevice::ApplyCategoryVolumes() {
        for (auto &[handle, source]: m_Sources) {
            float effectiveVolume = CalculateEffectiveVolume(source->GetCategory());
        }
    }

    float MiniAudioDevice::CalculateEffectiveVolume(const AudioCategory category) const {
        float volume = m_MasterVolume;

        auto it = m_CategoryVolumes.find(category);
        if (it != m_CategoryVolumes.end()) {
            if (it->second.Muted)
                return 0.0f;

            volume *= it->second.Volume;
        }

        return volume;
    }
}
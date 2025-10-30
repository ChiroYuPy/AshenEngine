#include "Ashen/Audio/MiniAudioImpl/MiniAudioSource.h"
#include "Ashen/Core/Logger.h"

#include <miniaudio.h>

namespace ash {
    MiniAudioSource::MiniAudioSource(ma_engine *engine, const String &filepath, const AudioSourceConfig &config,
                                     const AudioSourceHandle handle)
        : m_Engine(engine)
          , m_Sound(new ma_sound)
          , m_Handle(handle)
          , m_Type(config.Type)
          , m_Category(AudioCategory::SFX)
          , m_Volume(config.Volume)
          , m_Pitch(config.Pitch)
          , m_Loop(config.Loop)
          , m_Spatial(config.Spatial)
          , m_Position(config.Position)
          , m_Velocity(config.Velocity)
          , m_MinDistance(config.MinDistance)
          , m_MaxDistance(config.MaxDistance)
          , m_Rolloff(config.Rolloff)
          , m_Filepath(filepath) {
        ma_uint32 flags = 0;
        if (config.Type == AudioSourceType::Streaming)
            flags |= MA_SOUND_FLAG_STREAM;

        if (!config.Spatial)
            flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;

        const ma_result result = ma_sound_init_from_file(m_Engine, filepath.c_str(), flags, nullptr, nullptr, m_Sound);

        if (result != MA_SUCCESS) {
            Logger::Error(Format("Failed to load audio file: {} (error code: {})", filepath, static_cast<int>(result)));
            delete m_Sound;
            m_Sound = nullptr;
            return;
        }

        Logger::Debug(Format("Audio file loaded: {}", filepath));

        SetVolume(config.Volume);
        SetPitch(config.Pitch);
        SetLooping(config.Loop);

        if (config.Spatial) {
            SetPosition(config.Position);
            SetVelocity(config.Velocity);
            SetMinDistance(config.MinDistance);
            SetMaxDistance(config.MaxDistance);
            SetRolloff(config.Rolloff);
        }
    }

    MiniAudioSource::~MiniAudioSource() {
        if (m_Sound) {
            ma_sound_uninit(m_Sound);
            delete m_Sound;
        }
    }

    void MiniAudioSource::Play() {
        if (m_Sound) {
            if (ma_sound_at_end(m_Sound))
                ma_sound_seek_to_pcm_frame(m_Sound, 0);

            ma_sound_start(m_Sound);
        }
    }

    void MiniAudioSource::Pause() {
        if (m_Sound && IsPlaying())
            ma_sound_stop(m_Sound);
    }

    void MiniAudioSource::Stop() {
        if (m_Sound) {
            ma_sound_stop(m_Sound);
            ma_sound_seek_to_pcm_frame(m_Sound, 0);
        }
    }

    void MiniAudioSource::Resume() {
        if (m_Sound && !IsPlaying())
            ma_sound_start(m_Sound);
    }

    AudioState MiniAudioSource::GetState() const {
        if (!m_Sound) return AudioState::Stopped;

        if (ma_sound_is_playing(m_Sound))
            return AudioState::Playing;

        if (ma_sound_at_end(m_Sound))
            return AudioState::Stopped;

        return AudioState::Paused;
    }

    bool MiniAudioSource::IsPlaying() const {
        return m_Sound && ma_sound_is_playing(m_Sound);
    }

    bool MiniAudioSource::IsPaused() const {
        return GetState() == AudioState::Paused;
    }

    bool MiniAudioSource::IsStopped() const {
        return GetState() == AudioState::Stopped;
    }

    void MiniAudioSource::SetPlaybackPosition(const float seconds) {
        if (!m_Sound) return;

        ma_uint32 sampleRate;
        ma_sound_get_data_format(m_Sound, nullptr, nullptr, &sampleRate, nullptr, 0);
        const ma_uint64 frameIndex = seconds * static_cast<float>(sampleRate);
        ma_sound_seek_to_pcm_frame(m_Sound, frameIndex);
    }

    float MiniAudioSource::GetPlaybackPosition() const {
        if (!m_Sound) return 0.0f;

        float cursor;
        ma_sound_get_cursor_in_seconds(m_Sound, &cursor);
        return cursor;
    }

    float MiniAudioSource::GetDuration() const {
        if (!m_Sound) return 0.0f;

        float length;
        ma_sound_get_length_in_seconds(m_Sound, &length);
        return length;
    }

    void MiniAudioSource::SetVolume(const float volume) {
        m_Volume = glm::clamp(volume, 0.0f, 1.0f);
        if (m_Sound)
            ma_sound_set_volume(m_Sound, m_Volume);
    }

    float MiniAudioSource::GetVolume() const {
        return m_Volume;
    }

    void MiniAudioSource::SetPitch(const float pitch) {
        m_Pitch = glm::max(pitch, 0.01f);
        if (m_Sound)
            ma_sound_set_pitch(m_Sound, m_Pitch);
    }

    float MiniAudioSource::GetPitch() const {
        return m_Pitch;
    }

    void MiniAudioSource::SetLooping(const bool loop) {
        m_Loop = loop;
        if (m_Sound)
            ma_sound_set_looping(m_Sound, loop ? MA_TRUE : MA_FALSE);
    }

    bool MiniAudioSource::IsLooping() const {
        return m_Loop;
    }

    void MiniAudioSource::SetSpatial(const bool spatial) {
        m_Spatial = spatial;
        if (m_Sound)
            ma_sound_set_spatialization_enabled(m_Sound, spatial ? MA_TRUE : MA_FALSE);
    }

    bool MiniAudioSource::IsSpatial() const {
        return m_Spatial;
    }

    void MiniAudioSource::SetPosition(const Vec3 &position) {
        m_Position = position;
        if (m_Sound && m_Spatial)
            ma_sound_set_position(m_Sound, position.x, position.y, position.z);
    }

    Vec3 MiniAudioSource::GetPosition() const {
        return m_Position;
    }

    void MiniAudioSource::SetVelocity(const Vec3 &velocity) {
        m_Velocity = velocity;
        if (m_Sound && m_Spatial)
            ma_sound_set_velocity(m_Sound, velocity.x, velocity.y, velocity.z);
    }

    Vec3 MiniAudioSource::GetVelocity() const {
        return m_Velocity;
    }

    void MiniAudioSource::SetMinDistance(const float distance) {
        m_MinDistance = glm::max(distance, 0.0f);
        if (m_Sound && m_Spatial)
            ma_sound_set_min_distance(m_Sound, m_MinDistance);
    }

    float MiniAudioSource::GetMinDistance() const {
        return m_MinDistance;
    }

    void MiniAudioSource::SetMaxDistance(const float distance) {
        m_MaxDistance = glm::max(distance, m_MinDistance);
        if (m_Sound && m_Spatial)
            ma_sound_set_max_distance(m_Sound, m_MaxDistance);
    }

    float MiniAudioSource::GetMaxDistance() const {
        return m_MaxDistance;
    }

    void MiniAudioSource::SetRolloff(const float rolloff) {
        m_Rolloff = glm::max(rolloff, 0.0f);
        if (m_Sound && m_Spatial)
            ma_sound_set_rolloff(m_Sound, m_Rolloff);
    }

    float MiniAudioSource::GetRolloff() const {
        return m_Rolloff;
    }

    void MiniAudioSource::SetCategory(const AudioCategory category) {
        m_Category = category;
    }

    AudioCategory MiniAudioSource::GetCategory() const {
        return m_Category;
    }
}
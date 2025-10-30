#ifndef ASHEN_AUDIOTYPES_H
#define ASHEN_AUDIOTYPES_H

#include "Ashen/Core/Types.h"
#include "Ashen/Math/Math.h"

namespace ash {
    enum class AudioFormat {
        Unknown,
        WAV,
        MP3,
        OGG,
        FLAC
    };

    enum class AudioState {
        Stopped,
        Playing,
        Paused
    };

    enum class AudioSourceType {
        Static,
        Streaming
    };

    struct AudioSourceConfig {
        AudioSourceType Type = AudioSourceType::Static;
        bool Loop = false;
        float Volume = 1.0f;
        float Pitch = 1.0f;
        bool Spatial = false;
        Vec3 Position = Vec3(0.0f);
        Vec3 Velocity = Vec3(0.0f);
        float MinDistance = 1.0f;
        float MaxDistance = 100.0f;
        float Rolloff = 1.0f;
    };

    struct AudioListenerConfig {
        Vec3 Position = Vec3(0.0f);
        Vec3 Velocity = Vec3(0.0f);
        Vec3 Forward = Vec3(0.0f, 0.0f, -1.0f);
        Vec3 Up = Vec3(0.0f, 1.0f, 0.0f);
    };

    struct AudioDeviceInfo {
        std::string Name;
        uint32_t SampleRate;
        uint32_t Channels;
        bool IsDefault;
    };

    using AudioSourceHandle = uint32_t;
    constexpr AudioSourceHandle INVALID_AUDIO_SOURCE = 0;

    enum class AudioCategory {
        Master,
        Music,
        SFX,
        Ambient,
        Voice,
        UI,
        Custom
    };

    struct AudioCategoryVolume {
        AudioCategory Category;
        float Volume = 1.0f;
        bool Muted = false;
    };
}

#endif // ASHEN_AUDIOTYPES_H
#pragma once

namespace Engine {

    const u8 NumSoundChannels = 8;

    struct SoundCueSpec {
        std::string soundFile;
        float volume = 1.0f;

        // Set by engine
        f32 length = -1.0f;
        u32 soundID = 0;
    };

    struct BackingTrackSpec {
        std::string soundFile;
        u32 soundID;
    };

    struct SoundChannelStatus {
        bool active;
        float length, current;
        std::string cue;
        float volume;

        u32 soundID;
    };

    struct SoundEngineStatus {
        SoundChannelStatus channels[NumSoundChannels];
        int queueSize;
    };

    class SoundEngine {
    public:

        static void Init();

        static void CreateSoundCue(const std::string& cue, SoundCueSpec spec);
        static void CueSound(const std::string& cue);

        static void CreateBackingTrack(const std::string& track, BackingTrackSpec spec);
        static void PlayTrack(const std::string& track);

        static SoundEngineStatus GetStatus();

        static void Update(double dt);

        static void Shutdown();
    };
}
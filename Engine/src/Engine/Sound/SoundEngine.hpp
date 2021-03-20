#pragma once

namespace Engine {

    const u8 NumSoundChannels = 8;

    struct SoundCueSpec {
        std::string soundFile;
    };

    struct BackingTrackSpec {
        std::string soundFile;
    };

    struct SoundChannelStatus {
        bool active;
        float length, current;
        std::string cue;
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
#include <enpch.hpp>

#include "SoundEngine.hpp"

#include "Engine/Sound/SoundDevice.hpp"
#include "Engine/Sound/SoundContext.hpp"
#include "Engine/Sound/SoundBuffer.hpp"
#include "Engine/Sound/SoundSource.hpp"
#include "Engine/Sound/SoundFileFormats.hpp"

namespace Engine {

    struct SoundEngineData {
        std::unordered_map<std::string, SoundCueSpec> soundCues; // should hold actual sound cues, not jus tthe spec?
        std::unordered_map<std::string, BackingTrackSpec> backingTracks;
        SoundEngineStatus status;

        std::queue<std::string> soundsToPlay;
    };

    static SoundEngineData s_SoundData;

    void SoundEngine::Init() {
        for (int n = 0; n < NumSoundChannels; n++) {
            s_SoundData.status.channels[n].current = 0;
            s_SoundData.status.channels[n].length = 1;
            s_SoundData.status.channels[n].active = false;
        }
        s_SoundData.status.queueSize = 0;
    }

    void SoundEngine::Update(double dt) {
        for (int n = 0; n < NumSoundChannels; n++) {
            auto& chan = s_SoundData.status.channels[n];

            if (chan.active) {
                chan.current += dt;
                if (chan.current > chan.length) {
                    // remove this sound from the channel.
                    // if there is another sound queued up, play that.
                    if (s_SoundData.soundsToPlay.size() > 0) {
                        chan.current = 0;
                        chan.length = 3;
                        chan.cue = s_SoundData.soundsToPlay.front();
                        s_SoundData.soundsToPlay.pop();
                    }
                    chan.active = false;
                }
            }
            else {
                // Check to see if a new sound can be added to this channel
                if (s_SoundData.soundsToPlay.size() > 0) {
                    chan.active = true;
                    chan.current = 0;
                    chan.length = 3;
                    chan.cue = s_SoundData.soundsToPlay.front();
                    s_SoundData.soundsToPlay.pop();
                }
            }
        }
        s_SoundData.status.queueSize = s_SoundData.soundsToPlay.size();
    }

    void SoundEngine::Shutdown() {
    }

    void SoundEngine::CreateSoundCue(const std::string& cue, SoundCueSpec spec) {
        // create sound
        auto sound = spec;

        s_SoundData.soundCues.emplace(cue, sound);
    }
    void SoundEngine::CueSound(const std::string& cue) {
        const auto& cues = s_SoundData.soundCues;
        if (cues.find(cue) == cues.end()) {
            ENGINE_LOG_WARN("Could not find sound cue [{0}]", cue);
            return;
        }

        auto sound = cues.at(cue);
        s_SoundData.soundsToPlay.push(cue);
        // play sound
    }

    void SoundEngine::CreateBackingTrack(const std::string& track, BackingTrackSpec spec) {
        s_SoundData.backingTracks.emplace(track, spec);
    }
    void SoundEngine::PlayTrack(const std::string& track) {
        const auto& tracks = s_SoundData.backingTracks;
        if (tracks.find(track) == tracks.end()) {
            ENGINE_LOG_WARN("Could not find sound track [{0}]", track);
            return;
        }

        auto sound = tracks.at(track);
        // play sound
    }

    SoundEngineStatus SoundEngine::GetStatus() {
        return s_SoundData.status;
    }
}
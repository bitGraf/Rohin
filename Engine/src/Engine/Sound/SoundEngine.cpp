#include <enpch.hpp>

#include "SoundEngine.hpp"

#include "Engine/Sound/SoundDevice.hpp"
#include "Engine/Sound/SoundContext.hpp"
#include "Engine/Sound/SoundBuffer.hpp"
#include "Engine/Sound/SoundSource.hpp"
#include "Engine/Sound/SoundFileFormats.hpp"
#include "Engine/Sound/SoundStream.hpp"
#include "Engine/Sound/SoundEffect.hpp"

namespace Engine {

    struct SoundQueueEntry {
        std::string cueName;

        math::vec3 position;
        math::vec3 velocty;
    };

    struct SoundEngineData {
        std::unordered_map<std::string, SoundCueSpec> soundCues; // should hold actual sound cues, not jus the spec?
        std::unordered_map<std::string, BackingTrackSpec> backingTracks;
        SoundEngineStatus status;

        std::queue<SoundQueueEntry> soundsToPlay;

        std::unordered_map<std::string, Ref<SoundBuffer>> loadedSounds;

        Ref<SoundDevice> device;
        Ref<SoundContext> context;

        Ref<SoundSource> sources[NumSoundChannels];
        Ref<SoundStream> stream;
    };

    static SoundEngineData s_SoundData;

    void SoundEngine::Init() {
        s_SoundData.device = SoundDevice::Create();
        s_SoundData.device->Open();

        s_SoundData.context = SoundContext::Create(s_SoundData.device);
        s_SoundData.context->MakeCurrent();

        for (int n = 0; n < NumSoundChannels; n++) {
            s_SoundData.status.channels[n].current = 0;
            s_SoundData.status.channels[n].length = 1;
            s_SoundData.status.channels[n].active = false;
            s_SoundData.status.channels[n].soundID = 0;

            s_SoundData.sources[n] = SoundSource::Create();
        }
        s_SoundData.status.queueSize = 0;

        s_SoundData.stream = SoundStream::Create("run_tree/Data/Sounds/tape.ogg");

        // create effects
        SoundEffect::Init(s_SoundData.device);
        SoundEffect::CreateReverbEffect("reverb_default");
        SoundEffect::CreateEffectSlot("slot_reverb");

        SoundEffect::BindEffectToSlot("reverb_default", "slot_reverb");
        s_SoundData.stream->SetEffectSlot(SoundEffect::GetEffectSlot("slot_reverb"));
    }

    void SoundEngine::Shutdown() {
        for (int n = 0; n < NumSoundChannels; n++) {
            s_SoundData.sources[n]->Destroy();
        }

        s_SoundData.context->MakeCurrent();
        s_SoundData.context->Destroy();

        s_SoundData.device->Close();
    }

    void SoundEngine::Update(double dt) {
        for (int n = 0; n < NumSoundChannels; n++) {
            auto& chan = s_SoundData.status.channels[n];

            if (chan.active) {
                chan.current += dt;
                if (chan.current > chan.length) {
                    chan.active = false; // mark as inactive
                    const auto& source = s_SoundData.sources[n];
                    source->Stop(); // or source->Pause();??
                }
            }
        }

        // loop through and add new sounds to inactive channels
        for (int n = 0; n < NumSoundChannels; n++) {
            if (s_SoundData.soundsToPlay.size() > 0) {
                auto& chan = s_SoundData.status.channels[n];

                if (!chan.active) {
                    // Get next sound to play
                    auto cueInfo = s_SoundData.soundsToPlay.front();
                    s_SoundData.soundsToPlay.pop();
                    chan.cue = cueInfo.cueName;

                    const auto& soundSpec = s_SoundData.soundCues[chan.cue];
                    auto source = s_SoundData.sources[n];

                    chan.active = true;
                    chan.current = 0;
                    chan.length = soundSpec.length;
                    chan.soundID = soundSpec.soundID;
                    chan.volume = soundSpec.volume;

                    source->SetBuffer(chan.soundID);
                    source->SetGain(chan.volume);
                    source->SetPosition(cueInfo.position.x, cueInfo.position.y, cueInfo.position.z);
                    source->SetVelocity(cueInfo.velocty.x, cueInfo.velocty.y, cueInfo.velocty.z);
                    source->Play();
                }
            }
            else {
                break;
            }
        }

        s_SoundData.status.queueSize = s_SoundData.soundsToPlay.size();

        s_SoundData.stream->UpdateStream(dt);
    }

    void SoundEngine::StartSteam() {
        s_SoundData.stream->PlayStream();
    }

    void SoundEngine::StopSteam() {
        s_SoundData.stream->StopStream();
    }

    bool LoadSound(const std::string& filename) {
        if (s_SoundData.loadedSounds.find(filename) != s_SoundData.loadedSounds.end()) {
            // sound already loaded
            return true;
        }

        size_t extLoc = filename.find_last_of('.');
        if (extLoc == std::string::npos) {
            ENGINE_LOG_ERROR("Tried to load sound file [{0}], but could not determine file type", filename);
            return false;
        }
        std::string ext = filename.substr(extLoc);
        if (ext.compare(".ogg") == 0) {
            // load .ogg
            u8 channels, bitsPerSample;
            s32 sampleRate, size;
            short* data = load_ogg(filename, channels, sampleRate, bitsPerSample, size);
            if (data == nullptr) return false;
            auto format = GetSoundFormat(channels, bitsPerSample);
            s_SoundData.loadedSounds.emplace(filename, SoundBuffer::Create(format, data, size, sampleRate));
            free(data); // I think this is okay
            ENGINE_LOG_INFO("Loaded [{0}] ({4:.1f}Kb of {1} {2}-bit channels at {3}Hz)", 
                filename, channels, bitsPerSample, sampleRate, size/1024.0f);
        } else if (ext.compare(".wav") == 0) {
            // load .wav
            u8 channels, bitsPerSample;
            s32 sampleRate, size;
            char* data = load_wav(filename, channels, sampleRate, bitsPerSample, size);
            auto format = GetSoundFormat(channels, bitsPerSample);
            s_SoundData.loadedSounds.emplace(filename, SoundBuffer::Create(format, data, size, sampleRate));
            free(data); // I think this is okay
            ENGINE_LOG_INFO("Loaded [{0}] ({4:.1f}Kb of {1} {2}-bit channels at {3}Hz)",
                filename, channels, bitsPerSample, sampleRate, size/1024.0f);
        }
        else {
            ENGINE_LOG_ERROR("Cannot load sound file of {0} type", ext);
            return false;
        }

        return true;
    }

    void SoundEngine::CreateSoundCue(const std::string& cue, SoundCueSpec spec) {
        // create sound
        if (!LoadSound(spec.soundFile)) {
            ENGINE_LOG_ERROR("Could not create sound cue [{0}]", cue);
            return;
        }

        auto newSpec = spec;
        if (spec.length <= 0.0f) // allow for manual overrides
            newSpec.length = s_SoundData.loadedSounds[spec.soundFile]->GetLength_s();
        if (spec.soundID <= 0) // allow for manual overrides
            newSpec.soundID = s_SoundData.loadedSounds[spec.soundFile]->GetNativeID();

        s_SoundData.soundCues.emplace(cue, newSpec);
    }
    void SoundEngine::CueSound(const std::string& cue, math::vec3 position) {
        const auto& cues = s_SoundData.soundCues;
        if (cues.find(cue) == cues.end()) {
            ENGINE_LOG_WARN("Could not find sound cue [{0}]", cue);
            return;
        }

        s_SoundData.soundsToPlay.push({ cue, position, {0,0,0} });
    }
    void SoundEngine::CueSound(const std::string& cue) {
        CueSound(cue, math::vec3());
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

    void SoundEngine::SetListenerPosition(math::vec3 position) {
        s_SoundData.context->SetListenerPosition(position);
    }

    void SoundEngine::SetListenerVelocity(math::vec3 velocity) {
        s_SoundData.context->SetListenerVelocity(velocity);
    }

    void SoundEngine::SetListenerOrientation(math::vec3 at, math::vec3 up) {
        s_SoundData.context->SetListenerOrientation(at, up);
    }
}
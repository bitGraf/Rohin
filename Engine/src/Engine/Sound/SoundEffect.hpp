#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Sound/SoundDevice.hpp"

namespace Engine {

    class SoundEffect {
    public:

        static void Init(Ref<SoundDevice> device);
        static void Destroy();

        static void CreateEffectSlot(const std::string& slot_name);
        static u32 GetEffectSlot(const std::string& slot_name);

        static void CreateReverbEffect(const std::string& reverb_name);
        static void CreateReverbEffect(const std::string& reverb_name,
            float Density, float Diffusion,
            float Gain, float GainHF, float GainLF,
            float DecayTime, float DecayHFRatio, float DecayLFRatio,
            float ReflectionsGain, float ReflectionsDelay, float ReflectionsPan[3],
            float LateReverbGain, float LateReverbDelay, float LateReverbPan[3],
            float EchoTime, float EchoDepth,
            float ModulationTime, float ModulationDepth,
            float AirAbsorptionGainHF,
            float HFReference, float LFReference,
            float RoomRolloffFactor,
            int DecayHFLimit);
        static void CreateReverbEffect(const std::string& reverb_name,
            float Density, float Diffusion,
            float Gain, float GainHF,
            float DecayTime, float DecayHFRatio,
            float ReflectionsGain, float ReflectionsDelay,
            float LateReverbGain, float LateReverbDelay,
            float AirAbsorptionGainHF,
            float RoomRolloffFactor,
            int DecayHFLimit);
        static u32 GetReverbEffect(const std::string& reverb_name);

        static void BindEffectToSlot(const std::string& effect_name, const std::string& slot_name);
    };
}
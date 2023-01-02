#include <enpch.hpp>

#include "Engine/Sound/SoundEffect.hpp"

#include "AL/al.h"
#include "AL/efx.h"
#include "AL/efx-presets.h"

/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

namespace rh {

    struct EffectData {
        std::unordered_map<std::string, ALuint> m_effects;
        std::unordered_map<std::string, ALuint> m_slots;
    };

    static EffectData s_EffectData;

    /* LoadEffect loads the given reverb properties into a new OpenAL effect
     * object, and returns the new effect ID. */
    static ALuint LoadEffect(const EFXEAXREVERBPROPERTIES *reverb)
    {
        ALuint effect = 0;
        ALenum err;

        /* Create the effect object and check if we can do EAX reverb. */
        alGenEffects(1, &effect);
        if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0)
        {
            printf("Using EAX Reverb\n");

            /* EAX Reverb is available. Set the EAX effect type then load the
             * reverb properties. */
            alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

            alEffectf(effect, AL_EAXREVERB_DENSITY, reverb->flDensity);
            alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
            alEffectf(effect, AL_EAXREVERB_GAIN, reverb->flGain);
            alEffectf(effect, AL_EAXREVERB_GAINHF, reverb->flGainHF);
            alEffectf(effect, AL_EAXREVERB_GAINLF, reverb->flGainLF);
            alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
            alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
            alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
            alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
            alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
            alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
            alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
            alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
            alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
            alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
            alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
            alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
            alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
            alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
            alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
            alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
            alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
            alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
        }
        else
        {
            printf("Using Standard Reverb\n");

            /* No EAX Reverb. Set the standard reverb effect type then load the
             * available reverb properties. */
            alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

            alEffectf(effect, AL_REVERB_DENSITY, reverb->flDensity);
            alEffectf(effect, AL_REVERB_DIFFUSION, reverb->flDiffusion);
            alEffectf(effect, AL_REVERB_GAIN, reverb->flGain);
            alEffectf(effect, AL_REVERB_GAINHF, reverb->flGainHF);
            alEffectf(effect, AL_REVERB_DECAY_TIME, reverb->flDecayTime);
            alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
            alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
            alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
            alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
            alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
            alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
            alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
            alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
        }

        /* Check if an error occured, and clean up if so. */
        err = alGetError();
        if (err != AL_NO_ERROR)
        {
            fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
            if (alIsEffect(effect))
                alDeleteEffects(1, &effect);
            return 0;
        }

        return effect;
    }

    void SoundEffect::Init(Ref<SoundDevice> device) {
        if (!alcIsExtensionPresent((ALCdevice*)device->GetNativeDevice(), "ALC_EXT_EFX")) {
            ENGINE_LOG_CRITICAL("EFX extension not found!");
            return;
        }

        /* Define a macro to help load the function pointers. */
#define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))
        LOAD_PROC(LPALGENEFFECTS, alGenEffects);
        LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
        LOAD_PROC(LPALISEFFECT, alIsEffect);
        LOAD_PROC(LPALEFFECTI, alEffecti);
        LOAD_PROC(LPALEFFECTIV, alEffectiv);
        LOAD_PROC(LPALEFFECTF, alEffectf);
        LOAD_PROC(LPALEFFECTFV, alEffectfv);
        LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
        LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
        LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
        LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

        LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
        LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
        LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
        LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
        LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
        LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
        LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
        LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
        LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
        LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
        LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC
    }

    void SoundEffect::Destroy() {
        for (auto slot : s_EffectData.m_slots) {
            alDeleteAuxiliaryEffectSlots(1, &slot.second);
        }
        for (auto effect : s_EffectData.m_effects) {
            alDeleteEffects(1, &effect.second);
        }
    }

    void SoundEffect::CreateEffectSlot(const std::string& slot_name) {
        ALuint slot = 0;
        alGenAuxiliaryEffectSlots(1, &slot);

        s_EffectData.m_slots.emplace(slot_name, slot);
    }

    u32 SoundEffect::GetEffectSlot(const std::string& slot_name) {
        if (s_EffectData.m_slots.find(slot_name) == s_EffectData.m_slots.end()) {
            ENGINE_LOG_WARN("Effect slot {0} does not exist!", slot_name);
            return 0;
        }

        return s_EffectData.m_slots.at(slot_name);
    }

    void SoundEffect::CreateReverbEffect(const std::string& reverb_name) {
        EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;
        auto effect = LoadEffect(&reverb);

        s_EffectData.m_effects.emplace(reverb_name, effect);
    }

    void SoundEffect::CreateReverbEffect(const std::string& reverb_name,
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
        int DecayHFLimit) {

        EFXEAXREVERBPROPERTIES reverb;
        reverb.flDensity = Density;
        reverb.flDiffusion = Diffusion;
        reverb.flGain = Gain;
        reverb.flGainHF = GainHF;
        reverb.flGainLF = GainLF;
        reverb.flDecayTime = DecayTime;
        reverb.flDecayHFRatio = DecayHFRatio;
        reverb.flDecayLFRatio = DecayLFRatio;
        reverb.flReflectionsGain = ReflectionsGain;
        reverb.flReflectionsDelay = ReflectionsDelay;
        memcpy(reverb.flReflectionsPan, ReflectionsPan, sizeof(reverb.flReflectionsPan));
        reverb.flLateReverbGain = LateReverbGain;
        reverb.flLateReverbDelay = LateReverbDelay;
        memcpy(reverb.flLateReverbPan, LateReverbPan, sizeof(reverb.flLateReverbPan));
        reverb.flEchoTime = EchoTime;
        reverb.flEchoDepth = EchoDepth;
        reverb.flModulationTime = ModulationTime;
        reverb.flModulationDepth = ModulationDepth;
        reverb.flAirAbsorptionGainHF = AirAbsorptionGainHF;
        reverb.flHFReference = HFReference;
        reverb.flLFReference = LFReference;
        reverb.flRoomRolloffFactor = RoomRolloffFactor;
        reverb.iDecayHFLimit = DecayHFLimit;

        auto effect = LoadEffect(&reverb);

        s_EffectData.m_effects.emplace(reverb_name, effect);
    }

    void SoundEffect::CreateReverbEffect(const std::string& reverb_name,
        float Density, float Diffusion,
        float Gain, float GainHF,
        float DecayTime, float DecayHFRatio,
        float ReflectionsGain, float ReflectionsDelay,
        float LateReverbGain, float LateReverbDelay,
        float AirAbsorptionGainHF,
        float RoomRolloffFactor,
        int DecayHFLimit) {

        EFXEAXREVERBPROPERTIES reverb;
        reverb.flDensity = Density;
        reverb.flDiffusion = Diffusion;
        reverb.flGain = Gain;
        reverb.flGainHF = GainHF;
        reverb.flDecayTime = DecayTime;
        reverb.flDecayHFRatio = DecayHFRatio;
        reverb.flReflectionsGain = ReflectionsGain;
        reverb.flReflectionsDelay = ReflectionsDelay;
        reverb.flLateReverbGain = LateReverbGain;
        reverb.flLateReverbDelay = LateReverbDelay;
        reverb.flAirAbsorptionGainHF = AirAbsorptionGainHF;
        reverb.flRoomRolloffFactor = RoomRolloffFactor;
        reverb.iDecayHFLimit = DecayHFLimit;

        auto effect = LoadEffect(&reverb);

        s_EffectData.m_effects.emplace(reverb_name, effect);
    }



    u32  SoundEffect::GetReverbEffect(const std::string& reverb_name) {
        if (s_EffectData.m_effects.find(reverb_name) == s_EffectData.m_effects.end()) {
            ENGINE_LOG_WARN("Reverb {0} does not exist!", reverb_name);
            return 0;
        }

        return s_EffectData.m_effects.at(reverb_name);
    }

    void SoundEffect::BindEffectToSlot(const std::string& effect_name, const std::string& slot_name) {
        auto effect = s_EffectData.m_effects.at(effect_name);
        auto slot   = s_EffectData.m_slots.at(slot_name);
        /* Tell the effect slot to use the loaded effect object. Note that the this
         * effectively copies the effect properties. You can modify or delete the
         * effect object afterward without affecting the effect slot.
         */
        alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
        assert(alGetError() == AL_NO_ERROR && "Failed to set effect slot");
    }
}
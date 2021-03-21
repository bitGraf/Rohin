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

namespace Engine {

    struct EffectData {
        ALuint effect;
        ALuint slot;
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

        EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;
        s_EffectData.effect = LoadEffect(&reverb);

        s_EffectData.slot = 0;
        alGenAuxiliaryEffectSlots(1, &s_EffectData.slot);

        /* Tell the effect slot to use the loaded effect object. Note that the this
         * effectively copies the effect properties. You can modify or delete the
         * effect object afterward without affecting the effect slot.
         */
        alAuxiliaryEffectSloti(s_EffectData.slot, AL_EFFECTSLOT_EFFECT, (ALint)s_EffectData.effect);
        assert(alGetError() == AL_NO_ERROR && "Failed to set effect slot");
    }

    void SoundEffect::Destroy() {
        alDeleteAuxiliaryEffectSlots(1, &s_EffectData.slot);
        alDeleteEffects(1, &s_EffectData.effect);
    }

    u32 SoundEffect::GetReverbSlot() {
        return s_EffectData.slot;
    }
}
/*
* This file is part of the OpenSupaplex distribution (https://github.com/sergiou87/open-supaplex).
* Copyright (c) 2020 Sergio Padrino
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "../audio.h"
#include "../logging.h"

#include <maxmod9.h>

#include "soundbank.h"

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;

mm_word musicID = MOD_MUSIC_BEEP;
uint8_t gIsMusicInitialized = 0;
uint8_t gMusicVolume = kMaxVolume;

mm_word gSoundEffectsAdlib[SoundEffectCount] = {
    SFX_EXPLOSION_ADLIB,
    SFX_INFOTRON_ADLIB,
    SFX_PUSH_ADLIB,
    SFX_FALL_ADLIB,
    SFX_BUG_ADLIB,
    SFX_BASE_ADLIB,
    SFX_EXIT_ADLIB,
};

mm_word gSoundEffectsBeep[SoundEffectCount] = {
    SFX_EXPLOSION_BEEP,
    SFX_INFOTRON_BEEP,
    SFX_PUSH_BEEP,
    SFX_FALL_BEEP,
    SFX_BUG_BEEP,
    SFX_BASE_BEEP,
    SFX_EXIT_BEEP,
};

mm_word gSoundEffectsBlaster[SoundEffectCount] = {
    SFX_EXPLOSION_BLASTER,
    SFX_INFOTRON_BLASTER,
    SFX_PUSH_BLASTER,
    SFX_FALL_BLASTER,
    SFX_BUG_BLASTER,
    SFX_BASE_BLASTER,
    SFX_EXIT_BLASTER,
};

mm_word gSoundEffectsSamples[SoundEffectCount] = {
    SFX_EXPLOSION_SAMPLE,
    SFX_INFOTRON_SAMPLE,
    SFX_PUSH_SAMPLE,
    SFX_FALL_SAMPLE,
    SFX_BUG_SAMPLE,
    SFX_BASE_SAMPLE,
    SFX_EXIT_SAMPLE,
};

mm_word *gSoundEffects = NULL;
uint8_t gIsSoundEffectsInitialized = 0;
uint8_t gSoundEffectsVolume = kMaxVolume;


void loadMusic(void);
void destroyMusic(void);

void loadSounds(void);
void destroySounds(void);

int8_t initializeAudio()
{
    mmInitDefault("nitro:/soundbank.bin");
    spLogInfo("Initialized maxmod driver correctly");
    return 0;
}

void destroyAudio()
{
    stopMusic();
}

void loadMusic()
{
    if (gIsMusicInitialized == 1)
    {
        return;
    }

    switch (musType)
    {
        case SoundTypeRoland:
            musicID = MOD_MUSIC_ROLAND;
            break;
        case SoundTypeAdlib:
        case SoundTypeSoundBlaster:
            musicID = MOD_MUSIC_ADLIB;
            break;
        case SoundTypeInternalSamples:
        case SoundTypeInternalStandard:
            musicID = MOD_MUSIC_BEEP;
            break;
        case SoundTypeNone:
            return;
    }

    mmLoad(musicID);
    gIsMusicInitialized = 1;
}

void loadSounds()
{
    if (gIsSoundEffectsInitialized == 1)
    {
        return;
    }

    switch (sndType)
    {
        case SoundTypeRoland:
        case SoundTypeSoundBlaster:
            gSoundEffects = gSoundEffectsBlaster;
            break;
        case SoundTypeAdlib:
            gSoundEffects = gSoundEffectsAdlib;
            break;
        case SoundTypeInternalSamples:
            gSoundEffects = gSoundEffectsSamples;
            break;
        case SoundTypeInternalStandard:
            gSoundEffects = gSoundEffectsBeep;
            break;
        case SoundTypeNone:
            return;
    }

    for (int i = 0; i < SoundEffectCount; ++i)
    {
        mmLoadEffect(gSoundEffects[i]);
    }

    gIsSoundEffectsInitialized = 1;
}

void destroySounds()
{
    if (gIsSoundEffectsInitialized == 0)
    {
        return;
    }

    for (int i = 0; i < SoundEffectCount; ++i)
    {
        mmUnloadEffect(gSoundEffects[i]);
    }

    gIsSoundEffectsInitialized = 0;
}

void setSoundType(SoundType musicType, SoundType effectsType)
{
    destroyMusic();
    destroySounds();

    sndType = effectsType;
    musType = musicType;

    loadMusic();
    loadSounds();
}

uint8_t getMusicVolume(void)
{
    return gMusicVolume;
}

void setMusicVolume(uint8_t volume)
{
    gMusicVolume = volume;
    mmSetModuleVolume((volume * 1024) / 10);
}

uint8_t getSoundEffectsVolume(void)
{
    return gSoundEffectsVolume;
}

void setSoundEffectsVolume(uint8_t volume)
{
    gSoundEffectsVolume = volume;
}

void playMusic()
{
    if (gIsMusicInitialized == 0)
    {
        return;
    }

    mmStart(musicID, MM_PLAY_LOOP);
}

void stopMusic()
{
    if (gIsMusicInitialized == 0)
    {
        return;
    }

    mmStop();
}

void destroyMusic()
{
    if (gIsMusicInitialized == 0)
    {
        return;
    }

    stopMusic();

    mmUnload(musicID);
    gIsMusicInitialized = 0;
}

void playSoundEffect(SoundEffect soundEffect)
{
    if (gIsSoundEffectsInitialized == 0)
    {
        return;
    }

    mm_sfxhand handle = mmEffect(gSoundEffects[soundEffect]);
    mmEffectVolume(handle, (gSoundEffectsVolume * 255) / 10 );
    mmEffectRelease(handle);
}

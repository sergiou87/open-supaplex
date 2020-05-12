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

#include "audio.h"

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "logging.h"

#define kMaxSoundFilenameLength 256

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;

static const char *kRolandSoundFileNameSuffix = "roland";
static const char *kAdlibSoundFileNameSuffix = "adlib";
static const char *kBlasterSoundFileNameSuffix = "blaster";
static const char *kSamplesSoundFileNameSuffix = "sample";
static const char *kStandardSoundFileNameSuffix = "beep";

Mix_Music *gMusic = NULL;

Mix_Chunk *gSoundEffectChunks[SoundEffectCount];
char *gSoundEffectNames[SoundEffectCount] = {
    "explosion",
    "infotron",
    "push",
    "fall",
    "bug",
    "base",
    "exit",
};

int gCurrentSoundChannel = -1;

void loadMusic(void);
void destroyMusic(void);

void loadSounds(void);
void destroySounds(void);

#ifdef __vita__
static const char *kBaseAudioFolder = "app0:/audio";
#else
static const char *kBaseAudioFolder = "audio";
#endif

int8_t initializeAudio()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    // Keep buffer size as low as possible to prevent latency with sound effects.
    // In macOS I was able to set it to 512, but that caused a lot of issues playing music on Nintendo Switch.
    // 768 bytes seems to work on both platforms.
    // However, if it ever supports WASM, that needs a power of 2 as buffer size.
    //
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 768) == -1)
    {
        spLog("Mix_Init: Failed to open audio!\n");
        spLog("Mix_Init: %s\n", Mix_GetError());
        return 1;
    }

    int flags = MIX_INIT_OGG;
    int initted = Mix_Init(flags);
    if((initted & flags) != flags)
    {
        spLog("Mix_Init: Failed to init required ogg support!\n");
        spLog("Mix_Init: %s\n", Mix_GetError());
        return 1;
    }

    spLog("Audio initialized correctly");

    return 0;
}

void destroyAudio()
{
    stopMusic();
    // TODO: stop sounds
    Mix_Quit();
    Mix_CloseAudio();
}

void loadMusic()
{
    const char *musicSuffix = NULL;

    switch (musType)
    {
        case SoundTypeRoland:
            musicSuffix = kRolandSoundFileNameSuffix;
            break;
        case SoundTypeAdlib:
        case SoundTypeSoundBlaster:
            musicSuffix = kBlasterSoundFileNameSuffix;
            break;
        case SoundTypeInternalSamples:
        case SoundTypeInternalStandard:
            musicSuffix = kStandardSoundFileNameSuffix;
            break;
        case SoundTypeNone:
            return;
    }

    char filename[kMaxSoundFilenameLength] = "";
    snprintf(filename, kMaxSoundFilenameLength, "%s/music-%s.ogg", kBaseAudioFolder, musicSuffix);

    gMusic = Mix_LoadMUS(filename);

    if(gMusic == NULL)
    {
        spLog("Unable to load Ogg file: %s\n", Mix_GetError());
        return;
    }
}

void loadSounds()
{
    const char *effectsSuffix = NULL;

    switch (sndType)
    {
        case SoundTypeRoland:
        case SoundTypeSoundBlaster:
            effectsSuffix = kBlasterSoundFileNameSuffix;
            break;
        case SoundTypeAdlib:
            effectsSuffix = kAdlibSoundFileNameSuffix;
            break;
        case SoundTypeInternalSamples:
            effectsSuffix = kSamplesSoundFileNameSuffix;
            break;
        case SoundTypeInternalStandard:
            effectsSuffix = kStandardSoundFileNameSuffix;
            break;
        case SoundTypeNone:
            return;
    }

    char filename[kMaxSoundFilenameLength] = "";

    for (int i = 0; i < SoundEffectCount; ++i)
    {
        snprintf(filename, kMaxSoundFilenameLength, "%s/%s-%s.ogg", kBaseAudioFolder, gSoundEffectNames[i], effectsSuffix);
        gSoundEffectChunks[i] = Mix_LoadWAV(filename);
    }
}

void destroySounds()
{
    for (int i = 0; i < SoundEffectCount; ++i)
    {
        if (gSoundEffectChunks[i] != NULL)
        {
            Mix_FreeChunk(gSoundEffectChunks[i]);
        }
        gSoundEffectChunks[i] = NULL;
    }
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
    return roundf((float)Mix_VolumeMusic(-1) * 10 / SDL_MIX_MAXVOLUME);
}

void setMusicVolume(uint8_t volume)
{
    Mix_VolumeMusic(volume * SDL_MIX_MAXVOLUME / 10);
}

uint8_t getSoundEffectsVolume(void)
{
    return roundf((float)Mix_Volume(-1, -1) * 10 / SDL_MIX_MAXVOLUME);
}

void setSoundEffectsVolume(uint8_t volume)
{
    Mix_Volume(-1, volume * SDL_MIX_MAXVOLUME / 10);
}

void playMusic()
{
    if (gMusic == NULL)
    {
        return;
    }

    if(Mix_PlayMusic(gMusic, -1) == -1)
    {
        spLog("Unable to play Ogg file: %s\n", Mix_GetError());
        return;
    }

    Mix_ResumeMusic();
}

void stopMusic()
{
    if (gMusic != NULL)
    {
        Mix_PauseMusic();
    }
}

void destroyMusic()
{
    stopMusic();

    if (gMusic != NULL)
    {
        Mix_FreeMusic(gMusic);
    }

    gMusic = NULL;
}

void playSoundEffect(SoundEffect soundEffect)
{
    if (soundEffect >= SoundEffectCount)
    {
        return;
    }
    
    Mix_HaltChannel(gCurrentSoundChannel);
    gCurrentSoundChannel = Mix_PlayChannel(-1, gSoundEffectChunks[soundEffect], 0);
}

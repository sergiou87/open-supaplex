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

#if defined(_3DS)
#include <3ds.h>
#endif

#include <math.h>

#if HAVE_SDL2
#include <SDL2/SDL.h>
#if TARGET_OS_MAC
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#elif HAVE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif

#include "../logging.h"

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
uint8_t gIsAudioInitialized = 0;

void loadMusic(void);
void destroyMusic(void);

void loadSounds(void);
void destroySounds(void);

#ifdef __vita__
static const char *kBaseAudioFolder = "app0:/audio";
#elif defined(_3DS)
static const char *kBaseAudioFolder = "sdmc:/OpenSupaplex/audio";
#elif defined(__PSL1GHT__)
static const char *kBaseAudioFolder = "/dev_hdd0/game/" PS3APPID "/USRDIR/audio";
#elif defined(__WII__)
static const char *kBaseAudioFolder = "/apps/OpenSupaplex/audio";
#elif defined(__WIIU__)
static const char *kBaseAudioFolder = "fs:/vol/external01/wiiu/apps/OpenSupaplex/audio";
#else
static const char *kBaseAudioFolder = "audio";
#endif

// Keep buffer size as low as possible to prevent latency with sound effects.
// In macOS I was able to set it to 512, but that caused a lot of issues playing music on Nintendo Switch.
// If it ever supports WASM, that needs a power of 2 as buffer size.
//
#if defined(__SWITCH__) || defined(__WIIU__) || defined(__WII__) || (defined(_3DS) && NEW3DS)
const int kAudioBufferSize = 1024;
#elif defined(_3DS) && NEW3DS
const int kAudioBufferSize = 768;
#else // macOS, PS Vita, PS3, PSP and Windows
// PS3 seems to ignore this and always use number_of_samples (256) * sizeof(float) * number_of_channels = 2048
// PSP and PS Vita only need this value to be a multiple of 64
const int kAudioBufferSize = 512;
#endif

// PSP and 3DS can't handle High quality audio, it kills performance
#if defined(_3DS) && !NEW3DS
static const char *kBaseAudioFolderSuffix = "lq"; // Low quality
static const int kSampleRate = 11025;
static const int kNumberOfChannels = 1;
#elif defined(__PSP__) || (defined(_3DS) && NEW3DS)
static const char *kBaseAudioFolderSuffix = "mq"; // Medium quality
static const int kSampleRate = 22050;
static const int kNumberOfChannels = 1;
#else
static const char *kBaseAudioFolderSuffix = "hq"; // High quality
static const int kSampleRate = 44100;
static const int kNumberOfChannels = 2;
#endif

int8_t initializeAudio()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

#if defined(_3DS)
    _Bool isN3DS;
	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
    {
		osSetSpeedupEnable(true);
        spLogInfo("Using New3DS speed up for better performance");
    }
#endif

    spLogInfo("Trying to initialize audio: %dHz, %d channels, format 0x%04x, %d bytes buffer", kSampleRate, kNumberOfChannels, MIX_DEFAULT_FORMAT, kAudioBufferSize);

    if (Mix_OpenAudio(kSampleRate, MIX_DEFAULT_FORMAT, kNumberOfChannels, kAudioBufferSize) == -1)
    {
        spLogInfo("Mix_OpenAudio: Failed to open audio!\n");
        spLogInfo("Mix_OpenAudio: %s\n", Mix_GetError());
        return 1;
    }

    int flags = MIX_INIT_OGG;
    int initted = Mix_Init(flags);
    if((initted & flags) != flags)
    {
        spLogInfo("Mix_Init: Failed to init required ogg support!\n");
        spLogInfo("Mix_Init: %s\n", Mix_GetError());
        return 1;
    }

    int sampleRate;
    Uint16 format;
    int channels;
    Mix_QuerySpec(&sampleRate, &format, &channels);
    
    spLogInfo("Audio initialized: %dHz, %d channels, format 0x%04x, %d bytes buffer", sampleRate, channels, format, kAudioBufferSize);
    gIsAudioInitialized = 1;

    return 0;
}

void destroyAudio()
{
    if (gIsAudioInitialized == 0)
    {
        return;
    }

    stopMusic();
    // TODO: stop sounds
    Mix_Quit();
    Mix_CloseAudio();
}

void loadMusic()
{
    if (gIsAudioInitialized == 0)
    {
        return;
    }
    
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
    snprintf(filename, kMaxSoundFilenameLength, "%s-%s/music-%s.ogg", kBaseAudioFolder, kBaseAudioFolderSuffix, musicSuffix);

    gMusic = Mix_LoadMUS(filename);

    if(gMusic == NULL)
    {
        spLogInfo("Unable to load music file: %s\n", Mix_GetError());
        return;
    }
}

void loadSounds()
{
    if (gIsAudioInitialized == 0)
    {
        return;
    }

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
        snprintf(filename, kMaxSoundFilenameLength, "%s-%s/%s-%s.ogg", kBaseAudioFolder, kBaseAudioFolderSuffix, gSoundEffectNames[i], effectsSuffix);
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
    if (gIsAudioInitialized == 0)
    {
        return;
    }
    
    destroyMusic();
    destroySounds();

    sndType = effectsType;
    musType = musicType;

    loadMusic();
    loadSounds();
}

uint8_t getMusicVolume(void)
{
    if (gIsAudioInitialized == 0)
    {
        return 0;
    }
    
    return roundf((float)Mix_VolumeMusic(-1) * 10 / SDL_MIX_MAXVOLUME);
}

void setMusicVolume(uint8_t volume)
{
    if (gIsAudioInitialized == 0)
    {
        return;
    }
    
    Mix_VolumeMusic(volume * SDL_MIX_MAXVOLUME / 10);
}

uint8_t getSoundEffectsVolume(void)
{
    if (gIsAudioInitialized == 0)
    {
        return 0;
    }
    
    return roundf((float)Mix_Volume(-1, -1) * 10 / SDL_MIX_MAXVOLUME);
}

void setSoundEffectsVolume(uint8_t volume)
{
    if (gIsAudioInitialized == 0)
    {
        return;
    }
    
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
        spLogInfo("Unable to play music file: %s\n", Mix_GetError());
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
    if (gIsAudioInitialized == 0)
    {
        return;
    }
    
    if (soundEffect >= SoundEffectCount)
    {
        return;
    }
    
    Mix_HaltChannel(gCurrentSoundChannel);
    gCurrentSoundChannel = Mix_PlayChannel(-1, gSoundEffectChunks[soundEffect], 0);
}

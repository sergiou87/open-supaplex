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

#include <math.h>

#if HAVE_SDL2
#include <SDL.h>
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
#include "../system.h"

#define kMaxSoundFilenameLength 256

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;

static const char *kRolandSoundFileNameSuffix = "roland";
static const char *kAdlibSoundFileNameSuffix = "adlib";
static const char *kBlasterSoundFileNameSuffix = "blaster";
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

#if defined(FILE_DATA_PATH)
#define FILE_PATH_STR_HELPER(x) #x
#define FILE_PATH_STR(x) FILE_PATH_STR_HELPER(x)
#define FILE_BASE_PATH FILE_PATH_STR(FILE_DATA_PATH)
static const char *kBaseAudioFolder = FILE_BASE_PATH "/audio";
#elif defined(__vita__)
static const char *kBaseAudioFolder = "app0:/audio";
#elif defined(_3DS)
static const char *kBaseAudioFolder = "romfs:/audio";
#elif defined(__PSL1GHT__)
static const char *kBaseAudioFolder = "/dev_hdd0/game/" PS3APPID "/USRDIR/audio";
#elif defined(__riscos__)
static const char *kBaseAudioFolder = "/<OpenSupaplex$Dir>/audio";
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
#if defined(__SWITCH__) || defined(__WIIU__) || defined(__WII__) || defined(_3DS)
const int kAudioBufferSize = 1024;
#else // macOS, PS Vita, PS3, PSP and Windows
// PS3 seems to ignore this and always use number_of_samples (256) * sizeof(float) * number_of_channels = 2048
// PSP and PS Vita only need this value to be a multiple of 64
const int kAudioBufferSize = 512;
#endif

// 3DS can't handle High quality audio, it kills performance
#if defined(_3DS)
static const int kSampleRate = 22050;
static const int kNumberOfChannels = 1;
#else
static const int kSampleRate = 44100;
static const int kNumberOfChannels = 2;
#endif

int8_t initializeAudio()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    int sampleRate = kSampleRate;
    int numberOfChannels = kNumberOfChannels;
    int audioBufferSize = kAudioBufferSize;

    // The Old Nintendo 3DS requires very low quality audio to perform well
    if (isOld3DSSystem())
    {
        sampleRate = 11025;
        numberOfChannels = 1;
        audioBufferSize = 512;
    }

    spLogInfo("Trying to initialize audio: %dHz, %d channels, format 0x%04x, %d bytes buffer",
              sampleRate, numberOfChannels, MIX_DEFAULT_FORMAT, audioBufferSize);

    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, numberOfChannels, audioBufferSize) == -1)
    {
        spLogInfo("Mix_OpenAudio: Failed to open audio!\n");
        spLogInfo("Mix_OpenAudio: %s\n", Mix_GetError());
        return 1;
    }

    int flags = 0;

    // Wii and Wii U use ModPlug instead of MikMod, so MIX_INIT_MOD is not required
#if !defined(__WII__) && !defined(__WIIU__)
     flags |= MIX_INIT_MOD;
#endif

    int initted = Mix_Init(flags);
    if((initted & flags) != flags)
    {
        spLogInfo("Mix_Init: Failed to init required mod support!\n");
        spLogInfo("Mix_Init: %s\n", Mix_GetError());
        return 1;
    }

    int queriedSampleRate;
    Uint16 queriedFormat;
    int queriedChannels;
    Mix_QuerySpec(&queriedSampleRate, &queriedFormat, &queriedChannels);
    
    spLogInfo("Audio initialized: %dHz, %d channels, format 0x%04x, %d bytes buffer",
              queriedSampleRate, queriedChannels, queriedFormat, kAudioBufferSize);
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
            musicSuffix = kAdlibSoundFileNameSuffix;
            break;
        case SoundTypeInternalSamples:
        case SoundTypeInternalStandard:
            musicSuffix = kStandardSoundFileNameSuffix;
            break;
        case SoundTypeNone:
            return;
    }

    char filename[kMaxSoundFilenameLength] = "";
    snprintf(filename, kMaxSoundFilenameLength, "%s/music-%s.xm", kBaseAudioFolder, musicSuffix);

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
            effectsSuffix = kRolandSoundFileNameSuffix;
            break;
        case SoundTypeSoundBlaster:
        case SoundTypeInternalSamples:
            effectsSuffix = kBlasterSoundFileNameSuffix;
            break;
        case SoundTypeAdlib:
            effectsSuffix = kAdlibSoundFileNameSuffix;
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
        snprintf(filename, kMaxSoundFilenameLength, "%s/%s-%s.wav", kBaseAudioFolder, gSoundEffectNames[i], effectsSuffix);
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

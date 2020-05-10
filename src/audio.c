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
Mix_Chunk *gExplosionSound = NULL;
Mix_Chunk *gInfotronSound = NULL;
Mix_Chunk *gPushSound = NULL;
Mix_Chunk *gFallSound = NULL;
Mix_Chunk *gBugSound = NULL;
Mix_Chunk *gBaseSound = NULL;
Mix_Chunk *gExitSound = NULL;

void loadMusic(void);
void destroyMusic(void);

void loadSounds(void);
void destroySounds(void);

int8_t initializeAudio()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
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
    snprintf(filename, kMaxSoundFilenameLength, "audio/music-%s.ogg", musicSuffix);

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

#define LOAD_CHUNK(__chunk, __name) \
    do { \
        snprintf(filename, kMaxSoundFilenameLength, "audio/" #__name "-%s.ogg", effectsSuffix); \
        __chunk = Mix_LoadWAV(filename); \
    } while(0)

    LOAD_CHUNK(gExplosionSound, explosion);
    LOAD_CHUNK(gInfotronSound, infotron);
    LOAD_CHUNK(gPushSound, push);
    LOAD_CHUNK(gFallSound, fall);
    LOAD_CHUNK(gBugSound, bug);
    LOAD_CHUNK(gBaseSound, base);
    LOAD_CHUNK(gExitSound, exit);

#undef LOAD_CHUNK
}

void destroySounds()
{
#define SAFE_FREE_CHUNK(__chunk) \
    do { \
        if (__chunk != NULL) \
        { \
            Mix_FreeChunk(__chunk); \
            __chunk = NULL; \
        } \
    } while(0)

    SAFE_FREE_CHUNK(gExplosionSound);
    SAFE_FREE_CHUNK(gInfotronSound);
    SAFE_FREE_CHUNK(gPushSound);
    SAFE_FREE_CHUNK(gFallSound);
    SAFE_FREE_CHUNK(gBugSound);
    SAFE_FREE_CHUNK(gBaseSound);
    SAFE_FREE_CHUNK(gExitSound);

#undef SAFE_FREE_CHUNK
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

#define DEFINE_PLAY_SOUND_FUNCTION(__chunk) \
void play##__chunk##Sound(void) \
{ \
    Mix_PlayChannel(-1, g##__chunk##Sound, 0); \
}

DEFINE_PLAY_SOUND_FUNCTION(Explosion);
DEFINE_PLAY_SOUND_FUNCTION(Infotron);
DEFINE_PLAY_SOUND_FUNCTION(Push);
DEFINE_PLAY_SOUND_FUNCTION(Fall);
DEFINE_PLAY_SOUND_FUNCTION(Bug);
DEFINE_PLAY_SOUND_FUNCTION(Base);
DEFINE_PLAY_SOUND_FUNCTION(Exit);

#undef DEFINE_PLAY_SOUND_FUNCTION

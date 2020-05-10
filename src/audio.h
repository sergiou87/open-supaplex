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

#ifndef audio_h
#define audio_h

#include <stdint.h>

typedef enum
{
    SoundTypeNone = 0,
    SoundTypeInternalStandard = 1,
    SoundTypeInternalSamples = 2,
    SoundTypeAdlib = 3,
    SoundTypeSoundBlaster = 4,
    SoundTypeRoland = 5,
} SoundType;

typedef enum
{
    SoundEffectExplosion,
    SoundEffectInfotron,
    SoundEffectPush,
    SoundEffectFall,
    SoundEffectBug,
    SoundEffectBase,
    SoundEffectExit,
    SoundEffectCount,
} SoundEffect;

extern SoundType sndType;
extern SoundType musType;

/// @return 0 on success, anything else on error
int8_t initializeAudio(void);
void destroyAudio(void);

void setSoundType(SoundType musicType, SoundType effectsType);

#define kMaxVolume 10

uint8_t getMusicVolume(void);
void setMusicVolume(uint8_t volume);

uint8_t getSoundEffectsVolume(void);
void setSoundEffectsVolume(uint8_t volume);

void playMusic(void);
void stopMusic(void);

void playSoundEffect(SoundEffect soundEffect);

#endif /* audio_h */

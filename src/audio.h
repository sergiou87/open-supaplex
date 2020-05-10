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

extern SoundType sndType;
extern SoundType musType;

/// @return 0 on success, anything else on error
int8_t initializeAudio(void);
void destroyAudio(void);

void setSoundType(SoundType musicType, SoundType effectsType);

void playMusic(void);
void stopMusic(void);

#define DECLARE_PLAY_SOUND_FUNCTION(__sound) \
void play##__sound##Sound(void);

DECLARE_PLAY_SOUND_FUNCTION(Explosion);
DECLARE_PLAY_SOUND_FUNCTION(Infotron);
DECLARE_PLAY_SOUND_FUNCTION(Push);
DECLARE_PLAY_SOUND_FUNCTION(Fall);
DECLARE_PLAY_SOUND_FUNCTION(Bug);
DECLARE_PLAY_SOUND_FUNCTION(Base);
DECLARE_PLAY_SOUND_FUNCTION(Exit);

#undef DECLARE_PLAY_SOUND_FUNCTION

#endif /* audio_h */

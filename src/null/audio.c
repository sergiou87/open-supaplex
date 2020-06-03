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

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;

int8_t initializeAudio()
{
    spLogInfo("Initialized null driver correctly");
    return 0;
}

void destroyAudio()
{
	// Do nothing
}

void setSoundType(SoundType musicType, SoundType effectsType)
{
    sndType = effectsType;
    musType = musicType;
}

uint8_t getMusicVolume(void)
{
	return 0;
}

void setMusicVolume(uint8_t volume)
{
	// Do nothing
}

uint8_t getSoundEffectsVolume(void)
{
	return 0;
}

void setSoundEffectsVolume(uint8_t volume)
{
	// Do nothing
}

void playMusic()
{
	// Do nothing
}

void stopMusic()
{
	// Do nothing
}

void playSoundEffect(SoundEffect soundEffect)
{
	// Do nothing
}

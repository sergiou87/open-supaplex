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

#ifndef platform_h
#define platform_h

#include <stdint.h>

#if defined(__vita__) || defined(__SWITCH__) || defined(__PSP__) || defined(__NDS__) || defined(_3DS) || defined(__PSL1GHT__) || defined(__WII__) || defined(__WIIU__) || defined(__PS2__)
#define PLATFORM_SUPPORTS_REAL_KEYBOARD 0
#define PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK 0
#else
#define PLATFORM_SUPPORTS_REAL_KEYBOARD 1
#define PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK 1
#endif

#if defined(__vita__) || defined(_3DS) || defined(__PSL1GHT__)
#define PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD 1
#else
#define PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD 0
#endif

#if defined(__SWITCH__)
#define PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD 1
#else
#define PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD 0
#endif

uint8_t supportsRealKeyboard(void);
uint8_t supportsVirtualKeyboard(void);

// This is for platforms that allow in some way to select a 00S000$0.SP demo file for playback. For example, on
// PC the user can use a command line option to play a specific demo. But right now not on consoles using the
// advanced menu.
// TODO: add some kind of demo file selector to advanced menu.
//
uint8_t supportsSPFileDemoPlayback(void);

const char *platformReadonlyBasePath(void);
const char *platformWritableBasePath(void);
void platformCreateWritableBaseFolder(void);

void platformInitializeLogging(void);
void platformDestroyLogging(void);
void platformSPLog(const char *message);

void platformInitializeSystem(void);
void platformDestroySystem(void);

const char *platformAudioBasePath(void);
void platformGetAudioSettings(int *sampleRate, int *numberOfChannels, int *audioBufferSize);
uint8_t platformNeedsMixerInitMod(void);

int platformSDLWindowWidth(void);
int platformSDLWindowHeight(void);
uint32_t platformSDLWindowFlags(void);
void platformSdl1WaitForPresentationSync(uint8_t scalingMode);
uint8_t platformSdl1HorizontalHatMin(void);
uint8_t platformSdl1HorizontalHatMax(void);
uint8_t platformSdl1VerticalHatMin(void);
uint8_t platformSdl1VerticalHatMax(void);
uint8_t platformSdl1HasWiiConfirmCancelAliases(void);
uint8_t platformSdl1ConfirmButtonIsB(void);

uint8_t platformSdl2ConfirmButtonIsB(void);
uint32_t platformSdl2TextureFormat(void);

#endif /* platform_h */
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

#include "platform.h"

#include "logging.h"

#if HAVE_SDL2
#include <SDL.h>
#elif HAVE_SDL
#include <SDL/SDL.h>
#endif

#ifndef SDL_HAT_UP
#define SDL_HAT_UP 0x01
#endif

#ifndef SDL_HAT_RIGHT
#define SDL_HAT_RIGHT 0x02
#endif

#ifndef SDL_HAT_DOWN
#define SDL_HAT_DOWN 0x04
#endif

#ifndef SDL_HAT_LEFT
#define SDL_HAT_LEFT 0x08
#endif

#include "video.h"

#if defined(FILE_DATA_PATH)
#define PLATFORM_PATH_STR_HELPER(x) #x
#define PLATFORM_PATH_STR(x) PLATFORM_PATH_STR_HELPER(x)
#endif

#if (defined(__vita__) || defined(__PSL1GHT__)) && DEBUG
#define USE_DEBUGNET 1
#endif

#if USE_DEBUGNET
#include <debugnet.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#if defined(__PSP__)
#include <pspdisplay.h>
#endif

#if defined(__NDS__)
#include <filesystem.h>
#endif

#if defined(_3DS)
#include <3ds.h>
#endif

#if defined(__vita__)
#include <psp2/io/stat.h>
#endif

#if defined(_3DS) || defined(__riscos__)
#include <sys/stat.h>
#endif

uint8_t supportsRealKeyboard(void)
{
    return PLATFORM_SUPPORTS_REAL_KEYBOARD;
}

uint8_t supportsVirtualKeyboard(void)
{
#if PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD
    return 1;
#elif PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD
    // If app is ran in applet mode, SwKbd will not work
    AppletType at = appletGetAppletType();
    return at == AppletType_Application || at == AppletType_SystemApplication;
#else
    return 0;
#endif
}

uint8_t supportsSPFileDemoPlayback(void)
{
    return PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK;
}

const char *platformReadonlyBasePath(void)
{
#if defined(_3DS)
    return "romfs:/";
#elif defined(__NDS__)
    return "nitro:/resources/";
#elif defined(__PSL1GHT__)
    return "/dev_hdd0/game/" PS3APPID "/USRDIR/";
#elif defined(__riscos__)
    return "/<OpenSupaplex$Dir>/data/";
#elif defined(__vita__)
    return "app0:/";
#elif defined(__WII__)
    return "/apps/OpenSupaplex/";
#elif defined(__WIIU__)
    return "fs:/vol/external01/wiiu/apps/OpenSupaplex/";
#else
    return "";
#endif
}

const char *platformWritableBasePath(void)
{
#if defined(_3DS)
    return "sdmc:/OpenSupaplex/";
#elif defined(__NDS__)
    return "";
#elif defined(__PSL1GHT__)
    return "/dev_hdd0/game/" PS3APPID "/USRDIR/";
#elif defined(__riscos__)
    return "/<OpenSupaplex$Saves>/";
#elif defined(__vita__)
    return "ux0:/data/OpenSupaplex/";
#elif defined(__WII__)
    return "/apps/OpenSupaplex/";
#elif defined(__WIIU__)
    return "fs:/vol/external01/wiiu/apps/OpenSupaplex/";
#else
    return "";
#endif
}

void platformCreateWritableBaseFolder(void)
{
    const char *basePath = platformWritableBasePath();

    if (basePath[0] == '\0')
    {
        return;
    }

#if defined(__vita__)
    sceIoMkdir(basePath, 0777);
#elif defined(_3DS) || defined(__riscos__)
    mkdir(basePath, 0777);
#endif
}

void platformInitializeLogging(void)
{
#if USE_DEBUGNET
    debugNetInit(DEBUGNETIP, 18194, DEBUG);
#endif

#if defined(__SWITCH__) && DEBUG
    socketInitializeDefault();
    nxlinkStdio();
#endif
}

void platformDestroyLogging(void)
{
#if defined(__SWITCH__) && DEBUG
    socketExit();
#endif
}

void platformSPLog(const char *message)
{
#if USE_DEBUGNET
    debugNetPrintf(INFO, "%s\n", message);
#endif

#if HAVE_SDL2
    SDL_Log("%s", message);
#else
    printf("%s\n", message);
#endif
}

#if defined(_3DS)
static uint8_t isOld3DSSystem(void)
{
    _Bool isN3DS;
    APT_CheckNew3DS(&isN3DS);
    if (isN3DS == 0)
    {
        return 1;
    }
    
    return 0;
}
#endif

void platformInitializeSystem(void)
{
#if defined(__NDS__)
    nitroFSInit(NULL);
#endif

#if defined(_3DS)
    if (isOld3DSSystem() == 0)
    {
        osSetSpeedupEnable(true);
        spLogInfo("Using New3DS speed up for better performance");
    }

    romfsInit();
#endif
}

void platformDestroySystem(void)
{
#if defined(_3DS)
    romfsExit();
#endif
}

const char *platformAudioBasePath(void)
{
#if defined(FILE_DATA_PATH)
    return PLATFORM_PATH_STR(FILE_DATA_PATH) "/audio";
#elif defined(__vita__)
    return "app0:/audio";
#elif defined(_3DS)
    return "romfs:/audio";
#elif defined(__PSL1GHT__)
    return "/dev_hdd0/game/" PS3APPID "/USRDIR/audio";
#elif defined(__riscos__)
    return "/<OpenSupaplex$Dir>/audio";
#elif defined(__WII__)
    return "/apps/OpenSupaplex/audio";
#elif defined(__WIIU__)
    return "fs:/vol/external01/wiiu/apps/OpenSupaplex/audio";
#else
    return "audio";
#endif
}

void platformGetAudioSettings(int *sampleRate, int *numberOfChannels, int *audioBufferSize)
{
    int defaultSampleRate;
    int defaultChannelCount;
    int defaultBufferSize;

    // Keep buffer size as low as possible to prevent latency with sound effects.
    // In macOS I was able to set it to 512, but that caused a lot of issues playing music on Nintendo Switch.
    // If it ever supports WASM, that needs a power of 2 as buffer size.
    //
    // PS3 seems to ignore this and always use number_of_samples (256) * sizeof(float) * number_of_channels = 2048
    // PSP and PS Vita only need this value to be a multiple of 64

#if defined(__SWITCH__) || defined(__WIIU__) || defined(__WII__) || defined(_3DS)
    defaultBufferSize = 1024;
#else
    defaultBufferSize = 512;
#endif

#if defined(_3DS)
    defaultSampleRate = 22050;
    defaultChannelCount = 1;

    // The Old Nintendo 3DS requires very low quality audio to perform well.
    if (isOld3DSSystem())
    {
        defaultSampleRate = 11025;
        defaultChannelCount = 1;
    }

#else
    defaultSampleRate = 44100;
    defaultChannelCount = 2;
#endif

    *sampleRate = defaultSampleRate;
    *numberOfChannels = defaultChannelCount;
    *audioBufferSize = defaultBufferSize;
}

uint8_t platformNeedsMixerInitMod(void)
{
#if defined(__WII__) || defined(__WIIU__)
    return 0;
#else
    return 1;
#endif
}

int platformSDLWindowWidth(void)
{
#if HAVE_SDL2
#if defined(__PSP__)
    return 480;
#elif defined(__vita__)
    return 960;
#elif defined(__PSL1GHT__) || defined(__WIIU__)
    return 1280;
#else
    return kScreenWidth * 4;
#endif
#elif HAVE_SDL
#if defined(__PSP__)
    return 480;
#elif defined(_3DS)
    return 400;
#elif defined(__NDS__)
    return kScreenWidth;
#elif defined(__WII__)
    return 640;
#elif defined(__riscos__)
    return kScreenWidth * 2;
#else
    return kScreenWidth * 4;
#endif
#else
    return 0;
#endif
}

int platformSDLWindowHeight(void)
{
#if HAVE_SDL2
#if defined(__PSP__)
    return 272;
#elif defined(__vita__)
    return 544;
#elif defined(__PSL1GHT__) || defined(__WIIU__)
    return 720;
#else
    return kScreenHeight * 4;
#endif
#elif HAVE_SDL
#if defined(__PSP__)
    return 272;
#elif defined(_3DS)
    return 240;
#elif defined(__NDS__)
    return kScreenHeight;
#elif defined(__WII__)
    return 480;
#elif defined(__riscos__)
    return kScreenHeight * 2;
#else
    return kScreenHeight * 4;
#endif
#else
    return 0;
#endif
}

uint32_t platformSDLWindowFlags(void)
{
#if HAVE_SDL2
#if defined(__PSP__) || defined(__vita__) || defined(__PSL1GHT__) || defined(__WIIU__) || defined(__SWITCH__) || defined(__PS2__)
    return SDL_WINDOW_FULLSCREEN;
#else
    return 0;
#endif
#elif HAVE_SDL
#if defined(__PSP__)
    return SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#elif defined(_3DS)
#if DEBUG
    return SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_CONSOLEBOTTOM;
#else
    return SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#endif
#elif defined(__NDS__)
    return SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_BOTTOMSCR;
#elif defined(__WII__)
    return SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#else
    return SDL_RESIZABLE | SDL_SWSURFACE | SDL_HWPALETTE;
#endif
#else
    return 0;
#endif
}

void platformSdl1WaitForPresentationSync(uint8_t scalingMode)
{
#if defined(__PSP__)
    if (scalingMode == ScalingModeIntegerFactor)
    {
        sceDisplayWaitVblankStart();
    }
#endif
}

uint8_t platformSdl1HorizontalHatMin(void)
{
#if defined(__WII__)
    return SDL_HAT_UP;
#else
    return SDL_HAT_LEFT;
#endif
}

uint8_t platformSdl1HorizontalHatMax(void)
{
#if defined(__WII__)
    return SDL_HAT_DOWN;
#else
    return SDL_HAT_RIGHT;
#endif
}

uint8_t platformSdl1VerticalHatMin(void)
{
#if defined(__WII__)
    return SDL_HAT_RIGHT;
#else
    return SDL_HAT_UP;
#endif
}

uint8_t platformSdl1VerticalHatMax(void)
{
#if defined(__WII__)
    return SDL_HAT_LEFT;
#else
    return SDL_HAT_DOWN;
#endif
}

uint8_t platformSdl1HasWiiConfirmCancelAliases(void)
{
#if defined(__WII__)
    return 1;
#else
    return 0;
#endif
}

uint8_t platformSdl1ConfirmButtonIsB(void)
{
#if defined(_3DS)
    return 1;
#else
    return 0;
#endif
}

uint8_t platformSdl2ConfirmButtonIsB(void)
{
#if defined(__SWITCH__) || defined(__WIIU__)
    return 1;
#else
    return 0;
#endif
}

uint32_t platformSdl2TextureFormat(void)
{
#if HAVE_SDL2
#if defined(__PSP__)
    return SDL_PIXELFORMAT_ABGR32;
#else
    return SDL_PIXELFORMAT_RGB24;
#endif
#else
    return 0;
#endif
}
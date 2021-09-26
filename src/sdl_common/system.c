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

#include "../system.h"

#include <errno.h>
#if HAVE_SDL2
#include <SDL2/SDL.h>
#elif HAVE_SDL
#include <SDL/SDL.h>
#endif
#include <stdarg.h>
#include <stdio.h>

#ifdef __NDS__
#include <stdbool.h>
#include <filesystem.h>
#endif

#if defined(_3DS)
#include <3ds.h>
#endif

#include "../globals.h"
#include "../logging.h"

void initializeSystem(void)
{
    int ret = SDL_Init(SDL_INIT_TIMER);
    if (ret)
    {
        spLogInfo("SDL_Init failed with %d", ret);
        exit(1);
    }

    spLogInfo("SDL_Init succeeded.");

#ifdef __NDS__
    nitroFSInit(NULL);
#endif

#if defined(_3DS)
	if(isOld3DSSystem() == 0)
    {
		osSetSpeedupEnable(true);
        spLogInfo("Using New3DS speed up for better performance");
    }
    romfsInit();
#endif
}

void destroySystem(void)
{
    SDL_Quit();
#if defined(_3DS)
    romfsExit();
#endif
}

uint8_t isOld3DSSystem(void)
{
#if defined(_3DS)
    _Bool isN3DS;
	APT_CheckNew3DS(&isN3DS);
	if (isN3DS == 0)
    {
        return 1;
    }
#endif

    return 0;
}

void exitWithError(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    SDL_Quit();
#if defined(_3DS)
    romfsExit();
#endif
    exit(errno);
}

void int8handler() // proc far        ; DATA XREF: setint8+10o
{
    if (gIsGameRunning != 0)
    {
        gAuxGameSeconds20msAccumulator++;
        if (gAuxGameSeconds20msAccumulator >= 50)
        {
            gAuxGameSeconds20msAccumulator = 0;
            gGameSeconds++;
            if (gGameSeconds >= 60)
            {
                gGameSeconds = 0;
                gGameMinutes++;
                if (gGameMinutes >= 60)
                {
                    gGameMinutes = 0;
                    gGameHours++;
                }
            }
        }
    }

//loc_473B4:              ; CODE XREF: int8handler+4Fj
    // 01ED:0751
    if (gCurrentSoundDuration != 0)
    {
        gCurrentSoundDuration--;
        if (gCurrentSoundDuration == 0)
        {
            gCurrentSoundPriority = 0;
        }
    }
}

void emulateClock()
{
    static Uint32 sLastTickCount = 0;
    static Uint32 sRemainingTicks = 0;
    static const Uint32 sClockInterval = 20; // 20 ms is what the original game seemed to use

    if (sLastTickCount == 0)
    {
        sLastTickCount = getTime();
        return;
    }

    sRemainingTicks += getTime() - sLastTickCount;

    while (sRemainingTicks > sClockInterval)
    {
        int8handler();
        sRemainingTicks -= sClockInterval;
    }

    sLastTickCount = getTime();
}

void handleSystemEvents()
{
    if (gFastMode != FastModeTypeUltra)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gShouldExitLevel = 1;
                gShouldExitGame = 1;
            }
        }
    }

    emulateClock();
}

uint32_t getTime(void)
{
    return SDL_GetTicks();
}

void waitTime(uint32_t time)
{
    SDL_Delay(time);
}

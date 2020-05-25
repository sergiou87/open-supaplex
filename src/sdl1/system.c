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
#include <SDL/SDL.h>
#include <stdio.h>

#include "../globals.h"

void exitWithError(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    SDL_Quit();
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

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

#include "logging.h"

#include <SDL2/SDL.h>

#if defined(__vita__) && DEBUG
#include <debugnet.h>
#endif

void initializeLogging(void)
{
 #if defined(__vita__) && DEBUG
     int ret;
     ret = debugNetInit(DEBUGNETIP, 18194, DEBUG);
 #endif

    spLog("Logging system initialized.");
}

void spLog(const char *format, ...)
{
    char buffer[0x800];

    va_list argptr;
    va_start(argptr, format);
    vsprintf(buffer, format, argptr);
    va_end(argptr);
#if defined(__vita__) && DEBUG
    debugNetPrintf(INFO, "%s\n", buffer);
#endif

    SDL_Log("%s", buffer);
}

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

#include "../logging.h"

#include <SDL/SDL.h>

LogLevel gLogLevel = LogLevelInfo;

void setLogLevel(LogLevel logLevel)
{
    gLogLevel = logLevel;
}

void initializeLogging(void)
{
    spLogInfo("Logging system initialized.");
}

void destroyLogging(void)
{
    spLogInfo("Destroying logging system...");
    spLogInfo("Logging system destroyed");
}

void spLog(LogLevel level, const char *format, ...)
{
    char buffer[0x800];

    if (gLogLevel > level)
    {
        return;
    }

    va_list argptr;
    va_start(argptr, format);
    vsprintf(buffer, format, argptr);
    va_end(argptr);

    printf("%s\n", buffer);
}

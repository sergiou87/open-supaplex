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

#include <stdarg.h>
#include <stdio.h>

#ifndef logging_h
#define logging_h

typedef enum {
    LogLevelInfo,
    LogLevelDemo,
} LogLevel;

void initializeLogging(void);
void destroyLogging(void);
void setLogLevel(LogLevel logLevel);

void spLog(LogLevel level, const char *format, ...);

#define spLogInfo(...) spLog(LogLevelInfo, __VA_ARGS__)
#define spLogDemo(...) spLog(LogLevelDemo, __VA_ARGS__)

#endif /* logging_h */

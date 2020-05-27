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

#include "file.h"

#include <string.h>

#ifdef __vita__
#include <psp2/io/stat.h>

#define VITA_WRITABLE_PATH "ux0:/data/OpenSupaplex/"

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    // app0:/ references the folder where the app was installed
    snprintf(outPath, kMaxFilePathLength, "app0:/%s", pathname);
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    snprintf(outPath, kMaxFilePathLength, VITA_WRITABLE_PATH "%s", pathname);
}

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    char finalPathname[kMaxFilePathLength];
    getReadonlyFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    // Create base folder in a writable area
    sceIoMkdir(VITA_WRITABLE_PATH, 0777);

    // For writable files we'll use a subfolder in ux0:/data/
    char finalPathname[kMaxFilePathLength];
    getWritableFilePath(pathname, finalPathname);

    return fopen(finalPathname, mode);
}
#elif defined(_3DS)
void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    snprintf(outPath, kMaxFilePathLength, "sdmc:/OpenSupaplex/%s", pathname);
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    getReadonlyFilePath(pathname, outPath);
}

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    char finalPathname[kMaxFilePathLength];
    getReadonlyFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    return openReadonlyFile(pathname, mode);
}
#else
void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    getWritableFilePath(pathname, outPath);
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    strncpy(outPath, pathname, kMaxFilePathLength);
}

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    return fopen(pathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    return fopen(pathname, mode);
}
#endif

FILE *openWritableFileWithReadonlyFallback(const char *pathname, const char *mode)
{
    FILE *file = openWritableFile(pathname, mode);

    if (file == NULL)
    {
        return openReadonlyFile(pathname, mode);
    }

    return file;
}

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

#include "utils.h"

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
#else // the rest of the platforms just have a different base path

#if defined(_3DS)
#define FILE_BASE_PATH "sdmc:/OpenSupaplex/"
#elif defined(__PSL1GHT__)
#define FILE_BASE_PATH "/dev_hdd0/game/" PS3APPID "/USRDIR/"
#elif defined(__WII__)
#define FILE_BASE_PATH "/apps/OpenSupaplex/"
#elif defined(__WIIU__)
#define FILE_BASE_PATH "fs:/vol/external01/wiiu/apps/OpenSupaplex/"
#else
#define FILE_BASE_PATH ""
#endif

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    snprintf(outPath, kMaxFilePathLength, FILE_BASE_PATH "%s", pathname);
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

size_t fileReadUInt16(uint16_t *value, FILE *file)
{
    size_t bytes = fread(value, 1, sizeof(uint16_t), file);

    if (bytes == sizeof(uint16_t))
    {
        *value = convert16LE(*value);
    }

    return bytes;
}

size_t fileReadUInt8(uint8_t *value, FILE *file)
{
    return fread(value, 1, sizeof(uint8_t), file);
}

size_t fileReadBytes(void *buffer, size_t count, FILE *file)
{
    return fread(buffer, 1, count, file);
}

size_t fileWriteUInt16(uint16_t value, FILE *file)
{
    uint16_t finalValue = convert16LE(value);
    return fwrite(&finalValue, 1, sizeof(uint16_t), file);
}

size_t fileWriteUInt8(uint8_t value, FILE *file)
{
    return fwrite(&value, 1, sizeof(uint8_t), file);
}

size_t fileWriteBytes(void *buffer, size_t count, FILE *file)
{
    return fwrite(buffer, 1, count, file);
}

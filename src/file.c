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

#if defined(FILE_FHS_XDG_DIRS)

#if defined(FILE_DATA_PATH)
#define FILE_PATH_STR_HELPER(x) #x
#define FILE_PATH_STR(x) FILE_PATH_STR_HELPER(x)
#define FILE_BASE_PATH FILE_PATH_STR(FILE_DATA_PATH)
#else
#define FILE_BASE_PATH ""
#endif

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    if (pathname[0] == '/')
    {
        strncpy(outPath, pathname, kMaxFilePathLength);
        return;
    }
    
    // This can be used to ignore /usr/share and just use local dir
    // Unfortunately, with current Makefile there's no way to build test
    // separately from the main executable, so this allows to run tests during
    // emerge on Gentoo before installing the package.
    char *path = getenv("OPENSUPAPLEX_PATH");
    
    if (path && *path) 
    {
        snprintf(outPath, kMaxFilePathLength, "%s/%s", path, pathname);
        return;
    }
    
    snprintf(outPath, kMaxFilePathLength, "%s/%s", FILE_BASE_PATH, pathname);
}

#include <errno.h>
#include <sys/stat.h>

// https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
static int mkdir_p(const char *path)
{
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[kMaxFilePathLength];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path) - 1)
    {
        errno = ENAMETOOLONG;
        return -1;
    }
    
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) 
    {
        if (*p == '/') 
        {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) 
            {
                if (errno != EEXIST)
                {
                    return -1;
                }
            }

            *p = '/';
        }
    }

    if (mkdir(_path, S_IRWXU) != 0) 
    {
        if (errno != EEXIST)
        {
            return -1;
        }
    }

    return 0;
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    if (pathname[0] == '/') 
    {
        strncpy(outPath, pathname, kMaxFilePathLength);
        return;
    }
    
    char *path = getenv("OPENSUPAPLEX_PATH");
    
    if (path && *path) 
    {
        getReadonlyFilePath(pathname, outPath);
        return;
    }
    
    char *xdg = getenv("XDG_DATA_HOME");
    char *home = getenv("HOME");
    char dirPath[kMaxFilePathLength] = {};
    
    if (xdg && *xdg) 
    {
        snprintf(dirPath, kMaxFilePathLength, "%s/OpenSupaplex", xdg);
    } 
    else if (home && *home) 
    {
        snprintf(dirPath, kMaxFilePathLength, "%s/.local/share/OpenSupaplex", home);
    }
    
    mkdir_p(dirPath);
    snprintf(outPath, kMaxFilePathLength, "%s/%s", dirPath, pathname);
}

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    char finalPathname[kMaxFilePathLength];
    getReadonlyFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    char finalPathname[kMaxFilePathLength];
    getWritableFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
}

#else // the rest of the platforms just have different base paths

#if defined(_3DS)
#include <sys/stat.h>
#define FILE_BASE_PATH "romfs:/"
#define FILE_BASE_WRITABLE_PATH "sdmc:/3ds/OpenSupaplex/"
#elif defined(__NDS__)
#define FILE_BASE_PATH "nitro:/resources/"
#define FILE_BASE_WRITABLE_PATH ""
#elif defined(__PSL1GHT__)
#define FILE_BASE_PATH "/dev_hdd0/game/" PS3APPID "/USRDIR/"
#elif defined(__riscos__)
#include <sys/stat.h>
#define FILE_BASE_PATH "/<OpenSupaplex$Dir>/data/"
#define FILE_BASE_WRITABLE_PATH "/<OpenSupaplex$Saves>/"
#elif defined(__vita__)
#include <psp2/io/stat.h>
#define FILE_BASE_PATH "app0:/"
#define FILE_BASE_WRITABLE_PATH "ux0:/data/OpenSupaplex/"
#elif defined(__WII__)
#define FILE_BASE_PATH "/apps/OpenSupaplex/"
#elif defined(__WIIU__)
#define FILE_BASE_PATH "fs:/vol/external01/wiiu/apps/OpenSupaplex/"
#else
#define FILE_BASE_PATH ""
#endif

#ifndef FILE_BASE_WRITABLE_PATH
#define FILE_BASE_WRITABLE_PATH FILE_BASE_PATH
#endif

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    snprintf(outPath, kMaxFilePathLength, FILE_BASE_PATH "%s", pathname);
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    snprintf(outPath, kMaxFilePathLength, FILE_BASE_WRITABLE_PATH "%s", pathname);
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
#ifdef __vita__
    sceIoMkdir(FILE_BASE_WRITABLE_PATH, 0777);
#elif defined(__riscos__) || defined(_3DS)
    mkdir(FILE_BASE_WRITABLE_PATH, 0777);
#endif

    char finalPathname[kMaxFilePathLength];
    getWritableFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
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

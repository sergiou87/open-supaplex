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

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    // app0:/ references the folder where the app was installed
    char finalPathname[256] = "app0:/";
    strcat(finalPathname, pathname);
    return fopen(finalPathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    // For writable files we'll use a subfolder in ux0:/data/
    char finalPathname[256] = "ux0:/data/OpenSupaplex/";

    // Create base folder in a writable area
    sceIoMkdir(finalPathname, 0777);

    strcat(finalPathname, pathname);

    return fopen(finalPathname, mode);
}
#else
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

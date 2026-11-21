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

@import Foundation;

#include <string.h>

#include "utils.h"

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    strncpy(outPath, pathname, kMaxFilePathLength);

    NSString *pathNameString = [NSString stringWithUTF8String:pathname];

    if ([pathNameString isAbsolutePath])
    {
        return;
    }

    NSURL *fileURL = [NSBundle.mainBundle
                      URLForResource:pathNameString.stringByDeletingPathExtension
                      withExtension:pathNameString.pathExtension];

    if (fileURL == nil)
    {
        return;
    }

    strncpy(outPath, [fileURL.path cStringUsingEncoding:NSUTF8StringEncoding], kMaxFilePathLength);
}

NSString *writableFolderPath()
{
    NSArray *theDirs = [NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
    NSURL *folderURL = theDirs.firstObject;
    return [folderURL.path stringByAppendingPathComponent:@"OpenSupaplex"];
}

void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength])
{
    strncpy(outPath, pathname, kMaxFilePathLength);

    NSString *pathNameString = [NSString stringWithUTF8String:pathname];

    if ([pathNameString isAbsolutePath])
    {
        return;
    }

    NSString *finalPath = [writableFolderPath() stringByAppendingPathComponent:pathNameString];

    if (finalPath == nil)
    {
        return;
    }

    strncpy(outPath, [finalPath cStringUsingEncoding:NSUTF8StringEncoding], kMaxFilePathLength);
}

FILE *openReadonlyFile(const char *pathname, const char *mode)
{
    // app0:/ references the folder where the app was installed
    char finalPathname[kMaxFilePathLength];
    getReadonlyFilePath(pathname, finalPathname);
    return fopen(finalPathname, mode);
}

FILE *openWritableFile(const char *pathname, const char *mode)
{
    // Create base folder in a writable area
    [NSFileManager.defaultManager
     createDirectoryAtPath:writableFolderPath()
     withIntermediateDirectories:YES
     attributes:nil
     error:NULL];

    // For writable files we'll use a subfolder in ~/Library/Application Support/
    char finalPathname[kMaxFilePathLength];
    getWritableFilePath(pathname, finalPathname);

    return fopen(finalPathname, mode);
}

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

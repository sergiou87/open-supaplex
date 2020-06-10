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

#include "demo.h"

#include <string.h>

uint16_t kOriginalDemoFileSizes[10] = { // word_599DC
    0x00CE, 0x016A, 0x0146, 0x00CD, 0x024D,
    0x012C, 0x01A7, 0x01FB, 0x01D2, 0x02FD
};

// These are literally the first 4 bytes of the original files, used by the spfix version to detect when a demo from
// the original game was being parsed (since those had a different format).
//
FirstOriginalDemoFileChunk kOriginalDemoFirstFileChunks[10] = { // word_599E4
    { 0x01, { 0xF0, 0xF0, 0xF1 } },
    { 0x03, { 0xF0, 0x50, 0xF3 } },
    { 0x07, { 0xF0, 0x60, 0xF4 } },
    { 0x0B, { 0xF0, 0xF0, 0xF0 } },
    { 0x1D, { 0xF0, 0xF0, 0xF0 } },
    { 0x26, { 0xF0, 0xF0, 0x50 } },
    { 0x37, { 0xF0, 0xD0, 0x41 } },
    { 0x5F, { 0x10, 0xF3, 0xF3 } },
    { 0x68, { 0xF0, 0xF0, 0x10 } },
    { 0x6C, { 0x10, 0xF4, 0x94 } },
};

uint16_t gDemoRandomSeeds[kNumberOfDemos];

Demos gDemos; // this is located in the demo segment, starting at 0x0000

char gCurrentDemoLevelName[kListLevelNameLength] = ".SP\0----- DEMO LEVEL! -----"; // 0x87DA

char gRecordingDemoMessage[kLevelNameLength] = "--- RECORDING DEMO0 ---";

uint8_t getLevelNumberFromOriginalDemoFile(FILE *file, uint16_t fileLength) // fileReadUnk1    proc near       ; CODE XREF: start+B6p readDemoFiles+81p
{
    // 01ED:048F

    // Return value:
    //  - carry flag = 0 on success, 1 on error
    //  - ax = 0 on error, level number of the original demo on success

    uint8_t isDemoFromOriginalGame = 0;
    uint8_t originalDemoIndex = 0;

    for (int i = 0; i < 10; ++i)
    {
//loop_:                  //; CODE XREF: getLevelNumberFromOriginalDemoFile+Fj
        if (kOriginalDemoFileSizes[i] == fileLength)
        {
            isDemoFromOriginalGame = 1;
            originalDemoIndex = i;
            break;
        }
    }

    if (isDemoFromOriginalGame == 0)
    {
        return 0;
    }

//loc_47105:              //; CODE XREF: getLevelNumberFromOriginalDemoFile+Aj
    int result = fseek(file, 0, SEEK_SET);

    if (result < 0)
    {
        return 0;
    }

    FirstOriginalDemoFileChunk firstChunk;
    size_t bytes = fileReadBytes(&firstChunk, sizeof(firstChunk), file);

    if (bytes < sizeof(firstChunk))
    {
        return 0;
    }

    FirstOriginalDemoFileChunk referenceChunk = kOriginalDemoFirstFileChunks[originalDemoIndex];

    // If the first chunk we just read doesn't match the reference chunk, we won't
    // consider it a demo from the original game.
    //
    if (memcmp(&referenceChunk, &firstChunk, sizeof(FirstOriginalDemoFileChunk)) != 0)
    {
        return 0;
    }

    return firstChunk.levelNumber;
}

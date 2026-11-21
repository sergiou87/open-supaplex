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

#ifndef demo_h
#define demo_h

#include <stdint.h>
#include <stdio.h>

#include "globals.h"

extern uint16_t kOriginalDemoFileSizes[10]; // word_599DC

typedef struct {
    uint8_t levelNumber;
    uint8_t firstUserInputs[3];
} FirstOriginalDemoFileChunk;

// These are literally the first 4 bytes of the original files, used by the spfix version to detect when a demo from
// the original game was being parsed (since those had a different format).
//
extern FirstOriginalDemoFileChunk kOriginalDemoFirstFileChunks[10]; // word_599E4

#define kMaxDemoInputSteps 48648
#define kMaxBaseDemoSize 1 + kMaxDemoInputSteps + 1

// This struct defines the demo format of the original game (previous to the speed fix mods)
typedef struct
{
    // In demos recorded with the original game (not speed fix versions), it was the level number. After the speed fix
    // versions, the level itself was included in the demo file, so they set the MSB to 1 to mark them and this byte
    // loses its original function.
    //
    uint8_t levelNumber;
    uint8_t inputSteps[kMaxDemoInputSteps + 1]; // of UserInput, finishes with 0xFF
} BaseDemo;

#define kMaxDemoSignatureLength 511
#define kMaxDemoSignatureSize kMaxDemoSignatureLength + 1

// This struct defines the demo format of the game after the speed fix mods (which includes a demo of the original
// format inside).
//
typedef struct
{
    Level level;
    BaseDemo baseDemo;
    uint8_t signature[kMaxDemoSignatureSize]; // text that ends with 0xFF
} DemoFile;

#define kNumberOfDemos 10

typedef struct
{
    uint16_t demoFirstIndices[kNumberOfDemos + 1]; // index of the last byte of all demos (starting at demo-segment:0000). there are 11 words because the end of this "list" is marked with 0xFFFF
    uint8_t demoData[1 + kMaxDemoInputSteps + 1]; // to fit at least one huge demo with 1 byte for level number, then all the possible steps, then 0xFF
    Level level[kNumberOfDemos];
} Demos;

extern uint16_t gDemoRandomSeeds[kNumberOfDemos];

extern Demos gDemos; // this is located in the demo segment, starting at 0x0000

// uint16_t word_58AEA = 0x3030; // -> 0x87DA
// uint16_t word_58AEC = 0x0030; // -> 0x87DC
extern char gCurrentDemoLevelName[kListLevelNameLength]; // 0x87DA

extern char gRecordingDemoMessage[kLevelNameLength];

uint8_t getLevelNumberFromOriginalDemoFile(FILE *file, uint16_t fileLength);

#endif /* demo_h */

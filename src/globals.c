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

#include "globals.h"

const uint8_t kGameVersion = 0x71;

uint16_t gLevelData[kLevelSize];
uint8_t gPaddedLevelListData[kNumberOfLevelsWithPadding * kListLevelNameLength]; // 0x2A34
uint8_t *gLevelListData = &gPaddedLevelListData[kFirstLevelIndex * kListLevelNameLength];

const int kNotCompletedLevelEntryColor = 2;
const int kCompletedLevelEntryColor = 4;
const int kBlockedLevelEntryColor = 6;
const int kSkippedLevelEntryColor = 8;

uint8_t gCurrentPlayerPaddedLevelData[kNumberOfLevelsWithPadding]; // 0x949C
uint8_t *gCurrentPlayerLevelData = &gCurrentPlayerPaddedLevelData[kFirstLevelIndex]; // 0x949E
char gCurrentLevelName[kListLevelNameLength]; // 0x87A8

HallOfFameEntry gHallOfFameData[kNumberOfHallOfFameEntries];

PlayerEntry gPlayerListData[kNumberOfPlayers];

char a00s0010_sp[12] = "00S001$0.SP"; // 0xA014
char gPlayerName[kPlayerNameLength + 1] = "WIBBLE??"; // 0x879F
uint8_t gCurrentPlayerIndex = 0; // byte_5981F
uint8_t byte_58D46 = 0;

char gRankingTextEntries[kNumberOfPlayers + 4][kRankingEntryTextLength] = { //0x880E
    "                      ",
    "                      ",
    "001 SUPAPLEX 000:00:00",
    "002 SUPAPLEX 000:00:00",
    "003 SUPAPLEX 000:00:00",
    "004 SUPAPLEX 000:00:00",
    "005 SUPAPLEX 000:00:00",
    "006 SUPAPLEX 000:00:00",
    "007 SUPAPLEX 000:00:00",
    "008 SUPAPLEX 000:00:00",
    "009 SUPAPLEX 000:00:00",
    "010 SUPAPLEX 000:00:00",
    "011 SUPAPLEX 000:00:00",
    "012 SUPAPLEX 000:00:00",
    "013 SUPAPLEX 000:00:00",
    "014 SUPAPLEX 000:00:00",
    "015 SUPAPLEX 000:00:00",
    "016 SUPAPLEX 000:00:00",
    "017 SUPAPLEX 000:00:00",
    "018 SUPAPLEX 000:00:00",
    "019 SUPAPLEX 000:00:00",
    "020 SUPAPLEX 000:00:00",
    "                      ",
    "                      ",
};

char gLevelsDatFilename[11] = "LEVELS.DAT";
char gLevelLstFilename[10] = "LEVEL.LST";
char gPlayerLstFilename[11] = "PLAYER.LST";
char gHallfameLstFilename[13] = "HALLFAME.LST";
char gDemo0BinFilename[10] = "DEMO0.BIN";
char gSavegameSavFilename[13] = "SAVEGAME.SAV";

uint16_t gIsDebugModeEnabled = 0; // word_51970
uint8_t gIsForcedLevel = 0; // byte_59B84
uint8_t gIsForcedCheatMode = 0; // byte_59B85

uint8_t gGameSpeed = kDefaultGameSpeed; // gameSpeed
char demoFileName[kMaxFilePathLength] = "";

FastModeType gFastMode = FastModeTypeNone;

uint16_t gScrollOffsetX = 0; // word_5195F
uint16_t gScrollOffsetY = 0; // word_51961

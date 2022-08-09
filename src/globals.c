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

char gSPDemoFileName[12] = "00S001-0.SP"; // 0xA014
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
uint8_t gIsGameSpeedChangeButtonPressed = 0;
char demoFileName[kMaxFilePathLength] = "";

FastModeType gFastMode = FastModeTypeNone;

uint8_t gShouldShowFPS = 0;
uint8_t gShouldLimitFPS = 1;

uint8_t gIsPlayingDemo = 0; // byte_510DE -> 0DCE
uint8_t gIsRecordingDemo = 0; // byte_510E3 -> 0DD3

uint16_t gScrollOffsetX = 0; // word_5195F
uint16_t gScrollOffsetY = 0; // word_51961

uint16_t gMouseButtonStatus = 0; // word_5847D
uint16_t gMouseX = 0, gMouseY = 0;

uint16_t gShouldExitGame = 0; // word_5197A

uint8_t gIsGameRunning = 0; // byte_510AE

uint8_t gCurrentSoundPriority = 0; // byte_59889
uint8_t gCurrentSoundDuration = 0; // byte_5988B

// ----------- BEGINNING OF STATE SAVED IN SAVEGAMES -----------

StatefulLevelTile gCurrentLevelStateWithPadding[kLevelDataLength + kSizeOfLevelStatePrecedingPadding]; // 0x1584
StatefulLevelTile *gCurrentLevelState = &gCurrentLevelStateWithPadding[kSizeOfLevelStatePrecedingPadding]; // located at 0x1834, size is kLevelDataLength items
int8_t gExplosionTimers[kLevelDataLength]; // 0x2434
uint8_t gIsGravityEnabled = 0; // byte_5101C -> 1 = turn on, anything else (0) = turn off
uint8_t gAreZonksFrozen = 0; // byte_51035 -> 2 = turn on, anything else (0) = turn off  (1=off!)
uint8_t gNumberOfInfoTrons = 0; // 0xd26 -> byte_51036 -> this seems to be _inside_ of fileLevelData when a level is read
uint8_t gNumberOfSpecialPorts = 0; // 0xd27 -> byte_51037 this seems to be _inside_ of fileLevelData when a level is read, and it's numberOfSpecialPorts
uint16_t gRandomSeed = 0; // word_51076
// uint16_t word_510A2 = 0; // -> used to preserve some palette info??
// uint8_t gNumberOfDotsToShiftDataLeft = 0; // byte_510A6 Used for the scroll effect
// uint16_t word_510A7  dw 0
// uint16_t word_510A9  dw 0
// uint8_t byte_510AB = 0;
// uint16_t word_510AC = 0; // stored in 0B5D:0D9C
uint8_t gAuxGameSeconds20msAccumulator = 0; // byte_510AF ->  -> accumulates game time. The total time is its value * 20ms, so when it reaches 50 it means 1 second. Used to increase the game time in the bottom panel
uint8_t gGameSeconds = 0; // byte_510B0
uint8_t gGameMinutes = 0; // byte_510B1
uint8_t gGameHours = 0; // byte_510B2
uint8_t gShouldUpdateTotalLevelTime = 0;
uint8_t gLevelFailed = 0; // byte_510BA
PlayerLevelState gCurrentPlayerLevelState = PlayerLevelStateNotCompleted;
// uint16_t word_510BC = 0;
// uint16_t word_510BE = 0;
uint8_t gIsExplosionStarted = 0; // byte_510C0 -> Set to 1 when an explosion is just created. Set back to 0 when _any_ of the explosions on the screen disappears.
// These two were actually grouped in word_510C1 for some (compiler) reason
uint8_t gShouldShowGamePanel = 0; // byte_510C1 -> 0DB1
uint8_t gToggleGamePanelKeyAutoRepeatCounter = 0; // byte_510C2 -> 0DB2
int16_t gMurphyTileX = 0; // word_510C3
int16_t gMurphyTileY = 0; // word_510C5
int16_t gMurphyPreviousLocation = 0;
int16_t gMurphyLocation = 0;
uint16_t gIsMurphyLookingLeft = 0;
uint16_t gMurphyYawnAndSleepCounter = 0; // word_510CD
uint16_t gIsMurphyUpdated = 0; // word_510CF
uint16_t gShouldKillMurphy = 0; // word_510D1
uint8_t gPreviousUserInputWasNone = 0; // byte_510D3 -> used to detect when to release the red disk
uint8_t gAreEnemiesFrozen = 0; // byte_510D7 -> 1 = turn on, anything else (0) = turn off
uint8_t gScratchGravity = 0; // byte_510D8 -> not sure what scratch gravity means exactly, but can be 0 (off) or 1 (on)
uint16_t gIsMurphyGoingThroughPortal = 0; // word_510D9
uint8_t gPlantedRedDiskCountdown = 0; // byte_510DB
uint16_t gPlantedRedDiskPosition = 0; // word_510DC
uint16_t gDemoCurrentInputIndex = 0;
uint8_t gDemoCurrentInput = 0; // byte_510E1 -> 0xDD1
uint8_t gDemoCurrentInputRepeatCounter = 0; // -> 0xDD2 -> byte_510E2
uint16_t gDemoIndexOrDemoLevelNumber = 0; // word_510E6
uint16_t gMurphyPositionX = 0; // word_510E8
uint16_t gMurphyPositionY = 0; // word_510EA
uint16_t gMurphyCounterToStartPushAnimation = 0;
MurphyAnimationDescriptor gCurrentMurphyAnimation; // -> starts at 0x0DE0
uint8_t gNumberOfRemainingInfotrons = 0; // byte_5195A
uint8_t gTotalNumberOfInfotrons = 0; // byte_5195B
uint8_t gNumberOfRemainingRedDisks = 0; // byte_5195C
uint16_t gFrameCounter = 0xF000; // word_5195D -> 0x1268
// uint16_t word_51967 = 0; // scroll / first pixel of the scroll window
// uint8_t byte_51969 = 0; //  db 0
uint8_t gTerminalMaxFramesToNextScroll = 0;  //  db 0
uint8_t gAreYellowDisksDetonated = 0; //  db 0
uint16_t gShouldLeaveMainMenu = 0; //  dw 0
//dw 1
//dw 1
uint16_t gShouldExitLevel = 0; // word_51974
uint16_t gQuitLevelCountdown = 0; // word_51978 -> this is a counter to end the level after certain number of iterations (to let the game progress a bit before going back to the menu)
uint8_t gAdditionalInfoInGamePanelFrameCounter = 0; //  db 0
// fileLevelData starts at 0x768, when it contains a level goes to 0xD67
Level gCurrentLevel; // 0x988B
// ----------- END OF STATE SAVED IN SAVEGAMES -----------

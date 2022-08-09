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

#ifndef globals_h
#define globals_h

#include <stdint.h>

#include "animations.h"
#include "file.h"

#define VERSION_STRING "7.2"
extern const uint8_t kGameVersion;

typedef enum
{
    LevelTileTypeSpace = 0,
    LevelTileTypeZonk = 1,
    LevelTileTypeBase = 2,
    LevelTileTypeMurphy = 3,
    LevelTileTypeInfotron = 4,
    LevelTileTypeChip = 5,
    LevelTileTypeHardware = 6,
    LevelTileTypeExit = 7,
    LevelTileTypeOrangeDisk = 8,
    LevelTileTypePortRight = 9,
    LevelTileTypePortDown = 10,
    LevelTileTypePortLeft = 11,
    LevelTileTypePortUp = 12,
    LevelTileTypeSportRight = 13,
    LevelTileTypeSportDown = 14,
    LevelTileTypeSportLeft = 15,
    LevelTileTypeSportUp = 16,
    LevelTileTypeSnikSnak = 17,
    LevelTileTypeYellowDisk = 18,
    LevelTileTypeTerminal = 19,
    LevelTileTypeRedDisk = 20,
    LevelTileTypePortVertical = 21,
    LevelTileTypePortHorizontal = 22,
    LevelTileTypePort4Way = 23,
    LevelTileTypeElectron = 24,
    LevelTileTypeBug = 25,
    LevelTileTypeHorizontalChipLeft = 26,
    LevelTileTypeHorizontalChipRight = 27,
    LevelTileTypeHardware2 = 28,
    LevelTileTypeHardware3 = 29,
    LevelTileTypeHardware4 = 30,
    LevelTileTypeExplosion = 31,
    LevelTileTypeHardware6 = 32,
    LevelTileTypeHardware7 = 33,
    LevelTileTypeHardware8 = 34,
    LevelTileTypeHardware9 = 35,
    LevelTileTypeHardware10 = 36,
    LevelTileTypeHardware11 = 37,
    LevelTileTypeHorizontalChipTop = 38,
    LevelTileTypeHorizontalChipBottom = 39,
    LevelTileTypeCount,
} LevelTileType;

// exact length of a level file, even of each level inside the LEVELS.DAT file
#define kLevelDataLength 1536

#define kLevelWidth 60 // 3Ch
#define kLevelHeight 24 // 18h
#define kLevelSize (kLevelWidth * kLevelHeight) // 1440  or 5A0h
//static const size_t kLevelcells = levelSize
extern uint16_t gLevelData[kLevelSize];

// Seems like the way the game handle the level list is having empty level entries at the beginning and at the
// end. They do that instead of checking boundaries when drawing the list. So these two empty spaces will be rendered
// (but not selectable) in the level list. For now I will mimick that behavior, in case it's something on which the
// rest of the game depends. Once everything is reverse-engineered, it can be changed to a normal and sane implementation.
// Also, aside from the levels themselves, they seem to have some kind of informative entries at the end of the level
// list encouraging the player to replay skipped levels.
//
#define kNumberOfLevels 111
#define kNumberOfLevelsWithPadding (kNumberOfLevels + 5)
#define kFirstLevelIndex 2
#define kLastLevelIndex (kFirstLevelIndex + kNumberOfLevels)
#define kListLevelNameLength 28 // In the list of levels, every level is 28 bytes long and looks like "001                        \n"
extern uint8_t gPaddedLevelListData[kNumberOfLevelsWithPadding * kListLevelNameLength]; // 0x2A34

#define kLevelListDataLength (kNumberOfLevels * kListLevelNameLength)
extern uint8_t *gLevelListData;

extern const int kNotCompletedLevelEntryColor;
extern const int kCompletedLevelEntryColor;
extern const int kBlockedLevelEntryColor;
extern const int kSkippedLevelEntryColor;

// This array contains info about the levels. It's updated when player is changed.
// There are different values per level, so far I've seen:
// - 02 level not completed, but can be played
// - 04 level completed
// - 06 level not completed, and it CANNOT be played
// - 08 level skipped
// These also seem to be used as colors. Not sure if they're colors used as some other info, or some other info used as colors.
//
extern uint8_t gCurrentPlayerPaddedLevelData[kNumberOfLevelsWithPadding]; // 0x949C

static const int kCurrentPlayerkLevelDataLength = kNumberOfLevels;
extern uint8_t *gCurrentPlayerLevelData; // 0x949E

// uint16_t word_58AB8 = 0x3231; // -> 0x87A8
// uint16_t word_58ABA = 0x3433; // -> 0x87AA
extern char gCurrentLevelName[kListLevelNameLength]; // 0x87A8

#define kLevelNameLength 24

typedef struct {
    uint16_t position; // If (x,y) are the coordinates of a port in the field
                       // and (0,0) is the top-left corner, the 16 bit value
                       // here calculates as 2*(x+(y*60)).  This is twice of
                       // what you may have expected: Supaplex works with a
                       // game field in memory, which is 2 bytes per sprite.

    uint8_t gravity; // 1 = turn on, anything else (0) = turn off
    uint8_t freezeZonks; // 2 = turn on, anything else (0) = turn off  (1=off!)
    uint8_t freezeEnemies; // 1 = turn on, anything else (0) = turn off
    uint8_t unused; // Doesn't matter: is ignored.
} SpecialPortInfo;

#define kLevelMaxNumberOfSpecialPorts 10

typedef struct
{
    uint8_t tiles[kLevelSize]; // [0-0x59F] of LevelTileType
    uint8_t unused[4];
    uint8_t initialGravitation;
    uint8_t speedFixMagicNumber; // Used from versions 5.3 and up as: 20h + SpeedFix version number in hex format: v5.3 -> 73h, v6.2 -> 82h
    char name[kLevelNameLength - 1];
    uint8_t freezeZonks; // 2 = on, anything else (including 1) = off
    uint8_t numberOfInfotrons; // 0 means that Supaplex will count the total amount of Infotrons in the level, and use the low byte of that number. (A multiple of 256 Infotrons will then result in 0-to-eat, etc.!)
    uint8_t numberOfSpecialPorts; // maximum 10
    SpecialPortInfo specialPortsInfo[kLevelMaxNumberOfSpecialPorts];

    // This byte carries the information of the slowest speed used during the demo recording. 0x00=fastest, 0x0A=slowest
    // This information is exclusive-ored with the high random number byte (byte scrambledSpeed). (Each bit is toggled, where in byte highByteRandomSeed a 1 appears.)
    // The result is the value of byte scrambledSpeed (and is used to scramble byte scrambledChecksum).
    //
    uint8_t scrambledSpeed;

    // All upper nibbles of each demo byte (without first level number byte and without ending 0xFF), each nibble
    // incremented by 1, are added up. This total equals the total number of demo frames and reflects the normalized
    // demo time with 35 frames per second.
    // To this total, of which only the lower 8 bits are used, the lower random number byte (byte lowByteRandomSeed) is added.
    // The resulting lower 8 bits are exclusive-ored with the final contents of byte scrambledSpeed. (Each bit is toggled,
    // where in byte scrambledSpeed a 1 appears.)
    // The resulting lower 8 bits is the value of byte scrambledChecksum.
    // Note: Megaplex does not put any information into bytes scrambledSpeed and scrambledChecksum.
    //
    uint8_t scrambledChecksum;

    // All Bugs are fired randomly, so in order to be able to make a recording of a level with Bugs, it is necessary to let
    // them fire exactly at the same time in each playback of that recording. In order to guarantee that, we need a
    // predictable random number generator and start it each playback with the same starting value (seed) as when the
    // recording was started. When the sequence of all following random numbers is repeatable, all Bugs will always fire
    // the same way during each playback as during the creation of the recording.
    // Luckily the original Supaplex uses a very simple random number generator for this purpose, which is not depending
    // on external influences like date and time or a keypress. Start the random number generator with a random number
    // seed and the next random number is calculated, which is also used as seed for the next calculation. A certain
    // seed will always result in only one specific random number. The sequence of all following random numbers is
    // thus fixed for each seed.
    // So at the start of each recording, we need to remember the starting random number as seed for the random number
    // generator during each playback.
    // Each random number is a 16 bit number. After each random number calculation, only the lower 16 bits are kept as
    // seed for the next calculation: new_random_number_seed = ((old_random_number_seed * 1509) + 49) modulo 65536
    // This "modulo 65536" just signifies keeping only the lower 16 bits and reject all higher bits.
    //
    uint16_t randomSeed;
} Level; // size 1536 = 0x600

#define kPlayerNameLength 8

typedef struct
{
    char playerName[kPlayerNameLength + 1];
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} HallOfFameEntry;

#define kNumberOfHallOfFameEntries 3
extern HallOfFameEntry gHallOfFameData[kNumberOfHallOfFameEntries]; // 0x9514 -> asc_59824

typedef enum {
    PlayerLevelStateNotCompleted = 0,
    PlayerLevelStateCompleted = 1,
    PlayerLevelStateSkipped = 2,
} PlayerLevelState;

// This is a structure I still need to reverse-engineer. It's 128 bytes long:
// - 9 bytes (0x00-0x08): player name
// - 1 byte (0x09): hours
// - 1 byte (0x0A): minutes
// - 1 byte (0x0B): seconds
// - 111 bytes (0x0C-0x7A): level state (1 byte per level)
// - 0x7B, 0x7C, 0x7D ??
// - 1 byte (0x7E): next level to play
// - 1 byte (0x7F): 1 if the user finished all levels ??
//
typedef struct
{
    char name[kPlayerNameLength + 1];
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t levelState[kNumberOfLevels]; // values are PlayerLevelState
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t nextLevelToPlay;
    uint8_t completedAllLevels; // Still not 100% sure
} PlayerEntry;

#define kNumberOfPlayers 20
//static const int kPlayerEntryLength = 128;
extern PlayerEntry gPlayerListData[kNumberOfPlayers]; // 0x8A9C -> word_58DAC

extern char gSPDemoFileName[12]; // a00s0010_sp -> 0xA014
extern char gPlayerName[kPlayerNameLength + 1]; // 0x879F
extern uint8_t gCurrentPlayerIndex; // byte_5981F
extern uint8_t byte_58D46; // also has the player index??

#define kRankingFirstPlayerIndex 2
#define kRankingEntryTextLength 23

extern char gRankingTextEntries[kNumberOfPlayers + 4][kRankingEntryTextLength];

extern char gLevelsDatFilename[11];
extern char gLevelLstFilename[10];
extern char gPlayerLstFilename[11];
extern char gHallfameLstFilename[13];
extern char gDemo0BinFilename[10];
extern char gSavegameSavFilename[13];

extern uint16_t gIsDebugModeEnabled; // word_51970
extern uint8_t gIsForcedLevel; // byte_59B84
extern uint8_t gIsForcedCheatMode; // byte_59B85

// Game Speed in "Speed Fix values" from 0 to 10, but inverted. 10 is the
//   fastest, 0 the slowest. 5 is the "target" speed in spfix63, which was 35 game iterations per second.
//
#define kNumberOfGameSpeeds 11
#define kDefaultGameSpeed 10
extern uint8_t gGameSpeed; // gameSpeed
extern uint8_t gIsGameSpeedChangeButtonPressed;
extern char demoFileName[kMaxFilePathLength];

typedef enum
{
    FastModeTypeNone,
    FastModeTypeNormal,
    FastModeTypeUltra,
} FastModeType;

extern FastModeType gFastMode;

extern uint8_t gShouldShowFPS;
extern uint8_t gShouldLimitFPS;

extern uint8_t gIsPlayingDemo; // byte_510DE -> 0DCE
extern uint8_t gIsRecordingDemo; // byte_510E3 -> 0DD3

extern uint16_t gScrollOffsetX; // word_5195F
extern uint16_t gScrollOffsetY; // word_51961

extern uint16_t gMouseButtonStatus; // word_5847D
extern uint16_t gMouseX, gMouseY;

extern uint16_t gShouldExitGame; // word_5197A

extern uint8_t gIsGameRunning; // byte_510AE

extern uint8_t gCurrentSoundPriority; // byte_59889 -> 0x9579 -> the lower this value, the higher its priority. 0 means no sound playing
extern uint8_t gCurrentSoundDuration; // byte_5988B -> 0x957B -> remaining time of the current sound, each unit is 20ms

typedef struct __attribute__ ((packed)) {
    uint8_t tile; // of LevelTileType
    uint8_t state;
} StatefulLevelTile;

// ----------- BEGINNING OF STATE SAVED IN SAVEGAMES -----------

// Use 1536 (level file length) instead of 1440 (level actual size in tiles, 60x24) here. The reason? Seems like
// some levels/demos rely on this when they use the glitch with empty borders.
// A level is 1536 bytes. when loaded into memory , we only care about 1440 tiles (60 x 24) in two "playfields":
// - one where every tile needs 2 bytes: that's where the game is actually represented
// - another one where every tile needs 1 byte: that's the "explosion timers" for each tile
//
// I saw the original code used  1536 * 2  bytes for the first playfield, and  1536 * 1  bytes for the second, instead of 1440*2  and 1440*1  respectively. also they were contiguous in memory.
// since I knew 1440 is the right size, I just used that and ignored the original code. what happens? when there are empty borders, without the extra padding of those 1536, seems like it was writing in the explosion timers because it's contiguous in memory :joy:
// I just added that extra padding back and :tada: no more memory corrupted :joy:
// This was reproduced with test 03/03s049-3.sp
//
// Also some levels rely on the memory that lies BEFORE the level state, which contains many things including keys
// pressed, some strings, etc. This implementation will use some hardcoded data that will be injected when the game is
// initialized and never touched again. Should be enough to simulate the behavior of the original game.
//
#define kSizeOfLevelStatePrecedingPadding 344
extern StatefulLevelTile gCurrentLevelStateWithPadding[kLevelDataLength + kSizeOfLevelStatePrecedingPadding]; // 0x1584
extern StatefulLevelTile *gCurrentLevelState; // located at 0x1834, size is kLevelDataLength items
extern int8_t gExplosionTimers[kLevelDataLength]; // 0x2434
extern uint8_t gIsGravityEnabled; // byte_5101C -> 1 = turn on, anything else (0) = turn off
extern uint8_t gAreZonksFrozen; // byte_51035 -> 2 = turn on, anything else (0) = turn off  (1=off!)
extern uint8_t gNumberOfInfoTrons; // 0xd26 -> byte_51036 -> this seems to be _inside_ of fileLevelData when a level is read
extern uint8_t gNumberOfSpecialPorts; // 0xd27 -> byte_51037 this seems to be _inside_ of fileLevelData when a level is read, and it's numberOfSpecialPorts
extern uint16_t gRandomSeed; // word_51076
// extern uint16_t word_510A2; // -> used to preserve some palette info??
// uint8_t gNumberOfDotsToShiftDataLeft; // byte_510A6 Used for the scroll effect
// uint16_t word_510A7  dw 0
// uint16_t word_510A9  dw 0
// extern uint8_t byte_510AB;
// extern uint16_t word_510AC; // stored in 0B5D:0D9C
extern uint8_t gAuxGameSeconds20msAccumulator; // byte_510AF ->  -> accumulates game time. The total time is its value * 20ms, so when it reaches 50 it means 1 second. Used to increase the game time in the bottom panel
extern uint8_t gGameSeconds; // byte_510B0
extern uint8_t gGameMinutes; // byte_510B1
extern uint8_t gGameHours; // byte_510B2
extern uint8_t gShouldUpdateTotalLevelTime; // byte_510B3
extern uint8_t gLevelFailed; // byte_510BA
extern PlayerLevelState gCurrentPlayerLevelState; // byte_510BB
// extern uint16_t word_510BC; -> stores the same as gMurphyTileX wtf?
// extern uint16_t word_510BE; -> stores the same as gMurphyTileY wtf?
extern uint8_t gIsExplosionStarted; // byte_510C0 -> Set to 1 when an explosion is just created. Set back to 0 when _any_ of the explosions on the screen disappears.
// These two were actually grouped in word_510C1 for some (compiler) reason
extern uint8_t gShouldShowGamePanel; // byte_510C1 -> 0DB1
extern uint8_t gToggleGamePanelKeyAutoRepeatCounter; // byte_510C2 -> 0DB2
extern int16_t gMurphyTileX; // word_510C3
extern int16_t gMurphyTileY; // word_510C5
extern int16_t gMurphyPreviousLocation; // word_510C7
extern int16_t gMurphyLocation;
extern uint16_t gIsMurphyLookingLeft; // word_510CB
extern uint16_t gMurphyYawnAndSleepCounter; // word_510CD
extern uint16_t gIsMurphyUpdated; // word_510CF
extern uint16_t gShouldKillMurphy; // word_510D1
extern uint8_t gPreviousUserInputWasNone; // byte_510D3 -> used to detect when to release the red disk
extern uint8_t gAreEnemiesFrozen; // byte_510D7 -> 1 = turn on, anything else (0) = turn off
extern uint8_t gScratchGravity; // byte_510D8 -> not sure what scratch gravity means exactly, but can be 0 (off) or 1 (on)
extern uint16_t gIsMurphyGoingThroughPortal; // word_510D9
extern uint8_t gPlantedRedDiskCountdown; // byte_510DB
extern uint16_t gPlantedRedDiskPosition; // word_510DC
extern uint16_t gDemoCurrentInputIndex; // word_510DF
extern uint8_t gDemoCurrentInput; // byte_510E1 -> 0xDD1
extern uint8_t gDemoCurrentInputRepeatCounter; // -> 0xDD2 -> byte_510E2
extern uint16_t gDemoIndexOrDemoLevelNumber; // word_510E6
extern uint16_t gMurphyPositionX; // word_510E8
extern uint16_t gMurphyPositionY; // word_510EA
extern uint16_t gMurphyCounterToStartPushAnimation; // word_510EE -> also used in the "release red disk" animation
extern MurphyAnimationDescriptor gCurrentMurphyAnimation; // -> starts at 0x0DE0
extern uint8_t gNumberOfRemainingInfotrons; // byte_5195A
extern uint8_t gTotalNumberOfInfotrons; // byte_5195B
extern uint8_t gNumberOfRemainingRedDisks; // byte_5195C
extern uint16_t gFrameCounter; // word_5195D -> 0x1268
// uint16_t word_51967; // scroll / first pixel of the scroll window
// extern uint8_t byte_51969; //  db 0
extern uint8_t gTerminalMaxFramesToNextScroll; // byte_5196A -> this is used to make the terminals scroll their screens faster after the yellow disks have been detonated
extern uint8_t gAreYellowDisksDetonated; // byte_5196B
extern uint16_t gShouldLeaveMainMenu; // word_5196C
//dw 1
//dw 1
extern uint16_t gShouldExitLevel; // word_51974
extern uint16_t gQuitLevelCountdown; // word_51978 -> this is a counter to end the level after certain number of iterations (to let the game progress a bit before going back to the menu)
extern uint8_t gAdditionalInfoInGamePanelFrameCounter; // byte_5197C -> how many more frames the additional info in the game panel will be
// fileLevelData starts at 0x768, when it contains a level goes to 0xD67
extern Level gCurrentLevel; // 0x988B
// ----------- END OF STATE SAVED IN SAVEGAMES -----------

#endif /* globals_h */

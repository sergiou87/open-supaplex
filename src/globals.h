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

#include "file.h"

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
} LevelTileType;

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

static const int kCurrentPlayerLevelDataLength = kNumberOfLevels;
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
    uint8_t unknown0[4];
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

enum PlayerLevelState {
    PlayerLevelStateNotCompleted = 0,
    PlayerLevelStateCompleted = 1,
    PlayerLevelStateSkipped = 2,
};

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

extern char a00s0010_sp[12]; // 0xA014
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
//   fastest, 0 the slowest. 5 means the original speed which was 35 game iterations per second.
//
#define kNumberOfGameSpeeds 11
extern const uint8_t kDefaultGameSpeed;
extern uint8_t gGameSpeed; // gameSpeed
extern char demoFileName[kMaxFilePathLength];

#endif /* globals_h */

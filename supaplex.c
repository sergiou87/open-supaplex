/*
 * This file is part of the Supaplex-RE distribution (https://github.com/sergiou87/supaplex-re).
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

#include <assert.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <time.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(v, a, b) MIN(MAX(a, v), b)

static const int kScreenWidth = 320;
static const int kScreenHeight = 200;

// title1DataBuffer -> A000:4DAC - A000:CAAC
// title2DataBuffer -> 0x4DD4 - 0xCAD4

// maps are 58 x 22 tiles

uint8_t fastMode = 0;

static const int levelDataLength = 1536; // exact length of a level file, even of each level inside the LEVELS.DAT file
uint8_t gPlayerListDownButtonPressed = 0; // byte_50910
uint8_t gPlayerListUpButtonPressed = 0; // byte_50911
uint8_t gPlayerListButtonPressed = 0; // byte_50912
uint8_t gRankingListDownButtonPressed = 0; // byte_50913
uint8_t gRankingListUpButtonPressed = 0; // byte_50914
uint8_t gRankingListButtonPressed = 0; // byte_50915
uint8_t gLevelListDownButtonPressed = 0; // byte_50916
uint8_t gLevelListUpButtonPressed = 0; // byte_50917
uint8_t gLevelListButtonPressed = 0; // byte_50918
uint8_t byte_50941 = 0;
uint8_t byte_50946 = 0;
uint8_t byte_50953 = 0;
uint8_t byte_50954 = 0;
uint8_t byte_50919 = 0;
uint8_t gNumberOfDotsToShiftDataLeft = 0; // byte_510A6 Used for the scroll effect
uint8_t byte_510AB = 0;
uint8_t byte_510B3 = 0;
uint8_t byte_510BA = 0;
uint8_t byte_510DE = 0;
uint8_t byte_51969 = 0;
uint8_t byte_5197E = 0;
uint8_t byte_51999 = 0;
uint8_t byte_519B6 = 0;
uint8_t byte_519C3 = 0;
uint8_t byte_519C5 = 0;
uint8_t byte_519C8 = 0;
uint8_t byte_519CA = 0;
uint8_t byte_519CD = 0;
uint8_t byte_519F4 = 0;
uint8_t byte_519F5 = 0;
uint8_t byte_519F6 = 0;
uint8_t byte_519F7 = 0;
uint8_t byte_519F8 = 0;
uint8_t byte_519F9 = 0;
uint8_t byte_51ABE = 0;
uint8_t gIsMouseAvailable = 0; // byte_58487
uint8_t byte_58D46 = 0;
uint8_t byte_58D47 = 0;
uint8_t gNewPlayerEntryIndex = 0; // byte_59820
uint8_t byte_59821 = 0;
uint8_t byte_59822 = 0;
uint8_t byte_59823 = 0;
uint8_t byte_599D4 = 0;
uint8_t byte_59B6B = 0;
uint8_t byte_59B71 = 0;
uint8_t byte_59B83 = 0;
uint8_t byte_59B84 = 0;
uint8_t byte_59B85 = 0;
uint8_t byte_59B86 = 0;
uint8_t byte_5A19B = 0;
uint8_t byte_5A19C = 0;
uint8_t byte_5A2F9 = 0;
uint8_t byte_5A33E = 0;
uint8_t byte_5A33F = 0;
uint8_t byte_59B64 = 0;
uint8_t byte_510AE = 0;
uint8_t byte_510AF = 0;
uint8_t byte_510B0 = 0;
uint8_t byte_510B1 = 0;
uint8_t byte_510B2 = 0;
uint8_t byte_510BB = 0;
uint8_t byte_519B5 = 0;
uint8_t byte_519B8 = 0;
uint8_t byte_519B9 = 0;
uint8_t byte_519BA = 0;
uint8_t byte_519BB = 0;
uint8_t byte_519BC = 0;
uint8_t byte_519BD = 0;
uint8_t byte_519BE = 0;
uint8_t byte_519BF = 0;
uint8_t byte_519C0 = 0;
uint8_t byte_519C1 = 0;
uint8_t byte_519D4 = 0;
uint8_t byte_519D5 = 0;
uint8_t byte_59889 = 0;
uint8_t byte_5988A = 0;
uint8_t byte_5988B = 0;
uint8_t byte_5988C = 0;
uint8_t byte_59B94 = 0;
uint8_t byte_59B96 = 0;
uint8_t byte_5091A = 0;
uint8_t byte_5A320 = 0;
uint8_t byte_5A321 = 0;
uint8_t byte_5A322 = 0;
uint8_t byte_5195C = 0;
uint8_t byte_5197C = 0;
uint8_t byte_51036 = 0;
uint8_t byte_510B9 = 0;
uint8_t byte_510B4 = 0;
uint8_t byte_510B5 = 0;
uint8_t byte_510B6 = 0;
uint8_t byte_510C0 = 0;
uint8_t byte_5195A = 0;
uint8_t byte_5195B = 0;
uint8_t byte_5196A = 0;
uint8_t byte_5196B = 0;
uint8_t byte_510D7 = 0;
uint8_t byte_510DB = 0;
uint8_t byte_510E3 = 0;

uint8_t gCurrentPlayerIndex = 0; // byte_5981F
uint8_t byte_59B62 = 0;
uint8_t byte_5870F = 0;
uint16_t word_58467 = 0;
uint16_t gCurrentSelectedLevelIndex = 0; // word_51ABC
uint16_t word_510DC = 0;
uint16_t word_586FD = 0;
uint16_t word_586FF = 0;
uint16_t word_58701 = 0;
uint16_t word_58703 = 0;
uint16_t word_5870D = 0;
uint16_t word_58710 = 0;
uint16_t word_58712 = 0;
uint16_t word_58714 = 0;
uint16_t word_586FB = 0;
uint16_t word_50942 = 0;
uint16_t word_50944 = 0;
uint16_t word_5094F = 0;
uint16_t word_50951 = 0;
uint16_t word_510A2 = 0;
uint16_t word_510E6 = 0;
uint16_t word_5196C = 0;
uint16_t word_51970 = 0;
uint16_t word_5197A = 0;
uint16_t word_599D8 = 0;
uint16_t word_58463 = 0;
uint16_t word_58465 = 0;
uint16_t gNewPlayerNameLength = 0; // word_58475
uint16_t word_5195D = 0;
uint16_t word_51974 = 0;
uint16_t word_51076 = 0;
uint16_t word_510BC = 0;
uint16_t word_510C3 = 0;
uint16_t word_510BE = 0;
uint16_t word_510C5 = 0;
uint16_t word_510CB = 0;
uint16_t word_510D1 = 0;
uint16_t word_51978 = 0;
uint16_t word_510CD = 0;
uint16_t word_510E8 = 0;
uint16_t word_510EA = 0;
uint16_t word_5157E = 0;
uint16_t word_59B90 = 0;
uint16_t word_59B92 = 0;
uint16_t word_510B7 = 0;
uint16_t word_510C1 = 0;
uint16_t word_510D9 = 0;
uint16_t word_5184A = 0x2A66;
uint16_t word_5184C = 0x2A67;
uint16_t word_5184E = 0x2E36;
uint16_t word_51850 = 0x2E37;
uint16_t word_51852 = 0x2A68;
uint16_t word_51854 = 0x2A69;
uint16_t word_51856 = 0x2E38;
uint16_t word_51858 = 0x2E39;
uint16_t word_5195F = 0;
uint16_t word_51961 = 0;
uint16_t word_51967 = 0;
uint16_t word_58469 = 0;
uint16_t word_5846B = 0;
uint16_t word_5846D = 0;
uint16_t word_5846F = 0;
uint16_t word_58471 = 0;
uint16_t word_58473 = 0;
uint16_t word_59B8C = 0;
uint16_t word_59B8E = 0;
uint16_t word_599D6 = 0;
uint16_t gCursorX = 0; // word_58481
uint16_t gCursorY = 0; // word_58485
uint16_t *dword_58488 = 0;
uint16_t word_5094B = 0;
uint16_t word_599DA = 0;
uint16_t word_5094D = 0;
uint8_t fileIsDemo = 0;
uint8_t isJoystickEnabled = 0; // byte_50940
uint8_t isMusicEnabled = 0; // byte_59886
uint8_t isFXEnabled = 0; // byte_59885
uint8_t videoStatusUnk = 0;
SDL_Scancode keyPressed = 0;
int8_t speed1 = 0;
int8_t speed2 = 0;
int8_t speed3 = 0;
int8_t gameSpeed = 0;
uint8_t demoFileName = 0; // Probably should be another type but whatever for now

// This points to the address on the screen where the mouse cursor was
// drawn the last time, used to clear the cursor before redrawing it again
//
uint16_t gLastMouseCursorOriginAddress = 0; // word_5847B

// This buffer will hold the bitmap of the area where the cursor is going to be drawn
// with the intention of restoring that area before rendering the mouse in a new
// position, hence clearing the trail the mouse would leave.
//
static const int kLastMouseCursorAreaSize = 8;
uint8_t gLastMouseCursorAreaBitmap[kLastMouseCursorAreaSize * kLastMouseCursorAreaSize];

static const int kConfigDataLength = 4;

uint32_t gTimeOfDay = 0;

typedef enum
{
    SoundTypeNone = 0,
    SoundTypeInternalStandard = 1,
    SoundTypeInternalSamples = 2,
    SoundTypeAdlib = 3,
    SoundTypeSoundBlaster = 4,
    SoundTypeRoland = 5,
} SoundType;

enum MouseButton
{
    MouseButtonLeft = 1 << 0,
    MouseButtonRight = 1 << 1,
};

uint16_t gMouseButtonStatus = 0; // word_5847D
uint16_t gMouseX = 0, gMouseY = 0;

char a00s0010_sp[12] = "00S001$0.SP";
char aLevels_dat_0[11] = "LEVELS.DAT";
char gPlayerName[9] = "WIBBLE  "; // 0x879F // TODO: read this address when the game starts and put the same text here

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
} LevelTileType;

static const size_t kLevelWidth = 60; // 3Ch
static const size_t kLevelHeight = 24; // 18h
static const size_t kLevelSize = kLevelWidth * kLevelHeight; // 1440/5A0h
//static const size_t kLevelcells = levelSize
uint16_t gLevelData[kLevelSize];
uint16_t gMurphyLocation = 0;

// Seems like the way the game handle the level list is having empty level entries at the beginning and at the
// end. They do that instead of checking boundaries when drawing the list. So these two empty spaces will be rendered
// (but not selectable) in the level list. For now I will mimick that behavior, in case it's something on which the
// rest of the game depends. Once everything is reverse-engineered, it can be changed to a normal and sane implementation.
// Also, aside from the levels themselves, they seem to have some kind of informative entries at the end of the level
// list encouraging the player to replay skipped levels.
//
static const int kNumberOfLevels = 111;
static const int kNumberOfLevelsWithPadding = kNumberOfLevels + 5;
static const int kFirstLevelIndex = 2;
static const int kLastLevelIndex = kFirstLevelIndex + kNumberOfLevels;
static const int kLevelNameLength = 28; // In the list of levels, every level is 28 bytes long and looks like "001                        \n"
uint8_t gPaddedLevelListData[kNumberOfLevelsWithPadding * kLevelNameLength];

static const int kLevelListDataLength = kNumberOfLevels * kLevelNameLength;
uint8_t *gLevelListData = &gPaddedLevelListData[kFirstLevelIndex * kLevelNameLength];

static const int kNotCompletedLevelEntryColor = 2;
static const int kCompletedLevelEntryColor = 4;
static const int kBlockedLevelEntryColor = 6;
static const int kSkippedLevelEntryColor = 8;

// This array contains info about the levels. It's updated when player is changed.
// There are different values per level, so far I've seen:
// - 02 level not completed, but can be played
// - 04 level completed
// - 06 level not completed, and it CANNOT be played
// - 08 level skipped
// These also seem to be used as colors. Not sure if they're colors used as some other info, or some other info used as colors.
//
uint8_t gCurrentPlayerPaddedLevelData[kNumberOfLevelsWithPadding]; // 0x949C

static const int kCurrentPlayerLevelDataLength = kNumberOfLevels;
uint8_t *gCurrentPlayerLevelData = &gCurrentPlayerPaddedLevelData[kFirstLevelIndex]; // 0x949E

char *gCurrentLevelName[kLevelNameLength]; // 0x87A8

typedef struct
{
    uint8_t tiles[1440]; // of LevelTileType
    uint8_t unknown0[6];
    char name[kLevelNameLength];
    uint8_t unknown2[62];
} Level; // size 1536 = 0x600

Level gCurrentLevel; // 0x988B

// Stores the same info as gCurrentLevel but with each byte separated by 0x00, it's done in readLevels for some unknown reason
uint16_t gCurrentLevelWord[levelDataLength]; // 0x1834
// And this is initialized to 0 in readLevels, and in memory it's supposed to exist right after gCurrentLevelWord
Level gCurrentLevelAfterWord; // 0x2434

// 30 elements...
int word_599DC[] = { 0x00CE, 0x016A, 0x0146, 0x00CD, 0x024D, 0x012C, 0x01A7, 0x01FB, 0x01D2,
                    0x02FD, 0xF001, 0xF1F0, 0xF003, 0xF350, 0xF007, 0xF460,
                    0xF00B, 0xF0F0, 0xF01D, 0xF0F0, 0xF026, 0x50F0, 0xF037,
                    0x41D0, 0x105F, 0xF3F3, 0xF068, 0x10F0, 0x106C, 0x94F4 };

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;
uint8_t soundEnabled = 0;

static const uint8_t kNumberOfColors = 16;
static const size_t kPaleteDataSize = kNumberOfColors * 4;
static const uint8_t kNumberOfPalettes = 4;

typedef SDL_Color ColorPalette[kNumberOfColors];
typedef uint8_t ColorPaletteData[kPaleteDataSize];

static const size_t kBitmapFontCharacterHeight = 7;
static const size_t kBitmapFontCharacterWidth = 6;
static const size_t kNumberOfCharactersInBitmapFont = 64;
static const size_t kBitmapFontLength = kNumberOfCharactersInBitmapFont * 8; // The size of the bitmap is a round number, not 100% related to the size font, there is some padding :shrug:

uint8_t gChars6BitmapFont[kBitmapFontLength];
uint8_t gChars8BitmapFont[kBitmapFontLength];

// This is a 320x24 bitmap
uint8_t gPanelBitmapData[3840];

static const size_t kFullScreenBitmapLength = kScreenWidth * kScreenHeight / 2; // They use 4 bits to encode pixels

// These buffers contain the raw bitmap data of the corresponding DAT files, separated in 4 bitmaps:
//  - The bitmap has 4 columns of 40 bytes width x 200 bytes height
//  - Every column represents a component (R, G, B and intensity).
//  - In one of these columns/components, every bit represents a pixel (40 bytes x 8 bits = 320 pixels)
//  - The way these are decoded seems to be to pick, for every pixel, the bit of the r column, the bit of the g column,
//    the bit from the b column and the bit from the intensity column, and create a 4 bit number that represents an index
//    in the 16 color palette.
//
uint8_t gMenuBitmapData[kFullScreenBitmapLength]; // 0x4D34 ??
uint8_t gControlsBitmapData[kFullScreenBitmapLength];
uint8_t gBackBitmapData[kFullScreenBitmapLength];
uint8_t gGfxBitmapData[kFullScreenBitmapLength];

static const size_t kFullScreenFramebufferLength = kScreenWidth * kScreenHeight; // We only use 16 colors, but SDL doesn't support that mode, so we use 256 colors

// This buffer has the contents of TITLE2.DAT after it's been "decoded" (i.e. after picking the different channels
// every 40 bytes and forming the 4 bit palette index for each pixel).
//
uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];

uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];

static const uint16_t kLevelEdgeSize = 8;
static const int kTileSize = 16;
static const int kLevelBitmapWidth = kTileSize * (kLevelWidth - 2) + kLevelEdgeSize + kLevelEdgeSize;
static const int kLevelBitmapHeight = kTileSize * (kLevelHeight - 2) + kLevelEdgeSize + kLevelEdgeSize;
uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

typedef struct
{
    char playerName[9];
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} HallOfFameEntry;

static const int kNumberOfHallOfFameEntries = 3;
HallOfFameEntry gHallOfFameData[kNumberOfHallOfFameEntries]; // 0x9514

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
    char name[9];
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

static const int kNumberOfPlayers = 20;
//static const int kPlayerEntryLength = 128;
PlayerEntry gPlayerListData[kNumberOfPlayers]; // 0x8A9C

char gRankingTextEntries[kNumberOfPlayers][23] = { //0x880E
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
    "                      ",
};

typedef struct
{
    uint16_t startX, startY;
    uint16_t endX, endY;
    void (*handler)(void);
} ButtonDescriptor;

void handleNewPlayerOptionClick(void);
void handlePlayerListScrollUp(void);
void handlePlayerListScrollDown(void);
void handlePlayerListClick(void);
void handleLevelListScrollUp(void);
void handleLevelListScrollDown(void);
void handleRankingListScrollUp(void);
void handleRankingListScrollDown(void);
void handleLevelCreditsClick(void);
void handleGfxTutorOptionClick(void);
void handleSkipLevelOptionClick(void);
void handleFloppyDiskButtonClick(void);
void handleDeletePlayerOptionClick(void);
void handleStatisticsOptionClick(void);
void handleControlsOptionClick(void);
void handleOkButtonClick(void);

static const uint8_t kNumberOfMainMenuButtons = 17;
static const ButtonDescriptor kMainMenuButtonDescriptors[kNumberOfMainMenuButtons] = { // located in DS:0000
    {
        5, 6,
        157, 14,
        handleNewPlayerOptionClick, // New player
    },
    {
        5, 15,
        157, 23,
        handleDeletePlayerOptionClick, // Delete player
    },
    {
        5, 24,
        157, 32,
        handleSkipLevelOptionClick, // Skip level
    },
    {
        5, 33,
        157, 41,
        handleStatisticsOptionClick, // Statistics
    },
    {
        5, 42,
        157, 50,
        handleGfxTutorOptionClick, // GFX-tutor
    },
    {
        5, 51,
        157, 59,
        handleLevelListScrollDown, //sub_4B159, // Demo
    },
    {
        5, 60,
        157, 69,
        handleControlsOptionClick, // Controls
    },
    {
        140, 90,
        155, 108,
        handleRankingListScrollUp, // Rankings arrow up
    },
    {
        140, 121,
        155, 138,
        handleRankingListScrollDown, // Rankings arrow down
    },
    {
        96, 140,
        115, 163,
        handleOkButtonClick, // Ok button
    },
    {
        83, 168,
        126, 192,
        handleFloppyDiskButtonClick, // Insert data disk according to https://supaplex.fandom.com/wiki/Main_menu
    },
    {
        11, 142,
        67, 153,
        handlePlayerListScrollUp, // Players arrow up
    },
    {
        11, 181,
        67, 192,
        handlePlayerListScrollDown, // Players arrow down
    },
    {
        11, 154,
        67, 180,
        handlePlayerListClick, // Players list area
    },
    {
        142, 142,
        306, 153,
        handleLevelListScrollUp, // Levels arrow up
    },
    {
        142, 181,
        306, 192,
        handleLevelListScrollDown, // Levels arrow down
    },
    {
        297, 37,
        312, 52,
        handleLevelCreditsClick, // Credits
    }
};

void handleOptionsExitAreaClick(void);
void handleOptionsMusicClick(void);
void handleOptionsAdlibClick(void);
void handleOptionsSoundBlasterClick(void);
void handleOptionsRolandClick(void);
void handleOptionsCombinedClick(void);
void handleOptionsStandardClick(void);
void handleOptionsSamplesClick(void);
void handleOptionsInternalClick(void);
void handleOptionsFXClick(void);
void handleOptionsKeyboardClick(void);
void handleOptionsJoystickClick(void);

static const uint8_t kNumberOfOptionsMenuButtons = 13;
static const ButtonDescriptor kOptionsMenuButtonDescriptors[kNumberOfOptionsMenuButtons] = { // located in DS:00AC
    {
        12, 13,
        107, 36,
        handleOptionsAdlibClick, // Adlib
    },
    {
        12, 49,
        107, 72,
        handleOptionsSoundBlasterClick, // Sound Blaster
    },
    {
        12, 85,
        107, 108,
        handleOptionsRolandClick, // Roland
    },
    {
        12, 121,
        107, 144,
        handleOptionsCombinedClick, // Combined
    },
    {
        132, 13,
        211, 31,
        handleOptionsInternalClick, // Internal
    },
    {
        126, 43,
        169, 54,
        handleOptionsStandardClick, // Standard
    },
    {
        174, 43,
        217, 54,
        handleOptionsSamplesClick, // Samples
    },
    {
        132, 86,
        175, 120,
        handleOptionsMusicClick, // Music
    },
    {
        134, 132,
        168, 152,
        handleOptionsFXClick, // FX
    },
    {
        201, 80,
        221, 154,
        handleOptionsKeyboardClick, // Keyboard
    },
    {
        233, 80,
        252, 154,
        handleOptionsJoystickClick, // Joystick
    },
    {
        0, 181,
        319, 199,
        handleOptionsExitAreaClick, // Exit (bottom)
    },
    {
        284, 0,
        319, 180,
        handleOptionsExitAreaClick, // Exit (right)
    },
};

enum ButtonBorderLineType {
    ButtonBorderLineTypeHorizontal = 0, // from left to right
    ButtonBorderLineTypeVertical = 1, // from bottom to top
    ButtonBorderLineTypeBottomLeftToTopRightDiagonal = 2,
    ButtonBorderLineTypeTopLeftToBottomRightDiagonal = 3,
};

typedef struct {
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint16_t length;
} ButtonBorderLineDescriptor;

typedef struct {
    ButtonBorderLineDescriptor lines[15];
    uint8_t numberOfLines;
} ButtonBorderDescriptor;

static const int kNumberOfOptionsMenuBorders = 20;
static const ButtonBorderDescriptor kOptionsMenuBorders[kNumberOfOptionsMenuBorders] = {
    { // 0: 130
        {
            { ButtonBorderLineTypeVertical, 144, 81, 5 },
            { ButtonBorderLineTypeHorizontal, 127, 76, 18 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 122, 71, 5 },
            { ButtonBorderLineTypeVertical, 122, 70, 69 },
            { ButtonBorderLineTypeHorizontal, 76, 2, 46 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 70, 8, 7 },
        },
        6
    },
    { // 1: 161
        {
            { ButtonBorderLineTypeHorizontal, 103, 39, 16 },
            { ButtonBorderLineTypeVertical, 118, 77, 38 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 119, 78, 6 },
            { ButtonBorderLineTypeVertical, 124, 85, 2 },
            { ButtonBorderLineTypeVertical, 124, 112, 8 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 121, 115, 3 },
            { ButtonBorderLineTypeVertical, 120, 117, 2 },
            { ButtonBorderLineTypeHorizontal, 121, 126, 24 },
            { ButtonBorderLineTypeVertical, 144, 127, 2 },
        },
        9
    },
    { // 2: 1A7
        {
            { ButtonBorderLineTypeVertical, 94, 44, 3 },
            { ButtonBorderLineTypeHorizontal, 94, 42, 19 },
            { ButtonBorderLineTypeVertical, 113, 81, 40 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 114, 82, 14 },
        },
        4
    },
    { // 3: 1CA
        {
            { ButtonBorderLineTypeVertical, 94, 78, 2 },
            { ButtonBorderLineTypeHorizontal, 95, 78, 11 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 106, 79, 9 },
            { ButtonBorderLineTypeVertical, 115, 129, 42 },
            { ButtonBorderLineTypeHorizontal, 116, 129, 20 },
        },
        5
    },
    { // 4: 01F4
        {
            { ButtonBorderLineTypeVertical, 88, 115, 3 },
            { ButtonBorderLineTypeHorizontal, 89, 115, 23 },
            { ButtonBorderLineTypeVertical, 112, 115, 3 },
            { ButtonBorderLineTypeVertical, 119, 94, 2 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 120, 93, 8 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 120, 94, 8 },
        },
        6
    },
    { // 5: 225
        {
            { ButtonBorderLineTypeVertical, 22, 114, 2 },
            { ButtonBorderLineTypeHorizontal, 11, 114, 11 },
            { ButtonBorderLineTypeVertical, 10, 151, 38 },
            { ButtonBorderLineTypeHorizontal, 6, 151, 4 },
            { ButtonBorderLineTypeVertical, 5, 156, 6 },
            { ButtonBorderLineTypeVertical, 14, 155, 3 },
            { ButtonBorderLineTypeHorizontal, 15, 153, 6 },
            { ButtonBorderLineTypeVertical, 21, 160, 8 },
            { ButtonBorderLineTypeHorizontal, 22, 160, 20 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 42, 159, 9 },
            { ButtonBorderLineTypeHorizontal, 51, 151, 74 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 125, 152, 10 },
            { ButtonBorderLineTypeHorizontal, 135, 161, 9 },
            { ButtonBorderLineTypeVertical, 144, 161, 5 },
        },
        14
    },
    { // 6: 28E
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 34, 113, 5 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 70, 116, 3 },
            { ButtonBorderLineTypeVertical, 73, 113, 5 },
            { ButtonBorderLineTypeVertical, 73, 84, 7 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 72, 76, 2 },
            { ButtonBorderLineTypeHorizontal, 9, 111, 5 },
            { ButtonBorderLineTypeHorizontal, 9, 112, 6 },
            { ButtonBorderLineTypeVertical, 7, 112, 68 },
            { ButtonBorderLineTypeVertical, 8, 112, 68 },
            { ButtonBorderLineTypeHorizontal, 9, 45, 6 },
            { ButtonBorderLineTypeHorizontal, 9, 46, 5 },
        },
        11
    },
    { // 7: 2E2
        {
            { ButtonBorderLineTypeVertical, 138, 37, 2 },
            { ButtonBorderLineTypeHorizontal, 125, 37, 13 },
            { ButtonBorderLineTypeVertical, 124, 67, 31 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 125, 68, 6 },
            { ButtonBorderLineTypeHorizontal, 131, 73, 19 },
            { ButtonBorderLineTypeVertical, 150, 81, 9 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 151, 36, 4 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 182, 39, 4 },
            { ButtonBorderLineTypeVertical, 192, 38, 3 },
        },
        9
    },
    { // 8: 328
        {
            { ButtonBorderLineTypeVertical, 156, 65, 7 },
            { ButtonBorderLineTypeVertical, 156, 69, 2 },
            { ButtonBorderLineTypeVertical, 157, 69, 2 },
            { ButtonBorderLineTypeHorizontal, 182, 71, 2 },
            { ButtonBorderLineTypeVertical, 183, 85, 14 },
            { ButtonBorderLineTypeVertical, 183, 102, 7 },
            { ButtonBorderLineTypeVertical, 183, 124, 13 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 168, 139, 15 },
            { ButtonBorderLineTypeHorizontal, 165, 139, 3 },
        },
        9
    },
    { // 9: 36E
        {
            { ButtonBorderLineTypeVertical, 180, 65, 7 },
            { ButtonBorderLineTypeVertical, 180, 69, 2 },
            { ButtonBorderLineTypeVertical, 181, 69, 2 },
            { ButtonBorderLineTypeHorizontal, 182, 71, 2 },
            { ButtonBorderLineTypeVertical, 183, 85, 14 },
            { ButtonBorderLineTypeVertical, 183, 102, 7 },
            { ButtonBorderLineTypeVertical, 183, 124, 13 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 168, 139, 15 },
            { ButtonBorderLineTypeHorizontal, 165, 139, 3 },
        },
        9
    },
    { // 10: 3B4
        {
            { ButtonBorderLineTypeHorizontal, 180, 90, 6 },
            { ButtonBorderLineTypeVertical, 186, 90, 24 },
            { ButtonBorderLineTypeHorizontal, 187, 67, 38 },
            { ButtonBorderLineTypeVertical, 225, 67, 33 },
            { ButtonBorderLineTypeHorizontal, 226, 35, 33 },
        },
        5
    },
    { // 11: 3DE
        {
            { ButtonBorderLineTypeVertical, 150, 160, 4 },
            { ButtonBorderLineTypeHorizontal, 150, 161, 31 },
            { ButtonBorderLineTypeVertical, 181, 161, 29 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 181, 133, 8 },
            { ButtonBorderLineTypeVertical, 189, 125, 22 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 189, 103, 4 },
            { ButtonBorderLineTypeVertical, 192, 99, 25 },
            { ButtonBorderLineTypeHorizontal, 192, 74, 51 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 243, 74, 17 },
        },
        9
    },
    { // 12: 424
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 126, 10 },
        },
        1
    },
    { // 13: 432
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 131, 10 },
        },
        1
    },
    { // 14: 440
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 136, 10 },
        },
        1
    },
    { // 15: 44E
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 141, 10 },
        },
        1
    },
    { // 16: 45C
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 146, 10 },
        },
        1
    },
    { // 17: 46A
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 151, 10 },
        },
        1
    },
    { // 18: 478
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 116, 4 },
            { ButtonBorderLineTypeHorizontal, 261, 119, 4 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 111, 2 },
            { ButtonBorderLineTypeHorizontal, 259, 112, 7 },
            { ButtonBorderLineTypeHorizontal, 268, 113, 2 },
            { ButtonBorderLineTypeVertical, 272, 114, 2 },
            { ButtonBorderLineTypeVertical, 273, 114, 2 },
            { ButtonBorderLineTypeHorizontal, 280, 113, 7 },
            { ButtonBorderLineTypeHorizontal, 280, 114, 7 },
            { ButtonBorderLineTypeHorizontal, 283, 119, 3 },
            { ButtonBorderLineTypeHorizontal, 292, 119, 2 },
            { ButtonBorderLineTypeVertical, 293, 131, 12 },
            { ButtonBorderLineTypeHorizontal, 289, 131, 4 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 283, 126, 4 },
        },
        14
    },
    { // 19: 04E1
        {
            { ButtonBorderLineTypeVertical, 159, 180, 14 },
            { ButtonBorderLineTypeHorizontal, 160, 167, 28 },
            { ButtonBorderLineTypeVertical, 187, 166, 8 },
            { ButtonBorderLineTypeHorizontal, 187, 149, 10 },
        },
        4
    },
};

static const int kNumberOfMainMenuButtonBorders = 12;
static const ButtonBorderDescriptor kMainMenuButtonBorders[kNumberOfMainMenuButtonBorders] = { // starts on 0x504? or before?
    // Player List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 11, 152, 11 },
            { ButtonBorderLineTypeHorizontal, 12, 142, 56 },
        },
        2
    },
    // Player List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 12, 153, 56 },
            { ButtonBorderLineTypeVertical, 67, 153, 11 },
        },
        2
    },
    // Player List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 11, 191, 11 },
            { ButtonBorderLineTypeHorizontal, 12, 181, 56 },
        },
        2
    },
    // Player List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 12, 192, 56 },
            { ButtonBorderLineTypeVertical, 67, 192, 11 },
        },
        2
    },
    // Ranking List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 141, 105, 16 },
            { ButtonBorderLineTypeHorizontal, 142, 90, 13 },
        },
        2
    },
    // Ranking List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 141, 106, 14 },
            { ButtonBorderLineTypeVertical, 154, 105, 15 },
        },
        2
    },
    // Ranking List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 141, 135, 16 },
            { ButtonBorderLineTypeHorizontal, 142, 120, 13 },
        },
        2
    },
    // Ranking List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 141, 136, 14 },
            { ButtonBorderLineTypeVertical, 154, 135, 15 },
        },
        2
    },
    // Level List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 142, 152, 11 },
            { ButtonBorderLineTypeHorizontal, 143, 142, 163 },
        },
        2
    },
    // Level List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 143, 153, 164 },
            { ButtonBorderLineTypeVertical, 306, 152, 11 },
        },
        2
    },
    // Level List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 142, 191, 11 },
            { ButtonBorderLineTypeHorizontal, 143, 181, 164 },
        },
        2
    },
    // Level List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 142, 192, 164 },
            { ButtonBorderLineTypeVertical, 306, 192, 12 },
        },
        2
    },
};

ColorPalette gCurrentPalette;

// Game palettes:
// - 0: level credits
// - 1: main menu
// - 2: ???
// - 3: ???
//
ColorPalette gPalettes[kNumberOfPalettes];
ColorPalette gBlackPalette = {
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
};

ColorPaletteData gTitlePaletteData = {
    0x02, 0x03, 0x05, 0x00, 0x0D, 0x0A, 0x04, 0x0C, 0x02, 0x06, 0x06, 0x02, 0x03, 0x09, 0x09, 0x03,
    0x0B, 0x08, 0x03, 0x06, 0x02, 0x07, 0x07, 0x0A, 0x08, 0x06, 0x0D, 0x09, 0x06, 0x04, 0x0B, 0x01,
    0x09, 0x01, 0x00, 0x04, 0x0B, 0x01, 0x00, 0x04, 0x0D, 0x01, 0x00, 0x0C, 0x0F, 0x01, 0x00, 0x0C,
    0x0F, 0x06, 0x04, 0x0C, 0x02, 0x05, 0x06, 0x08, 0x0F, 0x0C, 0x06, 0x0E, 0x0C, 0x0C, 0x0D, 0x0E,
};

ColorPaletteData gTitle1PaletteData = {
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
    0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
    0x08, 0x08, 0x08, 0x08, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08,
};

ColorPaletteData gTitle2PaletteData = {
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x06, 0x06, 0x06, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
    0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
    0x01, 0x03, 0x07, 0x00, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
    0x00, 0x02, 0x0A, 0x01, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x07,
};

SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;
SDL_Surface *gTextureSurface = NULL;

static const uint16_t kFixedBitmapWidth = 640;
static const uint16_t kFixedBitmapHeight = 16;
uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];

static const uint16_t kMovingBitmapWidth = 320;
static const uint16_t kMovingBitmapHeight = 462;
uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];

static const uint16_t kSoundBufferSize = 40 * 1024; // 40KB
uint8_t gSoundBuffer1[kSoundBufferSize];
uint8_t gSoundBuffer2[kSoundBufferSize];

// registers to prevent compiler errors
uint8_t cf;
uint8_t ah, al, bh, bl, ch, cl, dh, dl;
uint16_t ax, bx, cx, dx, ds, cs, es, bp, sp, di, si;

void setPalette(ColorPalette palette);
void fadeToPalette(ColorPalette palette);
void readTitleDatAndGraphics(void);
void videoloop(void);
void loopForVSync(void);
void convertPaletteDataToPalette(ColorPaletteData paletteData, ColorPalette outPalette);
void loadScreen2(void);
void readEverything(void);
void exitWithError(const char *format, ...);
void readMenuDat(void);
void drawSpeedFixTitleAndVersion(void);
void openCreditsBlock(void);
void drawSpeedFixCredits(void);
void readConfig(void);
void readSound(char *filename, size_t size);
void readSound2(char *filename, size_t size);
void activateAdlibSound(void);
void activateSoundBlasterSound(void);
void activateRolandSound(void);
void activateCombinedSound(void);
void activateInternalStandardSound(void);
void activateInternalSamplesSound(void);
void sub_4921B(void);
void enableFloppy(void);
void prepareSomeKindOfLevelIdentifier(void);
void runMainMenu(void);
void convertNumberTo3DigitPaddedString(uint8_t number, char numberString[3], char useSpacesForPadding);
void sound1(void);
void sound2(void);
void sound3(void);
void sound4(void);
void sound11(void);
void savePlayerListData(void);
void saveHallOfFameData(void);
void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus);
void drawMainMenuButtonBorders(void);
void drawMainMenuButtonBorder(ButtonBorderDescriptor border, uint8_t color);
void waitForJoystickKey(void); // sub_49FED
void getTime(void);
void checkVideo(void);
void initializeFadePalette(void);
void initializeMouse(void);
void setint8(void);
void loadMurphySprites(void);
void prepareLevelDataForCurrentPlayer(void);
void drawPlayerList(void);
void drawLevelList(void);
void saveLastMouseAreaBitmap(void);
void restoreLastMouseAreaBitmap(void);
void drawMouseCursor(void);
void drawRankings(void);
void drawTextWithChars6FontWithOpaqueBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars6FontWithTransparentBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8Font_method1(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text);
void sub_48E59(void);
void waitForKeyMouseOrJoystick(void);
void drawMenuTitleAndDemoLevelResult(void);
void scrollRightToNewScreen(void);
void scrollLeftToMainMenu(void);
void drawMenuBackground(void);
void convertNumberTo3DigitStringWithPadding0(uint8_t number, char numberString[3]);
void changePlayerCurrentLevelState(void);
void updateHallOfFameEntries(void);
void drawOptionsBackground(uint8_t *dest);
void drawBackBackground(void);
void drawGfxTutorBackground(uint8_t *dest);
void drawFullScreenBitmap(uint8_t *bitmapData, uint8_t *dest);
void drawSoundTypeOptionsSelection(uint8_t *destBuffer);
void dimOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer);
void drawOptionsMenuLine(ButtonBorderDescriptor border, uint8_t color, uint8_t *destBuffer);
void highlightOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer);
void drawAudioOptionsSelection(uint8_t *destBuffer);
void drawInputOptionsSelection(uint8_t *destBuffer);
void updateOptionsMenuState(uint8_t *destBuffer);
void sub_4BF4A(uint8_t number);
void readLevels(void);
void sub_48A20(void);
void sub_4D464(void);
void sub_48F6D(void);
void sub_501C0(void);
void sub_4A2E6(void);
void sub_4A3BB(void);
void sub_4FD21(void);
void sub_4FDFD(void);
void sub_4A2E6(void);
void sub_4A3BB(void);
void findMurphy(void);
void drawGamePanelText(void);
void sub_5024B(void);
void sub_4A291(void);
void sub_4F200(void);
void runLevel(void);
void slideDownGameDash(void);

static const int kWindowWidth = kScreenWidth * 4;
static const int kWindowHeight = kScreenHeight * 4;

//         public start
int main(int argc, const char * argv[])
{
    gWindow = SDL_CreateWindow("Supaplex",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               kWindowWidth,
                               kWindowHeight,
                               0);
    if (gWindow == NULL)
    {
      SDL_Log("Could not create a window: %s", SDL_GetError());
      return -1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, 0);

    gTexture = SDL_CreateTexture(gRenderer,
                                             SDL_PIXELFORMAT_ARGB32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             kScreenWidth, kScreenHeight);

    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_ARGB32);

    gScreenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);
    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    // Get an event
    SDL_Event event;
    SDL_PollEvent(&event);

/*
//; FUNCTION CHUNK AT 027F SIZE 00000217 BYTES

        // mov dx, seg data
        // mov ds, dx
        // assume ds:data

programStart:               //; CODE XREF: runMainMenu+28Fp
                    //; DATA XREF: data:000Ao
        //;db 26h, 8Ah, 0Eh, 80h, 00h
        cl = *(es:80h); // 0x80 -> Number of bytes on command-line (https://en.wikipedia.org/wiki/Program_Segment_Prefix)
        cbCommandLine = cl
        if (cl >= 2)
        {
            goto hasCommandLine
        }
        goto doesNotHaveCommandLine
//// ; ---------------------------------------------------------------------------

hasCommandLine:             //; CODE XREF: start+11j
        //cld
        ch = 0; // cx = number of command line bytes, this cleans cx MSBs
        di = 0x81; // '?' start of command line args in PSP (https://en.wikipedia.org/wiki/Program_Segment_Prefix)
        push(di);
        push(cx);

readAndProcessCommandLine:
        si = &commandLine

// Copy char by char the command line into commandLine
copyNextCmdLineByte:            //; CODE XREF: start+28j
        al = es:[di];
        *si = al;
        si++;
        di++;
        cx--;
        if (cx > 0)
        {
            goto copyNextCmdLineByte
        }
        pop(cx) // recovers original value (number of command line bytes)
        pop(di)
        di = ds
        es = di
        // assume es:data
        di = &commandLine
        push(di)
        push(cx) // saves number of cmd line bytes again

processCommandLine:         //; CODE XREF: start+F7j
        if ((di = strchr(di, ':')) == NULL)
        {
            goto nohascolon;
        }
        else
        {
            goto hascolon;
        }
//// ; ---------------------------------------------------------------------------

hascolon:                //; CODE XREF: start+39j
        al = 0x20; // ' ' (space)
        si = &demoFileName;

// This copies the argument to demoFileName, stops when it finds a space or the end of the command line
copyArgument:              //; CODE XREF: start+52j
        if (cx == 0) // check if the : was found in the last byte of the command line (cx was used to iterate through it)
        {
            goto finishArgumentCopy;
        }
        ah = es:[di];
        if (ah == al)
        {
            goto finishArgumentCopy;
        }
        *si = ah;
        si++;
        di++;
        cx--;
        if (cx > 0)
        {
            goto copyArgument;
        }

finishArgumentCopy:              //; CODE XREF: start+45j start+4Cj
        al = 0;
        *si = al; // Adds \0 at the end?
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        al = 0x3A; // ':'
        di = strchr(di, ':'); //repne scasb
        di--;
        cx++;
        al = 0x20; // ' ' (espacio)

// No idea why (yet) but this replaces the argument that was just copied with spaces :shrug:
// Ok I just learned why :joy: It's removing the entire argument because after processing it, the app will iterate
// repeat the process in the whole command line text again, looking for `:` and so on, so this chunk of code
// replaces the whole `:someArgument` text with spaces in the original command line text to make sure it doesn't
// interfere parsing the rest of the arguments.
removeArgumentByteFromCommandLine:              //; CODE XREF: start+6Aj
        if (es:[di] == al)
        {
            goto openDemoFile;
        }
        es:[di] = al; // stosb
        cx--;
        if (cx > 0)
        {
            goto removeArgumentByteFromCommandLine;
        }

openDemoFile:              //; CODE XREF: start+67j
        ax = 0x3D00;
        dx = &demoFileName;
        FILE *file = fopen(demoFileName, 'r');
        // int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
        //             ; DS:DX -> ASCIZ filename
        //             ; AX = action -> 0x3D - open file
        //             ; AL = access mode -> 0 - read
        if (file == NULL)
        {
            goto errorReadingDemoFile;
        }
        bx = file;
        push(bx);

loc_46C99:
        ax = 0x4202;
        cx = 0;
        dx = cx;
        int result = fseek(file, 0, SEEK_END);
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: 0x02 -> offset from end of file
        //             ; dx:ax is the new seek position
        int fileLength = ftell(file);
        pop(bx);
        pushf();
        if (result < 0) // jb  short errorSeekingArgumentFile
        {
            goto errorSeekingArgumentFile
        }
        
        if (fileLength > 0xFFFF) // checks if dx != 0 (so fileLength is > 0xFFFF);
        {
            goto errorSeekingArgumentFile
        }

        if (fileLength < levelDataLength)
        {
            goto loc_46CB7;
        }

errorSeekingArgumentFile:              //; CODE XREF: start+84j start+88j
        push(ax);
        push(dx);
        ah = 0x3E;
        fclose(file);
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        pop(dx);
        pop(ax);

loc_46CB7:              //; CODE XREF: start:loc_46CADj
        //db 8Dh, 36h, 65h, 9Fh
        // si = **aFileDemo; //;lea si, [aFileDemo] ; "!! File >> Demo: "
        
        word_599DA = 0
        popf();
        
        if (result < 0) // jb  short errorReadingDemoFile
        {
            goto errorReadingDemoFile
        }
        if (fileLength > 0xFFFF) // checks if dx != 0 (so fileLength is > 0xFFFF);
        {
            goto lookForAtSignInCommandLine;
        }
        if (fileLength > 0xC60A) // 50968 no idea what this number is
        {
            goto lookForAtSignInCommandLine;
        }
        si = *aFileLevel; // lea si, aFileLevel    ; "!! File < Level: "

        if (fileLength >= levelDataLength) // all demo files are greater than a level (1536 bytes)
        {
            goto loc_46CF4;
        }
        int success = fileReadUnk1(file, fileLength);
        word_599DA = fileLength;
        pushf();
        ah = 0x3E;
        fclose(file);
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        popf();
        if (success)
        {
            goto loc_46CF6; // jnb short loc_46CF6  // the flag CF to check the jnb comes from the fileReadUnk1 result (CF=0 success, CF=1 error);
        }

lookForAtSignInCommandLine:              //; CODE XREF: start+A6j start+ABj
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        al = 0x40; // '@'
        if ((di = strchr(di, '@')) == NULL) // looks for @ in the rest of the command line for some reason
        {
            goto errorReadingDemoFile;
        }
        
        conprintln(); // print some error message?
        goto immediateexit;
// ; ---------------------------------------------------------------------------

loc_46CF4:              //; CODE XREF: start+B4j
        if (ax == levelDataLength) // all demo files are greater than a level (1536 bytes)
        {
            goto loc_46CFB;
        }

loc_46CF6:              //; CODE XREF: start+C2j
        fileIsDemo = 1;

loc_46CFB:              //; CODE XREF: start:loc_46CF4j
        if (*demoFileName == 0)
        {
            goto errorReadingDemoFile;
        }
        
        byte_599D4 = 2;
        goto loc_46D13;
// ; ---------------------------------------------------------------------------

errorReadingDemoFile:              //; CODE XREF: start+74j start+A2j ...
        *demoFileName = 0;
        byte_599D4 = 0;

loc_46D13:              //; CODE XREF: start+E7j
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        goto processCommandLine;
// ; ---------------------------------------------------------------------------

nohascolon:               ; CODE XREF: start+3Bj
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        al = '@';
        int hasAt = strch(some_string, '@'); // repne scasb
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        if (hasAt == NULL)
        {
            goto runSpFile;
        }
        //; When @ is specd we need a valid file
        if (demoFileName != 0)
        {
            goto demoFileNotMissing;
        }
        si = *a@ErrorBadOrMis; // "\"@\"-ERROR: Bad or missing \":filename"...
        conprint();
        goto immediateexit;
// ; ---------------------------------------------------------------------------

demoFileNotMissing:              //; CODE XREF: start+10Bj
        if (fileIsDemo == 1)
        {
            goto spHasAtAndDemo;
        }
        si = *aSpWithoutDemo; //"SP without demo: "
        conprintln();
        goto immediateexit;
// ; ---------------------------------------------------------------------------

spHasAtAndDemo:              //; CODE XREF: start+11Cj
        fastMode = 1;
        speed1 = 0

runSpFile:              //; CODE XREF: start+104j
        push(di);
        push(cx);
        al = 0x41h; // 'A'
        goto loc_46D5A;
// ; ---------------------------------------------------------------------------

loc_46D58:              ; CODE XREF: start+14Fj
        al = 0x61; // 'a'

loc_46D5A:              ; CODE XREF: start+136j
        bx = *word_59B60;

// This weird loop seems to go from A to Z (finishing at "[") and then from
// a to z (finishing at "{") looking for those characters in the command line.
// if they're present, they're copied to the correspondent position of bx, which starts
// at 0x59B60 per the line above. That + 26 uppercase letters + 26 lowercase letters means
// at 0x59B60 starts an array of 52 bytes that ends at 0x59B94??? WTF
loc_46D5E:              //; CODE XREF: start+14Dj start+153j
        int result = strchr(di, al);
        if (result == NULL)
        {
            goto loc_46D64;
        }
        [bx] = al; // or [bx], al writes al in [bx] (if [bx] is 0...)

loc_46D64:              //; CODE XREF: start+140j
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        bx++;
        al++;
        if (al < 0x5B) // '['
        {
            goto loc_46D5E;
        }
        else if (al == 0x5B) // '['
        {
            goto loc_46D58;
        }
        
        if (al < 0x7B) // '{'
        {
            goto loc_46D5E;
        }
        
        if (byte_59B63 == 0) // this location is where the D (uppercase) would go (it's &word_59B60 + 3). WTF
        {
            goto loc_46D82;
        }
        word_51970 = 1;

loc_46D82:              //; CODE XREF: start+15Aj
        al = 0x21; // '!'
        int result = strchr(di, '!');
        if (result == NULL)
        {
            goto loc_46DBF;
        }
        
        if (cx < 1) // if '!' was found in the last character?
        {
            goto loc_46DBF;
        }
        ax = es:[di];
        if (al < 0x30) // '0'
        {
            goto loc_46DBF;
        }
        
        if (al > 0x39) // '9'
        {
            goto loc_46DBF;
        }

        if (ah < 0x30) // '0'
        {
            goto loc_46DA2;
        }

        if (ah <0 0x39) // '9'
        {
            goto modifysetfns;
        }

loc_46DA2:              //; CODE XREF: start+17Bj
        ah = al;
        al = 0x30; // '0'

modifysetfns:              //; CODE XREF: start+180j
        // This changes the suffix of the files with whatever is in AX? like LEVELS.DAT with LEVELS.D00 or LEVEL.LST with LEVEL.L10 ??
        aLevels_dat_0[8] = ax; // "AT"
        aLevel_lst[7] = ax; // "ST"
        aDemo0_bin[7] = ax; // "IN"
        aPlayer_lst[8] = ax; // "ST"
        aHallfame_lst[0Ah] = ax; // "ST"
        if (*dword_59B76 != 0) // cmp byte ptr dword_59B76, 0
        {
            goto loc_46DBF;
        }
        
        aSavegame_sav[0Ah] = ax; //"AV"

loc_46DBF:              //; CODE XREF: start+166j start+16Bj ...
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x23; //'#'
        
        if (strchr(something, '#') == NULL)
        {
            goto loc_46E18;
        }
        byte_59B85 = 0x0FF;
        word_58DAC = 0x4628;
        word_58DAE = 0x524F;
        word_58DB0 = 0x4543;
        word_58DB2 = 0x2944;
        di = 0x8AA8;
        cx = 0x6F; // 'o'
        al = 2;
        push(es);
        push(ds);
        pop(es);
        rep stosb
        al = 0x20; // ' '
        cx = 8;
        di = 0x8B1C; //
        rep stosb
        cx = 8;
        di = 0x8B9C;
        rep stosb
        cx = 0x16;
        di = 0x8853;
        rep stosb
        cx = 0x16;
        di = 0x886A;
        rep stosb
        pop(es);
        // assume es:nothing
        goto loc_46E39;
//// ; ---------------------------------------------------------------------------

loc_46E18:              //; CODE XREF: start+1A7j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x26; // '&'
        
        if (strchr(something, '&') == NULL)
        {
            goto loc_46E39;
        }
        atoi(something) // reset ax
        if (al >= 1)
        {
            goto loc_46E2B;
        }
        al++;

loc_46E2B:              //; CODE XREF: start+207j
        if (al <= 0x14) // In ascii this is DC4 - Device Control 4??
        {
            goto loc_46E31;
        }
        al = 0x14;

loc_46E31:              //; CODE XREF: start+20Dj
        al--;
        gCurrentPlayerIndex = al;
        byte_58D46 = al;

loc_46E39:              //; CODE XREF: start+1F6j start+200j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x2A; // '*'
        if (strchr(something, '*') == NULL)
        {
            goto loc_46E59;
        }
        atoi(whatever)        // ; reset ax
        if (al <= 0x0A) // '\n'
        {
            goto loc_46E4C;
        }
        al = 0x0A; // '\n'

loc_46E4C:              //; CODE XREF: start+228j
        ah =  0x0A;
        ah -= al;
        if (ah < 0)
        {
            goto loc_46E59;
        }
        
        ah |= 0x0C0;
        speed3 = ah;

loc_46E59:              //; CODE XREF: start+221j start+230j
        pop(cx);
        pop(di);
        al = 0x2F; // '/'
        if (strchr(something, '/') == NULL)
        {
            goto doesNotHaveCommandLine;
        }
        byte_59B84 = 0x0FF;
        atoi()        //; reset ax
        
        if (al >= 1)
        {
            goto loc_46E6F;
        }
        al++;

loc_46E6F:              //; CODE XREF: start+24Bj
        if (al <= 0x6F) // 'o'
        {
            goto loc_46E75;
        }
        al = 0x6F; // 'o'

loc_46E75:              //; CODE XREF: start+251j
        byte_59B84 = al;

 */

//doesNotHaveCommandLine:         //; CODE XREF: start+13j start+23Fj ...
    getTime();
    checkVideo();
    if (byte_59B64 != 0)
    {
        videoStatusUnk = 2;
    }

//leaveVideoStatus:           //; CODE XREF: start+28Aj
//        ax = 0xA000;
//        es = ax;
//         assume es:nothing
    initializeFadePalette(); // 01ED:026F
    initializeMouse();
//    setint8(); // Timer or sound speaker?
//    setint24(); // Fatal error handler?
//    setint9(); // Keyboard
//    initializeSound();
    if (fastMode == 0)
    {
//    initializeVideo2();
//    initializeVideo3();

        setPalette(gBlackPalette);
        readTitleDatAndGraphics();
        ColorPalette titleDatPalette; // si = 0x5F15;
        convertPaletteDataToPalette(gTitlePaletteData, titleDatPalette);
        fadeToPalette(titleDatPalette);
    }

//isFastMode:              //; CODE XREF: start+2ADj
    loadMurphySprites();
//    initializeVideo3();
    // Conditions to whether show
    al = byte_59B6B;
    al |= byte_59B84;
    al |= byte_599D4;
    al |= fastMode;
    if (al != 0)
    {
        readEverything();
    }
    else
    {
//openingSequence:
        loadScreen2();    // 01ED:02B9
        readEverything(); // 01ED:02BC
        drawSpeedFixTitleAndVersion(); // 01ED:02BF
        openCreditsBlock(); // credits inside the block // 01ED:02C2
        drawSpeedFixCredits();   // credits below the block (herman perk and elmer productions) // 01ED:02C5
    }

//afterOpeningSequence:              //; CODE XREF: start+2DEj
    readConfig();
    if (byte_50946 == 0)
    {
        isJoystickEnabled = 0;
    }
    else
    {
        isJoystickEnabled = 1;
    }

//loc_46F25:              //; CODE XREF: start+2FEj
    if (fastMode == 0)
    {
//isNotFastMode:              //; CODE XREF: start+30Aj
        fadeToPalette(gBlackPalette);
        word_58467 = 1;
    }

    // Can't think of a better way to handle this right now, but at this point we had a "jmp loc_46FBE" which basically
    // skipped a few lines of the loop in the first iteration. Will revisit later when I have more knowledge of the code.
    //
    uint8_t shouldSkipFirstPart = 1;

    do
    {
//loc_46F3E:              //; CODE XREF: start+428j start+444j
        if (shouldSkipFirstPart == 0)
        {
            readLevels(); // 01ED:02F7
            fadeToPalette(gBlackPalette);
            byte_5A33F = 0;
            drawPlayerList();
            sub_48A20();
            sub_4D464(); // configures the viewport for the bottom panel, which is not very useful to this re-implementation?
            sub_48F6D(); // draws the tiles?
//            sub_501C0(); // 01ED:0311
//            sub_4A2E6();
//            sub_4A3BB();
//            enableFloppy();
//            findMurphy();
//            si = 0x6015;
            fadeToPalette(gPalettes[1]); // At this point the screen fades in and shows the game
            SDL_Delay(5000); // TODO: remove
            if (isMusicEnabled == 0)
            {
                sound3();
            }

//loc_46F77:              //; CODE XREF: start+352j
            byte_5A33F = 1;
            runLevel();
            byte_599D4 = 0;
            if (word_5197A != 0)
            {
                break; // goto loc_47067;
            }

//loc_46F8E:              //; CODE XREF: start+369j
            if (fastMode == 1)
            {
//                goto doneWithDemoPlayback;
            }

//isNotFastMode2:              //; CODE XREF: start+373j
            slideDownGameDash();
            if (byte_59B71 != 0)
            {
                loadMurphySprites();
            }

//loc_46FA5:              //; CODE XREF: start+380j
            byte_5A33F = 0;
            if (word_5197A != 0)
            {
                break; // goto loc_47067;
            }

//loc_46FB4:              //; CODE XREF: start+38Fj
            if (isMusicEnabled == 0)
            {
                sound2();
            }
        }

        shouldSkipFirstPart = 0;

//loc_46FBE:              //; CODE XREF: start+30Cj start+31Bj ...
        enableFloppy(); // 01ED:0377
        prepareSomeKindOfLevelIdentifier();

        uint8_t shouldDoSomething = 1; // still no idea what this is

        if (byte_599D4 == 2)
        {
//            goto loc_46FFF; <- if byte_599D4 != 2

            byte_599D4 = 1;
            if (fileIsDemo == 1)
            {
//              ax = 0;
//              demoSomething();
            }
            else
            {
//loc_46FDF:              //; CODE XREF: start+3B5j
                byte_510DE = 0;
            }

//loc_46FE4:              //; CODE XREF: start+3BDj
            ax = 1;
            byte_510B3 = 0;
            byte_5A2F9 = 1;
            a00s0010_sp[3] = 0x2D; // '-' ; "001$0.SP"
            a00s0010_sp[4] = 0x2D; // "01$0.SP"
            a00s0010_sp[5] = 0x2D; // "1$0.SP"
//          push(ax);
//          goto loc_4701A;
        }
        else
        {
//loc_46FFF:              //; CODE XREF: start+3A9j
            al = byte_59B84;
            byte_59B84 = 0;
            byte_510DE = 0;
            al++;
            if (al != 0)
            {
                al--;
                if (al != 0)
                {
//                  ah = 0;
//                  push(ax);
                    sub_4BF4A(al);
                }
                else
                {
                    shouldDoSomething = 0;
                }
            }
            else
            {
                shouldDoSomething = 0;
            }
        }

        if (shouldDoSomething)
        {
//loc_4701A:              //; CODE XREF: start+3DDj start+433j
            byte_5A33F = 1;
            byte_51ABE = 1;
            prepareLevelDataForCurrentPlayer();
            drawPlayerList();
            word_58467 = 0;
            sound2();
//          pop(ax);
//          gCurrentSelectedLevelIndex = ax;
            restoreLastMouseAreaBitmap();
            drawLevelList();
            word_5196C = 0;
            byte_5A19B = 0;
            continue;
        }

//loc_4704B:              //; CODE XREF: start+3EEj start+3F2j
        ax = 1;
        if (byte_59B6B != 0)
        {
//            goto loc_4701A;
        }
        byte_5A2F9 = 0;
        runMainMenu();
    }
    while (word_5197A == 0);

//loc_47067:              //; CODE XREF: start+36Bj start+391j ...
        fadeToPalette(gBlackPalette); // 0x60D5
/*
doneWithDemoPlayback:           //; CODE XREF: start+375j
        resetint9();
        resetVideoMode();
        if (fastMode != 1)
        {
            goto isNotFastMode3;
        }

        if (byte_5A19C == 0)
        {
            goto loc_47094;
        }

        if (byte_5A19B == 0)
        {
            goto loc_4708E;
        }

        si = *aDemoSuccessful; // "Demo successful: "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_4708E:              //; CODE XREF: start+466j
        si = *aDemoFailed; //"Demo failed:     "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_47094:              //; CODE XREF: start+45Fj
        if (byte_5A19B == 0)
        {
            goto loc_470A1;
        }
        si = *a@ErrorLevel?Su; // "\"@\"-ERROR: Level(?) successful: "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_470A1:              //; CODE XREF: start+479j
        si = *a@ErrorLevel?Fa; // "\"@\"-ERROR: Level(?) failed:     "

printMessageAfterward:          //; CODE XREF: start+46Cj start+472j ...
        conprintln();

isNotFastMode3:              //; CODE XREF: start+458j
        resetint24();
        resetint8();
        soundShutdown?();

immediateexit:              //; CODE XREF: start+D1j start+114j ...
        exit(0);
//         mov ax, 4C00h
//         int 21h     ; DOS - 2+ - QUIT WITH EXIT CODE (EXIT)
// ; END OF FUNCTION CHUNK FOR start   ; AL = exit code


// ; ---------------------------------------------------------------------------
// ; START OF FUNCTION CHUNK FOR loadScreen2

exit:                   //; CODE XREF: readConfig:loc_474BBj
*/

        // Tidy up
        SDL_FreeSurface(gTextureSurface);
        SDL_DestroyTexture(gTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();

        return 0;

//    //; readConfig+3Bj ...
//        push(ax);
//        resetint9();
//        resetint24();
//        soundShutdown?();
//        resetVideoMode();
//        resetint8();
//        pop(ax);
//        push(ax);
//        writeexitmessage();
//        pop(ax);
//        if (al != 0)
//        {
//            goto exitnow;
//        }
//        al--;
//
//exitnow:                //; CODE XREF: loadScreen2-7C9j
//        exit(al);  //mov ah, 4Ch
////         int 21h     ; DOS - 2+ - QUIT WITH EXIT CODE (EXIT)
//// ; END OF FUNCTION CHUNK FOR loadScreen2 ; AL = exit code
}

/*
//; =============== S U B R O U T I N E =======================================


// Return value: carry flag = 0 on success, 1 on error
void fileReadUnk1(FILE *file, long fileLength) //    proc near       ; CODE XREF: start+B6p readDemoFiles+81p
{
        push(bx);
        bx = word_599DC; // load array from word_599DC
        cx = 10;

loop_:                  //; CODE XREF: fileReadUnk1+Fj
        if (*bx == fileLength)
        {
            goto loc_47105;
        }
        bx += 2;
        cx--;
        if (cx > 0)
        {
            goto loop_;
        }
        goto bail;
// ; ---------------------------------------------------------------------------

loc_47105:              //; CODE XREF: fileReadUnk1+Aj
        pop(bx);
        push(bx);
        push(cx);
        ax = 0x4200;
        cx = 0;
        dx = cx;
        int result = fseek(file, 0, SEEK_SET);
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: offset from beginning of file
        pop(ax);
        if (result < 0)
        {
            goto bail;
        }
        pop(bx);
        push(bx);
        push(ax);
        cx = 4;
        dx = *fileLevelData; // load string from fileLevelData?
        ax = 0x3F00;
        result = fread(fileLevelData, 1, 4, file); // Read the first 4 bytes into fileLevelData
        // int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
        //             ; BX = file handle, CX = number of bytes to read
        //             ; DS:DX -> buffer
        pop(bx);
        if (result == 0)
        {
            goto bail;
        }
        bx -= 10;
        bx = -bx;
        bx = bx << 1;
        bx = bx << 1;
        si = &fileLevelData;
        ax = [bx-691Eh];
        if (ax != word_50A7A)
        {
            goto bail;
        }
        ax = [bx-6920h];
        if (ax != fileLevelData)
        {
            goto bail;
        }
        ah = 0;
        clc(); // clears carry flag
        goto done;
// ; ---------------------------------------------------------------------------

bail:                   //; CODE XREF: fileReadUnk1+11j
                    //; fileReadUnk1+20j ...
        ax = 0;
        stc(); // sets carry flag

done:                   //; CODE XREF: fileReadUnk1+57j
        pop(bx);
}


; =============== S U B R O U T I N E =======================================


crt?2       proc near       ; CODE XREF: slideDownGameDash:loc_4720Cp
        cmp byte ptr word_510C1, 0
        jz  short loc_4715C
        mov cx, 5Fh ; '_'
        jmp short loc_4715F
// ; ---------------------------------------------------------------------------

loc_4715C:              ; CODE XREF: crt?2+5j
        mov cx, 90h ; '?'

loc_4715F:              ; CODE XREF: crt?2+Aj crt?2+5Fj
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    // ; bit 8 for certain CRTC regs. Data bits:
                    // ; 0: vertical total (Reg 06)
                    // ; 1: vert disp'd enable end (Reg 12H)
                    // ; 2: vert retrace start (Reg 10H)
                    // ; 3: start vert blanking (Reg 15H)
                    // ; 4: line compare (Reg 18H)
                    // ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov bx, word_51967
        cmp bx, 4DAEh
        jbe short loc_4718E
        sub bx, 7Ah ; 'z'
        mov word_51967, bx

loc_4718E:              ; CODE XREF: crt?2+35j
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    loopForVSync
        add cx, 1
        cmp cx, 90h ; '?'
        jbe short loc_4715F
        return;
crt?2       endp


; =============== S U B R O U T I N E =======================================


crt?1       proc near       ; CODE XREF: slideDownGameDash+7p
        cmp byte ptr word_510C1, 0
        jz  short loc_471BE
        mov cx, 0B0h ; '?'
        jmp short loc_471C1
// ; ---------------------------------------------------------------------------

loc_471BE:              ; CODE XREF: crt?1+5j
        mov cx, 0C8h ; '?'

loc_471C1:              ; CODE XREF: crt?1+Aj crt?1+4Bj
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov bx, word_51967
        cmp bx, 4DAEh
        jbe short loc_471DC
        sub bx, 7Ah ; 'z'
        mov word_51967, bx

loc_471DC:              ; CODE XREF: crt?1+21j
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    loopForVSync
        add cx, 1
        cmp cx, 0C8h ; '?'
        jbe short loc_471C1
        return;
crt?1       endp
*/
void slideDownGameDash() // proc near     ; CODE XREF: start:isNotFastMode2p
{
    /*
    cmp videoStatusUnk, 2
    jnz short loc_4720C
    call    crt?1
    jmp short locret_4720F
// ; ---------------------------------------------------------------------------

loc_4720C:              ; CODE XREF: slideDownGameDash+5j
    call    crt?2

locret_4720F:               ; CODE XREF: slideDownGameDash+Aj
    return;
     */
}

void setint9()
{
    // waits for shift, ctrl, alt, system, and suspend to NOT be pressed
    // then replaces the int 9 handler with our own (int9handler) and stores the original one in originalInt9Handler
// ; waits for shift, ctrl, alt, system, and suspend to NOT be pressed
//         push    ds
//         mov ax, 0040h;
//         mov ds, ax
//         assume ds:nothing

// keysstillpressed:
//         ; test keyboard state flags at 0040:0017 (BIOS keyboard status byte: http://www.powernet.co.za/info/bios/mem/40_0017.htm)
//         test    word ptr [ds:0017h], 0F70Fh 
//         jnz short keysstillpressed 

// ; keys no longer pressed, we can continue
//         pop ds
//         assume ds:data
//         push    ds
//         push    es
//         mov ah, 35h ; '5'
//         al = 9
//         int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
//                     ; AL = interrupt number
//                     ; Return: ES:BX = value of interrupt vector
//         mov originalInt9Handler+2, es
//         mov originalInt9Handler, bx
//         mov dx, offset int9handler
//         mov ax, seg code
//         mov ds, ax
//         assume ds:code
//         mov ah, 25h ; '%'
//         al = 9
//         int 21h     ; DOS - SET INTERRUPT VECTOR
//                     ; AL = interrupt number
//                     ; DS:DX = new vector to be used for specified interrupt
//         pop es
//         assume es:nothing
//         pop ds
//         assume ds:data
//         return;
// setint9     endp
}

void resetint9() //   proc near       ; CODE XREF: start:doneWithDemoPlaybackp
// ; loadScreen2-7E0p ...
{
    /*
    cmp byte_5199A, 0
    jnz short resetint9
    cmp byte ptr word_519A7, 0
    jnz short resetint9
    cmp byte ptr word_519B3, 0
    jnz short resetint9
    cmp byte_519B5, 0
    jnz short resetint9
    cmp byte_519B7, 0
    jnz short resetint9
    cmp byte_519C2, 0
    jnz short resetint9
    cmp byte_519C3, 0
    jnz short resetint9
    cmp byte_519CF, 0
    jnz short resetint9
    cmp byte_519D1, 0
    jnz short resetint9
    push    ds
    push    es
    mov dx, originalInt9Handler
    mov ax, originalInt9Handler+2
    mov ds, ax
    mov ah, 25h ; '%'
    al = 9
    int 21h     ; DOS - SET INTERRUPT VECTOR
                ; AL = interrupt number
                ; DS:DX = new vector to be used for specified interrupt
    pop es
    pop ds
     */
}

void int9handler() // proc far        ; DATA XREF: setint9+1Fo
{
    // This alternative int9 handler seems to control the keys +, -, *, / in the numpad
    // to alter the game speed, and also the key X for something else.
//    push    ax
//    push    bx
//    push(cx);
//    push    ds
//    mov ax, seg data
//    mov ds, ax
//    in  al, 60h     ; 8042 keyboard input buffer
//    mov cl, al
//    mov bl, al
//    in  al, 61h     ; PC/XT PPI port B bits:
//                ; 0: Tmr 2 gate ??? OR 03H=spkr ON
//                ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
//                ; 3: 1=read high switches
//                ; 4: 0=enable RAM parity checking
//                ; 5: 0=enable I/O channel check
//                ; 6: 0=hold keyboard clock low
//                ; 7: 0=enable kbrd
//    or  al, 10000000b
//    out 61h, al     ; enable keyboard
//    and al, 1111111b
//    out 61h, al     ; disable keyboard
//    al = 0;
//    bh = 0;
//    bl = bl << 1;
//    cf = 1 - cf; // cmc
//    rcl al, 1
//    bl = bl >> 1;
//    bx[0x166D] = al;
    SDL_Event e;
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {
        ;
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    keyPressed = SDL_SCANCODE_UNKNOWN;

    for (SDL_Scancode scancode = 0; scancode < SDL_NUM_SCANCODES; ++scancode)
    {
        if (keys[scancode])
        {
            keyPressed = scancode;
            break;
        }
    }

    if (keyPressed != SDL_SCANCODE_UNKNOWN) //test    cl, 80h     ; think key up
    {
//storeKey:               ; CODE XREF: int9handler+2Bj
//        keyPressed = cl;
        if (speed3 >= 0)
        {
            if (keyPressed == SDL_SCANCODE_KP_MULTIPLY // Key * in the numpad, restore speed
                && speed3 >= 0)
            {
                gameSpeed = speed3;
//              push(cx);
                cl = speed2;
                cl = cl & 0xF0;
                cl = cl | gameSpeed;
                if (speed2 > cl)
                {
                    speed2 = cl;
                }
//              pop(cx);
            }
            else
            {
//checkSlash:             ; CODE XREF: int9handler+3Ej
//                ; int9handler+45j
                if (cl == SDL_SCANCODE_KP_DIVIDE) // Key / (numpad or not)
                {
                    speed1 = 0;
                    gameSpeed = 10;
                }
                else
                {
//checkPlus:              ; CODE XREF: int9handler+54j
                    if (cl == SDL_SCANCODE_KP_PLUS) // Key + in the numpad, speed up
                    {
                        speed1 = 0;
                        if (gameSpeed < 10) // 10
                        {
                            gameSpeed++;
                        }
                    }
//checkMinus:             ; CODE XREF: int9handler+65j
//                ; int9handler+71j
                    if (cl == SDL_SCANCODE_KP_MINUS) // Key - in the numpad, speed down
                    {
                        speed1 = 0;
                        if (gameSpeed != 0)
                        {
                            gameSpeed--;
//loc_47320:              ; CODE XREF: int9handler+4Fj
//                          push(cx);
                            cl = speed2;
                            cl = cl & 0xF0;
                            cl = cl | gameSpeed;
                            if (speed2 > cl)
                            {
                                speed2 = cl;
                            }
//dontUpdatespeed2:          ; CODE XREF: int9handler+9Cj
//                          pop(cx);
                        }
                    }
                }
            }
        }

    //checkX:                 ; CODE XREF: int9handler+39j
    //                ; int9handler+60j ...
        if (cl == SDL_SCANCODE_X // Key X
            && byte_519B5 != 0)
        {
            word_51974 = 1;
            word_5197A = 1;
        }
    }
//    else
//    {
//        keyPressed = 0;
//    }

//doneexit:                   ; CODE XREF: int9handler+A6j
//                ; int9handler+ADj ...
//    al = 100000b ; nonspecific EOI
//    out 20h, al     ; Interrupt controller, 8259A.
//    pop ds
//    pop(cx);
//    pop bx
//    pop ax
 }

void int8handler() // proc far        ; DATA XREF: setint8+10o
{
//    push    ds
//    push    dx
//    push    ax
//    mov ax, seg data
//    mov ds, ax
    byte_59B96++;
    if (byte_510AE != 0)
    {
        byte_510AF++;
        if (byte_510AF >= 0x32) // '2' or 50
        {
            byte_510AF = 0;
            byte_510B0++;
            if (byte_510B0 >= 0x3C) // '<' or 60
            {
                byte_510B0 = 0;
                byte_510B1++;
                if (byte_510B1 >= 0x3C) // '<' or 60
                {
                    byte_510B1 = 0;
                    byte_510B2++;
                }
            }
        }
    }

//loc_473AA:              ; CODE XREF: int8handler+11j
//                ; int8handler+1Dj ...
    if (soundEnabled != 0)
    {
        sound11();
    }

//loc_473B4:              ; CODE XREF: int8handler+4Fj
    if (byte_5988B != 0)
    {
        byte_5988B--;
        if (byte_5988B == 0)
        {
            byte_59889 = 0;
        }
    }

//loc_473C6:              ; CODE XREF: int8handler+59j
//                ; int8handler+5Fj
    if (byte_5988C != 0)
    {
        byte_5988C--;
        if (byte_5988C == 0)
        {
            byte_5988A = 0;
        }
    }

//loc_473D8:              ; CODE XREF: int8handler+6Bj
//                ; int8handler+71j
    byte_5A320++;
    byte_5A321++;
    if (byte_5A320 == 3)
    {
        byte_5A322++;
        byte_5A320 = 0;
    }
//loc_473F0:              ; CODE XREF: int8handler+85j
    else if (byte_5A321 == 0x21) // '!' or 33
    {
        byte_5A322++;
        byte_5A321 = 0;
    }
//loc_47400:              ; CODE XREF: int8handler+95j
    else if (byte_5A322 != 0)
    {
        byte_5A322--;
    //    pop ax
    //    pop dx
    //    pushf
    //    originalInt8Handler();
    //    pop ds
        return; // iret
    }
    else
    {
//loc_47414:              ; CODE XREF: int8handler+A5j
        al = 0x20; // ' ' or 32
    //    out 20h, al     ; Interrupt controller, 8259A.
    //    pop ax
    //    pop dx
    //    pop ds
        return; //iret
    }
}


void setint8() //     proc near       ; CODE XREF: start+29Cp
{
    // This replaces the current int 8 handler with our own one (int8handler), saves the previous one in originalInt8Handler
        // push    ds
        // push    es
        // mov ah, 35h ; '5'
        // al = 8
        // int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
        //             ; AL = interrupt number
        //             ; Return: ES:BX = value of interrupt vector
        // mov word ptr originalInt8Handler+2, es
        // mov word ptr originalInt8Handler, bx
        // mov dx, offset int8handler
        // mov ax, seg code
        // mov ds, ax
        // assume ds:code
        // mov ah, 25h ; '%'
        // al = 8
        // int 21h     ; DOS - SET INTERRUPT VECTOR
        //             ; AL = interrupt number
        //             ; DS:DX = new vector to be used for specified interrupt
    
        // Info about these ports: https://wiki.osdev.org/PIT
        // Port 43 is the Mode/command register. 0x36 is 00110110, which means:
        // It selects channel 0 (which port is 0x40), access mode lobyte/hibyte, mode 3 (square wave generator), 16-bit binary
//        al = 36h ; '6'
//        out 43h, al     ; Timer 8253-5 (AT: 8254.2).
        
        // No idea what this does exactly but seems to configure the timer
        // al = 38h ; '8'
        // out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        // al = 5Dh ; ']'
        // out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        // pop es
        // pop ds
        // assume ds:data
        // return;
}


void resetint8() //   proc near       ; CODE XREF: start+48Bp
//                    ; loadScreen2-7D4p
{
//    push    ds
//    push    es
//    mov dx, word ptr originalInt8Handler
//    mov ax, word ptr originalInt8Handler+2
//    mov ds, ax
//    mov ah, 25h ; '%'
//    al = 8
//    int 21h     ; DOS - SET INTERRUPT VECTOR
//                ; AL = interrupt number
//                ; DS:DX = new vector to be used for specified interrupt
//    al = 36h ; '6'
//    out 43h, al     ; Timer 8253-5 (AT: 8254.2).
//    al = 0FFh
//    out 40h, al     ; Timer 8253-5 (AT: 8254.2).
//    al = 0FFh
//    out 40h, al     ; Timer 8253-5 (AT: 8254.2).
//    pop es
//    pop ds
}

void setint24() //    proc near       ; CODE XREF: start+29Fp
{
    // This function replaces the handler of interruption 24 with our own (int24handler) and stores the original in originalInt24Handler
        // push    ds
        // push    es
        // mov ah, 35h ; '5'
        // al = 24h ; '$'
        // int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
        //             ; AL = interrupt number
        //             ; Return: ES:BX = value of interrupt vector
        // mov originalInt24Handler+2, es
        // mov originalInt24Handler, bx
        // mov dx, offset int24handler
        // mov ax, seg code
        // mov ds, ax
        // assume ds:code
        // mov ah, 25h ; '%'
        // al = 24h ; '$'
        // int 21h     ; DOS - SET INTERRUPT VECTOR
        //             ; AL = interrupt number
        //             ; DS:DX = new vector to be used for specified interrupt
        // pop es
        // pop ds
        // assume ds:data
        // return;
}

void resetint24() //  proc near       ; CODE XREF: start:isNotFastMode3p
//                    ; loadScreen2-7DDp
{
//        push    ds
//        push    es
//        mov dx, originalInt24Handler
//        mov ax, originalInt24Handler+2
//        mov ds, ax
//        mov ah, 25h ; '%'
//        al = 24h ; '$'
//        int 21h     ; DOS - SET INTERRUPT VECTOR
//                    ; AL = interrupt number
//                    ; DS:DX = new vector to be used for specified interrupt
//        pop es
//        pop ds
//        return;
}

void int24handler() //    proc far        ; DATA XREF: setint24+10o
{
//    al = 1
//    iret
}

void readConfig() //  proc near       ; CODE XREF: start:loc_46F0Fp
{
    FILE *file = fopen("SUPAPLEX.CFG", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
//loc_47551:              //; CODE XREF: readConfig+Fj
                       // ; readConfig+17j
            activateInternalStandardSound();
            isJoystickEnabled = 0;
            return;
        }
        else
        {
            exitWithError("Error opening SUPAPLEX.CFG\n");
        }
    }

//loc_474BE:             // ; CODE XREF: readConfig+8j

    uint8_t configData[kConfigDataLength];

    size_t bytes = fread(&configData, 1, sizeof(configData), file);

    if (fclose(file) != 0)
    {
        exitWithError("Error closing SUPAPLEX.CFG\n");
    }

//loc_474DF:             // ; CODE XREF: readConfig+39j
    if (bytes < sizeof(configData))
    {
        exitWithError("Error reading SUPAPLEX.CFG\n");
    }

//loc_474E5:             // ; CODE XREF: readConfig+3Fj
    uint8_t soundSetting = configData[0];

    if (soundSetting == 's')
    {
        activateInternalSamplesSound();
    }
    else if (soundSetting == 'a')
    {
        activateAdlibSound();
    }
    else if (soundSetting == 'b')
    {
        activateSoundBlasterSound();
    }
    else if (soundSetting == 'r')
    {
        activateRolandSound();
    }
    else if (soundSetting == 'c')
    {
        activateCombinedSound();
    }
    else
    {
        activateInternalStandardSound();
    }

//loc_4751D:             // ; CODE XREF: readConfig+4Fj
               // ; readConfig+59j ...

    isJoystickEnabled = 0;
    if (configData[1] == 'j')
    {
        isJoystickEnabled = 1;
        sub_4921B();
    }

//loc_47530:              //; CODE XREF: readConfig+85j
    isMusicEnabled = (configData[2] == 'm');

//loc_47540:              //; CODE XREF: readConfig+98j
    isFXEnabled = (configData[3] == 'x');
// readConfig  endp
}

void saveConfiguration() // sub_4755A      proc near               ; CODE XREF: code:loc_4CAECp
{
    FILE *file = fopen("SUPAPLEX.CFG", "w");
    if (file == NULL)
    {
        exitWithError("Error opening SUPAPLEX.CFG\n");
    }

//loc_4756A:                              ; CODE XREF: saveConfiguration+Bj
    uint8_t configData[kConfigDataLength];

    if (sndType == SoundTypeInternalSamples)
    {
        configData[0] = 's';
    }
    else if (sndType == SoundTypeInternalStandard)
    {
        configData[0] = 'i';
    }
    else if (sndType == SoundTypeAdlib)
    {
        configData[0] = 'a';
    }
    else if (sndType == SoundTypeRoland)
    {
        configData[0] = 'r';
    }
    else if (musType == SoundTypeRoland)
    {
        configData[0] = 'c';
    }
    else
    {
        configData[0] = 'b';
    }

//loc_475AF:                              ; CODE XREF: saveConfiguration+20j
//                            ; saveConfiguration+2Cj ...
    if (isJoystickEnabled == 0)
    {
        configData[1] = 'k';
    }
    else
    {
        configData[1] = 'j';
    }

//loc_475BF:                              ; CODE XREF: saveConfiguration+60j
    if (isMusicEnabled != 0)
    {
        configData[2] = 'm';
    }
    else
    {
        configData[2] = 'n';
    }

//loc_475CF:                              ; CODE XREF: saveConfiguration+70j
    if (isFXEnabled != 0)
    {
        configData[3] = 'x';
    }
    else
    {
        configData[3] = 'y';
    }

//loc_475DF:                              ; CODE XREF: saveConfiguration+80j
    size_t bytes = fwrite(configData, 1, sizeof(configData), file);
    if (bytes < sizeof(configData))
    {
        exitWithError("Error writing SUPAPLEX.CFG\n");
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing SUPAPLEX.CFG\n");
    }
}

void enableFloppy() //   proc near       ; CODE XREF: start+341p
                    // ; start:loc_46FBEp ...
{
    // mov dx, 3F2h
    // al = 1111b
    // out dx, al      ; Floppy: digital output reg bits:
    //             ; 0-1: Drive to select 0-3 (AT: bit 1 not used)
    //             ; 2:   0=reset diskette controller; 1=enable controller
    //             ; 3:   1=enable diskette DMA and interrupts
    //             ; 4-7: drive motor enable.  Set bits to turn drive ON.
    //             ;
    // return;
// enableFloppy   endp
}
/*
void readDemoFiles() //    proc near       ; CODE XREF: readEverything+12p
                  //  ; sub_4B159p ...
{
        push(es);
        ax = seg demoseg;
        es = ax;
        // assume es:demoseg
        word_510DF = 22;
        word_5A33C = 22;
        // ax = 0xFFFF;
        // di = 0;
        // cx = 0xB; // 11
        // cld(); // clear direction flag
        memset(di, 0xFF, 22); // rep stosw // fills 11 words (22 bytes) with 0xFFFF
        di += 22;
        cx = 0;

loc_47629:              //; CODE XREF: readDemoFiles+175j
        push(cx);
        word_599D8 = 0;
        if (byte_599D4 != 1)
        {
            goto loc_4763C;
        }
        dx = &demoFileName;
        goto loc_47647;
// ; ---------------------------------------------------------------------------

loc_4763C:             // ; CODE XREF: readDemoFiles+2Cj
        bx = &aDemo0_bin; // "DEMO0.BIN"
        cl += 0x30; // adds '0'. I assume at this point cl will take a value between 0 and 9,
                    // and then by adding 0x30 that number will be converted to its character in ascii
        *(bx + 4) = cl; // replaces the 0 in DEMO 0 with the value of cl 
        dx = bx;

loc_47647:             // ; CODE XREF: readDemoFiles+31j
        // mov ax, 3D00h
        // int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
        //             ; DS:DX -> ASCIZ filename
        //             ; AL = access mode
        //             ; 0 - read
        FILE *file = fopen(bx, "r"); // bx will store the name of the demo file after changing the number (DEMOx.BIN)
        if (file != NULL)
        { 
            goto loc_47651;
        }
        goto loc_47783;
// ; ---------------------------------------------------------------------------

loc_47651:              //; CODE XREF: readDemoFiles+43j
        lastFileHandle = ax;
        bx = ax;
        if (byte_599D4 != 1)
        {
            goto loc_47674;
        }
        if (word_599DA != 0)
        {
            goto loc_476DB;
        }
        // mov ax, 4200h
        // cx = 0;
        // mov dx, levelDataLength
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: offset from beginning of file
        fseek(lastFileHandle, levelDataLength, SEEK_SET);
        bx = lastFileHandle;
        goto loc_476DB;
// ; ---------------------------------------------------------------------------

loc_47674:             // ; CODE XREF: readDemoFiles+52j
        push(bx);
        // ax = 0x4202;
        // cx = 0;
        // dx = cx;
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: offset from end of file
        int result = fseek(lastFileHandle, 0, SEEK_END);
        // value returned in dx:ax
        pop(bx);
        if (result != 0)
        {
            goto loc_47690;
        }
        if (dx != 0) // the file size shouldn't be more than 0xFFFF, so DX should be 0x0
        {
            goto loc_47690;
        }
        if (ax >= levelDataLength)
        {
            goto loc_47690;
        }
        fileReadUnk1();
        word_599D8 = ax;

loc_47690:             // ; CODE XREF: readDemoFiles+76j readDemoFiles+7Aj ...
        // mov ax, 4200h
        // xor cx, cx
        // mov dx, cx
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: offset from beginning of file
        fseek(lastFileHandle, 0, SEEK_SET);
        if (word_599D8 != 0)
        {
            goto loc_476DB;
        }
        ax = 0x3F00;
        bx = lastFileHandle;
        pop(cx);
        push(cx);
        dx = cx; 
        cx = cx << 1;
        dx += cx;
        cl = 9;
        dx = dx << cl;
        dx += 0xBE20;
        cx = levelDataLength;
        push(ds);
        push(dx);
        push(es);
        pop(ds);
        // assume ds:demoseg
        // int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
        //             ; BX = file handle, CX = number of bytes to read
        //             ; DS:DX -> buffer
        int bytes = fread(&some_buffer_in_demoseg, 1, levelDataLength, lastFileHandle);
        pop(bx);
        dx = *(bx + 0x5FE); // position 1534, so levelDataLength - 2, and it's copying a word... so it's copying the last 2 bytes into dx
        pop(ds);
        // assume ds:data
        if (bytes > 0)
        {
            goto loc_476CB;
        }
        goto loc_47783;
// ; ---------------------------------------------------------------------------

loc_476CB:             // ; CODE XREF: readDemoFiles+BDj
        if (bytes == levelDataLength)
        {
            goto loc_476D3;
        }
        goto loc_47783;
// ; ---------------------------------------------------------------------------

loc_476D3:           //   ; CODE XREF: readDemoFiles+C5j
        pop(bx);
        push(bx);
        bx = bx << 1;
        *(bx - 0x67CA) = dx; // 26570 wtf??? dx still has the last 2 bytes of the level data

loc_476DB:             // ; CODE XREF: readDemoFiles+59j readDemoFiles+69j ...
        cx = 0xBE09; // 48649
        cx -= word_510DF;
        if (cx <= 0xBE09h) // weird way of checking if word_510DF > 0 ????
        {
            goto loc_476EA;
        }
        cx = 0;

loc_476EA:             // ; CODE XREF: readDemoFiles+DDj
        if (cx != 0)
        {
            goto loc_476F3;
        }
        ax = 0;
        goto loc_4771A;
// ; ---------------------------------------------------------------------------

loc_476F3:              // ; CODE XREF: readDemoFiles+E4j
        dx = word_510DF;
        bx = lastFileHandle;
        push(ds);
        ax = es;
        ds = ax;
        // assume ds:nothing
        // mov ax, 3F00h
        // int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
        //             ; BX = file handle, CX = number of bytes to read
        //             ; DS:DX -> buffer
        int bytes = fread(word_510DF, 1, cx, lastFileHandle);
        if (bytes > 0)
        {
            goto loc_47719;
        }
        pop(ds);
        // assume ds:data
        // mov ax, 3E00h
        // mov bx, lastFileHandle
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        ax = 0;
        if (fclose(lastFileHandle) == 0)
        {
            // mov ax, 0
            goto loc_47783
        }
        // mov ax, 0
        goto exit;
// ; ---------------------------------------------------------------------------

loc_47719:             // ; CODE XREF: readDemoFiles+FCj
        pop(ds);

loc_4771A:             // ; CODE XREF: readDemoFiles+E8j
        push(ax);
        // mov ax, 3E00h
        // mov bx, lastFileHandle
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        if (fclose(lastFileHandle) == 0)
        {
            goto loc_47729;
        }
        goto exit;
// ; ---------------------------------------------------------------------------

loc_47729:              ; CODE XREF: readDemoFiles+11Bj
        pop(ax)
        bx = word_510DF;
        *bx = *bx & 0x7F; //and byte ptr es:[bx], 7Fh
        int isNotZero = (word_599D8 != 0);
        word_599D8 = 0;
        if (isNotZero)
        {
            goto loc_47743;
        }
        *bx = *bx | 0x80;

loc_47743:             // ; CODE XREF: readDemoFiles+134j
        cx = bx;
        bx += ax;
        push(ds);
        push(es);
        pop(ds);
        // assume ds:nothing
        bx--;
        if (bx == 0xFFFF)
        {
            goto loc_4775A;
        }
        if (ax <= 1)
        {
            goto loc_4775A;
        }
        if (*bx == 0xFF)
        {
            goto loc_47765;
        }

loc_4775A:             // ; CODE XREF: readDemoFiles+145j
                   // ; readDemoFiles+14Aj
        if (bx >= maxdemolength)
        {
            goto loc_47765;
        }
        bx++;
        ax++;
        *bx = 0xFF;

loc_47765:             // ; CODE XREF: readDemoFiles+14Fj
                   // ; readDemoFiles+155j
        pop(ds);
        // assume ds:data
        pop(cx);
        bx = cx;
        bx = bx << 1;
        dx = word_510DF;
        *bx = dx;  // db 26h, 89h, 97h, 00h, 00h; mov es:[bx+0], dx
        word_510DF += ax;
        cx++;
        if (cx == 10)
        {
            goto loc_47781;
        }
        goto loc_47629;
// ; ---------------------------------------------------------------------------

loc_47781:             // ; CODE XREF: readDemoFiles+173j
        pop(es);
        // assume es:nothing
        return;
// ; ---------------------------------------------------------------------------

loc_47783:              //; CODE XREF: readDemoFiles+45j readDemoFiles+BFj ...
        pop(cx);
        pop(es);
        // return;
// readDemoFiles    endp
}


; =============== S U B R O U T I N E =======================================
*/

void convertPaletteDataToPalette(ColorPaletteData paletteData, ColorPalette outPalette)
{
    int kExponent = 4; // no idea why (yet)

    for (int i = 0; i < kNumberOfColors; ++i)
    {
        outPalette[i].r = paletteData[i * 4 + 0] << kExponent;
        outPalette[i].g = paletteData[i * 4 + 1] << kExponent;
        outPalette[i].b = paletteData[i * 4 + 2] << kExponent;
        outPalette[i].a = paletteData[i * 4 + 3] << kExponent; // intensity, for now
    }
}

void readPalettes()  // proc near       ; CODE XREF: readEverythingp
{
    FILE *file = fopen("PALETTES.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
    //        recoverFilesFromFloppyDisk();
    //        jb  short loc_4779C // if success try again?
    //        goto short loc_4779F;
        }

        exitWithError("Error opening PALETTES.DAT\n");
    }

//loc_4779F:              // ; CODE XREF: readPalettes+8j
    ColorPaletteData palettesDataBuffer[kNumberOfPalettes];
    size_t bytes = fread(&palettesDataBuffer, sizeof(ColorPaletteData), kNumberOfPalettes, file);
    if (bytes == 0)
    {
        exitWithError("Couldn't read PALETTES.DAT\n");
    }

    for (int i = 0; i < kNumberOfPalettes; ++i)
    {
        convertPaletteDataToPalette(palettesDataBuffer[i], gPalettes[i]);
    }

//loc_477B6:              //; CODE XREF: readPalettes+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PALETTES.DAT\n");
    }

// readPalettes   endp
}

void openCreditsBlock() // proc near      ; CODE XREF: start+2E9p
{
    static const int kEdgeWidth = 16;
    static const int kEdgeHeight = 148;
    static const int kEdgeStep = 8;
    static const int kEdgeTopY = 26;
    static const int kNumberOfFrames = 15;

    int leftEdgeX = 144 - kEdgeStep; // Left edge starts at x=144
    int rightEdgeX = leftEdgeX + kEdgeWidth + kEdgeStep - 1;

    for (int j = 0; j < kNumberOfFrames; ++j)
    {
//loc_47800:             // ; CODE XREF: openCreditsBlock+AFj
        // Renders the screen twice (to remove some weird artifacts?)
        for (int i = 0; i < 2; ++i)
        {
            videoloop();
            loopForVSync();
        }

        // This loop moves both edges of the panel, and fills the inside of the panel with the contents of TITLE2.DAT
        for (int y = kEdgeTopY; y < kEdgeTopY + kEdgeHeight; ++y)
        {
            // Left edge
            for (int x = leftEdgeX; x < leftEdgeX + kEdgeWidth; ++x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr] = gScreenPixels[addr + kEdgeStep]; // Move panel edge
                gScreenPixels[addr + kEdgeStep] = gTitle2DecodedBitmapData[addr + kEdgeStep]; // Content of now visible panel
            }

            // Right edge
            for (int x = rightEdgeX + kEdgeWidth; x > rightEdgeX; --x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr + kEdgeStep] = gScreenPixels[addr]; // Move panel edge
                gScreenPixels[addr] = gTitle2DecodedBitmapData[addr]; // Content of now visible panel
            }
        }

        leftEdgeX -= kEdgeStep;
        rightEdgeX += kEdgeStep;
    }

//loc_47884:             // ; CODE XREF: openCreditsBlock+C7j
    videoloop();
    loopForVSync();

//    word_51967 = title2DataBuffer; // points to where the title 2 has been RENDERED

    // Display now the contents of TITLE2.DAT starting at the y=panel_edge_top_y (to prevent removing the top title)
    // This basically makes the edges of the panel docked at the sides of the screen look better (as intended in TITLE2.DAT
    // compared to how they look when the "crafted" animation concludes).
    //
    size_t copyOffset = kEdgeTopY * kScreenWidth;
    memcpy(gScreenPixels + copyOffset, gTitle2DecodedBitmapData + copyOffset, sizeof(gTitle2DecodedBitmapData) - copyOffset);

    ColorPalette title2Palette;
    convertPaletteDataToPalette(gTitle2PaletteData, title2Palette);
    fadeToPalette(title2Palette); // fades current frame buffer into the title 2.dat (screen with the credits)
}

void loadScreen2() // proc near       ; CODE XREF: start:loc_46F00p
{
    //loc_478C0:              // ; CODE XREF: loadScreen2+8j
    FILE *file = fopen("TITLE1.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE1.DAT\n");
    }

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
//loc_478E7:              //; CODE XREF: loadScreen2+6Bj
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, 1, kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE1.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Copy directly to the screen too
            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE1.DAT\n");
    }

//loc_4792E:              //; CODE XREF: loadScreen2+76j
//    word_51967 = gScreenPixels??; // points to where title1.dat was RENDERED
    videoloop();
//    gNumberOfDotsToShiftDataLeft = 0;
    ColorPalette title1DatPalette;
    convertPaletteDataToPalette(gTitle1PaletteData, title1DatPalette);
    setPalette(title1DatPalette);

    file = fopen("TITLE2.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE2.DAT\n");
    }

// loc_47978:              //; CODE XREF: loadScreen2+C0j
    for (int y = 0; y < kScreenHeight; y++)
    {
        // loc_47995:              //; CODE XREF: loadScreen2+119j
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, sizeof(uint8_t), kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE2.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_479AF:              ; CODE XREF: loadScreen2+113j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gTitle2DecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE2.DAT\n");
    }
//loc_479DC:             // ; CODE XREF: loadScreen2+124j
// loadScreen2 endp ; sp-analysis failed
}

void loadMurphySprites() //  proc near       ; CODE XREF: start:isFastModep
                    //; start+382p ...
{
//    int baseDestAddress;      // = word ptr -6
//    int y;      // = word ptr -4
//    char component;      // = byte ptr -1

    // push    bp
    // mov bp, sp
    // add sp, 0FFFAh

//loc_479ED:              // ; CODE XREF: loadMurphySprites+27j

// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// MOVING.DAT bitmap size is 320x462
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    FILE *file = fopen("MOVING.DAT", "r");

    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
            // Try to recover files from floppy disk and retry
            // We won't do that here
//            recoverFilesFromFloppyDisk();
//            pushf();
//            if (byte_59B86 != 0FFh)
//            {
//                goto loc_47A0B;
//            }
//            popf();
//            goto loc_47AE3;
//
// loc_47A0B:              ; CODE XREF: loadMurphySprites+1Ej
//            popf
//            jb  short loc_47A10
//            jmp short loc_479ED
        }

//    loc_47A10:              ; CODE XREF: loadMurphySprites+13j
//                    ; loadMurphySprites+25j
        exitWithError("Error opening MOVING.DAT\n");
    }

//loc_47A13:              ; CODE XREF: loadMurphySprites+Ej

    // There is an error in the original code, it uses height 464 (which should be
    // harmless in the original implementation since fread doesn't fail there, just
    // returns 0. The new implementation will fail on partial reads.
    for (int y = 0; y < kMovingBitmapHeight; ++y)
    {
    // var_4 = 0;

        uint8_t fileData[kMovingBitmapWidth / 2];

        size_t bytes = fread(fileData, 1, sizeof(fileData), file);
        if (bytes < sizeof(fileData))
        {
            exitWithError("Error reading MOVING.DAT\n");
        }

//loc_47A40:              ; CODE XREF: loadMurphySprites+BAj
        for (int x = 0; x < kMovingBitmapWidth; ++x)
        {
        // var_1 = 0;

//loc_47A45:              ; CODE XREF: loadMurphySprites+AEj
            uint32_t destPixelAddress = y * kMovingBitmapWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gMovingDecodedBitmapData[destPixelAddress] = finalColor;
            /*

//loc_47A59:              ; CODE XREF: loadMurphySprites+6Dj
//            si = &fileLevelData;
            // ax = var_4;
            // bx = 0x7A; // 122
            // ax = ax * bx;
            di = y * 122;
            di += baseDestAddress;

//loc_47A69:              ; CODE XREF: loadMurphySprites+8Cj
            while (di >= 19764)
            {
                di -= 0x4D0C; //19724
                // goto loc_47A69;
            }
            // if (di < 0x4D34) //19764
            // {
            //     goto loc_47A75;
            // }
            // di -= 0x4D0C;

//loc_47A75:              //; CODE XREF: loadMurphySprites+86j
            cl = component;
            ah = 1;
            ah = ah << cl;
            // mov dx, 3C4h
            // al = 2
            // out dx, al      ; EGA: sequencer address reg
            //             ; map mask: data bits 0-3 enable writes to bit planes 0-3
//            ports[0x3C4] = 2;
            // inc dx
            // al = ah
            // out dx, al      ; EGA port: sequencer data register
//            ports[0x3C5] = ah;
            cx = 0x14; // 20
            memcpy(di, si, 20); // rep movsw
            di += 20;
            si += 20;
             */
        }
    }
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MOVING.DAT\n");
    }

//loc_47AB1:              ; CODE XREF: loadMurphySprites+C5j
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // FIXED.DAT bitmap size is 640x16
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    file = fopen("FIXED.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening FIXED.DAT\n");
    }

//loc_47ABE:              //; CODE XREF: loadMurphySprites+D2j
    uint8_t bitmapData[kFixedBitmapWidth * kFixedBitmapHeight / 2];
    size_t bytes = fread(bitmapData, 1, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading FIXED.DAT\n");
    }

    for (int y = 0; y < kFixedBitmapHeight; ++y)
    {
        for (int x = 0; x < kFixedBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kFixedBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kFixedBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 160] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 240] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gFixedDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47AD5:              //; CODE XREF: loadMurphySprites+E9j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing FIXED.DAT\n");
    }

//loc_47AE3:              //; CODE XREF: loadMurphySprites+21j
                // ; loadMurphySprites+F7j
    // mov dx, 3C4h
    // al = 2
    // out dx, al      ; EGA: sequencer address reg
    //             ; map mask: data bits 0-3 enable writes to bit planes 0-3
//    ports[0x3C4] = 2;
    // inc dx
    // al = 0FFh
    // out dx, al      ; EGA port: sequencer data register
//    ports[0x3C5] = 0xFF;
    // mov sp, bp
    // pop bp
    // return;
}

void readPanelDat() //    proc near       ; CODE XREF: readPanelDat+14j
                    // ; readEverything+6p
{
    FILE *file = fopen("PANEL.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
            // recoverFilesFromFloppyDisk();
            // jb  short loc_47B07 // try again
        }

        exitWithError("Error opening PANEL.DAT\n");
    }

//loc_47B0A:             // ; CODE XREF: readPanelDat+8j
    size_t bytes = fread(gPanelBitmapData, sizeof(uint8_t), sizeof(gPanelBitmapData), file);
    if (bytes < sizeof(gPanelBitmapData))
    {
        exitWithError("Error reading PANEL.DAT\n");
    }

//loc_47B21:              // ; CODE XREF: readPanelDat+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PANEL.DAT\n");
    }

// readPanelDat    endp
}

void readBackDat() // proc near       ; CODE XREF: readBackDat+14j
                    // ; readEverything+15p
{
    // address: 01ED:0ECD
    FILE *file = fopen("BACK.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
            // recoverFilesFromFloppyDisk();
            // jb  short loc_47B46 // Try again
        }

        exitWithError("Error opening BACK.DAT\n");
    }

//loc_47B49:             // ; CODE XREF: readBackDat+8j
    size_t bytes = fread(gBackBitmapData, 1, sizeof(gBackBitmapData), file);
    if (bytes < sizeof(gBackBitmapData))
    {
        exitWithError("Error reading BACK.DAT\n");
    }

//loc_47B67:              //; CODE XREF: readBackDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing BACK.DAT\n");
    }

// readBackDat endp
}

// The bitmap fonts are bitmaps of 512x8 pixels. The way they're encoded is each bit is a pixel that
// is either on or off, so this is encoded in exactly 512 bytes.
// These bits are used as bitmasks to render on top of an existing image combining:
// - enabling the corresponding pixels in the frame buffer
// - setting those pixels from the font in the bitmask to prevent the VGA controller from overriding
//   other pixels (i.e. preserving the content of the original image in the pixels that are "empty"
//   in the font).
//
void readBitmapFonts() //   proc near       ; CODE XREF: readBitmapFonts+14j
                    // ; readEverything+3p
{
    FILE *file = fopen("CHARS6.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
            // recoverFilesFromFloppyDisk();
            // jb  short loc_47B8D // try again
        }

        exitWithError("Error opening CHARS6.DAT\n");
    }

//loc_47B8D:             // ; CODE XREF: readBitmapFonts+Dj
                //; readBitmapFonts+12j

//loc_47B90:              // ; CODE XREF: readBitmapFonts+8j
    size_t bytes = fread(gChars6BitmapFont, sizeof(uint8_t), kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS6.DAT\n");
    }

//loc_47BA7:              // ; CODE XREF: readBitmapFonts+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS6.DAT\n");
    }

//loc_47BB5:              //; CODE XREF: readBitmapFonts+39j
    file = fopen("CHARS8.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS8.DAT\n");
    }

//loc_47BC2:              //; CODE XREF: readBitmapFonts+46j
    bytes = fread(gChars8BitmapFont, sizeof(uint8_t), kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS8.DAT\n");
    }

//loc_47BD9:             // ; CODE XREF: readBitmapFonts+5Dj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS8.DAT\n");
    }
}

void readTitleDatAndGraphics() // proc near  ; CODE XREF: start+2BBp
{
//  word_51967 = 0x4D84; // address where the bitmap will be rendered
    videoloop();
//  gNumberOfDotsToShiftDataLeft = 0;
    FILE *file = fopen("TITLE.DAT", "r");

    if (file == NULL)
    {
        return; //goto exit;
    }

//loc_47C18:              // ; CODE XREF: readTitleDatAndGraphics+2Bj

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
        //loc_47C3F:              //; CODE XREF: readTitleDatAndGraphics+8Ej
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, sizeof(uint8_t), kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            return; //goto exit;
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_47C54:             // ; CODE XREF: readTitleDatAndGraphics+67j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

//loc_47C59:              ; CODE XREF: readTitleDatAndGraphics+88j
            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        return; //goto exit;
    }

//loc_47C86:              //; CODE XREF: readTitleDatAndGraphics+99j
}

void readLevelsLst() //   proc near       ; CODE XREF: readLevelsLst+CCj
                    // ; readEverything+Fp ...
{
    // 01ED:1038

//    memset(gLevelListData, 0, sizeof(gLevelListData));
    char paddingEntryText[kLevelNameLength] = "                           ";
    for (int i = 0; i < kNumberOfLevelsWithPadding; ++i)
    {
        memcpy(&gPaddedLevelListData[i * kLevelNameLength], paddingEntryText, sizeof(paddingEntryText));
    }
    memcpy(&gPaddedLevelListData[kLastLevelIndex * kLevelNameLength],
           "- REPLAY SKIPPED LEVELS!! -",
           kLevelNameLength);
    memcpy(&gPaddedLevelListData[(kLastLevelIndex + 1) * kLevelNameLength],
           "---- UNBELIEVEABLE!!!! ----",
           kLevelNameLength);

    FILE *file = fopen("LEVEL.LST", "r");
    if (file == NULL)
    {
//errorOpeningLevelLst:             // ; CODE XREF: readLevelsLst+8j
        FILE *file = fopen("LEVELS.DAT", "r");
        if (file == NULL)
        {
//errorOpeningLevelsDat:             // ; CODE XREF: readLevelsLst+17j
            if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
            {
                // This would try to recover the files from the floppy disk
                // in the original game and then try again to load the level
                // list. That won't be an option here...
                //
                // recoverFilesFromFloppyDisk();
                // jb  short loc_47D6A
//                goto readLevelsLst; // wtf?
            }
            exitWithError("Error opening LEVELS.DAT\n");
        }
//successOpeningLevelsDat:             // ; CODE XREF: readLevelsLst+15j

        for (int i = 0; i < kNumberOfLevels; ++i)
        {
    //loc_47CC4:             // ; CODE XREF: readLevelsLst:loc_47CE4j
            char number[5];
            sprintf(number, "%03d ", i + 1);

            memcpy(gLevelListData + i * kLevelNameLength, number, sizeof(number) - 1);
            gLevelListData[i * kLevelNameLength + kLevelNameLength - 1] = '\n';
    //loc_47CE4:            //  ; CODE XREF: readLevelsLst+3Aj
        }

        for (int i = 0; i < kNumberOfLevels; ++i)
        {
// loc_47CF1:             //  ; CODE XREF: readLevelsLst+83j

            int seekOffset = 0x5A6 + i * levelDataLength;

            fseek(file, seekOffset, SEEK_SET); // position 1446
            size_t bytes = fread(gLevelListData + i * kLevelNameLength + 4, 1, 23, file);

            if (bytes < 23)
            {
                fclose(file);
                exitWithError("Error reading LEVELS.DAT\n");
            }
        }
        if (fclose(file) != 0)
        {
            exitWithError("Error closing LEVELS.DAT\n");
        }

        if (byte_59B62 == 0)
        {
            return;
        }

//    loc_47D35:             // ; CODE XREF: readLevelsLst+95j
        file = fopen("LEVEL.LST", "w");
        if (file == NULL)
        {
            exitWithError("Error opening LEVEL.LST\n");
        }

//    writeLevelLstData:             // ; CODE XREF: readLevelsLst+A5j
        size_t bytes = fwrite(gLevelListData, 1, kLevelListDataLength, file);
        if (bytes < kLevelListDataLength)
        {
            exitWithError("Error writing LEVEL.LST\n");
        }

//    loc_47D5B:             // ; CODE XREF: readLevelsLst+BBj
        if (fclose(file) != 0)
        {
            exitWithError("Error closing LEVEL.LST\n");
        }
        return;
    }

//successOpeningLevelLst:             // ; CODE XREF: readLevelsLst+Aj
    size_t bytes = fread(gLevelListData, 1, kLevelListDataLength, file);
    if (bytes < kLevelListDataLength)
    {
        fclose(file);
        exitWithError("Error reading LEVEL.LST\n");
    }

//loc_47D8D:             // ; CODE XREF: readLevelsLst+8Aj
               // ; readLevelsLst:loc_47D5Bj ...
    if (fclose(file) != 0)
    {
        exitWithError("Error closing LEVEL.LST\n");
    }
}

void readGfxDat() //  proc near       ; CODE XREF: readGfxDat+14j
                   // ; readEverything+1Ep
{
    FILE *file = fopen("GFX.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
            // recoverFilesFromFloppyDisk();
            // jb  short loc_47DB2 // try again
        }
        exitWithError("Error opening GFX.DAT\n");
    }

//loc_47DB5:             // ; CODE XREF: readGfxDat+8j
    size_t bytes = fread(gGfxBitmapData, 1, sizeof(gGfxBitmapData), file);
    if (bytes < sizeof(gGfxBitmapData))
    {
        exitWithError("Error reading GFX.DAT\n");
    }

//loc_47DD3:             // ; CODE XREF: readGfxDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing GFX.DAT\n");
    }

// readGfxDat  endp
}

void readControlsDat() // proc near       ; CODE XREF: readControlsDat+14j
                    // ; readEverything+Cp
{
    FILE *file = fopen("CONTROLS.DAT", "r");
    if (file == NULL)
    {
        if (errno == ENOENT) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
//          recoverFilesFromFloppyDisk();
//          jb  short loc_47DF9 // try again
        }

        exitWithError("Error opening CONTROLS.DAT\n");
    }

//loc_47DFC:             // ; CODE XREF: readControlsDat+8j
    size_t bytes = fread(gControlsBitmapData, 1, sizeof(gControlsBitmapData), file);
    if (bytes < sizeof(gControlsBitmapData))
    {
        exitWithError("Error reading CONTROLS.DAT\n");
    }

//loc_47E1A:             // ; CODE XREF: readControlsDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CONTROLS.DAT\n");
    }

// readControlsDat endp
}

void readPlayersLst() //  proc near       ; CODE XREF: readEverything+1Bp
                    // ; handleFloppyDiskButtonClick+149p
{
    if (byte_59B85 != 0)
    {
        return;
    }

    FILE *file = fopen("PLAYER.LST", "r");
    if (file == NULL)
    {
        return;
    }

    size_t bytes = fread(gPlayerListData, 1, sizeof(gPlayerListData), file);
    if (bytes == 0)
    {
        return;
    }
    
    fclose(file);

// readPlayersLst  endp
}

void readHallfameLst() // proc near       ; CODE XREF: readEverything+18p
                    // ; handleFloppyDiskButtonClick+146p
{
    if (byte_59B85 != 0)
    {
        return;
    }
    
    FILE *file = fopen("HALLFAME.LST", "r");
    if (file == NULL)
    {
        return;
    }
    
    size_t bytes = fread(gHallOfFameData, 1, sizeof(gHallOfFameData), file);
    if (bytes == 0)
    {
        return;
    }
    
    fclose (file);

// readHallfameLst endp
}

void readEverything() //  proc near       ; CODE XREF: start+2DBp start+2E3p ...
{
    // 01ED:1213
    readPalettes();
    readBitmapFonts();
    readPanelDat();
    readMenuDat();
    readControlsDat();
    readLevelsLst(); // TODO: need to test & debug when LEVEL.LST is not available and it's generated from LEVELS.DAT
//    readDemoFiles(); // TODO: just crazy, needs more RE work
    readBackDat();
    readHallfameLst();
    readPlayersLst();
    readGfxDat();
}

void waitForKeyMouseOrJoystick() // sub_47E98  proc near       ; CODE XREF: recoverFilesFromFloppyDisk+4Ap
//                    ; handleStatisticsOptionClick+216p ...
{
    byte_59B86 = 0;

    do
    {
//keyIsPressed:               ; CODE XREF: waitForKeyMouseOrJoystick+16j
        if (byte_5197E != 0)
        {
            byte_59B86 = 0xFF;
        }

        int9handler();
//loc_47EA9:              ; CODE XREF: waitForKeyMouseOrJoystick+Aj
    }
    while (keyPressed != SDL_SCANCODE_UNKNOWN);

    uint16_t mouseButtonsStatus;

    do
    {
//mouseIsClicked:              ; CODE XREF: waitForKeyMouseOrJoystick+1Ej
        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
    }
    while (mouseButtonsStatus != 0);

    do
    {
//loc_47EB8:              ; CODE XREF: waitForKeyMouseOrJoystick+28j
        sub_48E59();
    }
    while (byte_50941 > 4);

    for (int i = 0; i < 4200; ++i)
    {
//loc_47EC6:              ; CODE XREF: waitForKeyMouseOrJoystick+57j
        videoloop();
        loopForVSync();

        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
        int9handler();

        if (mouseButtonsStatus != 0)
        {
            break;
        }
        if (keyPressed != SDL_SCANCODE_UNKNOWN)
        {
            break;
        }
        sub_48E59();
        if (byte_50941 > 4)
        {
            break;
        }
    }

    if (mouseButtonsStatus != 0)
    {
//loc_47F02:              ; CODE XREF: waitForKeyMouseOrJoystick+44j
//                ; waitForKeyMouseOrJoystick+70j
        do
        {
            getMouseStatus(NULL, NULL, &mouseButtonsStatus);
        }
        while (mouseButtonsStatus != 0);
    }
    else if (keyPressed != SDL_SCANCODE_UNKNOWN)
    {
        do
        {
//loc_47F0C:              ; CODE XREF: waitForKeyMouseOrJoystick+4Bj
//                ; waitForKeyMouseOrJoystick+85j
        if (byte_5197E != 0)
        {
            byte_59B86 = 0xFF;
        }

        int9handler();

//loc_47F18:              ; CODE XREF: waitForKeyMouseOrJoystick+79j
        }
        while (keyPressed != SDL_SCANCODE_UNKNOWN);
    }
    else if (byte_50941 > 4)
    {
        do
        {
//loc_47F21:              ; CODE XREF: waitForKeyMouseOrJoystick+55j
//                ; waitForKeyMouseOrJoystick+9Dj
            if (byte_5197E != 0)
            {
                byte_59B86 = 0xFF;
            }

//loc_47F2D:              ; CODE XREF: waitForKeyMouseOrJoystick+8Ej
            sub_48E59();
        }
        while (byte_50941 > 4);
    }

//loc_47EF1:              ; CODE XREF: waitForKeyMouseOrJoystick+3Cj
//                ; waitForKeyMouseOrJoystick+72j ...
    word_510A2 = 0;
    char shouldJump = (byte_59B86 == 0);
//    clc // returns success?
    if (shouldJump == 0) // Checks an error/success?
    {
//    stc // returns error?
    }
}

/*
void recoverFilesFromFloppyDisk() //   proc near       ; CODE XREF: readPalettes+Fp
                   // ; loadMurphySprites+15p ...
{
        call    enableFloppy
        mov si, 60D5h
        call    setPalette
        call    drawBackBackground
        cmp byte_53A10, 1
        jnz short loc_47F56
        mov si, 370Fh
        mov byte_53A10, 2
        jmp short loc_47F5E
// ; ---------------------------------------------------------------------------

loc_47F56:              ; CODE XREF: recoverFilesFromFloppyDisk+11j
        mov si, 3701h
        mov byte_53A10, 1

loc_47F5E:              ; CODE XREF: recoverFilesFromFloppyDisk+1Bj
        mov di, 7D3Bh
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        mov si, palettesDataBuffer
        call    setPalette
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    waitForKeyMouseOrJoystick
        pushf
        mov si, 60D5h
        call    setPalette
        popf
        return;
// recoverFilesFromFloppyDisk   endp
}

// ; ---------------------------------------------------------------------------
        nop

; =============== S U B R O U T I N E =======================================


movefun   proc near       ; DATA XREF: data:160Co
        cmp byte ptr leveldata[si], 1
        jz  short loc_47F98
        return;
// ; ---------------------------------------------------------------------------

loc_47F98:              ; CODE XREF: movefun+5j
        mov ax, leveldata[si]
        cmp ax, 1
        jz  short loc_47FA4
        jmp loc_48035
// ; ---------------------------------------------------------------------------

loc_47FA4:              ; CODE XREF: movefun+Fj
        cmp byte_51035, 2
        jnz short loc_47FAC
        return;
// ; ---------------------------------------------------------------------------

loc_47FAC:              ; CODE XREF: movefun+19j
        mov ax, leveldata[si+78h]
        cmp ax, 0
        jz  short loc_47FF4
        cmp ax, 1
        jz  short loc_47FC5
        cmp ax, 4
        jz  short loc_47FC5
        cmp ax, 5
        jz  short loc_47FC5
        return;
// ; ---------------------------------------------------------------------------

loc_47FC5:              ; CODE XREF: movefun+28j
                    ; movefun+2Dj ...
        cmp leveldata[si+78h-2], 0
        jz  short loc_47FFB
        cmp leveldata[si+78h-2], 8888h
        jz  short loc_47FFB
        cmp leveldata[si+78h-2], 0AAAAh
        jz  short loc_47FFB

loc_47FDC:              ; CODE XREF: movefun+72j
                    ; movefun+1F1j
        cmp leveldata[si+78h+2], 0
        jz  short loc_48011
        cmp leveldata[si+78h+2], 8888h
        jz  short loc_48011
        cmp leveldata[si+78h+2], 0AAAAh
        jz  short loc_48011
        return;
// ; ---------------------------------------------------------------------------

loc_47FF4:              ; CODE XREF: movefun+23j
        mov byte ptr leveldata[si+1], 40h ; '@'
        jmp short loc_48035
// ; ---------------------------------------------------------------------------

loc_47FFB:              ; CODE XREF: movefun+3Aj
                    ; movefun+42j ...
        cmp leveldata[si-2], 0
        jz  short loc_48004
        jmp short loc_47FDC
// ; ---------------------------------------------------------------------------

loc_48004:              ; CODE XREF: movefun+70j
        mov byte ptr leveldata[si+1], 50h ; 'P'
        mov leveldata[si-2], 8888h
        jmp short loc_48035
// ; ---------------------------------------------------------------------------

loc_48011:              ; CODE XREF: movefun+51j
                    ; movefun+59j ...
        cmp leveldata[si+2], 0
        jz  short loc_48028
        cmp leveldata[si+2], 9999h
        jnz short locret_48027
        cmp leveldata[si-78h+2h], 1
        jz  short loc_48028

locret_48027:               ; CODE XREF: movefun+8Ej
        return;
// ; ---------------------------------------------------------------------------

loc_48028:              ; CODE XREF: movefun+86j
                    ; movefun+95j
        mov byte ptr leveldata[si+1], 60h ; '`'
        mov leveldata[si+2], 8888h
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48035:              ; CODE XREF: movefun+11j
                    ; movefun+69j ...
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        al = bl
        and al, 0F0h
        cmp al, 10h
        jnz short loc_48045
        jmp short loc_48078
// ; ---------------------------------------------------------------------------

loc_48045:              ; CODE XREF: movefun+B1j
        cmp al, 20h ; ' '
        jnz short loc_4804C
        jmp loc_48212
// ; ---------------------------------------------------------------------------

loc_4804C:              ; CODE XREF: movefun+B7j
        cmp al, 30h ; '0'
        jnz short loc_48053
        jmp loc_48277
// ; ---------------------------------------------------------------------------

loc_48053:              ; CODE XREF: movefun+BEj
        cmp byte_51035, 2
        jnz short loc_4805B
        return;
// ; ---------------------------------------------------------------------------

loc_4805B:              ; CODE XREF: movefun+C8j
        cmp al, 40h ; '@'
        jnz short loc_48062
        jmp loc_482DC
// ; ---------------------------------------------------------------------------

loc_48062:              ; CODE XREF: movefun+CDj
        cmp al, 50h ; 'P'
        jnz short loc_48069
        jmp loc_4830A
// ; ---------------------------------------------------------------------------

loc_48069:              ; CODE XREF: movefun+D4j
        cmp al, 60h ; '`'
        jnz short loc_48070
        jmp loc_48378
// ; ---------------------------------------------------------------------------

loc_48070:              ; CODE XREF: movefun+DBj
        cmp al, 70h ; 'p'
        jnz short locret_48077
        jmp loc_483E6
// ; ---------------------------------------------------------------------------

locret_48077:               ; CODE XREF: movefun+E2j
        return;
// ; ---------------------------------------------------------------------------

loc_48078:              ; CODE XREF: movefun+B3j
                    ; movefun+475j
        xor bh, bh
        shl bl, 1
        shl bl, 1
        and bl, 1Fh
        mov di, [si+60DDh]
        add di, [bx+6C95h]
        push    si
        mov si, word_515A2
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_48096:              ; CODE XREF: movefun+10Ej
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_48096
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 16h
        jnz short loc_480BB
        mov [si+1835h], bl
        sub si, 78h ; 'x'
        call    sub_488DC
        add si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_480BB:              ; CODE XREF: movefun+11Bj
        cmp bl, 18h
        jge short loc_480C5
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_480C5:              ; CODE XREF: movefun+12Ej
        mov byte ptr [si+1835h], 0
        cmp byte_51035, 2
        jnz short loc_480D2
        return;
// ; ---------------------------------------------------------------------------

loc_480D2:              ; CODE XREF: movefun+13Fj
        cmp word ptr [si+18ACh], 0
        jnz short loc_480DC
        jmp loc_4816D
// ; ---------------------------------------------------------------------------

loc_480DC:              ; CODE XREF: movefun+147j
        cmp word ptr [si+18ACh], 9999h
        jnz short loc_480E7
        jmp loc_4816D
// ; ---------------------------------------------------------------------------

loc_480E7:              ; CODE XREF: movefun+152j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_480F1
        jmp loc_481A4
// ; ---------------------------------------------------------------------------

loc_480F1:              ; CODE XREF: movefun+15Cj
        cmp byte ptr [si+18ACh], 11h
        jnz short loc_480FB
        jmp loc_481FE
// ; ---------------------------------------------------------------------------

loc_480FB:              ; CODE XREF: movefun+166j
        cmp word ptr [si+18ACh], 2BBh
        jnz short loc_48106
        jmp loc_481C6
// ; ---------------------------------------------------------------------------

loc_48106:              ; CODE XREF: movefun+171j
        cmp word ptr [si+18ACh], 4BBh
        jnz short loc_48111
        jmp loc_481E2
// ; ---------------------------------------------------------------------------

loc_48111:              ; CODE XREF: movefun+17Cj
        cmp byte ptr [si+18ACh], 18h
        jnz short loc_4811B
        jmp loc_481FE
// ; ---------------------------------------------------------------------------

loc_4811B:              ; CODE XREF: movefun+186j
        cmp word ptr [si+18ACh], 8
        jnz short loc_48125
        jmp loc_48205
// ; ---------------------------------------------------------------------------

loc_48125:              ; CODE XREF: movefun+190j
        call    sound7
        cmp word ptr [si+18ACh], 1
        jz  short loc_4813E
        cmp word ptr [si+18ACh], 4
        jz  short loc_4813E
        cmp word ptr [si+18ACh], 5
        jz  short loc_4813E
        return;
// ; ---------------------------------------------------------------------------

loc_4813E:              ; CODE XREF: movefun+19Dj
                    ; movefun+1A4j ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_4817A
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_4817A
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_4817A
        cmp word ptr [si+18AEh], 0
        jz  short loc_48190
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_48190
        cmp word ptr [si+18AEh], 0AAAAh

loc_4816A:
        jz  short loc_48190
        return;
// ; ---------------------------------------------------------------------------

loc_4816D:              ; CODE XREF: movefun+149j
                    ; movefun+154j
        mov word ptr leveldata[si], 7001h
        mov word ptr [si+18ACh], 9999h
        return;
// ; ---------------------------------------------------------------------------

loc_4817A:              ; CODE XREF: movefun+1B3j
                    ; movefun+1BBj ...
        cmp word ptr [si+1832h], 0
        jz  short loc_48184
        jmp loc_47FDC
// ; ---------------------------------------------------------------------------

loc_48184:              ; CODE XREF: movefun+1EFj
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_48190:              ; CODE XREF: movefun+1CAj
                    ; movefun+1D2j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_48198
        return;
// ; ---------------------------------------------------------------------------

loc_48198:              ; CODE XREF: movefun+205j
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_481A4:              ; CODE XREF: movefun+15Ej
        mov bl, [si+18ADh]
        cmp bl, 0Eh
        jz  short locret_48211
        cmp bl, 0Fh
        jz  short locret_48211
        cmp bl, 28h ; '('
        jz  short locret_48211
        cmp bl, 29h ; ')'
        jz  short locret_48211
        cmp bl, 25h ; '%'
        jz  short locret_48211
        cmp bl, 26h ; '&'
        jz  short locret_48211

loc_481C6:              ; CODE XREF: movefun+173j
        cmp byte ptr [si+18AAh], 18h
        jnz short loc_481D3
        mov word ptr [si+18ACh], 18h

loc_481D3:              ; CODE XREF: movefun+23Bj
        cmp byte ptr [si+18AAh], 1Fh
        jz  short loc_481E0
        mov word ptr [si+18AAh], 0

loc_481E0:              ; CODE XREF: movefun+248j
        jmp short loc_481FE
// ; ---------------------------------------------------------------------------

loc_481E2:              ; CODE XREF: movefun+17Ej
        cmp byte ptr [si+18AEh], 18h
        jnz short loc_481EF
        mov word ptr [si+18ACh], 18h

loc_481EF:              ; CODE XREF: movefun+257j
        cmp byte ptr [si+18AEh], 1Fh
        jz  short loc_481FC
        mov word ptr [si+18AEh], 0

loc_481FC:              ; CODE XREF: movefun+264j
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_481FE:              ; CODE XREF: movefun+168j
                    ; movefun+188j ...
        add si, 78h ; 'x'
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_48205:              ; CODE XREF: movefun+192j
        add si, 78h ; 'x'
        mov bx, si
        shr bx, 1
        mov byte ptr [bx+2434h], 6

locret_48211:               ; CODE XREF: movefun+21Bj
                    ; movefun+220j ...
        return;
// ; ---------------------------------------------------------------------------

loc_48212:              ; CODE XREF: movefun+B9j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6155h]
        push    si
        mov si, 1294h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax

loc_48228:
        mov cx, 10h

loc_4822B:              ; CODE XREF: movefun+2A5j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_4822B
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 24h ; '$'
        jnz short loc_4824A
        mov word ptr [si+1836h], 0AAAAh

loc_4824A:              ; CODE XREF: movefun+2B2j
        cmp bl, 26h ; '&'
        jnz short loc_4825D
        mov [si+1835h], bl
        add si, 2
        call    sub_488DC
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4825D:              ; CODE XREF: movefun+2BDj
        cmp bl, 28h ; '('
        jge short loc_48267
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_48267:              ; CODE XREF: movefun+2D0j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        return;
// ; ---------------------------------------------------------------------------

loc_48277:              ; CODE XREF: movefun+C0j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6153h]
        push    si
        mov si, 12A4h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax

loc_4828D:
        mov cx, 10h

loc_48290:              ; CODE XREF: movefun+30Aj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48290
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 34h ; '4'
        jnz short loc_482AF
        mov word ptr [si+1832h], 0AAAAh

loc_482AF:              ; CODE XREF: movefun+317j
        cmp bl, 36h ; '6'
        jnz short loc_482C2
        mov [si+1835h], bl
        sub si, 2
        call    sub_488DC
        add si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_482C2:              ; CODE XREF: movefun+322j
        cmp bl, 38h ; '8'
        jge short loc_482CC
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_482CC:              ; CODE XREF: movefun+335j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        return;
// ; ---------------------------------------------------------------------------

loc_482DC:              ; CODE XREF: movefun+CFj
        inc bl
        cmp bl, 42h ; 'B'
        jge short loc_482E8
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_482E8:              ; CODE XREF: movefun+351j
        cmp word ptr [si+18ACh], 0
        jz  short loc_482F6
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_482F6:              ; CODE XREF: movefun+35Dj
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 10h
        return;
// ; ---------------------------------------------------------------------------

loc_4830A:              ; CODE XREF: movefun+D6j
        xor bh, bh
        and bl, 0Fh
        shl bl, 1
        mov di, [si+6153h]
        push    si
        mov si, 1294h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48323:              ; CODE XREF: movefun+39Dj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48323
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 52h ; 'R'
        jge short loc_48341
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_48341:              ; CODE XREF: movefun+3AAj
        cmp word ptr [si+18AAh], 0
        jnz short loc_48371
        cmp word ptr [si+1832h], 0
        jz  short loc_48357
        cmp word ptr [si+1832h], 8888h
        jnz short loc_48371

loc_48357:              ; CODE XREF: movefun+3BDj
        mov leveldata[si], 0FFFFh
        sub si, 2
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 22h ; '"'
        mov word ptr [si+18ACh], 0FFFFh
        return;
// ; ---------------------------------------------------------------------------

loc_48371:              ; CODE XREF: movefun+3B6j
                    ; movefun+3C5j
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_48378:              ; CODE XREF: movefun+DDj
        xor bh, bh
        and bl, 7
        shl bl, 1
        mov di, [si+6155h]
        push    si
        mov si, 12A4h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48391:              ; CODE XREF: movefun+40Bj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48391
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 62h ; 'b'
        jge short loc_483AF
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_483AF:              ; CODE XREF: movefun+418j
        cmp word ptr [si+18AEh], 0
        jnz short loc_483DF
        cmp word ptr [si+1836h], 0
        jz  short loc_483C5
        cmp word ptr [si+1836h], 8888h
        jnz short loc_483DF

loc_483C5:              ; CODE XREF: movefun+42Bj
        mov word ptr leveldata[si], 0FFFFh
        add si, 2
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 32h ; '2'

loc_483D8:
        mov word ptr [si+18ACh], 0FFFFh

locret_483DE:
        return;
// ; ---------------------------------------------------------------------------

loc_483DF:              ; CODE XREF: movefun+424j
                    ; movefun+433j
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_483E6:              ; CODE XREF: movefun+E4j
        cmp word ptr [si+18ACh], 0
        jz  short loc_483F6
        cmp word ptr [si+18ACh], 9999h
        jz  short loc_483F6
        return;
// ; ---------------------------------------------------------------------------

loc_483F6:              ; CODE XREF: movefun+45Bj
                    ; movefun+463j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        jmp loc_48078
movefun   endp


; =============== S U B R O U T I N E =======================================


movefun2  proc near       ; DATA XREF: data:1612o
        cmp byte ptr leveldata[si], 4
        jz  short loc_48410
        return;
// ; ---------------------------------------------------------------------------

loc_48410:              ; CODE XREF: movefun2+5j
        mov ax, [si+1834h]
        cmp ax, 4
        jz  short loc_4841B
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_4841B:              ; CODE XREF: movefun2+Fj
        mov ax, [si+18ACh]
        cmp ax, 0
        jz  short loc_48463
        cmp ax, 1
        jz  short loc_48434
        cmp ax, 4
        jz  short loc_48434
        cmp ax, 5
        jz  short loc_48434
        return;
// ; ---------------------------------------------------------------------------

loc_48434:              ; CODE XREF: movefun2+1Fj
                    ; movefun2+24j ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_4846A
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_4846A
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_4846A

loc_4844B:              ; CODE XREF: movefun2+69j
                    ; movefun2+1C7j
        cmp word ptr [si+18AEh], 0
        jz  short loc_48480

loc_48452:
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_48480
        cmp word ptr [si+18AEh], 0AAAAh
        jz  short loc_48480
        return;
// ; ---------------------------------------------------------------------------

loc_48463:              ; CODE XREF: movefun2+1Aj
        mov byte ptr [si+1835h], 40h ; '@'
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_4846A:              ; CODE XREF: movefun2+31j
                    ; movefun2+39j ...
        cmp word ptr [si+1832h], 0
        jz  short loc_48473
        jmp short loc_4844B
// ; ---------------------------------------------------------------------------

loc_48473:              ; CODE XREF: movefun2+67j
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_48480:              ; CODE XREF: movefun2+48j
                    ; movefun2+50j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_48488
        return;
// ; ---------------------------------------------------------------------------

loc_48488:              ; CODE XREF: movefun2+7Dj
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48495:              ; CODE XREF: movefun2+11j
                    ; movefun2+60j ...
        mov bl, [si+1835h]
        xor bh, bh
        al = bl
        and al, 0F0h
        cmp al, 10h
        jnz short loc_484A5
        jmp short loc_484D0
// ; ---------------------------------------------------------------------------

loc_484A5:              ; CODE XREF: movefun2+99j
        cmp al, 20h ; ' '
        jnz short loc_484AC
        jmp loc_4861B
// ; ---------------------------------------------------------------------------

loc_484AC:              ; CODE XREF: movefun2+9Fj
        cmp al, 30h ; '0'
        jnz short loc_484B3
        jmp loc_48677
// ; ---------------------------------------------------------------------------

loc_484B3:              ; CODE XREF: movefun2+A6j
        cmp al, 40h ; '@'
        jnz short loc_484BA
        jmp loc_486D2
// ; ---------------------------------------------------------------------------

loc_484BA:              ; CODE XREF: movefun2+ADj
        cmp al, 50h ; 'P'
        jnz short loc_484C1
        jmp loc_48700
// ; ---------------------------------------------------------------------------

loc_484C1:              ; CODE XREF: movefun2+B4j
        cmp al, 60h ; '`'
        jnz short loc_484C8
        jmp loc_4876E
// ; ---------------------------------------------------------------------------

loc_484C8:              ; CODE XREF: movefun2+BBj
        cmp al, 70h ; 'p'

loc_484CA:
        jnz short locret_484CF

loc_484CC:
        jmp loc_487DC
// ; ---------------------------------------------------------------------------

locret_484CF:               ; CODE XREF: movefun2:loc_484CAj
        return;
// ; ---------------------------------------------------------------------------

loc_484D0:              ; CODE XREF: movefun2+9Bj
                    ; movefun2+3F3j
        xor bh, bh
        shl bl, 1
        shl bl, 1
        and bl, 1Fh
        mov di, [si+60DDh]
        add di, [bx+6C95h]
        push    si
        mov si, word_515C4
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_484EE:              ; CODE XREF: movefun2+EEj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_484EE
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 16h
        jnz short loc_48513
        mov [si+1835h], bl
        sub si, 78h ; 'x'
        call    sub_48957
        add si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_48513:              ; CODE XREF: movefun2+FBj
        cmp bl, 18h
        jge short loc_4851D
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4851D:              ; CODE XREF: movefun2+10Ej
        mov byte ptr [si+1835h], 0
        cmp word ptr [si+18ACh], 0
        jnz short loc_4852C
        jmp loc_485BB
// ; ---------------------------------------------------------------------------

loc_4852C:              ; CODE XREF: movefun2+11Fj
        cmp word ptr [si+18ACh], 9999h
        jnz short loc_48537
        jmp loc_485BB
// ; ---------------------------------------------------------------------------

loc_48537:              ; CODE XREF: movefun2+12Aj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_48541
        jmp loc_485F2
// ; ---------------------------------------------------------------------------

loc_48541:              ; CODE XREF: movefun2+134j
        cmp word ptr [si+18ACh], 14h
        jnz short loc_4854B
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_4854B:              ; CODE XREF: movefun2+13Ej
        cmp byte ptr [si+18ACh], 11h
        jnz short loc_48555
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48555:              ; CODE XREF: movefun2+148j
        cmp byte ptr [si+18ACh], 18h
        jnz short loc_4855F
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_4855F:              ; CODE XREF: movefun2+152j
        cmp word ptr [si+18ACh], 12h
        jnz short loc_48569
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48569:              ; CODE XREF: movefun2+15Cj
        cmp word ptr [si+18ACh], 8
        jnz short loc_48573
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48573:              ; CODE XREF: movefun2+166j
        call    sound7
        cmp word ptr [si+18ACh], 1
        jz  short loc_4858C
        cmp word ptr [si+18ACh], 4
        jz  short loc_4858C
        cmp word ptr [si+18ACh], 5
        jz  short loc_4858C
        return;
// ; ---------------------------------------------------------------------------

loc_4858C:              ; CODE XREF: movefun2+173j
                    ; movefun2+17Aj ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_485C8
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_485C8
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_485C8
        cmp word ptr [si+18AEh], 0
        jz  short loc_485DE
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_485DE
        cmp word ptr [si+18AEh], 0AAAAh
        jz  short loc_485DE
        return;
// ; ---------------------------------------------------------------------------

loc_485BB:              ; CODE XREF: movefun2+121j
                    ; movefun2+12Cj
        mov word ptr leveldata[si], 7004h
        mov word ptr [si+18ACh], 9999h
        return;
// ; ---------------------------------------------------------------------------

loc_485C8:              ; CODE XREF: movefun2+189j
                    ; movefun2+191j ...
        cmp word ptr [si+1832h], 0
        jz  short loc_485D2
        jmp loc_4844B
// ; ---------------------------------------------------------------------------

loc_485D2:              ; CODE XREF: movefun2+1C5j
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_485DE:              ; CODE XREF: movefun2+1A0j
                    ; movefun2+1A8j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_485E6
        return;
// ; ---------------------------------------------------------------------------

loc_485E6:              ; CODE XREF: movefun2+1DBj
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_485F2:              ; CODE XREF: movefun2+136j
        mov bl, [si+18ADh]
        cmp bl, 0Eh
        jz  short locret_4861A
        cmp bl, 0Fh
        jz  short locret_4861A
        cmp bl, 28h ; '('
        jz  short locret_4861A
        cmp bl, 29h ; ')'
        jz  short locret_4861A
        cmp bl, 25h ; '%'
        jz  short locret_4861A
        cmp bl, 26h ; '&'
        jz  short locret_4861A

loc_48614:              ; CODE XREF: movefun2+140j
                    ; movefun2+14Aj ...
        add si, 78h ; 'x'
        call    sub_4A61F

locret_4861A:               ; CODE XREF: movefun2+1F1j
                    ; movefun2+1F6j ...
        return;
// ; ---------------------------------------------------------------------------

loc_4861B:              ; CODE XREF: movefun2+A1j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6155h]
        push    si
        mov si, 12B6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48634:              ; CODE XREF: movefun2+236j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48634
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 24h ; '$'
        jnz short loc_48653
        mov word ptr [si+1836h], 0AAAAh

loc_48653:              ; CODE XREF: movefun2+243j
        cmp bl, 26h ; '&'
        jnz short loc_48666
        mov [si+1835h], bl
        add si, 2
        call    sub_48957
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_48666:              ; CODE XREF: movefun2+24Ej
        cmp bl, 28h ; '('
        jge short loc_48670
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_48670:              ; CODE XREF: movefun2+261j
        mov word ptr leveldata[si], 7004h
        return;
// ; ---------------------------------------------------------------------------

loc_48677:              ; CODE XREF: movefun2+A8j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6153h]
        push    si
        mov si, 12C6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48690:              ; CODE XREF: movefun2+292j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48690
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 34h ; '4'
        jnz short loc_486AF
        mov word ptr [si+1832h], 0AAAAh

loc_486AF:              ; CODE XREF: movefun2+29Fj
        cmp bl, 36h ; '6'
        jnz short loc_486C1
        mov [si+1835h], bl
        sub si, 2
        call    sub_48957
        add si, 2

loc_486C1:              ; CODE XREF: movefun2+2AAj
        cmp bl, 38h ; '8'
        jge short loc_486CB
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_486CB:              ; CODE XREF: movefun2+2BCj
        mov word ptr leveldata[si], 7004h
        return;
// ; ---------------------------------------------------------------------------

loc_486D2:              ; CODE XREF: movefun2+AFj
        inc bl
        cmp bl, 42h ; 'B'
        jge short loc_486DE
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_486DE:              ; CODE XREF: movefun2+2CFj
        cmp word ptr [si+18ACh], 0
        jz  short loc_486EC
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_486EC:              ; CODE XREF: movefun2+2DBj
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 10h
        return;
// ; ---------------------------------------------------------------------------

loc_48700:              ; CODE XREF: movefun2+B6j
        xor bh, bh
        and bl, 0Fh
        shl bl, 1
        mov di, [si+6153h]
        push    si
        mov si, 12B6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48719:              ; CODE XREF: movefun2+31Bj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48719
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 52h ; 'R'
        jge short loc_48737
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_48737:              ; CODE XREF: movefun2+328j
        cmp word ptr [si+18AAh], 0
        jnz short loc_48767
        cmp word ptr [si+1832h], 0
        jz  short loc_4874D
        cmp word ptr [si+1832h], 8888h
        jnz short loc_48767

loc_4874D:              ; CODE XREF: movefun2+33Bj
        mov word ptr leveldata[si], 0FFFFh
        sub si, 2
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 22h ; '"'
        mov word ptr [si+18ACh], 9999h
        return;
// ; ---------------------------------------------------------------------------

loc_48767:              ; CODE XREF: movefun2+334j
                    ; movefun2+343j
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4876E:              ; CODE XREF: movefun2+BDj
        xor bh, bh
        and bl, 7
        shl bl, 1
        mov di, [si+6155h]
        push    si
        mov si, 12C6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48787:              ; CODE XREF: movefun2+389j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48787
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 62h ; 'b'
        jge short loc_487A5
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_487A5:              ; CODE XREF: movefun2+396j
        cmp word ptr [si+18AEh], 0
        jnz short loc_487D5
        cmp word ptr [si+1836h], 0
        jz  short loc_487BB
        cmp word ptr [si+1836h], 8888h
        jnz short loc_487D5

loc_487BB:              ; CODE XREF: movefun2+3A9j
        mov word ptr leveldata[si], 0FFFFh
        add si, 2
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 32h ; '2'
        mov word ptr [si+18ACh], 9999h
        return;
// ; ---------------------------------------------------------------------------

loc_487D5:              ; CODE XREF: movefun2+3A2j
                    ; movefun2+3B1j
        dec bl
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_487DC:              ; CODE XREF: movefun2:loc_484CCj
        cmp word ptr [si+18ACh], 0
        jz  short loc_487EC
        cmp word ptr [si+18ACh], 9999h
        jz  short loc_487EC
        return;
// ; ---------------------------------------------------------------------------

loc_487EC:              ; CODE XREF: movefun2+3D9j
                    ; movefun2+3E1j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1004h
        jmp loc_484D0
movefun2  endp


; =============== S U B R O U T I N E =======================================


sub_487FE   proc near       ; CODE XREF: update?+E0Cp update?+E2Ap ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4880B
        mov word ptr leveldata[si], 0

loc_4880B:              ; CODE XREF: sub_487FE+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_48835
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_48835
        cmp word ptr [si+17BCh], 1
        jz  short loc_48829
        cmp word ptr [si+17BCh], 4
        jz  short loc_4882F
        return;
// ; ---------------------------------------------------------------------------

loc_48829:              ; CODE XREF: sub_487FE+21j
        mov byte ptr [si+17BDh], 40h ; '@'
        return;
// ; ---------------------------------------------------------------------------

loc_4882F:              ; CODE XREF: sub_487FE+28j
        mov byte ptr [si+17BDh], 40h ; '@'
        return;
// ; ---------------------------------------------------------------------------

loc_48835:              ; CODE XREF: sub_487FE+12j
                    ; sub_487FE+1Aj
        cmp word ptr [si+17BAh], 1
        jz  short loc_48852
        cmp word ptr [si+17BAh], 4
        jz  short loc_48897

loc_48843:              ; CODE XREF: sub_487FE+69j
                    ; sub_487FE+AEj
        cmp word ptr [si+17BEh], 1
        jz  short loc_48875
        cmp word ptr [si+17BEh], 4
        jz  short loc_488BA
        return;
// ; ---------------------------------------------------------------------------

loc_48852:              ; CODE XREF: sub_487FE+3Cj
        cmp word ptr [si+1832h], 1
        jz  short loc_48869
        cmp word ptr [si+1832h], 4
        jz  short loc_48869
        cmp word ptr [si+1832h], 5
        jz  short loc_48869
        jmp short loc_48843
// ; ---------------------------------------------------------------------------

loc_48869:              ; CODE XREF: sub_487FE+59j
                    ; sub_487FE+60j ...
        mov byte ptr [si+17BBh], 60h ; '`'
        mov word ptr [si+17BCh], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_48875:              ; CODE XREF: sub_487FE+4Aj
        cmp word ptr [si+1836h], 1
        jz  short loc_4888B
        cmp word ptr [si+1836h], 4
        jz  short loc_4888B
        cmp word ptr [si+1836h], 5
        jz  short loc_4888B
        return;
// ; ---------------------------------------------------------------------------

loc_4888B:              ; CODE XREF: sub_487FE+7Cj
                    ; sub_487FE+83j ...
        mov byte ptr [si+17BFh], 50h ; 'P'
        mov word ptr [si+17BCh], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_48897:              ; CODE XREF: sub_487FE+43j
        cmp word ptr [si+1832h], 1
        jz  short loc_488AE
        cmp word ptr [si+1832h], 4
        jz  short loc_488AE
        cmp word ptr [si+1832h], 5
        jz  short loc_488AE
        jmp short loc_48843
// ; ---------------------------------------------------------------------------

loc_488AE:              ; CODE XREF: sub_487FE+9Ej
                    ; sub_487FE+A5j ...
        mov byte ptr [si+17BBh], 60h ; '`'
        mov word ptr [si+17BCh], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_488BA:              ; CODE XREF: sub_487FE+51j
        cmp word ptr [si+1836h], 1
        jz  short loc_488D0
        cmp word ptr [si+1836h], 4
        jz  short loc_488D0
        cmp word ptr [si+1836h], 5
        jz  short loc_488D0
        return;
// ; ---------------------------------------------------------------------------

loc_488D0:              ; CODE XREF: sub_487FE+C1j
                    ; sub_487FE+C8j ...
        mov byte ptr [si+17BFh], 50h ; 'P'
        mov word ptr [si+17BCh], 8888h
        return;
sub_487FE   endp


; =============== S U B R O U T I N E =======================================


sub_488DC   proc near       ; CODE XREF: movefun+124p
                    ; movefun+2C6p ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_488E9
        mov word ptr leveldata[si], 0

loc_488E9:              ; CODE XREF: sub_488DC+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_48901
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_488F9
        return;
// ; ---------------------------------------------------------------------------

loc_488F9:              ; CODE XREF: sub_488DC+1Aj
        cmp byte ptr [si+1744h], 4
        jz  short loc_48901
        return;
// ; ---------------------------------------------------------------------------

loc_48901:              ; CODE XREF: sub_488DC+12j
                    ; sub_488DC+22j
        cmp word ptr [si+17BAh], 1
        jz  short loc_48910

loc_48908:              ; CODE XREF: sub_488DC+49j
        cmp word ptr [si+17BEh], 1
        jz  short loc_48934
        return;
// ; ---------------------------------------------------------------------------

loc_48910:              ; CODE XREF: sub_488DC+2Aj
        cmp word ptr [si+1832h], 1
        jz  short loc_48927
        cmp word ptr [si+1832h], 4
        jz  short loc_48927
        cmp word ptr [si+1832h], 5
        jz  short loc_48927
        jmp short loc_48908
// ; ---------------------------------------------------------------------------

loc_48927:              ; CODE XREF: sub_488DC+39j
                    ; sub_488DC+40j ...
        mov word ptr [si+17BAh], 6001h
        mov word ptr [si+17BCh], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_48934:              ; CODE XREF: sub_488DC+31j
        cmp word ptr [si+1836h], 1
        jz  short loc_4894A
        cmp word ptr [si+1836h], 4
        jz  short loc_4894A
        cmp word ptr [si+1836h], 5
        jz  short loc_4894A
        return;
// ; ---------------------------------------------------------------------------

loc_4894A:              ; CODE XREF: sub_488DC+5Dj
                    ; sub_488DC+64j ...
        mov word ptr [si+17BEh], 5001h
        mov word ptr [si+17BCh], 8888h
        return;
sub_488DC   endp


; =============== S U B R O U T I N E =======================================


sub_48957   proc near       ; CODE XREF: movefun2+104p
                    ; movefun2+257p ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_48964
        mov word ptr leveldata[si], 0

loc_48964:              ; CODE XREF: sub_48957+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4897C
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_48974
        return;
// ; ---------------------------------------------------------------------------

loc_48974:              ; CODE XREF: sub_48957+1Aj
        cmp byte ptr [si+1744h], 1
        jz  short loc_4897C
        return;
// ; ---------------------------------------------------------------------------

loc_4897C:              ; CODE XREF: sub_48957+12j
                    ; sub_48957+22j
        cmp word ptr [si+17BAh], 4
        jz  short loc_4898B

loc_48983:              ; CODE XREF: sub_48957+49j
        cmp word ptr [si+17BEh], 4
        jz  short loc_489AF
        return;
// ; ---------------------------------------------------------------------------

loc_4898B:              ; CODE XREF: sub_48957+2Aj
        cmp word ptr [si+1832h], 1
        jz  short loc_489A2
        cmp word ptr [si+1832h], 4
        jz  short loc_489A2
        cmp word ptr [si+1832h], 5
        jz  short loc_489A2
        jmp short loc_48983
// ; ---------------------------------------------------------------------------

loc_489A2:              ; CODE XREF: sub_48957+39j
                    ; sub_48957+40j ...
        mov word ptr [si+17BAh], 6004h
        mov word ptr [si+17BCh], 8888h
        return;
// ; ---------------------------------------------------------------------------

loc_489AF:              ; CODE XREF: sub_48957+31j
        cmp word ptr [si+1836h], 1
        jz  short loc_489C5
        cmp word ptr [si+1836h], 4
        jz  short loc_489C5
        cmp word ptr [si+1836h], 5
        jz  short loc_489C5
        return;
// ; ---------------------------------------------------------------------------

loc_489C5:              ; CODE XREF: sub_48957+5Dj
                    ; sub_48957+64j ...
        mov word ptr [si+17BEh], 5004h
        mov word ptr [si+17BCh], 8888h
        return;
sub_48957   endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

// ; =============== S U B R O U T I N E =======================================

// ; ax contains a status code if a DOS function fails.
void writeexitmessage() //    proc near       ; CODE XREF: loadScreen2-7CFp
{
        push    es
        mov bx, ax
        mov ax, seg doserrors
        mov es, ax
        assume es:doserrors
        mov di, 0
        al = 0

        ; find message #bx
keeplooking:              ; CODE XREF: writeexitmessage+13j
        mov cx, 1000
        repne scasb             ; look for zero term
        dec bx
        jnz short keeplooking

writechar:              ; CODE XREF: writeexitmessage+23j
        mov ah, 0Eh
        al = es:[di]
        cmp al, 0
        jz  short write_crlf
        inc di
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        jmp short writechar
// ; ---------------------------------------------------------------------------

write_crlf:              ; CODE XREF: writeexitmessage+1Cj
        mov ah, 0Eh
        al = 0Ah
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        mov ah, 0Eh
        al = 0Dh
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        pop es
        assume es:nothing
        return;
}

// ; ---------------------------------------------------------------------------
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +

*/
void sub_48A20() //   proc near       ; CODE XREF: start+32Fp
                // ; runLevel:notFunctionKeyp ...
{
    // 01ED:1DBD
    word_510BC = word_510C3;
    word_510BE = word_510C5;
    ax = 0;
    word_510CB = 0;
    word_510D1 = 0;
    word_51974 = 0;
    word_51978 = 0;
    byte_5195C = 0;
    byte_5197C = 0;
    word_510CD = 0;
    word_510B7 = 0xFFFF;
    byte_510B9 = 0xFF; // 255
    byte_510AE = 1;
    byte_510AF = 0;
    byte_510B0 = 0;
    byte_510B1 = 0;
    byte_510B2 = 0;
    byte_510B4 = 0;
    byte_510B5 = 0;
    byte_510B6 = 0;
    byte_510C0 = 0;
    byte_5196A = 0x7F; // 127
    byte_5196B = 0;
    word_5195D = 0;
//    mov byte ptr word_510C1, 1
//    mov byte ptr word_510C1+1, 0
    word_510C1 = 0x0001;
    byte_510D7 = 0;
    gNumberOfDotsToShiftDataLeft = 0;
    word_510D9 &= 0xFF00; // mov byte ptr word_510D9, 0
    byte_510DB = 0;
    word_510DC = 0;
}

void runLevel() //    proc near       ; CODE XREF: start+35Cp
{
/*
    cmp byte_510DE, 0
    jz  short loc_48ACE
    mov byte_5A19C, 1
    mov byte_510BA, 0
    jmp short loc_48AD8
// ; ---------------------------------------------------------------------------

loc_48ACE:              ; CODE XREF: runLevel+5j
    mov byte_5A19C, 0
    mov byte_510BA, 1

loc_48AD8:              ; CODE XREF: runLevel+11j
    cmp byte_5A2F8, 1
    jnz short loc_48B09

loc_48ADF:              ; CODE XREF: runLevel+BAj
    mov byte_5A2F8, 0
    call    sub_4FDFD

isFunctionKey:              ; CODE XREF: runLevel+35j
    al = keyPressed
    cmp al, 3Bh ; ';'   ; F1
    jb  short notFunctionKey
    cmp al, 44h ; 'D'   ; F10
    jbe short isFunctionKey

notFunctionKey:             ; CODE XREF: runLevel+31j
    call    sub_48A20
    cmp isMusicEnabled, 0
    jnz short loc_48AFF
    call    sound3

loc_48AFF:              ; CODE XREF: runLevel+3Fj
    mov byte_5A19C, 0
    mov byte_510BA, 1

loc_48B09:              ; CODE XREF: runLevel+22j
    mov byte_510DB, 0
    mov byte_5A323, 0
    mov word_510A2, 1

gamelooprep:                ; CODE XREF: runLevel+33Cj
                ; runLevel+345j
    cmp byte_510DE, 0
    jnz short loc_48B23
    call    sub_48E59

loc_48B23:              ; CODE XREF: runLevel+63j
    call    getMouseStatus
    cmp bx, 2
    jnz short loc_48B38
    cmp word_51978, 0
    jg  short loc_48B38
    mov word_510D1, 1

loc_48B38:              ; CODE XREF: runLevel+6Ej runLevel+75j
    cmp word_51970, 0
    jz  short loc_48B6B
    cmp byte_59B7A, 0
    jz  short loc_48B4A
    dec byte_59B7A

loc_48B4A:              ; CODE XREF: runLevel+89j
    cmp byte_51999, 0
    jnz short loc_48B6B
    cmp bx, 1
    jnz short loc_48B6B
    cmp byte_59B7A, 0
    jnz short loc_48B6B
    mov byte_59B7A, 0Ah
    call    levelScanThing
    call    sub_48F6D
    call    sub_4A2E6

loc_48B6B:              ; CODE XREF: runLevel+82j runLevel+94j ...
    call    sub_4955B
    cmp byte_5A2F8, 1
    jnz short loc_48B78
    jmp loc_48ADF
// ; ---------------------------------------------------------------------------

loc_48B78:              ; CODE XREF: runLevel+B8j
    cmp word ptr flashingbackgroundon, 0
    jz  short noFlashing
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    al = 35h ; '5'
    out dx, al
    out dx, al
    out dx, al

noFlashing:              ; CODE XREF: runLevel+C2j
    call    gameloop?
    cmp word ptr flashingbackgroundon, 0
    jz  short noFlashing2
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    al = 21h ; '!'
    out dx, al
    out dx, al
    out dx, al

noFlashing2:              ; CODE XREF: runLevel+D8j
    call    sub_4FDFD
    call    sub_4FD65
    cmp word ptr flashingbackgroundon, 0
    jz  short noFlashing3
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    al = 2Dh ; '-'
    out dx, al
    al = 21h ; '!'
    out dx, al
    al = 0Fh
    out dx, al

noFlashing3:              ; CODE XREF: runLevel+F1j
    call    sub_4A910
    call    sub_4A5E0
    call    sub_49EBE
    mov ax, word_5195F
    and al, 7
    mov gNumberOfDotsToShiftDataLeft, al
    test    speed3, 40h
    jnz short loc_48BED
    test    speed3, 80h
    jz  short loc_48BED
    al = gameSpeed
    xchg    al, speed3
    cmp al, 0BFh ; '?'
    jz  short loc_48BED
    and al, 0Fh
    mov gameSpeed, al

loc_48BED:              ; CODE XREF: runLevel+119j
                ; runLevel+120j ...
    cmp speed1, 0
    jnz short loc_48BF7
    jmp loc_48C86
// ; ---------------------------------------------------------------------------

loc_48BF7:              ; CODE XREF: runLevel+137j
    inc byte_59B94
    cmp byte_59B96, 0Ah
    jnb short loc_48C05
    jmp loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C05:              ; CODE XREF: runLevel+145j
    mov byte_59B96, 0
    cmp byte_59B94, 8
    ja  short loc_48C1A
    cmp byte_59B94, 6
    jb  short loc_48C1A
    cmp al, al

loc_48C1A:              ; CODE XREF: runLevel+154j
                ; runLevel+15Bj
    mov byte_59B94, 0
    ja  short loc_48C79
    jb  short loc_48C5F
    dec speed1
    cmp speed1, 0FEh ; '?'
    ja  short loc_48C86
    and speed3, 0BFh
    mov speed1, 0
    cmp gameSpeed, 4
    jz  short loc_48C4F
    cmp gameSpeed, 6
    jz  short loc_48C6C
    cmp gameSpeed, 9
    jz  short loc_48C4F
    jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C4F:              ; CODE XREF: runLevel+182j
                ; runLevel+190j
    cmp gameSpeed, 0Ah
    jb  short loc_48C59
    jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48C59:              ; CODE XREF: runLevel+199j
                ; runLevel+1A9j
    inc gameSpeed
    jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C5F:              ; CODE XREF: runLevel+166j
    cmp gameSpeed, 0Ah
    jb  short loc_48C59
    dec speed1
    jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C6C:              ; CODE XREF: runLevel+189j
    cmp gameSpeed, 0
    jz  short loc_48C86

loc_48C73:              ; CODE XREF: runLevel+1C3j
    dec gameSpeed
    jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C79:              ; CODE XREF: runLevel+164j
    cmp gameSpeed, 0
    ja  short loc_48C73
    dec speed1
    jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48C86:              ; CODE XREF: runLevel+139j
                ; runLevel+147j ...
    cmp fastMode, 1
    jnz short isNotFastMode4
    jmp loc_48D59
// ; ---------------------------------------------------------------------------

isNotFastMode4:              ; CODE XREF: runLevel+1D0j
    cmp gameSpeed, 0Ah
    jb  short loc_48C9A
    jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48C9A:              ; CODE XREF: runLevel+1DAj
    mov cx, 6
    sub cl, gameSpeed
    jg  short loc_48CBD
    al = gameSpeed
    inc byte_59B95
    sub al, 5
    cmp al, byte_59B95
    jb  short loc_48CB5
    jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48CB5:              ; CODE XREF: runLevel+1F5j
    mov byte_59B95, 0
    mov cx, 1

loc_48CBD:              ; CODE XREF: runLevel+1E6j
    push    bx

loc_48CBE:              ; CODE XREF: runLevel+29Aj
    mov dx, word_5195F
    sub dx, word_59B90
    cmp dx, 10h
    jg  short loc_48D38
    cmp dx, 0FFF0h
    jl  short loc_48D38
    mov ax, word_51961
    sub ax, word_59B92
    cmp ax, 10h
    jg  short loc_48D38
    cmp ax, 0FFF0h
    jl  short loc_48D38
    push(cx);
    inc cx
    idiv    cl
    shl ah, 1
    jnb short loc_48CF3
    neg ah
    cmp ah, cl
    jb  short loc_48CF9
    dec al
    jmp short loc_48CF9
// ; ---------------------------------------------------------------------------

loc_48CF3:              ; CODE XREF: runLevel+22Cj
    cmp ah, cl
    jb  short loc_48CF9
    al++;

loc_48CF9:              ; CODE XREF: runLevel+232j
                ; runLevel+236j ...
    cbw
    add word_59B92, ax
    mov ax, dx
    idiv    cl
    shl ah, 1
    jnb short loc_48D10
    neg ah
    cmp ah, cl
    jb  short loc_48D16
    dec al
    jmp short loc_48D16
// ; ---------------------------------------------------------------------------

loc_48D10:              ; CODE XREF: runLevel+249j
    cmp ah, cl
    jb  short loc_48D16
    al++;

loc_48D16:              ; CODE XREF: runLevel+24Fj
                ; runLevel+253j ...
    cbw
    add word_59B90, ax
    mov ax, word_59B90
    mov bx, ax
    and al, 7
    mov gNumberOfDotsToShiftDataLeft, al
    mov cl, 3
    shr bx, cl
    mov ax, word_59B92
    mov cx, 7Ah ; 'z'
    mul cx
    add bx, ax
    add bx, 4D34h
    pop(cx);

loc_48D38:              ; CODE XREF: runLevel+20Ej
                ; runLevel+213j ...
    mov dx, 3D4h
    al = 0Dh
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (low)
    inc dx
    al = bl
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 0Ch
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (high)
    inc dx
    al = bh
    out dx, al      ; Video: CRT controller internal registers
    call    videoloop
    call    loopForVSync
    dec cx
    jz  short loc_48D58
    jmp loc_48CBE
// ; ---------------------------------------------------------------------------

loc_48D58:              ; CODE XREF: runLevel+298j
    pop bx

loc_48D59:              ; CODE XREF: runLevel+19Bj
                ; runLevel+1D2j ...
    mov ax, word_51961
    mov word_59B92, ax
    mov ax, word_5195F
    mov word_59B90, ax
    and al, 7
    mov gNumberOfDotsToShiftDataLeft, al
    mov dx, 3D4h
    al = 0Dh
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (low)
    inc dx
    al = bl
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 0Ch
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (high)
    inc dx
    al = bh
    out dx, al      ; Video: CRT controller internal registers
    mov cx, word_5195F
    mov ah, cl
    and ah, 7
    cmp word ptr flashingbackgroundon, 0
    jz  short noFlashing4
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    al = 3Fh ; '?'
    out dx, al
    out dx, al
    out dx, al

noFlashing4:              ; CODE XREF: runLevel+2D1j
    mov gNumberOfDotsToShiftDataLeft, ah
    cmp fastMode, 1
    jz  short isFastMode2
    call    videoloop

isFastMode2:              ; CODE XREF: runLevel+2E8j
    cmp word_51A07, 1
    jbe short loc_48DB2
    call    sound9

loc_48DB2:              ; CODE XREF: runLevel+2F2j
    mov cx, word_51A07

loc_48DB6:              ; CODE XREF: runLevel+310j
    dec cx
    jz  short loc_48DCD
    cmp fastMode, 1
    jz  short isFastMode3
    call    loopForVSync
    call    videoloop

isFastMode3:              ; CODE XREF: runLevel+303j
    push(cx);
    call    sub_4955B
    pop(cx);
    jmp short loc_48DB6
// ; ---------------------------------------------------------------------------

loc_48DCD:              ; CODE XREF: runLevel+2FCj
    cmp word ptr flashingbackgroundon, 0
    jz  short noFlashing5
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    out dx, al
    out dx, al
    out dx, al

noFlashing5:              ; CODE XREF: runLevel+317j
    cmp word_5197A, 0
    jnz short loc_48E03
    inc word_5195D
    cmp word_51974, 1
    jz  short loc_48E03
    cmp word_51978, 0
    jnz short loc_48DFA
    jmp gamelooprep
// ; ---------------------------------------------------------------------------

loc_48DFA:              ; CODE XREF: runLevel+33Aj
    dec word_51978
    jz  short loc_48E03
    jmp gamelooprep
// ; ---------------------------------------------------------------------------

loc_48E03:              ; CODE XREF: runLevel+328j
                ; runLevel+333j ...
    mov word_510A2, 0
    cmp byte_510E3, 0
    jz  short loc_48E13
    call    somethingspsig

loc_48E13:              ; CODE XREF: runLevel+353j
    cmp byte_5A2F9, 0
    mov byte_5A2F9, 0
    jnz short loc_48E30
    cmp byte_510B3, 0
    jz  short loc_48E30
    cmp byte_5A323, 0
    jnz short loc_48E30
    call    sub_4A95F

loc_48E30:              ; CODE XREF: runLevel+362j
                ; runLevel+369j ...
    mov word_51A01, 0
    mov word_51963, 0
    mov word_51965, 0
    mov word ptr flashingbackgroundon, 0
    mov word_51A07, 1
    mov dx, 3C8h
    xor al, al
    out dx, al
    inc dx
    out dx, al
    out dx, al
    out dx, al
    return;
    */
}

void sub_48E59() //   proc near       ; CODE XREF: waitForKeyMouseOrJoystick:loc_47EB8p
//                    ; waitForKeyMouseOrJoystick+4Dp ...
{
    ax = 0;
    bl = 0;
    if (isJoystickEnabled != 0)
    {
//loc_48E67:              ; CODE XREF: sub_48E59+9j
        word_50942 = ax;
        word_50944 = ax;
        word_5094F = ax;
        word_50951 = ax;
        byte_50953 = al;
        byte_50954 = al;
        al = byte_50946;
        if (al != 0)
        {
//loc_48E83:              ; CODE XREF: sub_48E59+25j
            ah = 1;
            waitForJoystickKey();

            if (1 /* result of waitForJoystickKey causes CF = 0 */) // jnb short loc_48E8D
            {
//loc_48E8D:              ; CODE XREF: sub_48E59+2Fj
                word_50942 = ax;
                dx = word_5094B;
                ax = ax * dx;
                ax = ax << 1;
                dx = dx + cf; // adc dx, 0
                ax = dx;
                if (ax > 16)
                {
                    ax = 16;
                }

//loc_48EA5:              ; CODE XREF: sub_48E59+47j
                if (byte_519F7 == 0)
                {
                    if (byte_519F6 != 0)
                    {
                        ax = 16;
                    }
                }
                else
                {
                    ax = 1;
                }
//loc_48EBB:              ; CODE XREF: sub_48E59+56j
//                ; sub_48E59+5Dj
                word_5094F = ax;
                ah = 2;
                waitForJoystickKey();
                if (1 /* result of waitForJoystickKey causes CF = 0 */) // jnb short loc_48EC8
                {
//loc_48EC8:              ; CODE XREF: sub_48E59+6Aj
                    word_50944 = ax;
                    dx = word_5094D;
                    ax = ax * dx;
                    ax = ax << 1;
                    dx = dx + cf; // adc dx, 0
                    ax = dx;
                    if (ax > 16)
                    {
                        ax = 16;
                    }

//loc_48EE0:              ; CODE XREF: sub_48E59+82j
                    if (byte_519F9 == 0)
                    {
                        if (byte_519F8 != 0)
                        {
                            ax = 16;
                        }
                    }
                    else
                    {
                        ax = 1;
                    }
//loc_48EF6:              ; CODE XREF: sub_48E59+91j
//                ; sub_48E59+98j
                    word_50951 = ax;
                    ax = word_50951;
                    cx = 0x11;
                    ax = ax * cx;
                    si = word_5094F;
                    si += ax;
                    si += 0x645;
                    bl = *(uint8_t *)si;
                    if (bl != 0)
                    {
//                    mov dx, 201h
//                    in  al, dx      ; Game I/O port
//                                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
//                                ; bits 4-7: Buttons/Triggers (digital inputs)
                        if ((al & 0x10))
                        {
                            //loc_48F1F:              ; CODE XREF: sub_48E59+BFj
                            if ((al & 0x20))
                            {
                                //loc_48F28:              ; CODE XREF: sub_48E59+C8j
                                if (byte_519F5 == 0)
                                {
                                    //loc_48F34:              ; CODE XREF: sub_48E59+D4j
                                    if (byte_519F4 == 0)
                                    {
                                        bl += 4;
                                    }
                                }
                                else
                                {
                                    bl += 4;
                                }
                            }
                            else
                            {
                                bl += 4;
                            }
                        }
                        else
                        {
                            bl += 4;
                        }
                    }
                    else
                    {
//loc_48F40:              ; CODE XREF: sub_48E59+B7j
//                    mov dx, 201h
//                    in  al, dx      ; Game I/O port
//                                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
//                                ; bits 4-7: Buttons/Triggers (digital inputs)
                        if ((al & 0x10) == 0)
                        {
                            bl += 9;

//loc_48F4C:              ; CODE XREF: sub_48E59+EDj
                            if ((al & 0x20) == 0)
                            {
                                bl += 9;

//loc_48F54:              ; CODE XREF: sub_48E59+F5j
                                if (byte_519F5 != 0)
                                {
                                    bl += 9;

//loc_48F5F:              ; CODE XREF: sub_48E59+100j
                                    if (byte_519F4 != 0)
                                    {
                                        bl += 9;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
//loc_48F68:              ; CODE XREF: sub_48E59+Bj
//                ; sub_48E59+27j ...
    byte_50941 = bl;
    return;
}

void sub_48F6D() //   proc near       ; CODE XREF: start+335p runLevel+AAp ...
{
    // 01ED:230A
    /*
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    inc dx
    al = 1
    out dx, al      ; EGA port: graphics controller data register
     */
//    push    ds
    si = word_5184A;
    ax = es;
    ds = ax;
    di = 0x4D35;
    cx = 8;

    static const uint16_t kMovingBitmapTopLeftCornerX = 288;
    static const uint16_t kMovingBitmapTopLeftCornerY = 388;
    static const uint16_t kMovingBitmapTopRightCornerX = 296;
    static const uint16_t kMovingBitmapTopRightCornerY = 388;
    static const uint16_t kMovingBitmapBottomRightCornerX = 296;
    static const uint16_t kMovingBitmapBottomRightCornerY = 396;
    static const uint16_t kMovingBitmapBottomLeftCornerX = 288;
    static const uint16_t kMovingBitmapBottomLeftCornerY = 396;
    static const uint16_t kMovingBitmapTopEdgeX = 304;
    static const uint16_t kMovingBitmapTopEdgeY = 396;
    static const uint16_t kMovingBitmapRightEdgeX = 304;
    static const uint16_t kMovingBitmapRightEdgeY = 388;
    static const uint16_t kMovingBitmapBottomEdgeX = 304;
    static const uint16_t kMovingBitmapBottomEdgeY = 396;
    static const uint16_t kMovingBitmapLeftEdgeX = 312;
    static const uint16_t kMovingBitmapLeftEdgeY = 388;

    // Draws top-left corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_48F86:              ; CODE XREF: sub_48F6D+20j
            size_t srcAddress = (kMovingBitmapTopLeftCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws top edge
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: sub_48F6D+41j
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_48FA3:              ; CODE XREF: sub_48F6D+38j
            size_t srcAddress = (kMovingBitmapTopEdgeY + y) * kMovingBitmapWidth + kMovingBitmapTopEdgeX + (x % kLevelEdgeSize);
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

/*
    cx = 0x74; // 116

    // ?? no idea
    for (int i = 0; i < 116; ++i)
    {
//loc_48FB3:              ; CODE XREF: sub_48F6D+48j
//        *si = *di; // movsb
        si++; di++;
        si--;
    }
    di -= 0x356; // 854
//    pop ds
    si = word_5184C;
//    push    ds
//    mov ax, es
//    mov ds, ax
    cx = 8;
*/

    // Top-right corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FC8:              ; CODE XREF: sub_48F6D+62j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapTopRightCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Right edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: sub_48F6D+41j
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FA3:              ; CODE XREF: sub_48F6D+38j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y % kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapRightEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapRightEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-right corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FFE:              ; CODE XREF: sub_48F6D+98j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomRightCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom edge
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
//loc_4901C:              ; CODE XREF: sub_48F6D+BDj
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_4901F:              ; CODE XREF: sub_48F6D+B4j
            int srcX = x % kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    /*
    cx = 0x74; // 116

    // ?? no idea
    for (int i = 0; i < 116; ++i)
    {
//loc_4902F:              ; CODE XREF: sub_48F6D+C4j
//        *si = *di; // movsb
        si++; di++;
        si--;
    }
     */

    // Draws left edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_49047:              ; CODE XREF: sub_48F6D+EBj
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_4904A:              ; CODE XREF: sub_48F6D+E4j
            int srcY = y % kLevelEdgeSize;

            size_t srcAddress = (kMovingBitmapLeftEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapLeftEdgeX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-left corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_49067:              ; CODE XREF: sub_48F6D+101j
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomLeftCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    for (int tileY = 1; tileY < kLevelHeight - 1; ++tileY)
    {
        for (int tileX = 1; tileX < kLevelWidth - 1; ++tileX)
        {
            int bitmapTileX = tileX - 1;
            int bitmapTileY = tileY - 1;

            size_t startDstX = kLevelEdgeSize + bitmapTileX * kTileSize;
            size_t startDstY = kLevelEdgeSize + bitmapTileY * kTileSize;
            uint16_t tileValue = gCurrentLevel.tiles[tileY * kLevelWidth + tileX];
            size_t startSrcX = tileValue * kTileSize;

            for (int y = 0; y < kTileSize; ++y)
            {
                for (int x = 0; x < kTileSize; ++x)
                {
                    size_t dstAddress = (startDstY + y) * kLevelBitmapWidth + startDstX + x;
                    size_t srcAddress = (y * kFixedBitmapWidth) + startSrcX + x;
                    gLevelBitmapData[dstAddress] = gFixedDecodedBitmapData[srcAddress];
                }
            }
        }
    }

    // TODO: refactor, this draws the viewport on the screen
    int scrollX = kLevelBitmapWidth - kScreenWidth;
    int scrollY = kLevelBitmapHeight - kScreenHeight;
    for (int y = 0; y < kScreenHeight; ++y)
    {
        for (int x = 0; x < kScreenWidth; ++x)
        {
            gScreenPixels[y * kScreenWidth + x] = gLevelBitmapData[(scrollY + y) * kLevelBitmapWidth + x + scrollX];
        }
    }
    videoloop(); // TODO: Remove
    return;

//    bx = offset leveldata;
    di = 0x4D34; // 19764
    cx = 0x16; // 22
    bx += 0x76; // 118
    di += 0x3CE; // 974

    do
    {
//loc_4909F:              ; CODE XREF: sub_48F6D+18Ej
//      push(cx);
        cx = 0x3A; // 58
        bx += 4;
        di += 4;

        do
        {
//loc_490A9:              ; CODE XREF: sub_48F6D+184j
//            al = bx[0];
            if (al > 0x28) // 40
            {
                al = 0;
            }

//loc_490B1:              ; CODE XREF: sub_48F6D+140j
            bx += 2;
            ah = 0;
            ax = ax << 1;
            si = ax;
//            si += fixedDataBuffer; // 0x3815
//          push(cx);
            cx = 0x10; // 16

            for (int i = 0; i < 16; ++i)
            {
//loc_490C2:              ; CODE XREF: sub_48F6D+17Aj
                ah = 1;

                do
                {
//loc_490C4:              ; CODE XREF: sub_48F6D+175j
    /*
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = ah
    out dx, al      ; EGA port: sequencer data register
     */
    //                al = si[0]
    //              es:[di] = al; // left half of a tile
                    si++;
    //                al = si[0];
    //              es:[di+1] = al; // right half of a tile
                    ah = ah << 1;
                    si += 0x4F; // 79
                }
                while ((ah & 0xF) != 0);

                di += 0x7A; // 122
            }
//          pop(cx);
            di -= 0x79E; // 1950
            cx--;
        }
        while (cx != 0);

//loc_490F3:              ; CODE XREF: sub_48F6D+182j
//      pop(cx);
        di += 0x728; // 1832
        cx--;
    }
    while (cx != 0);

//loc_490FD:              ; CODE XREF: sub_48F6D+18Cj
    /*
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: sequencer data register
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: graphics controller data register
     */
    bx = word_5195F;
    cl = 3;
    bx = bx >> cl;
    ax = word_51961;
    cx = 0x7A; // 122
    ax = ax * cx;
    bx += ax;
    bx += 0x4D34; // 19764
    word_51967 = bx;
    /*
    mov dx, 3D4h
    al = 0Dh
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (low)
    inc dx
    al = bl
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 0Ch
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (high)
    inc dx
    al = bh
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    inc dx
    al = 1
    out dx, al      ; EGA port: graphics controller data register
     */
}

/*
sub_4914A   proc near       ; CODE XREF: sub_4955B+7p
        cmp byte_519C8, 0
        jz  short loc_49159
        dec word_51963
        dec word_51963

loc_49159:              ; CODE XREF: sub_4914A+5j
        cmp byte_519CA, 0
        jz  short loc_49168
        inc word_51963
        inc word_51963

loc_49168:              ; CODE XREF: sub_4914A+14j
        cmp byte_519C5, 0
        jz  short loc_49177
        dec word_51965
        dec word_51965

loc_49177:              ; CODE XREF: sub_4914A+23j
        cmp byte_519CD, 0
        jz  short loc_49186
        inc word_51965
        inc word_51965

loc_49186:              ; CODE XREF: sub_4914A+32j
        cmp byte_519C9, 0
        jz  short loc_49199
        mov word_51963, 0
        mov word_51965, 0

loc_49199:              ; CODE XREF: sub_4914A+41j
        mov bx, 8
        sub bx, word_59B88
        mov ax, word_59B8A
        neg ax
        cmp byte_519CF, 0
        jz  short loc_491B3
        mov word_51963, bx
        mov word_51965, ax

loc_491B3:              ; CODE XREF: sub_4914A+60j
        add bx, 138h
        cmp byte_519C4, 0
        jz  short loc_491C5
        mov word_51963, bx
        mov word_51965, ax

loc_491C5:              ; CODE XREF: sub_4914A+72j
        add bx, 138h
        cmp byte_519C6, 0
        jz  short loc_491D7
        mov word_51963, bx
        mov word_51965, ax

loc_491D7:              ; CODE XREF: sub_4914A+84j
        sub bx, 270h
        add ax, 0A8h ; '?'
        cmp byte ptr word_510C1, 0
        jz  short loc_491E8
        add ax, 18h

loc_491E8:              ; CODE XREF: sub_4914A+99j
        cmp byte_519D0, 0
        jz  short loc_491F6
        mov word_51963, bx
        mov word_51965, ax

loc_491F6:              ; CODE XREF: sub_4914A+A3j
        add bx, 138h
        cmp byte_519CC, 0
        jz  short loc_49208
        mov word_51963, bx
        mov word_51965, ax

loc_49208:              ; CODE XREF: sub_4914A+B5j
        add bx, 138h
        cmp byte_519CE, 0
        jz  short locret_4921A
        mov word_51963, bx
        mov word_51965, ax

locret_4921A:               ; CODE XREF: sub_4914A+C7j
        return;
sub_4914A   endp


; =============== S U B R O U T I N E =======================================

*/
void sub_4921B() //   proc near       ; CODE XREF: readConfig+8Cp
                   // ; sub_4955B+31p ...
{
    /*
        push    bp
        xor ax, ax
        mov byte_50946, al
        mov word_50947, ax
        mov word_50949, ax
        mov word_5094B, ax
        mov word_5094D, ax
        xor bp, bp
        mov ah, 1
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        shr bp, 1
        mov word_50947, bp
        mov dx, 10h
        xor ax, ax
        cmp bp, dx
        jbe short loc_492A6
        div bp
        mov word_5094B, ax
        xor bp, bp
        mov ah, 2
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    waitForJoystickKey
        jb  short loc_492A6
        add bp, ax
        shr bp, 1
        mov word_50949, bp
        mov dx, 10h
        xor ax, ax
        cmp bp, dx
        jbe short loc_492A6
        div bp
        mov word_5094D, ax
        al = 1
        mov byte_50946, al

loc_492A6:              ; CODE XREF: sub_4921B+19j
                    ; sub_4921B+22j ...
        pop bp
        return;
     */
}

/*
; =============== S U B R O U T I N E =======================================


sub_492A8   proc near       ; CODE XREF: sub_4955B+27p
                    ; sub_4A3E9+76p
        al = byte_510E2
        dec al
        jz  short loc_492B3
        mov byte_510E2, al
        return;
// ; ---------------------------------------------------------------------------

loc_492B3:              ; CODE XREF: sub_492A8+5j
        push    es
        mov ax, seg demoseg
        mov es, ax
        assume es:demoseg
        mov bx, word_510DF
        al = es:[bx]
        inc bx
        pop es
        assume es:nothing
        cmp al, 0FFh
        jz  short loc_492E3
        mov word_510DF, bx

loc_492CA:              ; CODE XREF: sub_492A8+47j
        mov ah, al
        and ah, 0Fh
        mov byte_50941, ah
        and al, 0F0h
        shr al, 1
        shr al, 1
        shr al, 1
        shr al, 1
        al++;
        mov byte_510E2, al
        return;
// ; ---------------------------------------------------------------------------

loc_492E3:              ; CODE XREF: sub_492A8+1Cj
        mov word_51978, 64h ; 'd'
        mov word_51974, 1
        jmp short loc_492CA
sub_492A8   endp


; =============== S U B R O U T I N E =======================================


sub_492F1   proc near       ; CODE XREF: sub_4955B+1Dp
        inc byte_510E2
        mov bl, byte_50941
        cmp byte_510E2, 0FFh
        jnz short loc_49311
        mov byte_510E1, bl
        mov ax, gTimeOfDay
        mov word_5A199, ax
        mov byte_59B5F, ah
        mov byte_59B5C, al

loc_49311:              ; CODE XREF: sub_492F1+Dj
        cmp byte_510E1, bl
        jnz short loc_4931E
        cmp byte_510E2, 0Fh
        jnz short locret_49359

loc_4931E:              ; CODE XREF: sub_492F1+24j
        al = byte_510E1
        mov ah, byte_510E2
        shl ah, 1
        shl ah, 1
        shl ah, 1
        shl ah, 1
        or  al, ah
        mov byte_510E1, al
        al = byte_510E2
        add byte_59B5C, al
        inc byte_59B5C
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD1h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov byte_510E2, 0FFh
        mov bl, byte_50941
        mov byte_510E1, bl

locret_49359:               ; CODE XREF: sub_492F1+2Bj
        return;
sub_492F1   endp


; =============== S U B R O U T I N E =======================================


somethingspsig  proc near       ; CODE XREF: runLevel+355p
                    ; sub_4945D+30p ...
        al = speed2
        xor al, byte_59B5F
        mov byte_59B5D, al
        xor al, byte_59B5C
        mov byte_59B5E, al
        mov ah, 42h ; 'B'
        mov bx, word_510E4
        xor cx, cx
        xor al, al
        mov dx, 5FCh
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 2
        mov dx, 984Dh
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 2
        mov dx, 9E89h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov ah, 42h ; 'B'
        mov bx, word_510E4
        xor cx, cx
        xor dx, dx
        al = 2
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from end of file
        mov byte_510E1, 0FFh
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD1h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        cmp byte_5A19B, 0
        jz  short loc_49435
        mov ax, 3D00h
        mov dx, offset aMyspsig_txt ; "MYSPSIG.TXT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jb  short loc_49435
        mov bx, ax
        push    bx
        mov ax, 4202h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from end of file
        pop bx
        jb  short loc_49430
        mov cx, 1FFh
        or  dx, dx
        jnz short loc_493EB
        or  ax, ax
        jz  short loc_49430
        cmp ax, cx
        ja  short loc_493EB
        mov cx, ax

loc_493EB:              ; CODE XREF: somethingspsig+85j
                    ; somethingspsig+8Dj
        push    bx
        push(cx);
        mov ax, 4200h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        pop(cx);
        pop bx
        jb  short loc_49430
        push    bx
        push(cx);
        mov ax, 3F00h
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pop(cx);
        pop bx
        jb  short loc_49430
        push    bx
        push    es
        push    ds
        pop es
        assume es:data
        push(di);
        push(cx);
        mov di, offset fileLevelData
        al = 0FFh
        cld
        repne scasb
        jz  short loc_4941C
        dec cx
        mov byte ptr [di], 0FFh

loc_4941C:              ; CODE XREF: somethingspsig+BCj
        pop ax
        pop(di);
        pop es
        assume es:nothing
        sub cx, ax
        neg cx
        mov ax, 4000h
        mov bx, word_510E4
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        pop bx

loc_49430:              ; CODE XREF: somethingspsig+7Ej
                    ; somethingspsig+89j ...
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

loc_49435:              ; CODE XREF: somethingspsig+65j
                    ; somethingspsig+6Fj
        mov ax, 3E00h
        mov bx, word_510E4
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        mov byte_510E3, 0
        cmp byte_5A33E, 0
        jz  short loc_4944F
        mov byte_510DE, 1

loc_4944F:              ; CODE XREF: somethingspsig+EEj
        call    drawGamePanelText
        mov byte_5A33F, 1
        mov byte_510DE, 0
        return;
somethingspsig  endp


; =============== S U B R O U T I N E =======================================


sub_4945D   proc near       ; CODE XREF: sub_4955B+294p
                    ; sub_4955B+2A4p ...
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        push    ax
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp byte_510E3, 0
        jz  short loc_49490
        call    somethingspsig

loc_49490:              ; CODE XREF: sub_4945D+2Ej
        pop ax
        mov bx, 380Ah
        add al, 30h ; '0'
        mov [bx+4], al
        cmp byte ptr word_59B6E, 0
        jnz short loc_494A6
        mov bx, 0A014h
        mov [bx+7], al

loc_494A6:              ; CODE XREF: sub_4945D+41j
        mov byte ptr aRecordingDemo0+12h, al ; "0 ---"
        mov cx, 0
        mov dx, bx
        mov ax, 3C00h
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jnb short loc_494B8
        jmp locret_49543
// ; ---------------------------------------------------------------------------

loc_494B8:              ; CODE XREF: sub_4945D+56j
        mov word_510E4, ax
        mov byte_5A140, 83h ; '?'
        mov bl, speed3
        mov cl, 4
        shl bl, cl
        or  bl, gameSpeed
        mov speed2, bl
        mov bx, word_510E4
        mov ax, 4000h
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_49543
        mov ax, gCurrentSelectedLevelIndex
        or  al, 80h
        mov byte_510E2, al
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD2h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_49543
        mov byte_510E1, 0
        mov byte_5A2F8, 1
        mov byte_510DE, 0
        mov byte_510E2, 0FEh ; '?'
        mov word_51A07, 1
        cmp byte_599D4, 0
        jnz short loc_4952A
        mov ax, word_58AB8
        mov word_58AEA, ax
        mov ax, word_58ABA
        and ax, 0FFh
        mov word_58AEC, ax

loc_4952A:              ; CODE XREF: sub_4945D+BCj
        mov byte_510E3, 1
        cmp byte_5A33E, 0
        jz  short loc_4953B
        mov byte_510DE, 1

loc_4953B:              ; CODE XREF: sub_4945D+D7j
        call    sub_4A463
        mov byte_510DE, 0

locret_49543:               ; CODE XREF: sub_4945D+58j
                    ; sub_4945D+82j ...
        return;
sub_4945D   endp

*/

void prepareSomeKindOfLevelIdentifier() // sub_49544  proc near       ; CODE XREF: start+3A1p
                   // ; handleOkButtonClick:loc_4B40Fp ...
{
    // 01ED:28E1

    char char8 = aLevels_dat_0[8];
    char char9 = aLevels_dat_0[9];

    // Checks if the last two chars are "00" like LEVELS.D00?
    if (char8 == '0' && char9 == '0')
    {
        // replaces the content with "--"
        char8 = '-';
        char9 = '-';
    }

//loc_4954F:             // ; CODE XREF: prepareSomeKindOfLevelIdentifier+6j
    // Now checks if the last two chars are "AT" like LEVELS.DAT?
    if (char8 == 'A' && char9 == 'T')
    {
        // replaces the content with "00"
        char8 = '0';
        char9 = '0';
    }

//loc_49557:             // ; CODE XREF: prepareSomeKindOfLevelIdentifier+Ej
    // WTF replaces the first two chars of the string "00S001$0.SP" with what is at ax??
    a00s0010_sp[0] = char8;
    a00s0010_sp[1] = char9;
}

/*

sub_4955B   proc near       ; CODE XREF: runLevel:loc_48B6Bp
                    ; runLevel+30Cp

; FUNCTION CHUNK AT 2DA8 SIZE 0000038B BYTES

        cmp word_51A01, 0
        jz  short loc_49567
        call    sub_4914A
        jmp short loc_4957B
// ; ---------------------------------------------------------------------------

loc_49567:              ; CODE XREF: sub_4955B+5j
        cmp byte_510DE, 0
        jnz short loc_4957B
        call    sub_4A1BF
        cmp byte_510E3, 0
        jz  short loc_4957B
        call    sub_492F1

loc_4957B:              ; CODE XREF: sub_4955B+Aj
                    ; sub_4955B+11j ...
        cmp byte_510DE, 0
        jz  short loc_49585
        call    sub_492A8

loc_49585:              ; CODE XREF: sub_4955B+25j
        cmp byte_519A1, 0
        jz  short loc_4958F
        call    sub_4921B

loc_4958F:              ; CODE XREF: sub_4955B+2Fj
        cmp byte ptr word_510C1+1, 0
        jz  short loc_4959A
        dec byte ptr word_510C1+1

loc_4959A:              ; CODE XREF: sub_4955B+39j
        cmp byte_51999, 0
        jnz short loc_495A9
        mov byte ptr word_510C1+1, 0
        jmp loc_49635
// ; ---------------------------------------------------------------------------

loc_495A9:              ; CODE XREF: sub_4955B+44j
        cmp byte ptr word_510C1+1, 0
        jz  short loc_495B3
        jmp loc_49635
// ; ---------------------------------------------------------------------------

loc_495B3:              ; CODE XREF: sub_4955B+53j
        mov byte ptr word_510C1+1, 20h ; ' '
        cmp byte ptr word_510C1, 0
        jz  short loc_495FB
        mov byte ptr word_510C1, 0
        cmp videoStatusUnk, 1
        jnz short loc_495ED
        mov cl, 90h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_495ED:              ; CODE XREF: sub_4955B+6Ej
        mov cl, 0C8h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_495FB:              ; CODE XREF: sub_4955B+62j
        mov byte ptr word_510C1, 1
        cmp videoStatusUnk, 1
        jnz short loc_49629
        mov cl, 5Fh ; '_'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_49629:              ; CODE XREF: sub_4955B+AAj
        mov cl, 0B0h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers

loc_49635:              ; CODE XREF: sub_4955B+4Bj
                    ; sub_4955B+55j ...
        cmp word_51970, 1
        jz  short loc_4963F
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_4963F:              ; CODE XREF: sub_4955B+DFj
        cmp byte_510E3, 0
        jz  short loc_49649
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_49649:              ; CODE XREF: sub_4955B+E9j
        cmp byte ptr dword_519AF, 0
        jz  short loc_49656
        mov word_51A01, 1

loc_49656:              ; CODE XREF: sub_4955B+F3j
        cmp byte_5199D, 0
        jz  short loc_49663
        mov word ptr flashingbackgroundon, 1

loc_49663:              ; CODE XREF: sub_4955B+100j
        cmp byte ptr word_519A9, 0
        jz  short loc_4966F
        mov ah, 1
        call    sub_4A23C

loc_4966F:              ; CODE XREF: sub_4955B+10Dj
        cmp byte ptr word_519AD, 0
        jz  short loc_4967B
        mov ah, 2
        call    sub_4A23C

loc_4967B:              ; CODE XREF: sub_4955B+119j
        cmp byte_519A0, 0
        jz  short loc_49687
        mov ah, 6
        call    sub_4A23C

loc_49687:              ; CODE XREF: sub_4955B+125j
        cmp byte ptr word_519AB, 0
        jz  short loc_49693
        mov ah, 5
        call    sub_4A23C

loc_49693:              ; CODE XREF: sub_4955B+131j
        cmp byte_5199C, 0
        jz  short loc_4969F
        mov ah, 11h
        call    sub_4A23C

loc_4969F:              ; CODE XREF: sub_4955B+13Dj
        cmp byte_51990, 0
        jz  short loc_496AC
        call    videoloop
        call    sub_4A3E9

loc_496AC:              ; CODE XREF: sub_4955B+149j
        cmp byte_510DE, 0
        jz  short loc_496B6
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_496B6:              ; CODE XREF: sub_4955B+156j
        cmp speed3, 0
        jge short loc_496C0
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_496C0:              ; CODE XREF: sub_4955B+160j
        cmp byte_5197F, 0
        jz  short loc_496CD
        mov word_51A07, 1

loc_496CD:              ; CODE XREF: sub_4955B+16Aj
        cmp byte_51980, 0
        jz  short loc_496DA
        mov word_51A07, 2

loc_496DA:              ; CODE XREF: sub_4955B+177j
        cmp byte_51981, 0
        jz  short loc_496E7
        mov word_51A07, 3

loc_496E7:              ; CODE XREF: sub_4955B+184j
        cmp byte_51982, 0
        jz  short loc_496F4
        mov word_51A07, 4

loc_496F4:              ; CODE XREF: sub_4955B+191j
        cmp byte_51983, 0
        jz  short loc_49701
        mov word_51A07, 6

loc_49701:              ; CODE XREF: sub_4955B+19Ej
        cmp byte_51984, 0
        jz  short loc_4970E
        mov word_51A07, 8

loc_4970E:              ; CODE XREF: sub_4955B+1ABj
        cmp byte_51985, 0
        jz  short loc_4971B
        mov word_51A07, 0Ch

loc_4971B:              ; CODE XREF: sub_4955B+1B8j
        cmp byte_51986, 0
        jz  short loc_49728
        mov word_51A07, 10h

loc_49728:              ; CODE XREF: sub_4955B+1C5j
        cmp byte_51987, 0
        jz  short loc_49735
        mov word_51A07, 18h

loc_49735:              ; CODE XREF: sub_4955B+1D2j
        cmp byte_51988, 0
        jz  short loc_49742
        mov word_51A07, 20h ; ' '

loc_49742:              ; CODE XREF: sub_4955B+EBj
                    ; sub_4955B+158j ...
        cmp byte_5199A, 1
        jnz short loc_4974C
        jmp loc_497D1
// ; ---------------------------------------------------------------------------

loc_4974C:              ; CODE XREF: sub_4955B+1ECj
        mov ax, word_5195D
        and ax, 7
        jz  short loc_49757
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49757:              ; CODE XREF: sub_4955B+1F7j
        cmp byte_510E3, 0
        jz  short loc_49761
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49761:              ; CODE XREF: sub_4955B+201j
        cmp byte_519B8, 0
        jnz short loc_4976F
        mov byte_59B7C, 0
        jmp short loc_49786
// ; ---------------------------------------------------------------------------

loc_4976F:              ; CODE XREF: sub_4955B+20Bj
        cmp byte_59B7C, 0
        jnz short loc_49786
        dec byte_59B7C
        and byte_5101C, 1
        xor byte_5101C, 1
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_49786:              ; CODE XREF: sub_4955B+212j
                    ; sub_4955B+219j
        cmp byte_519B9, 0
        jnz short loc_49794
        mov byte_59B7D, 0
        jmp short loc_497AB
// ; ---------------------------------------------------------------------------

loc_49794:              ; CODE XREF: sub_4955B+230j
        cmp byte_59B7D, 0
        jnz short loc_497AB
        dec byte_59B7D
        and byte_51035, 2
        xor byte_51035, 2
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_497AB:              ; CODE XREF: sub_4955B+237j
                    ; sub_4955B+23Ej
        cmp byte_519BA, 0
        jnz short loc_497B9
        mov byte_59B7E, 0
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_497B9:              ; CODE XREF: sub_4955B+255j
        cmp byte_59B7E, 0
        jnz short loc_497CE
        dec byte_59B7E
        and byte_510D7, 1
        xor byte_510D7, 1

loc_497CE:              ; CODE XREF: sub_4955B+229j
                    ; sub_4955B+24Ej ...
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497D1:              ; CODE XREF: sub_4955B+1EEj
        cmp byte_510DE, 0
        jz  short loc_497DB
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497DB:              ; CODE XREF: sub_4955B+27Bj
        cmp speed3, 0
        jge short loc_497E5
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497E5:              ; CODE XREF: sub_4955B+285j
        cmp byte_519B8, 1
        jnz short loc_497F5
        mov ax, 0
        call    sub_4945D
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497F5:              ; CODE XREF: sub_4955B+28Fj
        cmp byte_519B9, 1
        jnz short loc_49805
        mov ax, 1
        call    sub_4945D
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49805:              ; CODE XREF: sub_4955B+29Fj
        cmp byte_519BA, 1
        jnz short loc_49814
        mov ax, 2
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49814:              ; CODE XREF: sub_4955B+2AFj
        cmp byte_519BB, 1
        jnz short loc_49823
        mov ax, 3
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49823:              ; CODE XREF: sub_4955B+2BEj
        cmp byte_519BC, 1
        jnz short loc_49832
        mov ax, 4
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49832:              ; CODE XREF: sub_4955B+2CDj
        cmp byte_519BD, 1
        jnz short loc_49841
        mov ax, 5
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49841:              ; CODE XREF: sub_4955B+2DCj
        cmp byte_519BE, 1
        jnz short loc_49850
        mov ax, 6
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49850:              ; CODE XREF: sub_4955B+2EBj
        cmp byte_519BF, 1
        jnz short loc_4985F
        mov ax, 7
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4985F:              ; CODE XREF: sub_4955B+2FAj
        cmp byte_519C0, 1
        jnz short loc_4986E
        mov ax, 8
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4986E:              ; CODE XREF: sub_4955B+309j
        cmp byte_519C1, 1
        jnz short loc_4987D
        mov ax, 9
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4987D:              ; CODE XREF: sub_4955B+318j
        cmp byte_519D5, 1
        jnz short loc_4988E
        cmp byte_510E3, 0
        jz  short loc_4988E
        call    somethingspsig

loc_4988E:              ; CODE XREF: sub_4955B+1F9j
                    ; sub_4955B+203j ...
        cmp byte_510E3, 0
        jz  short loc_49898
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_49898:              ; CODE XREF: sub_4955B+338j
        cmp byte_510DE, 0
        jz  short loc_498A2
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_498A2:              ; CODE XREF: sub_4955B+342j
        cmp byte_51989, 0
        jnz short loc_498B5
        mov byte_59B7F, 0
        mov byte_59B80, 5
        jmp short loc_498FC
// ; ---------------------------------------------------------------------------

loc_498B5:              ; CODE XREF: sub_4955B+34Cj
        cmp byte_59B7F, 0
        jz  short loc_498C2
        dec byte_59B7F
        jmp short loc_498FC
// ; ---------------------------------------------------------------------------

loc_498C2:              ; CODE XREF: sub_4955B+35Fj
        cmp byte_59B80, 0
        jz  short loc_498D2
        dec byte_59B80
        mov byte_59B7F, 10h

loc_498D2:              ; CODE XREF: sub_4955B+36Cj
        cmp gCurrentSelectedLevelIndex, 1
        ja  short loc_498DF
        mov gCurrentSelectedLevelIndex, 2

loc_498DF:              ; CODE XREF: sub_4955B+37Cj
        dec gCurrentSelectedLevelIndex
        cmp gCurrentSelectedLevelIndex, 6Fh ; 'o'
        jbe short loc_498F0
        mov gCurrentSelectedLevelIndex, 6Fh ; 'o'

loc_498F0:              ; CODE XREF: sub_4955B+38Dj
        mov ax, gCurrentSelectedLevelIndex
        call    sub_4BF4A
        call    drawLevelList
        call    sub_4A3D2

loc_498FC:              ; CODE XREF: sub_4955B+358j
                    ; sub_4955B+365j
        cmp byte_5198A, 0
        jnz short loc_4990F
        mov byte_59B81, 0
        mov byte_59B82, 5
        jmp short loc_49949
// ; ---------------------------------------------------------------------------

loc_4990F:              ; CODE XREF: sub_4955B+3A6j
        cmp byte_59B81, 0
        jz  short loc_4991C
        dec byte_59B81
        jmp short loc_49949
// ; ---------------------------------------------------------------------------

loc_4991C:              ; CODE XREF: sub_4955B+3B9j
        cmp byte_59B82, 0
        jz  short loc_4992C
        dec byte_59B82
        mov byte_59B81, 10h

loc_4992C:              ; CODE XREF: sub_4955B+3C6j
        cmp gCurrentSelectedLevelIndex, 6Fh ; 'o'
        jb  short loc_49939
        mov gCurrentSelectedLevelIndex, 6Eh ; 'n'

loc_49939:              ; CODE XREF: sub_4955B+3D6j
        inc gCurrentSelectedLevelIndex
        mov ax, gCurrentSelectedLevelIndex
        call    sub_4BF4A
        call    drawLevelList
        call    sub_4A3D2

loc_49949:              ; CODE XREF: sub_4955B+E1j
                    ; sub_4955B+33Aj ...
        cmp byte_59B6B, 0
        mov byte_59B6B, 0
        jz  short loc_49958
        jmp loc_49A89
// ; ---------------------------------------------------------------------------

loc_49958:              ; CODE XREF: sub_4955B+3F8j
        cmp byte_5199A, 1
        jz  short loc_49962
        jmp loc_49C41
// ; ---------------------------------------------------------------------------

loc_49962:              ; CODE XREF: sub_4955B+402j
        cmp byte_519D5, 1
        jnz short loc_49984
        cmp byte_510DE, 0
        jz  short loc_49984
        mov byte_510DE, 0
        mov byte_510B3, 0
        mov byte_5A2F9, 1
        mov byte_5A33E, 1

loc_49984:              ; CODE XREF: sub_4955B+40Cj
                    ; sub_4955B+413j
        cmp byte_519C3, 1
        jnz short loc_499AA
        mov word_51970, 1
        cmp videoStatusUnk, 1
        jnz short loc_499AA
        mov di, 6D2h
        mov ah, 6
        push    si
        mov si, 0A00Ah
        call    drawTextWithChars8Font
        pop si
        mov byte_5197C, 46h ; 'F'

loc_499AA:              ; CODE XREF: sub_4955B+42Ej
                    ; sub_4955B+43Bj
        cmp byte_5198E, 1
        jz  short loc_499C8
        jmp loc_49A7F
sub_4955B   endp


; =============== S U B R O U T I N E =======================================


readFromFh1 proc near       ; CODE XREF: sub_4955B+54Ep
                    ; sub_4955B+57Cp ...
        mov ax, 3F00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        return;
readFromFh1 endp


; =============== S U B R O U T I N E =======================================


writeToFh1  proc near       ; CODE XREF: sub_4955B+486p
                    ; sub_4955B+494p ...
        mov ax, 4000h
        mov bx, fh1
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        return;
writeToFh1  endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR sub_4955B

loc_499C8:              ; CODE XREF: sub_4955B+454j
        mov cx, 0
        mov dx, offset aSavegame_sav ; "SAVEGAME.SAV"
        mov ax, 3C00h
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jnb short loc_499D8
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_499D8:              ; CODE XREF: sub_4955B+478j
        mov fh1, ax
        mov cx, 4
        mov dx, 9FF9h
        call    writeToFh1
        jnb short loc_499E9
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_499E9:              ; CODE XREF: sub_4955B+489j
        mov cx, 1238h
        mov dx, offset leveldata
        call    writeToFh1
        jnb short loc_499F7
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_499F7:              ; CODE XREF: sub_4955B+497j
        cmp byte_510DE, 0
        jnz short loc_49A03
        mov dx, 87A8h
        jmp short loc_49A06
// ; ---------------------------------------------------------------------------

loc_49A03:              ; CODE XREF: sub_4955B+4A1j
        mov dx, 87DAh

loc_49A06:              ; CODE XREF: sub_4955B+4A6j
        mov cx, 1Ch
        push    dx
        call    writeToFh1
        pop bx
        jnb short loc_49A13
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A13:              ; CODE XREF: sub_4955B+4B3j
        mov cx, 6
        mov byte_5988D, 63h ; 'c'
        mov ax, word ptr aLevels_dat_0+8 ; "AT"
        mov word_5988E, ax
        al = [bx]
        cmp byte_510DE, 0
        jz  short loc_49A2C
        or  al, 80h

loc_49A2C:              ; CODE XREF: sub_4955B+4CDj
        mov byte_59890, al
        mov ax, [bx+1]
        mov word_59891, ax
        mov dx, 957Dh
        call    writeToFh1
        jnb short loc_49A40
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A40:              ; CODE XREF: sub_4955B+4E0j
        mov cx, 0E6h ; '?'
        mov dx, 0D08h
        call    writeToFh1
        jnb short loc_49A4E
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A4E:              ; CODE XREF: sub_4955B+4EEj
        mov cx, 23h ; '#'
        mov dx, 164Ah
        call    writeToFh1
        jnb short loc_49A5C
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A5C:              ; CODE XREF: sub_4955B+4FCj
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        call    writeToFh1
        jnb short loc_49A6A
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A6A:              ; CODE XREF: sub_4955B+50Aj
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_49A78
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49A78:              ; CODE XREF: sub_4955B+518j
        push    si
        mov si, 0A001h
        jmp loc_49C2C
// ; ---------------------------------------------------------------------------

loc_49A7F:              ; CODE XREF: sub_4955B+456j
        cmp byte ptr dword_519A3, 1
        jz  short loc_49A89
        jmp loc_49C41
// ; ---------------------------------------------------------------------------

loc_49A89:              ; CODE XREF: sub_4955B+3FAj
                    ; sub_4955B+529j
        mov ax, 3D00h
        mov dx, offset aSavegame_sav ; "SAVEGAME.SAV"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_49A96
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49A96:              ; CODE XREF: sub_4955B+536j
        mov fh1, ax
        cmp byte_510E3, 0
        jz  short loc_49AA3
        call    somethingspsig

loc_49AA3:              ; CODE XREF: sub_4955B+543j
        mov cx, 4
        mov dx, 9FFDh
        call    readFromFh1
        jnb short loc_49AB1
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AB1:              ; CODE XREF: sub_4955B+551j
        mov cx, word_5A30D
        cmp cx, word_5A309
        jnz short loc_49AC5
        mov cx, word_5A30F
        cmp cx, word_5A30B
        jz  short loc_49AD1

loc_49AC5:              ; CODE XREF: sub_4955B+55Ej
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49AD1:              ; CODE XREF: sub_4955B+568j
        mov cx, 1238h
        mov dx, offset leveldata
        call    readFromFh1
        jnb short loc_49ADF
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49ADF:              ; CODE XREF: sub_4955B+57Fj
        mov cx, 1Ch
        mov dx, 87A8h
        call    readFromFh1
        jnb short loc_49AED
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AED:              ; CODE XREF: sub_4955B+58Dj
        mov cx, 6
        mov dx, 957Dh
        call    readFromFh1
        jnb short loc_49AFB
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AFB:              ; CODE XREF: sub_4955B+59Bj
        mov cx, 84h ; '?'
        mov dx, 0D08h
        call    readFromFh1
        jnb short loc_49B09
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B09:              ; CODE XREF: sub_4955B+5A9j
        mov cx, 4
        mov dx, 0D9Bh
        call    readFromFh1
        jnb short loc_49B17
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B17:              ; CODE XREF: sub_4955B+5B7j
        mov cx, 7
        mov dx, 0D90h
        call    readFromFh1
        jnb short loc_49B25
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B25:              ; CODE XREF: sub_4955B+5C5j
        mov cx, 4
        mov dx, 0D9Bh
        call    readFromFh1
        jnb short loc_49B33
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B33:              ; CODE XREF: sub_4955B+5D3j
        mov cx, 53h ; 'S'
        mov dx, 0D9Bh
        push    word_510C1
        call    readFromFh1
        pop word_510C1
        jnb short loc_49B49
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B49:              ; CODE XREF: sub_4955B+5E9j
        mov cx, 23h ; '#'
        mov dx, 164Ah
        push    word_51961
        push    word_51967
        push    word_51970
        call    readFromFh1
        pop word_51970
        pop word_51967
        pop word_51961
        jnb short loc_49B6F
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B6F:              ; CODE XREF: sub_4955B+60Fj
        cmp byte_5988D, 0
        jz  short loc_49B84
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        call    readFromFh1
        jb  short loc_49B84
        call    sub_49D53

loc_49B84:              ; CODE XREF: sub_4955B+619j
                    ; sub_4955B+624j
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        mov byte_510DE, 0
        mov byte_510E3, 0
        mov byte_50941, 0
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        call    getTime
        call    sub_4A1AE
        mov si, 60D5h
        call    setPalette
        call    sub_48F6D
        call    sub_501C0
        call    sub_4A2E6
        mov si, murphyloc
        mov ax, si
        shr ax, 1
        call    sub_4A291
        mov ax, 0FFFFh
        mov word_510B7, ax
        mov byte_510B9, al
        call    sub_4FDFD
        mov byte_5A2F9, 1
        mov byte_510E3, 0
        push    si
        mov si, 0A004h
        cmp videoStatusUnk, 1
        jnz short loc_49C12
        mov di, 6D2h
        mov ah, 6
        call    drawTextWithChars8Font
        mov byte_5197C, 46h ; 'F'

loc_49C12:              ; CODE XREF: sub_4955B+6A8j
        mov si, 6015h
        call    fade
        jmp short loc_49C40
// ; ---------------------------------------------------------------------------

loc_49C1A:              ; CODE XREF: sub_4955B+553j
                    ; sub_4955B+581j ...
        mov byte_510E3, 0

loc_49C1F:              ; CODE XREF: sub_4955B+48Bj
                    ; sub_4955B+499j ...
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

loc_49C28:              ; CODE XREF: sub_4955B+47Aj
                    ; sub_4955B+51Aj ...
        push    si
        mov si, 0A007h

loc_49C2C:              ; CODE XREF: sub_4955B+521j
        cmp videoStatusUnk, 1
        jnz short loc_49C40
        mov di, 6D2h
        mov ah, 6
        call    drawTextWithChars8Font
        mov byte_5197C, 46h ; 'F'

loc_49C40:              ; CODE XREF: sub_4955B+6BDj
                    ; sub_4955B+6D6j
        pop si

loc_49C41:              ; CODE XREF: sub_4955B+404j
                    ; sub_4955B+52Bj
        cmp byte_519B5, 1
        jnz short loc_49C96
        cmp byte_519C3, 1
        jnz short loc_49C96
        mov word_51970, 0
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp videoStatusUnk, 1
        jnz short loc_49C96
        mov di, 6D2h
        mov ah, 6
        push    si
        mov si, 0A00Dh
        call    drawTextWithChars8Font
        pop si
        mov byte_5197C, 46h ; 'F'

loc_49C96:              ; CODE XREF: sub_4955B+6EBj
                    ; sub_4955B+6F2j ...
        cmp byte_51996, 0
        jz  short loc_49CC8
        mov byte_510AE, 0
        mov si, 6095h
        call    fade

loc_49CA8:              ; CODE XREF: sub_4955B+752j
        cmp byte_51996, 1
        jz  short loc_49CA8

loc_49CAF:              ; CODE XREF: sub_4955B+759j
        cmp byte_51996, 0
        jz  short loc_49CAF

loc_49CB6:              ; CODE XREF: sub_4955B+760j
        cmp byte_51996, 1
        jz  short loc_49CB6
        mov si, 6015h
        call    fade
        mov byte_510AE, 1

loc_49CC8:              ; CODE XREF: sub_4955B+740j
        cmp byte_519C2, 0
        jz  short loc_49D15
        mov byte_510AE, 0
        mov si, 6095h
        call    fade

loc_49CDA:              ; CODE XREF: sub_4955B+784j
        cmp byte_519C2, 1
        jz  short loc_49CDA
        mov si, 179Ah

loc_49CE4:              ; CODE XREF: sub_4955B+7A6j
        mov bx, [si]
        cmp bx, 0FFFFh
        jnz short loc_49CF3
        mov word_51970, 0
        jmp short loc_49D03
// ; ---------------------------------------------------------------------------

loc_49CF3:              ; CODE XREF: sub_4955B+78Ej
        cmp byte ptr [bx+166Dh], 0
        jz  short loc_49CFC
        inc si
        inc si

loc_49CFC:              ; CODE XREF: sub_4955B+79Dj
        cmp byte_519C2, 0
        jz  short loc_49CE4

loc_49D03:              ; CODE XREF: sub_4955B+796j
                    ; sub_4955B+7ADj
        cmp byte_519C2, 1
        jz  short loc_49D03
        mov si, 6015h
        call    fade
        mov byte_510AE, 1

loc_49D15:              ; CODE XREF: sub_4955B+772j
        cmp byte_5197E, 0
        jz  short loc_49D29
        cmp word_51978, 0
        jg  short loc_49D29
        mov word_510D1, 1

loc_49D29:              ; CODE XREF: sub_4955B+7BFj
                    ; sub_4955B+7C6j
        cmp byte_5198D, 0
        jz  short loc_49D48
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0

loc_49D48:              ; CODE XREF: sub_4955B+7D3j
        cmp byte ptr word_519B3, 0
        jz  short locret_49D52
        call    sub_4FDCE

locret_49D52:               ; CODE XREF: sub_4955B+7F2j
        return;
; END OF FUNCTION CHUNK FOR sub_4955B

; =============== S U B R O U T I N E =======================================


sub_49D53   proc near       ; CODE XREF: sub_4955B+626p
                    ; sub_4A23C+21p
        mov byte_59B7B, 0
sub_49D53   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


levelScanThing   proc near       ; CODE XREF: runLevel+A7p
        push    es
        push    ds
        pop es
        assume es:data
        cld
        mov cx, 5A0h
        mov di, offset leveldata
        mov ax, 0F11Fh

loc_49D65:              ; CODE XREF: levelScanThing+18j
        cmp es:[di], al
        jnz short loc_49D6D
        mov es:[di], ah

loc_49D6D:              ; CODE XREF: levelScanThing+10j
        inc di
        inc di
        dec cx
        jnz short loc_49D65
        cmp byte_59B7B, 0
        mov byte_59B7B, 0
        jnz short loc_49DDE
        mov cx, 5A0h
        mov di, offset leveldata

loc_49D84:              ; CODE XREF: levelScanThing+4Cj
        mov ax, 6
        repne scasw
        jnz short loc_49DA6
        mov bx, 59Fh
        sub bx, cx
        al = [bx-6775h]
        cmp ax, 1Ch
        jb  short loc_49DA2
        cmp ax, 25h ; '%'
        ja  short loc_49DA2
        mov es:[di-2], ax

loc_49DA2:              ; CODE XREF: levelScanThing+3Fj
                    ; levelScanThing+44j
        or  cx, cx
        jnz short loc_49D84

loc_49DA6:              ; CODE XREF: levelScanThing+31j
        mov cx, 5A0h
        mov di, offset leveldata

loc_49DAC:              ; CODE XREF: levelScanThing+7Fj
        mov ax, 5
        repne scasw
        jnz short loc_49DD9
        mov bx, 59Fh
        sub bx, cx
        al = [bx-6775h]
        cmp ax, 1Ah
        jb  short loc_49DD5
        cmp ax, 27h //; '''
        ja  short loc_49DD5
        sub ax, 1Ch
        cmp ax, 0Ah
        jb  short loc_49DD5
        add ax, 1Ch
        mov es:[di-2], ax

loc_49DD5:              ; CODE XREF: levelScanThing+67j
                    ; levelScanThing+6Cj ...
        or  cx, cx
        jnz short loc_49DAC

loc_49DD9:              ; CODE XREF: levelScanThing+59j
        mov byte_59B7B, 1

loc_49DDE:              ; CODE XREF: levelScanThing+24j
        pop es
        assume es:nothing
        return;
levelScanThing   endp


; =============== S U B R O U T I N E =======================================


gameloop?   proc near       ; CODE XREF: runLevel:noFlashingp

        ; set graphics write mode = 1
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        mov si, murphyloc
        call    update?
        mov murphyloc, si

        cmp word ptr flashingbackgroundon, 0
        jz  short loc_49E0A

        ; flashes background
        ; set palette ix 0 = #3f2121 (maroon)
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 3Fh
        out dx, al
        out dx, al
        al = 21h
        out dx, al

loc_49E0A:
        cmp byte ptr word_510C1, 0
        jz  short loc_49E14
        call    sub_5024B

loc_49E14:
        ; set graphics write mode = 1
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        cmp word ptr flashingbackgroundon, 0
        jz  short loc_49E33
        
        ; flashes background
        ; set palette ix 0 = #3f2121 (maroon)
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 3Fh
        out dx, al
        al = 21h
        out dx, al
        out dx, al

loc_49E33:
        mov di, offset movingObjects?
        mov si, 7Ah ; level width in words + 2 (index of first gamefiled cell)
        mov cx, 526h ; unsure count
        xor dx, dx
        xor bh, bh

checkCellForMovingObject:              ; CODE XREF: gameloop?+84j
        mov bl, byte ptr leveldata[si]

        test    bl, 0Dh
        jz  short moveToNextCell

        cmp bl, 20h
        jnb short moveToNextCell

        ; multiply by 2 to go from byte to word
        shl bx, 1
        mov ax, movingFunctions[bx]
        or  ax, ax
        jz  short moveToNextCell

        ; log moving object for call
        ; each index has 2 bytes for index and 2 bytes as fn ptr to call
        inc dx
        mov [di], si
        mov [di+2], ax
        add di, 4

moveToNextCell:

        add si, 2
        loop    checkCellForMovingObject

        cmp dx, 0
        jz  short doneWithGameLoop ; not a single moving object
        mov cx, dx ; cx = number of moving objects
        mov di, offset movingObjects?

; call moving functions one by one
callNextMovingFunction:
        push(di);
        push(cx);
        mov si, [di]    ; the cell for the moving object
        mov ax, [di+2]  ; the function to call
        call    ax
        pop(cx);
        pop(di);
        add di, 4
        loop    callNextMovingFunction

doneWithGameLoop:
        ; set graphics write mode = 0
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 0
        out dx, al

        cmp word_510D1, 1
        jz  short loc_49E99
        cmp word_510CF, 0
        jz  short loc_49E99
        return;
// ; ---------------------------------------------------------------------------

loc_49E99:              ; CODE XREF: gameloop?+AFj
                    ; gameloop?+B6j
        cmp word_51978, 0
        jnz short loc_49EB3
        mov word_510D1, 0
        mov si, word_510C7
        call    sub_4A61F
        mov word_51978, 40h ; '@'

loc_49EB3:
        ; set graphics write mode = 0
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        return;
gameloop?   endp
*/
void sub_49EBE() //   proc near       ; CODE XREF: runLevel+109p
                   // ; sub_4A291+29p
{
    /*
    xor dx, dx
    cmp byte_510C0, 0
    jz  short loc_49ECC
    call    sub_4A1AE
    mov dx, ax

//loc_49ECC:              ; CODE XREF: sub_49EBE+7j
    mov bx, word_510E8
    mov ax, word_510EA
    sub bx, 98h ; '?'
    cmp bx, 8
    jge short loc_49EDF
    mov bx, 8

//loc_49EDF:              ; CODE XREF: sub_49EBE+1Cj
    cmp bx, 278h
    jbe short loc_49EE8
    mov bx, 278h

//loc_49EE8:              ; CODE XREF: sub_49EBE+25j
    cmp byte ptr word_510C1, 0
    jz  short loc_49EF4
    sub ax, 58h ; 'X'
    jmp short loc_49EF7

//loc_49EF4:              ; CODE XREF: sub_49EBE+2Fj
    sub ax, 64h ; 'd'

//loc_49EF7:              ; CODE XREF: sub_49EBE+34j
    cmp ax, 0
    jge short loc_49EFE
    xor ax, ax

//loc_49EFE:              ; CODE XREF: sub_49EBE+3Cj
    cmp byte ptr word_510C1, 0
    jz  short loc_49F0F
    cmp ax, 0C0h ; '?'
    jle short loc_49F0D
    mov ax, 0C0h ; '?'

//loc_49F0D:              ; CODE XREF: sub_49EBE+4Aj
    jmp short loc_49F17

//loc_49F0F:              ; CODE XREF: sub_49EBE+45j
    cmp ax, 0A8h ; '?'
    jle short loc_49F17
    mov ax, 0A8h ; '?'

//loc_49F17:              ; CODE XREF: sub_49EBE:loc_49F0Dj
//                ; sub_49EBE+54j
    cmp word_51A01, 0
    jz  short loc_49F25
    cmp byte_519C9, 0
    jz  short loc_49F2E

//loc_49F25:              ; CODE XREF: sub_49EBE+5Ej
    mov word_59B88, bx
    mov word_59B8A, ax
    jmp short loc_49FA9

//loc_49F2E:              ; CODE XREF: sub_49EBE+65j
    mov bx, word_59B88
    mov ax, word_59B8A
    mov cx, bx
    add bx, word_51963
    cmp bx, 8
    jge short loc_49F56
    cmp byte_59B6C, 0
    jnz short loc_49F4C
    mov bx, 8
    jmp short loc_49F66

//loc_49F4C:              ; CODE XREF: sub_49EBE+87j
    add bx, 3D0h
    dec word_51965
    jmp short loc_49F66

//loc_49F56:              ; CODE XREF: sub_49EBE+80j
    cmp byte_59B6C, 0
    jnz short loc_49F6E
    cmp bx, 278h
    jbe short loc_49F6E
    mov bx, 278h

//loc_49F66:              ; CODE XREF: sub_49EBE+8Cj
//                ; sub_49EBE+96j
    sub cx, bx
    neg cx
    mov word_51963, cx

//loc_49F6E:              ; CODE XREF: sub_49EBE+9Dj
//                ; sub_49EBE+A3j
    mov cx, ax
    add ax, word_51965
    jge short loc_49F81
    cmp byte_59B6C, 0
    jnz short loc_49FA9
    xor ax, ax
    jmp short loc_49FA1

//loc_49F81:              ; CODE XREF: sub_49EBE+B6j
    cmp byte_59B6C, 0
    jnz short loc_49FA9
    cmp byte ptr word_510C1, 0
    jz  short loc_49F99
    cmp ax, 0C0h ; '?'
    jbe short loc_49FA9
    mov ax, 0C0h ; '?'
    jmp short loc_49FA1

//loc_49F99:              ; CODE XREF: sub_49EBE+CFj
    cmp ax, 0A8h ; '?'
    jbe short loc_49FA9
    mov ax, 0A8h ; '?'

//loc_49FA1:              ; CODE XREF: sub_49EBE+C1j
//                ; sub_49EBE+D9j
    sub cx, ax
    neg cx
    mov word_51965, cx

//loc_49FA9:              ; CODE XREF: sub_49EBE+6Ej
//                ; sub_49EBE+BDj ...
    cmp byte_59B72, 0
    jnz short loc_49FBE
    cmp byte_59B6D, 0
    jnz short loc_49FD2
    cmp byte ptr word_51978, 0
    jz  short loc_49FD2

//loc_49FBE:              ; CODE XREF: sub_49EBE+F0j
    xor cx, cx
    and dx, 101h
    xchg    cl, dh
    add ax, cx
    cmp bx, 13Ch
    jbe short loc_49FD0
    neg dx

//loc_49FD0:              ; CODE XREF: sub_49EBE+10Ej
    add bx, dx

//loc_49FD2:              ; CODE XREF: sub_49EBE+F7j
//                ; sub_49EBE+FEj
    mov word_5195F, bx
    mov word_51961, ax
    mov cl, 3
    shr bx, cl
    mov cx, 7Ah ; 'z'
    mul cx
    add bx, ax
    add bx, 4D34h
    mov word_51967, bx
    return;
     */
}

void waitForJoystickKey() // sub_49FED  proc near       ; CODE XREF: sub_48E59+2Cp
                   // ; sub_48E59+67p ...
{
    // Maybe it waits for a key in the joystick to be pressed???
    // Parameters:
    // ah: bitmask of the button/coordiante to test

    if (isJoystickEnabled == 0)
    {
        ax = 0;
        cf = 1; // stc
        return;
    }

//loc_49FF8:              ; CODE XREF: waitForJoystickKey+5j
    dx = 0x201; // 513
//    cli
    cx = 0;

    char keyWasPressed = 1;

    do
    {
//loc_49FFF:              ; CODE XREF: waitForJoystickKey+21j
        // 2 possible joysticks: (X1, Y1, 11, 12) and (X2, Y2, 21, 22)
    //    in  al, dx      ; Game I/O port
    //                ; bits 0-3: Coordinates (resistive, time-dependent inputs) X1, Y1, X2, Y2
    //                ; bits 4-7: Buttons/Triggers (digital inputs) 11, 12, 21, 12
        if ((al & ah) == 0)
        {
            keyWasPressed = 0;
            break;
        }
        cx--;
    }
    while (cx > 0);

    if (keyWasPressed == 1)
    {
        cf = 1; //stc
    }
    else
    {
//loc_4A013:              ; CODE XREF: waitForJoystickKey+1Fj
    //    out dx, al      ; Game I/O port
    //                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
    //                ; bits 4-7: Buttons/Triggers (digital inputs)
        cx = 0;

        do
        {
//loc_4A021:              ; CODE XREF: waitForJoystickKey+44j
        //    in  al, dx      ; Game I/O port
        //                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
        //                ; bits 4-7: Buttons/Triggers (digital inputs)
            if ((al & ah) == 0)
            {
                keyWasPressed = 0;
                break;
            }
            cx--;
        }
        while (cx > 0);

        if (keyWasPressed == 1)
        {
            cf = 1; //stc
        }
        else
        {
//loc_4A036:              ; CODE XREF: waitForJoystickKey+42j
            cx = -cx;
            ax = cx;
            cf = 0; // clc
        }
    }

//loc_4A03B:              ; CODE XREF: waitForJoystickKey+24j
//                ; waitForJoystickKey+47j
//    sti
    return;
}

/*

movefun7  proc near       ; DATA XREF: data:163Co
        cmp byte ptr leveldata[si], 19h
        jz  short loc_4A045
        return;
// ; ---------------------------------------------------------------------------

loc_4A045:              ; CODE XREF: movefun7+5j
        mov ax, word_5195D
        and ax, 3
        cmp ax, 0
        jz  short loc_4A051
        return;
// ; ---------------------------------------------------------------------------

loc_4A051:              ; CODE XREF: movefun7+11j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 0Eh
        jl  short loc_4A067
        call    sub_4A1AE
        and al, 3Fh
        add al, 20h ; ' '
        neg al
        mov bl, al

loc_4A067:              ; CODE XREF: movefun7+1Dj
        mov [si+1835h], bl
        cmp bl, 0
        jge short loc_4A071
        return;
// ; ---------------------------------------------------------------------------

loc_4A071:              ; CODE XREF: movefun7+31j
        cmp byte ptr [si+17BAh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+17BEh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+1832h], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+1836h], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18AAh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18AEh], 3
        jz  short loc_4A0AB
        jmp short loc_4A0AE
// ; ---------------------------------------------------------------------------

loc_4A0AB:              ; CODE XREF: movefun7+39j
                    ; movefun7+40j ...
        call    sound8

loc_4A0AE:              ; CODE XREF: movefun7+6Cj
        xor bh, bh
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 1272h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A0C6:              ; CODE XREF: movefun7+91j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A0C6
        pop ds
        return;
movefun7  endp


; =============== S U B R O U T I N E =======================================


movefun5  proc near       ; DATA XREF: data:1630o
        cmp byte ptr leveldata[si], 13h
        jz  short loc_4A0DA
        return;
// ; ---------------------------------------------------------------------------

loc_4A0DA:              ; CODE XREF: movefun5+5j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 0
        jg  short loc_4A0EA
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4A0EA:              ; CODE XREF: movefun5+11j
        call    sub_4A1AE
        and al, byte_5196A
        neg al
        mov bl, al
        mov [si+1835h], bl
        mov di, [si+6155h]
        mov si, di
        add di, 4C4h
        add si, 0F4h ; '?'
        push    ds
        mov ax, es
        mov ds, ax
        push    si
        push(di);
        movsb
        add di, 0FC2Fh
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        pop(di);
        pop si
        inc si
        inc di
        movsb
        add di, 0FC2Fh
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        pop ds
        return;
movefun5  endp

*/
void getTime() //     proc near       ; CODE XREF: start:doesNotHaveCommandLinep
                    // ; sub_4955B+669p ...
{
//        mov ax, 0
//        int 1Ah     ; CLOCK - GET TIME OF DAY
//                    ; Return: CX:DX = clock count
//                    ; AL = 00h if clock was read or written (via AH=0,1) since the previous
//                    ; midnight
//                    ; Otherwise, AL > 0
//    xor cx, dx
//    mov gTimeOfDay, cx
    gTimeOfDay = (uint32_t)time(NULL);
}

/*
sub_4A1AE   proc near       ; CODE XREF: sub_4955B+66Cp
                    ; sub_49EBE+9p ...
        mov ax, gTimeOfDay
        mov bx, 5E5h
        mul bx
        add ax, 31h ; '1'
        mov gTimeOfDay, ax
        shr ax, 1
        return;
sub_4A1AE   endp
*/

void sub_4A1BF() //   proc near       ; CODE XREF: sub_4955B+13p
                   // ; runMainMenu+BDp ...
{
    ah = 0;

    if (byte_519C5 != 0
        || byte_519F9 != 0)
    {
//loc_4A1CF:              ; CODE XREF: sub_4A1BF+7j
        byte_50941 = 1;
        ah = 1;
    }

//loc_4A1D6:              ; CODE XREF: sub_4A1BF+Ej
    if (byte_519C8 != 0
        || byte_519F7 != 0)
    {
//loc_4A1E4:              ; CODE XREF: sub_4A1BF+1Cj
        byte_50941 = 2;
        ah = 1;
    }

//loc_4A1EB:              ; CODE XREF: sub_4A1BF+23j
    if (byte_519CD != 0
        || byte_519F8 != 0)
    {
//loc_4A1F9:              ; CODE XREF: sub_4A1BF+31j
        byte_50941 = 3;
        ah = 1;
    }

//loc_4A200:              ; CODE XREF: sub_4A1BF+38j
    if (byte_519CA != 0
        || byte_519F6 != 0)
    {
//loc_4A20E:              ; CODE XREF: sub_4A1BF+46j
        byte_50941 = 4;
        ah = 1;
    }

//loc_4A215:              ; CODE XREF: sub_4A1BF+4Dj
    if (byte_519B6 == 0
        && byte_519F5 == 0
        && byte_519F4 == 0)
    {
        return;
    }

//loc_4A22A:              ; CODE XREF: sub_4A1BF+5Bj
//                ; sub_4A1BF+62j
    if (ah == 1)
    {
        byte_50941 += 4;
        return;
    }

//loc_4A236:              ; CODE XREF: sub_4A1BF+6Ej
    byte_50941 = 9;

//locret_4A23B:               ; CODE XREF: sub_4A1BF+69j
//                ; sub_4A1BF+75j
    return;
}
/*
sub_4A23C   proc near       ; CODE XREF: sub_4955B+111p
                    ; sub_4955B+11Dp ...
        mov si, 0
        mov cx, 5A0h

loc_4A242:              ; CODE XREF: sub_4A23C+1Fj
        al = [si+1834h]
        cmp al, ah
        jz  short loc_4A253
        cmp ah, 11h
        jnz short loc_4A259
        cmp al, 0BBh ; '?'
        jnz short loc_4A259

loc_4A253:              ; CODE XREF: sub_4A23C+Cj
        mov word ptr leveldata[si], 0

loc_4A259:              ; CODE XREF: sub_4A23C+11j
                    ; sub_4A23C+15j
        inc si
        inc si
        loop    loc_4A242
        call    sub_49D53
        call    sub_48F6D
        call    sub_4A2E6
        mov byte_510B3, 0
        mov byte_5A2F9, 1
        return;
sub_4A23C   endp
*/
void findMurphy() //   proc near       ; CODE XREF: start+344p sub_4A463+22p
{
    for (int i = 0; i < kLevelSize; ++i)
    {
        if (gLevelData[i] == LevelTileTypeMurphy)
        {
            gMurphyLocation = i;
            break;
        }
    }

    sub_4A291();
}

void sub_4A291() //   proc near       ; CODE XREF: sub_4955B+686p
{
    // Parameters:
    // si: murphy location?
    bl = 0x3C; // 60
    al = al / bl;
    bl = ah;
    bh = 0;
    word_510C3 = bx;
    ah = 0;
    word_510C5 = ax;
    cl = 4;
    ax = ax << cl;
    bx = bx << cl;
    word_510E8 = bx;
    word_510EA = ax;
//    di = si[0x6155];
    si = word_5157E;
    sub_4F200();
    sub_49EBE();
    ax = word_51961;
    word_59B92 = word_51961;
    ax = word_5195F;
    word_59B90 = word_5195F;
    al = al & 7;
    gNumberOfDotsToShiftDataLeft = al;
    /*
    mov dx, 3D4h
    al = 0Dh
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (low)
    inc dx
    al = bl
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 0Ch
    out dx, al      ; Video: CRT cntrlr addr
                ; regen start address (high)
    inc dx
    al = bh
    out dx, al      ; Video: CRT controller internal registers
     */
    videoloop();
    return;
}

void sub_4A2E6() //   proc near       ; CODE XREF: start+33Bp runLevel+ADp ...
{
    bx = 0;
    dx = 0;
    cx = 0x5A0; // 1440 = 120 * 12 -> width = 120, height = 12
//    mov si, offset leveldata
    uint16_t *levelData = NULL;
    uint16_t numberOfSomething = 0; // this is bx, probably a return value but I don't see it used anywhere???

    for (int i = 0; i < 1440; ++i)
    {
//loc_4A2F0:              ; CODE XREF: sub_4A2E6+D1j
        uint16_t *currentEntry = &levelData[i];
        uint16_t value = *currentEntry;
        uint8_t valueLow = (value & 0xFF);
        numberOfSomething++;

    //    ax = si[0];
        if (valueLow == 0xF1)
        {
//            si[0] = 0x1F;
            continue; // jmp short loc_4A3B0
        }

//loc_4A2FC:              ; CODE XREF: sub_4A2E6+Ej
        if (byte_5A33F != 1)
        {
            if (value == 4)
            {
//loc_4A33C:              ; CODE XREF: sub_4A2E6+20j
                dx++;
                continue; // jmp short loc_4A3B0
            }
        }
        if (byte_5A33F == 1 || value != 0x11) //jz  short loc_4A34B
        {
            if (byte_5A33F == 1 || value != 0x18) //jz  short loc_4A379
            {
//loc_4A312:              ; CODE XREF: sub_4A2E6+1Bj
                if (value == 0x1A
                    || value == 0x1B
                    || value == 0x26
                    || value == 0x27)
                {
//loc_4A33F:              ; CODE XREF: sub_4A2E6+2Fj
//                ; sub_4A2E6+34j ...
                    levelData[i] = 5; // mov word ptr [si], 5
                    continue; // jmp short loc_4A3B0
                }
                if (value >= 0x1C)
                {
                    if (value < 0x26)
                    {
//loc_4A345:              ; CODE XREF: sub_4A2E6+48j
                        levelData[i] = 6; // mov word ptr [si], 6
                        continue; // jmp short loc_4A3B0
                    }
                }

//loc_4A330:              ; CODE XREF: sub_4A2E6+43j
                if (value >= 0xD
                    && value < 0x11)
                {
//loc_4A3A7:              ; CODE XREF: sub_4A2E6+52j
                    levelData[i] = 0x0100 + (valueLow - 4);
//                    si[0] -= 4; // sub byte ptr [si], 4
//                    si[1] = 1; // mov byte ptr [si+1], 1
                    // This -- is to compensate that I'm doing the ++ at the beginning of the loop, instead of at
                    // the end like the original asm code. If I put it at the end, this break would skip the ++,
                    // but would make the rest of the code much more complex. In any case I don't think that number
                    // is used anywhere so I'll probably delete it later...
                    //
                    numberOfSomething--;
        //        jmp short $+2 // wtf this was right above loc_4A3B0
                    break;
                }
                else
                {
//loc_4A33A:              ; CODE XREF: sub_4A2E6+4Dj
                    continue; // jmp short loc_4A3B0
                }
            }
        }
        if (value != 0x18) //jz  short loc_4A379
        {
//loc_4A34B:              ; CODE XREF: sub_4A2E6+25j
            if (levelData[i - 1] == 0) //cmp word ptr [si-2], 0
            {
                levelData[i] = 0x0100 + valueLow;
//                si[1] = 1; //mov byte ptr [si+1], 1
                continue; // jmp short loc_4A3B0
            }
//loc_4A357:              ; CODE XREF: sub_4A2E6+69j
        // 0x78 = 120
            if (levelData[i - 60] == 0) //cmp word ptr [si-78h], 0
            {
                levelData[i - 60] = 0x1011; // 4113
                levelData[i] = 0xFFFF; // mov word ptr [si], 0FFFFh
                continue; // jmp short loc_4A3B0
            }
//loc_4A368:              ; CODE XREF: sub_4A2E6+75j
            if (levelData[i + 1] == 0) //cmp word ptr [si+2], 0
            {
                levelData[i + 1] = 0x2811; // mov word ptr [si+2], 2811h
                levelData[i] = 0xFFFF; // mov word ptr [si], 0FFFFh
                continue; // jmp short loc_4A3B0
            }
        }
//loc_4A379:              ; CODE XREF: sub_4A2E6+2Aj
        if (levelData[i - 1] == 0) //cmp word ptr [si-2], 0
        {
//          si[1] = 1; //mov byte ptr [si+1], 1
            continue; // jmp short loc_4A3B0
        }
//loc_4A385:              ; CODE XREF: sub_4A2E6+97j
        if (levelData[i - 60] == 0) //cmp word ptr [si-78h], 0
        {
            levelData[i - 60] = 0x1018; // mov word ptr [si-78h], 1018h
            levelData[i] = 0xFFFF; // mov word ptr [si], 0FFFFh
            continue; // jmp short loc_4A3B0
        }
//loc_4A396:              ; CODE XREF: sub_4A2E6+A3j
        if (levelData[i + 1] == 0) //cmp word ptr [si+2], 0
        {
            levelData[i + 1] = 0x2818; // mov word ptr [si+2], 2818h
            levelData[i] = 0xFFFF; // mov word ptr [si], 0FFFFh
            continue; // jmp short loc_4A3B0
        }

//loc_4A3B0:              ; CODE XREF: sub_4A2E6+13j
//                ; sub_4A2E6:loc_4A33Aj ...
        bx++;
    }

//locret_4A3BA:               ; CODE XREF: sub_4A2E6+CFj
    return;
}

void sub_4A3BB() //   proc near       ; CODE XREF: start+33Ep sub_4A463+17p
{
    if (byte_51036 != 0)
    {
        dl = byte_51036;
    }

//loc_4A3C6:              ; CODE XREF: sub_4A3BB+5j
    byte_5195A = dl;
    byte_5195B = dl;
    sub_4FD21();
}

/*sub_4A3D2   proc near       ; CODE XREF: sub_4955B+39Ep
                    ; sub_4955B+3EBp
        mov byte_599D4, 0
        mov word_599D8, 0
        cmp byte_5A33E, 0
        mov byte_5A33E, 0
        jnz short $+12
sub_4A3D2   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4A3E9   proc near       ; CODE XREF: sub_4955B+14Ep
        cmp byte_5A33E, 0
        jnz short loc_4A3F3
        call    sub_4A95F

loc_4A3F3:              ; CODE XREF: sub_4A3D2+15j
                    ; sub_4A3E9+5j
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp byte_5A33E, 0
        jz  short loc_4A427
        mov byte_510DE, 1

loc_4A427:              ; CODE XREF: sub_4A3E9+37j
        mov byte_5A33F, 0
        call    sub_4A463
        mov byte_5A33F, 1
        cmp byte_5A33E, 1
        jb  short loc_4A446
        mov byte_510DE, 0
        jnz short loc_4A446
        inc byte_5A33E

loc_4A446:              ; CODE XREF: sub_4A3E9+50j
                    ; sub_4A3E9+57j
        mov byte_50941, 0
        cmp byte_510DE, 0
        jz  short locret_4A462
        mov bx, word_5A33C
        mov word_510DF, bx
        mov byte_510E2, 1
        call    sub_492A8

locret_4A462:               ; CODE XREF: sub_4A3E9+67j
        return;
sub_4A3E9   endp


; =============== S U B R O U T I N E =======================================


sub_4A463   proc near       ; CODE XREF: sub_4945D:loc_4953Bp
                    ; sub_4A3E9+43p
        call    readLevels
        call    sub_4D464
        call    sub_48F6D
        call    sub_501C0
        neg byte_5A33F
        call    sub_4A2E6
        neg byte_5A33F
        call    sub_4A3BB
        mov byte_59B7B, 1
        call    sub_48A20
        call    findMurphy
        return;
sub_4A463   endp


; =============== S U B R O U T I N E =======================================


movefun3  proc near       ; DATA XREF: data:161Ao
        cmp byte ptr leveldata[si], 8
        jz  short loc_4A491
        return;
// ; ---------------------------------------------------------------------------

loc_4A491:              ; CODE XREF: movefun3+5j
        mov ax, [si+1834h]
        cmp ax, 3008h
        jge short loc_4A4D4
        cmp ax, 2008h
        jge short loc_4A4B4
        mov ax, [si+18ACh]
        cmp ax, 0
        jz  short loc_4A4A9
        return;
// ; ---------------------------------------------------------------------------

loc_4A4A9:              ; CODE XREF: movefun3+1Dj
        mov byte ptr [si+1835h], 20h ; ' '
        mov byte ptr [si+18ADh], 8
        return;
// ; ---------------------------------------------------------------------------

loc_4A4B4:              ; CODE XREF: movefun3+14j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4A4C2
        mov word ptr leveldata[si], 8
        return;
// ; ---------------------------------------------------------------------------

loc_4A4C2:              ; CODE XREF: movefun3+30j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 22h ; '"'
        jnz short loc_4A4CF
        mov bl, 30h ; '0'

loc_4A4CF:              ; CODE XREF: movefun3+42j
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4A4D4:              ; CODE XREF: movefun3+Fj
        push    si
        mov bl, [si+1835h]
        xor bh, bh
        al = bl
        shl bx, 1
        ;and bx, byte ptr 0Fh
        db 83h, 0E3h, 0Fh
        mov di, [si+6155h]
        shl bx, 1
        add di, [bx+6C95h]
        mov si, 12F6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4A4F9:              ; CODE XREF: movefun3+78j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A4F9
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        al = bl
        and al, 7
        jz  short loc_4A516
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4A516:              ; CODE XREF: movefun3+86j
        mov word ptr leveldata[si], 0
        mov word ptr [si+18ACh], 8
        add si, 78h ; 'x'
        cmp word ptr [si+18ACh], 0
        jnz short loc_4A537
        mov byte ptr [si+1835h], 30h ; '0'
        mov byte ptr [si+18ADh], 8
        return;
// ; ---------------------------------------------------------------------------

loc_4A537:              ; CODE XREF: movefun3+A1j
        cmp byte ptr [si+18ACh], 1Fh
        jnz short loc_4A53F
        return;
// ; ---------------------------------------------------------------------------

loc_4A53F:              ; CODE XREF: movefun3+B3j
        call    sub_4A61F
        return;
movefun3  endp

// ; ---------------------------------------------------------------------------

loc_4A543:              ; DATA XREF: data:1648o
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4A54B
        return;
// ; ---------------------------------------------------------------------------

loc_4A54B:              ; CODE XREF: code:3928j
        mov ax, word_5195D
        and ax, 3
        cmp ax, 0
        jz  short loc_4A557
        return;
// ; ---------------------------------------------------------------------------

loc_4A557:              ; CODE XREF: code:3934j
        mov bl, [si+1835h]
        test    bl, 80h
        jnz short loc_4A5A0
        inc bl
        mov [si+1835h], bl
        push    si
        push    bx
        xor bh, bh
        dec bl
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 12D6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A582:              ; CODE XREF: code:396Aj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A582
        pop ds
        pop bx
        pop si
        cmp bl, 8
        jnz short locret_4A59F
        mov word ptr leveldata[si], 0
        mov byte_510C0, 0

locret_4A59F:               ; CODE XREF: code:3972j
        return;
// ; ---------------------------------------------------------------------------

loc_4A5A0:              ; CODE XREF: code:393Ej
        inc bl
        cmp bl, 89h ; '?'
        jnz short loc_4A5B3
        mov word ptr leveldata[si], 4
        mov byte_510C0, 0
        return;
// ; ---------------------------------------------------------------------------

loc_4A5B3:              ; CODE XREF: code:3985j
        mov [si+1835h], bl
        xor bh, bh
        dec bl
        and bl, 0Fh
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 12E6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A5D4:              ; CODE XREF: code:39BCj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A5D4
        pop ds
        return;

; =============== S U B R O U T I N E =======================================


sub_4A5E0   proc near       ; CODE XREF: runLevel+106p
        mov si, 0
        mov bx, 0
        mov cx, 5A0h

loc_4A5E9:              ; CODE XREF: sub_4A5E0+25j
        cmp byte ptr [bx+2434h], 0
        jz  short loc_4A601
        jl  short loc_4A608
        dec byte ptr [bx+2434h]
        jnz short loc_4A601
        push    si
        push(cx);
        push    bx
        call    sub_4A61F
        pop bx
        pop(cx);
        pop si

loc_4A601:              ; CODE XREF: sub_4A5E0+Ej
                    ; sub_4A5E0+16j ...
        add si, 2
        inc bx
        loop    loc_4A5E9
        return;
// ; ---------------------------------------------------------------------------

loc_4A608:              ; CODE XREF: sub_4A5E0+10j
        inc byte ptr [bx+2434h]
        jnz short loc_4A601
        push    si
        push(cx);
        push    bx
        mov word ptr leveldata[si], 0FF18h
        call    sub_4A61F
        pop bx
        pop(cx);
        pop si
        jmp short loc_4A601
sub_4A5E0   endp


; =============== S U B R O U T I N E =======================================


sub_4A61F   proc near       ; CODE XREF: movefun+271p
                    ; movefun2+20Fp ...
        cmp byte ptr leveldata[si], 6
        jnz short loc_4A627
        return;
// ; ---------------------------------------------------------------------------

loc_4A627:              ; CODE XREF: sub_4A61F+5j
        mov byte_510C0, 1
        cmp byte ptr leveldata[si], 3
        jnz short loc_4A639
        mov word_510D1, 1

loc_4A639:              ; CODE XREF: sub_4A61F+12j
        cmp byte ptr leveldata[si], 18h
        jnz short loc_4A647
        mov cx, 801Fh
        mov dl, 0F3h ; '?'
        jmp short loc_4A64C
// ; ---------------------------------------------------------------------------

loc_4A647:              ; CODE XREF: sub_4A61F+1Fj
        mov cx, 1Fh
        mov dl, 0Dh

loc_4A64C:              ; CODE XREF: sub_4A61F+26j
        mov bx, si
        shr bx, 1
        mov dh, dl
        push(cx);
        mov ax, leveldata[si-78h-2h]
        cmp al, 8
        jz  short loc_4A680
        cmp al, 12h
        jz  short loc_4A680
        cmp al, 11h
        jz  short loc_4A680
        cmp al, 1
        jz  short loc_4A69C
        cmp al, 4
        jz  short loc_4A692
        cmp al, 18h
        jnz short loc_4A676
        neg dh
        mov cx, 801Fh
        jmp short loc_4A680
// ; ---------------------------------------------------------------------------

loc_4A676:              ; CODE XREF: sub_4A61F+4Ej
        cmp al, 3
        jnz short loc_4A688
        mov word_510D1, 1

loc_4A680:              ; CODE XREF: sub_4A61F+3Aj
                    ; sub_4A61F+3Ej ...
        cmp al, 6
        jz  short loc_4A688
        mov [bx+23F7h], dh

loc_4A688:              ; CODE XREF: sub_4A61F+59j
                    ; sub_4A61F+63j
        cmp al, 6
        jz  short loc_4A690
        mov [si+17BAh], cx

loc_4A690:              ; CODE XREF: sub_4A61F+6Bj
        jmp short loc_4A6A6
// ; ---------------------------------------------------------------------------

loc_4A692:              ; CODE XREF: sub_4A61F+4Aj
        push    si
        add si, 0FF86h
        call    sub_4AA34
        pop si
        jmp short loc_4A6A6
// ; ---------------------------------------------------------------------------

loc_4A69C:              ; CODE XREF: sub_4A61F+46j
        push    si
        add si, 0FF86h
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A6A6:              ; CODE XREF: sub_4A61F:loc_4A690j
                    ; sub_4A61F+7Bj ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+17BCh]
        cmp al, 8
        jz  short loc_4A6D7
        cmp al, 12h
        jz  short loc_4A6D7
        cmp al, 11h
        jz  short loc_4A6D7
        cmp al, 1
        jz  short loc_4A6F3
        cmp al, 4
        jz  short loc_4A6E9
        cmp al, 18h
        jnz short loc_4A6CD
        neg dh
        mov cx, 801Fh
        jmp short loc_4A6D7
// ; ---------------------------------------------------------------------------

loc_4A6CD:              ; CODE XREF: sub_4A61F+A5j
        cmp al, 3
        jnz short loc_4A6DF
        mov word_510D1, 1

loc_4A6D7:              ; CODE XREF: sub_4A61F+91j
                    ; sub_4A61F+95j ...
        cmp al, 6
        jz  short loc_4A6DF
        mov [bx+23F8h], dh

loc_4A6DF:              ; CODE XREF: sub_4A61F+B0j
                    ; sub_4A61F+BAj
        cmp al, 6
        jz  short loc_4A6E7
        mov [si+17BCh], cx

loc_4A6E7:              ; CODE XREF: sub_4A61F+C2j
        jmp short loc_4A6FD
// ; ---------------------------------------------------------------------------

loc_4A6E9:              ; CODE XREF: sub_4A61F+A1j
        push    si
        add si, 0FF88h
        call    sub_4AA34
        pop si
        jmp short loc_4A6FD
// ; ---------------------------------------------------------------------------

loc_4A6F3:              ; CODE XREF: sub_4A61F+9Dj
        push    si
        add si, 0FF88h
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A6FD:              ; CODE XREF: sub_4A61F:loc_4A6E7j
                    ; sub_4A61F+D2j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+17BEh]
        cmp al, 8
        jz  short loc_4A72E
        cmp al, 12h
        jz  short loc_4A72E
        cmp al, 11h
        jz  short loc_4A72E
        cmp al, 1
        jz  short loc_4A74A
        cmp al, 4
        jz  short loc_4A740
        cmp al, 18h
        jnz short loc_4A724
        neg dh
        mov cx, 801Fh
        jmp short loc_4A72E
// ; ---------------------------------------------------------------------------

loc_4A724:              ; CODE XREF: sub_4A61F+FCj
        cmp al, 3
        jnz short loc_4A736
        mov word_510D1, 1

loc_4A72E:              ; CODE XREF: sub_4A61F+E8j
                    ; sub_4A61F+ECj ...
        cmp al, 6
        jz  short loc_4A736
        mov [bx+23F9h], dh

loc_4A736:              ; CODE XREF: sub_4A61F+107j
                    ; sub_4A61F+111j
        cmp al, 6
        jz  short loc_4A73E
        mov [si+17BEh], cx

loc_4A73E:              ; CODE XREF: sub_4A61F+119j
        jmp short loc_4A754
// ; ---------------------------------------------------------------------------

loc_4A740:              ; CODE XREF: sub_4A61F+F8j
        push    si
        add si, 0FF8Ah
        call    sub_4AA34
        pop si
        jmp short loc_4A754
// ; ---------------------------------------------------------------------------

loc_4A74A:              ; CODE XREF: sub_4A61F+F4j
        push    si
        add si, 0FF8Ah
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A754:              ; CODE XREF: sub_4A61F:loc_4A73Ej
                    ; sub_4A61F+129j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+1832h]
        cmp al, 8
        jz  short loc_4A785
        cmp al, 12h
        jz  short loc_4A785
        cmp al, 11h
        jz  short loc_4A785
        cmp al, 1
        jz  short loc_4A7A1
        cmp al, 4
        jz  short loc_4A797
        cmp al, 18h
        jnz short loc_4A77B
        neg dh
        mov cx, 801Fh
        jmp short loc_4A785
// ; ---------------------------------------------------------------------------

loc_4A77B:              ; CODE XREF: sub_4A61F+153j
        cmp al, 3
        jnz short loc_4A78D
        mov word_510D1, 1

loc_4A785:              ; CODE XREF: sub_4A61F+13Fj
                    ; sub_4A61F+143j ...
        cmp al, 6
        jz  short loc_4A78D
        mov [bx+2433h], dh

loc_4A78D:              ; CODE XREF: sub_4A61F+15Ej
                    ; sub_4A61F+168j
        cmp al, 6
        jz  short loc_4A795
        mov [si+1832h], cx

loc_4A795:              ; CODE XREF: sub_4A61F+170j
        jmp short loc_4A7AB
// ; ---------------------------------------------------------------------------

loc_4A797:              ; CODE XREF: sub_4A61F+14Fj
        push    si
        add si, 0FFFEh
        call    sub_4AA34
        pop si
        jmp short loc_4A7AB
// ; ---------------------------------------------------------------------------

loc_4A7A1:              ; CODE XREF: sub_4A61F+14Bj
        push    si
        add si, 0FFFEh
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A7AB:              ; CODE XREF: sub_4A61F:loc_4A795j
                    ; sub_4A61F+180j ...
        pop(cx);
        mov [si+1834h], cx
        mov dh, dl
        push(cx);
        mov ax, [si+1836h]
        cmp al, 8
        jz  short loc_4A7E0
        cmp al, 12h
        jz  short loc_4A7E0
        cmp al, 11h
        jz  short loc_4A7E0
        cmp al, 1
        jz  short loc_4A7FC
        cmp al, 4
        jz  short loc_4A7F2
        cmp al, 18h
        jnz short loc_4A7D6
        neg dh
        mov cx, 801Fh
        jmp short loc_4A7E0
// ; ---------------------------------------------------------------------------

loc_4A7D6:              ; CODE XREF: sub_4A61F+1AEj
        cmp al, 3
        jnz short loc_4A7E8
        mov word_510D1, 1

loc_4A7E0:              ; CODE XREF: sub_4A61F+19Aj
                    ; sub_4A61F+19Ej ...
        cmp al, 6
        jz  short loc_4A7E8
        mov [bx+2435h], dh

loc_4A7E8:              ; CODE XREF: sub_4A61F+1B9j
                    ; sub_4A61F+1C3j
        cmp al, 6
        jz  short loc_4A7F0
        mov [si+1836h], cx

loc_4A7F0:              ; CODE XREF: sub_4A61F+1CBj
        jmp short loc_4A806
// ; ---------------------------------------------------------------------------

loc_4A7F2:              ; CODE XREF: sub_4A61F+1AAj
        push    si
        add si, 2
        call    sub_4AA34
        pop si
        jmp short loc_4A806
// ; ---------------------------------------------------------------------------

loc_4A7FC:              ; CODE XREF: sub_4A61F+1A6j
        push    si
        add si, 2
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A806:              ; CODE XREF: sub_4A61F:loc_4A7F0j
                    ; sub_4A61F+1DBj ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18AAh]
        cmp al, 8
        jz  short loc_4A837
        cmp al, 12h
        jz  short loc_4A837
        cmp al, 11h
        jz  short loc_4A837
        cmp al, 1
        jz  short loc_4A853
        cmp al, 4
        jz  short loc_4A849
        cmp al, 18h
        jnz short loc_4A82D
        neg dh
        mov cx, 801Fh
        jmp short loc_4A837
// ; ---------------------------------------------------------------------------

loc_4A82D:              ; CODE XREF: sub_4A61F+205j
        cmp al, 3
        jnz short loc_4A83F
        mov word_510D1, 1

loc_4A837:              ; CODE XREF: sub_4A61F+1F1j
                    ; sub_4A61F+1F5j ...
        cmp al, 6
        jz  short loc_4A83F
        mov [bx+246Fh], dh

loc_4A83F:              ; CODE XREF: sub_4A61F+210j
                    ; sub_4A61F+21Aj
        cmp al, 6
        jz  short loc_4A847
        mov [si+18AAh], cx

loc_4A847:              ; CODE XREF: sub_4A61F+222j
        jmp short loc_4A85D
// ; ---------------------------------------------------------------------------

loc_4A849:              ; CODE XREF: sub_4A61F+201j
        push    si
        add si, 76h ; 'v'
        call    sub_4AA34
        pop si
        jmp short loc_4A85D
// ; ---------------------------------------------------------------------------

loc_4A853:              ; CODE XREF: sub_4A61F+1FDj
        push    si
        add si, 76h ; 'v'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A85D:              ; CODE XREF: sub_4A61F:loc_4A847j
                    ; sub_4A61F+232j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18ACh]
        cmp al, 8
        jz  short loc_4A88E
        cmp al, 12h
        jz  short loc_4A88E
        cmp al, 11h
        jz  short loc_4A88E
        cmp al, 1
        jz  short loc_4A8AA
        cmp al, 4
        jz  short loc_4A8A0
        cmp al, 18h
        jnz short loc_4A884
        neg dh
        mov cx, 801Fh
        jmp short loc_4A88E
// ; ---------------------------------------------------------------------------

loc_4A884:              ; CODE XREF: sub_4A61F+25Cj
        cmp al, 3
        jnz short loc_4A896
        mov word_510D1, 1

loc_4A88E:              ; CODE XREF: sub_4A61F+248j
                    ; sub_4A61F+24Cj ...
        cmp al, 6
        jz  short loc_4A896
        mov [bx+2470h], dh

loc_4A896:              ; CODE XREF: sub_4A61F+267j
                    ; sub_4A61F+271j
        cmp al, 6
        jz  short loc_4A89E
        mov [si+18ACh], cx

loc_4A89E:              ; CODE XREF: sub_4A61F+279j
        jmp short loc_4A8B4
// ; ---------------------------------------------------------------------------

loc_4A8A0:              ; CODE XREF: sub_4A61F+258j
        push    si
        add si, 78h ; 'x'
        call    sub_4AA34
        pop si
        jmp short loc_4A8B4
// ; ---------------------------------------------------------------------------

loc_4A8AA:              ; CODE XREF: sub_4A61F+254j
        push    si
        add si, 78h ; 'x'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A8B4:              ; CODE XREF: sub_4A61F:loc_4A89Ej
                    ; sub_4A61F+289j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18AEh]
        cmp al, 8
        jz  short loc_4A8E5
        cmp al, 12h
        jz  short loc_4A8E5
        cmp al, 11h
        jz  short loc_4A8E5
        cmp al, 1
        jz  short loc_4A901
        cmp al, 4
        jz  short loc_4A8F7
        cmp al, 18h
        jnz short loc_4A8DB
        neg dh
        mov cx, 801Fh
        jmp short loc_4A8E5
// ; ---------------------------------------------------------------------------

loc_4A8DB:              ; CODE XREF: sub_4A61F+2B3j
        cmp al, 3
        jnz short loc_4A8ED
        mov word_510D1, 1

loc_4A8E5:              ; CODE XREF: sub_4A61F+29Fj
                    ; sub_4A61F+2A3j ...
        cmp al, 6
        jz  short loc_4A8ED
        mov [bx+2471h], dh

loc_4A8ED:              ; CODE XREF: sub_4A61F+2BEj
                    ; sub_4A61F+2C8j
        cmp al, 6
        jz  short loc_4A8F5
        mov [si+18AEh], cx

loc_4A8F5:              ; CODE XREF: sub_4A61F+2D0j
        jmp short loc_4A90B
// ; ---------------------------------------------------------------------------

loc_4A8F7:              ; CODE XREF: sub_4A61F+2AFj
        push    si
        add si, 7Ah ; 'z'
        call    sub_4AA34
        pop si
        jmp short loc_4A90B
// ; ---------------------------------------------------------------------------

loc_4A901:              ; CODE XREF: sub_4A61F+2ABj
        push    si
        add si, 7Ah ; 'z'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A90B:              ; CODE XREF: sub_4A61F:loc_4A8F5j
                    ; sub_4A61F+2E0j ...
        pop(cx);
        call    sound4
        return;
sub_4A61F   endp


; =============== S U B R O U T I N E =======================================


sub_4A910   proc near       ; CODE XREF: runLevel:noFlashing3p
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        al = byte_510DB
        cmp al, 1
        jle short loc_4A954
        mov si, word_510DC
        cmp word ptr leveldata[si], 0
        jnz short loc_4A932
        mov word ptr leveldata[si], 14h

loc_4A932:              ; CODE XREF: sub_4A910+1Aj
        mov di, [si+6155h]
        mov si, word_5177E
        call    sub_4F200
        inc byte_510DB
        cmp byte_510DB, 28h ; '('
        jl  short loc_4A954
        mov si, word_510DC
        call    sub_4A61F
        mov byte_510DB, 0

loc_4A954:              ; CODE XREF: sub_4A910+Fj
                    ; sub_4A910+36j
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        return;
sub_4A910   endp


; =============== S U B R O U T I N E =======================================


sub_4A95F   proc near       ; CODE XREF: runLevel+372p
                    ; sub_4A3E9+7p ...
        al = byte_510B0
        mov byte_510B4, al
        al = byte_510B1
        mov byte_510B5, al
        al = byte_510B2
        mov byte_510B6, al
        cmp byte_510DE, 0
        jnz short locret_4A97F
        cmp byte_599D4, 0
        jz  short loc_4A980

locret_4A97F:               ; CODE XREF: sub_4A95F+17j
        return;
// ; ---------------------------------------------------------------------------

loc_4A980:              ; CODE XREF: sub_4A95F+1Ej
        mov bh, gCurrentPlayerIndex
        xor bl, bl
        shr bx, 1
        mov si, bx
        add si, gPlayerListData
        al = byte_510B4
        add al, [si+0Bh]

loc_4A994:              ; CODE XREF: sub_4A95F+3Ej
        cmp al, 3Ch ; '<'
        jl  short loc_4A99F
        sub al, 3Ch ; '<'
        inc byte ptr [si+0Ah]
        jmp short loc_4A994
// ; ---------------------------------------------------------------------------

loc_4A99F:              ; CODE XREF: sub_4A95F+37j
        mov [si+0Bh], al
        al = byte_510B5
        add al, [si+0Ah]

loc_4A9A8:              ; CODE XREF: sub_4A95F+52j
        cmp al, 3Ch ; '<'
        jl  short loc_4A9B3
        sub al, 3Ch ; '<'
        inc byte ptr [si+9]
        jmp short loc_4A9A8
// ; ---------------------------------------------------------------------------

loc_4A9B3:              ; CODE XREF: sub_4A95F+4Bj
        mov [si+0Ah], al
        al = byte_510B6
        add al, [si+9]
        jnb short loc_4A9C0
        al = 0FFh

loc_4A9C0:              ; CODE XREF: sub_4A95F+5Dj
        mov [si+9], al
        return;
sub_4A95F   endp


; =============== S U B R O U T I N E =======================================


sub_4A9C4   proc near       ; CODE XREF: sub_4A61F+81p
                    ; sub_4A61F+D8p ...
        mov [si+1834h], cx
        and ah, 0F0h
        cmp ah, 10h
        jz  short loc_4A9EF
        cmp ah, 70h ; 'p'
        jz  short loc_4A9EF
        cmp ah, 20h ; ' '
        jz  short loc_4AA05
        cmp ah, 30h ; '0'
        jz  short loc_4AA12
        cmp ah, 50h ; 'P'
        jz  short loc_4AA1F
        cmp ah, 60h ; '`'
        jz  short loc_4AA26
        cmp ah, 70h ; 'p'
        jz  short loc_4AA2D
        return;
// ; ---------------------------------------------------------------------------

loc_4A9EF:              ; CODE XREF: sub_4A9C4+Aj sub_4A9C4+Fj
        sub si, 78h ; 'x'
        call    sub_4AAB4
        add si, 0F0h ; '?'
        cmp word ptr leveldata[si], 9999h
        jnz short locret_4AA04
        call    sub_4AAB4

locret_4AA04:               ; CODE XREF: sub_4A9C4+3Bj
        return;
// ; ---------------------------------------------------------------------------

loc_4AA05:              ; CODE XREF: sub_4A9C4+14j
        add si, 2
        call    sub_4AAB4
        add si, 76h ; 'v'
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AA12:              ; CODE XREF: sub_4A9C4+19j
        sub si, 2
        call    sub_4AAB4
        add si, 7Ah ; 'z'
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AA1F:              ; CODE XREF: sub_4A9C4+1Ej
        sub si, 2
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AA26:              ; CODE XREF: sub_4A9C4+23j
        add si, 2
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AA2D:              ; CODE XREF: sub_4A9C4+28j
        add si, 78h ; 'x'
        call    sub_4AAB4
        return;
sub_4A9C4   endp


; =============== S U B R O U T I N E =======================================


sub_4AA34   proc near       ; CODE XREF: sub_4A61F+77p
                    ; sub_4A61F+CEp ...
        mov leveldata[si], cx
        and ah, 0F0h
        cmp ah, 10h
        jz  short loc_4AA5F
        cmp ah, 70h ; 'p'
        jz  short loc_4AA5F
        cmp ah, 20h ; ' '
        jz  short loc_4AA75
        cmp ah, 30h ; '0'
        jz  short loc_4AA8A
        cmp ah, 50h ; 'P'
        jz  short loc_4AA9F
        cmp ah, 60h ; '`'
        jz  short loc_4AAA6
        cmp ah, 70h ; 'p'
        jz  short loc_4AAAD
        return;
// ; ---------------------------------------------------------------------------

loc_4AA5F:              ; CODE XREF: sub_4AA34+Aj sub_4AA34+Fj
        sub si, 78h ; 'x'
        call    sub_4AAB4
        add si, 0F0h ; '?'
        cmp leveldata[si], 9999h
        jnz short locret_4AA74
        call    sub_4AAB4

locret_4AA74:               ; CODE XREF: sub_4AA34+3Bj
        return;
// ; ---------------------------------------------------------------------------

loc_4AA75:              ; CODE XREF: sub_4AA34+14j
        add si, 2
        call    sub_4AAB4
        add si, 76h ; 'v'
        cmp leveldata[si], 9999h
        jnz short locret_4AA89
        call    sub_4AAB4

locret_4AA89:               ; CODE XREF: sub_4AA34+50j
        return;
// ; ---------------------------------------------------------------------------

loc_4AA8A:              ; CODE XREF: sub_4AA34+19j
        sub si, 2
        call    sub_4AAB4
        add si, 7Ah ; 'z'
        cmp word ptr leveldata[si], 9999h
        jnz short locret_4AA9E
        call    sub_4AAB4

locret_4AA9E:               ; CODE XREF: sub_4AA34+65j
        return;
// ; ---------------------------------------------------------------------------

loc_4AA9F:              ; CODE XREF: sub_4AA34+1Ej
        sub si, 2
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AAA6:              ; CODE XREF: sub_4AA34+23j
        add si, 2
        call    sub_4AAB4
        return;
// ; ---------------------------------------------------------------------------

loc_4AAAD:              ; CODE XREF: sub_4AA34+28j
        add si, 78h ; 'x'
        call    sub_4AAB4
        return;
sub_4AA34   endp


; =============== S U B R O U T I N E =======================================


sub_4AAB4   proc near       ; CODE XREF: sub_4A9C4+2Ep
                    ; sub_4A9C4+3Dp ...
        cmp byte ptr leveldata[si], 1Fh
        jnz short loc_4AABC
        return;
// ; ---------------------------------------------------------------------------

loc_4AABC:              ; CODE XREF: sub_4AAB4+5j
        mov word ptr leveldata[si], 0
        push    si
        push    ds
        mov di, [si+6155h]
        mov si, word_51580
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4AAD3:              ; CODE XREF: sub_4AAB4+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4AAD3
        pop ds
        pop si
        return;
sub_4AAB4   endp
*/

void readMenuDat() // proc near       ; CODE XREF: readEverything+9p
{
    FILE *file = fopen("MENU.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening MENU.DAT\n");
    }

//loc_4AAED:             // ; CODE XREF: readMenuDat+8j

    size_t bytes = fread(gMenuBitmapData, sizeof(uint8_t), sizeof(gMenuBitmapData), file);
    if (bytes < sizeof(gMenuBitmapData))
    {
        exitWithError("Error reading MENU.DAT\n");
    }

//loc_4AB0B:              // ; CODE XREF: readMenuDat+25j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MENU.DAT\n");
    }

// readMenuDat endp
}

#define SDL_SCANCODE_TO_CHAR_CASE(scancode, char) case scancode: return char

char characterForSDLScancode(SDL_Scancode scancode)
{
    // 0x16FA points to what seems to be a map from key code (http://stanislavs.org/helppc/make_codes.html) to ASCII
    //        0B5D:16FA     00 00 31 32 33 34 35 36 37 38 39 30 2D 00 08 00  ..1234567890-...
    //        0B5D:170A     51 57 45 52 54 59 55 49 4F 50 00 00 0A 00 41 53  QWERTYUIOP....AS
    //        0B5D:171A     44 46 47 48 4A 4B 4C 00 00 00 00 00 5A 58 43 56  DFGHJKL.....ZXCV
    //        0B5D:172A     42 4E 4D 00 00 00 00 00 00 20 00 00 00 00 00 00  BNM...... ......
    //        0B5D:173A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:174A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:175A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:176A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:177A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:178A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................

    switch (scancode)
    {
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_0, '0');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_1, '1');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_2, '2');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_3, '3');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_4, '4');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_5, '5');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_6, '6');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_7, '7');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_8, '8');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_9, '9');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_Q, 'Q');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_W, 'W');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_E, 'E');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_R, 'R');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_T, 'T');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_Y, 'Y');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_U, 'U');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_I, 'I');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_O, 'O');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_P, 'P');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_A, 'A');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_S, 'S');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_D, 'D');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_F, 'F');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_G, 'G');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_H, 'H');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_J, 'J');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_K, 'K');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_L, 'L');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_Z, 'Z');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_X, 'X');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_C, 'C');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_V, 'V');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_B, 'B');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_N, 'N');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_M, 'M');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_SPACE, ' ');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_MINUS, '-');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_BACKSPACE, '\b');
        SDL_SCANCODE_TO_CHAR_CASE(SDL_SCANCODE_RETURN, '\n');

        default:
            return '\0';
    }
}

void handleNewPlayerOptionClick() // sub_4AB1B  proc near       ; CODE XREF: runMainMenu+28Fp
//                    ; DATA XREF: data:off_50318o
{
    // 01ED:3EB8
    if (byte_59B85 != 0)
    {
        //jnz short loc_4AB4A
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "PLAYER LIST FULL       ");
        return;
    }

    int newPlayerIndex = -1;

    for (int i = 0; i < kNumberOfPlayers; ++i)
    {
        PlayerEntry currentPlayerEntry = gPlayerListData[i];

//loc_4AB2D:              ; CODE XREF: handleNewPlayerOptionClick+2Dj
        if (strcmp(currentPlayerEntry.name, "--------") == 0)
        {
            newPlayerIndex = i;
            break;
        }
//loc_4AB42:              ; CODE XREF: handleNewPlayerOptionClick+14j
//                ; handleNewPlayerOptionClick+19j ...
    }

    if (newPlayerIndex == -1)
    {
//loc_4AB4A:              ; CODE XREF: handleNewPlayerOptionClick+5j
//        mov di, 89F7h
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "PLAYER LIST FULL       ");
        return;
    }

//loc_4AB56:              ; CODE XREF: handleNewPlayerOptionClick+25j
    gNewPlayerEntryIndex = newPlayerIndex;
    restoreLastMouseAreaBitmap();
//    mov di, 89F7h
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, "YOUR NAME:             ");

    char newPlayerName[9] = "        ";
    gNewPlayerNameLength = 0;

    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    do
    {
//loc_4AB7F:              ; CODE XREF: handleNewPlayerOptionClick+6Aj
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);

    SDL_Scancode lastPressedKey = SDL_SCANCODE_UNKNOWN;

    do
    {
//noKeyPressed:               ; CODE XREF: handleNewPlayerOptionClick+79j
//                ; handleNewPlayerOptionClick+8Aj ...
        videoloop();

        int9handler();
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        if (mouseButtonStatus != 0)
        {
            break;
        }
        if (keyPressed == SDL_SCANCODE_UNKNOWN)
        {
            lastPressedKey = SDL_SCANCODE_UNKNOWN;
            continue;
        }

        if (lastPressedKey == keyPressed)
        {
            continue;
        }

        lastPressedKey = keyPressed;

        char character = characterForSDLScancode(lastPressedKey);

        if (character == 0) // For keys without a valid representation
        {
            continue;
        }
        if (character == '\n') // \n -> enter -> create player
        {
            break;
        }
        if (character == '\b') // backspace -> delete last char
        {
//loc_4ABCC:              ; CODE XREF: handleNewPlayerOptionClick+92j
            if (gNewPlayerNameLength == 0)
            {
                continue;
            }
            gNewPlayerNameLength--;
            newPlayerName[gNewPlayerNameLength] = ' ';
            drawTextWithChars6FontWithOpaqueBackground(232, 127, 6, newPlayerName);
            continue;
        }
        if (gNewPlayerNameLength >= 8) // when more than 8 chars were entered, ignore the rest?
        {
            continue;
        }
        newPlayerName[gNewPlayerNameLength] = character; // mov [bx+si], al
        gNewPlayerNameLength++;
        drawTextWithChars6FontWithOpaqueBackground(232, 127, 6, newPlayerName);
    }
    while (1);

    do
    {
//loc_4ABEB:              ; CODE XREF: handleNewPlayerOptionClick+72j
//                ; handleNewPlayerOptionClick+8Ej ...
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);

    // Completely empty name: ignore
    if (strcmp(newPlayerName, "        ") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "                       ");
        saveLastMouseAreaBitmap();
        drawMouseCursor();
        return;
    }

//loc_4AC1E:              ; CODE XREF: handleNewPlayerOptionClick+E0j
//                ; handleNewPlayerOptionClick+E5j ...
    // Name with all dashes: invalid
    if (strcmp(newPlayerName, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "INVALID NAME           ");
        saveLastMouseAreaBitmap();
        drawMouseCursor();
        return;
    }

//loc_4AC46:              ; CODE XREF: handleNewPlayerOptionClick+108j
//                ; handleNewPlayerOptionClick+10Dj ...

    // Move spaces at the end of the name to the beginning
    const int kLastNameCharacterIndex = sizeof(newPlayerName) - 2;
    while (newPlayerName[kLastNameCharacterIndex] == ' ')
    {
//loc_4AC4B:              ; CODE XREF: handleNewPlayerOptionClick+14Cj
        for (int i = kLastNameCharacterIndex; i >= 1; --i)
        {
            newPlayerName[i] = newPlayerName[i - 1];
        }
        newPlayerName[0] = ' ';
    }

//loc_4AC69:              ; CODE XREF: handleNewPlayerOptionClick+137j

    for (int i = 0; i < kNumberOfPlayers; ++i)
    {
        PlayerEntry player = gPlayerListData[i];
//loc_4AC73:              ; CODE XREF: handleNewPlayerOptionClick+18Cj
        if (strcmp(player.name, newPlayerName) == 0)
        {
            drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "PLAYER EXISTS          ");
            saveLastMouseAreaBitmap();
            drawMouseCursor();
            return;
        }
//loc_4ACA3:              ; CODE XREF: handleNewPlayerOptionClick+15Cj
//                ; handleNewPlayerOptionClick+164j ...
    }
    gCurrentPlayerIndex = gNewPlayerEntryIndex;
    PlayerEntry *newPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
    memcpy(newPlayerEntry->name, newPlayerName, sizeof(newPlayerName));

    drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "                       ");
    savePlayerListData();
    saveHallOfFameData();
    byte_51ABE = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    drawLevelList();
    drawRankings();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void handleDeletePlayerOptionClick() // sub_4AD0E  proc near
{
    if (byte_59B85 != 0)
    {
        //loc_4AD3C:              ; CODE XREF: handleDeletePlayerOptionClick+5j
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
//    *dword_58477 = currentPlayerEntry; // mov word ptr dword_58477, si
    if (strcmp(currentPlayerEntry->name, "--------") == 0)
    {
//loc_4AD3C:              ; CODE XREF: handleDeletePlayerOptionClick+5j
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4AD48:              ; CODE XREF: handleDeletePlayerOptionClick+1Dj
//                ; handleDeletePlayerOptionClick+22j ...
    char message[24] = "";
    sprintf(message, "DELETE '%s' ???  ", currentPlayerEntry->name);

    drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, message);

    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    do
    {
//loc_4AD6C:              ; CODE XREF: handleDeletePlayerOptionClick+64j
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);

    do
    {
//loc_4AD74:              ; CODE XREF: handleDeletePlayerOptionClick+88j
        videoloop();
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        gMouseButtonStatus = mouseButtonStatus;
        gMouseX = mouseX;
        gMouseY = mouseY;
        restoreLastMouseAreaBitmap();
        saveLastMouseAreaBitmap();
        drawMouseCursor();
    }
    while (gMouseButtonStatus == 0);

    ButtonDescriptor okButtonDescriptor = kMainMenuButtonDescriptors[9];

    if (gMouseX >= okButtonDescriptor.startX
        && gMouseY >= okButtonDescriptor.startY
        && gMouseX <= okButtonDescriptor.endX
        && gMouseY <= okButtonDescriptor.endY)
    {
//        mov di, word ptr dword_58477 // recover current player entry pointer
        memset(currentPlayerEntry, 0, sizeof(PlayerEntry));
        memset(currentPlayerEntry->name, '-', sizeof(currentPlayerEntry->name) - 1);
    }

//loc_4ADCE:              ; CODE XREF: handleDeletePlayerOptionClick+97j
//                ; handleDeletePlayerOptionClick+9Cj ...
    restoreLastMouseAreaBitmap();
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "                       ");
    savePlayerListData();
    saveHallOfFameData();
    byte_51ABE = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    drawLevelList();
    drawRankings();

    do
    {
//loc_4ADF3:              ; CODE XREF: handleDeletePlayerOptionClick+EBj
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);
    saveLastMouseAreaBitmap();
}

void handleSkipLevelOptionClick() // sub_4ADFF  proc near
{
    // 01ED:419C
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];

    if (strcmp(currentPlayerEntry.name, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4AE2E:              ; CODE XREF: handleSkipLevelOptionClick+12j
//                ; handleSkipLevelOptionClick+17j ...
    int numberOfSkippedLevels = 0;

    for (int i = 0; i < kNumberOfLevels; ++i)
    {
//loc_4AE38:              ; CODE XREF: handleSkipLevelOptionClick+40j
        if (currentPlayerEntry.levelState[i] == PlayerLevelStateSkipped)
        {
            numberOfSkippedLevels++;
        }
//loc_4AE3E:              ; CODE XREF: handleSkipLevelOptionClick+3Bj
    }
    if (word_51970 == 0)
    {
//loc_4AE4A:              ; CODE XREF: handleSkipLevelOptionClick+47j
        if (numberOfSkippedLevels >= 3)
        {
            drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "SKIP NOT POSSIBLE      ");
            return;
        }
    }

//loc_4AE5B:              ; CODE XREF: handleSkipLevelOptionClick+49j
//                ; handleSkipLevelOptionClick+4Ej
    if (gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1] != kNotCompletedLevelEntryColor)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, "COLORBLIND I GUESS     ");
        return;
    }

//loc_4AE75:              ; CODE XREF: handleSkipLevelOptionClick+68j
    char levelNumber[4] = "000";
    convertNumberTo3DigitStringWithPadding0(gCurrentSelectedLevelIndex, levelNumber);

    char message[24];
    sprintf(message, "SKIP LEVEL %s ???     ", levelNumber);
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, message);

    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    do
    {
//loc_4AE89:              ; CODE XREF: handleSkipLevelOptionClick+90j
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);

    do
    {
//loc_4AE91:              ; CODE XREF: handleSkipLevelOptionClick+B4j
        videoloop();
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        gMouseButtonStatus = mouseButtonStatus;
        gMouseX = mouseX;
        gMouseY = mouseY;
        restoreLastMouseAreaBitmap();
        saveLastMouseAreaBitmap();
        drawMouseCursor();
    }
    while (gMouseButtonStatus == 0);

    ButtonDescriptor okButtonDescriptor = kMainMenuButtonDescriptors[9];

    if (gMouseX >= okButtonDescriptor.startX
        && gMouseY >= okButtonDescriptor.startY
        && gMouseX <= okButtonDescriptor.endX
        && gMouseY <= okButtonDescriptor.endY)
    {
        byte_510BB = 2;
        changePlayerCurrentLevelState(); // 01ED:4275
        savePlayerListData();
        saveHallOfFameData();
        byte_51ABE = 0;
        prepareLevelDataForCurrentPlayer();
    }

//loc_4AEE9:              ; CODE XREF: handleSkipLevelOptionClick+C3j
//                ; handleSkipLevelOptionClick+C8j ...
    restoreLastMouseAreaBitmap();
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "                       ");
    drawPlayerList();
    drawLevelList();
    drawRankings();

    do
    {
//loc_4AF00:              ; CODE XREF: handleSkipLevelOptionClick+107j
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
    }
    while (mouseButtonStatus != 0);
    saveLastMouseAreaBitmap();
}

void handleStatisticsOptionClick() // sub_4AF0C   proc near
{
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];
    if (strcmp(currentPlayerEntry.name, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4AF3E:              ; CODE XREF: handleStatisticsOptionClick+15j
//                ; handleStatisticsOptionClick+1Aj ...
    uint16_t someValue = 0x5F5F;
    word_586FB = someValue;
    word_586FD = someValue;
    word_586FF = someValue;
    word_58701 = someValue;
    word_58703 = someValue;
    word_5870D = someValue;
    byte_5870F = someValue & 0xFF;
    word_58710 = someValue;
    word_58712 = someValue;
    word_58714 = someValue;
    if (word_51970 != 0)
    {
    //    al = 0Ah
    //    sub al, gameSpeed
    //    aam
    //    or  ax, 3030h
    //    xchg    al, ah
        word_586FB = 0x3C20;
        word_586FD = 0x4C44;
        word_586FF = 0x3A59;
    //    word_58701 = 0xax
        word_58703 = 0x203E;
        word_5870D = 0x3C20;
        word_58712 = 0x5A48;
        word_58714 = 0x203E;
        byte_59B94 = 0x0;
        byte_59B96 = 0x0;

        do
        {
    //loc_4AFAA:              ; CODE XREF: handleStatisticsOptionClick+A3j
            // TODO: update timer
        }
        while ((byte_59B96 & 0xFF) == 0); // test    byte_59B96, 0FFh
        byte_59B96 = 0;

        do
        {
    //loc_4AFB6:              ; CODE XREF: handleStatisticsOptionClick+B9j
            videoloop();
            loopForVSync();
            byte_59B94++;
        }
        while (byte_59B96 < 0x32); // '2'

        al = byte_59B94;
        ah = 0;
    //    push(cx);
        cl = 100;
        al = ax / cl;
        ah = ax % cl;
    //    pop(cx);
        al |= 0x30;
        byte_5870F = al;
        al = ah;
    //    aam
        ax |= 0x3030;
    //    xchg    al, ah
        word_58710 = ax;
    }

//loc_4AFE3:              ; CODE XREF: handleStatisticsOptionClick+58j
    fadeToPalette(gBlackPalette);

    uint8_t screenPixelsBackup[kFullScreenFramebufferLength];
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();

    byte_5091A = 0;
    drawTextWithChars6FontWithTransparentBackground(80, 20, 15, "SUPAPLEX  BY DREAM FACTORY");
    drawTextWithChars6FontWithTransparentBackground(64, 50, 15, "(C) DIGITAL INTEGRATION LTD 1991");
    drawTextWithChars6FontWithTransparentBackground(16, 60, 15, "________________________________________________");
    drawTextWithChars6FontWithTransparentBackground(80, 80, 15, "SUPAPLEX PLAYER STATISTICS");

    char currentPlayerText[27] = "";
    sprintf(currentPlayerText, "CURRENT PLAYER :  %s", currentPlayerEntry.name);
    drawTextWithChars6FontWithTransparentBackground(80, 100, 15, currentPlayerText);

    if (currentPlayerEntry.nextLevelToPlay == kLastLevelIndex)
    {
        byte_5091A = 1;
    }

//loc_4B046:              ; CODE XREF: handleStatisticsOptionClick+133j
    char levelNumberString[4] = "000";
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.nextLevelToPlay, levelNumberString);

    char currentLevelText[27] = "";
    sprintf(currentLevelText, "CURRENT LEVEL  :       %s", levelNumberString);
    drawTextWithChars6FontWithTransparentBackground(80, 110, 15, currentLevelText);

    char secondsNumberString[4] = ":00";
    char minutesNumberString[4] = ":00";
    char hoursNumberString[4] = "  0";

    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.seconds, secondsNumberString);
    secondsNumberString[0] = ':';

    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.minutes, minutesNumberString);
    minutesNumberString[0] = ':';

    convertNumberTo3DigitPaddedString(currentPlayerEntry.hours, hoursNumberString, 1);

    char usedTimeText[27] = "";
    sprintf(usedTimeText, "USED TIME      : %s%s%s", hoursNumberString, minutesNumberString, secondsNumberString);

    drawTextWithChars6FontWithTransparentBackground(80, 120, 15, usedTimeText);

    uint32_t totalMinutes = currentPlayerEntry.hours * 60 + currentPlayerEntry.minutes;

    if (currentPlayerEntry.seconds >= 30)
    {
        totalMinutes++;
    }

//loc_4B0A1:              ; CODE XREF: handleStatisticsOptionClick+192j

    char averageTimeString[6] = "000.0";
    uint16_t averageMinutesWhole = totalMinutes / currentPlayerEntry.nextLevelToPlay;
    uint16_t averageMinutesFraction = (totalMinutes % currentPlayerEntry.nextLevelToPlay);
    averageMinutesFraction = averageMinutesFraction / currentPlayerEntry.nextLevelToPlay;
    convertNumberTo3DigitStringWithPadding0(averageMinutesFraction, &averageTimeString[2]);

    if (averageMinutesWhole == 0)
    {
        byte_5091A = 2;
    }

//loc_4B0C2:              ; CODE XREF: handleStatisticsOptionClick+1AFj
    averageTimeString[3] = '.';

    convertNumberTo3DigitPaddedString(averageMinutesWhole, averageTimeString, 1);
    if (byte_5091A == 1)
    {
        drawTextWithChars6FontWithTransparentBackground(24, 140, 15, "YOU'VE COMPLETED ALL LEVELS! CONGRATULATIONS!!!");
    }
//loc_4B0E2:              ; CODE XREF: handleStatisticsOptionClick+1C7j
    else if (byte_5091A == 2)
    {
        drawTextWithChars6FontWithTransparentBackground(40, 140, 15, "STILL UNDER ONE MINUTE (KEEP IT UP...)");
    }
//loc_4B0F6:              ; CODE XREF: handleStatisticsOptionClick+1DBj
    else
    {
        char averageTimeMessage[44] = "";
        sprintf(averageTimeMessage, "AVERAGE TIME USED PER LEVEL  %s MINUTES", averageTimeString);
        drawTextWithChars6FontWithTransparentBackground(32, 140, 15, averageTimeMessage);
    }
//loc_4B105:              ; CODE XREF: handleStatisticsOptionClick+1D4j
//                ; handleStatisticsOptionClick+1E8j
    fadeToPalette(gPalettes[0]);
    waitForKeyMouseOrJoystick();
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);
    fadeToPalette(gPalettes[1]);
}

void handleGfxTutorOptionClick() // sub_4B149   proc near
{
    drawGfxTutorBackground(gScrollDestinationScreenBitmapData);
    scrollRightToNewScreen();
    waitForKeyMouseOrJoystick();
    scrollLeftToMainMenu();
    drawMenuTitleAndDemoLevelResult();
}

void sub_4B159() //   proc near       ; CODE XREF: runMainMenu+6Fp
{
//    exitWithError("function sub_4B159 not implemented yet\n");
    /*
        call    readDemoFiles
        or  cx, cx
        jnz short loc_4B163
        jmp locret_4B1F1
// ; ---------------------------------------------------------------------------

loc_4B163:              ; CODE XREF: sub_4B159+5j
        mov word_5196C, 1
        mov byte_510DE, 1
        push    es
        mov ax, seg demoseg
        mov es, ax
        assume es:demoseg
        mov bx, 0
        mov cx, 0

loc_4B17A:              ; CODE XREF: sub_4B159+2Dj
        mov ax, es:[bx]
        add bx, 2
        cmp ax, 0FFFFh
        jz  short loc_4B188
        inc cx
        jmp short loc_4B17A
// ; ---------------------------------------------------------------------------

loc_4B188:              ; CODE XREF: sub_4B159+2Aj
        push(cx);
        call    getTime
        call    sub_4A1AE
        xor dx, dx
        pop(cx);
        div cx
        mov bx, 0
        shl dx, 1
        add bx, dx
        mov bx, es:[bx]
        cmp bx, 0FFFFh
        jnz short loc_4B1AE
        mov word_5196C, 0
        mov byte_510DE, 0

loc_4B1AE:              ; CODE XREF: sub_4B159+48j
        al = es:[bx]
        xor ah, ah
        push    bx
        mov bx, dx
        shr dx, 1
        mov word_599D6, dx
        mov word_599D8, 0
        cmp al, 6Fh ; 'o'
        ja  short loc_4B1CF
        or  al, al
        jz  short loc_4B1CF
        mov byte ptr word_599D8, al
        mov dl, al

loc_4B1CF:              ; CODE XREF: sub_4B159+6Bj
                    ; sub_4B159+6Fj
        al = dl
        mov bx, [bx-67CAh]
        mov gTimeOfDay, bx
        pop bx
        mov word_510E6, ax
        inc bx
        pop es
        assume es:nothing
        mov word_510DF, bx
        mov word_5A33C, bx
        mov byte_510E1, 0
        mov byte_510E2, 1

locret_4B1F1:               ; CODE XREF: sub_4B159+7j
        return;
*/
}

/*
demoSomething  proc near       ; CODE XREF: start+3BAp
                    ; runMainMenu+12Ep ...
        push    ax
        call    readDemoFiles
        pop ax
        push    ax
        mov bx, ax
        shl bx, 1
        mov ax, [bx-67CAh]
        mov gTimeOfDay, ax
        pop ax
        mov word_5196C, 1
        mov byte_510DE, 1
        push    es
        mov dx, seg demoseg
        mov es, dx
        assume es:demoseg
        mov bx, 0
        shl ax, 1
        add bx, ax
        mov bx, es:[bx]
        cmp bx, 0FFFFh
        jnz short loc_4B22F
        mov word_5196C, 0
        mov byte_510DE, 0

loc_4B22F:              ; CODE XREF: demoSomething+30j
        mov word_599D8, 0
        shr al, 1
        mov ah, es:[bx]
        cmp ah, 6Fh ; 'o'
        ja  short loc_4B248
        or  ah, ah
        jz  short loc_4B248
        al = ah
        mov byte ptr word_599D8, al

loc_4B248:              ; CODE XREF: demoSomething+4Bj
                    ; demoSomething+4Fj
        xor ah, ah
        mov word_510E6, ax
        inc bx
        pop es
        assume es:nothing
        mov word_510DF, bx
        mov word_5A33C, bx
        mov byte_510E1, 0
        mov byte_510E2, 1
        return;
demoSomething  endp

 */

void handleRankingListScrollUp() // loc_4B262
{
    gRankingListButtonPressed = 1;
    gRankingListDownButtonPressed = 0;
    gRankingListUpButtonPressed = 1;
    ax = word_5195D;
    ax -= word_58471;
    if (ax < word_58473)
    {
        return;
    }

//loc_4B27F:              ; CODE XREF: code:465Cj
    restoreLastMouseAreaBitmap();
    word_58473 = word_5195D;
    if (word_58471 > 1)
    {
        word_58471--;
    }

//loc_4B293:              ; CODE XREF: code:466Dj
    if (byte_59B85 == 0
        && byte_58D46 > 0)
    {
        byte_58D46--;
    }

//loc_4B2A5:              ; CODE XREF: code:4678j code:467Fj
    drawRankings();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void handleRankingListScrollDown() // loc_4B2AF
{
    gRankingListButtonPressed = 1;
    gRankingListDownButtonPressed = 1;
    gRankingListUpButtonPressed = 0;
    ax = word_5195D;
    ax -= word_58471;
    if (ax < word_58473)
    {
        return;
    }

//loc_4B2CC:              ; CODE XREF: code:46A9j
    restoreLastMouseAreaBitmap();
    word_58473 = word_5195D;
    if (word_58471 > 1)
    {
        word_58471--;
    }

//loc_4B2E0:              ; CODE XREF: code:46BAj
    if (byte_59B85 == 0
        && byte_58D46 < kNumberOfPlayers - 1)
    {
        byte_58D46++;
    }

//loc_4B2F2:              ; CODE XREF: code:46C5j code:46CCj
    drawRankings();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void showCongratulationsScreen() // sub_4B2FC   proc near       ; CODE XREF: handleOkButtonClick+56p
{
    fadeToPalette(gBlackPalette);

    uint8_t screenPixelsBackup[kFullScreenFramebufferLength];
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();
//    mov si, 8718h // "CONGRATULATIONS"
//    mov ah, 0Fh
//    mov di, 5BDFh
    drawTextWithChars6FontWithTransparentBackground(120, 30, 15, "CONGRATULATIONS");
//    mov si, 8728h // "YOU HAVE COMPLETED ALL 111 LEVELS OF SUPAPLEX"
//    mov ah, 0Fh
//    mov di, 6EE3h
    drawTextWithChars6FontWithTransparentBackground(24, 70, 15, "YOU HAVE COMPLETED ALL 111 LEVELS OF SUPAPLEX");
//    mov si, 8756h // "YOUR BRAIN IS IN FANTASTIC SHAPE"
//    mov ah, 0Fh
//    mov di, 760Eh
    drawTextWithChars6FontWithTransparentBackground(64, 85, 15, "YOUR BRAIN IS IN FANTASTIC SHAPE");
//    mov si, 8777h // "NOT MANY PEOPLE ARE ABLE TO MANAGE THIS"
//    mov ah, 0Fh
//    mov di, 7D31h
    drawTextWithChars6FontWithTransparentBackground(40, 100, 15, "NOT MANY PEOPLE ARE ABLE TO MANAGE THIS");
//    mov bx, 4D84h
//    mov dx, 3D4h
//    al = 0Dh
//    out dx, al      ; Video: CRT cntrlr addr
//                ; regen start address (low)
//    inc dx
//    al = bl
//    out dx, al      ; Video: CRT controller internal registers
//    mov dx, 3D4h
//    al = 0Ch
//    out dx, al      ; Video: CRT cntrlr addr
//                ; regen start address (high)
//    inc dx
//    al = bh
//    out dx, al      ; Video: CRT controller internal registers
//    mov si, palettesDataBuffer
    fadeToPalette(gPalettes[0]);
    waitForKeyMouseOrJoystick();
//    mov si, 60D5h
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);
//    mov bx, 4D5Ch
//    mov dx, 3D4h
//    al = 0Dh
//    out dx, al      ; Video: CRT cntrlr addr
//                ; regen start address (low)
//    inc dx
//    al = bl
//    out dx, al      ; Video: CRT controller internal registers
//    mov dx, 3D4h
//    al = 0Ch
//    out dx, al      ; Video: CRT cntrlr addr
//                ; regen start address (high)
//    inc dx
//    al = bh
//    out dx, al      ; Video: CRT controller internal registers

    fadeToPalette(gPalettes[1]); // 6015h
    return;
}

void handleOkButtonClick() // sub_4B375  proc near       ; CODE XREF: runMainMenu+11Ep
{
    // 01ED:4712
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];

    if (strcmp(currentPlayerEntry.name, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4B3A4:              ; CODE XREF: handleOkButtonClick+12j
//                ; handleOkButtonClick+17j ...
//    ax = gCurrentSelectedLevelIndex;
    if (gCurrentSelectedLevelIndex == kLastLevelIndex)
    {
//loc_4B3B4:              ; CODE XREF: handleOkButtonClick+3Cj
        uint8_t numberOfCompletedLevels = 0;

        for (int i = 0; i < kNumberOfLevels; ++i)
        {
//loc_4B3BC:              ; CODE XREF: handleOkButtonClick+4Fj
            if (currentPlayerEntry.levelState[i] == PlayerLevelStateCompleted)
            {
                numberOfCompletedLevels++;
            }
//loc_4B3C3:              ; CODE XREF: handleOkButtonClick+4Aj
        }
        if (numberOfCompletedLevels == kNumberOfLevels)
        {
            showCongratulationsScreen();
            return;
        }
        else
        {
//loc_4B3CF:              ; CODE XREF: handleOkButtonClick+54j
            drawTextWithChars6FontWithOpaqueBackground(168, 127, 2, "COLORBLIND I GUESS     ");
            return;
        }
    }
    else if (gCurrentSelectedLevelIndex > kNumberOfLevels)
    {
        return;
    }

//loc_4B3DB:              ; CODE XREF: handleOkButtonClick+35j
    uint8_t currentLevelColor = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1];

    if (currentLevelColor == kBlockedLevelEntryColor)
    {
//loc_4B404:              ; CODE XREF: handleOkButtonClick+70j
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "COLORBLIND I GUESS     ");
        return;
    }
    word_5196C = 1;
    byte_510DE = 0;

    if (currentLevelColor == kCompletedLevelEntryColor)
    {
        byte_510B3 = 0;
    }
    else
    {
//loc_4B3FD:              ; CODE XREF: handleOkButtonClick+7Fj
        byte_510B3 = 1;
    }

//loc_4B40F:              ; CODE XREF: handleOkButtonClick+86j
//                ; handleOkButtonClick+8Dj
    prepareSomeKindOfLevelIdentifier(); // 01ED:47AC
    sub_4BF4A(gCurrentSelectedLevelIndex); // 01ED:47B2
}

void handleFloppyDiskButtonClick() // sub_4B419  proc near
{
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 6, "WHAT'S A FLOPPY DISK?  ");
}

void handlePlayerListScrollDown() // sub_4B671  proc near
{
    gPlayerListButtonPressed = 1;
    gPlayerListDownButtonPressed = 1;
    gPlayerListUpButtonPressed = 0;
    ax = word_5195D;
    ax -= word_5846D;
    if (ax < word_5846F)
    {
        return;
    }

//loc_4B68E:              ; CODE XREF: handlePlayerListScrollDown+1Aj
    word_5846F = word_5195D;
    if (word_5846D > 1)
    {
        word_5846D--;
    }

//loc_4B69F:              ; CODE XREF: handlePlayerListScrollDown+28j
    if (byte_59B85 == 0
        && gCurrentPlayerIndex < kNumberOfPlayers - 1)
    {
        gCurrentPlayerIndex++;
    }

//loc_4B6B1:              ; CODE XREF: handlePlayerListScrollDown+33j
//                ; handlePlayerListScrollDown+3Aj
    restoreLastMouseAreaBitmap();
    byte_51ABE = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    drawLevelList();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void handlePlayerListScrollUp() // sub_4B6C9  proc near
{
    gPlayerListButtonPressed = 1;
    gPlayerListDownButtonPressed = 0;
    gPlayerListUpButtonPressed = 1;
    ax = word_5195D;
    ax -= word_5846D;
    if (ax < word_5846F)
    {
        return;
    }

//loc_4B6E6:              ; CODE XREF: handlePlayerListScrollUp+1Aj
    word_5846F = word_5195D;
    if (word_5846D > 1)
    {
        word_5846D--;
    }

//loc_4B6F7:              ; CODE XREF: handlePlayerListScrollUp+28j
    if (byte_59B85 == 0
        && gCurrentPlayerIndex > 0)
    {
        gCurrentPlayerIndex--;
    }

//loc_4B709:              ; CODE XREF: handlePlayerListScrollUp+33j
//                ; handlePlayerListScrollUp+3Aj
    restoreLastMouseAreaBitmap(); // Clears mouse trail
    byte_51ABE = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    drawLevelList();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void handlePlayerListClick() // sub_4B721  proc near
{
    byte_58D46 = byte_58D47;
    drawRankings();
}

void handleLevelListScrollDown() // sub_4B72B  proc near
{
    gLevelListButtonPressed = 1;
    gLevelListDownButtonPressed = 1;
    gLevelListUpButtonPressed = 0;
    ax = word_5195D;
    ax -= word_58469;
    if (ax < word_5846B)
    {
        return;
    }

//loc_4B748:              ; CODE XREF: handleLevelListScrollDown+1Aj
    word_5846B = word_5195D;
    if (word_58469 > 1)
    {
        word_58469--;
    }

//loc_4B759:              ; CODE XREF: handleLevelListScrollDown+28j
    if (gCurrentSelectedLevelIndex >= 113)
    {
        return;
    }
    gCurrentSelectedLevelIndex++;
    restoreLastMouseAreaBitmap();
    drawLevelList();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void handleLevelListScrollUp() // sub_4B771  proc near
{
    gLevelListButtonPressed = 1;
    gLevelListDownButtonPressed = 0;
    gLevelListUpButtonPressed = 1;
    ax = word_5195D;
    ax -= word_58469;
    if (ax < word_5846B)
    {
        return;
    }

//loc_4B78E:              ; CODE XREF: handleLevelListScrollUp+1Aj
    word_5846B = word_5195D;
    if (word_58469 > 1)
    {
        word_58469--;
    }

//loc_4B79F:              ; CODE XREF: handleLevelListScrollUp+28j
    if (gCurrentSelectedLevelIndex <= 1)
    {
        return;
    }
    gCurrentSelectedLevelIndex--;
    restoreLastMouseAreaBitmap();
    drawLevelList();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
//locret_4B7B6:               ; CODE XREF: handleLevelListScrollUp+33j
}

void handleLevelCreditsClick() // sub_4B7B7  proc near
{
    fadeToPalette(gBlackPalette);

    uint8_t screenPixelsBackup[kFullScreenFramebufferLength];
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();

    drawTextWithChars6FontWithTransparentBackground(80, 10, 15, "SUPAPLEX  BY DREAM FACTORY");
    drawTextWithChars6FontWithTransparentBackground(56, 40, 15, "ORIGINAL DESIGN BY PHILIP JESPERSEN");
    drawTextWithChars6FontWithTransparentBackground(88, 50, 15, "AND MICHAEL STOPP");
    drawTextWithChars6FontWithTransparentBackground(56, 90, 15, "NEARLY ALL LEVELS BY MICHEAL STOPP");
    drawTextWithChars6FontWithTransparentBackground(64, 100, 15, "A FEW LEVELS BY PHILIP JESPERSEN");
    drawTextWithChars6FontWithTransparentBackground(56, 110, 15, "HARDLY ANY LEVELS BY BARBARA STOPP");
    drawTextWithChars6FontWithTransparentBackground(64, 170, 15, "NOTE: PRESS ENTER TO REMOVE PANEL");
    drawTextWithChars6FontWithTransparentBackground(64, 190, 15, "(C) DIGITAL INTEGRATION LTD 1991");
    fadeToPalette(gPalettes[0]);
    waitForKeyMouseOrJoystick();
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);
    fadeToPalette(gPalettes[1]);
}

// This function calculates where in the main menu bitmap the cursor will be drawn, and takes in advance
// the area of the screen so that it can be restored later.
//
void saveLastMouseAreaBitmap() // sub_4B85C  proc near       ; CODE XREF: handleNewPlayerOptionClick+FCp
//                    ; handleNewPlayerOptionClick+124p ...
{
    // 01ED:4BF9
    gLastMouseCursorOriginAddress = gMouseY * kScreenWidth + gMouseX;

    for (int y = 0; y < kLastMouseCursorAreaSize; ++y)
    {
        for (int x = 0; x < kLastMouseCursorAreaSize; ++x)
        {
            uint32_t destAddress = y * kLastMouseCursorAreaSize + x;
            uint32_t sourceAddress = (gLastMouseCursorOriginAddress
                                      + y * kScreenWidth
                                      + x);

            gLastMouseCursorAreaBitmap[destAddress] = gScreenPixels[sourceAddress];
        }
    }
}

// This function will restore the last portion of the background where the mouse was
// hence clearing its trail.
//
void restoreLastMouseAreaBitmap() // sub_4B899  proc near       ; CODE XREF: start+417p handleNewPlayerOptionClick+3Fp ...
{
    for (int y = 0; y < kLastMouseCursorAreaSize; ++y)
    {
        for (int x = 0; x < kLastMouseCursorAreaSize; ++x)
        {
            uint32_t destAddress = y * kLastMouseCursorAreaSize + x;
            uint32_t sourceAddress = (gLastMouseCursorOriginAddress
                                      + y * kScreenWidth
                                      + x);

            gScreenPixels[sourceAddress] = gLastMouseCursorAreaBitmap[destAddress];
        }
    }
}

void drawMouseCursor() // sub_4B8BE  proc near       ; CODE XREF: handleNewPlayerOptionClick+FFp
//                    ; handleNewPlayerOptionClick+127p ...
{
    // word_5195D = some kind of counter for animations?
    uint8_t frameNumber = (word_5195D / 4) % 8;
    uint8_t frameRow = frameNumber / 4;
    uint8_t frameColumn = frameNumber % 4;

    static const uint8_t kMouseCursorSize = 8;
    static const uint32_t kMouseCursorOriginY = 445;

    // In the original code, the address below was just 0x1520 (+ the offset for the current frame)
    const uint32_t kStartMouseCursorAddr = ((kMouseCursorOriginY * kMovingBitmapWidth) // First pixel of the cursor frames in MOVING.DAT
                                            + (frameRow * (kMouseCursorSize + 1) * kMovingBitmapWidth) // Y offset depending on the row in which the frame we're gonna draw is. The +1 to the size is because there is 1 pixel separation between rows :shrug:
                                            + (frameColumn * kMouseCursorSize)); // X offset depending on the column where the frame is

    for (int y = 0; y < kMouseCursorSize; ++y)
    {
        for (int x = 0; x < kMouseCursorSize; ++x)
        {
            uint8_t sourcePixel = gMovingDecodedBitmapData[kStartMouseCursorAddr + y * kMovingBitmapWidth + x];

            // Only draw non-0 pixels
            if (sourcePixel > 0)
            {
                gScreenPixels[gLastMouseCursorOriginAddress + y * kScreenWidth + x] = sourcePixel;
            }
        }
    }
}

void drawTextWithChars6FontWithOpaqueBackground(size_t destX, size_t destY, uint8_t color, const char *text) // sub_4BA5F  proc near       ; CODE XREF: handleNewPlayerOptionClick+37p
                  //  ; handleNewPlayerOptionClick+4Ap ...
{
    // Parameters:
    // - di is the destination surface
    // - si is the text to be rendered
    // - ah is the color index in the current palette

    // Address: 01ED:4DFC
    if (byte_5A33F == 1)
    {
        return;
    }

//loc_4BA69:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackground+5j
    byte_51969 = color;

//loc_4BA8D:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackground:loc_4BDECj
    if (text[0] == '\0')
    {
        return;
    }

//loc_4BA9F:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackground+3Bj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

        //loc_4BA97:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackground+33j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacterWidth; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacterWidth + destX + x);
                gScreenPixels[destAddress] = color * pixelValue;
            }
        }
    }
}

void drawTextWithChars6FontWithTransparentBackground(size_t destX, size_t destY, uint8_t color, const char *text)  // sub_4BDF0 proc near       ; CODE XREF: recoverFilesFromFloppyDisk+2Ap
                   // ; handleStatisticsOptionClick+EDp ...
{
    if (byte_5A33F == 1)
    {
        return;
    }

//loc_4BDFA:             // ; CODE XREF: drawTextWithChars6FontWithTransparentBackground+5j
    byte_51969 = color;
    cl = 0;

//loc_4BE1E:             // ; CODE XREF: drawTextWithChars6FontWithTransparentBackground:loc_4BF46j
    if (text[0] == '\0')
    {
        return;
    }

//loc_4BE30:             // ; CODE XREF: drawTextWithChars6FontWithTransparentBackground+3Bj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

        //loc_4BA97:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackground+33j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacterWidth; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                if (pixelValue == 1)
                {
                    // 6 is the wide (in pixels) of this font
                    size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacterWidth + destX + x);
                    gScreenPixels[destAddress] = color;
                }
            }
        }
    }

}

void sub_4BF4A(uint8_t number) //   proc near       ; CODE XREF: start+3F7p sub_4955B+398p ...
{
    convertNumberTo3DigitStringWithPadding0(number, &a00s0010_sp[3]);
}

void convertNumberTo3DigitStringWithPadding0(uint8_t number, char numberString[3]) //  proc near       ; CODE XREF: handleSkipLevelOptionClick+7Cp
                   // ; handleStatisticsOptionClick+13Dp ...
{
    convertNumberTo3DigitPaddedString(number, numberString, 0);
}

void convertNumberTo3DigitPaddedString(uint8_t number, char numberString[3], char useSpacesForPadding) // sub_4BF4F  proc near       ; CODE XREF: handleStatisticsOptionClick+16Fp
                   // ; handleStatisticsOptionClick+1BFp ...
{
    // This function converts a number to a 3-digit string, so basically 123 to "123".
    // It also adds padding to the left, so 7 is converted to "007", with the option
    // to turn 0's in padding into spaces: 7 to "  7"
    // Parameters in the original implementation:
    // - al: number to convert
    // - ah: can be ' ' (space) or anything else. When it's ' ', the padding will be done with spaces, otherwise 0's will be used.
    // - si: the string to write the result

    numberString[0] = (number / 100) + '0';
    numberString[1] = ((number % 100) / 10) + '0';
    numberString[2] = (number % 10) + '0';

    if (useSpacesForPadding)
    {
        if (numberString[0] == '0')
        {
            numberString[0] = ' ';

            if (numberString[1] == '0')
            {
                numberString[1] = ' ';
            }
        }
    }
}

void prepareRankingTextEntries() // sub_4BF8D  proc near       ; CODE XREF: drawRankingsp
{
    // 01ED:532A
    if (byte_59B85 != 0)
    {
        return;
    }

    typedef struct
    {
        uint8_t playerIndex;
        uint8_t nextLevelToPlay;
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
    } RankingEntry;

    RankingEntry rankingEntries[kNumberOfPlayers];

    for (int i = 0; i < 20; ++i)
    {
//loc_4BFA2:              ; CODE XREF: prepareRankingTextEntries+38j
        RankingEntry *rankingEntry = &rankingEntries[i];
        PlayerEntry player = gPlayerListData[i];

        rankingEntry->playerIndex = i;
        rankingEntry->nextLevelToPlay = player.nextLevelToPlay;
        rankingEntry->hours = player.hours;
        rankingEntry->minutes = player.minutes;
        rankingEntry->seconds = player.seconds;
    }

//loc_4BFC7:              ; CODE XREF: prepareRankingTextEntries+B4j
    uint8_t numberOfChanges = 0;

    do
    {
        numberOfChanges = 0;

        for (int i = 0; i < kNumberOfPlayers - 1; ++i)
        {
//loc_4BFD3:              ; CODE XREF: prepareRankingTextEntries+AFj
            RankingEntry *rankingEntry = &rankingEntries[i];
            RankingEntry *nextRankingEntry = &rankingEntries[i + 1];

            uint32_t totalSeconds = rankingEntry->hours * 3600 + rankingEntry->minutes * 60 + rankingEntry->seconds;
            uint32_t nextTotalSeconds = rankingEntry->hours * 3600 + rankingEntry->minutes * 60 + rankingEntry->seconds;

            if (nextRankingEntry->nextLevelToPlay > rankingEntry->nextLevelToPlay
                || (nextRankingEntry->nextLevelToPlay == rankingEntry->nextLevelToPlay
                    && nextTotalSeconds > totalSeconds)
                )
            {
//loc_4BFFD:              ; CODE XREF: prepareRankingTextEntries+4Ej
//                ; prepareRankingTextEntries+58j ...
                RankingEntry aux = *nextRankingEntry;
                *nextRankingEntry = *rankingEntry;
                *rankingEntry = aux;
                numberOfChanges++;
            }
        }
    }
    while (numberOfChanges > 0);

    for (int i = 0; i < 20; ++i)
    {
//loc_4C04B:              ; CODE XREF: prepareRankingTextEntries+CFj
        if (rankingEntries[i].playerIndex == gCurrentPlayerIndex)
        {
            byte_58D47 = i;
        }
    }

//loc_4C061:
//    di = 0x883C; <- entry 2 of gRankingTextEntries

    for (int i = 0; i < 20; ++i)
    {
        RankingEntry *rankingEntry = &rankingEntries[i];
        char *textEntry = gRankingTextEntries[i + 2]; // No idea why the first two are always empty
//loc_4C067:              ; CODE XREF: prepareRankingTextEntries+14Dj
        if (rankingEntry->nextLevelToPlay == 0x71) // 113
        {
            textEntry[0] =
            textEntry[1] =
            textEntry[2] = '9';
        }
        else
        {
//loc_4C078:              ; CODE XREF: prepareRankingTextEntries+DFj
            convertNumberTo3DigitPaddedString(rankingEntry->nextLevelToPlay, textEntry, 0);
        }

//loc_4C07F:              ; CODE XREF: prepareRankingTextEntries+E9j
        PlayerEntry playerEntry = gPlayerListData[rankingEntry->playerIndex];

//loc_4C091:              ; CODE XREF: prepareRankingTextEntries+10Bj
        memcpy(&textEntry[4], playerEntry.name, sizeof(playerEntry.name) - 1);

        convertNumberTo3DigitStringWithPadding0(rankingEntry->seconds, &textEntry[19]);
        textEntry[19] = ':';

        convertNumberTo3DigitStringWithPadding0(rankingEntry->minutes, &textEntry[16]);
        textEntry[16] = ':';

        convertNumberTo3DigitStringWithPadding0(rankingEntry->hours, &textEntry[13]);
    }
}

void drawRankings() //   proc near       ; CODE XREF: handleNewPlayerOptionClick+1E9p
//                    ; handleDeletePlayerOptionClick+E2p ...
{
    // 01ED:547A
    prepareRankingTextEntries();

    const uint8_t kDistanceBetweenLines = 9;

    for (int i = 0; i < 5; ++i)
    {
        const uint8_t y = 110 + kDistanceBetweenLines * (i - 2);
        const uint8_t color = (i == 2 ? 6 : 8);
        drawTextWithChars6FontWithOpaqueBackground(8, y, color, gRankingTextEntries[byte_58D46 + i]);
    }

    char numberString[4] = "001"; // 0x8359
    convertNumberTo3DigitStringWithPadding0(byte_58D46, numberString);
    drawTextWithChars6FontWithOpaqueBackground(144, 110, 6, &numberString[1]); // Remove the first (left most) digit
}

void drawLevelList() // sub_4C141  proc near       ; CODE XREF: start+41Ap sub_4955B+39Bp ...
{
    // 01ED:54DE
    byte_59821 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 2];
    byte_59822 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1];
    byte_59823 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex];

    char *previousLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 2) * kLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 155, byte_59821, previousLevelName);

    char *currentLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 1) * kLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 164, byte_59822, currentLevelName);

    memcpy(gCurrentLevelName, currentLevelName, kLevelNameLength);

    char *nextLevelName = (char *)&gLevelListData[gCurrentSelectedLevelIndex * kLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 173, byte_59823, nextLevelName);
}

void drawHallOfFame() //   proc near       ; CODE XREF: handleFloppyDiskButtonClick+15Ap
//                    ; drawMenuTitleAndDemoLevelResult+11p
{
    // 01ED:5546
    char text[19] = "                  ";

    for (int i = 0; i < kNumberOfHallOfFameEntries; ++i)
    {
//loc_4C1B7:              ; CODE XREF: drawHallOfFame+56j
        HallOfFameEntry entry = gHallOfFameData[i];

        convertNumberTo3DigitStringWithPadding0(entry.seconds, &text[15]);
        text[15] = ':';

        convertNumberTo3DigitStringWithPadding0(entry.minutes, &text[12]);
        text[12] = ':';

        convertNumberTo3DigitPaddedString(entry.hours, &text[9], 1);

        memcpy(text, entry.playerName, sizeof(entry.playerName) - 1);

        drawTextWithChars6FontWithOpaqueBackground(184, 28 + i * 9, 8, text);
    }
}

void drawCurrentPlayerRanking() //   proc near       ; CODE XREF: drawPlayerList+5Bp // sub_4C224
{
    // 01ED:55C1
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];
    drawTextWithChars6FontWithOpaqueBackground(168, 93, 8, currentPlayerEntry.name);

    char timeText[10] = "000:00:00";

    // Seconds
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.seconds, &timeText[6]);
    timeText[6] = ':';

    // Minutes
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.minutes, &timeText[3]);
    timeText[3] = ':';

    // Hours
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.hours, timeText);

    drawTextWithChars6FontWithOpaqueBackground(224, 93, 8, timeText);

    char nextLevelText[4] = "000";
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.nextLevelToPlay, nextLevelText);
    drawTextWithChars6FontWithOpaqueBackground(288, 93, 8, nextLevelText);
}

void drawPlayerList() // sub_4C293  proc near       ; CODE XREF: start+32Cp start+407p ...
{
    // 01ED:5630
    PlayerEntry currentPlayer = gPlayerListData[gCurrentPlayerIndex];
    memcpy(gPlayerName, currentPlayer.name, sizeof(currentPlayer.name) - 1);
    drawTextWithChars6FontWithOpaqueBackground(16, 164, 6, currentPlayer.name);

    char *prevPlayerName = "";

    if (gCurrentPlayerIndex <= 0)
    {
        prevPlayerName = "        "; // just 8 spaces :shrug:
    }
    else
    {
        prevPlayerName = gPlayerListData[gCurrentPlayerIndex - 1].name;
    }

//loc_4C2CD:              // ; CODE XREF: drawPlayerList+35j
    drawTextWithChars6FontWithOpaqueBackground(16, 155, 8, prevPlayerName);

    char *nextPlayerName = "";

    if (gCurrentPlayerIndex >= kNumberOfPlayers - 1) // 19
    {
        nextPlayerName = "        "; // just 8 spaces :shrug:
    }
    else
    {
        nextPlayerName = gPlayerListData[gCurrentPlayerIndex + 1].name;
    }

//loc_4C2E6:              // ; CODE XREF: drawPlayerList+4Ej
    drawTextWithChars6FontWithOpaqueBackground(16, 173, 8, nextPlayerName);
    drawCurrentPlayerRanking();
}

void drawMenuTitleAndDemoLevelResult() //   proc near       ; CODE XREF: handleGfxTutorOptionClick+Cp
                    // ; sub_4C407+1Fp ...
{
    // 01ED:568F
    drawTextWithChars6FontWithOpaqueBackground(180, 127, 4, "WELCOME TO SUPAPLEX");
    drawPlayerList();
    drawLevelList();
    drawHallOfFame();
    drawRankings();
    if (byte_59B83 == 0)
    {
        return;
    }
    byte_59B83 = 0;

    char *message = "";
    if (byte_5A19B == 0)
    {
        if (byte_5A19C == 0)
        {
            message = "LEVEL FAILED";
        }
        else
        {
            message = "DEMO FAILED";
        }
    }
    else
    {
        if (byte_5A19C == 0)
        {
            message = "LEVEL SUCCESSFUL";
        }
        else
        {
            message = "DEMO SUCCESSFUL";
        }
    }

//loc_4C33C:              // ; CODE XREF: drawMenuTitleAndDemoLevelResult+34j
                // ; drawMenuTitleAndDemoLevelResult+39j ...
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, message);
    byte_5A19B = 0;
}

void prepareLevelDataForCurrentPlayer() //   proc near       ; CODE XREF: start+404p handleNewPlayerOptionClick+1E0p ...
{
    // 01ED:56E7
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];

    uint8_t *currentPlayerLevelState = currentPlayerEntry.levelState;

    // Sets everything to 6 which seems to mean all levels are blocked
    memset(gCurrentPlayerPaddedLevelData, kSkippedLevelEntryColor, kNumberOfLevelsWithPadding);
    memset(gCurrentPlayerLevelData, kBlockedLevelEntryColor, kCurrentPlayerLevelDataLength);

    char isFirstUncompletedLevel = 1;

    for (int i = 0; i < kNumberOfLevels; ++i)
    {
//loc_4C373:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+53j
        if (currentPlayerLevelState[i] == PlayerLevelStateSkipped)
        {
            gCurrentPlayerLevelData[i] = kSkippedLevelEntryColor;
        }
//loc_4C37F:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+2Ej
        else if (currentPlayerLevelState[i] == PlayerLevelStateCompleted) // Completed levels
        {
            gCurrentPlayerLevelData[i] = kCompletedLevelEntryColor;
        }
//loc_4C389:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+38j
        else if (currentPlayerLevelState[i] == PlayerLevelStateNotCompleted) // Levels not completed
        {
            if (isFirstUncompletedLevel == 1)
            {
                // The first uncompleted is not blocked
                gCurrentPlayerLevelData[i] = kNotCompletedLevelEntryColor;
            }
            else
            {
                // The rest uncompleted levels are blocked
                gCurrentPlayerLevelData[i] = kBlockedLevelEntryColor;
            }
            isFirstUncompletedLevel = 0;
        }
    }

    char hasCompletedAllLevels = 1;
    uint8_t nextLevelToPlay = 1;

    // Looks for the first uncompleted level
    for (int i = 0; i < kNumberOfLevels; ++i)
    {
//loc_4C3A7:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+65j
        if (currentPlayerLevelState[i] == PlayerLevelStateNotCompleted) // not completed
        {
            hasCompletedAllLevels = 0;
            break;
        }

        nextLevelToPlay++;
    }

    if (hasCompletedAllLevels == 1)
    {
        nextLevelToPlay = 1;

        // Looks for the first completed level
        for (int i = 0; i < kNumberOfLevels; ++i)
        {
//loc_4C3BA:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+78j
            if (currentPlayerLevelState[i] == PlayerLevelStateSkipped)
            {
                hasCompletedAllLevels = 0;
                break;
            }
            nextLevelToPlay++;
        }
    }

    if (hasCompletedAllLevels == 1)
    {
        if (byte_51ABE != 0)
        {
            gCurrentSelectedLevelIndex = kLastLevelIndex;
        }

//loc_4C3D1:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+7Fj
        currentPlayerEntry.nextLevelToPlay = kLastLevelIndex;
        return;
    }

//loc_4C3D6:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+61j
                // ; prepareLevelDataForCurrentPlayer+74j
    if (byte_51ABE != 0)
    {
        gCurrentSelectedLevelIndex = nextLevelToPlay;
    }

//loc_4C3E1:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+91j
    if (nextLevelToPlay == 1)
    {
        if (currentPlayerEntry.name[0] == 0x2D && currentPlayerEntry.name[1] == 0x2D)
        {
            if (currentPlayerEntry.name[2] == 0x2D && currentPlayerEntry.name[3] == 0x2D)
            {
                if (currentPlayerEntry.name[4] == 0x2D && currentPlayerEntry.name[5] == 0x2D)
                {
                    if (currentPlayerEntry.name[6] == 0x2D && currentPlayerEntry.name[7] == 0x2D)
                    {
                        nextLevelToPlay = 0;
                    }
                }
            }
        }
    }

//loc_4C403:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+9Aj
                // ; prepareLevelDataForCurrentPlayer+A0j ...
    currentPlayerEntry.nextLevelToPlay = nextLevelToPlay; // 0x7e = 126
}

void sub_4C407() //   proc near       ; CODE XREF: runMainMenu+5Dp
{
    gNumberOfDotsToShiftDataLeft = 0;
    if (byte_510BA != 0)
    {
        byte_510BA = 0;
//        sub_4C4F9();
        drawMenuBackground();
        byte_51ABE = 0;
        prepareLevelDataForCurrentPlayer();
        drawMenuTitleAndDemoLevelResult();
    //    mov si, 6015h
        fadeToPalette(gPalettes[1]);
//        jmp short loc_4C499
    }
    else
    {
//loc_4C431:              ; CODE XREF: sub_4C407+Aj
        /*
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
         */
        if (word_5195F >= 0x140) // 320
        {
//            mov di, 4D34h
//            sub_4C4BD();
        }
    }

//loc_4C449:              ; CODE XREF: sub_4C407+3Aj
//    mov di, 4D84h
//    sub_4C4BD();

    scrollLeftToMainMenu();
}

void scrollLeftToMainMenu() //loc_4C44F:              ; CODE XREF: handleGfxTutorOptionClick+9p
{
    uint8_t currentScreenPixels[kFullScreenFramebufferLength];
    memcpy(currentScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    drawMenuBackground();
    byte_51ABE = 0;

    prepareLevelDataForCurrentPlayer();
    drawMenuTitleAndDemoLevelResult();

    // These two lines were in the original code, but will be
    // left out here to prevent flickering.
//    videoloop();
//    loopForVSync();

    uint8_t menuScreenPixels[kFullScreenFramebufferLength];
    memcpy(menuScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 40;
    const int kStepSize = kScreenWidth / kNumberOfSteps;

    // Draws the current scroll animation step
    for (int i = kNumberOfSteps; i >= 0; --i)
    {
        for (int y = 0; y < kScreenHeight; ++y)
        {
            int limitFromLeft = kStepSize * (kNumberOfSteps - i);
            int limitFromRight = kScreenWidth - limitFromLeft;

            // Main menu side
            for (int x = 0; x < kScreenWidth - limitFromRight; ++x)
            {
                gScreenPixels[y * kScreenWidth + x] = menuScreenPixels[y * kScreenWidth + x + limitFromRight];
            }

            // GFX background side
            for (int x = limitFromLeft; x < kScreenWidth; ++x)
            {
                gScreenPixels[y * kScreenWidth + x] = currentScreenPixels[y * kScreenWidth + x - limitFromLeft];
            }
        }

//loc_4C466:              ; CODE XREF: sub_4C407+90j
//        gNumberOfDotsToShiftDataLeft = 7;
        videoloop();
        loopForVSync();
//        gNumberOfDotsToShiftDataLeft = 3;
        videoloop();
        loopForVSync();
    }

//loc_4C499:              ; CODE XREF: sub_4C407+28j
//    gNumberOfDotsToShiftDataLeft = 0;
    videoloop();

    // This will prevent to leave traces of the options menu
    // area in the main menu.
    //
    saveLastMouseAreaBitmap();
}

//sub_4C407   endp

/*

sub_4C4BD   proc near       ; CODE XREF: sub_4C407+3Fp
                    ; sub_4C407+45p
        mov si, word_51967
        mov word_51967, di
        mov bx, di
        mov dx, 0C8h ; '?'
        push    ds
        mov ax, es
        mov ds, ax

loc_4C4CF:              ; CODE XREF: sub_4C4BD+1Ej
        mov cx, 28h ; '('
        rep movsb
        add si, 52h ; 'R'
        add di, 52h ; 'R'
        dec dx
        jnz short loc_4C4CF
        pop ds
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    loopForVSync
        return;
sub_4C4BD   endp

sub_4C4F9   proc near       ; CODE XREF: sub_4C407+11p
        mov si, 60D5h
        call    setPalette
        call    drawBackBackground
        mov si, 8577h
        mov di, 6A2Ch
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        cmp byte_5195A, 0
        jnz short loc_4C52C
        mov si, 8582h
        mov di, 73A9h
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        mov si, 85ACh
        mov di, 7D35h
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        jmp short loc_4C55C
// ; ---------------------------------------------------------------------------

loc_4C52C:              ; CODE XREF: sub_4C4F9+19j
        mov si, 85DCh
        al = byte_5195B
        sub al, byte_5195A
        mov ah, 20h ; ' '
        call    convertNumberTo3DigitPaddedString
        mov si, 85EBh
        al = byte_5195B
        mov ah, 20h ; ' '
        call    convertNumberTo3DigitPaddedString
        mov si, 85C9h
        mov di, 73A9h
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        mov si, 85EFh
        mov di, 7D39h
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground

loc_4C55C:              ; CODE XREF: sub_4C4F9+31j
        mov si, 8600h
        mov di, 86BDh
        mov ah, 0Fh
        call    drawTextWithChars6FontWithTransparentBackground
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        mov si, palettesDataBuffer
        call    setPalette
        cmp word_5197A, 1
        jz  short loc_4C591
        call    waitForKeyMouseOrJoystick

loc_4C591:              ; CODE XREF: sub_4C4F9+93j
        mov si, 60D5h
        call    setPalette
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        return;
sub_4C4F9   endp
*/

void scrollRightToNewScreen() // sub_4C5AF   proc near       ; CODE XREF: handleGfxTutorOptionClick+3p
{
//    gNumberOfDotsToShiftDataLeft = 0;
    videoloop();

    uint8_t screenPixelsBackup[kFullScreenFramebufferLength];
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 40;
    const int kStepSize = kScreenWidth / kNumberOfSteps;

    // Draws the current scroll animation step
    for (int i = 0; i <= kNumberOfSteps; ++i)
    {
        for (int y = 0; y < kScreenHeight; ++y)
        {
            int limitFromLeft = kStepSize * (kNumberOfSteps - i);
            int limitFromRight = kScreenWidth - limitFromLeft;

            // Main menu side
            for (int x = 0; x < kScreenWidth - limitFromRight; ++x)
            {
                gScreenPixels[y * kScreenWidth + x] = screenPixelsBackup[y * kScreenWidth + x + limitFromRight];
            }

            // GFX background side
            for (int x = limitFromLeft; x < kScreenWidth; ++x)
            {
                gScreenPixels[y * kScreenWidth + x] = gScrollDestinationScreenBitmapData[y * kScreenWidth + x - limitFromLeft];
            }
        }

//loc_4C5BA:              ; CODE XREF: scrollRightToGfxTutor+3Cj
//        gNumberOfDotsToShiftDataLeft = 1;
        videoloop();
        loopForVSync();
//        gNumberOfDotsToShiftDataLeft = 5;
        videoloop();
        loopForVSync();
    }

//    gNumberOfDotsToShiftDataLeft = 0;
    videoloop();
}

void drawMenuBackground() //   proc near       ; CODE XREF: sub_4C407+14p
                    // ; sub_4C407:scrollLeftToMainMenup ...
{
    for (int y = 0; y < kScreenHeight; y++)
    {
//        loc_4C63E:             // ; CODE XREF: drawMenuBackground+4Dj
        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_4C641:             // ; CODE XREF: drawMenuBackground+47j
            uint32_t destPixelAddress = y * kScreenWidth + x;

            uint32_t sourcePixelAddress = y * kScreenWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (gMenuBitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (gMenuBitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (gMenuBitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (gMenuBitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            gScreenPixels[destPixelAddress] = finalColor;
        }
    }
}

void drawOptionsBackground(uint8_t *dest) // vgaloadcontrolsseg
{
    drawFullScreenBitmap(gControlsBitmapData, dest);
}

void drawBackBackground() // vgaloadbackseg
{
    drawFullScreenBitmap(gBackBitmapData, gScreenPixels);
}

void drawGfxTutorBackground(uint8_t *dest) // vgaloadgfxseg
{
    drawFullScreenBitmap(gGfxBitmapData, dest);
}

void drawFullScreenBitmap(uint8_t *bitmapData, uint8_t *dest)
{
    for (int y = 0; y < kScreenHeight; ++y)
    {
        for (int x = 0; x < kScreenWidth; ++x)
        {
            uint32_t destPixelAddress = y * kScreenWidth + x;

            uint32_t sourcePixelAddress = y * kScreenWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            dest[destPixelAddress] = finalColor;
        }
    }
}

void handleOptionsStandardClick() // sub_4C705  proc near       ; CODE XREF: code:5ADBp
{
    activateInternalStandardSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsInternalClick() // loc_4C6FB
{
    handleOptionsStandardClick();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsSamplesClick() // sub_4C70F  proc near
{
    activateInternalSamplesSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsSoundBlasterClick() // sub_4C719  proc near
{
    activateSoundBlasterSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsAdlibClick() // sub_4C723  proc near
{
    activateAdlibSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsRolandClick() // sub_4C72D  proc near
{
    activateRolandSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsCombinedClick() // sub_4C737  proc near
{
    activateCombinedSound();
    sound4();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsMusicClick() // sub_4C741   proc near
{
    if (isMusicEnabled == 1)
    {
        sound3();
        isMusicEnabled = 0;
    }
    else
    {
//loc_4C752:              ; CODE XREF: handleOptionsAdlibClick+5j
        isMusicEnabled = 1;
        sound2();
    }

//loc_4C75A:              ; CODE XREF: handleOptionsAdlibClick+Fj
    drawAudioOptionsSelection(gScreenPixels);
    return;
}

void handleOptionsFXClick() // loc_4C75E
{
    if (isFXEnabled == 1)
    {
        isFXEnabled = 0;
    }
    else
    {
//loc_4C76C:              ; CODE XREF: code:5B43j
        isFXEnabled = 1;
        sound4();
    }

//loc_4C774:              ; CODE XREF: code:5B4Aj
    drawAudioOptionsSelection(gScreenPixels);
}

void handleOptionsKeyboardClick() // loc_4C778
{
    isJoystickEnabled = 0;
    drawInputOptionsSelection(gScreenPixels);
}

void handleOptionsJoystickClick() // loc_4C781
{
    isJoystickEnabled = 1;
    sub_4921B();
    drawInputOptionsSelection(gScreenPixels);
}

void handleOptionsExitAreaClick() // loc_4C78D
{
    word_58463 = 1;
}

void runMainMenu() // proc near       ; CODE XREF: start+43Ap
{
    // 01ED:5B31
    byte_5A33E = 0;
    word_599D8 = 0;
    byte_599D4 = 0;
    word_58465 = 0xEF98;
    if (word_58467 != 0)
    {
//        goto loc_4C7EC; <- if word_58467 == 0
        drawMenuBackground(); // 01ED:5B4E
        byte_51ABE = 1;
        prepareLevelDataForCurrentPlayer(); // 01ED:5B56
        drawMenuTitleAndDemoLevelResult(); // 01ED:5B59

//        gNumberOfDotsToShiftDataLeft = 0;
        videoloop();
        fadeToPalette(gPalettes[1]); // 6015h
        word_58467 = 0;
    }
    else
    {
//loc_4C7EC:              // ; CODE XREF: runMainMenu+1Bj
        byte_59B83 = 1;
        sub_4C407(); // 01ED:5B8E
    }

//loc_4C7F4:              // ; CODE XREF: runMainMenu+56j
    sound2(); // 01ED:5B91
    saveLastMouseAreaBitmap();
    drawMouseCursor();

    while (1)
    {
//loc_4C7FD:              // ; CODE XREF: runMainMenu+121j
                   // ; runMainMenu+219j ...
        word_58465++;
        if (word_58465 == 0)
        {
            sub_4B159();
        }

//loc_4C806:              // ; CODE XREF: runMainMenu+6Dj
        if (word_5196C != 0)
        {
            word_5196C = 0;
            savePlayerListData();
            saveHallOfFameData();
            return;
        }

//loc_4C81A:              // ; CODE XREF: runMainMenu+77j
        videoloop();

        word_5195D++;
        uint16_t mouseX, mouseY;
        uint16_t mouseButtonStatus;
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        gMouseButtonStatus = mouseButtonStatus;
        if (gMouseX != mouseX
            || gMouseY != mouseY)
        {
//loc_4C834:              // ; CODE XREF: runMainMenu+98j
            word_58465 = 0xEF98;
        }

//loc_4C83A:              // ; CODE XREF: runMainMenu+9Ej
        gMouseX = mouseX;
        gMouseY = mouseY;
        restoreLastMouseAreaBitmap(); // 01ED:5BDF
        saveLastMouseAreaBitmap(); // 01ED:5BE2
        drawMouseCursor(); // 01ED:5BE5 Draws mouse cursor too?
        drawMainMenuButtonBorders(); // 01ED:5BE8
        sub_48E59();
        sub_4A1BF();
        if (gPlayerListDownButtonPressed != 0
            || gPlayerListUpButtonPressed != 0)
        {
            //loc_4C862:              // ; CODE XREF: runMainMenu+C5j
            gPlayerListButtonPressed = 1;
        }

//loc_4C867:              // ; CODE XREF: runMainMenu+CCj
        gPlayerListDownButtonPressed = 0;
        gPlayerListUpButtonPressed = 0;
        if (gRankingListDownButtonPressed != 0
            || gRankingListUpButtonPressed != 0)
        {
            //loc_4C87F:              // ; CODE XREF: runMainMenu+E2j
            gRankingListButtonPressed = 1;
        }

//loc_4C884:              // ; CODE XREF: runMainMenu+E9j
        gRankingListDownButtonPressed = 0;
        gRankingListUpButtonPressed = 0;
        if (gLevelListDownButtonPressed != 0
            || gLevelListUpButtonPressed != 0)
        {
            //loc_4C89C:              // ; CODE XREF: runMainMenu+FFj
            gLevelListButtonPressed = 1;
        }

//loc_4C8A1:              // ; CODE XREF: runMainMenu+106j
        gLevelListDownButtonPressed = 0;
        gLevelListUpButtonPressed = 0;
        if (byte_50941 > 4)
        {
            handleOkButtonClick();
        }
//loc_4C8B8:              // ; CODE XREF: runMainMenu+11Cj
        else if (byte_519B8 == 1)
        {
            ax = 0;
    //        demoSomething();
        }
//loc_4C8C8:              // ; CODE XREF: runMainMenu+129j
        else if (byte_519B9 == 1)
        {
            ax = 1;
    //        demoSomething();
        }
//loc_4C8D8:              // ; CODE XREF: runMainMenu+139j
        else if (byte_519BA == 1)
        {
            ax = 2;
    //        demoSomething();
        }
//loc_4C8E8:              // ; CODE XREF: runMainMenu+149j
        else if (byte_519BB == 1)
        {
            ax = 3;
    //        demoSomething();
        }
//loc_4C8F8:              // ; CODE XREF: runMainMenu+159j
        else if (byte_519BC == 1)
        {
            ax = 4;
    //        demoSomething();
        }
//loc_4C908:              // ; CODE XREF: runMainMenu+169j
        else if (byte_519BD == 1)
        {
            ax = 5;
    //        demoSomething();
        }
//loc_4C918:              // ; CODE XREF: runMainMenu+179j
        else if (byte_519BE == 1)
        {
            ax = 6;
    //        demoSomething();
        }
//loc_4C928:              // ; CODE XREF: runMainMenu+189j
        else if (byte_519BF == 1)
        {
            ax = 7;
//            demoSomething();
        }
//loc_4C937:              // ; CODE XREF: runMainMenu+199j
        else if (byte_519C0 == 1)
        {
            ax = 8;
    //        demoSomething();
        }
//loc_4C946:              // ; CODE XREF: runMainMenu+1A8j
        else if (byte_519C1 == 1)
        {
            ax = 9;
    //        demoSomething();
        }
//loc_4C955:              // ; CODE XREF: runMainMenu+1B7j
        else if (byte_519D4 == 1
                 && demoFileName != 0
                 && fileIsDemo == 1)
        {
            byte_599D4 = 1;
            ax = 0;
//            demoSomething();
        }
//loc_4C977:              // ; CODE XREF: runMainMenu+1C6j
                    // ; runMainMenu+1CDj ...
        else if (byte_519D5 == 1
                 && demoFileName != 0)
        {
            byte_599D4 = 1;
            word_5196C = 1;
            byte_510DE = 0;
            byte_510B3 = 0;
            byte_5A2F9 = 1;
            prepareSomeKindOfLevelIdentifier();
            // This adds dashes to the level name or something?
            a00s0010_sp[3] = 0x2D; // '-' ; "001$0.SP"
            a00s0010_sp[4] = 0x2D; // '-' ; "01$0.SP"
            a00s0010_sp[5] = 0x2D; // '-' ; "1$0.SP"
            continue;
        }
//loc_4C9B0:              // ; CODE XREF: runMainMenu+131j
                   // ; runMainMenu+141j ...
        if (gMouseButtonStatus == MouseButtonRight) // Right button -> exit game
        {
            break;
        }
        if (byte_5197E == 1)
        {
            break;
        }
        if (word_5197A == 1)
        {
            break;
        }
        if (gMouseButtonStatus == MouseButtonLeft)
        {
//loc_4C9FF:              // ; CODE XREF: runMainMenu+236j
            word_58465 = 0xEF98;

            for (int i = 0; i < kNumberOfMainMenuButtons; ++i)
            {
                ButtonDescriptor buttonDescriptor = kMainMenuButtonDescriptors[i];

//checkmousecoords:              // ; CODE XREF: runMainMenu+29Bj
                if (gMouseX >= buttonDescriptor.startX
                    && gMouseY >= buttonDescriptor.startY
                    && gMouseX <= buttonDescriptor.endX
                    && gMouseY <= buttonDescriptor.endY)
                {
                    buttonDescriptor.handler(); // 01ED:5DC0
                    break;
                }

//nomousehit:              // ; CODE XREF: runMainMenu+27Ej
                   // ; runMainMenu+283j ...
            }
        }
        else
        {
            word_58469 = 0x10;
            word_5846B = 0;
            word_5846D = 0x10;
            word_5846F = 0;
            word_58471 = 0x10;
            word_58473 = 0;
            word_59B8C = 0x10;
            word_59B8E = 0;
        }
    }

//loc_4CA34:              // ; CODE XREF: runMainMenu+223j
               // ; runMainMenu+22Aj ...
    word_5197A = 1;
    savePlayerListData();
    saveHallOfFameData();
}

void handleControlsOptionClick() //showControls:                              ; DATA XREF: data:0044o
{
    // 01ED:5DDE;
    byte_50919 = 0xFF;
    drawOptionsBackground(gScrollDestinationScreenBitmapData);
    drawSoundTypeOptionsSelection(gScrollDestinationScreenBitmapData);
    drawAudioOptionsSelection(gScrollDestinationScreenBitmapData);
    drawInputOptionsSelection(gScrollDestinationScreenBitmapData);
//    mov     si, 6055h
    setPalette(gPalettes[2]);
    scrollRightToNewScreen();
    word_58463 = 0;
    saveLastMouseAreaBitmap();
    drawMouseCursor();

    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    do
    {
//loc_4CA67:                              ; CODE XREF: code:5E89j
//                            ; code:5EBFj ...
        videoloop(); // 01ED:5E04
        updateOptionsMenuState(gScreenPixels);
        word_5195D++;
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);

        gMouseButtonStatus = mouseButtonStatus;
        gMouseX = mouseX;
        gMouseY = mouseY;

        restoreLastMouseAreaBitmap();
        saveLastMouseAreaBitmap();
        drawMouseCursor();
        if (gMouseButtonStatus == 2)
        {
            break;
        }
        if (byte_5197E == 1)
        {
            break;
        }
        if (word_58463 == 1)
        {
            break;
        }
        if (gMouseButtonStatus == 1)
        {
    //loc_4CAAB:                              ; CODE XREF: code:5E87j
        //    mov     si, offset controlsbuttons ; 0ACh // 01ED:5E54

            for (int i = 0; i < kNumberOfOptionsMenuButtons; ++i)
            {
                ButtonDescriptor buttonDescriptor = kOptionsMenuButtonDescriptors[i];
//loc_4CABA:                              ; CODE XREF: code:5EC7j
                if (gMouseX >= buttonDescriptor.startX
                    && gMouseY >= buttonDescriptor.startY
                    && gMouseX <= buttonDescriptor.endX
                    && gMouseY <= buttonDescriptor.endY)
                {
                    buttonDescriptor.handler(); // 01ED:5E6A

                    do
                    {
//loc_4CAD0:                              ; CODE XREF: code:5EBDj
                        videoloop();
                        word_5195D++;
                        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
                    }
                    while (mouseButtonStatus != 0);
                }
            }
        }
    }
    while (1);

//loc_4CAEC:                              ; CODE XREF: code:5E74j
//                            ; code:5E7Bj ...
    saveConfiguration();
    scrollLeftToMainMenu();
    drawMenuTitleAndDemoLevelResult();
    setPalette(gPalettes[1]);
}

void drawSoundTypeOptionsSelection(uint8_t *destBuffer) // sub_4CAFC   proc near       ; CODE XREF: code:5AE1p handleOptionsStandardClick+6p ...
{
    dimOptionsButtonText(40, 21, 40, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[0], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[1], 4, destBuffer);

    dimOptionsButtonText(24, 57, 72, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[2], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[3], 4, destBuffer);

    dimOptionsButtonText(32, 93, 56, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[4], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[5], 4, destBuffer);

    dimOptionsButtonText(24, 129, 64, 8, destBuffer);
    dimOptionsButtonText(136, 18, 72, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[7], 4, destBuffer);

    dimOptionsButtonText(128, 46, 40, 5, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[8], 4, destBuffer);

    dimOptionsButtonText(176, 46, 40, 5, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[9], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[6], 4, destBuffer);

    if (sndType == SoundTypeAdlib)
    {
        highlightOptionsButtonText(40, 21, 40, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[0], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[1], 6, destBuffer);
        return;
    }

//loc_4CBC6:              ; CODE XREF: drawSoundTypeOptionsSelection+A9j
    if (sndType == SoundTypeSoundBlaster)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[3], 6, destBuffer);

        if (musType == SoundTypeAdlib)
        {
            highlightOptionsButtonText(24, 57, 72, 8, destBuffer);
            drawOptionsMenuLine(kOptionsMenuBorders[2], 6, destBuffer);
            return;
        }

//loc_4CBF3:              ; CODE XREF: drawSoundTypeOptionsSelection+DEj
        highlightOptionsButtonText(24, 129, 64, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[4], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[6], 6, destBuffer);
        return;
    }

//loc_4CC11:              ; CODE XREF: drawSoundTypeOptionsSelection+CFj
    if (sndType == SoundTypeRoland)
    {
        highlightOptionsButtonText(32, 93, 56, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[4], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[5], 6, destBuffer);
        return;
    }

//loc_4CC36:              ; CODE XREF: drawSoundTypeOptionsSelection+11Aj
    highlightOptionsButtonText(136, 18, 72, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[7], 6, destBuffer);

    if (sndType == SoundTypeInternalStandard)
    {
        highlightOptionsButtonText(128, 46, 40, 5, destBuffer); // Standard
        drawOptionsMenuLine(kOptionsMenuBorders[8], 6, destBuffer);
        return;
    }

//loc_4CC67:              ; CODE XREF: drawSoundTypeOptionsSelection+153j
    highlightOptionsButtonText(176, 46, 40, 5, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[9], 6, destBuffer);
}

void drawAudioOptionsSelection(uint8_t *destBuffer) // sub_4CC7C   proc near       ; CODE XREF: handleOptionsAdlibClick:loc_4C75Ap
                   // ; code:loc_4C774p
{
    if (isMusicEnabled == 1)
    {
        highlightOptionsButtonText(134, 99, 40, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[10], 6, destBuffer);
    }
    else
    {
//loc_4CC99:              ; CODE XREF: drawAudioOptionsSelection+5j
        dimOptionsButtonText(134, 99, 40, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[10], 4, destBuffer);
    }

//loc_4CCAD:              ; CODE XREF: drawAudioOptionsSelection+1Bj
    if (isFXEnabled == 1)
    {
        highlightOptionsButtonText(136, 138, 24, 8, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[11], 6, destBuffer);
        return;
    }

//loc_4CCCA:              ; CODE XREF: drawAudioOptionsSelection+36j
    dimOptionsButtonText(136, 138, 24, 8, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[11], 4, destBuffer);
}

void drawInputOptionsSelection(uint8_t *destBuffer) // sub_4CCDF   proc near       ; CODE XREF: code:5B5Dp code:5B69p
{
    if (isJoystickEnabled == 0)
    {
        highlightOptionsButtonText(208, 87, 8, 62, destBuffer);
        dimOptionsButtonText(240, 88, 8, 58, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[18], 4, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[19], 6, destBuffer);
    }
    else
    {
//loc_4CD10:              ; CODE XREF: drawInputOptionsSelection+5j
        dimOptionsButtonText(208, 87, 8, 62, destBuffer);
        highlightOptionsButtonText(240, 88, 8, 58, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[18], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[19], 4, destBuffer);
    }

//loc_4CD38:              ; CODE XREF: drawInputOptionsSelection+2Fj
    updateOptionsMenuState(destBuffer);
}

void updateOptionsMenuState(uint8_t *destBuffer) // sub_4CD3C   proc near       ; CODE XREF: drawInputOptionsSelection:loc_4CD38p
{
    sub_48E59();
    sub_4A1BF();
    if (byte_50941 == byte_50919)
    {
        return;
    }

//loc_4CD4D:              ; CODE XREF: updateOptionsMenuState+Ej
    byte_50919 = byte_50941;
    if (byte_50941 == 0)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[12], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[17], 4, destBuffer);
    }
    else
    {
//loc_4CD6A:              ; CODE XREF: updateOptionsMenuState+1Aj
        if (byte_50941 > 4)
        {
//loc_4CD9E:              ; CODE XREF: updateOptionsMenuState+33j
            drawOptionsMenuLine(kOptionsMenuBorders[12], 4, destBuffer);
            drawOptionsMenuLine(kOptionsMenuBorders[17], 4, destBuffer);
        }
        else
        {
            drawOptionsMenuLine(kOptionsMenuBorders[17], 6, destBuffer);
            if (byte_50941 == 9)
            {
//loc_4CD8F:              ; CODE XREF: updateOptionsMenuState+42j
                byte_50941 -= 4;
                drawOptionsMenuLine(kOptionsMenuBorders[12], 4, destBuffer);
            }
            else
            {
                byte_50941 = 0;
                drawOptionsMenuLine(kOptionsMenuBorders[12], 6, destBuffer);
            }
        }
    }

//loc_4CDAE:              ; CODE XREF: updateOptionsMenuState+2Cj
//                ; updateOptionsMenuState+51j ...
    drawOptionsMenuLine(kOptionsMenuBorders[13], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[14], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[15], 4, destBuffer);
    drawOptionsMenuLine(kOptionsMenuBorders[16], 4, destBuffer);
    if (byte_50941 == 1)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[13], 6, destBuffer);
        return;
    }

//loc_4CDDF:              ; CODE XREF: updateOptionsMenuState+97j
    if (byte_50941 == 2)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[14], 6, destBuffer);
        return;
    }

//loc_4CDF0:              ; CODE XREF: updateOptionsMenuState+A8j
    if (byte_50941 == 3)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[15], 6, destBuffer);
        return;
    }

//loc_4CE01:              ; CODE XREF: updateOptionsMenuState+B9j
    if (byte_50941 == 4)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[16], 6, destBuffer);
    }
}

void highlightOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer) // sub_4CE11   proc near       ; CODE XREF: drawSoundTypeOptionsSelection+B4p
                   // ; drawSoundTypeOptionsSelection+E9p ...
{
    // Copies a portion of the buffer replacing color 0xF (not selected)
    // with color 0x1 (selected).
    // Used in the options screen to highlight text from buttons.
    //
    // Parameters:
    // - si: origin coordinates
    // - cx: width / 8
    // - dx: height

    restoreLastMouseAreaBitmap();

    for (size_t y = startY; y < startY + height; ++y)
    {
        for (size_t x = startX; x < startX + width; ++x)
        {
//loc_4CE68:              ; CODE XREF: highlightOptionsButtonText+5Aj
//                ; highlightOptionsButtonText+6Aj
            size_t addr = (y * kScreenWidth) + x;
            destBuffer[addr] = (destBuffer[addr] == 0xF
                                ? 0x1
                                : destBuffer[addr]);
        }
    }

    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void dimOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer) // sub_4CE9C   proc near       ; CODE XREF: drawSoundTypeOptionsSelection+9p
                   // ; drawSoundTypeOptionsSelection+25p ...
{
    // Copies a portion of the buffer replacing color 0x1 (selected)
    // with color 0xF (not selected).
    // Used in the options screen to dim text from buttons.
    //
    // Parameters:
    // - si: coordinates
    // - cx: width / 8
    // - dx: height

    restoreLastMouseAreaBitmap();

    for (size_t y = startY; y < startY + height; ++y)
    {
        for (size_t x = startX; x < startX + width; ++x)
        {
            //loc_4CEF3:              ; CODE XREF: dimOptionsButtonText+5Aj
            //                ; dimOptionsButtonText+6Aj
            size_t addr = (y * kScreenWidth) + x;

            destBuffer[addr] = (destBuffer[addr] == 0x1
                                ? 0xF
                                : destBuffer[addr]);
        }
    }

    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void drawOptionsMenuLine(ButtonBorderDescriptor border, uint8_t color, uint8_t *destBuffer) // sub_4CF13  proc near       ; CODE XREF: drawSoundTypeOptionsSelection+11p
                   // ; drawSoundTypeOptionsSelection+19p ...
{
    // Parameters:
    // - ah: color
    // - si: pointer to ButtonBorderDescriptor item

    restoreLastMouseAreaBitmap();

//loc_4CF38:              ; CODE XREF: drawOptionsMenuLine+96j
    for (int i = 0; i < border.numberOfLines; ++i)
    {
        ButtonBorderLineDescriptor line = border.lines[i];

        for (int j = 0; j < line.length; ++j)
        {
            size_t destAddress = 0;
            if (line.type == ButtonBorderLineTypeHorizontal)
            {
                destAddress = line.y * kScreenWidth + line.x + j;
            }
            else if (line.type == ButtonBorderLineTypeVertical)
            {
                destAddress = (line.y - j) * kScreenWidth + line.x;
            }
            else if (line.type == ButtonBorderLineTypeBottomLeftToTopRightDiagonal)
            {
                destAddress = (line.y - j) * kScreenWidth + line.x + j;
            }
            else if (line.type == ButtonBorderLineTypeTopLeftToBottomRightDiagonal)
            {
                destAddress = (line.y + j) * kScreenWidth + line.x + j;
            }

            destBuffer[destAddress] = color;

//loc_4CFA4:              ; CODE XREF: drawOptionsMenuLine:loc_4CF7Cj
//                ; drawOptionsMenuLine+73j ...
        }
    }

//loc_4CFAB:              ; CODE XREF: drawOptionsMenuLine+2Aj
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void savePlayerListData() //   proc near       ; CODE XREF: handleNewPlayerOptionClick+1D5p
//                    ; handleDeletePlayerOptionClick+CEp ...
{
    if (byte_59B85 != 0)
    {
        return;
    }
    FILE *file = fopen("PLAYER.LST", "w");
    if (file == NULL)
    {
        return;
    }

    assert(sizeof(gPlayerListData) == 0xA00);

    fwrite(gPlayerListData, 1, sizeof(gPlayerListData), file);

    fclose(file);
}

void saveHallOfFameData() //   proc near       ; CODE XREF: handleNewPlayerOptionClick+1D8p
//                    ; handleDeletePlayerOptionClick+D1p ...
{
    if (byte_59B85 != 0)
    {
        return;
    }

    FILE *file = fopen("HALLFAME.LST", "w");
    if (file == NULL)
    {
        return;
    }

    assert(sizeof(gHallOfFameData) == 0x24);

    fwrite(gHallOfFameData, 1, sizeof(gHallOfFameData), file);

    fclose(file);
}

void drawMainMenuButtonBorder(ButtonBorderDescriptor border, uint8_t color) // sub_4D004  proc near       ; CODE XREF: drawMainMenuButtonBorders+17p
//                    ; drawMainMenuButtonBorders+2Ap ...
{
    // 01ED:63A1
    // Parameters:
    // - si: button border descriptor
    // - ah: color??? it's either 7 or 0xD in drawMainMenuButtonBorders

    restoreLastMouseAreaBitmap();

//loc_4D029:              ; CODE XREF: drawButtonBorder+96j
    for (int i = 0; i < border.numberOfLines; ++i)
    {
        ButtonBorderLineDescriptor line = border.lines[i];

        for (int j = 0; j < line.length; ++j)
        {
            size_t destAddress = 0;
            if (line.type == ButtonBorderLineTypeHorizontal)
            {
                destAddress = line.y * kScreenWidth + line.x + j;
            }
            else if (line.type == ButtonBorderLineTypeVertical)
            {
                destAddress = (line.y - j) * kScreenWidth + line.x;
            }
            else if (line.type == ButtonBorderLineTypeBottomLeftToTopRightDiagonal)
            {
                destAddress = (line.y - j) * kScreenWidth + line.x + j;
            }
            else if (line.type == ButtonBorderLineTypeTopLeftToBottomRightDiagonal)
            {
                destAddress = (line.y + j) * kScreenWidth + line.x + j;
            }

            gScreenPixels[destAddress] = color;

//loc_4D095:              ; CODE XREF: drawButtonBorder:loc_4D06Dj
        //                ; drawButtonBorder+73j ...
        }
    }

//loc_4D09C:              ; CODE XREF: drawButtonBorder+2Aj
    saveLastMouseAreaBitmap();
    drawMouseCursor();
}

void drawMainMenuButtonBorders() // sub_4D0AD  proc near       ; CODE XREF: runMainMenu+B7p
{
    // 01ED:644A
    uint8_t color = 0;

    if (gPlayerListButtonPressed != 0)
    {
        if (gPlayerListUpButtonPressed == 0)
        {
            color = 7;
        }
        else
        {
            color = 0xD; // 13
        }

    //loc_4D0C1:              ; CODE XREF: drawMainMenuButtonBorders+10j
        drawMainMenuButtonBorder(kMainMenuButtonBorders[0], color);
        if (gPlayerListUpButtonPressed == 0)
        {
            color = 0xD; // 13
        }
        else
        {
            color = 7;
        }

    //loc_4D0D4:              ; CODE XREF: drawMainMenuButtonBorders+23j
        drawMainMenuButtonBorder(kMainMenuButtonBorders[1], color);
        if (gPlayerListDownButtonPressed == 0)
        {
            color = 7;
        }
        else
        {
            color = 0xD; // 13
        }

    //loc_4D0E7:              ; CODE XREF: drawMainMenuButtonBorders+36j
        drawMainMenuButtonBorder(kMainMenuButtonBorders[2], color);
        if (gPlayerListDownButtonPressed == 0)
        {
            color = 0xD; // 13
        }
        else
        {
            color = 7;
        }

    //loc_4D0FA:              ; CODE XREF: drawMainMenuButtonBorders+49j
        drawMainMenuButtonBorder(kMainMenuButtonBorders[3], color);
        gPlayerListButtonPressed = 0;
    }

//loc_4D105:              ; CODE XREF: drawMainMenuButtonBorders+5j
    if (gRankingListButtonPressed != 0)
    {
        if (gRankingListUpButtonPressed == 0)
        {
            color = 7;
        }
        else
        {
            color = 0xD; // 13
        }

    //loc_4D119:              ; CODE XREF: drawMainMenuButtonBorders+68j
//        si = 0x558; // 1368
        drawMainMenuButtonBorder(kMainMenuButtonBorders[4], color);
        if (gRankingListUpButtonPressed == 0)
        {
            color = 0xD;
        }
        else
        {
            color = 7;
        }

    //loc_4D12C:              ; CODE XREF: drawMainMenuButtonBorders+7Bj
//        si = 0x56D;
        drawMainMenuButtonBorder(kMainMenuButtonBorders[5], color);
        if (gRankingListDownButtonPressed == 0)
        {
            color = 7;
        }
        else
        {
            color = 0xD;
        }

    //loc_4D13F:              ; CODE XREF: drawMainMenuButtonBorders+8Ej
//        si = 0x582;
        drawMainMenuButtonBorder(kMainMenuButtonBorders[6], color);
        if (gRankingListDownButtonPressed == 0)
        {
            color = 0xD;
        }
        else
        {
            color = 7;
        }

    //loc_4D152:              ; CODE XREF: drawMainMenuButtonBorders+A1j
//        si = 0x597;
        drawMainMenuButtonBorder(kMainMenuButtonBorders[7], color);
        gRankingListButtonPressed = 0;
    }

//loc_4D15D:              ; CODE XREF: drawMainMenuButtonBorders+5Dj
    if (gLevelListButtonPressed == 0)
    {
        return;
    }
    if (gLevelListUpButtonPressed == 0)
    {
        color = 7;
    }
    else
    {
        color = 0xD;
    }

//loc_4D171:              ; CODE XREF: drawMainMenuButtonBorders+C0j
//    si = 0x5AC;
    drawMainMenuButtonBorder(kMainMenuButtonBorders[8], color);
    if (gLevelListUpButtonPressed == 0)
    {
        color = 0xD;
    }
    else
    {
        color = 7;
    }

//loc_4D184:              ; CODE XREF: drawMainMenuButtonBorders+D3j
//    si = 0x5C1;
    drawMainMenuButtonBorder(kMainMenuButtonBorders[9], color);
    if (gLevelListDownButtonPressed == 0)
    {
        color = 7;
    }
    else
    {
        color = 0xD;
    }

//loc_4D197:              ; CODE XREF: drawMainMenuButtonBorders+E6j
//    si = 0x5D6;
    drawMainMenuButtonBorder(kMainMenuButtonBorders[10], color);
    if (gLevelListDownButtonPressed == 0)
    {
        color = 0xD;
    }
    else
    {
        color = 7;
    }

//loc_4D1AA:              ; CODE XREF: drawMainMenuButtonBorders+F9j
//    si = 0x5EB;
    drawMainMenuButtonBorder(kMainMenuButtonBorders[11], color);
    gLevelListButtonPressed = 0;
}

void updateHallOfFameEntries() // sub_4D1B6  proc near       ; CODE XREF: changePlayerCurrentLevelState+2Ep
{
    // 01ED:6553
    if (byte_510DE != 0)
    {
        return;
    }

//loc_4D1BE:              ; CODE XREF: updateHallOfFameEntries+5j
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];
    if (currentPlayerEntry.completedAllLevels != 0)
    {
        return;
    }

//loc_4D1D2:              ; CODE XREF: updateHallOfFameEntries+19j
    int numberOfCompletedLevels = 0;

    for (int i = 0; i < kNumberOfLevels; ++i)
    {
//loc_4D1E2:              ; CODE XREF: updateHallOfFameEntries+33j
        if (currentPlayerEntry.levelState[i] == PlayerLevelStateCompleted)
        {
            numberOfCompletedLevels++;
        }
//loc_4D1E8:              ; CODE XREF: updateHallOfFameEntries+2Fj
    }

    if (numberOfCompletedLevels != kNumberOfLevels)
    {
        return;
    }

    currentPlayerEntry.completedAllLevels = 1;

    int newEntryInsertIndex = -1;
//    mov cx, 3
//    mov di, hallFameDataBuffer
    for (int i = 0; i < kNumberOfHallOfFameEntries; ++i)
    {
        HallOfFameEntry entry = gHallOfFameData[i];

//loc_4D1FA:              ; CODE XREF: updateHallOfFameEntries+78j
        if (entry.hours == 0
            && entry.minutes == 0
            && entry.seconds == 0)
        {
            newEntryInsertIndex = i;
            break;
        }

//loc_4D20E:              ; CODE XREF: updateHallOfFameEntries+48j
//                ; updateHallOfFameEntries+4Ej ...
        if (currentPlayerEntry.hours < entry.hours)
        {
            newEntryInsertIndex = i;
            break;
        }
        else if (currentPlayerEntry.hours == entry.hours)
        {
            if (currentPlayerEntry.minutes < entry.minutes)
            {
                newEntryInsertIndex = i;
                break;
            }
            else if (currentPlayerEntry.minutes == entry.minutes)
            {
                if (currentPlayerEntry.seconds < entry.seconds)
                {
                    newEntryInsertIndex = i;
                    break;
                }
            }
        }

//loc_4D22A:              ; CODE XREF: updateHallOfFameEntries+60j
//                ; updateHallOfFameEntries+6Aj
    }

    if (newEntryInsertIndex != -1)
    {
//loc_4D232:              ; CODE XREF: updateHallOfFameEntries+56j
//                ; updateHallOfFameEntries+5Ej ...

        // Shift the list to the right to make room for the new entry
        for (int i = kNumberOfHallOfFameEntries - 1; i >=  newEntryInsertIndex + 1; --i)
        {
            memcpy(&gHallOfFameData[i], &gHallOfFameData[i - 1], sizeof(HallOfFameEntry));
        }

        // Copy the player info into the new entry
        HallOfFameEntry *newEntry = &gHallOfFameData[newEntryInsertIndex];

        memcpy(newEntry->playerName,
               currentPlayerEntry.name,
               sizeof(currentPlayerEntry.name));
        newEntry->hours = currentPlayerEntry.hours;
        newEntry->minutes = currentPlayerEntry.minutes;
        newEntry->seconds = currentPlayerEntry.seconds;
    }

//loc_4D24B:              ; CODE XREF: updateHallOfFameEntries+38j
//                ; updateHallOfFameEntries+7Aj
}

void changePlayerCurrentLevelState() // sub_4D24D  proc near       ; CODE XREF: handleSkipLevelOptionClick+D9p
//                    ; update?:loc_4E6A4p
{
    // 01ED:65EA
    if (byte_510DE != 0)
    {
        return;
    }
    if (byte_5A2F9 != 0)
    {
        return;
    }
    uint8_t previousValue = byte_510BB;
    byte_510BB = 0;

    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
    currentPlayerEntry->levelState[gCurrentSelectedLevelIndex - 1] = previousValue;
    gCurrentSelectedLevelIndex++;
    updateHallOfFameEntries(); // 01ED:6618
}

void checkVideo() //  proc near       ; CODE XREF: start+282p
{
    videoStatusUnk = 1; // THIS IS NOT IN THE ASM!! I think we can do just this and trust is the best video mode
    return;
    /*
        // mov ah, 0Fh
        // int 10h     ; - VIDEO - GET CURRENT VIDEO MODE
        //             ; Return: AH = number of columns on screen
        //             ; AL = current video mode
        //             ; BH = current active display page
    currVideoMode = al;
    ax = 0x0D; // 320x200 16 color graphics (EGA, VGA) according to http://stanislavs.org/helppc/int_10-0.html
        // int 10h     ; - VIDEO - SET VIDEO MODE
        //             ; AL = mode
    bx = 0;

    cx = 0x100; // 256

    // The code below is something like this? Is trying to set different values to the bitmask
    // and then checking if the value was correctly stored??
    // Maybe to check some kind of capability?
    for (int i = 256; i > 0; ++i)
    {
videoCheckStart:            // ; CODE XREF: checkVideo:checkAgainj
        // dx = 0x3CE; // This is a VGA port
        // al = 8;
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; bit mask
        //             ; Bits 0-7 select bits to be masked in all planes
        ports[0x3CE] = 8; // this is to check the bitmask
        // dx++;
        // al = cl;
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = i;
        // in  al, dx      ; EGA port: graphics controller data register
        al = ports[0x3CF];

        if (ports[0x3CF] != i)
        {
            bx++; // increase number of "incompatible" bitmasks
        }

checkAgain:             //; CODE XREF: checkVideo+29j
        // cx--;
        // if (cx > 0)
        // {
        //     goto videoCheckStart;
        // }
    }
    
    if (bx != 0) // if there is at least 1 incompatible bitmask... set status flag to 2. less colors maybe?
    {
        goto setStatusBit2;
    }
    
    videoStatusUnk = 1;
    goto returnout;
// ; ---------------------------------------------------------------------------

setStatusBit2:              // ; CODE XREF: checkVideo+30j
    videoStatusUnk = 2;

returnout:                  // ; CODE XREF: checkVideo+37j
        //return;
     */
}

void initializeFadePalette() //   proc near       ; CODE XREF: start+296p
{
    if (videoStatusUnk == 1)
    {
        // Probably useless?
        /*
        // grayscale palette?
        for (int i = 16; i > 0; i--)
        {
//loc_4D2C9:              //; CODE XREF: initializeFadePalette+17j
            // push(cx);
            // ax = 0x1000;
            // bl = cl;
            // bl--;
            // bh = bl;
            // int 10h     ; - VIDEO - SET PALETTE REGISTER (Jr, PS, TANDY 1000, EGA, VGA)
            //             ; BL = palette register to set
            //             ; BH = color value to store
            // pop(cx);
            
            set_palette_register(i-1, i-1); // (number of register, value)
        }
//        jmp short $+2
         */
    }

//loc_4D2DA:              //; CODE XREF: initializeFadePalette+5j
                    //; initializeFadePalette+19j
    fadeToPalette(gBlackPalette);
}

void resetVideoMode() // sub_4D2E1   proc near       ; CODE XREF: start+450p
//                    ; loadScreen2-7D7p
{
    // Do nothing because we don't need to restore the video mode?
//    mov ah, 0
//    al = currVideoMode
//    int 10h     ; - VIDEO - SET VIDEO MODE
//                ; AL = mode
//    return;
}

void initializeMouse() //   proc near       ; CODE XREF: start+299p
{
    gIsMouseAvailable = 1; // THIS IS NOT FROM THE ASM: assume there is a mouse available
    SDL_ShowCursor(SDL_DISABLE);
    SDL_PumpEvents();
    return;
/*
    // Check if mouse handler is available
    gIsMouseAvailable = 0;
//    push    es
//    mov ah, 35h ; '5'
//    al = 33h ; '3'
//    int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
//                ; AL = interrupt number
//                ; Return: ES:BX = value of interrupt vector
    ax = es;
    ax = ax | bx;
    if (ax != 0)
    {
        if (*(es:bx) != 0xCF) // 207
        {
//            ax = 0;
            // int 33h     ; - MS MOUSE - RESET DRIVER AND READ STATUS
            //             ; Return: AX = status
            //             ; BX = number of buttons
            if (bx == 2 || bx == 3)
            {
//mouseHas2Or3Buttons:              ; CODE XREF: initializeMouse+20j
                gIsMouseAvailable = 1;
                // TODO: hide mouse

            //    ax = 2;
            //    int 33h     ; - MS MOUSE - HIDE MOUSE CURSOR
            //                ; SeeAlso: AX=0001h, INT 16/AX=FFFFh
                // TODO: limit mouse X between 16-304
            //    ax = 0x7;
            //    cx = 0x20; // 32
            //    dx = 0x260; // 608
            //    int 33h     ; - MS MOUSE - DEFINE HORIZONTAL CURSOR RANGE
            //                ; CX = minimum column, DX = maximum column
                // TODO: limit mouse Y between 8-192
            //    ax = 0x8;
            //    cx = 0x8; // 8
            //    dx = 0x0C0; // 192
            //    int 33h     ; - MS MOUSE - DEFINE VERTICAL CURSOR RANGE
            //                ; CX = minimum row, DX = maximum row

                // TODO: Center mouse on the screen: (160, 100)
//                ax = 0x4;
//                cx = 0x140;
//                dx = 0x64; // ; 'd' 100
            //    int 33h     ; - MS MOUSE - POSITION MOUSE CURSOR
            //                ; CX = column, DX = row
            }

//loc_4D33B:              ; CODE XREF: initializeMouse+10j
//                ; initializeMouse+16j ...
            gCursorX = 0xA0; // '?' 160
            gCursorY = 0x64; // 'd' 100
            *dword_58488 = 1; //mov word ptr dword_58488, 1
            //    pop es
        }
    }
*/
}

void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus) //   proc near       ; CODE XREF: waitForKeyMouseOrJoystick:mouseIsClickedp
//                    ; waitForKeyMouseOrJoystick+3Ep ...
{
    // Returns coordinate X in CX (0-320) and coordinate Y in DX (0-200).
    // Also button status in BX.

    if (gIsMouseAvailable != 0)
    {
        SDL_PumpEvents();

        int x, y;
        Uint32 state = SDL_GetMouseState(&x, &y);

        x = x * kScreenWidth / kWindowWidth;
        y = y * kScreenHeight / kWindowHeight;

        // Limit coordinates as in the original game
        x = CLAMP(x, 16, 304);
        y = CLAMP(y, 8, 192);

        if (mouseX != NULL)
        {
            *mouseX = x;
        }
        if (mouseY != NULL)
        {
            *mouseY = y;
        }

//        printf("mouse: %d, %d\n", x, y);

        uint8_t leftButtonPressed = (state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        uint8_t rightButtonPressed = (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

        if (mouseButtonStatus != NULL)
        {
            *mouseButtonStatus = (rightButtonPressed << 1
                                  | leftButtonPressed);
        }

        return;
    }

/*
    // No idea what is this? Maybe to control the mouse with a joystick or keyboard?

//loc_4D360:              ; CODE XREF: getMouseStatus+5j
    bx = 0;
    // This emulates the right mouse button
    if (byte_5197E == 1)
    {
        bx = 2;
    }

//loc_4D36C:              ; CODE XREF: getMouseStatus+18j
    // This emulates the left mouse button
    if (byte_51999 == 1)
    {
        bx = 1;
    }

//loc_4D376:              ; CODE XREF: getMouseStatus+22j
    cx = 1;
    // This might be something like the cursor speed?
    ax = *dword_58488; // mov ax, word ptr dword_58488
    ax = ax / 8;
    ax++;
    if (byte_519C5 == 1)
    {
        gCursorY -= ax;
        cx = 0;
    }

//loc_4D390:              ; CODE XREF: getMouseStatus+39j
    if (byte_519CD == 1)
    {
        gCursorY += ax;
        cx = 0;
    }

//loc_4D39D:              ; CODE XREF: getMouseStatus+46j
    if (byte_519C8 == 1)
    {
        gCursorX -= ax;
        cx = 0;
    }

//loc_4D3AA:              ; CODE XREF: getMouseStatus+53j
    if (byte_519CA == 1)
    {
        gCursorX += ax;
        cx = 0;
    }

//loc_4D3B7:              ; CODE XREF: getMouseStatus+60j
    // Maybe this makes the speed higher as you keep the joystick or whatever pressed
    if (cx == 0)
    {
        (*dword_58488)++; //inc word ptr dword_58488
    }
    else
    {
        *dword_58488 = 1; //mov word ptr dword_58488, 1
    }

//loc_4D3C7:              ; CODE XREF: getMouseStatus+70j
    // With a maximum of 64 pixels
    if (*dword_58488 > 0x40) // 64 or '@'
    {
        *dword_58488 = 0x40; //mov word ptr dword_58488, 40h ; '@'
    }

//loc_4D3D4:              ; CODE XREF: getMouseStatus+7Dj
    cx = gCursorX;
    dx = gCursorY;

    // Limit X between 16 and 304
    if (cx <= 16)
    {
        cx = 16;
        gCursorX = cx;
    }

//loc_4D3E8:              ; CODE XREF: getMouseStatus+90j
    if (cx >= 0x130) // 304
    {
        cx = 0x130; //mov cx, 130h
        gCursorX = cx;
    }

//loc_4D3F5:              ; CODE XREF: getMouseStatus+9Dj
    // Limit Y between 8 and 192
    if (dx <= 8)
    {
        dx = 8;
        gCursorY = dx;
    }

//loc_4D401:              ; CODE XREF: getMouseStatus+A9j
    if (dx >= 0xC0) // 192
    {
        dx = 0xC0;
        gCursorY = dx;
    }

    // Returns coordinate X in CX (0-320) and coordinate Y in DX (0-200).
    // Also button status in BX.
    *mouseButtonStatus = bx;
    *mouseX = cx;
    *mouseY = dx;
 */
}

void videoloop() //   proc near       ; CODE XREF: crt?2+52p crt?1+3Ep ...
{
    SDL_PumpEvents(); // Make sure the app stays responsive

    SDL_BlitSurface(gScreenSurface, NULL, gTextureSurface, NULL);

    SDL_UpdateTexture(gTexture, NULL, gTextureSurface->pixels, gTextureSurface->pitch);
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);

//        push    dx
//        push    ax
//        cmp byte ptr dword_59B67, 0
//        jnz short waitForVsync
//        mov dx, 3DAh // check http://stanislavs.org/helppc/6845.html
//        cli
//        in  al, dx      ; Video status bits:
//                    ; 0: retrace.  1=display is in vert or horiz retrace.
//                    ; 1: 1=light pen is triggered; 0=armed
//                    ; 2: 1=light pen switch is open; 0=closed
//                    ; 3: 1=vertical sync pulse is occurring.
//        mov ah, al
//        mov dx, 3C0h
//        al = 33h ; '3'
//        out dx, al      ; EGA: horizontal pixel panning:
//                    ; Number of dots to shift data left.
//                    ; Bits 0-3 valid (0-0fH)
//        al = gNumberOfDotsToShiftDataLeft
//        out dx, al      ; EGA: palette register: select colors for attribute AL:
//                    ; 0: RED
//                    ; 1: GREEN
//                    ; 2: BLUE
//                    ; 3: blue
//                    ; 4: green
//                    ; 5: red
//        test    ah, 8
//        jnz short loc_4D453
//
//waitForVsync:              ; CODE XREF: videoloop+7j
//                    ; videoloop+31j
//        sti
//        nop
//        nop
//        nop
//        nop
//        nop
//        nop
//        nop
//        nop
//        nop
//        nop
//        mov dx, 3DAh
//        cli
//        in  al, dx      ; Video status bits:
//                    ; 0: retrace.  1=display is in vert or horiz retrace.
//                    ; 1: 1=light pen is triggered; 0=armed
//                    ; 2: 1=light pen switch is open; 0=closed
//                    ; 3: 1=vertical sync pulse is occurring.
//        test    al, 1000b
//        jz  short waitForVsync
//        cmp byte ptr dword_59B67, 0
//        jz  short loc_4D453
//        mov dx, 3C0h
//        al = 33h ; '3'
//        out dx, al      ; EGA: horizontal pixel panning:
//                    ; Number of dots to shift data left.
//                    ; Bits 0-3 valid (0-0fH)
//        al = gNumberOfDotsToShiftDataLeft
//        out dx, al      ; EGA: palette register: select colors for attribute AL:
//                    ; 0: RED
//                    ; 1: GREEN
//                    ; 2: BLUE
//                    ; 3: blue
//                    ; 4: green
//                    ; 5: red
//
//loc_4D453:              ; CODE XREF: videoloop+1Dj
//                    ; videoloop+38j
//        sti
//        pop ax
//        pop dx
//        return;
// videoloop   endp
}

void loopForVSync() //   proc near       ; CODE XREF: crt?2+55p crt?1+41p ...
{
    // TODO: handle this properly to control FPS
    SDL_Delay(1000 / 60); // 60 fps
//        push    dx
//        push    ax
//
//loc_4D459:              ; CODE XREF: loopForVSync+8j
//        mov dx, 3DAh
//        in  al, dx      ; Video status bits:
//                    ; 0: retrace.  1=display is in vert or horiz retrace.
//                    ; 1: 1=light pen is triggered; 0=armed
//                    ; 2: 1=light pen switch is open; 0=closed
//                    ; 3: 1=vertical sync pulse is occurring.
//        test    al, 8
//        jnz short loc_4D459
//        pop ax
//        pop dx
//        return;

// loopForVSync   endp
}

void sub_4D464() //   proc near       ; CODE XREF: start+332p sub_4A463+3p
{
    // The comments added by IDA say something about split screen, so maybe
    // this is doing some work to show the bottom panel during a game.
    /*
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: graphics controller data register
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    inc dx
    al = 1
    out dx, al      ; EGA port: graphics controller data register
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: sequencer data register
    // mov dx, 3CEh
    // al = 8
    // out dx, al      ; EGA: graph 1 and 2 addr reg:
    //             ; bit mask
    //             ; Bits 0-7 select bits to be masked in all planes
    ports[0x3CE] = 8;
    inc dx
    al = 0FFh
    out dx, al      ; EGA port: graphics controller data register
    mov dx, 3D4h
    al = 13h
    out dx, al      ; Video: CRT cntrlr addr
                ; vertical displayed adjustment
    inc dx
    al = 3Dh ; '='
    out dx, al      ; Video: CRT controller internal registers
    cmp videoStatusUnk, 1
    jnz short loc_4D4CF
    mov dx, 3D4h
    al = 18h
    out dx, al      ; Video: CRT cntrlr addr
                ; line compare (scan line). Used for split screen operations.
    inc dx
    al = 5Fh ; '_'
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 7
    out dx, al      ; Video: CRT cntrlr addr
                ; bit 8 for certain CRTC regs. Data bits:
                ; 0: vertical total (Reg 06)
                ; 1: vert disp'd enable end (Reg 12H)
                ; 2: vert retrace start (Reg 10H)
                ; 3: start vert blanking (Reg 15H)
                ; 4: line compare (Reg 18H)
                ; 5: cursor location (Reg 0aH)
    inc dx
    al = 3Fh ; '?'
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 9
    out dx, al      ; Video: CRT cntrlr addr
                ; maximum scan line
    inc dx
    al = 80h ; '?'
    out dx, al      ; Video: CRT controller internal registers
     */
    videoloop();
    /*
    mov dx, 3DAh
    cli
    in  al, dx      ; Video status bits:
                ; 0: retrace.  1=display is in vert or horiz retrace.
                ; 1: 1=light pen is triggered; 0=armed
                ; 2: 1=light pen switch is open; 0=closed
                ; 3: 1=vertical sync pulse is occurring.
    mov dx, 3C0h
    al = 30h ; '0'
    out dx, al      ; EGA: mode control bits:
                ; 0: 1=graph modes, 0=text
                ; 1: 1=MDA
                ; 2: 1=9th dot=8th dot for line/box chars
                ;    0=use bkgd colr as 9th dot of char
                ; 3: 1=enable blink, 0=allow 4-bit bkgd
    al = 21h ; '!'
    out dx, al      ; EGA: palette register: select colors for attribute AL:
                ; 0: RED
                ; 1: GREEN
                ; 2: BLUE
                ; 3: blue
                ; 4: green
                ; 5: red
    sti
     */
    return;
/*
loc_4D4CF:              ; CODE XREF: sub_4D464+37j
    mov dx, 3D4h
    al = 18h
    out dx, al      ; Video: CRT cntrlr addr
                ; line compare (scan line). Used for split screen operations.
    inc dx
    al = 0B0h ; '?'
    out dx, al      ; Video: CRT controller internal registers
    mov dx, 3D4h
    al = 7
    out dx, al      ; Video: CRT cntrlr addr
                ; bit 8 for certain CRTC regs. Data bits:
                ; 0: vertical total (Reg 06)
                ; 1: vert disp'd enable end (Reg 12H)
                ; 2: vert retrace start (Reg 10H)
                ; 3: start vert blanking (Reg 15H)
                ; 4: line compare (Reg 18H)
                ; 5: cursor location (Reg 0aH)
    inc dx
    al = 1
    out dx, al      ; Video: CRT controller internal registers

locret_4D4E3:               ; CODE XREF: sub_4D464+69j
 */
    return;
}
/*
void initializeVideo3() //   proc near       ; CODE XREF: start+2B2p start+2C7p
{
        // mov dx, 3CEh
        // al = 1
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; enable set/reset
        ports[0x3CE] = 1; // enable set/reset
        // inc dx
        // al = 0Fh
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0x0F; // enable set/reset in all planes
        // mov dx, 3CEh
        // al = 5
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; mode register.Data bits:
        //             ; 0-1: Write mode 0-2
        //             ; 2: test condition
        //             ; 3: read mode: 1=color compare, 0=direct
        //             ; 4: 1=use odd/even RAM addressing
        //             ; 5: 1=use CGA mid-res map (2-bits/pixel)
        ports[0x3CE] = 5; // Select graphics mode
        // inc dx
        // al = 1
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0x1; // write mode 1: In write mode 1 the contents of the latch registers are first loaded by performing a read operation, then copied directly onto the color maps by performing a write operation. This mode is often used in moving areas of memory
        // mov dx, 3C4h
        // al = 2
        // out dx, al      ; EGA: sequencer address reg
        //             ; map mask: data bits 0-3 enable writes to bit planes 0-3
        ports[0x3C4] = 2; // map mask
        // inc dx
        // al = 0Fh
        // out dx, al      ; EGA port: sequencer data register
        ports[0x3C5] = 0x0F; // enable writing in all planes (rgb and intensity)
        // mov dx, 3CEh
        // al = 8
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; bit mask
        //             ; Bits 0-7 select bits to be masked in all planes
        ports[0x3CE] = 8; // bitmask
        // inc dx
        // al = 0FFh
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0xFF; // all bits protected from change
        // mov dx, 3D4h
        // al = 13h
        // out dx, al      ; Video: CRT cntrlr addr
        //             ; vertical displayed adjustment
        ports[0x3D4] = 0x13; // 19: offset register: Additional word offset to next logical line (page 155 or http://www.osdever.net/FreeVGA/vga/crtcreg.htm#13)
        // inc dx
        // al = 3Dh ; '='
        // out dx, al      ; Video: CRT controller internal registers
        //
        // This field specifies the address difference between consecutive scan lines or two lines of characters. Beginning with the second scan line, the starting scan line is increased by twice the value of this register multiplied by the current memory address size (byte = 1, word = 2, double-word = 4) each line. For text modes the following equation is used:
        //         Offset = Width / ( MemoryAddressSize * 2 )
        // and in graphics mode, the following equation is used:
        //          Offset = Width / ( PixelsPerAddress * MemoryAddressSize * 2 )
        // where Width is the width in pixels of the screen. This register can be modified to provide for a virtual resolution, in which case Width is the width is the width in pixels of the virtual screen. PixelsPerAddress is the number of pixels stored in one display memory address, and MemoryAddressSize is the current memory addressing size.
        //
        ports[0x3D5] = 0x3D; // 61: no idea what this is :joy: 
        // return;
// initializeVideo3   endp
}

void initializeVideo2() //   proc near       ; CODE XREF: start+2AFp
{
        // mov dx, 3CEh
        // mov al, 5
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; mode register.Data bits:
        //             ; 0-1: Write mode 0-2
        //             ; 2: test condition
        //             ; 3: read mode: 1=color compare, 0=direct
        //             ; 4: 1=use odd/even RAM addressing
        //             ; 5: 1=use CGA mid-res map (2-bits/pixel)
        ports[0x3CE] = 5; // Select graphics mode
        // inc dx
        // mov al, 0
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0; // write mode 0
        // mov dx, 3CEh
        // al = 0
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; set/reset.
        //             ; Data bits 0-3 select planes for write mode 00
        ports[0x3CE] = 0; // set/reset mode
        // inc dx
        // al = 0
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0; // clear all colors? 
        // mov dx, 3CEh
        // al = 1
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; enable set/reset
        ports[0x3CE] = 1; // enable set/reset
        // inc dx
        // al = 0Fh
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0x0F; // enable it in all planes
        // mov dx, 3CEh
        // al = 8
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; bit mask
        //             ; Bits 0-7 select bits to be masked in all planes
        ports[0x3CE] = 8; // bitmask
        // inc dx
        // al = 0FFh
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0xFF; // all bits protected from change
        cx = 0xFFFF;
        di = 0;
        
        memset(di, 0xFF, 0xFFFF);
        // mov di, 0
        // rep stosb // fill DI with 0xFFFF bytes with value 0xFF
        // return;
// initializeVideo2   endp
}
*/

void readLevels() //  proc near       ; CODE XREF: start:loc_46F3Ep
                    // ; sub_4A463p
{
    // 01ED:68E5
    char *filename = "";
    FILE *file = NULL;
    Level fileLevelData;

    if (byte_510DE != 0
        && (word_599D8 & 0xFF) == 0
        && byte_599D4 == 0)
    {
        ax = word_510E6;

    //    push    es
    //    push    ds
    //    push    ds
    //    pop es
    //    assume es:data
    //    mov si, seg demoseg
    //    mov ds, si
    //    assume ds:demoseg

        si = 0xBE20;
    //    di = offset fileLevelData;
        cx = ax;
        cx *= 2;
        ax += cx;
        cl = 9;
        ax = ax << cl;
        si += ax;
        cx = 0x300; // 768
    //    cld
        memcpy(di, si, 0x300 * 2);// rep movsw
        di -= 0x300 * 2;
        si -= 0x300 * 2;
        di = 0x87DA;
        ax = 0x532E;
    //    *di = *si; // stosw
        di--; si--;
        ax = 0x50; // 80
    //    *di = *si; // stosw
        di--; si--;
        si -= 0x5A; // 90
        cx = 0x17; // 23
        memcpy(di, si, 0x17 * 2);// rep movsw
        di -= 0x17 * 2;
        si -= 0x17 * 2;
    //    pop ds
    //    assume ds:data
    //    pop es
    //    assume es:nothing
    //    jmp loc_4D64B
    }
    else
    {
        if (byte_510DE == 0
            || byte_599D4 != 0)
        {
//loc_4D59F:              ; CODE XREF: readLevels+5j
//                ; readLevels+13j
            filename = aLevels_dat_0; // lea dx, aLevels_dat_0 ; "LEVELS.DAT"
        }

        if (byte_510DE != 0
            && (word_599D8 & 0xFF) != 0) //cmp byte ptr word_599D8, 0
        {
//loc_4D599:              ; CODE XREF: readLevels+Cj
            filename = "LEVELS.DAT"; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }
//loc_4D5A3:              ; CODE XREF: readLevels+55j
        else if (byte_599D4 != 0)
        {
//            filename = demoFileName; //    mov dx, offset demoFileName
        }
        else if (word_599DA != 0)
        {
            filename = "LEVELS.DAT"; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }

//loc_4D5BB:              ; CODE XREF: readLevels+63j
        file = fopen(filename, "r");
        if (file == NULL)
        {
            exitWithError("Error opening %s\n", filename);
        }

        uint8_t levelIndex = 0;

//loc_4D5C2:              ; CODE XREF: readLevels+75j
        if (byte_510DE != 0)
        {
            levelIndex = word_510E6;
        }
        else
        {
//loc_4D5D1:              ; CODE XREF: readLevels+82j
            levelIndex = gCurrentSelectedLevelIndex;
        }

//loc_4D5D4:              ; CODE XREF: readLevels+87j
        if (byte_599D4 != 0)
        {
            levelIndex = word_599DA;
            if (levelIndex == 0)
            {
                levelIndex++;
            }
        }

//loc_4D5E3:              ; CODE XREF: readLevels+91j
//                ; readLevels+98j
        levelIndex--; // Levels anywhere else are 1-index, we need them to start from 0 here
        size_t fileOffset = levelIndex * levelDataLength;
        // 01ED:699A
        int result = fseek(file, fileOffset, SEEK_SET);
        if (result != 0)
        {
            exitWithError("Error seeking %s\n", filename);
        }

//loc_4D604:              ; CODE XREF: readLevels+B7j
        // 01ED:69AE
        size_t bytes = fread(&fileLevelData, 1, levelDataLength, file);
        if (bytes < levelDataLength)
        {
            exitWithError("Error reading %s\n", filename);
        }

//loc_4D618:              ; CODE XREF: readLevels+CBj
        if ((word_599D8 & 0xFF) != 0) // cmp byte ptr word_599D8, 0
        {
            word_599D8 |= 0xFF00; // mov byte ptr word_599D8+1, 0FFh
        //    push    es
        //    mov ax, demoseg
        //    mov es, ax
        //    assume es:demoseg
        //    lea si, ds:[00768h]
        //    lea di, ds:[0BE20h]
            ax = word_599D6;
            word_510E6 = ax;
            cx = ax;
            cx = cx << 1;
            ax += cx;
            cl = 9;
            ax = ax << cl;
            di += ax;
            cx = 0x300; // 768
        //    cld
            memcpy(di, si, 0x300 * 2);// rep movsw
            di -= 0x300 * 2;
            si -= 0x300 * 2;
        //    pop es
        //    assume es:nothing
        }
    }

    char *levelName = "";

//loc_4D64B:              ; CODE XREF: readLevels+4Ej
//                ; readLevels+D5j
    if (byte_510DE != 0)
    {
        gTimeOfDay = word_51076;
        di = 0x87DA;
//      jmp short loc_4D660
    }
    else
    {
//loc_4D65D:              ; CODE XREF: readLevels+108j
        levelName = gCurrentLevelName;
    }

//loc_4D660:              ; CODE XREF: readLevels+113j
//    push    es
//    push    ds
//    pop es
//    assume es:data

//    cld
    if (word_599D8 != 0
        || (byte_599D4 != 0
            && word_599DA != 0))
    {
    //loc_4D679:              ; CODE XREF: readLevels+121j
        ax = 0x4942;
    //    *di = ax; // stosw
        ax = 0x4E; // 78
    //    *di = ax; // stosw
    }
    else if (byte_599D4 == 0)
    {
//loc_4D68C:              ; CODE XREF: readLevels+128j
        levelName += 4; // Skips the number directly to the title (from pointing "005 ------- EASY DEAL -------" to pointing "------- EASY DEAL -------")
    }
    else if (word_599DA == 0)
    {
//loc_4D682:              ; CODE XREF: readLevels+12Fj
        ax = 0x532E;
        //    *di = ax; // stosw
        ax = 0x50; // 80
        //    *di = ax; // stosw
    }

//loc_4D68F:              ; CODE XREF: readLevels+142j
//    si = 0x0D0E; // this is fileLevelData (buffer where data was read) + 1446 -> going to the title read from LEVELS.DAT
//    cx = 0x17;
//    memcpy(di, si, 0x17);// rep movsw // 01ED:6A32
//    di += 0x17;
//    si += 0x17;
    memcpy(filename, fileLevelData.name, sizeof(fileLevelData.name) - 5);

//    pop es
//    assume es:nothing
//    push    es
//    push    ds
//    pop es
//    assume es:data
//    si = offset fileLevelData;
//    di = offset levelBuffer; // 0x988B

//    cx = 0x300;
//    cld
//    memcpy(di, si, 0x300 * 2);// rep movsw // 01ED:6A42
//    di += 0x300 * 2;
//    si += 0x300 * 2;
    memcpy(&gCurrentLevel, &fileLevelData, sizeof(gCurrentLevel));
//    pop es
//    assume es:nothing
//    push    es
//    mov ax, ds
//    mov es, ax
//    assume es:data
    cx = levelDataLength;
//    si = offset fileLevelData;
//    di = offset leveldata; // 0x1834
    ah = 0;

    for (int i = 0; i < levelDataLength; ++i)
    {
//loc_4D6B8:              ; CODE XREF: readLevels+172j
        // This loads a byte, but then stores a word!!
        gCurrentLevelWord[i] = ((uint8_t *)&fileLevelData)[i];
    }
//    di = 0x2434; // this is leveldata (0x1834) + levelDataLength * 2... useless? when the loop finishes it should already have that value
//    al = 0;
//    cx = levelDataLength;
    memset(&gCurrentLevelAfterWord, 0, sizeof(gCurrentLevelAfterWord)); // rep stosb
//    di += levelDataLength;
//    pop es
//    assume es:nothing
    if (byte_510DE == 0
        || (word_599D8 & 0xFF) != 0
        || byte_599D4 != 0)
    {
//loc_4D6DC:              ; CODE XREF: readLevels+184j
//                ; readLevels+18Bj
        if (fclose(file) != 0)
        {
            exitWithError("Error closing %s\n", filename);
        }
    }

//loc_4D6EA:              ; CODE XREF: readLevels+192j
//                ; readLevels+19Dj
    word_599D8 &= 0xFF00; // mov byte ptr word_599D8, 0
}

void fadeToPalette(ColorPalette palette) //        proc near       ; CODE XREF: start+2C1p start+312p ...
{
    // Parameters:
    // si -> points to the first color of the palette to fade to

//    old_word_510A2 = word_510A2;
//    word_510A2 = 0;

    ColorPalette intermediatePalette;
    uint8_t totalSteps = 64;

    for (uint8_t step = 0; step < totalSteps; ++step)
    {
        uint8_t remainingSteps = totalSteps - step;

        for (uint8_t i = 0; i < kNumberOfColors; ++i)
        {
            uint8_t r = (palette[i].r * step / totalSteps) + (gCurrentPalette[i].r * remainingSteps / totalSteps);
            uint8_t g = (palette[i].g * step / totalSteps) + (gCurrentPalette[i].g * remainingSteps / totalSteps);
            uint8_t b = (palette[i].b * step / totalSteps) + (gCurrentPalette[i].b * remainingSteps / totalSteps);

            intermediatePalette[i] = (SDL_Color) { r, g, b, 255};
        }

        SDL_SetPaletteColors(gScreenSurface->format->palette, intermediatePalette, 0, kNumberOfColors);

        videoloop();
        loopForVSync();
    }

    setPalette(palette);
//        word_510A2 = old_word_510A2;
}

void setPalette(ColorPalette palette) //   proc near       ; CODE XREF: start+2B8p
                   // ; loadScreen2+B5p ...
{
//    int old_word_510A2; //       = word ptr -2

//    old_word_510A2 = word_510A2;
//    word_510A2 = 0;
    SDL_SetPaletteColors(gScreenSurface->format->palette, palette, 0, kNumberOfColors);
    memcpy(gCurrentPalette, palette, sizeof(ColorPalette));
//        word_510A2 = old_word_510A2;
}
/*
// ; ---------------------------------------------------------------------------
//        db  2Eh ; .
//        db  8Bh ; ?
//        db 0C0h ; +
//        db  2Eh ; .
//        db  8Bh ; ?
//        db 0C0h ; +
//        db  8Bh ; ?
//        db 0C0h ; +
//
//; =============== S U B R O U T I N E =======================================
*/
void initializeSound() //   proc near       ; CODE XREF: start+2A5p
{
//    push(ds)
//    ax = 0;
//    ds = ax;
//    // assume ds:nothing
//    bx = 0x200; // 512
//    [bx] = ax;
//    ax = seg soundseg; // 0x4C33
//    [bx + 2] = ax;
//    ax = 0;
//    bx = 0x204; // 516
//    [bx] = ax;
//    ax = seg sound2seg; // 0x4D92
//    [bx + 2] = ax;
//    pop(ds);
    // assume ds:data
    isFXEnabled = 0;
    soundEnabled = 0;
}

void soundShutdown() //  proc near       ; CODE XREF: start+48Ep
//                    ; loadScreen2-7DAp
{
    soundEnabled = 0;
    sound1();
}

 /*
// ; ---------------------------------------------------------------------------
        call    sound1
        mov musType, 0
        mov sndType, 0
        mov soundEnabled, 0
        return;

*/

void activateInternalStandardSound() // loadBeep   proc near       ; CODE XREF: readConfig:loc_4751Ap
//                    ; readConfig:loc_47551p ...
{
    sound1();
    readSound("BEEP.SND", 0x0AC4);
    musType = SoundTypeInternalStandard;
    sndType = SoundTypeInternalStandard;
    soundEnabled = 1;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void activateInternalSamplesSound() // loadBeep2  proc near       ; CODE XREF: readConfig+4Cp handleOptionsSamplesClickp
{
    sound1();
    readSound("BEEP.SND", 0x0AC4);
    readSound2("SAMPLE.SND", 0x8DAC);
    musType = SoundTypeInternalStandard;
    sndType = SoundTypeInternalSamples;
    soundEnabled = 1;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void activateAdlibSound() // loadAdlib  proc near       ; CODE XREF: readConfig+56p handleOptionsAdlibClickp
{
    sound1(); // 01ED:6D06
    readSound("ADLIB.SND", 0x14EA);
    musType = SoundTypeAdlib;
    sndType = SoundTypeAdlib;
    soundEnabled = 0;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void activateSoundBlasterSound() // loadBlaster  proc near       ; CODE XREF: readConfig+60p handleOptionsSoundBlasterClickp
{
    // 01ED:6D39
    sound1();
    readSound("ADLIB.SND", 0x14EA);
    readSound2("BLASTER.SND", 0x991B);
    musType = SoundTypeAdlib;
    sndType = SoundTypeSoundBlaster;
    soundEnabled = 0;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void activateRolandSound() // loadRoland  proc near       ; CODE XREF: readConfig+6Ap handleOptionsRolandClickp
{
    sound1();
    readSound("ROLAND.SND", 0x0F80);
    musType = SoundTypeRoland;
    sndType = SoundTypeRoland;
    soundEnabled = 0;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void activateCombinedSound() // loadCombined proc near       ; CODE XREF: readConfig+74p handleOptionsCombinedClickp
{
    sound1();
    readSound("ROLAND.SND", 0x0F80);
    readSound2("BLASTER.SND", 0x991B);
    musType = SoundTypeRoland;
    sndType = SoundTypeSoundBlaster;
    soundEnabled = 0;
    sound2();
    byte_59889 = 0;
    byte_5988A = 0x64;
    byte_5988B = 0;
    byte_5988C = 0;
}

void readSound(char *filename, size_t size) //   proc near       ; CODE XREF: activateInternalStandardSound+9p activateInternalSamplesSound+9p ...
{
    // 01ED:6DE4
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        exitWithError("Error opening %s\n", filename);
    }

//loc_4DA51:              ; CODE XREF: readSound+5j
//    mov lastFileHandle, ax
//    mov bx, lastFileHandle
//    push    ds
//    mov ax, seg soundseg
//    mov ds, ax
//    assume ds:soundseg
//    mov ax, 3F00h
//    mov dx, 0
//    int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to read
//                ; DS:DX -> buffer
    // It's saving it in soundseg:0000, which is 4C33:0000
    size_t bytes = fread(gSoundBuffer1, 1, size, file);
    if (bytes < size)
    {
        exitWithError("Error reading %s\n", filename);
    }

//loc_4DA6C:              ; CODE XREF: readSound+1Fj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing %s\n", filename);
    }
}

void readSound2(char *filename, size_t size) //  proc near       ; CODE XREF: activateInternalSamplesSound+12p
//                    ; activateSoundBlasterSound+12p ...
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        exitWithError("Error opening %s\n", filename);
    }

//loc_4DA86:              ; CODE XREF: readSound2+5j
//        mov lastFileHandle, ax
//        mov bx, lastFileHandle
//        push    ds
//        mov ax, seg sound2seg
//        mov ds, ax
//        assume ds:sound2seg
    // It's saving it in sound2seg:0000, which is 4D92:0000
    size_t bytes = fread(gSoundBuffer2, 1, size, file);
    if (bytes < size)
    {
        exitWithError("Error reading %s\n", filename);
    }

//loc_4DAA1:              ; CODE XREF: readSound2+1Fj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing %s\n", filename);
    }
}

void sound1() //     proc near       ; CODE XREF: soundShutdown?+5p
                 //   ; code:6CC7p ...
{
    // 01ED:6E4E
    soundEnabled = 0;
    if (musType == SoundTypeInternalStandard)
    {
//        mov ah, 2
//        int 80h     ; LINUX -
//        in  al, 61h     ; PC/XT PPI port B bits:
//                    ; 0: Tmr 2 gate ??? OR 03H=spkr ON
//                    ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
//                    ; 3: 1=read high switches
//                    ; 4: 0=enable RAM parity checking
//                    ; 5: 0=enable I/O channel check
//                    ; 6: 0=hold keyboard clock low
//                    ; 7: 0=enable kbrd
//        and al, 0FCh
//        out 61h, al     ; PC/XT PPI port B bits:
//                    ; 0: Tmr 2 gate ??? OR 03H=spkr ON
//                    ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
//                    ; 3: 1=read high switches
//                    ; 4: 0=enable RAM parity checking
//                    ; 5: 0=enable I/O channel check
//                    ; 6: 0=hold keyboard clock low
//                    ; 7: 0=enable kbrd
    }
//loc_4DAC9:              ; CODE XREF: sound1+Aj
    else if (musType == SoundTypeAdlib)
    {
//        mov dx, 388h
//        mov ah, 2
//        int 80h     ; LINUX -
    }
//loc_4DAD9:              ; CODE XREF: sound1+1Dj
    else if (musType == SoundTypeRoland)
    {
//    mov ah, 2
//    int 80h     ; LINUX -
//    mov ah, 10h
//    int 80h     ; LINUX -
    }

//loc_4DAE8:              ; CODE XREF: sound1+16j sound1+26j ...
    musType = SoundTypeNone;
    if (sndType == SoundTypeInternalSamples)
    {
//        mov ah, 1
//        int 81h
    }
//loc_4DAFA:              ; CODE XREF: sound1+41j
    else if (sndType == SoundTypeSoundBlaster)
    {
//        mov ah, 2
//        int 81h
    }

//loc_4DB05:              ; CODE XREF: sound1+47j sound1+4Ej
    sndType = SoundTypeNone;
}

void sound2() //     proc near       ; CODE XREF: start+39Bp start+410p ...
{
    // 01ED:6EA8
    if (isMusicEnabled != 1)
    {
        return;
    }

//loc_4DB13:              ; CODE XREF: sound2+5j
    if (musType == SoundTypeInternalStandard)
    {
//        mov ax, 0
//        int 80h     // ; LINUX - old_setup_syscall
        soundEnabled = 1;
        return;
    }

//loc_4DB26:              ; CODE XREF: sound2+Dj
    if (musType == SoundTypeAdlib)
    {
//        mov dx, 388h
//        mov ax, 0
//        int 80h     ; LINUX - old_setup_syscall
        soundEnabled = 1;
        return;
    }

//loc_4DB3C:              ; CODE XREF: sound2+20j
    if (musType == SoundTypeRoland)
    {
//        mov ax, 0
//        int 80h     ; LINUX - old_setup_syscall
        soundEnabled = 1;
    }
}

void sound3() //     proc near       ; CODE XREF: start+354p runLevel+41p ...
{
    if (musType == SoundTypeInternalStandard)
    {
    //    mov ah, 2
    //    int 80h     ; LINUX -
        return;
    }

//loc_4DB5B:              ; CODE XREF: sound3+5j
    if (musType == SoundTypeAdlib)
    {
    //    mov dx, 388h
    //    mov ah, 2
    //    int 80h     ; LINUX -
        return;
    }

//loc_4DB6B:              ; CODE XREF: sound3+12j
    if (musType == SoundTypeAdlib)
    {
//        mov ah, 2
//        int 80h     ; LINUX -
    }

//locret_4DB76:               ; CODE XREF: sound3+Bj sound3+1Bj ...
}

void sound4() //     proc near       ; CODE XREF: sub_4A61F+2EDp code:5ADEp ...
{
    if (isFXEnabled != 1)
    {
        return;
    }

//loc_4DB7F:              ; CODE XREF: sound4+5j
    if (byte_59889 >= 5)
    {
        return;
    }

//loc_4DB87:              ; CODE XREF: sound4+Dj
    byte_5988B = 0xF;
    byte_59889 = 5;
    if (sndType == SoundTypeInternalStandard)
    {
//        mov ax, 400h
//        int 80h     ; LINUX -
        return;
    }

//loc_4DB9F:              ; CODE XREF: sound4+1Fj
    if (sndType == SoundTypeInternalSamples)
    {
//        mov dx, 5D38h
//        mov ah, 3
//        int 81h
//        mov ax, 0
//        int 81h
        return;
    }

//loc_4DBB4:              ; CODE XREF: sound4+2Dj
    if (sndType == SoundTypeAdlib)
    {
//        mov ax, 400h
//        mov dx, 388h
//        int 80h     ; LINUX -
        return;
    }

//loc_4DBC5:              ; CODE XREF: sound4+42j
    if (sndType == SoundTypeSoundBlaster)
    {
//        mov ax, 0
//        int 81h
        return;
    }

//loc_4DBD3:              ; CODE XREF: sound4+53j
    if (sndType == SoundTypeRoland)
    {
//        mov ax, 400h
//        int 80h     ; LINUX -
        return;
    }
}

/*
sound5     proc near       ; CODE XREF: update?:loc_4E55Cp
                    ; update?:loc_4E588p ...
        cmp isFXEnabled, 1
        jz  short loc_4DBE8
        return;
// ; ---------------------------------------------------------------------------

loc_4DBE8:              ; CODE XREF: sound5+5j
        cmp byte_59889, 5
        jl  short loc_4DBF0
        return;
// ; ---------------------------------------------------------------------------

loc_4DBF0:              ; CODE XREF: sound5+Dj
        mov byte_5988B, 0Fh
        mov byte_59889, 4
        cmp sndType, 1
        jnz short loc_4DC08
        mov ax, 401h
        int 80h     ; LINUX -
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC08:              ; CODE XREF: sound5+1Fj
        cmp sndType, 2
        jnz short loc_4DC1D
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 1
        int 81h
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC1D:              ; CODE XREF: sound5+2Dj
        cmp sndType, 3
        jnz short loc_4DC2E
        mov ax, 401h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC2E:              ; CODE XREF: sound5+42j
        cmp sndType, 4
        jnz short loc_4DC3C
        mov ax, 1
        int 81h
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC3C:              ; CODE XREF: sound5+53j
        cmp sndType, 5
        jnz short locret_4DC48
        mov ax, 401h
        int 80h     ; LINUX -

locret_4DC48:               ; CODE XREF: sound5+26j sound5+3Bj ...
        return;
sound5     endp


; =============== S U B R O U T I N E =======================================


sound6     proc near       ; CODE XREF: update?+B8Bp
                    ; update?+136Cp
        cmp isFXEnabled, 1
        jz  short loc_4DC51
        return;
// ; ---------------------------------------------------------------------------

loc_4DC51:              ; CODE XREF: sound6+5j
        cmp byte_59889, 2
        jl  short loc_4DC59
        return;
// ; ---------------------------------------------------------------------------

loc_4DC59:              ; CODE XREF: sound6+Dj
        mov byte_5988B, 7
        mov byte_59889, 2
        cmp sndType, 1
        jnz short loc_4DC71
        mov ax, 402h
        int 80h     ; LINUX -
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC71:              ; CODE XREF: sound6+1Fj
        cmp sndType, 2
        jnz short loc_4DC86
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 2
        int 81h
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC86:              ; CODE XREF: sound6+2Dj
        cmp sndType, 3
        jnz short loc_4DC97
        mov ax, 402h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC97:              ; CODE XREF: sound6+42j
        cmp sndType, 4
        jnz short loc_4DCA5
        mov ax, 2
        int 81h
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DCA5:              ; CODE XREF: sound6+53j
        cmp sndType, 5
        jnz short locret_4DCB1
        mov ax, 402h
        int 80h     ; LINUX -

locret_4DCB1:               ; CODE XREF: sound6+26j sound6+3Bj ...
        return;
sound6     endp


; =============== S U B R O U T I N E =======================================


sound7     proc near       ; CODE XREF: movefun:loc_48125p
                    ; movefun2:loc_48573p
        cmp isFXEnabled, 1
        jz  short loc_4DCBA
        return;
// ; ---------------------------------------------------------------------------

loc_4DCBA:              ; CODE XREF: sound7+5j
        cmp byte_59889, 2
        jl  short loc_4DCC2
        return;
// ; ---------------------------------------------------------------------------

loc_4DCC2:              ; CODE XREF: sound7+Dj
        mov byte_5988B, 7
        mov byte_59889, 2
        cmp sndType, 1
        jnz short loc_4DCDA
        mov ax, 403h
        int 80h     ; LINUX -
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DCDA:              ; CODE XREF: sound7+1Fj
        cmp sndType, 2
        jnz short loc_4DCEF
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 3
        int 81h
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DCEF:              ; CODE XREF: sound7+2Dj
        cmp sndType, 3
        jnz short loc_4DD00
        mov ax, 403h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DD00:              ; CODE XREF: sound7+42j
        cmp sndType, 4
        jnz short loc_4DD0E
        mov ax, 3
        int 81h
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DD0E:              ; CODE XREF: sound7+53j
        cmp sndType, 5
        jnz short locret_4DD1A
        mov ax, 403h
        int 80h     ; LINUX -

locret_4DD1A:               ; CODE XREF: sound7+26j sound7+3Bj ...
        return;
sound7     endp


; =============== S U B R O U T I N E =======================================


sound8     proc near       ; CODE XREF: movefun7:loc_4A0ABp
        cmp isFXEnabled, 1
        jz  short loc_4DD23
        return;
// ; ---------------------------------------------------------------------------

loc_4DD23:              ; CODE XREF: sound8+5j
        cmp byte_59889, 3
        jl  short loc_4DD2B
        return;
// ; ---------------------------------------------------------------------------

loc_4DD2B:              ; CODE XREF: sound8+Dj
        mov byte_5988B, 3
        mov byte_59889, 3
        cmp sndType, 1
        jnz short loc_4DD43
        mov ax, 404h
        int 80h     ; LINUX -
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD43:              ; CODE XREF: sound8+1Fj
        cmp sndType, 2
        jnz short loc_4DD58
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 4
        int 81h
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD58:              ; CODE XREF: sound8+2Dj
        cmp sndType, 3
        jnz short loc_4DD69
        mov ax, 404h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD69:              ; CODE XREF: sound8+42j
        cmp sndType, 4
        jnz short loc_4DD77
        mov ax, 4
        int 81h
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD77:              ; CODE XREF: sound8+53j
        cmp sndType, 5
        jnz short locret_4DD83
        mov ax, 404h
        int 80h     ; LINUX -

locret_4DD83:               ; CODE XREF: sound8+26j sound8+3Bj ...
        return;
sound8     endp


; =============== S U B R O U T I N E =======================================


sound9     proc near       ; CODE XREF: runLevel+2F4p
                    ; update?:loc_4E3E1p ...
        cmp isFXEnabled, 1
        jz  short xxxxxxxxdcdc
        return;
// ; ---------------------------------------------------------------------------

xxxxxxxxdcdc:               ; CODE XREF: sound9+5j
        cmp byte_59889, 1
        jl  short loc_4DD94
        return;
// ; ---------------------------------------------------------------------------

loc_4DD94:              ; CODE XREF: sound9+Dj
        mov byte_5988B, 3
        mov byte_59889, 1
        cmp sndType, 1
        jnz short loc_4DDAC
        mov ax, 405h
        int 80h     ; LINUX -
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDAC:              ; CODE XREF: sound9+1Fj
        cmp sndType, 2
        jnz short loc_4DDC1
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 5
        int 81h
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDC1:              ; CODE XREF: sound9+2Dj
        cmp sndType, 3
        jnz short loc_4DDD2
        mov ax, 405h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDD2:              ; CODE XREF: sound9+42j
        cmp sndType, 4
        jnz short loc_4DDE0
        mov ax, 5
        int 81h
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDE0:              ; CODE XREF: sound9+53j
        cmp sndType, 5
        jnz short locret_4DDEC
        mov ax, 405h
        int 80h     ; LINUX -

locret_4DDEC:               ; CODE XREF: sound9+26j sound9+3Bj ...
        return;
sound9     endp


; =============== S U B R O U T I N E =======================================


sound10    proc near       ; CODE XREF: update?+7EBp
        cmp isFXEnabled, 1
        jz  short loc_4DDF5
        return;
// ; ---------------------------------------------------------------------------

loc_4DDF5:              ; CODE XREF: sound10+5j
        mov byte_5988B, 0FAh ; '?'
        mov byte_59889, 0Ah
        call    sound3
        cmp sndType, 1
        jnz short loc_4DE10
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE10:              ; CODE XREF: sound10+1Aj
        cmp sndType, 2
        jnz short loc_4DE25
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 6
        int 81h
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE25:              ; CODE XREF: sound10+28j
        cmp sndType, 3
        jnz short loc_4DE36
        mov ax, 1
        mov dx, 388h
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE36:              ; CODE XREF: sound10+3Dj
        cmp sndType, 4
        jnz short loc_4DE52
        cmp musType, 5
        jnz short loc_4DE4B
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE4B:              ; CODE XREF: sound10+55j
        mov ax, 6
        int 81h
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE52:              ; CODE XREF: sound10+4Ej
        cmp sndType, 5
        jnz short locret_4DE5E
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------

locret_4DE5E:               ; CODE XREF: sound10+21j sound10+36j ...
        return;
sound10    endp

*/

void sound11() //    proc near       ; CODE XREF: int8handler+51p
{
    if (musType == SoundTypeInternalStandard)
    {
//        mov ah, 1
//        int 80h     ; LINUX -
        return;
    }

//loc_4DE6C:              ; CODE XREF: sound11+5j
    if (musType == SoundTypeAdlib)
    {
//        mov dx, 388h
//        mov ah, 1
//        int 80h     ; LINUX -
        return;
    }

//loc_4DE7C:              ; CODE XREF: sound11+12j
    if (musType == SoundTypeRoland)
    {
//        mov ah, 1
//        int 80h     ; LINUX -
        return;
    }
}
/*
// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================


update?     proc near       ; CODE XREF: gameloop?+Ep

        cmp byte ptr leveldata[si], 3
        jz  short hasValidMurphy
        mov word_510CF, 0
        return;
// ; ---------------------------------------------------------------------------

hasValidMurphy:              ; CODE XREF: update?+5j
        mov word_510CF, 1
        mov word_510C7, si
        mov ax, leveldata[si]
        cmp ax, 3
        jz  short loc_4DEB4
        jmp loc_4EA07
// ; ---------------------------------------------------------------------------

loc_4DEB4:              ; CODE XREF: update?+1Fj
        mov byte_510D8, 0
        cmp byte_5101C, 0
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 0Ch
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 15h
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 17h
        jz  short loc_4DEE1
        cmp word ptr [si+18ACh], 0
        jnz short loc_4DEE1
        mov byte_510D8, 1

loc_4DEE1:              ; CODE XREF: update?+2Ej update?+35j ...
        mov bl, byte_50941
        cmp bl, 0
        jz  short loc_4DEED
        jmp loc_4E001
// ; ---------------------------------------------------------------------------

loc_4DEED:              ; CODE XREF: update?+58j
        mov byte_510D3, 1
        cmp byte_510D8, 0
        jz  short loc_4DEFC
        jmp loc_4E38A
// ; ---------------------------------------------------------------------------

loc_4DEFC:              ; CODE XREF: update?+67j
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4DF05
        return;
// ; ---------------------------------------------------------------------------

loc_4DF05:              ; CODE XREF: update?+72j
        inc word_510CD
        cmp word_510CD, 4
        jnz short loc_4DF1E
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4DF1E:              ; CODE XREF: update?+7Ej
        cmp word_510CD, 1F4h
        jg  short loc_4DF27
        return;
// ; ---------------------------------------------------------------------------

loc_4DF27:              ; CODE XREF: update?+94j
        cmp word_510CD, 20Ah
        jg  short loc_4DF4A
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 1F4h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4DF4A:              ; CODE XREF: update?+9Dj
        cmp word_510CD, 3E8h
        jg  short loc_4DF53
        return;
// ; ---------------------------------------------------------------------------

loc_4DF53:              ; CODE XREF: update?+C0j
        cmp word_510CD, 3FEh
        jg  short loc_4DF76
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 3E8h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4DF76:              ; CODE XREF: update?+C9j
        cmp word_510CD, 640h
        jg  short loc_4DF7F
        return;
// ; ---------------------------------------------------------------------------

loc_4DF7F:              ; CODE XREF: update?+ECj
        cmp word_510CD, 656h
        jg  short loc_4DFA2
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 640h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4DFA2:              ; CODE XREF: update?+F5j
        cmp word_510CD, 676h
        jg  short locret_4DFBE
        cmp word ptr [si+1832h], 0
        jnz short loc_4DFBF
        cmp word ptr [si+1836h], 0
        jnz short loc_4DFE0
        mov word_510CD, 24h ; '$'

locret_4DFBE:               ; CODE XREF: update?+118j
        return;
// ; ---------------------------------------------------------------------------

loc_4DFBF:              ; CODE XREF: update?+11Fj
        push    si
        mov di, [si+6155h]
        mov si, 150Eh
        mov bx, word_510CD
        sub bx, 656h
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4DFE0:              ; CODE XREF: update?+126j
        push    si
        mov di, [si+6155h]
        mov si, 1516h
        mov bx, word_510CD
        sub bx, 656h
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4E001:              ; CODE XREF: update?+5Aj
        cmp byte_510D8, 0
        jz  short loc_4E035
        cmp word ptr [si+18ACh], 0
        jnz short loc_4E035
        cmp bl, 1
        jnz short loc_4E01B
        cmp word ptr [si+17BCh], 2
        jz  short loc_4E035

loc_4E01B:              ; CODE XREF: update?+182j
        cmp bl, 2
        jnz short loc_4E027
        cmp word ptr [si+1832h], 2
        jz  short loc_4E035

loc_4E027:              ; CODE XREF: update?+18Ej
        cmp bl, 4
        jnz short loc_4E033
        cmp word ptr [si+1836h], 2
        jz  short loc_4E035

loc_4E033:              ; CODE XREF: update?+19Aj
        mov bl, 3

loc_4E035:              ; CODE XREF: update?+176j update?+17Dj ...
        cmp bl, 1
        jnz short loc_4E041
        mov byte_510D3, 0
        jmp short loc_4E0AA
// ; ---------------------------------------------------------------------------

loc_4E041:              ; CODE XREF: update?+1A8j
        cmp bl, 2
        jnz short loc_4E04E
        mov byte_510D3, 0
        jmp loc_4E10C
// ; ---------------------------------------------------------------------------

loc_4E04E:              ; CODE XREF: update?+1B4j
        cmp bl, 3
        jnz short loc_4E05B
        mov byte_510D3, 0
        jmp loc_4E186
// ; ---------------------------------------------------------------------------

loc_4E05B:              ; CODE XREF: update?+1C1j
        cmp bl, 4
        jnz short loc_4E068
        mov byte_510D3, 0
        jmp loc_4E1E8
// ; ---------------------------------------------------------------------------

loc_4E068:              ; CODE XREF: update?+1CEj
        cmp bl, 5
        jnz short loc_4E075
        mov byte_510D3, 0
        jmp loc_4E260
// ; ---------------------------------------------------------------------------

loc_4E075:              ; CODE XREF: update?+1DBj
        cmp bl, 6
        jnz short loc_4E082
        mov byte_510D3, 0
        jmp loc_4E28A
// ; ---------------------------------------------------------------------------

loc_4E082:              ; CODE XREF: update?+1E8j
        cmp bl, 7
        jnz short loc_4E08F
        mov byte_510D3, 0
        jmp loc_4E2BA
// ; ---------------------------------------------------------------------------

loc_4E08F:              ; CODE XREF: update?+1F5j
        cmp bl, 8
        jnz short loc_4E09C
        mov byte_510D3, 0
        jmp loc_4E2E4
// ; ---------------------------------------------------------------------------

loc_4E09C:              ; CODE XREF: update?+202j
        cmp bl, 9
        jnz short loc_4E0A4
        jmp loc_4E314
// ; ---------------------------------------------------------------------------

loc_4E0A4:              ; CODE XREF: update?+20Fj
        mov byte_510D3, 0
        return;
// ; ---------------------------------------------------------------------------

loc_4E0AA:              ; CODE XREF: update?+1AFj update?+279j
        mov ax, [si+17BCh]
        cmp ax, 0
        jnz short loc_4E0B6
        jmp loc_4E344
// ; ---------------------------------------------------------------------------

loc_4E0B6:              ; CODE XREF: update?+221j
        cmp ax, 2
        jnz short loc_4E0BE
        jmp loc_4E3E1
// ; ---------------------------------------------------------------------------

loc_4E0BE:              ; CODE XREF: update?+229j
        cmp al, 19h
        jnz short loc_4E0C5
        jmp loc_4E3D0
// ; ---------------------------------------------------------------------------

loc_4E0C5:              ; CODE XREF: update?+230j
        cmp ax, 4
        jnz short loc_4E0CD
        jmp loc_4E55C
// ; ---------------------------------------------------------------------------

loc_4E0CD:              ; CODE XREF: update?+238j
        cmp ax, 7
        jnz short loc_4E0D5
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E0D5:              ; CODE XREF: update?+240j
        cmp al, 13h
        jnz short loc_4E0DC
        jmp loc_4E712
// ; ---------------------------------------------------------------------------

loc_4E0DC:              ; CODE XREF: update?+247j
        cmp al, 0Ch
        jnz short loc_4E0E3
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0E3:              ; CODE XREF: update?+24Ej
        cmp al, 15h
        jnz short loc_4E0EA
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0EA:              ; CODE XREF: update?+255j
        cmp al, 17h
        jnz short loc_4E0F1
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0F1:              ; CODE XREF: update?+25Cj
        cmp al, 14h
        jnz short loc_4E0F8
        jmp loc_4E847
// ; ---------------------------------------------------------------------------

loc_4E0F8:              ; CODE XREF: update?+263j
        cmp al, 12h
        jnz short loc_4E0FF
        jmp loc_4E8F9
// ; ---------------------------------------------------------------------------

loc_4E0FF:              ; CODE XREF: update?+26Aj
        push    si
        sub si, 78h ; 'x'
        call    sub_4F21F
        pop si
        jb  short locret_4E10B
        jmp short loc_4E0AA
// ; ---------------------------------------------------------------------------

locret_4E10B:               ; CODE XREF: update?+277j
        return;
// ; ---------------------------------------------------------------------------

loc_4E10C:              ; CODE XREF: update?+1BBj update?+2F3j
        mov word_510CB, 1
        mov ax, [si+1832h]
        cmp ax, 0
        jnz short loc_4E11E
        jmp loc_4E36D
// ; ---------------------------------------------------------------------------

loc_4E11E:              ; CODE XREF: update?+289j
        cmp ax, 2
        jnz short loc_4E126
        jmp loc_4E41E
// ; ---------------------------------------------------------------------------

loc_4E126:              ; CODE XREF: update?+291j
        cmp al, 19h
        jnz short loc_4E12D
        jmp loc_4E40D
// ; ---------------------------------------------------------------------------

loc_4E12D:              ; CODE XREF: update?+298j
        cmp ax, 4
        jnz short loc_4E135
        jmp loc_4E588
// ; ---------------------------------------------------------------------------

loc_4E135:              ; CODE XREF: update?+2A0j
        cmp ax, 7
        jnz short loc_4E13D
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E13D:              ; CODE XREF: update?+2A8j
        cmp ax, 1
        jnz short loc_4E145
        jmp loc_4E6BA
// ; ---------------------------------------------------------------------------

loc_4E145:              ; CODE XREF: update?+2B0j
        cmp al, 13h
        jnz short loc_4E14C
        jmp loc_4E73C
// ; ---------------------------------------------------------------------------

loc_4E14C:              ; CODE XREF: update?+2B7j
        cmp al, 0Bh
        jnz short loc_4E153
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E153:              ; CODE XREF: update?+2BEj
        cmp al, 16h
        jnz short loc_4E15A
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E15A:              ; CODE XREF: update?+2C5j
        cmp al, 17h
        jnz short loc_4E161
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E161:              ; CODE XREF: update?+2CCj
        cmp ax, 14h
        jnz short loc_4E169
        jmp loc_4E863
// ; ---------------------------------------------------------------------------

loc_4E169:              ; CODE XREF: update?+2D4j
        cmp ax, 12h
        jnz short loc_4E171
        jmp loc_4E920
// ; ---------------------------------------------------------------------------

loc_4E171:              ; CODE XREF: update?+2DCj
        cmp ax, 8
        jnz short loc_4E179
        jmp loc_4E993
// ; ---------------------------------------------------------------------------

loc_4E179:              ; CODE XREF: update?+2E4j
        push    si
        sub si, 2
        call    sub_4F21F
        pop si
        jb  short locret_4E185
        jmp short loc_4E10C
// ; ---------------------------------------------------------------------------

locret_4E185:               ; CODE XREF: update?+2F1j
        return;
// ; ---------------------------------------------------------------------------

loc_4E186:              ; CODE XREF: update?+1C8j update?+355j
        mov ax, leveldata[si+78h]
        cmp ax, 0
        jnz short loc_4E192
        jmp loc_4E38A
// ; ---------------------------------------------------------------------------

loc_4E192:              ; CODE XREF: update?+2FDj
        cmp ax, 2
        jnz short loc_4E19A
        jmp loc_4E44F
// ; ---------------------------------------------------------------------------

loc_4E19A:              ; CODE XREF: update?+305j
        cmp al, 19h
        jnz short loc_4E1A1
        jmp loc_4E43E
// ; ---------------------------------------------------------------------------

loc_4E1A1:              ; CODE XREF: update?+30Cj
        cmp ax, 4
        jnz short loc_4E1A9
        jmp loc_4E5A8
// ; ---------------------------------------------------------------------------

loc_4E1A9:              ; CODE XREF: update?+314j
        cmp ax, 7
        jnz short loc_4E1B1
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E1B1:              ; CODE XREF: update?+31Cj
        cmp al, 13h
        jnz short loc_4E1B8
        jmp loc_4E766
// ; ---------------------------------------------------------------------------

loc_4E1B8:              ; CODE XREF: update?+323j
        cmp al, 0Ah
        jnz short loc_4E1BF
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1BF:              ; CODE XREF: update?+32Aj
        cmp al, 15h
        jnz short loc_4E1C6
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1C6:              ; CODE XREF: update?+331j
        cmp al, 17h
        jnz short loc_4E1CD
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1CD:              ; CODE XREF: update?+338j
        cmp al, 14h
        jnz short loc_4E1D4
        jmp loc_4E87F
// ; ---------------------------------------------------------------------------

loc_4E1D4:              ; CODE XREF: update?+33Fj
        cmp al, 12h
        jnz short loc_4E1DB
        jmp loc_4E947
// ; ---------------------------------------------------------------------------

loc_4E1DB:              ; CODE XREF: update?+346j
        push    si
        add si, 78h ; 'x'
        call    sub_4F21F
        pop si
        jb  short locret_4E1E7
        jmp short loc_4E186
// ; ---------------------------------------------------------------------------

locret_4E1E7:               ; CODE XREF: update?+353j
        return;
// ; ---------------------------------------------------------------------------

loc_4E1E8:              ; CODE XREF: update?+1D5j update?+3CDj
        mov word_510CB, 0
        mov ax, leveldata[si+2]
        cmp ax, 0
        jnz short loc_4E1FA
        jmp loc_4E3B3
// ; ---------------------------------------------------------------------------

loc_4E1FA:              ; CODE XREF: update?+365j
        cmp ax, 2
        jnz short loc_4E202
        jmp loc_4E48C
// ; ---------------------------------------------------------------------------

loc_4E202:              ; CODE XREF: update?+36Dj
        cmp al, 19h
        jnz short loc_4E209
        jmp loc_4E47B
// ; ---------------------------------------------------------------------------

loc_4E209:              ; CODE XREF: update?+374j
        cmp ax, 4
        jnz short loc_4E211
        jmp loc_4E5D4
// ; ---------------------------------------------------------------------------

loc_4E211:              ; CODE XREF: update?+37Cj
        cmp ax, 7
        jnz short loc_4E219
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E219:              ; CODE XREF: update?+384j
        cmp ax, 1
        jnz short loc_4E221
        jmp loc_4E6E1
// ; ---------------------------------------------------------------------------

loc_4E221:              ; CODE XREF: update?+38Cj
        cmp al, 13h
        jnz short loc_4E228
        jmp loc_4E790
// ; ---------------------------------------------------------------------------

loc_4E228:              ; CODE XREF: update?+393j
        cmp al, 9
        jnz short loc_4E22F
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E22F:              ; CODE XREF: update?+39Aj
        cmp al, 16h
        jnz short loc_4E236
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E236:              ; CODE XREF: update?+3A1j
        cmp al, 17h
        jnz short loc_4E23D
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E23D:              ; CODE XREF: update?+3A8j
        cmp al, 14h
        jnz short loc_4E244
        jmp loc_4E89A
// ; ---------------------------------------------------------------------------

loc_4E244:              ; CODE XREF: update?+3AFj
        cmp al, 12h
        jnz short loc_4E24B
        jmp loc_4E96D
// ; ---------------------------------------------------------------------------

loc_4E24B:              ; CODE XREF: update?+3B6j
        cmp ax, 8
        jnz short loc_4E253
        jmp loc_4E9B9
// ; ---------------------------------------------------------------------------

loc_4E253:              ; CODE XREF: update?+3BEj
        push    si
        add si, 2
        call    sub_4F21F
        pop si
        jb  short locret_4E25F
        jmp short loc_4E1E8
// ; ---------------------------------------------------------------------------

locret_4E25F:               ; CODE XREF: update?+3CBj
        return;
// ; ---------------------------------------------------------------------------

loc_4E260:              ; CODE XREF: update?+1E2j
        mov ax, leveldata[si-78h]
        cmp ax, 2
        jnz short loc_4E26C
        jmp loc_4E4BD
// ; ---------------------------------------------------------------------------

loc_4E26C:              ; CODE XREF: update?+3D7j
        cmp al, 19h
        jnz short loc_4E273
        jmp loc_4E4AC
// ; ---------------------------------------------------------------------------

loc_4E273:              ; CODE XREF: update?+3DEj
        cmp ax, 4
        jnz short loc_4E27B
        jmp loc_4E5F4
// ; ---------------------------------------------------------------------------

loc_4E27B:              ; CODE XREF: update?+3E6j
        cmp al, 13h
        jnz short loc_4E282
        jmp loc_4E712
// ; ---------------------------------------------------------------------------

loc_4E282:              ; CODE XREF: update?+3EDj
        cmp al, 14h
        jnz short locret_4E289
        jmp loc_4E8B6
// ; ---------------------------------------------------------------------------

locret_4E289:               ; CODE XREF: update?+3F4j
        return;
// ; ---------------------------------------------------------------------------

loc_4E28A:              ; CODE XREF: update?+1EFj
        mov word_510CB, 1
        mov ax, [si+1832h]
        cmp ax, 2
        jnz short loc_4E29C
        jmp loc_4E4E9
// ; ---------------------------------------------------------------------------

loc_4E29C:              ; CODE XREF: update?+407j
        cmp al, 19h
        jnz short loc_4E2A3
        jmp loc_4E4D8
// ; ---------------------------------------------------------------------------

loc_4E2A3:              ; CODE XREF: update?+40Ej
        cmp ax, 4
        jnz short loc_4E2AB
        jmp loc_4E614
// ; ---------------------------------------------------------------------------

loc_4E2AB:              ; CODE XREF: update?+416j
        cmp al, 13h
        jnz short loc_4E2B2
        jmp loc_4E73C
// ; ---------------------------------------------------------------------------

loc_4E2B2:              ; CODE XREF: update?+41Dj
        cmp al, 14h
        jnz short locret_4E2B9
        jmp loc_4E8C5
// ; ---------------------------------------------------------------------------

locret_4E2B9:               ; CODE XREF: update?+424j
        return;
// ; ---------------------------------------------------------------------------

loc_4E2BA:              ; CODE XREF: update?+1FCj
        mov ax, [si+18ACh]
        cmp ax, 2
        jnz short loc_4E2C6
        jmp loc_4E515
// ; ---------------------------------------------------------------------------

loc_4E2C6:              ; CODE XREF: update?+431j
        cmp al, 19h
        jnz short loc_4E2CD
        jmp loc_4E504
// ; ---------------------------------------------------------------------------

loc_4E2CD:              ; CODE XREF: update?+438j
        cmp ax, 4
        jnz short loc_4E2D5
        jmp loc_4E634
// ; ---------------------------------------------------------------------------

loc_4E2D5:              ; CODE XREF: update?+440j
        cmp al, 13h
        jnz short loc_4E2DC
        jmp loc_4E766
// ; ---------------------------------------------------------------------------

loc_4E2DC:              ; CODE XREF: update?+447j
        cmp al, 14h
        jnz short locret_4E2E3
        jmp loc_4E8D4
// ; ---------------------------------------------------------------------------

locret_4E2E3:               ; CODE XREF: update?+44Ej
        return;
// ; ---------------------------------------------------------------------------

loc_4E2E4:              ; CODE XREF: update?+209j
        mov word_510CB, 0
        mov ax, [si+1836h]
        cmp ax, 2
        jnz short loc_4E2F6
        jmp loc_4E541
// ; ---------------------------------------------------------------------------

loc_4E2F6:              ; CODE XREF: update?+461j
        cmp al, 19h
        jnz short loc_4E2FD
        jmp loc_4E530
// ; ---------------------------------------------------------------------------

loc_4E2FD:              ; CODE XREF: update?+468j
        cmp ax, 4
        jnz short loc_4E305
        jmp loc_4E654
// ; ---------------------------------------------------------------------------

loc_4E305:              ; CODE XREF: update?+470j
        cmp al, 13h
        jnz short loc_4E30C
        jmp loc_4E790
// ; ---------------------------------------------------------------------------

loc_4E30C:              ; CODE XREF: update?+477j
        cmp al, 14h
        jnz short locret_4E313
        jmp loc_4E8E3
// ; ---------------------------------------------------------------------------

locret_4E313:               ; CODE XREF: update?+47Ej
        return;
// ; ---------------------------------------------------------------------------

loc_4E314:              ; CODE XREF: update?+211j
        cmp byte_5195C, 0
        jz  short locret_4E343
        cmp byte_510DB, 0
        jnz short locret_4E343
        cmp byte_510D3, 1
        jnz short locret_4E343
        mov byte ptr [si+1835h], 2Ah ; '*'
        mov word_510EE, 40h ; '@'
        mov dx, 110Eh
        mov byte_510DB, 1
        mov word_510DC, si
        jmp loc_4E9F3
// ; ---------------------------------------------------------------------------

locret_4E343:               ; CODE XREF: update?+489j update?+490j ...
        return;
// ; ---------------------------------------------------------------------------

loc_4E344:              ; CODE XREF: update?+223j
        cmp word_510CB, 0
        jz  short loc_4E350
        mov dx, 0DFEh
        jmp short loc_4E353
// ; ---------------------------------------------------------------------------

loc_4E350:              ; CODE XREF: update?+4B9j
        mov dx, 0E0Eh

loc_4E353:              ; CODE XREF: update?+4BEj
        mov byte ptr [si+17BDh], 1
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E36D:              ; CODE XREF: update?+28Bj
        mov dx, 0E1Eh
        mov byte ptr [si+1833h], 2
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
        cmp word_510CB, 0
        jz  short loc_4E396
        mov dx, 0E2Eh
        jmp short loc_4E399
// ; ---------------------------------------------------------------------------

loc_4E396:              ; CODE XREF: update?+4FFj
        mov dx, 0E3Eh

loc_4E399:              ; CODE XREF: update?+504j
        mov byte ptr [si+18ADh], 3
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E3B3:              ; CODE XREF: update?+367j
        mov dx, 0E4Eh
        mov byte ptr [si+1837h], 4
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E3D0:              ; CODE XREF: update?+232j
        cmp byte ptr [si+17BDh], 0
        jl  short loc_4E3DB
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E3DB:              ; CODE XREF: update?+545j
        mov word ptr [si+17BCh], 2

loc_4E3E1:              ; CODE XREF: update?+22Bj
        call    sound9
        cmp word_510CB, 0
        jz  short loc_4E3F0
        mov dx, 0E6Eh
        jmp short loc_4E3F3
// ; ---------------------------------------------------------------------------

loc_4E3F0:              ; CODE XREF: update?+559j
        mov dx, 0E7Eh

loc_4E3F3:              ; CODE XREF: update?+55Ej
        mov byte ptr [si+17BDh], 5
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E40D:              ; CODE XREF: update?+29Aj
        cmp byte ptr [si+1833h], 0
        jl  short loc_4E418
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E418:              ; CODE XREF: update?+582j
        mov word ptr [si+1832h], 2

loc_4E41E:              ; CODE XREF: update?+293j
        call    sound9
        mov dx, 0E8Eh
        mov byte ptr [si+1833h], 2
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E43E:              ; CODE XREF: update?+30Ej
        cmp byte ptr [si+18ADh], 0
        jl  short loc_4E449
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E449:              ; CODE XREF: update?+5B3j
        mov word ptr [si+18ACh], 2

loc_4E44F:              ; CODE XREF: update?+307j
        call    sound9
        cmp word_510CB, 0
        jz  short loc_4E45E
        mov dx, 0E9Eh
        jmp short loc_4E461
// ; ---------------------------------------------------------------------------

loc_4E45E:              ; CODE XREF: update?+5C7j
        mov dx, 0EAEh

loc_4E461:              ; CODE XREF: update?+5CCj
        mov byte ptr [si+18ADh], 7
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E47B:              ; CODE XREF: update?+376j
        cmp byte ptr [si+1837h], 0
        jl  short loc_4E486
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E486:              ; CODE XREF: update?+5F0j
        mov word ptr [si+1836h], 2

loc_4E48C:              ; CODE XREF: update?+36Fj
        call    sound9
        mov dx, 0EBEh
        mov byte ptr [si+1837h], 8
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E4AC:              ; CODE XREF: update?+3E0j
        cmp byte ptr [si+17BDh], 0
        jl  short loc_4E4B7
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E4B7:              ; CODE XREF: update?+621j
        mov word ptr [si+17BCh], 2

loc_4E4BD:              ; CODE XREF: update?+3D9j
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        call    sound9
        mov dx, 0ECEh
        mov byte ptr [si+1835h], 10h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E4D8:              ; CODE XREF: update?+410j
        cmp byte ptr [si+1833h], 0
        jl  short loc_4E4E3
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E4E3:              ; CODE XREF: update?+64Dj
        mov word ptr [si+1832h], 2

loc_4E4E9:              ; CODE XREF: update?+409j
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        call    sound9
        mov dx, 0EDEh
        mov byte ptr [si+1835h], 11h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E504:              ; CODE XREF: update?+43Aj
        cmp byte ptr [si+18ADh], 0
        jl  short loc_4E50F
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E50F:              ; CODE XREF: update?+679j
        mov word ptr [si+18ACh], 2

loc_4E515:              ; CODE XREF: update?+433j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        call    sound9
        mov dx, 0EEEh
        mov byte ptr [si+1835h], 12h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E530:              ; CODE XREF: update?+46Aj
        cmp byte ptr [si+1837h], 0
        jl  short loc_4E53B
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4E53B:              ; CODE XREF: update?+6A5j
        mov word ptr [si+1836h], 2

loc_4E541:              ; CODE XREF: update?+463j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        call    sound9
        mov dx, 0EFEh
        mov byte ptr [si+1835h], 13h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E55C:              ; CODE XREF: update?+23Aj
        call    sound5
        cmp word_510CB, 0
        jz  short loc_4E56B
        mov dx, 0F0Eh
        jmp short loc_4E56E
// ; ---------------------------------------------------------------------------

loc_4E56B:              ; CODE XREF: update?+6D4j
        mov dx, 0F1Eh

loc_4E56E:              ; CODE XREF: update?+6D9j
        mov byte ptr [si+17BDh], 9
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E588:              ; CODE XREF: update?+2A2j
        call    sound5
        mov dx, 0F2Eh
        mov byte ptr [si+1833h], 0Ah
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5A8:              ; CODE XREF: update?+316j
        call    sound5
        cmp word_510CB, 0
        jz  short loc_4E5B7
        mov dx, 0F3Eh
        jmp short loc_4E5BA
// ; ---------------------------------------------------------------------------

loc_4E5B7:              ; CODE XREF: update?+720j
        mov dx, 0F4Eh

loc_4E5BA:              ; CODE XREF: update?+725j
        mov byte ptr [si+18ADh], 0Bh
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5D4:              ; CODE XREF: update?+37Ej
        call    sound5
        mov dx, 0F5Eh
        mov byte ptr [si+1837h], 0Ch
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5F4:              ; CODE XREF: update?+3E8j
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        call    sound5
        mov dx, 0F6Eh
        mov byte ptr [si+1835h], 14h
        mov byte ptr [si+17BDh], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E614:              ; CODE XREF: update?+418j
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        call    sound5
        mov dx, 0F7Eh
        mov byte ptr [si+1835h], 15h
        mov byte ptr [si+1833h], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E634:              ; CODE XREF: update?+442j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        call    sound5
        mov dx, 0F8Eh
        mov byte ptr [si+1835h], 16h
        mov byte ptr [si+18ADh], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E654:              ; CODE XREF: update?+472j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        call    sound5
        mov dx, 0F9Eh
        mov byte ptr [si+1835h], 17h
        mov byte ptr [si+1837h], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        cmp byte_5195A, 0
        jnz short locret_4E6B9
        call    sound10
        push    si
        mov byte_5A19B, 1
        mov byte_510BB, 1
        mov byte_510BA, 0
        cmp byte_5A2F9, 0
        jnz short loc_4E6A4
        cmp byte_510B3, 0
        jz  short loc_4E6A4
        mov byte_5A323, 1
        call    sub_4A95F

loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        call    changePlayerCurrentLevelState
        mov word_51978, 40h ; '@'
        pop si
        mov dx, 0E5Eh
        mov byte ptr [si+1835h], 0Dh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

locret_4E6B9:               ; CODE XREF: update?+7E9j
        return;
// ; ---------------------------------------------------------------------------

loc_4E6BA:              ; CODE XREF: update?+2B2j
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E6C4
        return;
// ; ---------------------------------------------------------------------------

loc_4E6C4:              ; CODE XREF: update?+831j
        mov byte ptr [si+1831h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 0FAEh
        mov byte ptr [si+1835h], 0Eh
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E6E1:              ; CODE XREF: update?+38Ej
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E6EB
        return;
// ; ---------------------------------------------------------------------------

loc_4E6EB:              ; CODE XREF: update?+858j
        mov ax, [si+18AEh]
        cmp ax, 0
        jnz short loc_4E6F5
        return;
// ; ---------------------------------------------------------------------------

loc_4E6F5:              ; CODE XREF: update?+862j
        mov byte ptr [si+1839h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 0FBEh
        mov byte ptr [si+1835h], 0Fh
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E712:              ; CODE XREF: update?+249j update?+3EFj
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E72D
        mov word_510CD, 0Ah
        return;
// ; ---------------------------------------------------------------------------

loc_4E72D:              ; CODE XREF: update?+894j
        push    si
        mov di, [si+60DDh]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E73C:              ; CODE XREF: update?+2B9j update?+41Fj
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E757
        mov word_510CD, 0Ah
        return;
// ; ---------------------------------------------------------------------------

loc_4E757:              ; CODE XREF: update?+8BEj
        push    si
        mov di, [si+6153h]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E766:              ; CODE XREF: update?+325j update?+449j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E781
        mov word_510CD, 0Ah
        return;
// ; ---------------------------------------------------------------------------

loc_4E781:              ; CODE XREF: update?+8E8j
        push    si
        mov di, [si+61CDh]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E790:              ; CODE XREF: update?+395j update?+479j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E7AB
        mov word_510CD, 0Ah
        return;
// ; ---------------------------------------------------------------------------

loc_4E7AB:              ; CODE XREF: update?+912j
        push    si
        mov di, [si+6157h]
        mov si, word_51848
        call    sub_4F200
        pop si

loc_4E7B8:              ; CODE XREF: update?+8AAj update?+8D4j ...
        mov byte_5196A, 7
        mov byte_5196B, 1
        push    si
        mov si, 0
        mov cx, 5A0h

loc_4E7C9:              ; CODE XREF: update?+94Aj
        cmp word ptr leveldata[si], 12h
        jnz short loc_4E7D7
        push(cx);
        push    si
        call    sub_4A61F
        pop si
        pop(cx);

loc_4E7D7:              ; CODE XREF: update?+93Ej
        add si, 2
        loop    loc_4E7C9
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4E7DE:              ; CODE XREF: update?+250j update?+257j ...
        cmp word ptr [si+1744h], 0
        jz  short loc_4E7E6
        return;
// ; ---------------------------------------------------------------------------

loc_4E7E6:              ; CODE XREF: update?+953j
        mov dx, 0FCEh
        mov byte ptr [si+1835h], 18h
        mov byte ptr [si+1745h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E7F5:              ; CODE XREF: update?+2C0j update?+2C7j ...
        cmp word ptr [si+1830h], 0
        jz  short loc_4E7FD
        return;
// ; ---------------------------------------------------------------------------

loc_4E7FD:              ; CODE XREF: update?+96Aj
        mov dx, 0FDEh
        mov byte ptr [si+1835h], 19h
        mov byte ptr [si+1831h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E80C:              ; CODE XREF: update?+32Cj update?+333j ...
        cmp word ptr [si+1924h], 0
        jz  short loc_4E814
        return;
// ; ---------------------------------------------------------------------------

loc_4E814:              ; CODE XREF: update?+981j
        mov dx, 0FEEh
        mov byte ptr [si+1835h], 1Ah
        mov byte ptr [si+1925h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E823:              ; CODE XREF: update?+39Cj update?+3A3j ...
        cmp word ptr [si+1838h], 0
        jz  short loc_4E82B
        return;
// ; ---------------------------------------------------------------------------

loc_4E82B:              ; CODE XREF: update?+998j
        mov dx, 0FFEh
        mov byte ptr [si+1835h], 1Bh
        mov byte ptr [si+1839h], 3

loc_4E838:              ; CODE XREF: update?+963j update?+97Aj ...
        mov word_510EE, 0
        mov word_510D9, 1
        jmp loc_4E9F3
// ; ---------------------------------------------------------------------------

loc_4E847:              ; CODE XREF: update?+265j
        cmp word_510CB, 0
        jz  short loc_4E853
        mov dx, 100Eh
        jmp short loc_4E856
// ; ---------------------------------------------------------------------------

loc_4E853:              ; CODE XREF: update?+9BCj
        mov dx, 101Eh

loc_4E856:              ; CODE XREF: update?+9C1j
        mov byte ptr [si+1835h], 1Ch
        mov byte ptr [si+17BDh], 3
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E863:              ; CODE XREF: update?+2D6j
        mov dx, 102Eh
        mov byte ptr [si+1833h], 1Dh
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E87F:              ; CODE XREF: update?+341j
        cmp word_510CB, 0
        jz  short loc_4E88B
        mov dx, 103Eh
        jmp short loc_4E88E
// ; ---------------------------------------------------------------------------

loc_4E88B:              ; CODE XREF: update?+9F4j
        mov dx, 104Eh

loc_4E88E:              ; CODE XREF: update?+9F9j
        mov byte ptr [si+1835h], 1Eh
        mov byte ptr [si+18ADh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E89A:              ; CODE XREF: update?+3B1j
        mov dx, 105Eh
        mov byte ptr [si+1837h], 1Fh
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8B6:              ; CODE XREF: update?+3F6j
        mov dx, 106Eh
        mov byte ptr [si+1835h], 20h ; ' '
        mov byte ptr [si+17BDh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8C5:              ; CODE XREF: update?+426j
        mov dx, 107Eh
        mov byte ptr [si+1835h], 21h ; '!'
        mov byte ptr [si+1833h], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8D4:              ; CODE XREF: update?+450j
        mov dx, 108Eh
        mov byte ptr [si+1835h], 22h ; '"'
        mov byte ptr [si+18ADh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8E3:              ; CODE XREF: update?+480j
        mov dx, 109Eh
        mov byte ptr [si+1835h], 23h ; '#'
        mov byte ptr [si+1837h], 3

loc_4E8F0:              ; CODE XREF: update?+4DAj update?+4F7j ...
        mov word_510EE, 0
        jmp loc_4E9ED
// ; ---------------------------------------------------------------------------

loc_4E8F9:              ; CODE XREF: update?+26Cj
        mov ax, [si+1744h]
        cmp ax, 0
        jz  short loc_4E903
        return;
// ; ---------------------------------------------------------------------------

loc_4E903:              ; CODE XREF: update?+A70j
        mov byte ptr [si+1745h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10AEh
        mov byte ptr [si+1835h], 24h ; '$'
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E920:              ; CODE XREF: update?+2DEj
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E92A
        return;
// ; ---------------------------------------------------------------------------

loc_4E92A:              ; CODE XREF: update?+A97j
        mov byte ptr [si+1831h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 10BEh
        mov byte ptr [si+1835h], 25h ; '%'
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E947:              ; CODE XREF: update?+348j
        mov ax, [si+1924h]
        cmp ax, 0
        jz  short loc_4E951
        return;
// ; ---------------------------------------------------------------------------

loc_4E951:              ; CODE XREF: update?+ABEj
        mov byte ptr [si+1925h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10CEh
        mov byte ptr [si+1835h], 27h ; '''
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E96D:              ; CODE XREF: update?+3B8j
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E977
        return;
// ; ---------------------------------------------------------------------------

loc_4E977:              ; CODE XREF: update?+AE4j
        mov byte ptr [si+1839h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10DEh
        mov byte ptr [si+1835h], 26h ; '&'
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E993:              ; CODE XREF: update?+2E6j
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E99D
        return;
// ; ---------------------------------------------------------------------------

loc_4E99D:              ; CODE XREF: update?+B0Aj
        mov byte ptr [si+1831h], 8
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 10EEh
        mov byte ptr [si+1835h], 28h ; '('
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E9B9:              ; CODE XREF: update?+3C0j
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E9C3
        return;
// ; ---------------------------------------------------------------------------

loc_4E9C3:              ; CODE XREF: update?+B30j
        mov ax, [si+18AEh]
        cmp ax, 0
        jnz short loc_4E9CD
        return;
// ; ---------------------------------------------------------------------------

loc_4E9CD:              ; CODE XREF: update?+B3Aj
        mov byte ptr [si+1839h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10FEh
        mov byte ptr [si+1835h], 29h ; ')'

loc_4E9E7:              ; CODE XREF: update?+84Ej update?+87Fj ...
        mov word_510EE, 8

loc_4E9ED:              ; CODE XREF: update?+A66j
        mov word_510D9, 0

loc_4E9F3:              ; CODE XREF: update?+4B0j update?+9B4j
        push    si
        push(di);
        push    es
        mov si, dx
        mov di, 0DE0h
        mov ax, ds
        mov es, ax
        assume es:data
        mov cx, 7
        rep movsw
        pop es
        assume es:nothing
        pop(di);
        pop si

loc_4EA07:              ; CODE XREF: update?+21j
        mov word_510CD, 0
        mov ax, word_510EE
        cmp ax, 0
        jz  short loc_4EA6B
        dec ax
        mov word_510EE, ax
        jnz short loc_4EA1E
        call    sound6

loc_4EA1E:              ; CODE XREF: update?+B89j
        mov bl, [si+1835h]
        cmp bl, 0Eh
        jnz short loc_4EA2A
        jmp loc_4ED49
// ; ---------------------------------------------------------------------------

loc_4EA2A:              ; CODE XREF: update?+B95j
        cmp bl, 0Fh
        jnz short loc_4EA32
        jmp loc_4ED81
// ; ---------------------------------------------------------------------------

loc_4EA32:              ; CODE XREF: update?+B9Dj
        cmp bl, 28h ; '('
        jnz short loc_4EA3A
        jmp loc_4EDB9
// ; ---------------------------------------------------------------------------

loc_4EA3A:              ; CODE XREF: update?+BA5j
        cmp bl, 29h ; ')'
        jnz short loc_4EA42
        jmp loc_4EDF1
// ; ---------------------------------------------------------------------------

loc_4EA42:              ; CODE XREF: update?+BADj
        cmp bl, 24h ; '$'
        jnz short loc_4EA4A
        jmp loc_4EE29
// ; ---------------------------------------------------------------------------

loc_4EA4A:              ; CODE XREF: update?+BB5j
        cmp bl, 25h ; '%'
        jnz short loc_4EA52
        jmp loc_4EE61
// ; ---------------------------------------------------------------------------

loc_4EA52:              ; CODE XREF: update?+BBDj
        cmp bl, 27h ; '''
        jnz short loc_4EA5A
        jmp loc_4EE99
// ; ---------------------------------------------------------------------------

loc_4EA5A:              ; CODE XREF: update?+BC5j
        cmp bl, 26h ; '&'
        jnz short loc_4EA62
        jmp loc_4EED1
// ; ---------------------------------------------------------------------------

loc_4EA62:              ; CODE XREF: update?+BCDj
        cmp bl, 2Ah ; '*'
        jnz short locret_4EA6A
        jmp loc_4EF09
// ; ---------------------------------------------------------------------------

locret_4EA6A:               ; CODE XREF: update?+BD5j
        return;
// ; ---------------------------------------------------------------------------

loc_4EA6B:              ; CODE XREF: update?+B83j
        push    si
        push(di);
        mov ax, word_510FA
        add word_510E8, ax
        mov ax, word_510FC
        add word_510EA, ax
        mov di, [si+6155h]
        add di, word_510F0
        mov si, word_510F8
        mov ax, [si]
        add si, 2
        mov word_510F8, si
        mov si, ax
        mov bx, word_510F4
        mov dx, word_510F6
        push    ds
        mov ax, es
        mov ds, ax

loc_4EA9F:              ; CODE XREF: update?+C28j
        mov cx, bx
        rep movsb
        add si, 7Ah ; 'z'
        sub si, bx
        add di, 7Ah ; 'z'
        sub di, bx
        cmp si, 4D34h
        jb  short loc_4EAB7
        sub si, 4D0Ch

loc_4EAB7:              ; CODE XREF: update?+C21j
        dec dx
        jnz short loc_4EA9F
        mov ax, ds
        pop ds
        cmp word_510D9, 0
        jz  short loc_4EAFA
        sub di, 7A0h
        add di, word_510F2
        mov si, word_510F8
        add si, 10h
        mov si, [si]
        mov dx, word_510F6
        push    ds
        mov ds, ax

loc_4EADC:              ; CODE XREF: update?+C65j
        mov cx, bx
        rep movsb
        add si, 7Ah ; 'z'
        sub si, bx
        add di, 7Ah ; 'z'
        sub di, bx
        cmp si, 4D34h
        jb  short loc_4EAF4
        sub si, 4D0Ch

loc_4EAF4:              ; CODE XREF: update?+C5Ej
        dec dx
        jnz short loc_4EADC
        pop ds
        jmp short loc_4EB04
// ; ---------------------------------------------------------------------------

loc_4EAFA:              ; CODE XREF: update?+C32j
        mov ax, word_510F0
        add ax, word_510F2
        mov word_510F0, ax

loc_4EB04:              ; CODE XREF: update?+C68j
        mov si, word_510F8
        cmp word ptr [si], 0FFFFh
        jz  short loc_4EB10
        pop(di);
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EB10:              ; CODE XREF: update?+C7Bj
        pop(di);
        pop si
        mov ax, word_510FA
        shr ax, 1
        mov bx, word_510FC
        shr bx, 1
        add word_510C3, ax
        add word_510C5, bx
        mov bl, [si+1835h]
        mov byte ptr [si+1835h], 0
        cmp bl, 1
        jnz short loc_4EB36
        jmp loc_4EC93
// ; ---------------------------------------------------------------------------

loc_4EB36:              ; CODE XREF: update?+CA1j
        cmp bl, 2
        jnz short loc_4EB3E
        jmp loc_4ECB1
// ; ---------------------------------------------------------------------------

loc_4EB3E:              ; CODE XREF: update?+CA9j
        cmp bl, 3
        jnz short loc_4EB46
        jmp loc_4ECCF
// ; ---------------------------------------------------------------------------

loc_4EB46:              ; CODE XREF: update?+CB1j
        cmp bl, 4
        jnz short loc_4EB4E
        jmp loc_4EF53
// ; ---------------------------------------------------------------------------

loc_4EB4E:              ; CODE XREF: update?+CB9j
        cmp bl, 5
        jnz short loc_4EB56
        jmp loc_4EC93
// ; ---------------------------------------------------------------------------

loc_4EB56:              ; CODE XREF: update?+CC1j
        cmp bl, 6
        jnz short loc_4EB5E
        jmp loc_4ECB1
// ; ---------------------------------------------------------------------------

loc_4EB5E:              ; CODE XREF: update?+CC9j
        cmp bl, 7
        jnz short loc_4EB66
        jmp loc_4ECCF
// ; ---------------------------------------------------------------------------

loc_4EB66:              ; CODE XREF: update?+CD1j
        cmp bl, 8
        jnz short loc_4EB6E
        jmp loc_4EF53
// ; ---------------------------------------------------------------------------

loc_4EB6E:              ; CODE XREF: update?+CD9j
        cmp bl, 9
        jnz short loc_4EB76
        jmp loc_4EC85
// ; ---------------------------------------------------------------------------

loc_4EB76:              ; CODE XREF: update?+CE1j
        cmp bl, 0Ah
        jnz short loc_4EB7E
        jmp loc_4ECA3
// ; ---------------------------------------------------------------------------

loc_4EB7E:              ; CODE XREF: update?+CE9j
        cmp bl, 0Bh
        jnz short loc_4EB86
        jmp loc_4ECC1
// ; ---------------------------------------------------------------------------

loc_4EB86:              ; CODE XREF: update?+CF1j
        cmp bl, 0Ch
        jnz short loc_4EB8E
        jmp loc_4EF45
// ; ---------------------------------------------------------------------------

loc_4EB8E:              ; CODE XREF: update?+CF9j
        cmp bl, 0Eh
        jnz short loc_4EB96
        jmp loc_4ECE3
// ; ---------------------------------------------------------------------------

loc_4EB96:              ; CODE XREF: update?+D01j
        cmp bl, 0Fh
        jnz short loc_4EB9E
        jmp loc_4ED06
// ; ---------------------------------------------------------------------------

loc_4EB9E:              ; CODE XREF: update?+D09j
        cmp bl, 10h
        jnz short loc_4EBA6
        jmp loc_4EF71
// ; ---------------------------------------------------------------------------

loc_4EBA6:              ; CODE XREF: update?+D11j
        cmp bl, 11h
        jnz short loc_4EBAE
        jmp loc_4EF8D
// ; ---------------------------------------------------------------------------

loc_4EBAE:              ; CODE XREF: update?+D19j
        cmp bl, 13h
        jnz short loc_4EBB6
        jmp loc_4EFC5
// ; ---------------------------------------------------------------------------

loc_4EBB6:              ; CODE XREF: update?+D21j
        cmp bl, 12h
        jnz short loc_4EBBE
        jmp loc_4EFA9
// ; ---------------------------------------------------------------------------

loc_4EBBE:              ; CODE XREF: update?+D29j
        cmp bl, 14h
        jnz short loc_4EBC6
        jmp loc_4EF63
// ; ---------------------------------------------------------------------------

loc_4EBC6:              ; CODE XREF: update?+D31j
        cmp bl, 15h
        jnz short loc_4EBCE
        jmp loc_4EF7F
// ; ---------------------------------------------------------------------------

loc_4EBCE:              ; CODE XREF: update?+D39j
        cmp bl, 17h
        jnz short loc_4EBD6
        jmp loc_4EFB7
// ; ---------------------------------------------------------------------------

loc_4EBD6:              ; CODE XREF: update?+D41j
        cmp bl, 16h
        jnz short loc_4EBDE
        jmp loc_4EF9B
// ; ---------------------------------------------------------------------------

loc_4EBDE:              ; CODE XREF: update?+D49j
        cmp bl, 0Dh
        jnz short loc_4EBE6
        jmp loc_4ED42
// ; ---------------------------------------------------------------------------

loc_4EBE6:              ; CODE XREF: update?+D51j
        cmp bl, 18h
        jnz short loc_4EBEE
        jmp loc_4EFD3
// ; ---------------------------------------------------------------------------

loc_4EBEE:              ; CODE XREF: update?+D59j
        cmp bl, 19h
        jnz short loc_4EBF6
        jmp loc_4F001
// ; ---------------------------------------------------------------------------

loc_4EBF6:              ; CODE XREF: update?+D61j
        cmp bl, 1Ah
        jnz short loc_4EBFE
        jmp loc_4F02E
// ; ---------------------------------------------------------------------------

loc_4EBFE:              ; CODE XREF: update?+D69j
        cmp bl, 1Bh
        jnz short loc_4EC06
        jmp loc_4F05C
// ; ---------------------------------------------------------------------------

loc_4EC06:              ; CODE XREF: update?+D71j
        cmp bl, 1Ch
        jnz short loc_4EC0E
        jmp loc_4F089
// ; ---------------------------------------------------------------------------

loc_4EC0E:              ; CODE XREF: update?+D79j
        cmp bl, 1Dh
        jnz short loc_4EC16
        jmp loc_4F09C
// ; ---------------------------------------------------------------------------

loc_4EC16:              ; CODE XREF: update?+D81j
        cmp bl, 1Eh
        jnz short loc_4EC1E
        jmp loc_4F0AC
// ; ---------------------------------------------------------------------------

loc_4EC1E:              ; CODE XREF: update?+D89j
        cmp bl, 1Fh
        jnz short loc_4EC26
        jmp loc_4F0BF
// ; ---------------------------------------------------------------------------

loc_4EC26:              ; CODE XREF: update?+D91j
        cmp bl, 20h ; ' '
        jnz short loc_4EC2E
        jmp loc_4F0CF
// ; ---------------------------------------------------------------------------

loc_4EC2E:              ; CODE XREF: update?+D99j
        cmp bl, 21h ; '!'
        jnz short loc_4EC36
        jmp loc_4F0E6
// ; ---------------------------------------------------------------------------

loc_4EC36:              ; CODE XREF: update?+DA1j
        cmp bl, 22h ; '"'
        jnz short loc_4EC3E
        jmp loc_4F0FD
// ; ---------------------------------------------------------------------------

loc_4EC3E:              ; CODE XREF: update?+DA9j
        cmp bl, 23h ; '#'
        jnz short loc_4EC46
        jmp loc_4F114
// ; ---------------------------------------------------------------------------

loc_4EC46:              ; CODE XREF: update?+DB1j
        cmp bl, 24h ; '$'
        jnz short loc_4EC4E
        jmp loc_4F12B
// ; ---------------------------------------------------------------------------

loc_4EC4E:              ; CODE XREF: update?+DB9j
        cmp bl, 25h ; '%'
        jnz short loc_4EC56
        jmp loc_4F148
// ; ---------------------------------------------------------------------------

loc_4EC56:              ; CODE XREF: update?+DC1j
        cmp bl, 27h //; '''
        jnz short loc_4EC5E
        jmp loc_4F165
// ; ---------------------------------------------------------------------------

loc_4EC5E:              ; CODE XREF: update?+DC9j
        cmp bl, 26h ; '&'
        jnz short loc_4EC66
        jmp loc_4F182
// ; ---------------------------------------------------------------------------

loc_4EC66:              ; CODE XREF: update?+DD1j
        cmp bl, 28h ; '('
        jnz short loc_4EC6E
        jmp loc_4F19F
// ; ---------------------------------------------------------------------------

loc_4EC6E:              ; CODE XREF: update?+DD9j
        cmp bl, 29h ; ')'
        jnz short loc_4EC76
        jmp loc_4F1BC
// ; ---------------------------------------------------------------------------

loc_4EC76:              ; CODE XREF: update?+DE1j
        cmp bl, 2Ah ; '*'
        jnz short loc_4EC7E
        jmp loc_4F1EA
// ; ---------------------------------------------------------------------------

loc_4EC7E:              ; CODE XREF: update?+DE9j
        mov word_51974, 1
        return;
// ; ---------------------------------------------------------------------------

loc_4EC85:              ; CODE XREF: update?+CE3j
        cmp byte_5195A, 0
        jbe short loc_4EC90
        dec byte_5195A

loc_4EC90:              ; CODE XREF: update?+DFAj
        call    sub_4FD21

loc_4EC93:              ; CODE XREF: update?+CA3j update?+CC3j
        mov word ptr leveldata[si], 3
        add si, 78h ; 'x'
        call    sub_487FE
        sub si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_4ECA3:              ; CODE XREF: update?+CEBj
        cmp byte_5195A, 0
        jbe short loc_4ECAE
        dec byte_5195A

loc_4ECAE:              ; CODE XREF: update?+E18j
        call    sub_4FD21

loc_4ECB1:              ; CODE XREF: update?+CABj update?+CCBj
        mov word ptr leveldata[si], 3
        add si, 2
        call    sub_487FE
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4ECC1:              ; CODE XREF: update?+CF3j
        cmp byte_5195A, 0
        jbe short loc_4ECCC
        dec byte_5195A

loc_4ECCC:              ; CODE XREF: update?+E36j
        call    sub_4FD21

loc_4ECCF:              ; CODE XREF: update?+CB3j update?+CD3j
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4ECDC
        mov word ptr [si+17BCh], 0

loc_4ECDC:              ; CODE XREF: update?+E44j
        mov word ptr leveldata[si], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4ECE3:              ; CODE XREF: update?+D03j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4ECF0
        mov word ptr leveldata[si], 0

loc_4ECF0:              ; CODE XREF: update?+E58j
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 1
        sub si, 4
        call    sub_4ED29
        add si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4ED06:              ; CODE XREF: update?+D0Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4ED13
        mov word ptr leveldata[si], 0

loc_4ED13:              ; CODE XREF: update?+E7Bj
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 1
        add si, 4
        call    sub_4ED29
        sub si, 2
        return;
update?     endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4ED29   proc near       ; CODE XREF: update?+E6Fp update?+E92p
        cmp byte ptr [si+18ACh], 11h
        jz  short loc_4ED38
        cmp byte ptr [si+18ACh], 0BBh ; '?'
        jz  short loc_4ED38
        return;
// ; ---------------------------------------------------------------------------

loc_4ED38:              ; CODE XREF: sub_4ED29+5j sub_4ED29+Cj
        add si, 78h ; 'x'
        call    sub_4A61F
        sub si, 78h ; 'x'
        return;
sub_4ED29   endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR update?

loc_4ED42:              ; CODE XREF: update?+D53j
        mov word_51974, 1
        return;
// ; ---------------------------------------------------------------------------

loc_4ED49:              ; CODE XREF: update?+B97j
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4ED5A
        cmp word ptr [si+1832h], 1
        jnz short loc_4ED5A
        return;
// ; ---------------------------------------------------------------------------

loc_4ED5A:              ; CODE XREF: update?+EC0j update?+EC7j
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 1
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4ED73
        mov word ptr [si+1830h], 0

loc_4ED73:              ; CODE XREF: update?+EDBj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4ED81:              ; CODE XREF: update?+B9Fj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4ED92
        cmp word ptr [si+1836h], 1
        jnz short loc_4ED92
        return;
// ; ---------------------------------------------------------------------------

loc_4ED92:              ; CODE XREF: update?+EF8j update?+EFFj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 1
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EDAB
        mov word ptr [si+1838h], 0

loc_4EDAB:              ; CODE XREF: update?+F13j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EDB9:              ; CODE XREF: update?+BA7j
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4EDCA
        cmp word ptr [si+1832h], 8
        jnz short loc_4EDCA
        return;
// ; ---------------------------------------------------------------------------

loc_4EDCA:              ; CODE XREF: update?+F30j update?+F37j
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 8
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4EDE3
        mov word ptr [si+1830h], 0

loc_4EDE3:              ; CODE XREF: update?+F4Bj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EDF1:              ; CODE XREF: update?+BAFj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4EE02
        cmp word ptr [si+1836h], 8
        jnz short loc_4EE02
        return;
// ; ---------------------------------------------------------------------------

loc_4EE02:              ; CODE XREF: update?+F68j update?+F6Fj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 8
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EE1B
        mov word ptr [si+1838h], 0

loc_4EE1B:              ; CODE XREF: update?+F83j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EE29:              ; CODE XREF: update?+BB7j
        mov bl, byte_50941
        cmp bl, 1
        jnz short loc_4EE3A
        cmp word ptr [si+17BCh], 12h
        jnz short loc_4EE3A
        return;
// ; ---------------------------------------------------------------------------

loc_4EE3A:              ; CODE XREF: update?+FA0j update?+FA7j
        mov word ptr leveldata[si], 3
        mov word ptr [si+17BCh], 12h
        cmp byte ptr [si+1744h], 1Fh
        jz  short loc_4EE53
        mov word ptr [si+1744h], 0

loc_4EE53:              ; CODE XREF: update?+FBBj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EE61:              ; CODE XREF: update?+BBFj
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4EE72
        cmp word ptr [si+1832h], 12h
        jnz short loc_4EE72
        return;
// ; ---------------------------------------------------------------------------

loc_4EE72:              ; CODE XREF: update?+FD8j update?+FDFj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 12h
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4EE8B
        mov word ptr [si+1830h], 0

loc_4EE8B:              ; CODE XREF: update?+FF3j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EE99:              ; CODE XREF: update?+BC7j
        mov bl, byte_50941
        cmp bl, 3
        jnz short loc_4EEAA
        cmp word ptr [si+18ACh], 12h
        jnz short loc_4EEAA
        return;
// ; ---------------------------------------------------------------------------

loc_4EEAA:              ; CODE XREF: update?+1010j
                    ; update?+1017j
        mov word ptr leveldata[si], 3
        mov word ptr [si+18ACh], 12h
        cmp byte ptr [si+1924h], 1Fh
        jz  short loc_4EEC3
        mov word ptr [si+1924h], 0

loc_4EEC3:              ; CODE XREF: update?+102Bj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EED1:              ; CODE XREF: update?+BCFj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4EEE2
        cmp word ptr [si+1836h], 12h
        jnz short loc_4EEE2
        return;
// ; ---------------------------------------------------------------------------

loc_4EEE2:              ; CODE XREF: update?+1048j
                    ; update?+104Fj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 12h
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EEFB
        mov word ptr [si+1838h], 0

loc_4EEFB:              ; CODE XREF: update?+1063j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EF09:              ; CODE XREF: update?+BD7j
        mov bl, byte_50941
        cmp bl, 9
        jnz short loc_4EF2C
        cmp word_510EE, 20h ; ' '
        jg  short locret_4EF2B
        push    si
        mov di, [si+6155h]
        mov si, word_51790
        call    sub_4F200
        mov byte_510DB, 1
        pop si

locret_4EF2B:               ; CODE XREF: update?+1087j
        return;
// ; ---------------------------------------------------------------------------

loc_4EF2C:              ; CODE XREF: update?+1080j
        push    si
        mov word ptr leveldata[si], 3
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        mov byte_510DB, 0
        pop si
        return;
// ; ---------------------------------------------------------------------------

loc_4EF45:              ; CODE XREF: update?+CFBj
        cmp byte_5195A, 0
        jbe short loc_4EF50
        dec byte_5195A

loc_4EF50:              ; CODE XREF: update?+10BAj
        call    sub_4FD21

loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
        sub si, 2
        call    sub_487FE
        add si, 2
        mov word ptr leveldata[si], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4EF63:              ; CODE XREF: update?+D33j
        cmp byte_5195A, 0
        jbe short loc_4EF6E
        dec byte_5195A

loc_4EF6E:              ; CODE XREF: update?+10D8j
        call    sub_4FD21

loc_4EF71:              ; CODE XREF: update?+D13j
        cmp byte ptr [si+17BCh], 1Fh
        jz  short locret_4EF7E
        mov word ptr [si+17BCh], 0

locret_4EF7E:               ; CODE XREF: update?+10E6j
        return;
// ; ---------------------------------------------------------------------------

loc_4EF7F:              ; CODE XREF: update?+D3Bj
        cmp byte_5195A, 0
        jbe short loc_4EF8A
        dec byte_5195A

loc_4EF8A:              ; CODE XREF: update?+10F4j
        call    sub_4FD21

loc_4EF8D:              ; CODE XREF: update?+D1Bj
        cmp byte ptr [si+1832h], 1Fh
        jz  short locret_4EF9A
        mov word ptr [si+1832h], 0

locret_4EF9A:               ; CODE XREF: update?+1102j
        return;
// ; ---------------------------------------------------------------------------

loc_4EF9B:              ; CODE XREF: update?+D4Bj
        cmp byte_5195A, 0
        jbe short loc_4EFA6
        dec byte_5195A

loc_4EFA6:              ; CODE XREF: update?+1110j
        call    sub_4FD21

loc_4EFA9:              ; CODE XREF: update?+D2Bj
        cmp byte ptr [si+18ACh], 1Fh
        jz  short locret_4EFB6
        mov word ptr [si+18ACh], 0

locret_4EFB6:               ; CODE XREF: update?+111Ej
        return;
// ; ---------------------------------------------------------------------------

loc_4EFB7:              ; CODE XREF: update?+D43j
        cmp byte_5195A, 0
        jbe short loc_4EFC2
        dec byte_5195A

loc_4EFC2:              ; CODE XREF: update?+112Cj
        call    sub_4FD21

loc_4EFC5:              ; CODE XREF: update?+D23j
        cmp byte ptr [si+1836h], 1Fh
        jz  short locret_4EFD2
        mov word ptr [si+1836h], 0

locret_4EFD2:               ; CODE XREF: update?+113Aj
        return;
// ; ---------------------------------------------------------------------------

loc_4EFD3:              ; CODE XREF: update?+D5Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4EFE0
        mov word ptr leveldata[si], 0

loc_4EFE0:              ; CODE XREF: update?+1148j
        mov word ptr [si+1744h], 3
        mov word_510D9, 0
        sub si, 0F0h ; '?'
        cmp byte ptr [si+18ADh], 1
        jnz short locret_4F000
        add si, 78h ; 'x'
        call    sub_4F2AF
        sub si, 78h ; 'x'

locret_4F000:               ; CODE XREF: update?+1165j
        return;
// ; ---------------------------------------------------------------------------

loc_4F001:              ; CODE XREF: update?+D63j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F00E
        mov word ptr leveldata[si], 0

loc_4F00E:              ; CODE XREF: update?+1176j
        mov word ptr [si+1830h], 3
        mov word_510D9, 0
        sub si, 4
        cmp byte ptr [si+1837h], 1
        jnz short locret_4F02D
        add si, 2
        call    sub_4F2AF
        sub si, 2

locret_4F02D:               ; CODE XREF: update?+1192j
        return;
// ; ---------------------------------------------------------------------------

loc_4F02E:              ; CODE XREF: update?+D6Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F03B
        mov word ptr leveldata[si], 0

loc_4F03B:              ; CODE XREF: update?+11A3j
        mov word ptr [si+1924h], 3
        mov word_510D9, 0
        add si, 0F0h ; '?'
        cmp byte ptr [si+17BDh], 1
        jnz short locret_4F05B
        sub si, 78h ; 'x'
        call    sub_4F2AF
        add si, 78h ; 'x'

locret_4F05B:               ; CODE XREF: update?+11C0j
        return;
// ; ---------------------------------------------------------------------------

loc_4F05C:              ; CODE XREF: update?+D73j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F069
        mov word ptr leveldata[si], 0

loc_4F069:              ; CODE XREF: update?+11D1j
        mov word ptr [si+1838h], 3
        mov word_510D9, 0
        add si, 4
        cmp byte ptr [si+1833h], 1
        jnz short locret_4F088
        sub si, 2
        call    sub_4F2AF
        add si, 2

locret_4F088:               ; CODE XREF: update?+11EDj
        return;
// ; ---------------------------------------------------------------------------

loc_4F089:              ; CODE XREF: update?+D7Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F096
        mov word ptr leveldata[si], 0

loc_4F096:              ; CODE XREF: update?+11FEj
        sub si, 78h ; 'x'
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F09C:              ; CODE XREF: update?+D83j
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F0A9
        mov word ptr [si+1836h], 0

loc_4F0A9:              ; CODE XREF: update?+1211j
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0AC:              ; CODE XREF: update?+D8Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F0B9
        mov word ptr leveldata[si], 0

loc_4F0B9:              ; CODE XREF: update?+1221j
        add si, 78h ; 'x'
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0BF:              ; CODE XREF: update?+D93j
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F0CC
        mov word ptr [si+1832h], 0

loc_4F0CC:              ; CODE XREF: update?+1234j
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0CF:              ; CODE XREF: update?+D9Bj
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4F0DC
        mov word ptr [si+17BCh], 0

loc_4F0DC:              ; CODE XREF: update?+1244j
        sub si, 78h ; 'x'
        call    sub_4FDB5
        add si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_4F0E6:              ; CODE XREF: update?+DA3j
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F0F3
        mov word ptr [si+1832h], 0

loc_4F0F3:              ; CODE XREF: update?+125Bj
        sub si, 2
        call    sub_4FDB5
        add si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F0FD:              ; CODE XREF: update?+DABj
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F10A
        mov word ptr [si+18ACh], 0

loc_4F10A:              ; CODE XREF: update?+1272j
        add si, 78h ; 'x'
        call    sub_4FDB5
        sub si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_4F114:              ; CODE XREF: update?+DB3j
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F121
        mov word ptr [si+1836h], 0

loc_4F121:              ; CODE XREF: update?+1289j
        add si, 2
        call    sub_4FDB5
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F12B:              ; CODE XREF: update?+DBBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F138
        mov word ptr leveldata[si], 0

loc_4F138:              ; CODE XREF: update?+12A0j
        mov word ptr [si+17BCh], 3
        mov word ptr [si+1744h], 12h
        sub si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_4F148:              ; CODE XREF: update?+DC3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F155
        mov word ptr leveldata[si], 0

loc_4F155:              ; CODE XREF: update?+12BDj
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 12h
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F165:              ; CODE XREF: update?+DCBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F172
        mov word ptr leveldata[si], 0

loc_4F172:              ; CODE XREF: update?+12DAj
        mov word ptr [si+18ACh], 3
        mov word ptr [si+1924h], 12h
        add si, 78h ; 'x'
        return;
// ; ---------------------------------------------------------------------------

loc_4F182:              ; CODE XREF: update?+DD3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F18F
        mov word ptr leveldata[si], 0

loc_4F18F:              ; CODE XREF: update?+12F7j
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 12h
        add si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F19F:              ; CODE XREF: update?+DDBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F1AC
        mov word ptr leveldata[si], 0

loc_4F1AC:              ; CODE XREF: update?+1314j
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 8
        sub si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F1BC:              ; CODE XREF: update?+DE3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F1C9
        mov word ptr leveldata[si], 0

loc_4F1C9:              ; CODE XREF: update?+1331j
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 8
        cmp word ptr [si+18B0h], 0
        jnz short loc_4F1E6
        mov byte ptr [si+1839h], 20h ; ' '
        mov byte ptr [si+18B1h], 8

loc_4F1E6:              ; CODE XREF: update?+134Aj
        add si, 2
        return;
// ; ---------------------------------------------------------------------------

loc_4F1EA:              ; CODE XREF: update?+DEBj
        mov word ptr leveldata[si], 3
        mov byte_510DB, 2
        dec byte_5195C
        call    sub_4FDCE
        call    sound6
        return;
; END OF FUNCTION CHUNK FOR update?
*/
void sub_4F200() //   proc near       ; CODE XREF: sub_4A291+26p
                   // ; sub_4A910+2Ap ...
{
    // Draws Murphy on the screen?
    // Parameters:
    // - di: coordinates on the screen
    // - si: coordinates on the MOVING.DAT bitmap to draw from?
    for (int i = 0; i < 16; ++i)
    {
//loc_4F208:              ; CODE XREF: sub_4F200+1Bj
//        *di = *si; di++; si++; // movsb
//        *di = *si; di++; si++; // movsb
        si += 0x78; // 120
        di += 0x78; // 120
        if (si >= 0x4D34)
        {
            si -= 0x4D0C;
        }
    }
}
/*
sub_4F21F   proc near       ; CODE XREF: update?+273p update?+2EDp ...
        cmp ax, 0FFFFh
        jz  short loc_4F296
        cmp ax, 0AAAAh
        jz  short loc_4F296
        cmp ah, 0
        jz  short loc_4F296
        cmp al, 1
        jz  short loc_4F24F
        cmp al, 1Fh
        jz  short loc_4F298
        cmp al, 8
        jz  short loc_4F296
        cmp al, 9
        jz  short loc_4F296
        cmp al, 0Ah
        jz  short loc_4F296
        cmp al, 0Bh
        jz  short loc_4F296
        cmp al, 0Ch
        jz  short loc_4F296
        call    sub_4A61F
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F24F:              ; CODE XREF: sub_4F21F+11j
        cmp bl, 2
        jz  short loc_4F25E
        cmp bl, 4
        jz  short loc_4F27A
        call    sub_4A61F
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F25E:              ; CODE XREF: sub_4F21F+33j
        and ah, 0F0h
        cmp ah, 20h ; ' '
        jz  short loc_4F278
        cmp ah, 40h ; '@'
        jz  short loc_4F278
        cmp ah, 50h ; 'P'
        jz  short loc_4F278
        cmp ah, 70h ; 'p'
        jz  short loc_4F278
        call    sub_4A61F

loc_4F278:              ; CODE XREF: sub_4F21F+45j
                    ; sub_4F21F+4Aj ...
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F27A:              ; CODE XREF: sub_4F21F+38j
        and ah, 0F0h
        cmp ah, 30h ; '0'
        jz  short loc_4F294
        cmp ah, 40h ; '@'
        jz  short loc_4F294
        cmp ah, 60h ; '`'
        jz  short loc_4F294
        cmp ah, 70h ; 'p'
        jz  short loc_4F294
        call    sub_4A61F

loc_4F294:              ; CODE XREF: sub_4F21F+61j
                    ; sub_4F21F+66j ...
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F296:              ; CODE XREF: sub_4F21F+3j sub_4F21F+8j ...
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F298:              ; CODE XREF: sub_4F21F+15j
        test    ah, 80h
        jnz short loc_4F2A2
        cmp ah, 4
        jge short loc_4F2A7

loc_4F2A2:              ; CODE XREF: sub_4F21F+7Cj
        call    sub_4A61F
        stc
        return;
// ; ---------------------------------------------------------------------------

loc_4F2A7:              ; CODE XREF: sub_4F21F+81j
        mov leveldata[si], 0
        clc
        return;
sub_4F21F   endp


; =============== S U B R O U T I N E =======================================


sub_4F2AF   proc near       ; CODE XREF: update?+116Ap
                    ; update?+1197p ...
        mov cl, byte_51037
        xor ch, ch
        cmp cx, 0
        jz  short locret_4F2CA
        mov di, 0D28h

loc_4F2BD:              ; CODE XREF: sub_4F2AF+19j
        mov bx, [di]
        xchg    bl, bh
        cmp bx, si
        jz  short loc_4F2CB
        add di, 6
        loop    loc_4F2BD

locret_4F2CA:               ; CODE XREF: sub_4F2AF+9j
        return;
// ; ---------------------------------------------------------------------------

loc_4F2CB:              ; CODE XREF: sub_4F2AF+14j
        al = [di+2]
        mov byte_5101C, al
        al = [di+3]
        mov byte_51035, al
        al = [di+4]
        mov byte_510D7, al
        mov ax, word_510AC
        xor ax, gTimeOfDay
        mov word_510AC, ax
        return;
sub_4F2AF   endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================


movefun4  proc near       ; DATA XREF: data:162Co
        cmp byte_510D7, 1
        jz  short notSnikSnak
        cmp byte ptr leveldata[si], sniksnak ; 17
        jnz short notSnikSnak
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        shl bx, 1
        mov ax, movingFunctions3[bx]
        cmp ax, 0FFFFh
        jz  short notSnikSnak
        call    ax

notSnikSnak:                ; CODE XREF: movefun4+5j
                    ; movefun4+Cj ...
        return;
movefun4  endp


; =============== S U B R O U T I N E =======================================


sub_4F312   proc near       ; DATA XREF: data:movingFunctions3o
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F320
        cmp ax, 3
        jz  short loc_4F362
        return;
// ; ---------------------------------------------------------------------------

loc_4F320:              ; CODE XREF: sub_4F312+6j
        push    ds
        push    si
        push    bx
        mov di, [si+6155h]
        mov si, [bx+1388h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F332:              ; CODE XREF: sub_4F312+28j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F332
        pop bx
        pop si
        pop ds
        shr bx, 1
        inc bx
        db  83h, 0E3h, 07h ;and bx, 7
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F34A:              ; CODE XREF: sub_4F312+79j
                    ; sub_4F312+9Cj ...
        cmp ah, 1Bh
        jz  short locret_4F361
        cmp ah, 19h
        jz  short locret_4F361
        cmp ah, 18h
        jz  short locret_4F361
        cmp ah, 1Ah
        jz  short locret_4F361
        call    sub_4A61F

locret_4F361:               ; CODE XREF: sub_4F312+3Bj
                    ; sub_4F312+40j ...
        return;
// ; ---------------------------------------------------------------------------

loc_4F362:              ; CODE XREF: sub_4F312+Bj
        mov bl, [si+1835h]
        cmp bl, 0
        jz  short loc_4F37B
        cmp bl, 2
        jz  short loc_4F39E
        cmp bl, 4
        jz  short loc_4F3C1
        cmp bl, 6
        jz  short loc_4F3E7
        return;
// ; ---------------------------------------------------------------------------

loc_4F37B:              ; CODE XREF: sub_4F312+57j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F38E
        mov ax, [si+17BCh]
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F34A
        return;
// ; ---------------------------------------------------------------------------

loc_4F38E:              ; CODE XREF: sub_4F312+6Ej
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F39E:              ; CODE XREF: sub_4F312+5Cj
        cmp word ptr [si+1832h], 0
        jz  short loc_4F3B1
        mov ax, [si+1832h]
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F34A
        return;
// ; ---------------------------------------------------------------------------

loc_4F3B1:              ; CODE XREF: sub_4F312+91j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        return;
// ; ---------------------------------------------------------------------------

loc_4F3C1:              ; CODE XREF: sub_4F312+61j
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F3D7
        mov ax, [si+18ACh]
        cmp byte ptr [si+18ACh], 3
        jnz short locret_4F3D6
        jmp loc_4F34A
// ; ---------------------------------------------------------------------------

locret_4F3D6:               ; CODE XREF: sub_4F312+BFj
        return;
// ; ---------------------------------------------------------------------------

loc_4F3D7:              ; CODE XREF: sub_4F312+B4j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F3E7:              ; CODE XREF: sub_4F312+66j
        cmp word ptr [si+1836h], 0
        jz  short loc_4F3FD
        mov ax, [si+1836h]
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F3FC
        jmp loc_4F34A
// ; ---------------------------------------------------------------------------

locret_4F3FC:               ; CODE XREF: sub_4F312+E5j
        return;
// ; ---------------------------------------------------------------------------

loc_4F3FD:              ; CODE XREF: sub_4F312+DAj
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        return;
sub_4F312   endp


; =============== S U B R O U T I N E =======================================


sub_4F40D   proc near       ; DATA XREF: data:155Ao
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F41B
        cmp ax, 3
        jz  short loc_4F45F
        return;
// ; ---------------------------------------------------------------------------

loc_4F41B:              ; CODE XREF: sub_4F40D+6j
        push    ds
        push    si
        mov di, [si+6155h]
        mov si, [bx+1388h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F42C:              ; CODE XREF: sub_4F40D+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F42C
        pop si
        pop ds
        shr bx, 1
        inc bl
        and bl, 7
        or  bl, 8
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F447:              ; CODE XREF: sub_4F40D+7Bj
                    ; sub_4F40D+9Ej ...
        cmp ah, 1Bh
        jz  short locret_4F45E
        cmp ah, 19h
        jz  short locret_4F45E
        cmp ah, 18h
        jz  short locret_4F45E
        cmp ah, 1Ah
        jz  short locret_4F45E
        call    sub_4A61F

locret_4F45E:               ; CODE XREF: sub_4F40D+3Dj
                    ; sub_4F40D+42j ...
        return;
// ; ---------------------------------------------------------------------------

loc_4F45F:              ; CODE XREF: sub_4F40D+Bj
        mov bl, [si+1835h]
        cmp bl, 8
        jz  short loc_4F478
        cmp bl, 0Ah
        jz  short loc_4F4E4
        cmp bl, 0Ch
        jz  short loc_4F4BE
        cmp bl, 0Eh
        jz  short loc_4F49B
        return;
// ; ---------------------------------------------------------------------------

loc_4F478:              ; CODE XREF: sub_4F40D+59j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F48B
        mov ax, [si+17BCh]
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F447
        return;
// ; ---------------------------------------------------------------------------

loc_4F48B:              ; CODE XREF: sub_4F40D+70j
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F49B:              ; CODE XREF: sub_4F40D+68j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F4AE
        mov ax, [si+1832h]
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F447
        return;
// ; ---------------------------------------------------------------------------

loc_4F4AE:              ; CODE XREF: sub_4F40D+93j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        return;
// ; ---------------------------------------------------------------------------

loc_4F4BE:              ; CODE XREF: sub_4F40D+63j
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F4D4
        mov ax, [si+18ACh]
        cmp byte ptr [si+18ACh], 3
        jnz short locret_4F4D3
        jmp loc_4F447
// ; ---------------------------------------------------------------------------

locret_4F4D3:               ; CODE XREF: sub_4F40D+C1j
        return;
// ; ---------------------------------------------------------------------------

loc_4F4D4:              ; CODE XREF: sub_4F40D+B6j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F4E4:              ; CODE XREF: sub_4F40D+5Ej
        cmp word ptr [si+1836h], 0
        jz  short loc_4F4FA
        mov ax, [si+1836h]
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F4F9
        jmp loc_4F447
// ; ---------------------------------------------------------------------------

locret_4F4F9:               ; CODE XREF: sub_4F40D+E7j
        return;
// ; ---------------------------------------------------------------------------

loc_4F4FA:              ; CODE XREF: sub_4F40D+DCj
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        return;
sub_4F40D   endp


; =============== S U B R O U T I N E =======================================


sub_4F50A   proc near       ; DATA XREF: data:156Ao
        push    si
        mov di, [si+61CDh]
        mov si, [bx+1388h]
        sub bx, 1Eh
        sub di, [bx+6C95h]
        sub di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F526:              ; CODE XREF: sub_4F50A+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F526
        pop ds
        pop si
        shr bx, 1
        cmp bl, 7
        jnz short loc_4F546
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F546
        mov word ptr [si+18ACh], 0

loc_4F546:              ; CODE XREF: sub_4F50A+2Dj
                    ; sub_4F50A+34j
        cmp bl, 8
        jge short loc_4F553
        add bl, 10h
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F553:              ; CODE XREF: sub_4F50A+3Fj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+1832h], 0
        jnz short loc_4F566
        mov byte ptr [si+1835h], 1
        return;
// ; ---------------------------------------------------------------------------

loc_4F566:              ; CODE XREF: sub_4F50A+54j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F573
        mov byte ptr [si+1835h], 1
        return;
// ; ---------------------------------------------------------------------------

loc_4F573:              ; CODE XREF: sub_4F50A+61j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F58A
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F58A:              ; CODE XREF: sub_4F50A+6Ej
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F595
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F595:              ; CODE XREF: sub_4F50A+85j
        cmp word ptr [si+1836h], 0
        jnz short loc_4F5A2
        mov byte ptr [si+1835h], 9
        return;
// ; ---------------------------------------------------------------------------

loc_4F5A2:              ; CODE XREF: sub_4F50A+90j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F5AF
        mov byte ptr [si+1835h], 9
        return;
// ; ---------------------------------------------------------------------------

loc_4F5AF:              ; CODE XREF: sub_4F50A+9Dj
        mov byte ptr [si+1835h], 1
        return;
sub_4F50A   endp


; =============== S U B R O U T I N E =======================================


sub_4F5B5   proc near       ; DATA XREF: data:157Ao
        push    si
        mov di, [si+6155h]
        mov si, [bx+1388h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F5C6:              ; CODE XREF: sub_4F5B5+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4F5C6
        pop ds
        pop si
        shr bx, 1
        db  83h, 0E3h, 07h ;and bx, 7
        inc bx
        cmp bl, 7
        jnz short loc_4F5EC
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F5EC
        mov word ptr [si+1836h], 0

loc_4F5EC:              ; CODE XREF: sub_4F5B5+28j
                    ; sub_4F5B5+2Fj
        cmp bl, 8
        jge short loc_4F5F9
        add bl, 18h
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F5F9:              ; CODE XREF: sub_4F5B5+3Aj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F60C
        mov byte ptr [si+1835h], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4F60C:              ; CODE XREF: sub_4F5B5+4Fj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F619
        mov byte ptr [si+1835h], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4F619:              ; CODE XREF: sub_4F5B5+5Cj
        cmp word ptr [si+1832h], 0
        jnz short loc_4F630
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        return;
// ; ---------------------------------------------------------------------------

loc_4F630:              ; CODE XREF: sub_4F5B5+69j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F63B
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F63B:              ; CODE XREF: sub_4F5B5+80j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F648
        mov byte ptr [si+1835h], 0Fh
        return;
// ; ---------------------------------------------------------------------------

loc_4F648:              ; CODE XREF: sub_4F5B5+8Bj
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F655
        mov byte ptr [si+1835h], 0Fh
        return;
// ; ---------------------------------------------------------------------------

loc_4F655:              ; CODE XREF: sub_4F5B5+98j
        mov byte ptr [si+1835h], 3
        return;
sub_4F5B5   endp


; =============== S U B R O U T I N E =======================================


sub_4F65B   proc near       ; DATA XREF: data:158Ao
        push    si
        mov di, [si+60DDh]
        mov si, [bx+1388h]
        sub bx, 40h ; '@'
        add di, [bx+6C95h]
sub_4F65B   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4F66B   proc near
        add di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F677:              ; CODE XREF: sub_4F66B+14j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F677
        pop ds
        pop si
        shr bx, 1
        inc bl
        cmp bl, 7
        jnz short loc_4F699
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4F699
        mov word ptr [si+17BCh], 0

loc_4F699:              ; CODE XREF: sub_4F66B+1Fj
                    ; sub_4F66B+26j
        cmp bl, 8
        jge short loc_4F6A6
        add bl, 20h ; ' '
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F6A6:              ; CODE XREF: sub_4F66B+31j
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+1836h], 0
        jnz short loc_4F6B9
        mov byte ptr [si+1835h], 5
        return;
// ; ---------------------------------------------------------------------------

loc_4F6B9:              ; CODE XREF: sub_4F66B+46j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F6C6
        mov byte ptr [si+1835h], 5
        return;
// ; ---------------------------------------------------------------------------

loc_4F6C6:              ; CODE XREF: sub_4F66B+53j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F6DD
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        return;
// ; ---------------------------------------------------------------------------

loc_4F6DD:              ; CODE XREF: sub_4F66B+60j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F6E8
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F6E8:              ; CODE XREF: sub_4F66B+77j
        cmp word ptr [si+1832h], 0
        jnz short loc_4F6F5
        mov byte ptr [si+1835h], 0Dh
        return;
// ; ---------------------------------------------------------------------------

loc_4F6F5:              ; CODE XREF: sub_4F66B+82j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F702
        mov byte ptr [si+1835h], 0Dh
        return;
// ; ---------------------------------------------------------------------------

loc_4F702:              ; CODE XREF: sub_4F66B+8Fj
        mov byte ptr [si+1835h], 5
        return;
sub_4F66B   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4F708   proc near       ; DATA XREF: data:159Ao
        push    si
        mov di, [si+6153h]
        mov si, [bx+1388h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F719:              ; CODE XREF: sub_4F708+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4F719
        pop ds
        pop si
        shr bx, 1
        and bl, 7
        inc bl
        cmp bl, 7
        jnz short loc_4F740
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F740
        mov word ptr [si+1832h], 0

loc_4F740:              ; CODE XREF: sub_4F708+29j
                    ; sub_4F708+30j
        cmp bl, 8
        jge short loc_4F74D
        add bl, 28h ; '('
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F74D:              ; CODE XREF: sub_4F708+3Bj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F760
        mov byte ptr [si+1835h], 7
        return;
// ; ---------------------------------------------------------------------------

loc_4F760:              ; CODE XREF: sub_4F708+50j
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F76D
        mov byte ptr [si+1835h], 7
        return;
// ; ---------------------------------------------------------------------------

loc_4F76D:              ; CODE XREF: sub_4F708+5Dj
        cmp word ptr [si+1836h], 0
        jnz short loc_4F784
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        return;
// ; ---------------------------------------------------------------------------

loc_4F784:              ; CODE XREF: sub_4F708+6Aj
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F78F
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F78F:              ; CODE XREF: sub_4F708+81j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F79C
        mov byte ptr [si+1835h], 0Bh
        return;
// ; ---------------------------------------------------------------------------

loc_4F79C:              ; CODE XREF: sub_4F708+8Cj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F7A9
        mov byte ptr [si+1835h], 0Bh
        return;
// ; ---------------------------------------------------------------------------

loc_4F7A9:              ; CODE XREF: sub_4F708+99j
        mov byte ptr [si+1835h], 7
        return;
sub_4F708   endp


; =============== S U B R O U T I N E =======================================


movefun6  proc near       ; DATA XREF: data:163Ao
        cmp byte_510D7, 1
        jz  short locret_4F7D0
        cmp byte ptr leveldata[si], 18h
        jnz short locret_4F7D0
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        shl bx, 1
        mov ax, movingFunctions2[bx]
        cmp ax, 0FFFFh
        jz  short locret_4F7D0
        call    ax

locret_4F7D0:               ; CODE XREF: movefun6+5j
                    ; movefun6+Cj ...
        return;
movefun6  endp


; =============== S U B R O U T I N E =======================================


sub_4F7D1   proc near       ; DATA XREF: data:movingFunctions2o
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F7DF
        cmp ax, 3
        jz  short loc_4F80D
        return;
// ; ---------------------------------------------------------------------------

loc_4F7DF:              ; CODE XREF: sub_4F7D1+6j
        push    ds
        push    si
        push    bx
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F7F1:              ; CODE XREF: sub_4F7D1+28j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F7F1
        pop bx
        pop si
        pop ds
        shr bx, 1
        inc bx
        db  83h, 0E3h, 07h ;and bx, 7
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F809:              ; CODE XREF: sub_4F7D1+61j
                    ; sub_4F7D1+80j ...
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F80D:              ; CODE XREF: sub_4F7D1+Bj
        mov bl, [si+1835h]
        cmp bl, 0
        jz  short loc_4F826
        cmp bl, 2
        jz  short loc_4F845
        cmp bl, 4
        jz  short loc_4F864
        cmp bl, 6
        jz  short loc_4F883
        return;
// ; ---------------------------------------------------------------------------

loc_4F826:              ; CODE XREF: sub_4F7D1+43j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F835
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F809
        return;
// ; ---------------------------------------------------------------------------

loc_4F835:              ; CODE XREF: sub_4F7D1+5Aj
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        return;
// ; ---------------------------------------------------------------------------

loc_4F845:              ; CODE XREF: sub_4F7D1+48j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F854
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F809
        return;
// ; ---------------------------------------------------------------------------

loc_4F854:              ; CODE XREF: sub_4F7D1+79j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        return;
// ; ---------------------------------------------------------------------------

loc_4F864:              ; CODE XREF: sub_4F7D1+4Dj
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F873
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4F809
        return;
// ; ---------------------------------------------------------------------------

loc_4F873:              ; CODE XREF: sub_4F7D1+98j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        return;
// ; ---------------------------------------------------------------------------

loc_4F883:              ; CODE XREF: sub_4F7D1+52j
        cmp word ptr [si+1836h], 0
        jz  short loc_4F895
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F894
        jmp loc_4F809
// ; ---------------------------------------------------------------------------

locret_4F894:               ; CODE XREF: sub_4F7D1+BEj
        return;
// ; ---------------------------------------------------------------------------

loc_4F895:              ; CODE XREF: sub_4F7D1+B7j
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        return;
sub_4F7D1   endp


; =============== S U B R O U T I N E =======================================


sub_4F8A5   proc near       ; DATA XREF: data:15BAo
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F8B3
        cmp ax, 3
        jz  short loc_4F8E3
        return;
// ; ---------------------------------------------------------------------------

loc_4F8B3:              ; CODE XREF: sub_4F8A5+6j
        push    ds
        push    si
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F8C4:              ; CODE XREF: sub_4F8A5+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F8C4
        pop si
        pop ds
        shr bx, 1
        inc bl
        and bl, 7
        or  bl, 8
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F8DF:              ; CODE XREF: sub_4F8A5+63j
                    ; sub_4F8A5+82j ...
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4F8E3:              ; CODE XREF: sub_4F8A5+Bj
        mov bl, [si+1835h]
        cmp bl, 8
        jz  short loc_4F8FC
        cmp bl, 0Ah
        jz  short loc_4F959
        cmp bl, 0Ch
        jz  short loc_4F93A
        cmp bl, 0Eh
        jz  short loc_4F91B
        return;
// ; ---------------------------------------------------------------------------

loc_4F8FC:              ; CODE XREF: sub_4F8A5+45j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F90B
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F8DF
        return;
// ; ---------------------------------------------------------------------------

loc_4F90B:              ; CODE XREF: sub_4F8A5+5Cj
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        return;
// ; ---------------------------------------------------------------------------

loc_4F91B:              ; CODE XREF: sub_4F8A5+54j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F92A
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F8DF
        return;
// ; ---------------------------------------------------------------------------

loc_4F92A:              ; CODE XREF: sub_4F8A5+7Bj
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        return;
// ; ---------------------------------------------------------------------------

loc_4F93A:              ; CODE XREF: sub_4F8A5+4Fj
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F949
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4F8DF
        return;
// ; ---------------------------------------------------------------------------

loc_4F949:              ; CODE XREF: sub_4F8A5+9Aj
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        return;
// ; ---------------------------------------------------------------------------

loc_4F959:              ; CODE XREF: sub_4F8A5+4Aj
        cmp word ptr [si+1836h], 0
        jz  short loc_4F96B
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F96A
        jmp loc_4F8DF
// ; ---------------------------------------------------------------------------

locret_4F96A:               ; CODE XREF: sub_4F8A5+C0j
        return;
// ; ---------------------------------------------------------------------------

loc_4F96B:              ; CODE XREF: sub_4F8A5+B9j
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        return;
sub_4F8A5   endp


; =============== S U B R O U T I N E =======================================


sub_4F97B   proc near       ; DATA XREF: data:15CAo
        push    si
        mov di, [si+61CDh]
        mov si, [bx+13E8h]
        sub bx, 1Eh
        sub di, [bx+6C95h]
        sub di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F997:              ; CODE XREF: sub_4F97B+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F997
        pop ds
        pop si
        shr bx, 1
        cmp bl, 7
        jnz short loc_4F9B7
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F9B7
        mov word ptr [si+18ACh], 0

loc_4F9B7:              ; CODE XREF: sub_4F97B+2Dj
                    ; sub_4F97B+34j
        cmp bl, 8
        jge short loc_4F9C4
        add bl, 10h
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4F9C4:              ; CODE XREF: sub_4F97B+3Fj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+1832h], 0
        jnz short loc_4F9D7
        mov byte ptr [si+1835h], 1
        return;
// ; ---------------------------------------------------------------------------

loc_4F9D7:              ; CODE XREF: sub_4F97B+54j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F9E4
        mov byte ptr [si+1835h], 1
        return;
// ; ---------------------------------------------------------------------------

loc_4F9E4:              ; CODE XREF: sub_4F97B+61j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F9FB
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        return;
// ; ---------------------------------------------------------------------------

loc_4F9FB:              ; CODE XREF: sub_4F97B+6Ej
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FA06
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4FA06:              ; CODE XREF: sub_4F97B+85j
        cmp word ptr [si+1836h], 0
        jnz short loc_4FA13
        mov byte ptr [si+1835h], 9
        return;
// ; ---------------------------------------------------------------------------

loc_4FA13:              ; CODE XREF: sub_4F97B+90j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FA20
        mov byte ptr [si+1835h], 9
        return;
// ; ---------------------------------------------------------------------------

loc_4FA20:              ; CODE XREF: sub_4F97B+9Dj
        mov byte ptr [si+1835h], 1
        return;
sub_4F97B   endp


; =============== S U B R O U T I N E =======================================


sub_4FA26   proc near       ; DATA XREF: data:15DAo
        push    si
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4FA37:              ; CODE XREF: sub_4FA26+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4FA37
        pop ds
        pop si
        shr bx, 1
        db  83h, 0E3h, 07h ;and bx, 7
        inc bx
        cmp bl, 7
        jnz short loc_4FA5D
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4FA5D
        mov word ptr [si+1836h], 0

loc_4FA5D:              ; CODE XREF: sub_4FA26+28j
                    ; sub_4FA26+2Fj
        cmp bl, 8
        jge short loc_4FA6A
        add bl, 18h
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4FA6A:              ; CODE XREF: sub_4FA26+3Aj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FA7D
        mov byte ptr [si+1835h], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4FA7D:              ; CODE XREF: sub_4FA26+4Fj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FA8A
        mov byte ptr [si+1835h], 3
        return;
// ; ---------------------------------------------------------------------------

loc_4FA8A:              ; CODE XREF: sub_4FA26+5Cj
        cmp word ptr [si+1832h], 0
        jnz short loc_4FAA1
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        return;
// ; ---------------------------------------------------------------------------

loc_4FAA1:              ; CODE XREF: sub_4FA26+69j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4FAAC
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4FAAC:              ; CODE XREF: sub_4FA26+80j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4FAB9
        mov byte ptr [si+1835h], 0Fh
        return;
// ; ---------------------------------------------------------------------------

loc_4FAB9:              ; CODE XREF: sub_4FA26+8Bj
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FAC6
        mov byte ptr [si+1835h], 0Fh
        return;
// ; ---------------------------------------------------------------------------

loc_4FAC6:              ; CODE XREF: sub_4FA26+98j
        mov byte ptr [si+1835h], 3
        return;
sub_4FA26   endp


; =============== S U B R O U T I N E =======================================


sub_4FACC   proc near       ; DATA XREF: data:15EAo
        push    si
        mov di, [si+60DDh]
        mov si, [bx+13E8h]
        sub bx, 40h ; '@'
        add di, [bx+6C95h]
        add di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4FAE8:              ; CODE XREF: sub_4FACC+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4FAE8
        pop ds
        pop si
        shr bx, 1
        inc bl
        cmp bl, 7
        jnz short loc_4FB0A
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4FB0A
        mov word ptr [si+17BCh], 0

loc_4FB0A:              ; CODE XREF: sub_4FACC+2Fj
                    ; sub_4FACC+36j
        cmp bl, 8
        jge short loc_4FB17
        add bl, 20h ; ' '
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4FB17:              ; CODE XREF: sub_4FACC+41j
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+1836h], 0
        jnz short loc_4FB2A
        mov byte ptr [si+1835h], 5
        return;
// ; ---------------------------------------------------------------------------

loc_4FB2A:              ; CODE XREF: sub_4FACC+56j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FB37
        mov byte ptr [si+1835h], 5
        return;
// ; ---------------------------------------------------------------------------

loc_4FB37:              ; CODE XREF: sub_4FACC+63j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FB4E
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        return;
// ; ---------------------------------------------------------------------------

loc_4FB4E:              ; CODE XREF: sub_4FACC+70j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FB59
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4FB59:              ; CODE XREF: sub_4FACC+87j
        cmp word ptr [si+1832h], 0
        jnz short loc_4FB66
        mov byte ptr [si+1835h], 0Dh
        return;
// ; ---------------------------------------------------------------------------

loc_4FB66:              ; CODE XREF: sub_4FACC+92j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4FB73
        mov byte ptr [si+1835h], 0Dh
        return;
// ; ---------------------------------------------------------------------------

loc_4FB73:              ; CODE XREF: sub_4FACC+9Fj
        mov byte ptr [si+1835h], 5
        return;
sub_4FACC   endp


; =============== S U B R O U T I N E =======================================


sub_4FB79   proc near       ; DATA XREF: data:15FAo
        push    si
        mov di, [si+6153h]
        mov si, [bx+13E8h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4FB8A:              ; CODE XREF: sub_4FB79+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4FB8A
        pop ds
        pop si
        shr bx, 1
        and bl, 7
        inc bl
        cmp bl, 7
        jnz short loc_4FBB1
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4FBB1
        mov word ptr [si+1832h], 0

loc_4FBB1:              ; CODE XREF: sub_4FB79+29j
                    ; sub_4FB79+30j
        cmp bl, 8
        jge short loc_4FBBE
        add bl, 28h ; '('
        mov [si+1835h], bl
        return;
// ; ---------------------------------------------------------------------------

loc_4FBBE:              ; CODE XREF: sub_4FB79+3Bj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+17BCh], 0
        jnz short loc_4FBD1
        mov byte ptr [si+1835h], 7
        return;
// ; ---------------------------------------------------------------------------

loc_4FBD1:              ; CODE XREF: sub_4FB79+50j
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FBDE
        mov byte ptr [si+1835h], 7
        return;
// ; ---------------------------------------------------------------------------

loc_4FBDE:              ; CODE XREF: sub_4FB79+5Dj
        cmp word ptr [si+1836h], 0
        jnz short loc_4FBF5
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        return;
// ; ---------------------------------------------------------------------------

loc_4FBF5:              ; CODE XREF: sub_4FB79+6Aj
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FC00
        call    sub_4A61F
        return;
// ; ---------------------------------------------------------------------------

loc_4FC00:              ; CODE XREF: sub_4FB79+81j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FC0D
        mov byte ptr [si+1835h], 0Bh
        return;
// ; ---------------------------------------------------------------------------

loc_4FC0D:              ; CODE XREF: sub_4FB79+8Cj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FC1A
        mov byte ptr [si+1835h], 0Bh
        return;
// ; ---------------------------------------------------------------------------

loc_4FC1A:              ; CODE XREF: sub_4FB79+99j
        mov byte ptr [si+1835h], 7
        return;
sub_4FB79   endp
*/

void drawGamePanelText() // sub_4FC20  proc near       ; CODE XREF: somethingspsig:loc_4944Fp
                   // ; sub_501C0+22p ...
{
    if (byte_510E3 != 0)
    {
        di = 0x177;
//loc_4FC36:              ; CODE XREF: drawGamePanelText+11j
//    mov si, 87D1h
//    mov ah, 8
        drawTextWithChars8Font(0, 0, 8, "");
        di = 0x6AE;
//loc_4FC4D:              ; CODE XREF: drawGamePanelText+28j
//    mov si, 87DAh
//    mov ah, 8
        drawTextWithChars8Font(0, 0, 8, "");
        di = 0x6B4;
//loc_4FC64:              ; CODE XREF: drawGamePanelText+3Fj
//        mov si, 87F6h
//        mov ah, 8
        drawTextWithChars8Font(0, 0, 8, "");
//        jmp loc_4FD1A
    }
    else
    {
//loc_4FC6F:              ; CODE XREF: drawGamePanelText+5j
        if (byte_510DE != 0)
        {
            di = 0x177;
//loc_4FC85:              ; CODE XREF: drawGamePanelText+60j
//            mov si, 87D1h
//            mov ah, 8
            drawTextWithChars8Font(0, 0, 8, "");
            di = 0x6AE;
//loc_4FC9C:              ; CODE XREF: drawGamePanelText+77j
//          mov si, 87DAh
//          mov ah, 8
            drawTextWithChars8Font(0, 0, 8, "");
            di = 0x6B4;
//loc_4FCB3:              ; CODE XREF: drawGamePanelText+8Ej
//          mov si, 87DEh
//          mov ah, 8
            drawTextWithChars8Font(0, 0, 8, "");
//        jmp short loc_4FD1A
        }
        else
        {

//loc_4FCC7:              ; CODE XREF: drawGamePanelText+54j
            di = 0x177;
//loc_4FCD6:              ; CODE XREF: drawGamePanelText+B1j
//          mov si, gPlayerName
//          mov ah, 6
            drawTextWithChars8Font(0, 0, 6, gPlayerName);
            di = 0x6AE;
//loc_4FCED:              ; CODE XREF: drawGamePanelText+C8j
//          mov si, 87A8h
//          si[3] = '\0'; // mov byte ptr [si+3], 0
//          mov ah, 8
            drawTextWithChars8Font(0, 0, 8, "");
            di = 0x6B4;

//loc_4FD08:              ; CODE XREF: drawGamePanelText+E3j
//          mov si, 87ACh
//          mov ah, 8
            drawTextWithChars8Font(0, 0, 8, "");
        }
    }

//loc_4FD1A:              ; CODE XREF: drawGamePanelText+4Cj
//                ; drawGamePanelText+A0j ...
    sub_4FD21();
    sub_4FDFD();
}

void sub_4FD21() //   proc near       ; CODE XREF: sub_4A3BB+13p
                   // ; update?:loc_4EC90p ...
{
    // Draws the level number?
//    push    si
    if (byte_5195A < 1)
    {
        byte_5195A = 0; // WTF?
    }

//loc_4FD2E:              ; CODE XREF: sub_4FD21+6j
//    al = byte_5195A;
//    mov si, 87C5h
    convertNumberTo3DigitStringWithPadding0(byte_5195A, "");
//    mov di, 6CEh

//loc_4FD46:              ; CODE XREF: sub_4FD21+20j
//    mov si, 87C5h
    if (byte_5195A != 0)
    {
//loc_4FD54:              ; CODE XREF: sub_4FD21+2Dj
//        mov ah, 8
    }
    else
    {
//        mov ah, 6
    }

//loc_4FD56:              ; CODE XREF: sub_4FD21+31j
    drawTextWithChars8Font(0, 0, ah, "");

//loc_4FD63:              ; CODE XREF: sub_4FD21+3Dj
}
/*
sub_4FD65   proc near       ; CODE XREF: runLevel+E9p
        cmp videoStatusUnk, 1
        jz  short loc_4FD6D
        return;
// ; ---------------------------------------------------------------------------

loc_4FD6D:              ; CODE XREF: sub_4FD65+5j
        al = byte_5197C
        cmp al, 0
        jnz short loc_4FD75
        return;
// ; ---------------------------------------------------------------------------

loc_4FD75:              ; CODE XREF: sub_4FD65+Dj
        dec al
        jz  short loc_4FD7D
        mov byte_5197C, al
        return;
// ; ---------------------------------------------------------------------------

loc_4FD7D:              ; CODE XREF: sub_4FD65+12j
        mov byte_5197C, al
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        push    ds
        mov si, word_5182E
        mov di, 6D2h
        mov ax, es
        mov ds, ax
        mov cx, 7

loc_4FD99:              ; CODE XREF: sub_4FD65+3Cj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4FD99
        pop ds
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        // inc dx
        // al = 0
        // out dx, al      ; EGA port: graphics controller data register
        ports[0x3CF] = 0;
        return;
sub_4FD65   endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR update?

loc_4FDAF:              ; CODE XREF: update?+1209j
                    ; update?:loc_4F0A9j ...
        mov word ptr leveldata[si], 3
; END OF FUNCTION CHUNK FOR update?

; =============== S U B R O U T I N E =======================================


sub_4FDB5   proc near       ; CODE XREF: update?+124Fp
                    ; update?+1266p ...
        cmp byte ptr word_59B73, 0
        jnz short loc_4FDCA
        cmp byte_510DB, 0
        jz  short loc_4FDCA
        cmp word_510DC, si
        jnz short loc_4FDCA
        return;
// ; ---------------------------------------------------------------------------

loc_4FDCA:              ; CODE XREF: sub_4FDB5+5j sub_4FDB5+Cj ...
        inc byte_5195C
sub_4FDB5   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4FDCE   proc near       ; CODE XREF: sub_4955B+7F4p
                    ; update?+1369p
        cmp videoStatusUnk, 1
        jz  short loc_4FDD6
        return;
// ; ---------------------------------------------------------------------------

loc_4FDD6:              ; CODE XREF: sub_4FDCE+5j
        push    si
        al = byte_5195C
        mov si, 87C9h
        call    convertNumberTo3DigitStringWithPadding0
        mov di, 6D2h
        mov si, 87CAh
        cmp byte_5195C, 0
        jnz short loc_4FDF1
        mov ah, 8
        jmp short loc_4FDF3
// ; ---------------------------------------------------------------------------

loc_4FDF1:              ; CODE XREF: sub_4FDCE+1Dj
        mov ah, 6

loc_4FDF3:              ; CODE XREF: sub_4FDCE+21j
        call    drawTextWithChars8Font
        pop si
        mov byte_5197C, 46h ; 'F'
        return;
sub_4FDCE   endp
*/
void sub_4FDFD() //   proc near       ; CODE XREF: runLevel+29p
                   // ; runLevel:noFlashing2p ...
{
    uint16_t var_2; //       = word ptr -2

//    push    bp
//    mov bp, sp
//    add sp, 0FFFEh
    var_2 = 0;
    al = byte_510B0;
    if ((word_510B7 & 0xFF) != al) // byte
    {
        word_510B7 = (word_510B7 & 0xFF00) + al; // byte
    //    mov si, 87CDh
        convertNumberTo3DigitStringWithPadding0(al, "");
    //    mov si, 87CEh
    //    mov di, 188h
//loc_4FE2C:              ; CODE XREF: sub_4FDFD+2Aj
    //    mov ah, 6
        drawTextWithChars8Font(0, 0, 6, "");
        var_2 = 1;
    }

//loc_4FE36:              ; CODE XREF: sub_4FDFD+12j
    al = byte_510B1;
    if ((word_510B7 >> 8) == al) // byte
    {
        //jz  short loc_4FE5F
    }
    word_510B7 = (al << 8) + (word_510B7 & 0x00FF); // byte
//    mov si, 87CDh
    convertNumberTo3DigitStringWithPadding0(al, "");
//    mov si, 87CEh
//    mov di, 185h

//loc_4FE5A:              ; CODE XREF: sub_4FDFD+58j
//    mov ah, 6
    drawTextWithChars8Font(0, 0, 6, "");

//loc_4FE5F:              ; CODE XREF: sub_4FDFD+40j
    al = byte_510B2;
    if (byte_510B9 == byte_510B2)
    {
        //jz  short loc_4FE88
    }
    byte_510B9 = al;
//    mov si, 87CDh
    convertNumberTo3DigitStringWithPadding0(byte_510B2, "");
//    mov si, 87CEh
//    mov di, 182h

//loc_4FE83:              ; CODE XREF: sub_4FDFD+81j
//    mov ah, 6
    drawTextWithChars8Font(0, 0, 6, "");

//loc_4FE98:              ; CODE XREF: sub_4FDFD+90j
//                ; sub_4FDFD+96j
}

void drawTextWithChars8Font_method1(size_t destX, size_t destY, uint8_t color, const char *text) //   proc near       ; CODE XREF: drawTextWithChars8Font+7p
{
    // Parameters:
    // - di is the destination surface
    // - si is the text to be rendered
    // - ah is the color index in the current palette

    byte_51969 = color;

//loc_4FEBE:              ; CODE XREF: drawTextWithChars8Font_method1+1C3j
    if (text[0] == '\0')
    {
        return;
    }

//loc_4FED0:              ; CODE XREF: drawTextWithChars8Font_method1+2Fj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

//loc_4FEC8:              ; CODE XREF: drawTextWithChars8Font_method1+27j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacterWidth; ++x)
            {
                uint8_t bitmapCharacterRow = gChars8BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacterWidth + destX + x);
                gScreenPixels[destAddress] = color * pixelValue;
            }
        }
    }
}

void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text) //   proc near       ; CODE XREF: sub_4955B+446p
                    // ; sub_4955B+6AFp ...
{
    drawTextWithChars8Font_method1(destX, destY, color, text);
}

void sub_501C0() //   proc near       ; CODE XREF: start+338p sub_4955B+678p ...
{
    // In theory videoStatusUnk will always be 1
    if (videoStatusUnk != 2)
    {
//        push    es
//        mov ax, seg zeg000
//        mov es, ax
//        assume es:zeg000
        di = 0;
//        si = panelDataBuffer;
        dx = 0x60; // 96

        do
        {
//loc_501D6:              ; CODE XREF: sub_501C0+1Fj
            cx = 0x28; // 40
            memcpy(di, si, 0x28); // rep movsb
            di += 0x28; si += 0x28;
            di += 2;
            dx--;
        }
        while (dx != 0);
//        pop es
//        assume es:nothing
        drawGamePanelText();
        sub_5024B();
        return;
    }

//loc_501EA:              ; CODE XREF: sub_501C0+5j
    di = 0;
//    si = panelDataBuffer;
    /*
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    // inc dx
    // al = 0
    // out dx, al      ; EGA port: graphics controller data register
    ports[0x3CF] = 0;
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    // inc dx
    // al = 0
    // out dx, al      ; EGA port: graphics controller data register
    ports[0x3CF] = 0;
    // mov dx, 3CEh
    // al = 8
    // out dx, al      ; EGA: graph 1 and 2 addr reg:
    //             ; bit mask
    //             ; Bits 0-7 select bits to be masked in all planes
    ports[0x3CE] = 8;
    inc dx
    al = 0FFh
    out dx, al      ; EGA port: graphics controller data register
     */
    cx = 0x18; // 24

    for (int i = 0; i < 24; ++i)
    {
//loc_50211:              ; CODE XREF: sub_501C0+71j
//        push(cx);
        ah = 1;

        do
        {
//loc_50214:              ; CODE XREF: sub_501C0+6Bj
            /*
            mov dx, 3C4h
            al = 2
            out dx, al      ; EGA: sequencer address reg
                        ; map mask: data bits 0-3 enable writes to bit planes 0-3
            inc dx
            al = ah
            out dx, al      ; EGA port: sequencer data register
             */
            cx = 0x28; // 40
            memcpy(di, si, 0x28);
            di += 0x28; si += 0x28;
            di -= 0x28;
            ah = ah << 1;
        }
        while ((ah & 0xF) != 0);
        di += 0x7A; // 122
//        pop(cx);
    }
    /*
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = 0FFh
    out dx, al      ; EGA port: sequencer data register
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: graphics controller data register
     */
    drawGamePanelText();
}

void sub_5024B() //   proc near       ; CODE XREF: gameloop?+31p
                  //  ; sub_501C0+25p
{
/*
    cmp videoStatusUnk, 1
    jnz short loc_50253
    return;

loc_50253:              ; CODE XREF: sub_5024B+5j
    mov si, 0
    mov bl, gNumberOfDotsToShiftDataLeft
    xor bh, bh
    mov ax, 0FC0h
    mul bx
    add si, ax
    push    ds
    mov ax, seg zeg000
    mov ds, ax
    assume ds:zeg000
    mov di, 0
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    // inc dx
    // al = 0
    // out dx, al      ; EGA port: graphics controller data register
    ports[0x3CF] = 0;
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    // inc dx
    // al = 0
    // out dx, al      ; EGA port: graphics controller data register
    ports[0x3CF] = 0;
    // mov dx, 3CEh
    // al = 8
    // out dx, al      ; EGA: graph 1 and 2 addr reg:
    //             ; bit mask
    //             ; Bits 0-7 select bits to be masked in all planes
    ports[0x3CE] = 8;
    inc dx
    al = 0FFh
    out dx, al      ; EGA port: graphics controller data register
    mov cx, 18h

loc_5028D:              ; CODE XREF: sub_5024B+62j
    push(cx);
    mov ah, 1

loc_50290:              ; CODE XREF: sub_5024B+5Cj
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = ah
    out dx, al      ; EGA port: sequencer data register
    mov cx, 2Ah ; '*'
    rep movsb
    sub di, 2Ah ; '*'
    shl ah, 1
    test    ah, 0Fh
    jnz short loc_50290
    add di, 7Ah ; 'z'
    pop(cx);
    loop    loc_5028D
    mov dx, 3C4h
    al = 2
    out dx, al      ; EGA: sequencer address reg
                ; map mask: data bits 0-3 enable writes to bit planes 0-3
    inc dx
    al = 0FFh
    out dx, al      ; EGA port: sequencer data register
    mov dx, 3CEh
    al = 1
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; enable set/reset
    inc dx
    al = 0Fh
    out dx, al      ; EGA port: graphics controller data register
    pop ds
    assume ds:data
    mov dx, 3CEh
    al = 5
    out dx, al      ; EGA: graph 1 and 2 addr reg:
                ; mode register.Data bits:
                ; 0-1: Write mode 0-2
                ; 2: test condition
                ; 3: read mode: 1=color compare, 0=direct
                ; 4: 1=use odd/even RAM addressing
                ; 5: 1=use CGA mid-res map (2-bits/pixel)
    inc dx
    al = 1
    out dx, al      ; EGA port: graphics controller data register
    return;
 */
}

void drawSpeedFixTitleAndVersion() //   proc near       ; CODE XREF: start+2E6p
{
    drawTextWithChars6FontWithOpaqueBackground(102, 11, 1, "SUPAPLEX VERSION 7.0");
}

void drawSpeedFixCredits() //  proc near       ; CODE XREF: start+2ECp
{
    drawTextWithChars6FontWithOpaqueBackground(60, 168, 0xE, "VERSIONS 1-4 + 6.X BY HERMAN PERK");
    drawTextWithChars6FontWithOpaqueBackground(60, 176, 0xE, "VERSIONS 5.X BY ELMER PRODUCTIONS");
    drawTextWithChars6FontWithOpaqueBackground(75, 184, 0xE, "VERSIONS 7.X BY SERGIO PADRINO");

    videoloop();

    do
    {
//loc_502F1:             // ; CODE XREF: drawSpeedFixCredits+28j
        int9handler();

        if (byte_519C3 == 1)
        {
            word_51970 = 1;
        }
//loc_50301:             // ; CODE XREF: drawSpeedFixCredits+1Ej
    }
    while (keyPressed == SDL_SCANCODE_UNKNOWN);

    byte_510AB = 1;
}

void exitWithError(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    exit(errno);
}

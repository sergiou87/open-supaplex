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

#include <assert.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "controller.h"
#include "file.h"
#include "logging.h"
#include "touchscreen.h"
#include "virtualKeyboard.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(v, a, b) MIN(MAX(a, v), b)
#define SWAP(x, y, __type) do { __type __temp__ = x; x = y; y = __temp__; } while (0)

#define kScreenWidth 320
#define kScreenHeight 200

// title1DataBuffer -> A000:4DAC - A000:CAAC
// title2DataBuffer -> 0x4DD4 - 0xCAD4

// maps are 58 x 22 tiles

uint8_t fastMode = 0;

typedef enum {
    UserInputNone = 0,
    UserInputUp = 1,
    UserInputLeft = 2,
    UserInputDown = 3,
    UserInputRight = 4,
    UserInputSpaceUp = 5,
    UserInputSpaceLeft = 6,
    UserInputSpaceDown = 7,
    UserInputSpaceRight = 8,
    UserInputSpaceOnly = 9,
} UserInput;

static const uint8_t kUserInputSpaceAndDirectionOffset = (UserInputSpaceUp - 1);

// exact length of a level file, even of each level inside the LEVELS.DAT file
#define levelDataLength 1536
uint8_t byte_50919 = 0;
uint8_t byte_5091A = 0;
UserInput gCurrentUserInput = 0; // byte_50941 -> 0x0631
uint8_t byte_50946 = 0;
uint16_t word_50947 = 0;
uint16_t word_50949 = 0;
uint8_t byte_50953 = 0;
uint8_t byte_50954 = 0;
uint8_t gIsGravityEnabled = 0; // byte_5101C -> 1 = turn on, anything else (0) = turn off
uint8_t gAreZonksFrozen = 0; // byte_51035 -> 2 = turn on, anything else (0) = turn off  (1=off!)
// uint8_t gNumberOfInfoTronsInCurrentLevel = 0; // 0xd26 -> byte_51036 -> this seems to be _inside_ of fileLevelData when a level is read
// uint8_t byte_51037 = 0; // 0xd27 -> this seems to be _inside_ of fileLevelData when a level is read, and it's numberOfSpecialPorts
uint8_t byte_510AB = 0;
uint16_t word_510AC = 0; // stored in 0B5D:0D9C
uint8_t gIsGamePaused = 0; // byte_510AE
uint8_t gAuxGameSeconds20msAccumulator = 0; // byte_510AF ->  -> accumulates game time. The total time is its value * 20ms, so when it reaches 50 it means 1 second. Used to increase the game time in the bottom panel
uint8_t byte_510B3 = 0;
uint8_t byte_510B4 = 0;
uint8_t byte_510B5 = 0;
uint8_t byte_510B6 = 0;
uint8_t gShouldShowFailedLevelResultScreen = 0; // byte_510BA
uint8_t byte_510BB = 0;
uint8_t gIsExplosionStarted = 0; // byte_510C0 -> Set to 1 when an explosion is just created. Set back to 0 when _any_ of the explosions on the screen disappears.
uint8_t byte_510D3 = 0;
uint8_t gAreEnemiesFrozen = 0; // byte_510D7 -> 1 = turn on, anything else (0) = turn off
uint8_t byte_510D8 = 0;
uint8_t gPlantedRedDiskCountdown = 0; // byte_510DB
uint16_t word_510DF = 0;
uint8_t byte_510E1 = 0;
uint8_t byte_510E2 = 0;
uint16_t word_510E4 = 0; // This is probably a FILE *
uint8_t gTotalNumberOfInfotrons = 0; // byte_5195B
uint8_t gNumberOfRemainingRedDisks = 0; // byte_5195C
uint8_t byte_51969 = 0;
uint8_t byte_5196A = 0;
uint8_t byte_5196B = 0;
uint8_t byte_5197C = 0;
uint8_t gIsEscapeKeyPressed = 0; // byte_5197E -> ESC
uint8_t gIs1KeyPressed = 0; // byte_5197F -> 1
uint8_t gIs2KeyPressed = 0; // byte_51980 -> 2
uint8_t gIs3KeyPressed = 0; // byte_51981 -> 3
uint8_t gIs4KeyPressed = 0; // byte_51982 -> 4
uint8_t gIs5KeyPressed = 0; // byte_51983 -> 5
uint8_t gIs6KeyPressed = 0; // byte_51984 -> 6
uint8_t gIs7KeyPressed = 0; // byte_51985 -> 7
uint8_t gIs8KeyPressed = 0; // byte_51986 -> 8
uint8_t gIs9KeyPressed = 0; // byte_51987 -> 9
uint8_t gIs0KeyPressed = 0; // byte_51988 -> 0
uint8_t gIsMinusKeyPressed = 0; // byte_51989 -> -
uint8_t gIsEqualsKeyPressed = 0; // byte_5198A -> =
// backspace
// tab
uint8_t gIsQKeyPressed = 0; // byte_5198D -> q
uint8_t gIsWKeyPressed = 0; // byte_5198E -> w
// e
uint8_t gIsRKeyPressed = 0; // byte_51990 -> r
// t
// y
// u
// i
// o
uint8_t gIsPKeyPressed = 0; // byte_51996 -> p
// [
// ]
uint8_t gIsEnterPressed = 0; // byte_51999 -> enter
uint8_t gIsLeftControlPressed = 0; // byte_5199A -> left control
//a
uint8_t gIsSKeyPressed = 0; // byte_5199C -> s
uint8_t gIsDKeyPressed = 0; // byte_5199D -> d
//f
//g
uint8_t gIsHKeyPressed = 0; // byte_519A0 -> h
uint8_t gIsJKeyPressed = 0; // byte_519A1 -> j
//k
uint32_t gIsLKeyPressed = 0; // dword_519A3 -> l
// ; ' `
uint8_t gIsLeftShiftPressed = 0; // word_519A7 -> "left shift"
// slash
uint8_t gIsZKeyPressed = 0; // word_519A9 -> z
// x
uint8_t gIsCKeyPressed = 0; // word_519AB -> c
// v
uint8_t gIsBKeyPressed = 0; // word_519AD -> b
// n
uint8_t gIsMKeyPressed = 0; // dword_519AF -> m
// , . /
uint8_t gIsRightShiftPressed = 0; // word_519B3 -> "right shift"
// "numpad *"
uint8_t gIsLeftAltPressed = 0; // byte_519B5 -> "left alt"
uint8_t gIsSpaceKeyPressed = 0; // byte_519B6 -> space
// caps lock
uint8_t gIsF1KeyPressed = 0; // byte_519B8 -> F1
uint8_t gIsF2KeyPressed = 0; // byte_519B9 -> F2
uint8_t gIsF3KeyPressed = 0; // byte_519BA -> F3
uint8_t gIsF4KeyPressed = 0; // byte_519BB -> F4
uint8_t gIsF5KeyPressed = 0; // byte_519BC -> F5
uint8_t gIsF6KeyPressed = 0; // byte_519BD -> F6
uint8_t gIsF7KeyPressed = 0; // byte_519BE -> F7
uint8_t gIsF8KeyPressed = 0; // byte_519BF -> F8
uint8_t gIsF9KeyPressed = 0; // byte_519C0 -> F9
uint8_t gIsF10KeyPressed = 0; // byte_519C1 -> F10
uint8_t gIsNumLockPressed = 0; // byte_519C2 -> 16B2 -> Num lock
uint8_t gIsScrollLockPressed = 0; // byte_519C3 -> Scroll lock
uint8_t gIsNumpad7Pressed = 0; // byte_519C4 -> numpad 7
uint8_t gIsUpKeyPressed = 0; // byte_519C5 -> 16B5 -> numpad 8 or up arrow
uint8_t gIsNumpad9Pressed = 0; // byte_519C6 -> numpad 9
// numpad -
uint8_t gIsLeftKeyPressed = 0; // byte_519C8 -> numpad 4
uint8_t gIsNumpad5Pressed = 0; // byte_519C9 -> numpad 5
uint8_t gIsRightKeyPressed = 0; // byte_519CA -> numpad 6
// numpad +
uint8_t gIsNumpad1Pressed = 0; // byte_519CC -> numpad 1
uint8_t gIsDownKeyPressed = 0; // byte_519CD -> numpad 2
uint8_t gIsNumpad3Pressed = 0; // byte_519CE -> numpad 3
uint8_t gIsNumpad0Pressed = 0; // byte_519CF -> numpad 0
uint8_t gIsNumpadPeriodPressed = 0; // byte_519D0 -> numpad .
// numpad SysReq
// Key 45
// numpad Enter
uint8_t gIsNumpadDividePressed = 0; // byte_519D4 -> numpad /
uint8_t byte_519D5 = 0; //
uint8_t byte_519F4 = 0; //
uint8_t byte_519F5 = 0; //
uint8_t byte_519F6 = 0; //
uint8_t byte_519F7 = 0; //
uint8_t byte_519F8 = 0; //
uint8_t byte_519F9 = 0; //
uint8_t gShouldAutoselectNextLevelToPlay = 0; // byte_51ABE
uint8_t byte_5870F = 0; //
uint8_t byte_58D46 = 0; //
uint8_t byte_58D47 = 0; //
uint8_t byte_59821 = 0; //
uint8_t byte_59822 = 0; //
uint8_t byte_59823 = 0; //
uint8_t byte_59889 = 0; //
uint8_t byte_5988A = 0; //
uint8_t byte_5988B = 0; //
uint8_t byte_5988C = 0; //
uint8_t byte_5988D = 0x53; // 83 or '+'
uint8_t byte_59890 = 0x58; // 88 or 'X'
uint8_t byte_599D4 = 0;
uint8_t byte_59B5C = 0;
uint8_t byte_59B5F = 0;
uint8_t byte_59B62 = 0;
// uint8_t byte_59B64 = 0;
uint16_t word_59B65 = 0;
uint8_t byte_59B6B = 0;
uint8_t byte_59B6D = 0;
uint8_t byte_59B71 = 0;
uint8_t byte_59B72 = 0;
uint8_t byte_59B7A = 0;
uint8_t byte_59B7B = 1;
uint8_t byte_59B7C = 0;
uint8_t byte_59B7D = 0;
uint8_t byte_59B7E = 0;
uint8_t byte_59B7F = 0;
uint8_t byte_59B80 = 0;
uint8_t byte_59B81 = 0;
uint8_t byte_59B82 = 0;
uint8_t byte_59B83 = 0;
uint8_t byte_59B84 = 0;
uint8_t byte_59B85 = 0;
uint8_t byte_59B86 = 0;
uint16_t word_5A199 = 0;
uint8_t byte_59B9A = 2;
uint8_t byte_5A140 = 0;
uint8_t byte_5A19B = 0;
uint8_t byte_5A19C = 0;
uint8_t byte_5A2F8 = 0;
uint8_t byte_5A2F9 = 0;
uint8_t byte_5A320 = 0;
uint8_t byte_5A321 = 0;
uint8_t byte_5A322 = 0;
uint8_t byte_5A323 = 0;
uint16_t word_5A33C = 0;
uint8_t byte_5A33E = 0;
uint8_t byte_5A33F = 0;
uint8_t gCurrentPlayerIndex = 0; // byte_5981F
uint8_t gGameHours = 0; // byte_510B2
uint8_t gGameMinutes = 0; // byte_510B1
uint8_t gGameSeconds = 0; // byte_510B0
uint8_t gIsMouseAvailable = 0; // byte_58487
uint8_t gIsPlayingDemo = 0; // byte_510DE -> 0DCE
uint8_t gIsRecordingDemo = 0; // byte_510E3 -> 0DD3
uint8_t gLastDrawnHours = 0; // byte_510B9
uint8_t gLevelListButtonPressed = 0; // byte_50918
uint8_t gLevelListDownButtonPressed = 0; // byte_50916
uint8_t gLevelListUpButtonPressed = 0; // byte_50917
uint8_t gNewPlayerEntryIndex = 0; // byte_59820
uint8_t gNumberOfDotsToShiftDataLeft = 0; // byte_510A6 Used for the scroll effect
uint8_t gNumberOfRemainingInfotrons = 0; // byte_5195A
uint8_t gPlayerListButtonPressed = 0; // byte_50912
uint8_t gPlayerListDownButtonPressed = 0; // byte_50910
uint8_t gPlayerListUpButtonPressed = 0; // byte_50911
uint8_t gRankingListButtonPressed = 0; // byte_50915
uint8_t gRankingListDownButtonPressed = 0; // byte_50913
uint8_t gRankingListUpButtonPressed = 0; // byte_50914
uint16_t gCurrentSelectedLevelIndex = 0; // word_51ABC
uint16_t gLastDrawnMinutesAndSeconds = 0; // word_510B7
uint16_t gMurphyPositionX = 0; // word_510E8
uint16_t gMurphyPositionY = 0; // word_510EA
uint16_t gMurphyTileX = 0; // word_510C3
uint16_t gMurphyTileY = 0; // word_510C5
uint16_t gNewPlayerNameLength = 0; // word_58475
uint16_t word_50942 = 0;
uint16_t word_50944 = 0;
uint16_t word_5094B = 0;
uint16_t word_5094D = 0;
uint16_t word_5094F = 0;
uint16_t word_50951 = 0;
uint16_t word_51076 = 0;
uint16_t word_510A2 = 0; // -> used to preserve some palette info??
uint16_t word_510BC = 0;
uint16_t word_510BE = 0;
uint16_t word_510C1 = 0; // -> 0DB1
uint16_t word_510C7 = 0; // stores gMurphyLocation too??
uint16_t word_510CB = 0;
uint16_t word_510CD = 0;
uint16_t word_510CF = 0;
uint16_t word_510D1 = 0;
uint16_t gIsMurphyGoingThroughPortal = 0; // word_510D9
uint16_t gPlantedRedDiskPosition = 0; // word_510DC
uint16_t gDemoIndexOrDemoLevelNumber = 0; // word_510E6
uint16_t word_510EE = 0;
uint8_t gShouldShowFPS = 0;

typedef struct {
    int16_t word_510F0; // value1; -> seems like an offset from the destination position (in tiles * 2)
    int16_t word_510F2; // value2; -> this increases the offset above frame by frame
    uint16_t width; // value3;
    uint16_t height; // value4;
    uint16_t animationIndex; // word_510F8; -> memory address in someBinaryData_5142E, looks like a list of coordinates of frames in MOVING.DAT
    int16_t speedX; // value6; -> applied to Murphy X position... speedX?
    int16_t speedY; // value7; -> applied to Murphy Y position... speedY?
    uint16_t currentFrame; // Not used in the original code, I will use it to keep track of the current animation frame
} MurphyAnimationDescriptor;

MurphyAnimationDescriptor gCurrentMurphyAnimation; // -> starts at 0x0DE0
//uint16_t word_510F0 = 0; // -> 0x0DE0 value1
//uint16_t word_510F2 = 0; // value2
//uint16_t word_510F4 = 0; // value3
//uint16_t word_510F6 = 0; // value4
//uint16_t word_510F8 = 0; // value5
//uint16_t word_510FA = 0; // value6
//uint16_t word_510FC = 0; // value7

uint8_t someBinaryData_510FE[16] = {
    0, // -> 0xdee
    0, // -> 0xdef
    0, // -> 0xdf0
    0, // -> 0xdf1
    0, // -> 0xdf2
    0, // -> 0xdf3
    0, // -> 0xdf4
    0, // -> 0xdf5
    0, // -> 0xdf6
    0, // -> 0xdf7
    0, // -> 0xdf8
    0, // -> 0xdf9
    0, // -> 0xdfa
    0, // -> 0xdfb
    0, // -> 0xdfc
    0, // -> 0xdfd
};

MurphyAnimationDescriptor someBinaryData_5110E[50] = {
    { // 0
        0x06ac, // -> dfe
        0xff0c, // -> e00
        0x0002, // -> e02
        0x0012, // -> e04
        0, // 0x111e, // -> e06
        0x0000, // -> e08
        0xfffe, // -> e0a
        0x0000, // -> e0c
    }, { // 1
        0x06ac, // -> e0e
        0xff0c, // -> e10
        0x0002, // -> e12
        0x0012, // -> e14
        1, // 0x1130, // -> e16
        0x0000, // -> e18
        0xfffe, // -> e1a
        0x0000, // -> e1c
    }, { // 2
        0x0000, // -> e1e
        0x0000, // -> e20
        0x0004, // -> e22
        0x0010, // -> e24
        4, // 0x1166, // -> e26
        0xfffe, // -> e28
        0x0000, // -> e2a
        0x0000, // -> e2c
    }, { // 3
        0xf860, // -> e2e
        0x00f4, // -> e30
        0x0002, // -> e32
        0x0012, // -> e34
        2, // 0x1142, // -> e36
        0x0000, // -> e38
        0x0002, // -> e3a
        0x0000, // -> e3c
    }, { // 4
        0xf860, // -> e3e
        0x00f4, // -> e40
        0x0002, // -> e42
        0x0012, // -> e44
        3, // 0x1154 // -> e46
        0x0000, // -> e48
        0x0002, // -> e4a
        0x0000, // -> e4c
    }, { // 5
        0xfffe, // -> e4e
        0x0000, // -> e50
        0x0004, // -> e52
        0x0010, // -> e54
        5, // 0x1178, // -> e56
        0x0002, // -> e58
        0x0000, // -> e5a
        0x0000, // -> e5c
    }, { // 6
        0x0000, // -> e5e
        0x0000, // -> e60
        0x0002, // -> e62
        0x0010, // -> e64
        6, // 0x118a, // -> e66
        0x0000, // -> e68
        0x0000, // -> e6a
        0x0000, // -> e6c
    }, { // 7
        0x06ac, // -> e6e
        0xff0c, // -> e70
        0x0002, // -> e72
        0x0012, // -> e74
        0, // 0x111e, // -> e76
        0x0000, // -> e78
        0xfffe, // -> e7a
        0x0000, // -> e7c
    }, { // 8
        0x06ac, // -> e7e
        0xff0c, // -> e80
        0x0002, // -> e82
        0x0012, // -> e84
        1, // 0x1130, // -> e86
        0x0000, // -> e88
        0xfffe, // -> e8a
        0x0000, // -> e8c
    }, { // 9
        0x0000, // -> e8e
        0x0000, // -> e90
        0x0004, // -> e92
        0x0010, // -> e94
        7, // 0x11dc, // -> e96
        0xfffe, // -> e98
        0x0000, // -> e9a
        0x0000, // -> e9c
    }, { // 10
        0xf860, // -> e9e
        0x00f4, // -> ea0
        0x0002, // -> ea2
        0x0012, // -> ea4
        2, // 0x1142, // -> ea6
        0x0000, // -> ea8
        0x0002, // -> eaa
        0x0000, // -> eac
    }, { // 11
        0xf860, // -> eae
        0x00f4, // -> eb0
        0x0002, // -> eb2
        0x0012, // -> eb4
        3, // 0x1154 // -> eb6
        0x0000, // -> eb8
        0x0002, // -> eba
        0x0000, // -> ebc
    }, { // 12
        0xfffe, // -> ebe
        0x0000, // -> ec0
        0x0004, // -> ec2
        0x0010, // -> ec4
        8, // 0x11ee, // -> ec6
        0x0002, // -> ec8
        0x0000, // -> eca
        0x0000, // -> ecc
    }, { // 13
        0xf860, // -> ece
        0x0000, // -> ed0
        0x0002, // -> ed2
        0x0010, // -> ed4
        9, // 0x1200, // -> ed6
        0x0000, // -> ed8
        0x0000, // -> eda
        0x0000, // -> edc
    }, { // 14
        0xfffe, // -> ede
        0x0000, // -> ee0
        0x0002, // -> ee2
        0x0010, // -> ee4
        9, // 0x1200, // -> ee6
        0x0000, // -> ee8
        0x0000, // -> eea
        0x0000, // -> eec
    }, { // 15
        0x07a0, // -> eee
        0x0000, // -> ef0
        0x0002, // -> ef2
        0x0010, // -> ef4
        9, // 0x1200, // -> ef6
        0x0000, // -> ef8
        0x0000, // -> efa
        0x0000, // -> efc
    }, { // 16
        0x0002, // -> efe
        0x0000, // -> f00
        0x0002, // -> f02
        0x0010, // -> f04
        9, // 0x1200, // -> f06
        0x0000, // -> f08
        0x0000, // -> f0a
        0x0000, // -> f0c
    }, { // 17
        0x06ac, // -> f0e
        0xff0c, // -> f10
        0x0002, // -> f12
        0x0012, // -> f14
        0, // 0x111e, // -> f16
        0x0000, // -> f18
        0xfffe, // -> f1a
        0x0000, // -> f1c
    }, { // 18
        0x06ac, // -> f1e
        0xff0c, // -> f20
        0x0002, // -> f22
        0x0012, // -> f24
        1, // 0x1130, // -> f26
        0x0000, // -> f28
        0xfffe, // -> f2a
        0x0000, // -> f2c
    }, { // 19
        0x0000, // -> f2e
        0x0000, // -> f30
        0x0004, // -> f32
        0x0010, // -> f34
        10, // 0x1212, // -> f36
        0xfffe, // -> f38
        0x0000, // -> f3a
        0x0000, // -> f3c
    }, { // 20
        0xf860, // -> f3e
        0x00f4, // -> f40
        0x0002, // -> f42
        0x0012, // -> f44
        2, // 0x1142, // -> f46
        0x0000, // -> f48
        0x0002, // -> f4a
        0x0000, // -> f4c
    }, { // 21
        0xf860, // -> f4e
        0x00f4, // -> f50
        0x0002, // -> f52
        0x0012, // -> f54
        3, // 0x1154 // -> f56
        0x0000, // -> f58
        0x0002, // -> f5a
        0x0000, // -> f5c
    }, { // 22
        0xfffe, // -> f5e
        0x0000, // -> f60
        0x0004, // -> f62
        0x0010, // -> f64
        11, // 0x1224, // -> f66
        0x0002, // -> f68
        0x0000, // -> f6a
        0x0000, // -> f6c
    }, { // 23
        0xf860, // -> f6e
        0x0000, // -> f70
        0x0002, // -> f72
        0x0010, // -> f74
        12, // 0x1236, // -> f76
        0x0000, // -> f78
        0x0000, // -> f7a
        0x0000, // -> f7c
    }, { // 24
        0xfffe, // -> f7e
        0x0000, // -> f80
        0x0002, // -> f82
        0x0010, // -> f84
        12, // 0x1236, // -> f86
        0x0000, // -> f88
        0x0000, // -> f8a
        0x0000, // -> f8c
    }, { // 25
        0x07a0, // -> f8e
        0x0000, // -> f90
        0x0002, // -> f92
        0x0010, // -> f94
        12, // 0x1236, // -> f96
        0x0000, // -> f98
        0x0000, // -> f9a
        0x0000, // -> f9c
    }, { // 26
        0x0002, // -> f9e
        0x0000, // -> fa0
        0x0002, // -> fa2
        0x0010, // -> fa4
        12, // 0x1236, // -> fa6
        0x0000, // -> fa8
        0x0000, // -> faa
        0x0000, // -> fac
    }, { // 27
        0xfffc, // -> fae
        0x0000, // -> fb0
        0x0006, // -> fb2
        0x0010, // -> fb4
        13, // 0x1246, // -> fb6
        0xfffe, // -> fb8
        0x0000, // -> fba
        0x0000, // -> fbc
    }, { // 28
        0x0000, // -> fbe
        0x0000, // -> fc0
        0x0006, // -> fc2
        0x0010, // -> fc4
        14, // 0x1258, // -> fc6
        0x0002, // -> fc8
        0x0000, // -> fca
        0x0000, // -> fcc
    }, { // 29
        0x0000, // -> fce
        0xf0c0, // -> fd0
        0x0002, // -> fd2
        0x0010, // -> fd4
        19, // 0x1340, // -> fd6
        0x0000, // -> fd8
        0xfffc, // -> fda
        0x0000, // -> fdc
    }, { // 30
        0x0000, // -> fde
        0xfffc, // -> fe0
        0x0002, // -> fe2
        0x0010, // -> fe4
        15, // 0x12f8, // -> fe6
        0xfffc, // -> fe8
        0x0000, // -> fea
        0x0000, // -> fec
    }, { // 31
        0x0000, // -> fee
        0x0f40, // -> ff0
        0x0002, // -> ff2
        0x0010, // -> ff4
        21, // 0x1364, // -> ff6
        0x0000, // -> ff8
        0x0004, // -> ffa
        0x0000, // -> ffc
    }, { // 32
        0x0000, // -> ffe
        0x0004, // -> 1000
        0x0002, // -> 1002
        0x0010, // -> 1004
        17, // 0x131c, // -> 1006
        0x0004, // -> 1008
        0x0000, // -> 100a
        0x0000, // -> 100c
    }, { // 33
        0xff0c, // -> 100e
        0xff0c, // -> 1010
        0x0002, // -> 1012
        0x0012, // -> 1014
        0, // 0x111e, // -> 1016
        0x0000, // -> 1018
        0xfffe, // -> 101a
        0x0000, // -> 101c
    }, { // 34
        0xff0c, // -> 101e
        0xff0c, // -> 1020
        0x0002, // -> 1022
        0x0012, // -> 1024
        1, // 0x1130, // -> 1026
        0x0000, // -> 1028
        0xfffe, // -> 102a
        0x0000, // -> 102c
    }, { // 35
        0x0000, // -> 102e
        0x0000, // -> 1030
        0x0004, // -> 1032
        0x0010, // -> 1034
        23, // 0x1448, // -> 1036
        0xfffe, // -> 1038
        0x0000, // -> 103a
        0x0000, // -> 103c
    }, { // 36
        0x0000, // -> 103e
        0x00f4, // -> 1040
        0x0002, // -> 1042
        0x0012, // -> 1044
        2, // 0x1142, // -> 1046
        0x0000, // -> 1048
        0x0002, // -> 104a
        0x0000, // -> 104c
    }, { // 37
        0x0000, // -> 104e
        0x00f4, // -> 1050
        0x0002, // -> 1052
        0x0012, // -> 1054
        3, // 0x1154 // -> 1056
        0x0000, // -> 1058
        0x0002, // -> 105a
        0x0000, // -> 105c
    }, { // 38
        0xfffe, // -> 105e
        0x0000, // -> 1060
        0x0004, // -> 1062
        0x0010, // -> 1064
        24, // 0x145a, // -> 1066
        0x0002, // -> 1068
        0x0000, // -> 106a
        0x0000, // -> 106c
    }, { // 39
        0xf860, // -> 106e
        0x0000, // -> 1070
        0x0002, // -> 1072
        0x0010, // -> 1074
        25, // 0x146e, // -> 1076
        0x0000, // -> 1078
        0x0000, // -> 107a
        0x0000, // -> 107c
    }, { // 40
        0xfffe, // -> 107e
        0x0000, // -> 1080
        0x0002, // -> 1082
        0x0010, // -> 1084
        25, // 0x146e, // -> 1086
        0x0000, // -> 1088
        0x0000, // -> 108a
        0x0000, // -> 108c
    }, { // 41
        0x07a0, // -> 108e
        0x0000, // -> 1090
        0x0002, // -> 1092
        0x0010, // -> 1094
        25, // 0x146e, // -> 1096
        0x0000, // -> 1098
        0x0000, // -> 109a
        0x0000, // -> 109c
    }, { // 42
        0x0002, // -> 109e
        0x0000, // -> 10a0
        0x0002, // -> 10a2
        0x0010, // -> 10a4
        25, // 0x146e, // -> 10a6
        0x0000, // -> 10a8
        0x0000, // -> 10aa
        0x0000, // -> 10ac
    }, { // 43
        0xf76c, // -> 10ae
        0xff0c, // -> 10b0
        0x0002, // -> 10b2
        0x0022, // -> 10b4
        28, // 0x1488, // -> 10b6
        0x0000, // -> 10b8
        0xfffe, // -> 10ba
        0x0000, // -> 10bc
    }, { // 44
        0xfffc, // -> 10be
        0x0000, // -> 10c0
        0x0006, // -> 10c2
        0x0010, // -> 10c4
        29, // 0x149a, // -> 10c6
        0xfffe, // -> 10c8
        0x0000, // -> 10ca
        0x0000, // -> 10cc
    }, { // 45
        0x0000, // -> 10ce
        0x00f4, // -> 10d0
        0x0002, // -> 10d2
        0x0022, // -> 10d4
        30, // 0x14ac, // -> 10d6
        0x0000, // -> 10d8
        0x0002, // -> 10da
        0x0000, // -> 10dc
    }, { // 46
        0x0000, // -> 10de
        0x0000, // -> 10e0
        0x0006, // -> 10e2
        0x0010, // -> 10e4
        31, // 0x14be, // -> 10e6
        0x0002, // -> 10e8
        0x0000, // -> 10ea
        0x0000, // -> 10ec
    }, { // 47
        0xfffc, // -> 10ee
        0x0000, // -> 10f0
        0x0006, // -> 10f2
        0x0010, // -> 10f4
        32, // 0x14d0, // -> 10f6
        0xfffe, // -> 10f8
        0x0000, // -> 10fa
        0x0000, // -> 10fc
    }, { // 48
        0x0000, // -> 10fe
        0x0000, // -> 1100
        0x0006, // -> 1102
        0x0010, // -> 1104
        33, // 0x14e2, // -> 1106
        0x0002, // -> 1108
        0x0000, // -> 110a
        0x0000, // -> 110c
    }, { // 49
        0x0000, // -> 110e
        0x0000, // -> 1110
        0x0002, // -> 1112
        0x0010, // -> 1114
        27, // 0x1484, // -> 1116
        0x0000, // -> 1118
        0x0000, // -> 111a
        0x0000, // -> 111c
    }
};

typedef struct
{
    uint16_t x;
    uint16_t y;
} Point;

typedef struct
{
    Point coordinates[105];
    uint8_t numberOfCoordinates;
} AnimationFrameCoordinates;

AnimationFrameCoordinates frameCoordinates_5142E[37] = {
    { // 0
        {
            { 0, 66 }, // -> 0x2ae6 -> 111e
            { 0, 66 }, // -> 0x2ae6 -> 1120
            { 16, 66 }, // -> 0x2ae8 -> 1122
            { 16, 66 }, // -> 0x2ae8 -> 1124
            { 32, 66 }, // -> 0x2aea -> 1126
            { 32, 66 }, // -> 0x2aea -> 1128
            { 16, 66 }, // -> 0x2ae8 -> 112a
            { 16, 66 }, // -> 0x2ae8 -> 112c
        },
        8
    },
    { // 1
        {
            { 48, 66 }, // -> 0x2aec -> 1130
            { 48, 66 }, // -> 0x2aec -> 1132
            { 64, 66 }, // -> 0x2aee -> 1134
            { 64, 66 }, // -> 0x2aee -> 1136
            { 80, 66 }, // -> 0x2af0 -> 1138
            { 80, 66 }, // -> 0x2af0 -> 113a
            { 64, 66 }, // -> 0x2aee -> 113c
            { 64, 66 }, // -> 0x2aee -> 113e
        },
        8
    },
    { // 2
        {
            { 0, 64 }, // -> 0x29f2 -> 1142
            { 0, 64 }, // -> 0x29f2 -> 1144
            { 16, 64 }, // -> 0x29f4 -> 1146
            { 16, 64 }, // -> 0x29f4 -> 1148
            { 32, 64 }, // -> 0x29f6 -> 114a
            { 32, 64 }, // -> 0x29f6 -> 114c
            { 16, 64 }, // -> 0x29f4 -> 114e
            { 16, 64 }, // -> 0x29f4 -> 1150
        },
        8
    },
    { // 3
        {
            { 48, 64 }, // -> 0x29f8 -> 1154
            { 48, 64 }, // -> 0x29f8 -> 1156
            { 64, 64 }, // -> 0x29fa -> 1158
            { 64, 64 }, // -> 0x29fa -> 115a
            { 80, 64 }, // -> 0x29fc -> 115c
            { 80, 64 }, // -> 0x29fc -> 115e
            { 64, 64 }, // -> 0x29fa -> 1160
            { 64, 64 }, // -> 0x29fa -> 1162
        },
        8
    },
    { // 4
        {
            { 32, 32 }, // -> 0x1ab6 -> 1166
            { 64, 32 }, // -> 0x1aba -> 1168
            { 96, 32 }, // -> 0x1abe -> 116a
            { 128, 32 }, // -> 0x1ac2 -> 116c
            { 160, 32 }, // -> 0x1ac6 -> 116e
            { 192, 32 }, // -> 0x1aca -> 1170
            { 224, 32 }, // -> 0x1ace -> 1172
            { 256, 32 }, // -> 0x1ad2 -> 1174
        },
        8
    },
    { // 5
        {
            { 288, 32 }, // -> 0x1ad6 -> 1178
            { 0, 48 }, // -> 0x2252 -> 117a
            { 32, 48 }, // -> 0x2256 -> 117c
            { 64, 48 }, // -> 0x225a -> 117e
            { 96, 48 }, // -> 0x225e -> 1180
            { 128, 48 }, // -> 0x2262 -> 1182
            { 160, 48 }, // -> 0x2266 -> 1184
            { 192, 48 }, // -> 0x226a -> 1186
        },
        8
    },
    { // 6
        {
            { 160, 64 }, // -> 0x2a06 -> 118a
            { 160, 64 }, // -> 0x2a06 -> 118c
            { 160, 64 }, // -> 0x2a06 -> 118e
            { 160, 64 }, // -> 0x2a06 -> 1190
            { 176, 64 }, // -> 0x2a08 -> 1192
            { 176, 64 }, // -> 0x2a08 -> 1194
            { 176, 64 }, // -> 0x2a08 -> 1196
            { 176, 64 }, // -> 0x2a08 -> 1198
            { 192, 64 }, // -> 0x2a0a -> 119a
            { 192, 64 }, // -> 0x2a0a -> 119c
            { 192, 64 }, // -> 0x2a0a -> 119e
            { 192, 64 }, // -> 0x2a0a -> 11a0
            { 208, 64 }, // -> 0x2a0c -> 11a2
            { 208, 64 }, // -> 0x2a0c -> 11a4
            { 208, 64 }, // -> 0x2a0c -> 11a6
            { 208, 64 }, // -> 0x2a0c -> 11a8
            { 224, 64 }, // -> 0x2a0e -> 11aa
            { 224, 64 }, // -> 0x2a0e -> 11ac
            { 224, 64 }, // -> 0x2a0e -> 11ae
            { 224, 64 }, // -> 0x2a0e -> 11b0
            { 240, 64 }, // -> 0x2a10 -> 11b2
            { 240, 64 }, // -> 0x2a10 -> 11b4
            { 240, 64 }, // -> 0x2a10 -> 11b6
            { 240, 64 }, // -> 0x2a10 -> 11b8
            { 256, 64 }, // -> 0x2a12 -> 11ba
            { 256, 64 }, // -> 0x2a12 -> 11bc
            { 256, 64 }, // -> 0x2a12 -> 11be
            { 256, 64 }, // -> 0x2a12 -> 11c0
            { 272, 64 }, // -> 0x2a14 -> 11c2
            { 272, 64 }, // -> 0x2a14 -> 11c4
            { 272, 64 }, // -> 0x2a14 -> 11c6
            { 272, 64 }, // -> 0x2a14 -> 11c8
            { 288, 64 }, // -> 0x2a16 -> 11ca
            { 288, 64 }, // -> 0x2a16 -> 11cc
            { 288, 64 }, // -> 0x2a16 -> 11ce
            { 288, 64 }, // -> 0x2a16 -> 11d0
            { 240, 0 }, // -> 0x0b90 -> 11d2
            { 240, 0 }, // -> 0x0b90 -> 11d4
            { 240, 0 }, // -> 0x0b90 -> 11d6
            { 240, 0 }, // -> 0x0b90 -> 11d8
        },
        40
    },
    { // 7
        {
            { 0, 0 }, // -> 0x0b72 -> 11dc
            { 32, 0 }, // -> 0x0b76 -> 11de
            { 64, 0 }, // -> 0x0b7a -> 11e0
            { 96, 0 }, // -> 0x0b7e -> 11e2
            { 128, 0 }, // -> 0x0b82 -> 11e4
            { 160, 0 }, // -> 0x0b86 -> 11e6
            { 192, 0 }, // -> 0x0b8a -> 11e8
            { 224, 0 }, // -> 0x0b8e -> 11ea
        },
        8
    },
    { // 8
        {
            { 256, 0 }, // -> 0x0b92 -> 11ee
            { 288, 0 }, // -> 0x0b96 -> 11f0
            { 0, 16 }, // -> 0x1312 -> 11f2
            { 32, 16 }, // -> 0x1316 -> 11f4
            { 64, 16 }, // -> 0x131a -> 11f6
            { 96, 16 }, // -> 0x131e -> 11f8
            { 128, 16 }, // -> 0x1322 -> 11fa
            { 160, 16 }, // -> 0x1326 -> 11fc
        },
        8
    },
    { // 9
        {
            { 256, 84 }, // -> 0x339a -> 1200
            { 272, 84 }, // -> 0x339c -> 1202
            { 288, 84 }, // -> 0x339e -> 1204
            { 304, 84 }, // -> 0x33a0 -> 1206
            { 256, 100 }, // -> 0x3b3a -> 1208
            { 272, 100 }, // -> 0x3b3c -> 120a
            { 288, 100 }, // -> 0x3b3e -> 120c
            { 304, 148 }, // -> 0x0514 -> 120e
        },
        8
    },
    { // 10
        {
            { 0, 212 }, // -> 0x236e -> 1212
            { 32, 212 }, // -> 0x2372 -> 1214
            { 64, 212 }, // -> 0x2376 -> 1216
            { 96, 212 }, // -> 0x237a -> 1218
            { 128, 212 }, // -> 0x237e -> 121a
            { 160, 212 }, // -> 0x2382 -> 121c
            { 192, 212 }, // -> 0x2386 -> 121e
            { 224, 212 }, // -> 0x238a -> 1220
        },
        8
    },
    { // 11
        {
            { 256, 212 }, // -> 0x238e -> 1224
            { 288, 212 }, // -> 0x2392 -> 1226
            { 0, 228 }, // -> 0x2b0e -> 1228
            { 32, 228 }, // -> 0x2b12 -> 122a
            { 64, 228 }, // -> 0x2b16 -> 122c
            { 96, 228 }, // -> 0x2b1a -> 122e
            { 128, 228 }, // -> 0x2b1e -> 1230
            { 160, 228 }, // -> 0x2b22 -> 1232
        },
        8
    },
    { // 12
        {
            { 192, 148 }, // -> 0x0506 -> 1236
            { 208, 148 }, // -> 0x0508 -> 1238
            { 224, 148 }, // -> 0x050a -> 123a
            { 256, 148 }, // -> 0x050e -> 123c
            { 272, 148 }, // -> 0x0510 -> 123e
            { 288, 148 }, // -> 0x0512 -> 1240
            { 304, 148 }, // -> 0x0514 -> 1242
        },
        7
    },
    { // 13
        {
            { 0, 116 }, // -> 0x42ba -> 1246
            { 48, 116 }, // -> 0x42c0 -> 1248
            { 96, 116 }, // -> 0x42c6 -> 124a
            { 144, 116 }, // -> 0x42cc -> 124c
            { 192, 116 }, // -> 0x42d2 -> 124e
            { 240, 116 }, // -> 0x42d8 -> 1250
            { 0, 132 }, // -> 0x4a5a -> 1252
            { 48, 132 }, // -> 0x4a60 -> 1254
        },
        8
    },
    { // 14
        {
            { 96, 132 }, // -> 0x4a66 -> 1258
            { 144, 132 }, // -> 0x4a6c -> 125a
            { 192, 132 }, // -> 0x4a72 -> 125c
            { 240, 132 }, // -> 0x4a78 -> 125e
            { 0, 148 }, // -> 0x04ee -> 1260
            { 48, 148 }, // -> 0x04f4 -> 1262
            { 96, 148 }, // -> 0x04fa -> 1264
            { 144, 148 }, // -> 0x0500 -> 1266
        },
        8
    },
//AnimationFrameCoordinates frameCoordinates_515C6[8] = {
    { // 15
        {
            { 48, 32 }, // -> 0x1ab8 -> 12f8
            { 80, 32 }, // -> 0x1abc -> 12fa
            { 112, 32 }, // -> 0x1ac0 -> 12fc
            { 144, 32 }, // -> 0x1ac4 -> 12fe
            { 176, 32 }, // -> 0x1ac8 -> 1300
            { 208, 32 }, // -> 0x1acc -> 1302
            { 240, 32 }, // -> 0x1ad0 -> 1304
            { 272, 32 }, // -> 0x1ad4 -> 1306
        },
        8
    },
    { // 16
        {
            { 32, 32 }, // -> 0x1ab6 -> 130a
            { 64, 32 }, // -> 0x1aba -> 130c
            { 96, 32 }, // -> 0x1abe -> 130e
            { 128, 32 }, // -> 0x1ac2 -> 1310
            { 160, 32 }, // -> 0x1ac6 -> 1312
            { 192, 32 }, // -> 0x1aca -> 1314
            { 224, 32 }, // -> 0x1ace -> 1316
            { 256, 32 }, // -> 0x1ad2 -> 1318
        },
        8
    },
    { // 17
        {
            { 288, 32 }, // -> 0x1ad6 -> 131c
            { 0, 48 }, // -> 0x2252 -> 131e
            { 32, 48 }, // -> 0x2256 -> 1320
            { 64, 48 }, // -> 0x225a -> 1322
            { 96, 48 }, // -> 0x225e -> 1324
            { 128, 48 }, // -> 0x2262 -> 1326
            { 160, 48 }, // -> 0x2266 -> 1328
            { 192, 48 }, // -> 0x226a -> 132a
        },
        8
    },
    { // 18
        {
            { 304, 32 }, // -> 0x1ad8 -> 132e
            { 16, 48 }, // -> 0x2254 -> 1330
            { 48, 48 }, // -> 0x2258 -> 1332
            { 80, 48 }, // -> 0x225c -> 1334
            { 112, 48 }, // -> 0x2260 -> 1336
            { 144, 48 }, // -> 0x2264 -> 1338
            { 176, 48 }, // -> 0x2268 -> 133a
            { 208, 48 }, // -> 0x226c -> 133c
        },
        8
    },
    { // 19
        {
            { 304, 134 }, // -> 0x4b74 -> 1340
            { 304, 136 }, // -> 0x4c68 -> 1342
            { 304, 138 }, // -> 0x0050 -> 1344
            { 304, 140 }, // -> 0x0144 -> 1346
            { 304, 142 }, // -> 0x0238 -> 1348
            { 304, 144 }, // -> 0x032c -> 134a
            { 304, 146 }, // -> 0x0420 -> 134c
            { 304, 148 }, // -> 0x0514 -> 134e
        },
        8
    },
    { // 20
        {
            { 304, 118 }, // -> 0x43d4 -> 1352
            { 304, 120 }, // -> 0x44c8 -> 1354
            { 304, 122 }, // -> 0x45bc -> 1356
            { 304, 124 }, // -> 0x46b0 -> 1358
            { 304, 126 }, // -> 0x47a4 -> 135a
            { 304, 128 }, // -> 0x4898 -> 135c
            { 304, 130 }, // -> 0x498c -> 135e
            { 304, 132 }, // -> 0x4a80 -> 1360
        },
        8
    },
    { // 21
        {
            { 304, 130 }, // -> 0x498c -> 1364
            { 304, 128 }, // -> 0x4898 -> 1366
            { 304, 126 }, // -> 0x47a4 -> 1368
            { 304, 124 }, // -> 0x46b0 -> 136a
            { 304, 122 }, // -> 0x45bc -> 136c
            { 304, 120 }, // -> 0x44c8 -> 136e
            { 304, 118 }, // -> 0x43d4 -> 1370
            { 304, 116 }, // -> 0x42e0 -> 1372
        },
        8
    },
    { // 22
        {
            { 304, 146 }, // -> 0x0420 -> 1376
            { 304, 144 }, // -> 0x032c -> 1378
            { 304, 142 }, // -> 0x0238 -> 137a
            { 304, 140 }, // -> 0x0144 -> 137c
            { 304, 138 }, // -> 0x0050 -> 137e
            { 304, 136 }, // -> 0x4c68 -> 1380
            { 304, 134 }, // -> 0x4b74 -> 1382
            { 304, 132 }, // -> 0x4a80 -> 1384
        },
        8
    },
//AnimationFrameCoordinates frameCoordinates_51714[2] = {
    { // 23
        {
            // Murphy eating red disk left
            { 128, 260 }, // -> 0x3a5e -> 1448
            { 160, 260 }, // -> 0x3a62 -> 144a
            { 192, 260 }, // -> 0x3a66 -> 144c
            { 224, 260 }, // -> 0x3a6a -> 144e
            { 256, 260 }, // -> 0x3a6e -> 1450
            { 288, 260 }, // -> 0x3a72 -> 1452
            { 288, 276 }, // -> 0x4212 -> 1454
            { 288, 292 }, // -> 0x49b2 -> 1456
        },
        8
    },
    { // 24
        {
            // Murphy eating red disk right
            { 192, 308 }, // -> 0x043a -> 145a
            { 224, 308 }, // -> 0x043e -> 145c
            { 256, 308 }, // -> 0x0442 -> 145e
            { 288, 308 }, // -> 0x0446 -> 1460
            { 288, 308 }, // -> 0x0446 -> 1462
            { 288, 324 }, // -> 0x0be6 -> 1464
            { 288, 340 }, // -> 0x1386 -> 1466
            { 192, 356 }, // -> 0x1b1a -> 1468
            { 224, 356 }, // -> 0x1b1e -> 146a
        },
        9
    },
    { // 25
        {
            { 256, 164 }, // 0x0CAE -> 146e
            { 272, 164 }, // 0x0CB0 -> 1470
            { 288, 164 }, // 0x0CB2 -> 1472
            { 304, 164 }, // 0x0CB4 -> 1474
            { 256, 180 }, // 0x144E -> 1476
            { 272, 180 }, // 0x1450 -> 1478
            { 288, 180 }, // 0x1452 -> 147A
            { 304, 180 }, // 0x1454 -> 147C
        },
        8
    },
    { // 26
        {
            { 288, 132 }, // 0x4A7E -> 1480
        },
        1
    },
    { // 27
        {
            { 256, 164 }, // 0x0CAE -> 1484
        },
        1
    },
    { // 28
        {
            { 304, 406 }, // 0x32FC -> 1488
            { 304, 406 }, // 0x32FC -> 148A
            { 304, 406 }, // 0x32FC -> 148C
            { 304, 406 }, // 0x32FC -> 148E
            { 304, 406 }, // 0x32FC -> 1490
            { 304, 406 }, // 0x32FC -> 1492
            { 304, 406 }, // 0x32FC -> 1494
            { 304, 406 }, // 0x32FC -> 1496
        },
        8
    },
    { // 29
        {
            { 0, 324 }, // 0x0BC2 -> 149A
            { 48, 324 }, // 0x0BC8 -> 149C
            { 96, 324 }, // 0x0BCE -> 149E
            { 144, 324 }, // 0x0BD4 -> 14A0
            { 192, 324 }, // 0x0BDA -> 14A2
            { 240, 324 }, // 0x0BE0 -> 14A4
            { 0, 340 }, // 0x1362 -> 14A6
            { 48, 340 }, // 0x1368 -> 14A8
        },
        8
    },
    { // 30
        {
            { 288, 406 }, // 0x32FA -> 14AC
            { 288, 406 }, // 0x32FA -> 14AE
            { 288, 406 }, // 0x32FA -> 14B0
            { 288, 406 }, // 0x32FA -> 14B2
            { 288, 406 }, // 0x32FA -> 14B4
            { 288, 406 }, // 0x32FA -> 14B6
            { 288, 406 }, // 0x32FA -> 14B8
            { 288, 406 }, // 0x32FA -> 14BA
        },
        8
    },
    { // 31
        {
            { 96, 340 }, // 0x136E -> 14BE
            { 144, 340 }, // 0x1374 -> 14C0
            { 192, 340 }, // 0x137A -> 14C2
            { 240, 340 }, // 0x1380 -> 14C4
            { 0, 356 }, // 0x1B02 -> 14C6
            { 48, 356 }, // 0x1B08 -> 14C8
            { 96, 356 }, // 0x1B0E -> 14CA
            { 144, 356 }, // 0x1B14 -> 14CC
        },
        8
    },
    { // 32
        {
            { 0, 276 }, // 0x41EE -> 14D0
            { 48, 276 }, // 0x41F4 -> 14D2
            { 96, 276 }, // 0x41FA -> 14D4
            { 144, 276 }, // 0x4200 -> 14D6
            { 192, 276 }, // 0x4206 -> 14D8
            { 240, 276 }, // 0x420C -> 14DA
            { 0, 292 }, // 0x498E -> 14DC
            { 48, 292 }, // 0x4994 -> 14DE
        },
        8
    },
    { // 33
        {
            { 96, 292 }, // 0x499A -> 14E2
            { 144, 292 }, // 0x49A0 -> 14E4
            { 192, 292 }, // 0x49A6 -> 14E6
            { 240, 292 }, // 0x49AC -> 14E8
            { 0, 308 }, // 0x0422 -> 14EA
            { 48, 308 }, // 0x0428 -> 14EC
            { 96, 308 }, // 0x042E -> 14EE
            { 144, 308 }, // 0x0434 -> 14F0
        },
        8
    },
    { // 34
        {
            { 32, 446 }, // 0x45EA -> 14F4
            { 48, 446 }, // 0x45EC -> 14F6
            { 64, 446 }, // 0x45EE -> 14F8
            { 80, 446 }, // 0x45F0 -> 14FA
            { 96, 446 }, // 0x45F2 -> 14FC
            { 112, 446 }, // 0x45F4 -> 14FE
            { 128, 446 }, // 0x45F6 -> 1500
            { 144, 446 }, // 0x45F8 -> 1502
            { 160, 446 }, // 0x45FA -> 1504
            { 176, 446 }, // 0x45FC -> 1506
            { 192, 446 }, // 0x45FE -> 1508
            { 208, 446 }, // 0x4600 -> 150A
        },
        12
    },
    { // 35
        {
            { 304, 446 }, // 0x460C -> 150E
            { 288, 446 }, // 0x460A -> 1510
            { 272, 446 }, // 0x4608 -> 1512
        },
        3
    },
    { // 36
        {
            { 224, 446 }, // 0x4602 -> 1516
            { 240, 446 }, // 0x4604 -> 1518
            { 256, 446 }, // 0x4606 -> 151A
        },
        3
    },
};

uint16_t word_5157A = 0x4A62; // -> 0x126A -> (64, 132)
uint16_t word_5157C = 0x0502; // -> 0x126C -> (97, 132)
uint16_t kMurphyStillSpriteCoordinates = 0x4A80; // word_5157E -> 0x126E -> (304, 132)
uint16_t word_51580 = 0x1AB2; // -> 0x1270 -> (0, 32)
static const Point kBugFrameCoordinates[16] = { // binaryData_51582
    { 304, 100 }, // 0x3B40 -> 0x1272
    { 256, 196 }, // 0x1BEE -> 0x1274
    { 272, 196 }, // 0x1BF0 -> 0x1276
    { 288, 196 }, // 0x1BF2 -> 0x1278
    { 304, 196 }, // 0x1BF4 -> 0x127A
    { 288, 196 }, // 0x1BF2 -> 0x127C
    { 272, 196 }, // 0x1BF0 -> 0x127E
    { 288, 196 }, // 0x1BF2 -> 0x1280
    { 304, 196 }, // 0x1BF4 -> 0x1282
    { 288, 196 }, // 0x1BF2 -> 0x1284
    { 272, 196 }, // 0x1BF0 -> 0x1286
    { 256, 196 }, // 0x1BEE -> 0x1288
    { 304, 100 }, // 0x3B40 -> 0x128A
    { 304, 64 }, // 0x2A18 -> 0x128C
    { 304, 64 }, // 0x2A18 -> 0x128E -> this one probably doesn't belong to this array (there are only 14 bug frames according to updateBugTiles)
    { 224, 84 }, // 0x3396 -> 0x1290 -> this is a zonk, probably doesn't belong to this array
};

uint16_t word_515A2 = 0x32A2; // -> 0x1292 -> (224, 82) ??

// Zonk frame coordinates
Point kZonkSlideLeftAnimationFrameCoordinates[8] = { // binaryData_515A4
    { 0, 84 }, // 0x337A -> 0x1294
    { 32, 84 }, // 0x337E -> 0x1296
    { 64, 84 }, // 0x3382 -> 0x1298
    { 96, 84 }, // 0x3386 -> 0x129A
    { 128, 84 }, // 0x338A -> 0x129C
    { 160, 84 }, // 0x338E -> 0x129E
    { 192, 84 }, // 0x3392 -> 0x12A0
    { 224, 84 }, // 0x3396 -> 0x12A2
};

Point kZonkSlideRightAnimationFrameCoordinates[8] = { // binaryData_515B4
    { 0, 100 }, // 0x3B1A -> 0x12A4
    { 32, 100 }, // 0x3B1E -> 0x12A6
    { 64, 100 }, // 0x3B22 -> 0x12A8
    { 96, 100 }, // 0x3B26 -> 0x12AA
    { 128, 100 }, // 0x3B2A -> 0x12AC
    { 160, 100 }, // 0x3B2E -> 0x12AE
    { 192, 100 }, // 0x3B32 -> 0x12B0
    { 224, 100 }, // 0x3B36 -> 0x12B2
};

uint16_t word_515C4 = 0x1358; // (240, 178) -> 0x12B4

static const Point kInfotronSlideLeftAnimationFrameCoordinates[8] = {
    { 0, 164 }, // -> 0x0c8e -> 12b6
    { 32, 164 }, // -> 0x0c92 -> 12b8
    { 64, 164 }, // -> 0x0c96 -> 12ba
    { 96, 164 }, // -> 0x0c9a -> 12bc
    { 8, 164 }, // -> 0x0c8f -> 12be
    { 160, 164 }, // -> 0x0ca2 -> 12c0
    { 192, 164 }, // -> 0x0ca6 -> 12c2
    { 224, 164 }, // -> 0x0caa -> 12c4
};

static const Point kInfotronSlideRightAnimationFrameCoordinates[8] = {
    { 0, 180 }, // -> 0x142e -> 12c6
    { 32, 180 }, // -> 0x1432 -> 12c8
    { 64, 180 }, // -> 0x1436 -> 12ca
    { 96, 180 }, // -> 0x143a -> 12cc
    { 128, 180 }, // -> 0x143e -> 12ce
    { 160, 180 }, // -> 0x1442 -> 12d0
    { 192, 180 }, // -> 0x1446 -> 12d2
    { 224, 180 }, // -> 0x144a -> 12d4
};

static const Point kRegularExplosionAnimationFrameCoordinates[8] = {
    { 0, 196 }, // -> 0x1bce -> 12d6
    { 16, 196 }, // -> 0x1bd0 -> 12d8
    { 32, 196 }, // -> 0x1bd2 -> 12da
    { 48, 196 }, // -> 0x1bd4 -> 12dc
    { 64, 196 }, // -> 0x1bd6 -> 12de
    { 80, 196 }, // -> 0x1bd8 -> 12e0
    { 96, 196 }, // -> 0x1bda -> 12e2
    { 112, 196 }, // -> 0x1bdc -> 12e4
};

static const Point kInfotronExplosionAnimationFrameCoordinates[8] = {
    { 128, 196 }, // -> 0x1bde -> 12e6
    { 144, 196 }, // -> 0x1be0 -> 12e8
    { 160, 196 }, // -> 0x1be2 -> 12ea
    { 176, 196 }, // -> 0x1be4 -> 12ec
    { 192, 196 }, // -> 0x1be6 -> 12ee
    { 208, 196 }, // -> 0x1be8 -> 12f0
    { 224, 196 }, // -> 0x1bea -> 12f2
    { 240, 196 }, // -> 0x1bec -> 12f4
};

// { 128, 64 }, // -> 0x2a02 -> 12f6 -> orange disk falling

// Here it should be frameCoordinates_515C6 but I had to move it up

// Snik snak animations
Point frameCoordinates_51654[48] = {
    { 192, 388 }, // -> 0x2a5a -> 1388
    { 64, 260 }, // -> 0x3a56 -> 138a
    { 96, 244 }, // -> 0x32ba -> 138c
    { 80, 260 }, // -> 0x3a58 -> 138e
    { 208, 388 }, // -> 0x2a5c -> 1390
    { 96, 260 }, // -> 0x3a5a -> 1392
    { 48, 260 }, // -> 0x3a54 -> 1394
    { 112, 260 }, // -> 0x3a5c -> 1396
    { 192, 388 }, // -> 0x2a5a -> 1398
    { 112, 260 }, // -> 0x3a5c -> 139a
    { 48, 260 }, // -> 0x3a54 -> 139c
    { 96, 260 }, // -> 0x3a5a -> 139e
    { 208, 388 }, // -> 0x2a5c -> 13a0
    { 80, 260 }, // -> 0x3a58 -> 13a2
    { 96, 244 }, // -> 0x32ba -> 13a4
    { 64, 260 }, // -> 0x3a56 -> 13a6
    { 0, 424 }, // -> 0x3b6a -> 13a8
    { 16, 424 }, // -> 0x3b6c -> 13aa
    { 32, 424 }, // -> 0x3b6e -> 13ac
    { 48, 424 }, // -> 0x3b70 -> 13ae
    { 64, 424 }, // -> 0x3b72 -> 13b0
    { 80, 424 }, // -> 0x3b74 -> 13b2
    { 96, 424 }, // -> 0x3b76 -> 13b4
    { 112, 424 }, // -> 0x3b78 -> 13b6
    { 192, 228 }, // -> 0x2b26 -> 13b8
    { 224, 228 }, // -> 0x2b2a -> 13ba
    { 256, 228 }, // -> 0x2b2e -> 13bc
    { 288, 228 }, // -> 0x2b32 -> 13be
    { 0, 244 }, // -> 0x32ae -> 13c0
    { 32, 244 }, // -> 0x32b2 -> 13c2
    { 64, 244 }, // -> 0x32b6 -> 13c4
    { 96, 244 }, // -> 0x32ba -> 13c6
    { 144, 422 }, // -> 0x3a88 -> 13c8
    { 160, 422 }, // -> 0x3a8a -> 13ca
    { 176, 422 }, // -> 0x3a8c -> 13cc
    { 192, 422 }, // -> 0x3a8e -> 13ce
    { 208, 422 }, // -> 0x3a90 -> 13d0
    { 224, 422 }, // -> 0x3a92 -> 13d2
    { 240, 422 }, // -> 0x3a94 -> 13d4
    { 256, 422 }, // -> 0x3a96 -> 13d6
    { 128, 244 }, // -> 0x32be -> 13d8
    { 160, 244 }, // -> 0x32c2 -> 13da
    { 192, 244 }, // -> 0x32c6 -> 13dc
    { 224, 244 }, // -> 0x32ca -> 13de
    { 256, 244 }, // -> 0x32ce -> 13e0
    { 288, 244 }, // -> 0x32d2 -> 13e2
    { 0, 260 }, // -> 0x3a4e -> 13e4
    { 32, 260 }, // -> 0x3a52 -> 13e6
};

// Electron animations
Point frameCoordinates_516B4[48] = {
    { 0, 404 }, // -> 0x31e2 -> 13e8
    { 16, 404 }, // -> 0x31e4 -> 13ea
    { 32, 404 }, // -> 0x31e6 -> 13ec
    { 48, 404 }, // -> 0x31e8 -> 13ee
    { 64, 404 }, // -> 0x31ea -> 13f0
    { 80, 404 }, // -> 0x31ec -> 13f2
    { 96, 404 }, // -> 0x31ee -> 13f4
    { 112, 404 }, // -> 0x31f0 -> 13f6
    { 0, 404 }, // -> 0x31e2 -> 13f8
    { 112, 404 }, // -> 0x31f0 -> 13fa
    { 96, 404 }, // -> 0x31ee -> 13fc
    { 80, 404 }, // -> 0x31ec -> 13fe
    { 64, 404 }, // -> 0x31ea -> 1400
    { 48, 404 }, // -> 0x31e8 -> 1402
    { 32, 404 }, // -> 0x31e6 -> 1404
    { 16, 404 }, // -> 0x31e4 -> 1406
    { 144, 404 }, // -> 0x31f4 -> 1408
    { 160, 404 }, // -> 0x31f6 -> 140a
    { 176, 404 }, // -> 0x31f8 -> 140c
    { 192, 404 }, // -> 0x31fa -> 140e
    { 208, 404 }, // -> 0x31fc -> 1410
    { 224, 404 }, // -> 0x31fe -> 1412
    { 240, 404 }, // -> 0x3200 -> 1414
    { 256, 404 }, // -> 0x3202 -> 1416
    { 0, 372 }, // -> 0x22a2 -> 1418
    { 32, 372 }, // -> 0x22a6 -> 141a
    { 64, 372 }, // -> 0x22aa -> 141c
    { 96, 372 }, // -> 0x22ae -> 141e
    { 128, 372 }, // -> 0x22b2 -> 1420
    { 160, 372 }, // -> 0x22b6 -> 1422
    { 192, 372 }, // -> 0x22ba -> 1424
    { 224, 372 }, // -> 0x22be -> 1426
    { 0, 402 }, // -> 0x30ee -> 1428
    { 16, 402 }, // -> 0x30f0 -> 142a
    { 32, 402 }, // -> 0x30f2 -> 142c
    { 48, 402 }, // -> 0x30f4 -> 142e
    { 64, 402 }, // -> 0x30f6 -> 1430
    { 80, 403 }, // -> 0x3172 -> 1432
    { 96, 403 }, // -> 0x3174 -> 1434
    { 112, 402 }, // -> 0x30fc -> 1436
    { 256, 372 }, // -> 0x22c2 -> 1438
    { 288, 372 }, // -> 0x22c6 -> 143a
    { 0, 388 }, // -> 0x2a42 -> 143c
    { 32, 388 }, // -> 0x2a46 -> 143e
    { 64, 388 }, // -> 0x2a4a -> 1440
    { 96, 388 }, // -> 0x2a4e -> 1442
    { 128, 388 }, // -> 0x2a52 -> 1444
    { 160, 388 }, // -> 0x2a56 -> 1446
};

// Here it should be frameCoordinates_51714 but I had to move it up

uint16_t word_5177E = 0x0CAE; //  -> 0x129 -> (256, 164)
uint16_t word_51790 = 0x4A7E; //  -> 0x129 -> (288, 132)
uint16_t word_5182E = 0x2A64; // -> (272, 388)
uint16_t word_51840 = 0x2A06; //  -> 0x129 -> (160, 64)
uint16_t word_51842 = 0x132C; //  -> 0x129 -> (208, 16) confirmed
uint16_t word_51844 = 0x2A08; //  -> 0x129 -> (176, 64)
uint16_t word_51846 = 0x132A; //  -> 0x129 -> (192, 16)
uint16_t kTerminalOnSpriteCoordinates = 0x2A62; // word_51848 -> 0x1268 -> (256, 388) I don't get the math for this one, but the coordinates are right
uint16_t word_5184A = 0x2A66; //  -> 0x1268 -> (
uint16_t word_5184C = 0x2A67; //  -> 0x1268 -> (
uint16_t word_5184E = 0x2E36; //  -> 0x1268 -> (
uint16_t word_51850 = 0x2E37; //  -> 0x1268 -> (
uint16_t word_51852 = 0x2A68; //  -> 0x1268 -> (
uint16_t word_51854 = 0x2A69; //  -> 0x1268 -> (
uint16_t word_51856 = 0x2E38; //  -> 0x1268 -> (
uint16_t word_51858 = 0x2E39; //  -> 0x1268 -> (
uint16_t word_5195D = 0xF000; //  -> 0x1268 -> (
uint16_t gScrollOffsetX = 0; // word_5195F
uint16_t gScrollOffsetY = 0; // word_51961
int16_t gAdditionalScrollOffsetX = 0; // word_51963
int16_t gAdditionalScrollOffsetY = 0; // word_51965
uint16_t word_51967 = 0; // scroll / first pixel of the scroll window
uint16_t word_5196C = 0;
uint16_t gIsDebugModeEnabled = 0; // word_51970
uint16_t gShouldExitLevel = 0; // word_51974
uint16_t gQuitLevelCountdown = 0; // word_51978 -> this is a counter to end the level after certain number of iterations (to let the game progress a bit before going back to the menu)
uint16_t gShouldExitGame = 0; // word_5197A
uint16_t gIsMoveScrollModeEnabled = 0; // word_51A01
uint16_t gDebugExtraRenderDelay = 1; // this was used to add an extra delay in debug mode using keys 1-9
uint16_t word_58463 = 0;
uint16_t word_58465 = 0;
uint16_t word_58467 = 0;
uint16_t word_58469 = 0;
uint16_t word_5846B = 0;
uint16_t word_5846D = 0;
uint16_t word_5846F = 0;
uint16_t word_58471 = 0;
uint16_t word_58473 = 0;
uint16_t word_586FB = 0;
uint16_t word_586FD = 0;
uint16_t word_586FF = 0;
uint16_t word_58701 = 0;
uint16_t word_58703 = 0;
uint16_t word_5870D = 0;
uint16_t word_58710 = 0;
uint16_t word_58712 = 0;
uint16_t word_58714 = 0;
uint16_t word_58AB8 = 0x3231;
uint16_t word_58ABA = 0x3433;
uint16_t word_58AEA = 0x3030;
uint16_t word_58AEC = 0x0030;
uint16_t word_5988E = 0x4650;
uint16_t word_59891 = 0x3336;
uint16_t word_599D6 = 0;
uint16_t word_599D8 = 0;
uint16_t word_599DA = 0;
uint16_t word_59B6E = 0;
uint16_t word_59B73 = 0;
uint32_t dword_59B76 = 0;
uint16_t word_59B88 = 0;
uint16_t word_59B8A = 0;
uint16_t word_59B8C = 0;
uint16_t word_59B8E = 0;
uint16_t word_59B90 = 0;
uint16_t word_59B92 = 0;
uint16_t word_5A309 = 0x5053;
uint16_t word_5A30B = 0x1A0D;
uint16_t word_5A30D = 0;
uint16_t word_5A30F = 0;
uint8_t fileIsDemo = 0;
uint8_t isJoystickEnabled = 0; // byte_50940
uint8_t isMusicEnabled = 0; // byte_59886
uint8_t isFXEnabled = 0; // byte_59885
SDL_Scancode keyPressed = 0;

char gLevelsDatFilename[11] = "LEVELS.DAT";
char gLevelLstFilename[10] = "LEVEL.LST";
char gPlayerLstFilename[11] = "PLAYER.LST";
char gHallfameLstFilename[13] = "HALLFAME.LST";
char gDemo0BinFilename[10] = "DEMO0.BIN";
char gSavegameSavFilename[13] = "SAVEGAME.SAV";

// Game Speed in "Speed Fix values" from 0 to 10, but inverted. 10 is the
//   fastest, 0 the slowest. 5 means the original speed which was 35 game iterations per second.
//
#define kNumberOfGameSpeeds 11
const uint8_t kDefaultGameSpeed = 5;
uint8_t gGameSpeed = kDefaultGameSpeed; // gameSpeed
uint8_t demoFileName = 0; // Probably should be another type but whatever for now
uint8_t gIsFlashingBackgroundModeEnabled = 0; // flashingbackgroundon
const float kSpeedTimeFactors[kNumberOfGameSpeeds] = { 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.5, 1.0 / 3.0, 1.0 / 4.0, 1.0 / 5.0, 0.0 };

// Used to measure display frame rate
float gFrameRate = 0.f;
Uint32 gFrameRateReferenceTime = 0;

// Used to measure game speed (reference is 35 iterations per second)
float gGameIterationRate = 0.f;
Uint32 gGameIterationRateReferenceTime = 0;

// Used to limit game speed
Uint32 gGameIterationStartTime = 0;
Uint32 gNumberOfGameIterations = 0;

// Time difference between 2 consecutive renders
Uint32 gRenderDeltaTime = 0;

static const uint16_t kFallAnimationGravityOffsets[18] = {
    0x0000, // -> 0x6C95
    0x007A, // -> 0x6C97
    0x00F4, // -> 0x6C99
    0x016E, // -> 0x6C9B
    0x01E8, // -> 0x6C9D
    0x0262, // -> 0x6C9F
    0x02DC, // -> 0x6CA1
    0x0356, // -> 0x6CA3
    0x03D0, // -> 0x6CA5
    0x044A, // -> 0x6CA7
    0x04C4, // -> 0x6CA9
    0x053E, // -> 0x6CAB
    0x05B8, // -> 0x6CAD
    0x0632, // -> 0x6CAF
    0x06AC, // -> 0x6CB1
    0x0726, // -> 0x6CB3
    0x07A0, // -> 0x6CB5
    0x081A, // -> 0x6CB7
};

// This points to the address on the screen where the mouse cursor was
// drawn the last time, used to clear the cursor before redrawing it again
//
uint16_t gLastMouseCursorOriginAddress = 0; // word_5847B

// This buffer will hold the bitmap of the area where the cursor is going to be drawn
// with the intention of restoring that area before rendering the mouse in a new
// position, hence clearing the trail the mouse would leave.
//
#define kLastMouseCursorAreaSize 8
uint8_t gLastMouseCursorAreaBitmap[kLastMouseCursorAreaSize * kLastMouseCursorAreaSize];

static const int kConfigDataLength = 4;

uint16_t gRandomGeneratorSeed = 0;

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

#define kPlayerNameLength 8

char a00s0010_sp[12] = "00S001$0.SP";
char gPlayerName[kPlayerNameLength + 1] = "WIBBLE  "; // 0x879F // TODO: read this address when the game starts and put the same text here

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
uint16_t gLevelData[kLevelSize];
uint16_t gMurphyLocation = 0;

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
uint8_t gPaddedLevelListData[kNumberOfLevelsWithPadding * kListLevelNameLength];

#define kLevelListDataLength (kNumberOfLevels * kListLevelNameLength)
uint8_t *gLevelListData = &gPaddedLevelListData[kFirstLevelIndex * kListLevelNameLength];

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

char gCurrentLevelName[kListLevelNameLength]; // 0x87A8

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

#define kMaxDemoInputSteps 48648

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

// This struct defines the demo format of the game after the speed fix mods (which includes a demo of the original
// format inside).
//
typedef struct
{
    Level level;
    BaseDemo baseDemo;
    uint8_t signature[kMaxDemoSignatureLength + 1]; // text that ends with 0xFF
} DemoFile;

#define kNumberOfDemos 10

typedef struct
{
    uint16_t demoFirstIndices[kNumberOfDemos + 1]; // index of the last byte of all demos (starting at demo-segment:0000). there are 11 words because the end of this "list" is marked with 0xFFFF
    BaseDemo baseDemo;
    Level level[kNumberOfDemos];
} Demos;

uint16_t gDemoRandomSeeds[kNumberOfDemos];

Demos gDemos; // this is located in the demo segment, starting at 0x0000
char gCurrentDemoLevelIdentifier[4] = ".SP"; // 0x87DA
char gCurrentDemoLevelName[kListLevelNameLength] = "??? ----- DEMO LEVEL! -----"; // 0x87DE

// fileLevelData starts at 0x768, when it contains a level goes to 0xD67
Level gCurrentLevel; // 0x988B

typedef struct {
    uint8_t tile; // of LevelTileType
    uint8_t movingObject;
} MovingLevelTile;

// Stores the same info as gCurrentLevel but with each byte separated by 0x00, it's done in readLevels for some unknown reason
MovingLevelTile gCurrentLevelWord[levelDataLength]; // 0x1834
// And this is initialized to 0 in readLevels, and in memory it's supposed to exist right after gCurrentLevelWord
Level gCurrentLevelAfterWord; // 0x2434

uint16_t kOriginalDemoFileSizes[10] = { // word_599DC
    0x00CE, 0x016A, 0x0146, 0x00CD, 0x024D,
    0x012C, 0x01A7, 0x01FB, 0x01D2, 0x02FD
};

typedef struct {
    uint8_t levelNumber;
    uint8_t firstUserInputs[3];
} FirstOriginalDemoFileChunk;

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

SoundType sndType = SoundTypeNone;
SoundType musType = SoundTypeInternalStandard;
uint8_t soundEnabled = 0;

#define kNumberOfColors 16
#define kPaleteDataSize (kNumberOfColors * 4)
#define kNumberOfPalettes 4

typedef SDL_Color ColorPalette[kNumberOfColors];
typedef uint8_t ColorPaletteData[kPaleteDataSize];

#define kBitmapFontCharacterHeight 7
#define kBitmapFontCharacter6Width 6
#define kBitmapFontCharacter8Width 8
#define kNumberOfCharactersInBitmapFont 64
#define kBitmapFontLength (kNumberOfCharactersInBitmapFont * 8) // The size of the bitmap is a round number, not 100% related to the size font, there is some padding :shrug:

uint8_t gChars6BitmapFont[kBitmapFontLength];
uint8_t gChars8BitmapFont[kBitmapFontLength];

// This is a 320x24 bitmap
#define kPanelBitmapWidth 320
#define kPanelBitmapHeight 24

uint8_t gPanelDecodedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gPanelRenderedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gCurrentPanelHeight = kPanelBitmapHeight;

#define kFullScreenBitmapLength (kScreenWidth * kScreenHeight / 2) // They use 4 bits to encode pixels

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

#define kFullScreenFramebufferLength (kScreenWidth * kScreenHeight) // We only use 16 colors, but SDL doesn't support that mode, so we use 256 colors

// This buffer has the contents of TITLE2.DAT after it's been "decoded" (i.e. after picking the different channels
// every 40 bytes and forming the 4 bit palette index for each pixel).
//
uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];

uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];

#define kLevelEdgeSize 8
#define kTileSize 16
#define kLevelBitmapWidth (kTileSize * (kLevelWidth - 2) + kLevelEdgeSize + kLevelEdgeSize)
#define kLevelBitmapHeight (kTileSize * (kLevelHeight - 2) + kLevelEdgeSize + kLevelEdgeSize)
uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

typedef struct
{
    char playerName[kPlayerNameLength + 1];
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} HallOfFameEntry;

#define kNumberOfHallOfFameEntries 3
HallOfFameEntry gHallOfFameData[kNumberOfHallOfFameEntries]; // 0x9514 -> asc_59824

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
PlayerEntry gPlayerListData[kNumberOfPlayers]; // 0x8A9C -> word_58DAC

char gRankingTextEntries[kNumberOfPlayers + 4][23] = { //0x880E
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
void handleDemoOptionClick(void);
void handleSkipLevelOptionClick(void);
void handleFloppyDiskButtonClick(void);
void handleDeletePlayerOptionClick(void);
void handleStatisticsOptionClick(void);
void handleControlsOptionClick(void);
void handleOkButtonClick(void);

#define kNumberOfMainMenuButtons 17
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
        handleDemoOptionClick, // Demo
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

#define kNumberOfOptionsMenuButtons 13
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

#define kNumberOfOptionsMenuBorders 20
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

#define kNumberOfMainMenuButtonBorders 12
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

typedef void (*MovingFunction)(uint16_t);
typedef void (*FrameBasedMovingFunction)(uint16_t, uint8_t);

void updateZonkTiles(uint16_t position);
void updateInfotronTiles(uint16_t position);
void updateOrangeDiskTiles(uint16_t position);
void updateSnikSnakTiles(uint16_t position);
void updateTerminalTiles(uint16_t position);
void updateElectronTiles(uint16_t position);
void updateBugTiles(uint16_t position);
void updateExplosionTiles(uint16_t position);

static const MovingFunction movingFunctions[32] = {
    NULL,
    updateZonkTiles,
    NULL,
    NULL,
    updateInfotronTiles,
    NULL,
    NULL,
    NULL,
    updateOrangeDiskTiles,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    updateSnikSnakTiles,
    NULL,
    updateTerminalTiles,
    NULL,
    NULL,
    NULL,
    NULL,
    updateElectronTiles,
    updateBugTiles,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    updateExplosionTiles,
};

void updateElectronTurnLeft(uint16_t position, uint8_t frame);
void updateElectronTurnRight(uint16_t position, uint8_t frame);
void updateElectronMovementUp(uint16_t position, uint8_t frame);
void updateElectronMovementDown(uint16_t position, uint8_t frame);
void updateElectronMovementRight(uint16_t position, uint8_t frame);
void updateElectronMovementLeft(uint16_t position, uint8_t frame);

static const FrameBasedMovingFunction kElectronMovingFunctions[] = {
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnLeft,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronTurnRight,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementUp,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementDown,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementRight,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
    updateElectronMovementLeft,
};

void updateSnikSnakTurnLeft(uint16_t position, uint8_t frame);
void updateSnikSnakTurnRight(uint16_t position, uint8_t frame);
void updateSnikSnakMovementUp(uint16_t position, uint8_t frame);
void updateSnikSnakMovementLeft(uint16_t position, uint8_t frame);
void updateSnikSnakMovementDown(uint16_t position, uint8_t frame);
void updateSnikSnakMovementRight(uint16_t position, uint8_t frame);

static const FrameBasedMovingFunction kSnikSnakMovingFunctions[48] = {
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnLeft,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakTurnRight,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementUp,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementLeft,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementDown,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
    updateSnikSnakMovementRight,
};

SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
SDL_Window *gWindow = NULL;
SDL_Rect gWindowViewport;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;
SDL_Surface *gTextureSurface = NULL;

#define kFixedBitmapWidth 640
#define kFixedBitmapHeight 16
uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];

#define kMovingBitmapWidth 320
#define kMovingBitmapHeight 462
uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];

#define kSoundBufferSize (40 * 1024) // 40KB
uint8_t gSoundBuffer1[kSoundBufferSize];
uint8_t gSoundBuffer2[kSoundBufferSize];

// registers to prevent compiler errors
uint8_t cf;
uint8_t ah, al, bh, bl, ch, cl, dh, dl;
uint16_t ax, bx, cx, dx, ds, cs, es, bp, sp, di, si;

void startTrackingRenderDeltaTime(void);
uint32_t updateRenderDeltaTime(void);
void updateWindowViewport(void);
int windowResizingEventWatcher(void* data, SDL_Event* event);
void emulateClock(void);
void handleSDLEvents(void);
void replaceCurrentPaletteColor(uint8_t index, SDL_Color color);
void setPalette(ColorPalette palette);
void fadeToPalette(ColorPalette palette);
void readTitleDatAndGraphics(void);
void videoloop(void);
void convertPaletteDataToPalette(ColorPaletteData paletteData, ColorPalette outPalette);
void loadScreen2(void);
void readEverything(void);
void exitWithError(const char *format, ...);
char characterForSDLScancode(SDL_Scancode scancode);
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
void prepareSomeKindOfLevelIdentifier(void);
void runMainMenu(void);
void convertNumberTo3DigitPaddedString(uint8_t number, char numberString[3], char useSpacesForPadding);
void sound1(void);
void sound2(void);
void sound3(void);
void sound4(void);
void sound5(void);
void sound6(void);
void sound7(void);
void sound8(void);
void sound9(void);
void sound10(void);
void sound11(void);
void savePlayerListData(void);
void saveHallOfFameData(void);
void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus);
void drawMainMenuButtonBorders(void);
void drawMainMenuButtonBorder(ButtonBorderDescriptor border, uint8_t color);
uint8_t waitForJoystickKeyReleased(uint8_t keyOrAxis, uint16_t *outTime); // sub_49FED
void generateRandomSeedFromClock(void);
void initializeFadePalette(void);
void initializeMouse(void);
void loadMurphySprites(void);
void prepareLevelDataForCurrentPlayer(void);
void drawPlayerList(void);
void drawLevelList(void);
void drawHallOfFame(void);
void saveLastMouseAreaBitmap(void);
void restoreLastMouseAreaBitmap(void);
void drawMouseCursor(void);
void drawRankings(void);
void drawTextWithChars6FontWithOpaqueBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars6FontWithTransparentBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8FontToBuffer(uint8_t *buffer, size_t destX, size_t destY, uint8_t color, const char *text);
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
void initializeGameInfo(void);
void drawFixedLevel(void);
void drawGamePanel(void);
void drawNumberOfRemainingInfotrons(void);
void drawGameTime(void);
void sub_4A2E6(void);
void resetNumberOfInfotrons(void);
void findMurphy(void);
void drawGamePanelText(void);
void sub_4A291(void);
void drawMovingFrame(uint16_t srcX, uint16_t srcY, uint16_t destPosition);
void runLevel(void);
void slideDownGameDash(void);
void updateScrollOffset(void);
uint16_t generateRandomNumber(void);
void handleGameUserInput(void);
void loc_49C41(void);
void loc_49C2C(char text[3]);
void loc_49C28(void);
void loc_49A89(void);
void loc_49949(void);
void loc_4988E(void);
void levelScanThing(void);
void updateMovingObjects(void);
uint16_t updateMurphy(uint16_t position);
uint16_t updateMurphyAnimation(uint16_t position);
uint16_t updateMurphyAnimationInfo(uint16_t position, MurphyAnimationDescriptor unknownMurphyData);
uint16_t handleMurphyDirectionRight(uint16_t position);
uint16_t handleMurphyDirectionDown(uint16_t position);
uint16_t handleMurphyDirectionLeft(uint16_t position);
uint16_t handleMurphyDirectionUp(uint16_t position);
void detonateYellowDisks(void);
void updateUserInputInScrollMovementMode(void);
void updateUserInput(void);
void sub_492F1(void);
void simulateDemoInput(void);
void sub_4A463(void);
void removeTiles(LevelTileType tileType);
void sub_4A3E9(void);
void sub_4945D(void);
void somethingspsig(void);
void sub_4A3D2(void);
void sub_49D53(void);
void drawNumberOfRemainingRedDisks(void);
void clearNumberOfRemainingRedDisks(void);
void updatePlantedRedDisk(void);
void sub_4A5E0(void);
void sub_4A95F(void);
void drawFailedLevelResultScreen(void);
void handleZonkStateAfterFallingOneTile(uint16_t position);
void detonateBigExplosion(uint16_t position);
void detonateZonk(uint16_t position, uint8_t movingObject, uint8_t tile);
void sub_4AA34(uint16_t position, uint8_t movingObject, uint8_t tile);
void sub_4AAB4(uint16_t position);
uint8_t checkMurphyMovementToPosition(uint16_t position);
void handleZonkPushedByMurphy(uint16_t position);
void decreaseRemainingRedDisksIfNeeded(uint16_t position);
void updateSpecialPort(uint16_t position);
void handleInfotronStateAfterFallingOneTile(uint16_t position);
void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawCurrentLevelViewport(uint16_t panelHeight);
void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, uint16_t dstX, uint16_t dstY);

#ifdef __vita__
static const int kWindowWidth = kScreenWidth;
static const int kWindowHeight = kScreenHeight;
#else
static const int kWindowWidth = kScreenWidth * 4;
static const int kWindowHeight = kScreenHeight * 4;
#endif

//         public start
int main(int argc, const char * argv[])
{
#if DEBUG
    gShouldShowFPS = 1;
#endif

    initializeLogging();

    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
    if (ret)
    {
        spLog("SDL_Init failed with %d", ret);
        exit(1);
    }

    gWindow = SDL_CreateWindow("OpenSupaplex",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               kWindowWidth,
                               kWindowHeight,
#if defined(__SWITCH__) || defined(__vita__)
                               SDL_WINDOW_FULLSCREEN);
#else
                               0);
#endif

    if (gWindow == NULL)
    {
        spLog("Could not create a window: %s", SDL_GetError());
        return -1;
    }

    SDL_SetWindowResizable(gWindow, SDL_TRUE);
    SDL_AddEventWatch(windowResizingEventWatcher, gWindow);

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);

    gTexture = SDL_CreateTexture(gRenderer,
                                             SDL_PIXELFORMAT_ARGB32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             kScreenWidth, kScreenHeight);

    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_ARGB32);

    gScreenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);
    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    handleSDLEvents();

    updateWindowViewport();

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
//
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
//
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
        FILE *file = openReadonlyFile(demoFileName, 'r');
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
        int success = getLevelNumberFromOriginalDemoFile(file, fileLength);
        word_599DA = fileLength;
        pushf();
        ah = 0x3E;
        fclose(file);
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        popf();
        if (success)
        {
            goto loc_46CF6; // jnb short loc_46CF6  // the flag CF to check the jnb comes from the getLevelNumberFromOriginalDemoFile result (CF=0 success, CF=1 error);
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

errorReadingDemoFile:              //; CODE XREF: start+74j start+A2j ...
        *demoFileName = 0;
        byte_599D4 = 0;

loc_46D13:              //; CODE XREF: start+E7j
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        goto processCommandLine;

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

demoFileNotMissing:              //; CODE XREF: start+10Bj
        if (fileIsDemo == 1)
        {
            goto spHasAtAndDemo;
        }
        si = *aSpWithoutDemo; //"SP without demo: "
        conprintln();
        goto immediateexit;

spHasAtAndDemo:              //; CODE XREF: start+11Cj
        fastMode = 1;
        gRemainingSpeedTestAttempts = 0

runSpFile:              //; CODE XREF: start+104j
        push(di);
        push(cx);
        al = 0x41h; // 'A'
        goto loc_46D5A;

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
        gIsDebugModeEnabled = 1;

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
        byte_59B85 = 0x0FF; // TODO: recheck switching levelsets after understanding this variable
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
//
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
    generateRandomSeedFromClock();
    // checkVideo();

//leaveVideoStatus:           //; CODE XREF: start+28Aj
    initializeFadePalette(); // 01ED:026F
    initializeMouse();
//    initializeSound();
    if (fastMode == 0)
    {
        setPalette(gBlackPalette);
        readTitleDatAndGraphics();
        ColorPalette titleDatPalette; // si = 0x5F15;
        convertPaletteDataToPalette(gTitlePaletteData, titleDatPalette);
        fadeToPalette(titleDatPalette);
    }

//isFastMode:              //; CODE XREF: start+2ADj
    loadMurphySprites(); // 01ED:029D
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
            initializeGameInfo();
            drawFixedLevel();
            drawGamePanel(); // 01ED:0311
            sub_4A2E6();
            resetNumberOfInfotrons();
            findMurphy();
            gCurrentPanelHeight = kPanelBitmapHeight;
            drawCurrentLevelViewport(gCurrentPanelHeight); // Added by me
//            si = 0x6015;
            fadeToPalette(gPalettes[1]); // At this point the screen fades in and shows the game

            if (isMusicEnabled == 0)
            {
                sound3();
            }

//loc_46F77:              //; CODE XREF: start+352j
            byte_5A33F = 1;
            runLevel();
            byte_599D4 = 0;
            if (gShouldExitGame != 0)
            {
                break; // goto loc_47067;
            }

//loc_46F8E:              //; CODE XREF: start+369j
            if (fastMode == 1)
            {
//                goto doneWithDemoPlayback;
            }

//isNotFastMode2:              //; CODE XREF: start+373j
            slideDownGameDash(); // 01ED:0351
            if (byte_59B71 != 0)
            {
                loadMurphySprites();
            }

//loc_46FA5:              //; CODE XREF: start+380j
            byte_5A33F = 0;
            if (gShouldExitGame != 0)
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
        prepareSomeKindOfLevelIdentifier(); // 01ED:037A

        uint8_t shouldDoSomething = 1; // still no idea what this is

        if (byte_599D4 == 2)
        {
//            goto loc_46FFF; <- if byte_599D4 != 2

            byte_599D4 = 1;
            if (fileIsDemo == 1)
            {
//              ax = 0;
//              playDemo();
            }
            else
            {
//loc_46FDF:              //; CODE XREF: start+3B5j
                gIsPlayingDemo = 0;
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
            gIsPlayingDemo = 0;
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
            gShouldAutoselectNextLevelToPlay = 1;
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
    while (gShouldExitGame == 0);

//loc_47067:              //; CODE XREF: start+36Bj start+391j ...
        fadeToPalette(gBlackPalette); // 0x60D5
/*
doneWithDemoPlayback:           //; CODE XREF: start+375j
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
//
loc_4708E:              //; CODE XREF: start+466j
        si = *aDemoFailed; //"Demo failed:     "
        goto printMessageAfterward;
//
loc_47094:              //; CODE XREF: start+45Fj
        if (byte_5A19B == 0)
        {
            goto loc_470A1;
        }
        si = *a@ErrorLevel?Su; // "\"@\"-ERROR: Level(?) successful: "
        goto printMessageAfterward;
//
loc_470A1:              //; CODE XREF: start+479j
        si = *a@ErrorLevel?Fa; // "\"@\"-ERROR: Level(?) failed:     "

printMessageAfterward:          //; CODE XREF: start+46Cj start+472j ...
        conprintln();

isNotFastMode3:              //; CODE XREF: start+458j
        soundShutdown?();

immediateexit:              //; CODE XREF: start+D1j start+114j ...
        exit(0);
//         mov ax, 4C00h
//         int 21h     ; DOS - 2+ - QUIT WITH EXIT CODE (EXIT)
// ; END OF FUNCTION CHUNK FOR start   ; AL = exit code


// ; START OF FUNCTION CHUNK FOR loadScreen2

exit:                   //; CODE XREF: readConfig:loc_474BBj
*/

        // Tidy up
        SDL_DelEventWatch(windowResizingEventWatcher, gWindow);
        SDL_FreeSurface(gTextureSurface);
        SDL_DestroyTexture(gTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        destroyLogging();
        SDL_Quit();

        return 0;

//    //; readConfig+3Bj ...
//        push(ax);
//        soundShutdown?();
//        resetVideoMode();
//        pop(ax);
//        push(ax);
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
    size_t bytes = fread(&firstChunk, 1, sizeof(firstChunk), file);

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

void slideDownGameDash() // proc near     ; CODE XREF: start:isNotFastMode2p
{
    // 01ED:04ED
    if ((word_510C1 & 0xFF) == 0)
    {
//loc_4715C:              ; CODE XREF: crt?2+5j
//        cx = 0x90; // 144
        return;
    }

    for (int panelHeight = kPanelBitmapHeight; panelHeight > 0; --panelHeight)
    {
        // This code probably keeps the scroll in valid values
        // bx = word_51967;
        // if (bx > 0x4DAE)
        // {
        //     bx -= 122;
        //     word_51967 = bx;
        // }

        // Move the bottom panel line by line to the bottom of the screen
        for (int y = kScreenHeight - 2; y >= kScreenHeight - panelHeight; --y)
        {
            uint16_t srcAddress = y * kScreenWidth;
            uint16_t dstAddress = (y + 1) * kScreenWidth;
            memcpy(&gScreenPixels[dstAddress], &gScreenPixels[srcAddress], kScreenWidth);
        }

        drawCurrentLevelViewport(panelHeight);
        videoloop();
    }
}

/// This alternative int9 handler seems to control the keys +, -, *, / in the numpad
/// to alter the game speed, and also the key X for something else.
/// @param shouldYieldCpu If 1, will sleep the thread for a bit to prevent 100% CPU usage.
void int9handler(uint8_t shouldYieldCpu) // proc far        ; DATA XREF: setint9+1Fo
{
    handleSDLEvents();

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

    gIsEscapeKeyPressed = keys[SDL_SCANCODE_ESCAPE];
    gIsSpaceKeyPressed = keys[SDL_SCANCODE_SPACE];
    gIsUpKeyPressed = keys[SDL_SCANCODE_UP];
    gIsDownKeyPressed = keys[SDL_SCANCODE_DOWN];
    gIsLeftKeyPressed = keys[SDL_SCANCODE_LEFT];
    gIsRightKeyPressed = keys[SDL_SCANCODE_RIGHT];
    gIsNumpad0Pressed = keys[SDL_SCANCODE_KP_0];
    gIsNumpad1Pressed = keys[SDL_SCANCODE_KP_1];
    gIsNumpad3Pressed = keys[SDL_SCANCODE_KP_3];
    gIsNumpad5Pressed = keys[SDL_SCANCODE_KP_5];
    gIsNumpad7Pressed = keys[SDL_SCANCODE_KP_7];
    gIsNumpad9Pressed = keys[SDL_SCANCODE_KP_9];
    gIsNumpadPeriodPressed = keys[SDL_SCANCODE_KP_PERIOD];
    gIsNumpadDividePressed = keys[SDL_SCANCODE_KP_DIVIDE];
    gIsNumLockPressed = keys[SDL_SCANCODE_NUMLOCKCLEAR];
    gIsScrollLockPressed = keys[SDL_SCANCODE_SCROLLLOCK];
    gIsLeftAltPressed = keys[SDL_SCANCODE_LALT];
    gIsLeftShiftPressed = keys[SDL_SCANCODE_LSHIFT];
    gIsRightShiftPressed = keys[SDL_SCANCODE_RSHIFT];
    gIsEnterPressed = keys[SDL_SCANCODE_RETURN];
    gIsLeftControlPressed = keys[SDL_SCANCODE_LCTRL];
    gIs1KeyPressed = keys[SDL_SCANCODE_1];
    gIs2KeyPressed = keys[SDL_SCANCODE_2];
    gIs3KeyPressed = keys[SDL_SCANCODE_3];
    gIs4KeyPressed = keys[SDL_SCANCODE_4];
    gIs5KeyPressed = keys[SDL_SCANCODE_5];
    gIs6KeyPressed = keys[SDL_SCANCODE_6];
    gIs7KeyPressed = keys[SDL_SCANCODE_7];
    gIs8KeyPressed = keys[SDL_SCANCODE_8];
    gIs9KeyPressed = keys[SDL_SCANCODE_9];
    gIs0KeyPressed = keys[SDL_SCANCODE_0];
    gIsMinusKeyPressed = keys[SDL_SCANCODE_MINUS];
    gIsEqualsKeyPressed = keys[SDL_SCANCODE_EQUALS];
    gIsQKeyPressed = keys[SDL_SCANCODE_Q];
    gIsWKeyPressed = keys[SDL_SCANCODE_W];
    gIsRKeyPressed = keys[SDL_SCANCODE_R];
    gIsPKeyPressed = keys[SDL_SCANCODE_P];
    gIsSKeyPressed = keys[SDL_SCANCODE_S];
    gIsDKeyPressed = keys[SDL_SCANCODE_D];
    gIsHKeyPressed = keys[SDL_SCANCODE_H];
    gIsJKeyPressed = keys[SDL_SCANCODE_J];
    gIsLKeyPressed = keys[SDL_SCANCODE_L];
    gIsZKeyPressed = keys[SDL_SCANCODE_Z];
    gIsCKeyPressed = keys[SDL_SCANCODE_C];
    gIsBKeyPressed = keys[SDL_SCANCODE_B];
    gIsMKeyPressed = keys[SDL_SCANCODE_M];
    gIsF1KeyPressed = keys[SDL_SCANCODE_F1];
    gIsF2KeyPressed = keys[SDL_SCANCODE_F2];
    gIsF3KeyPressed = keys[SDL_SCANCODE_F3];
    gIsF4KeyPressed = keys[SDL_SCANCODE_F4];
    gIsF5KeyPressed = keys[SDL_SCANCODE_F5];
    gIsF6KeyPressed = keys[SDL_SCANCODE_F6];
    gIsF7KeyPressed = keys[SDL_SCANCODE_F7];
    gIsF8KeyPressed = keys[SDL_SCANCODE_F8];
    gIsF9KeyPressed = keys[SDL_SCANCODE_F9];
    gIsF10KeyPressed = keys[SDL_SCANCODE_F10];

    // 01ED:0659
    if (keyPressed == SDL_SCANCODE_UNKNOWN) //test    cl, 80h     ; think key up
    {
        if (shouldYieldCpu)
        {
            SDL_Delay(10); // Avoid burning the CPU
        }
        return;
    }

    if (gIsLeftAltPressed && gIsEnterPressed)
    {
        uint8_t isFullscreen = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN;
        SDL_SetWindowFullscreen(gWindow, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    }

//storeKey:               ; CODE XREF: int9handler+2Bj
    if (keyPressed == SDL_SCANCODE_KP_MULTIPLY) // Key * in the numpad, restore speed
    {
        gGameSpeed = kDefaultGameSpeed;
    }
//checkSlash:             ; CODE XREF: int9handler+3Ej
//                ; int9handler+45j
    else if (keyPressed == SDL_SCANCODE_KP_DIVIDE) // Keypad / -> fastest playback seed
    {
        gGameSpeed = kNumberOfGameSpeeds - 1;
    }
//checkPlus:              ; CODE XREF: int9handler+54j
    else if (keyPressed == SDL_SCANCODE_KP_PLUS) // Key + in the numpad, speed up
    {
        if (gGameSpeed < kNumberOfGameSpeeds - 1)
        {
            gGameSpeed++;
        }
    }
//checkMinus:             ; CODE XREF: int9handler+65j
//                ; int9handler+71j
    else if (keyPressed == SDL_SCANCODE_KP_MINUS) // Key - in the numpad, speed down
    {
        if (gGameSpeed != 0)
        {
            gGameSpeed--;
        }
    }

//checkX:                 ; CODE XREF: int9handler+39j
//                ; int9handler+60j ...
    if (keyPressed == SDL_SCANCODE_X // Key X
        && gIsLeftAltPressed != 0)
    {
        gShouldExitLevel = 1;
        gShouldExitGame = 1;
    }

    if (shouldYieldCpu)
    {
        SDL_Delay(10); // Avoid burning the CPU
    }
}

void int8handler() // proc far        ; DATA XREF: setint8+10o
{
    if (gIsGamePaused != 0)
    {
        gAuxGameSeconds20msAccumulator++;
        if (gAuxGameSeconds20msAccumulator >= 50)
        {
            gAuxGameSeconds20msAccumulator = 0;
            gGameSeconds++;
            if (gGameSeconds >= 60)
            {
                gGameSeconds = 0;
                gGameMinutes++;
                if (gGameMinutes >= 60)
                {
                    gGameMinutes = 0;
                    gGameHours++;
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
        return; //iret
    }
}

void readConfig() //  proc near       ; CODE XREF: start:loc_46F0Fp
{
    FILE *file = openWritableFile("SUPAPLEX.CFG", "r");
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
}

void saveConfiguration() // sub_4755A      proc near               ; CODE XREF: code:loc_4CAECp
{
    FILE *file = openWritableFile("SUPAPLEX.CFG", "w");
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

/// @return Number of demo files read
uint8_t readDemoFiles() //    proc near       ; CODE XREF: readEverything+12p
                  //  ; handleDemoOptionClickp ...
{
    // 01ED:09A6

    word_510DF = 22;
    word_5A33C = 22;

    uint8_t *demosAsByteArray = (uint8_t *)&gDemos;

    memset(&gDemos.demoFirstIndices, 0xFF, 22); // rep stosw // fills 11 words (22 bytes) with 0xFFFF

    for (int i = 0; i < kNumberOfDemos; ++i)
    {
//loc_47629:              //; CODE XREF: readDemoFiles+175j
        word_599D8 = 0;
        if (byte_599D4 == 1)
        {
            // TODO: Implement loading a demo from command line
            // dx = &demoFileName;
        }
        else
        {
//loc_4763C:             // ; CODE XREF: readDemoFiles+2Cj
            gDemo0BinFilename[4] = '0' + i; // Replaces the number in "DEMO0.BIN" with the right value
        }

//loc_47647:             // ; CODE XREF: readDemoFiles+31j
        FILE *file = openReadonlyFile(gDemo0BinFilename, "r");
        if (file == NULL)
        {
            return i;
        }

//loc_47651:              //; CODE XREF: readDemoFiles+43j
        if (byte_599D4 == 1)
        {
            if (word_599DA == 0)
            {
                fseek(file, levelDataLength, SEEK_SET);
            }
        }
        else
        {
//loc_47674:             // ; CODE XREF: readDemoFiles+52j
            int result = fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);

            // this is probably to support old level formats
            if (result == 0
                && fileSize < levelDataLength)
            {
                word_599D8 = getLevelNumberFromOriginalDemoFile(file, fileSize);
            }

//loc_47690:             // ; CODE XREF: readDemoFiles+76j readDemoFiles+7Aj ...
            fseek(file, 0, SEEK_SET);

            if (word_599D8 == 0)
            {
                Level *level = &gDemos.level[i];
                size_t bytes = fread(level, 1, levelDataLength, file);

                if (bytes < levelDataLength)
                {
                    return i;
                }

//loc_476D3:           //   ; CODE XREF: readDemoFiles+C5j
                gDemoRandomSeeds[i] = level->randomSeed;
            }
        }

//loc_476DB:             // ; CODE XREF: readDemoFiles+59j readDemoFiles+69j ...
        uint16_t maxNumberOfBytesToRead = kMaxDemoInputSteps + 1; // 48649
        maxNumberOfBytesToRead -= word_510DF;

        if (maxNumberOfBytesToRead > kMaxDemoInputSteps + 1) // weird way of checking if word_510DF < 0 ????
        {
            maxNumberOfBytesToRead = 0;
        }

        uint16_t numberOfDemoBytesRead = 0;

//loc_476EA:             // ; CODE XREF: readDemoFiles+DDj
        if (maxNumberOfBytesToRead == 0)
        {
            numberOfDemoBytesRead = 0;
        }
        else
        {
//loc_476F3:              // ; CODE XREF: readDemoFiles+E4j
            // TODO: that word_510DF feels wrong
            numberOfDemoBytesRead = fread(&demosAsByteArray[word_510DF], 1, maxNumberOfBytesToRead, file);

            if (numberOfDemoBytesRead == 0)
            {
                ax = 0;
                if (fclose(file) != 0)
                {
                    exitWithError("Error closing DEMO file");
                }
                return i;
            }

//loc_47719:             // ; CODE XREF: readDemoFiles+FCj
        }

//loc_4771A:             // ; CODE XREF: readDemoFiles+E8j
        if (fclose(file) != 0)
        {
            exitWithError("Error closing DEMO file");
        }

//loc_47729:              ; CODE XREF: readDemoFiles+11Bj
        // pop(ax)
        bx = word_510DF;
        demosAsByteArray[word_510DF] = demosAsByteArray[word_510DF] & 0x7F; // this removes the MSB from the levelNumber that was added in the speed fix mods
        int isZero = (word_599D8 == 0);
        word_599D8 = 0;
        if (isZero)
        {
            demosAsByteArray[word_510DF] = demosAsByteArray[word_510DF] | 0x80; // This sets the MSB?? maybe the "interpreter" later needs it
        }

//loc_47743:             // ; CODE XREF: readDemoFiles+134j
        uint16_t demoLastByteIndex = word_510DF + numberOfDemoBytesRead - 1;
        // cx = bx; // bx here has the value of word_510DF
        // bx += numberOfDemoBytesRead; // ax here has the number of bytes read regarding the level itself (levelNumber + inputSteps)
        // push(ds);
        // push(es);
        // pop(ds);
        // assume ds:nothing
        // bx--;
        if (demoLastByteIndex == 0xFFFF // this would mean bx was 0. is this possible?
            || numberOfDemoBytesRead <= 1 // this means the demo is empty (only has levelNumber or nothing)
            || demosAsByteArray[demoLastByteIndex] != 0xFF)
        {
//loc_4775A:             // ; CODE XREF: readDemoFiles+145j
           // ; readDemoFiles+14Aj
            if (demoLastByteIndex < sizeof(BaseDemo))
            {
                numberOfDemoBytesRead++;
                demosAsByteArray[demoLastByteIndex + 1] = 0xFF;
            }
        }

//loc_47765:             // ; CODE XREF: readDemoFiles+14Fj
                   // ; readDemoFiles+155j
        gDemos.demoFirstIndices[i] = word_510DF;
        word_510DF += numberOfDemoBytesRead;
    }

    return kNumberOfDemos;
}

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
    FILE *file = openReadonlyFile("PALETTES.DAT", "r");
    if (file == NULL)
    {
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
}

void openCreditsBlock() // proc near      ; CODE XREF: start+2E9p
{
    static const int kEdgeWidth = 13;
    static const int kEdgeHeight = 148;
    static const int kEdgeStep = 4;
    static const int kEdgeTopY = 26;
    static const int kNumberOfFrames = 60;

    static const uint32_t kAnimationDuration = kNumberOfFrames * 1000 / 70; // ~429 ms

    uint32_t animationTime = 0;

    static const int kInitialLeftEdgeX = 147;
    static const int kInitialRightEdgeX = kInitialLeftEdgeX + kEdgeWidth + 1;

    static const int kEdgeAnimationDistance = (kEdgeStep * kNumberOfFrames) / 2 + 1;

    int leftEdgeX = kInitialLeftEdgeX;
    int rightEdgeX = kInitialRightEdgeX;

    startTrackingRenderDeltaTime();

    while (animationTime < kAnimationDuration)
    {
//loc_47800:             // ; CODE XREF: openCreditsBlock+AFj
        animationTime += updateRenderDeltaTime();
        animationTime = MIN(animationTime, kAnimationDuration);

        float animationFactor = (float)animationTime / kAnimationDuration;

        int previousLeftEdgeX = leftEdgeX;
        int previousRightEdgeX = rightEdgeX;

        int distance = ceilf(kEdgeAnimationDistance * animationFactor);

        leftEdgeX = kInitialLeftEdgeX - distance;
        rightEdgeX = kInitialRightEdgeX + distance;

        int leftEdgeStep = previousLeftEdgeX - leftEdgeX;
        int rightEdgeStep = rightEdgeX - previousRightEdgeX;

        // This loop moves both edges of the panel, and fills the inside of the panel with the contents of TITLE2.DAT
        for (int y = kEdgeTopY; y < kEdgeTopY + kEdgeHeight; ++y)
        {
            // Left edge
            for (int x = leftEdgeX; x < previousLeftEdgeX + kEdgeWidth - leftEdgeStep; ++x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr] = gScreenPixels[addr + leftEdgeStep]; // Move panel edge
            }

            // Content of visible panel unveiled by left edge
            for (int x = leftEdgeX + kEdgeWidth; x < previousLeftEdgeX + kEdgeWidth + 1; ++x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr] = gTitle2DecodedBitmapData[addr];
            }

            // Right edge
            for (int x = rightEdgeX + kEdgeWidth; x > previousRightEdgeX; --x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr] = gScreenPixels[addr - rightEdgeStep]; // Move panel edge
            }

            // Content of visible panel unveiled by right edge
            for (int x = previousRightEdgeX; x < rightEdgeX; ++x)
            {
                long addr = y * kScreenWidth + x;
                gScreenPixels[addr] = gTitle2DecodedBitmapData[addr];
            }
        }

        videoloop();
    }

//loc_47884:             // ; CODE XREF: openCreditsBlock+C7j
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
    FILE *file = openReadonlyFile("TITLE1.DAT", "r");
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
//    gNumberOfDotsToShiftDataLeft = 0;
    ColorPalette title1DatPalette;
    convertPaletteDataToPalette(gTitle1PaletteData, title1DatPalette);
    setPalette(title1DatPalette);
    videoloop();

    file = openReadonlyFile("TITLE2.DAT", "r");
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
}

void loadMurphySprites() // readMoving  proc near       ; CODE XREF: start:isFastModep
                    //; start+382p ...
{
    // 01ED:0D84

//loc_479ED:              // ; CODE XREF: loadMurphySprites+27j

// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// MOVING.DAT bitmap size is 320x462
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    FILE *file = openReadonlyFile("MOVING.DAT", "r");

    if (file == NULL)
    {
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
        uint8_t fileData[kMovingBitmapWidth / 2];

        size_t bytes = fread(fileData, 1, sizeof(fileData), file);
        if (bytes < sizeof(fileData))
        {
            exitWithError("Error reading MOVING.DAT\n");
        }

//loc_47A40:              ; CODE XREF: loadMurphySprites+BAj
        for (int x = 0; x < kMovingBitmapWidth; ++x)
        {
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

    file = openReadonlyFile("FIXED.DAT", "r");
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
}

void readPanelDat() //    proc near       ; CODE XREF: readPanelDat+14j
                    // ; readEverything+6p
{
    FILE *file = openReadonlyFile("PANEL.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening PANEL.DAT\n");
    }

//loc_47B0A:             // ; CODE XREF: readPanelDat+8j
    uint8_t bitmapData[kPanelBitmapWidth * kPanelBitmapHeight / 2];
    size_t bytes = fread(bitmapData, 1, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading PANEL.DAT\n");
    }

    for (int y = 0; y < kPanelBitmapHeight; ++y)
    {
        for (int x = 0; x < kPanelBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kPanelBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kPanelBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gPanelDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47B21:              // ; CODE XREF: readPanelDat+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PANEL.DAT\n");
    }
}

void readBackDat() // proc near       ; CODE XREF: readBackDat+14j
                    // ; readEverything+15p
{
    // address: 01ED:0ECD
    FILE *file = openReadonlyFile("BACK.DAT", "r");
    if (file == NULL)
    {
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
    FILE *file = openReadonlyFile("CHARS6.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS6.DAT\n");
    }

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
    file = openReadonlyFile("CHARS8.DAT", "r");
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
    FILE *file = openReadonlyFile("TITLE.DAT", "r");

    if (file == NULL)
    {
        exitWithError("Error opening TITLE.DAT\n");
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
            exitWithError("Error reading TITLE.DAT\n");
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
        exitWithError("Error closing TITLE.DAT\n");
    }
}

void readLevelsLst() //   proc near       ; CODE XREF: readLevelsLst+CCj
                    // ; readEverything+Fp ...
{
    // 01ED:1038

    char paddingEntryText[kListLevelNameLength] = "                           ";
    for (int i = 0; i < kNumberOfLevelsWithPadding; ++i)
    {
        memcpy(&gPaddedLevelListData[i * kListLevelNameLength], paddingEntryText, sizeof(paddingEntryText));
    }
    memcpy(&gPaddedLevelListData[kLastLevelIndex * kListLevelNameLength],
           "- REPLAY SKIPPED LEVELS!! -",
           kListLevelNameLength);
    memcpy(&gPaddedLevelListData[(kLastLevelIndex + 1) * kListLevelNameLength],
           "---- UNBELIEVEABLE!!!! ----",
           kListLevelNameLength);

    FILE *file = openWritableFile(gLevelLstFilename, "r");
    if (file == NULL)
    {
//errorOpeningLevelLst:             // ; CODE XREF: readLevelsLst+8j
        FILE *file = openReadonlyFile(gLevelsDatFilename, "r");
        if (file == NULL)
        {
//errorOpeningLevelsDat:             // ; CODE XREF: readLevelsLst+17j
            exitWithError("Error opening LEVELS.DAT\n");
        }
//successOpeningLevelsDat:             // ; CODE XREF: readLevelsLst+15j

        for (int i = 0; i < kNumberOfLevels; ++i)
        {
//loc_47CC4:             // ; CODE XREF: readLevelsLst:loc_47CE4j
            char number[5];
            sprintf(number, "%03d ", i + 1);

            memcpy(gLevelListData + i * kListLevelNameLength, number, sizeof(number) - 1);
            gLevelListData[i * kListLevelNameLength + kListLevelNameLength - 1] = '\n';
//loc_47CE4:            //  ; CODE XREF: readLevelsLst+3Aj
        }

        for (int i = 0; i < kNumberOfLevels; ++i)
        {
// loc_47CF1:             //  ; CODE XREF: readLevelsLst+83j

            int seekOffset = 0x5A6 + i * levelDataLength;

            fseek(file, seekOffset, SEEK_SET); // position 1446
            size_t bytes = fread(gLevelListData + i * kListLevelNameLength + 4, 1, 23, file);

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
        file = openWritableFile(gLevelLstFilename, "w");
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
    FILE *file = openReadonlyFile("GFX.DAT", "r");
    if (file == NULL)
    {
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
    FILE *file = openReadonlyFile("CONTROLS.DAT", "r");
    if (file == NULL)
    {
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
}

void readPlayersLst() //  proc near       ; CODE XREF: readEverything+1Bp
                    // ; handleFloppyDiskButtonClick+149p
{
    if (byte_59B85 != 0)
    {
        return;
    }

    for (int i = 0; i < kNumberOfPlayers; ++i)
    {
        strcpy(gPlayerListData[i].name, "--------");
    }

    FILE *file = openWritableFile(gPlayerLstFilename, "r");
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
}

void readHallfameLst() // proc near       ; CODE XREF: readEverything+18p
                    // ; handleFloppyDiskButtonClick+146p
{
    if (byte_59B85 != 0)
    {
        return;
    }
    
    FILE *file = openWritableFile(gHallfameLstFilename, "r");
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
}

void readEverything() //  proc near       ; CODE XREF: start+2DBp start+2E3p ...
{
    // 01ED:1213
    readPalettes();
    readBitmapFonts();
    readPanelDat();
    readMenuDat();
    readControlsDat();
    readLevelsLst();
    readDemoFiles();
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
        if (gIsEscapeKeyPressed != 0)
        {
            byte_59B86 = 0xFF;
        }

        int9handler(1);
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
    while (gCurrentUserInput > kUserInputSpaceAndDirectionOffset);

    for (int i = 0; i < 4200; ++i)
    {
//loc_47EC6:              ; CODE XREF: waitForKeyMouseOrJoystick+57j
        videoloop();

        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
        int9handler(0);

        if (mouseButtonsStatus != 0)
        {
            break;
        }
        if (keyPressed != SDL_SCANCODE_UNKNOWN)
        {
            break;
        }
        sub_48E59();
        if (gCurrentUserInput > kUserInputSpaceAndDirectionOffset)
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
            if (gIsEscapeKeyPressed != 0)
            {
                byte_59B86 = 0xFF;
            }

            int9handler(1);

//loc_47F18:              ; CODE XREF: waitForKeyMouseOrJoystick+79j
        }
        while (keyPressed != SDL_SCANCODE_UNKNOWN);
    }
    else if (gCurrentUserInput > kUserInputSpaceAndDirectionOffset)
    {
        do
        {
//loc_47F21:              ; CODE XREF: waitForKeyMouseOrJoystick+55j
//                ; waitForKeyMouseOrJoystick+9Dj
            if (gIsEscapeKeyPressed != 0)
            {
                byte_59B86 = 0xFF;
            }

//loc_47F2D:              ; CODE XREF: waitForKeyMouseOrJoystick+8Ej
            sub_48E59();
        }
        while (gCurrentUserInput > kUserInputSpaceAndDirectionOffset);
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

void updateZonkTiles(uint16_t position) //   proc near       ; DATA XREF: data:160Co
{
    // 01ED:132D

    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeZonk) // cmp byte ptr leveldata[si], 1
    {
        return;
    }

//loc_47F98:              ; CODE XREF: movefun+5j
    uint8_t shouldSkipFirstPartOfLoop = 0;

    if (currentTile->tile != LevelTileTypeZonk
        || currentTile->movingObject != 0)
    {
        shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48035"
    }
    else
    {
//loc_47FA4:              ; CODE XREF: movefun+Fj
        if (gAreZonksFrozen == 2)
        {
            return;
        }

//loc_47FAC:              ; CODE XREF: movefun+19j
        // Check if the zonk can just fall vertically
        if (belowTile->tile == LevelTileTypeSpace && belowTile->movingObject == 0)
        {
//loc_47FF4:              ; CODE XREF: movefun+23j
            currentTile->movingObject = 0x40;
            shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48035"
        }
        else
        {
            // Check if below the zonk is another object that could be used to slide down left or right
            if (belowTile->movingObject != 0
                || (belowTile->tile != LevelTileTypeZonk
                    && belowTile->tile != LevelTileTypeInfotron
                    && belowTile->tile != LevelTileTypeChip))
            {
                return;
            }

//loc_47FC5:              ; CODE XREF: movefun+28j
//                ; movefun+2Dj ...
            // Check if it can fall to the left side...
            if ((belowLeftTile->tile == LevelTileTypeSpace && belowLeftTile->movingObject == 0)
                || (belowLeftTile->tile == 0x88 && belowLeftTile->movingObject == 0x88)
                || (belowLeftTile->tile == 0xAA && belowLeftTile->movingObject == 0xAA))
            {
//loc_47FFB:              ; CODE XREF: movefun+3Aj
//                ; movefun+42j ...
                // ...but only if the left tile is empty
                if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
                {
//loc_48004:              ; CODE XREF: movefun+70j
                    currentTile->movingObject = 0x50;
                    leftTile->movingObject = 0x88;
                    leftTile->tile = 0x88;
                    shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48035"
                }
            }
        }
    }

    uint8_t shouldSkipTo_loc_481C6 = 0;

    do
    {
//loc_47FDC:              ; CODE XREF: movefun+72j
//                ; movefun+1F1j
        if (shouldSkipFirstPartOfLoop == 0) // used to emulate "jmp loc_48035"
        {
            // Checks if it can fall to the right side
            if ((belowRightTile->movingObject != 0 || belowRightTile->tile != LevelTileTypeSpace)
                && (belowRightTile->movingObject != 0x88 || belowRightTile->tile != 0x88)
                && (belowRightTile->movingObject != 0xAA || belowRightTile->tile != 0xAA))
            {
                return;
            }
            else
            {
//loc_48011:              ; CODE XREF: movefun+51j
//                ; movefun+59j ...
                // Only if the right tile is empty or... other circumstances?
                if ((rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeSpace)
                    && ((rightTile->movingObject != 0x99 || rightTile->tile != 0x99)
                        || (aboveRightTile->movingObject != 0 || aboveRightTile->tile != LevelTileTypeZonk)))
                {
                    return;
                }

//loc_48028:              ; CODE XREF: movefun+86j
//                ; movefun+95j
                currentTile->movingObject = 0x60;
                rightTile->movingObject = 0x88;
                rightTile->tile = 0x88;
            }
        }

        shouldSkipFirstPartOfLoop = 0; // don't skip the first part in the next iteration

//loc_48035:              ; CODE XREF: movefun+11j
//                ; movefun+69j ...
        uint8_t movingObject = currentTile->movingObject;
        uint8_t movingObjectType = movingObject & 0xF0;

        if (movingObjectType != 0x10) // 16
        {
//loc_48045:              ; CODE XREF: movefun+B1j
            if (movingObjectType == 0x20) // 32
            {
//loc_48212:              ; CODE XREF: movefun+B9j
                // 01ED:15AF
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 1294h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideLeftAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                // 01ED:15D6
                movingObject = currentTile->movingObject; // mov bl, [si+1835h]
                movingObject++;

                if (movingObject == 0x24) // 36
                {
                    rightTile->movingObject = 0xAA;
                    rightTile->tile = 0xAA;
                }
//loc_4824A:              ; CODE XREF: movefun+2B2j
                if (movingObject == 0x26) // 38
                {
                    currentTile->movingObject = movingObject;
                    handleZonkStateAfterFallingOneTile(position + 1);
                    return;
                }
//loc_4825D:              ; CODE XREF: movefun+2BDj
                else if (movingObject < 0x28) // 40
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_48267:              ; CODE XREF: movefun+2D0j
                else
                {
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->movingObject = 0x10;
                    belowTile->tile = LevelTileTypeZonk;
                    return;
                }
            }
//loc_4804C:              ; CODE XREF: movefun+B7j
            else if (movingObjectType == 0x30) // 48
            {
//loc_48277:              ; CODE XREF: movefun+C0j
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12A4h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideRightAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                movingObject = currentTile->movingObject;
                movingObject++;
                if (movingObject == 0x34) // 52
                {
                    leftTile->movingObject = 0xAA;
                    leftTile->tile = 0xAA;
                }
//loc_482AF:              ; CODE XREF: movefun+317j
                if (movingObject == 0x36) // 54
                {
                    currentTile->movingObject = movingObject;
                    handleZonkStateAfterFallingOneTile(position - 1); // left tile
                    return;
                }
//loc_482C2:              ; CODE XREF: movefun+322j
                else if (movingObject < 0x38) // 54
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_482CC:              ; CODE XREF: movefun+335j
                else
                {
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->movingObject = 0x10;
                    belowTile->tile = LevelTileTypeZonk;
                    return;
                }
            }
//loc_48053:              ; CODE XREF: movefun+BEj
            else if (gAreZonksFrozen == 2)
            {
                return;
            }
//loc_4805B:              ; CODE XREF: movefun+C8j
            else if (movingObjectType == 0x40) // 64
            {
//loc_482DC:              ; CODE XREF: movefun+CFj
                movingObject++;
                if (movingObject < 0x42) // 66
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_482E8:              ; CODE XREF: movefun+351j
                else if (belowTile->tile != LevelTileTypeSpace || belowTile->movingObject != 0) // cmp word ptr [si+18ACh], 0
                {
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_482F6:              ; CODE XREF: movefun+35Dj
                else
                {
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->movingObject = 0x10;
                    belowTile->tile = LevelTileTypeZonk;

                    return;
                }
            }
//loc_48062:              ; CODE XREF: movefun+CDj
            else if (movingObjectType == 0x50) // Zonk sliding left
            {
//loc_4830A:              ; CODE XREF: movefun+D6j
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 1294h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideLeftAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);
                
                movingObject = currentTile->movingObject;
                movingObject++;

                if (movingObject < 0x52) // 82
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_48341:              ; CODE XREF: movefun+3AAj
                else if (belowLeftTile->movingObject != 0 || belowLeftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
                {
//loc_48371:              ; CODE XREF: movefun+3B6j
    //                ; movefun+3C5j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else if ((leftTile->movingObject != 0 || leftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1832h], 0
                         && (leftTile->movingObject != 0x88 || leftTile->tile != 0x88)) // cmp word ptr [si+1832h], 8888h
                {
//loc_48371:              ; CODE XREF: movefun+3B6j
    //                ; movefun+3C5j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else
                {
//loc_48357:              ; CODE XREF: movefun+3BDj
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    leftTile->movingObject = 0x22;
                    leftTile->tile = LevelTileTypeZonk;
                    belowLeftTile->movingObject = 0xFF;
                    belowLeftTile->tile = 0xFF;
                    return;
                }
            }
//loc_48069:              ; CODE XREF: movefun+D4j
            else if (movingObjectType == 0x60) // 96
            {
//loc_48378:              ; CODE XREF: movefun+DDj
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12A4h
                Point frameCoordinates = kZonkSlideRightAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                movingObject = currentTile->movingObject;
                movingObject++;

                if (movingObject < 0x62) // 98
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_483AF:              ; CODE XREF: movefun+418j
                else if (belowRightTile->movingObject != 0 || belowRightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
                {
//loc_483DF:              ; CODE XREF: movefun+424j
    //                ; movefun+433j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else if ((rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1836h], 0
                         && (rightTile->movingObject != 0x88 || rightTile->tile != 0x88)) // cmp word ptr [si+1836h], 8888h
                {
//loc_483DF:              ; CODE XREF: movefun+424j
    //                ; movefun+433j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else
                {
//loc_483C5:              ; CODE XREF: movefun+42Bj
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    rightTile->movingObject = 0x32;
                    rightTile->tile = LevelTileTypeZonk;
                    belowRightTile->movingObject = 0xFF;
                    belowRightTile->tile = 0xFF;
                    return;
                }
            }
//loc_48070:              ; CODE XREF: movefun+DBj
            else if (movingObjectType == 0x70) // 112
            {
//loc_483E6:              ; CODE XREF: movefun+E4j
                if ((belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18ACh], 0
                    && (belowTile->movingObject != 0x99 && belowTile->tile != 0x99)) // cmp word ptr [si+18ACh], 9999h
                {
                    return;
                }

//loc_483F6:              ; CODE XREF: movefun+45Bj
//                ; movefun+463j
                currentTile->movingObject = 0xFF;
                currentTile->tile = 0xFF;

                // Move down and update tiles
                position += kLevelWidth;

                currentTile = &gCurrentLevelWord[position];
                belowTile = &gCurrentLevelWord[position + kLevelWidth];
                belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
                leftTile = &gCurrentLevelWord[position - 1];
                belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];
                rightTile = &gCurrentLevelWord[position + 1];
                aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];

                currentTile->movingObject = 0x10;
                currentTile->tile = LevelTileTypeZonk;
            }
//locret_48077:               ; CODE XREF: movefun+E2j
            else
            {
                return;
            }
        }

//loc_48078:              ; CODE XREF: movefun+B3j
//                ; movefun+475j
        // This animates the Zonk falling
        // 01ED:1415
        uint8_t somePositionThing = movingObject;
        somePositionThing *= 2;
        somePositionThing &= 0x1F;

        uint16_t offset = kFallAnimationGravityOffsets[somePositionThing];

        uint16_t finalPosition = position - kLevelWidth;
        uint8_t tileX = (finalPosition % kLevelWidth);
        uint8_t tileY = (finalPosition / kLevelWidth);

        uint16_t dstX = tileX * kTileSize + (offset % 122);
        uint16_t dstY = tileY * kTileSize + (offset / 122);

        drawMovingSpriteFrameInLevel(224, 82,
                                     kTileSize,
                                     kTileSize + 2,
                                     dstX, dstY);

        uint8_t newMovingObject = currentTile->movingObject;
        newMovingObject++;
        if (newMovingObject == 0x16) // 22
        {
            currentTile->movingObject = newMovingObject;
            handleZonkStateAfterFallingOneTile(position - kLevelWidth); // Tile above
            return;
        }
//loc_480BB:              ; CODE XREF: movefun+11Bj
        else if (newMovingObject < 0x18) // 24
        {
            currentTile->movingObject = newMovingObject;
            return;
        }

//loc_480C5:              ; CODE XREF: movefun+12Ej
        // This part handles what to do when the zonk finished falling 1 tile
        // 01ED:1462
        currentTile->movingObject = 0;
        if (gAreZonksFrozen == 2)
        {
            return;
        }

//loc_480D2:              ; CODE XREF: movefun+13Fj
        if ((belowTile->tile == LevelTileTypeSpace && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 0
            || (belowTile->tile == 0x99 && belowTile->movingObject == 0x99)) // cmp word ptr [si+18ACh], 9999h
        {
//loc_4816D:              ; CODE XREF: movefun+149j
//                ; movefun+154j
            currentTile->movingObject = 0x70;
            currentTile->tile = LevelTileTypeZonk;
            belowTile->movingObject = 0x99;
            belowTile->tile = 0x99;
            return;
        }

//loc_480E7:              ; CODE XREF: movefun+152j
        if (belowTile->tile == LevelTileTypeMurphy && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 3
        {
            break;
        }

//loc_480F1:              ; CODE XREF: movefun+15Cj
        if (belowTile->tile == LevelTileTypeSnikSnak) // cmp byte ptr [si+18ACh], 11h
        {
//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
            detonateBigExplosion(position + kLevelWidth); // Tile below
            return;
        }

//loc_480FB:              ; CODE XREF: movefun+166j
        if (belowTile->tile == 0xBB && belowTile->movingObject == 0x2) // cmp word ptr [si+18ACh], 2BBh
        {
            shouldSkipTo_loc_481C6 = 1;
            break;
        }

//loc_48106:              ; CODE XREF: movefun+171j
        if (belowTile->tile == 0xBB && belowTile->movingObject == 0x4) // cmp word ptr [si+18ACh], 4BBh
        {
//loc_481E2:              ; CODE XREF: movefun+17Ej
            if (belowRightTile->tile == LevelTileTypeElectron) // cmp byte ptr [si+18AEh], 18h
            {
                belowTile->tile = LevelTileTypeElectron;
                belowTile->movingObject = 0;
            }
//loc_481EF:              ; CODE XREF: movefun+257j
            else if (belowRightTile->tile != LevelTileTypeExplosion) // cmp byte ptr [si+18AEh], 1Fh
            {
                belowRightTile->tile = LevelTileTypeSpace;
                belowRightTile->movingObject = 0;
            }
//loc_481FC:              ; CODE XREF: movefun+264j
            else
            {
//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
                detonateBigExplosion(position + kLevelWidth); // Tile below
                return;
            }
        }

//loc_48111:              ; CODE XREF: movefun+17Cj
        if (belowTile->tile == LevelTileTypeElectron) // cmp byte ptr [si+18ACh], 18h
        {
//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
            detonateBigExplosion(position + kLevelWidth); // Tile below
            return;
        }

//loc_4811B:              ; CODE XREF: movefun+186j
        if (belowTile->tile == LevelTileTypeOrangeDisk && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 8
        {
//loc_48205:              ; CODE XREF: movefun+192j
            // TODO: no idea what this does yet. do orange disks turn into hardware tiles or something like that?
            // add si, 78h ; 'x'
            // mov bx, si
            // shr bx, 1
            // mov byte ptr [bx+2434h], 6
            gCurrentLevelAfterWord.tiles[position + kLevelWidth] = LevelTileTypeHardware;
            return;
        }

//loc_48125:              ; CODE XREF: movefun+190j
        sound7();
        if ((belowTile->tile != LevelTileTypeZonk || belowTile->movingObject != 0) // cmp word ptr [si+18ACh], 1
            && (belowTile->tile != LevelTileTypeInfotron || belowTile->movingObject != 0) // cmp word ptr [si+18ACh], 4
            && (belowTile->tile != LevelTileTypeChip || belowTile->movingObject != 0)) // cmp word ptr [si+18ACh], 5
        {
            return;
        }

//loc_4813E:              ; CODE XREF: movefun+19Dj
//                ; movefun+1A4j ...
        if ((belowLeftTile->movingObject == 0 && belowLeftTile->tile == LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
            || (belowLeftTile->movingObject == 0x88 && belowLeftTile->tile == 0x88) // cmp word ptr [si+18AAh], 8888h
            || (belowLeftTile->movingObject == 0xAA && belowLeftTile->tile == 0xAA)) // cmp word ptr [si+18AAh], 0AAAAh
        {
//loc_4817A:              ; CODE XREF: movefun+1B3j
//                ; movefun+1BBj ...
            if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace) //cmp word ptr [si+1832h], 0
            {
//loc_48184:              ; CODE XREF: movefun+1EFj
                currentTile->movingObject = 0x50;
                leftTile->movingObject = 0x88;
                leftTile->tile = 0x88;
                return;
            }
            else
            {
                continue;
            }
        }
        if ((belowRightTile->movingObject == 0 && belowRightTile->tile == LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
            || (belowRightTile->movingObject == 0x88 && belowRightTile->tile == 0x88) // cmp word ptr [si+18AEh], 8888h
            || (belowRightTile->movingObject == 0xAA && belowRightTile->tile == 0xAA)) // cmp word ptr [si+18AEh], 0AAAAh
        {
//loc_48190:              ; CODE XREF: movefun+1CAj
//                ; movefun+1D2j ...
            if (rightTile->tile == LevelTileTypeSpace && rightTile->movingObject == 0) //cmp word ptr [si+1836h], 0
            {
//loc_48198:              ; CODE XREF: movefun+205j
                currentTile->movingObject = 0x60;
                rightTile->movingObject = 0x88;
                rightTile->tile = 0x88;
                return;
            }
            else
            {
                return;
            }
        }

        return;

    }
    while (1);

    if (shouldSkipTo_loc_481C6 == 0)
    {
//loc_481A4:              ; CODE XREF: movefun+15Ej
        if (belowTile->movingObject == 0xE
            || belowTile->movingObject == 0xF
            || belowTile->movingObject == 0x28
            || belowTile->movingObject == 0x29
            || belowTile->movingObject == 0x25
            || belowTile->movingObject == 0x26)
        {
            return;
        }
    }

//loc_481C6:              ; CODE XREF: movefun+173j
    if (belowLeftTile->tile == LevelTileTypeElectron) // cmp byte ptr [si+18AAh], 18h
    {
        belowTile->tile = LevelTileTypeElectron;
        belowTile->movingObject = 0;
    }
//loc_481D3:              ; CODE XREF: movefun+23Bj
    else if (belowLeftTile->tile != LevelTileTypeExplosion) // cmp byte ptr [si+18AAh], 1Fh
    {
        belowLeftTile->tile = LevelTileTypeSpace;
        belowLeftTile->movingObject = 0;
    }

//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
    detonateBigExplosion(position + kLevelWidth); // Tile below
}

void updateInfotronTiles(uint16_t position) // movefun2  proc near       ; DATA XREF: data:1612o
{
    // 01ED:17A5

    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
    MovingLevelTile *belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    if (currentTile->tile != LevelTileTypeInfotron)
    {
        return;
    }

//loc_48410:              ; CODE XREF: movefun2+5j
    uint8_t shouldSkipFirstPartOfLoop = 0;

    if (currentTile->movingObject != 0 || currentTile->tile != LevelTileTypeInfotron)
    {
        shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48495"
    }
    else
    {
//loc_4841B:              ; CODE XREF: movefun2+Fj
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_48463:              ; CODE XREF: movefun2+1Aj
            currentTile->movingObject = 0x40;
            shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48495"
        }
        else
        {
            if ((belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeZonk)
                && (belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeInfotron)
                && (belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeChip))
            {
                return;
            }

//loc_48434:              ; CODE XREF: movefun2+1Fj
//                ; movefun2+24j ...
            if ((belowLeftTile->movingObject == 0 && belowLeftTile->tile == LevelTileTypeSpace)
                || (belowLeftTile->movingObject == 0x88 && belowLeftTile->tile == 0x88)
                || (belowLeftTile->movingObject == 0xAA && belowLeftTile->tile == 0xAA))
            {
//loc_4846A:              ; CODE XREF: movefun2+31j
//                ; movefun2+39j ...
                if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
                {
//loc_48473:              ; CODE XREF: movefun2+67j
                    currentTile->movingObject = 0x50;
                    leftTile->movingObject = 0x88;
                    leftTile->tile = 0x88;
                    shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48495"
                }
            }
        }
    }

    do
    {
        if (shouldSkipFirstPartOfLoop == 0)
        {
//loc_4844B:              ; CODE XREF: movefun2+69j
//                ; movefun2+1C7j
            if ((belowRightTile->movingObject != 0 || belowRightTile->tile != LevelTileTypeSpace)
                && (belowRightTile->movingObject != 0x88 || belowRightTile->tile != 0x88)
                && (belowRightTile->movingObject != 0xAA || belowRightTile->tile != 0xAA))
            {
                return;
            }

//loc_48480:              ; CODE XREF: movefun2+48j
//                ; movefun2+50j ...
            if (rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeSpace)
            {
                return;
            }

//loc_48488:              ; CODE XREF: movefun2+7Dj
            currentTile->movingObject = 0x60;
            rightTile->movingObject = 0x88;
            rightTile->tile = 0x88;
        }

//loc_48495:              ; CODE XREF: movefun2+11j
//                ; movefun2+60j ...
        uint8_t movingObject = currentTile->movingObject;
        uint8_t movingObjectType = movingObject & 0xF0;

        if (movingObjectType != 0x10)
        {
//loc_484A5:              ; CODE XREF: movefun2+99j
            if (movingObjectType == 0x20)
            {
//loc_4861B:              ; CODE XREF: movefun2+A1j
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                // mov si, 12B6h
                Point frameCoordinates = kInfotronSlideLeftAnimationFrameCoordinates[movingObjectFrame];

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX,
                                             dstY);

                movingObject = currentTile->movingObject;
                movingObject++;
                if (movingObject == 0x24) // 36
                {
                    rightTile->movingObject = 0xAA;
                    rightTile->tile = 0xAA;
                }
//loc_48653:              ; CODE XREF: movefun2+243j
                if (movingObject == 0x26) // 38
                {
                    currentTile->movingObject = movingObject;
                    handleInfotronStateAfterFallingOneTile(position + 1);
                    return;
                }
//loc_48666:              ; CODE XREF: movefun2+24Ej
                else if (movingObject < 0x28) // 40
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_48670:              ; CODE XREF: movefun2+261j
                else
                {
                    currentTile->movingObject = 0x70;
                    currentTile->tile = LevelTileTypeInfotron;
                    return;
                }
            }
//loc_484AC:              ; CODE XREF: movefun2+9Fj
            else if (movingObjectType == 0x30)
            {
//loc_48677:              ; CODE XREF: movefun2+A8j
                uint8_t movingObjectFrame = movingObject & 0x7;

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12C6h
                Point frameCoordinates = kInfotronSlideRightAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                movingObject = currentTile->movingObject;
                movingObject++;
                if (movingObject == 0x34) // 52
                {
                    leftTile->movingObject = 0xAA;
                    leftTile->tile = 0xAA;
                }
//loc_486AF:              ; CODE XREF: movefun2+29Fj
                if (movingObject == 0x36) // 54
                {
                    currentTile->movingObject = movingObject;
                    handleInfotronStateAfterFallingOneTile(position - 1); // left tile
                }
//loc_486C1:              ; CODE XREF: movefun2+2AAj
                if (movingObject < 0x38) // 54
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
                else
                {
                    currentTile->movingObject = 0x70;
                    currentTile->tile = LevelTileTypeInfotron;
                    return;
                }
            }
//loc_484B3:              ; CODE XREF: movefun2+A6j
            else if (movingObjectType == 0x40)
            {
//loc_486D2:              ; CODE XREF: movefun2+AFj
                movingObject++;
                if (movingObject < 0x42)
                {
                    currentTile->movingObject = movingObject;
                    return;
                }

//loc_486DE:              ; CODE XREF: movefun2+2CFj
                if (belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeSpace)
                {
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }

//loc_486EC:              ; CODE XREF: movefun2+2DBj
                currentTile->movingObject = 0xFF;
                currentTile->tile = 0xFF;
                belowTile->movingObject = 0x10;
                belowTile->tile = LevelTileTypeInfotron;
                return;
            }
//loc_484BA:              ; CODE XREF: movefun2+ADj
            else if (movingObjectType == 0x50)
            {
//loc_48700:              ; CODE XREF: movefun2+B6j
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12B6h
                Point frameCoordinates = kInfotronSlideLeftAnimationFrameCoordinates[movingObjectFrame];
                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);
                movingObject = currentTile->movingObject;
                movingObject++;

                if (movingObject < 0x52) // 82
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_48737:              ; CODE XREF: movefun2+328j
                else if (belowLeftTile->movingObject != 0 || belowLeftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
                {
//loc_48767:              ; CODE XREF: movefun2+334j
//                ; movefun2+343j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else if ((leftTile->movingObject != 0 || leftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1832h], 0
                         && (leftTile->movingObject != 0x88 || leftTile->tile != 0x88)) // cmp word ptr [si+1832h], 8888h
                {
//loc_48767:              ; CODE XREF: movefun2+334j
//                ; movefun2+343j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else
                {
//loc_4874D:              ; CODE XREF: movefun2+33Bj
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    leftTile->movingObject = 0x22;
                    leftTile->tile = LevelTileTypeInfotron;
                    belowLeftTile->movingObject = 0x99;
                    belowLeftTile->tile = 0x99;
                    return;
                }
            }
//loc_484C1:              ; CODE XREF: movefun2+B4j
            else if (movingObjectType == 0x60)
            {
//loc_4876E:              ; CODE XREF: movefun2+BDj
                uint8_t movingObjectFrame = movingObject & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12C6h
                Point frameCoordinates = kInfotronSlideRightAnimationFrameCoordinates[movingObjectFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                movingObject = currentTile->movingObject;
                movingObject++;

                if (movingObject < 0x62) // 98
                {
                    currentTile->movingObject = movingObject;
                    return;
                }
//loc_487A5:              ; CODE XREF: movefun2+396j
                else if (belowRightTile->movingObject != 0 || belowRightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
                {
//loc_487D5:              ; CODE XREF: movefun2+3A2j
//                ; movefun2+3B1j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else if ((rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1836h], 0
                         && (rightTile->movingObject != 0x88 || rightTile->tile != 0x88)) // cmp word ptr [si+1836h], 8888h
                {
//loc_487D5:              ; CODE XREF: movefun2+3A2j
//                ; movefun2+3B1j
                    movingObject--;
                    currentTile->movingObject = movingObject;
                    return;
                }
                else
                {
//loc_487BB:              ; CODE XREF: movefun2+3A9j
                    currentTile->movingObject = 0xFF;
                    currentTile->tile = 0xFF;
                    rightTile->movingObject = 0x32;
                    rightTile->tile = LevelTileTypeInfotron;
                    belowRightTile->movingObject = 0x99;
                    belowRightTile->tile = 0x99;
                    return;
                }
            }
//loc_484C8:              ; CODE XREF: movefun2+BBj
            else if (movingObjectType == 0x70)
            {
//loc_487DC:              ; CODE XREF: movefun2:loc_484CCj
                if ((belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeSpace)
                    && (belowTile->movingObject != 0x99 || belowTile->tile != 0x99))
                {
                    return;
                }

//loc_487EC:              ; CODE XREF: movefun2+3D9j
//                    ; movefun2+3E1j
                currentTile->movingObject = 0xFF;
                currentTile->tile = 0xFF;

                // TODO: Move down and update tiles??
                // add si, 78h ; 'x'
                position += kLevelWidth;

                currentTile = &gCurrentLevelWord[position];
                belowTile = &gCurrentLevelWord[position + kLevelWidth];
                belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
                leftTile = &gCurrentLevelWord[position - 1];
                belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];
                rightTile = &gCurrentLevelWord[position + 1];

                currentTile->movingObject = 0x10;
                currentTile->tile = LevelTileTypeInfotron;
            }
            else
            {
                return;
            }
        }

//loc_484D0:              ; CODE XREF: movefun2+9Bj
//                ; movefun2+3F3j
        // This animates the Infotron falling
        uint8_t somePositionThing = movingObject;
        somePositionThing *= 2;
        somePositionThing &= 0x1F;

        uint16_t offset = kFallAnimationGravityOffsets[somePositionThing];

        uint16_t finalPosition = position - kLevelWidth;
        uint8_t tileX = (finalPosition % kLevelWidth);
        uint8_t tileY = (finalPosition / kLevelWidth);

        uint16_t dstX = tileX * kTileSize + (offset % 122);
        uint16_t dstY = tileY * kTileSize + (offset / 122);

        // mov si, word_515C4
        drawMovingSpriteFrameInLevel(240, 178,
                                     kTileSize,
                                     kTileSize + 2,
                                     dstX, dstY);

        uint8_t newMovingObject = currentTile->movingObject;
        newMovingObject++;
        if (newMovingObject == 0x16) // 22
        {
            currentTile->movingObject = newMovingObject;
            handleInfotronStateAfterFallingOneTile(position - kLevelWidth); // Tile above
            return;
        }
//loc_48513:              ; CODE XREF: movefun2+FBj
        else if (newMovingObject < 0x18) // 24
        {
            currentTile->movingObject = newMovingObject;
            return;
        }

//loc_4851D:              ; CODE XREF: movefun2+10Ej
        // This part handles what to do when the Infotron finished falling 1 tile
        currentTile->movingObject = 0;

        if ((belowTile->tile == LevelTileTypeSpace && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 0
            || (belowTile->tile == 0x99 && belowTile->movingObject == 0x99)) // cmp word ptr [si+18ACh], 9999h
        {
//loc_485BB:              ; CODE XREF: movefun2+121j
//                ; movefun2+12Cj
            currentTile->movingObject = 0x70;
            currentTile->tile = LevelTileTypeInfotron;
            belowTile->movingObject = 0x99;
            belowTile->tile = 0x99;
            return;
        }

//loc_48537:              ; CODE XREF: movefun2+12Aj
        if (belowTile->tile == LevelTileTypeMurphy && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 3
        {
//loc_485F2:              ; CODE XREF: movefun2+136j
            if (belowTile->movingObject == 0xE
                || belowTile->movingObject == 0xF
                || belowTile->movingObject == 0x28
                || belowTile->movingObject == 0x29
                || belowTile->movingObject == 0x25
                || belowTile->movingObject == 0x26)
            {
                return;
            }

//loc_48614:              ; CODE XREF: movefun2+140j
//                ; movefun2+14Aj ...
            detonateBigExplosion(position + kLevelWidth);
            return;
        }

//loc_48541:              ; CODE XREF: movefun2+134j
        if ((belowTile->tile == LevelTileTypeRedDisk && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 14h
            || belowTile->tile == LevelTileTypeSnikSnak // cmp byte ptr [si+18ACh], 11h
            || belowTile->tile == LevelTileTypeElectron // cmp byte ptr [si+18ACh], 18h
            || (belowTile->tile == LevelTileTypeYellowDisk && belowTile->movingObject == 0) // cmp word ptr [si+18ACh], 12h
            || (belowTile->tile == LevelTileTypeOrangeDisk && belowTile->movingObject == 0)) // cmp word ptr [si+18ACh], 8
        {
//loc_48614:              ; CODE XREF: movefun2+140j
//                ; movefun2+14Aj ...
            detonateBigExplosion(position + kLevelWidth);
            return;
        }

//loc_48573:              ; CODE XREF: movefun2+166j
        sound7();
        if ((belowTile->tile != LevelTileTypeZonk || belowTile->movingObject != 0) // cmp word ptr [si+18ACh], 1
            && (belowTile->tile != LevelTileTypeInfotron || belowTile->movingObject != 0) // cmp word ptr [si+18ACh], 4
            && (belowTile->tile != LevelTileTypeChip || belowTile->movingObject != 0)) // cmp word ptr [si+18ACh], 5
        {
            return;
        }

//loc_4858C:              ; CODE XREF: movefun2+173j
//                ; movefun2+17Aj ...
        if ((belowLeftTile->tile == LevelTileTypeSpace && belowLeftTile->movingObject == 0) // cmp word ptr [si+18AAh], 0
            || (belowLeftTile->tile == 0x88 && belowLeftTile->movingObject == 0x88) // cmp word ptr [si+18AAh], 8888h
            || (belowLeftTile->tile == 0xAA && belowLeftTile->movingObject == 0xAA)) // cmp word ptr [si+18AAh], 0AAAAh
        {
//loc_485C8:              ; CODE XREF: movefun2+189j
//                ; movefun2+191j ...
            if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
            {
//loc_485D2:              ; CODE XREF: movefun2+1C5j
                currentTile->movingObject = 0x50;
                leftTile->movingObject = 0x88;
                leftTile->tile = 0x88;
                return;
            }
            else
            {
                continue; // jmp loc_4844B
            }
        }
        if ((belowRightTile->tile == LevelTileTypeSpace && belowRightTile->movingObject == 0) // cmp word ptr [si+18AEh], 0
            || (belowRightTile->tile == 0x88 && belowRightTile->movingObject == 0x88) // cmp word ptr [si+18AEh], 8888h
            || (belowRightTile->tile == 0xAA && belowRightTile->movingObject == 0xAA)) // cmp word ptr [si+18AEh], 0AAAAh
        {
//loc_485DE:              ; CODE XREF: movefun2+1A0j
//                ; movefun2+1A8j ...
            if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
            {
//loc_485E6:              ; CODE XREF: movefun2+1DBj
                currentTile->movingObject = 0x60;
                rightTile->movingObject = 0x88;
                rightTile->tile = 0x88;
                return;
            }
            return;
        }
        return;
    }
    while (1);
}

void handleMurphyCollisionAfterMovement(uint16_t position) // sub_487FE   proc near       ; CODE XREF: update?+E0Cp update?+E2Ap ...
{
    // 01ED:1B9B
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];
    MovingLevelTile *aboveLeftTile = &gCurrentLevelWord[position - kLevelWidth - 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->movingObject = 0;
        currentTile->tile = LevelTileTypeSpace;
    }

//loc_4880B:              ; CODE XREF: handleMurphyCollisionAfterMovement+5j
    if ((aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
        || (aboveTile->movingObject == 0x99 && aboveTile->tile == 0x99))
    {
//loc_48835:              ; CODE XREF: handleMurphyCollisionAfterMovement+12j
//                ; handleMurphyCollisionAfterMovement+1Aj
        if (aboveLeftTile->movingObject == 0 && aboveLeftTile->tile == LevelTileTypeZonk)
        {
//loc_48852:              ; CODE XREF: handleMurphyCollisionAfterMovement+3Cj
            if ((leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeZonk)
                || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeInfotron)
                || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeChip))
            {
//loc_48869:              ; CODE XREF: handleMurphyCollisionAfterMovement+59j
//                ; handleMurphyCollisionAfterMovement+60j ...
                aboveLeftTile->movingObject = 0x60;
                aboveTile->movingObject = 0x88;
                aboveTile->tile = 0x88;
                return;
            }
        }
        else if (aboveLeftTile->movingObject == 0 && aboveLeftTile->tile == LevelTileTypeInfotron)
        {
//loc_48897:              ; CODE XREF: handleMurphyCollisionAfterMovement+43j
            if ((leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeZonk)
                || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeInfotron)
                || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeChip))
            {
//loc_488AE:              ; CODE XREF: handleMurphyCollisionAfterMovement+9Ej
//                ; handleMurphyCollisionAfterMovement+A5j ...
                aboveLeftTile->movingObject = 0x60;
                aboveTile->movingObject = 0x88;
                aboveTile->tile = 0x88;
                return;
            }
        }
//loc_48843:              ; CODE XREF: handleMurphyCollisionAfterMovement+69j
//                ; handleMurphyCollisionAfterMovement+AEj
        if (aboveRightTile->movingObject == 0 && aboveRightTile->tile == LevelTileTypeZonk)
        {
//loc_48875:              ; CODE XREF: handleMurphyCollisionAfterMovement+4Aj
            if ((rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeZonk)
                || (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeInfotron)
                || (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeChip))
            {
//loc_4888B:              ; CODE XREF: handleMurphyCollisionAfterMovement+7Cj
//                ; handleMurphyCollisionAfterMovement+83j ...
                aboveRightTile->movingObject = 0x50;
                aboveTile->movingObject = 0x88;
                aboveTile->tile = 0x88;
            }
        }
        else if (aboveRightTile->movingObject == 0 && aboveRightTile->tile == LevelTileTypeInfotron)
        {
//loc_488BA:              ; CODE XREF: handleMurphyCollisionAfterMovement+51j
            if ((rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeZonk)
                || (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeInfotron)
                || (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeChip))
            {
//loc_488D0:              ; CODE XREF: handleMurphyCollisionAfterMovement+C1j
//                ; handleMurphyCollisionAfterMovement+C8j ...
                aboveRightTile->movingObject = 0x50;
                aboveTile->movingObject = 0x88;
                aboveTile->tile = 0x88;
            }
        }
    }
    else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeZonk)
    {
//loc_48829:              ; CODE XREF: handleMurphyCollisionAfterMovement+21j
        aboveTile->movingObject = 0x40;
    }
    else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeInfotron)
    {
//loc_4882F:              ; CODE XREF: handleMurphyCollisionAfterMovement+28j
        aboveTile->movingObject = 0x40;
    }
}

void handleZonkStateAfterFallingOneTile(uint16_t position) // sub_488DC   proc near       ; CODE XREF: movefun+124p
                   // ; movefun+2C6p ...
{
    // 01ED:1C79
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *aboveAboveTile = &gCurrentLevelWord[position - kLevelWidth * 2];
    MovingLevelTile *aboveLeftTile = &gCurrentLevelWord[position - kLevelWidth - 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->movingObject = 0;
        currentTile->tile = LevelTileTypeSpace;
    }

//loc_488E9:              ; CODE XREF: handleZonkStateAfterFallingOneTile+5j
    if (aboveTile->movingObject != 0 || aboveTile->tile != LevelTileTypeSpace) // cmp word ptr [si+17BCh], 0
    {
        if (aboveTile->movingObject != 0x99 || aboveTile->tile != 0x99) // cmp word ptr [si+17BCh], 9999h
        {
            return;
        }

//loc_488F9:              ; CODE XREF: handleZonkStateAfterFallingOneTile+1Aj
        if (aboveAboveTile->tile != LevelTileTypeInfotron) // cmp byte ptr [si+1744h], 4
        {
            return;
        }
    }

//loc_48901:              ; CODE XREF: handleZonkStateAfterFallingOneTile+12j
//                ; handleZonkStateAfterFallingOneTile+22j
    if (aboveLeftTile->movingObject == 0 && aboveLeftTile->tile == LevelTileTypeZonk) // cmp word ptr [si+17BAh], 1
    {
//loc_48910:              ; CODE XREF: handleZonkStateAfterFallingOneTile+2Aj
        if (leftTile->movingObject == 0
            && (leftTile->tile == LevelTileTypeZonk
                || leftTile->tile == LevelTileTypeInfotron
                || leftTile->tile == LevelTileTypeChip))
        {
//loc_48927:              ; CODE XREF: handleZonkStateAfterFallingOneTile+39j
//                ; handleZonkStateAfterFallingOneTile+40j ...
            // mov word ptr [si+17BAh], 6001h
            aboveLeftTile->movingObject = 0x60;
            aboveLeftTile->tile = LevelTileTypeZonk;
            // mov word ptr [si+17BCh], 8888h
            aboveTile->movingObject = 0x88;
            aboveTile->tile = 0x88;
            return;
        }
    }

//loc_48908:              ; CODE XREF: handleZonkStateAfterFallingOneTile+49j
    if (aboveRightTile->movingObject == 0 && aboveRightTile->tile == LevelTileTypeZonk) // cmp word ptr [si+17BEh], 1
    {
//loc_48934:              ; CODE XREF: handleZonkStateAfterFallingOneTile+31j
        if (rightTile->movingObject != 0
            || (rightTile->tile != LevelTileTypeZonk
                && rightTile->tile != LevelTileTypeInfotron
                && rightTile->tile != LevelTileTypeChip))
        {
            return;
        }

//loc_4894A:              ; CODE XREF: handleZonkStateAfterFallingOneTile+5Dj
//                ; handleZonkStateAfterFallingOneTile+64j ...
        // mov word ptr [si+17BEh], 5001h
        aboveRightTile->movingObject = 0x50;
        aboveRightTile->tile = LevelTileTypeZonk;
        // mov word ptr [si+17BCh], 8888h
        aboveTile->movingObject = 0x88;
        aboveTile->tile = 0x88;
        return;
    }
    return;
}

void handleInfotronStateAfterFallingOneTile(uint16_t position) // sub_48957   proc near       ; CODE XREF: movefun2+104p
//                    ; movefun2+257p ...
{
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *aboveAboveTile = &gCurrentLevelWord[position - kLevelWidth * 2];
    MovingLevelTile *aboveLeftTile = &gCurrentLevelWord[position - kLevelWidth - 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->movingObject = 0;
        currentTile->tile = LevelTileTypeSpace;
    }
//loc_48964:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+5j
    if (aboveTile->movingObject != 0 || aboveTile->tile != LevelTileTypeSpace)
    {
        if (aboveTile->movingObject == 0x99 && aboveTile->tile == 0x99)
        {
//loc_48974:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+1Aj
            if (aboveAboveTile->tile != LevelTileTypeZonk)
            {
                return;
            }
        }
        else
        {
            return;
        }
    }


//loc_4897C:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+12j
//                ; handleInfotronStateAfterFallingOneTile+22j
    if (aboveLeftTile->movingObject == 0 && aboveLeftTile->tile == LevelTileTypeInfotron)
    {
//loc_4898B:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+2Aj
        if ((leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeZonk)
            || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeInfotron)
            || (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeChip))
        {
//loc_489A2:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+39j
//                ; handleInfotronStateAfterFallingOneTile+40j ...
            aboveLeftTile->movingObject = 0x60;
            aboveLeftTile->tile = LevelTileTypeInfotron;
            aboveTile->movingObject = 0x88;
            aboveTile->tile = 0x88;
            return;
        }
    }

//loc_48983:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+49j
    if (aboveRightTile->movingObject != 0 || aboveRightTile->tile != LevelTileTypeInfotron)
    {
        return;
    }

//loc_489AF:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+31j
    if ((rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeZonk)
        && (rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeInfotron)
        && (rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeChip))
    {
        return;
    }

//loc_489C5:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+5Dj
//                ; handleInfotronStateAfterFallingOneTile+64j ...
    aboveRightTile->movingObject = 0x50;
    aboveRightTile->tile = LevelTileTypeInfotron;
    aboveTile->movingObject = 0x88;
    aboveTile->tile = 0x88;
}

void initializeGameInfo() // sub_48A20   proc near       ; CODE XREF: start+32Fp
                // ; runLevel:notFunctionKeyp ...
{
    // 01ED:1DBD
    word_510BC = gMurphyTileX;
    word_510BE = gMurphyTileY;
    ax = 0;
    word_510CB = 0;
    word_510D1 = 0;
    gShouldExitLevel = 0;
    gQuitLevelCountdown = 0;
    gNumberOfRemainingRedDisks = 0;
    byte_5197C = 0;
    word_510CD = 0;
    gLastDrawnMinutesAndSeconds = 0xFFFF;
    gLastDrawnHours = 0xFF; // 255
    gIsGamePaused = 1;
    gAuxGameSeconds20msAccumulator = 0;
    gGameSeconds = 0;
    gGameMinutes = 0;
    gGameHours = 0;
    byte_510B4 = 0;
    byte_510B5 = 0;
    byte_510B6 = 0;
    gIsExplosionStarted = 0;
    byte_5196A = 0x7F; // 127
    byte_5196B = 0;
    word_5195D = 0;
//    mov byte ptr word_510C1, 1
//    mov byte ptr word_510C1+1, 0
    word_510C1 = 0x0001;
    gAreEnemiesFrozen = 0;
    gNumberOfDotsToShiftDataLeft = 0;
    gIsMurphyGoingThroughPortal &= 0xFF00; // mov byte ptr gIsMurphyGoingThroughPortal, 0
    gPlantedRedDiskCountdown = 0;
    gPlantedRedDiskPosition = 0;
}


void handleGameIterationStarted()
{
    gGameIterationStartTime = SDL_GetTicks();
}

void handleGameIterationFinished()
{
    const float kOriginalIterationDuration = 1000.0 / 35; // 35 iterations per second in the original game
    float currentIterationDuration = SDL_GetTicks() - gGameIterationStartTime;

    float targetIterationDuration = kOriginalIterationDuration * kSpeedTimeFactors[gGameSpeed];

    if (fastMode == 1)
    {
        targetIterationDuration = 0;
    }

    if (currentIterationDuration < targetIterationDuration)
    {
        SDL_Delay(targetIterationDuration - currentIterationDuration);
    }

    gNumberOfGameIterations++;

    if (gGameIterationRateReferenceTime == 0)
    {
        gGameIterationRateReferenceTime = SDL_GetTicks();
    }
    else
    {
        Uint32 difference = SDL_GetTicks() - gGameIterationRateReferenceTime;

        if (difference > 1000)
        {
            gGameIterationRate = gNumberOfGameIterations * 1000.f / difference;
            gNumberOfGameIterations = 0;
            gGameIterationRateReferenceTime = SDL_GetTicks();
        }
    }
}

void runLevel() //    proc near       ; CODE XREF: start+35Cp
{
    // 01ED:1E58
    if (gIsPlayingDemo == 0)
    {
//loc_48ACE:              ; CODE XREF: runLevel+5j
        byte_5A19C = 0;
        gShouldShowFailedLevelResultScreen = 1;
    }
    else
    {
        byte_5A19C = 1;
        gShouldShowFailedLevelResultScreen = 0;
    }

//loc_48AD8:              ; CODE XREF: runLevel+11j
    if (byte_5A2F8 == 1)
    {
//loc_48ADF:              ; CODE XREF: runLevel+BAj
        byte_5A2F8 = 0;
        drawGameTime();

        do
        {
//isFunctionKey:              ; CODE XREF: runLevel+35j
            int9handler(1);
        }
        while (keyPressed >= SDL_SCANCODE_F1
               && keyPressed <= SDL_SCANCODE_F10);

//notFunctionKey:             ; CODE XREF: runLevel+31j
        initializeGameInfo();
        if (isMusicEnabled == 0)
        {
            sound3();
        }

//loc_48AFF:              ; CODE XREF: runLevel+3Fj
        byte_5A19C = 0;
        gShouldShowFailedLevelResultScreen = 1;
    }

//loc_48B09:              ; CODE XREF: runLevel+22j
    gPlantedRedDiskCountdown = 0;
    byte_5A323 = 0;
    word_510A2 = 1;

    do
    {
        handleGameIterationStarted();

        int9handler(0);

//updateMovingObjectsrep:                ; CODE XREF: runLevel+33Cj
//                ; runLevel+345j
        if (gIsPlayingDemo == 0)
        {
            sub_48E59(); // 01ED:1EBD
        }

        uint16_t mouseButtonsStatus;

//loc_48B23:              ; CODE XREF: runLevel+63j
        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
        if (mouseButtonsStatus == 2
            && gQuitLevelCountdown <= 0)
        {
            word_510D1 = 1;
        }

//loc_48B38:              ; CODE XREF: runLevel+6Ej runLevel+75j
        if (gIsDebugModeEnabled != 0)
        {
            if (byte_59B7A != 0)
            {
                byte_59B7A--;
            }

//loc_48B4A:              ; CODE XREF: runLevel+89j
            if (gIsEnterPressed == 0
                && mouseButtonsStatus == 1 //cmp bx, 1
                && byte_59B7A == 0)
            {
                byte_59B7A = 0xA;
                levelScanThing(); // 01ED:1EFF
                drawFixedLevel();
                sub_4A2E6();
            }
        }

//loc_48B6B:              ; CODE XREF: runLevel+82j runLevel+94j ...
        handleGameUserInput(); // 01ED:1F08
        if (byte_5A2F8 == 1)
        {
            // TODO: This goes back to the beginning of this function WTF
            // Maybe it just restarts the demo? Seems to be related to playing demos
//            jmp loc_48ADF
        }

//loc_48B78:              ; CODE XREF: runLevel+B8j
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (SDL_Color) { 0x35, 0x35, 0x35 });
        }

//noFlashing:              ; CODE XREF: runLevel+C2j
        updateMovingObjects(); // 01ED:1F28
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (SDL_Color) { 0x21, 0x21, 0x21 });
        }

//noFlashing2:              ; CODE XREF: runLevel+D8j
        drawGameTime();
        clearNumberOfRemainingRedDisks();
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (SDL_Color) { 0x2d, 0x21, 0x0f });
        }

//noFlashing3:              ; CODE XREF: runLevel+F1j
        // 01ED:1F5B
        updatePlantedRedDisk();
        sub_4A5E0();
        updateScrollOffset();
        ax = gScrollOffsetX;
        al &= 7;
        gNumberOfDotsToShiftDataLeft = al;

//loc_48D59:              ; CODE XREF: runLevel+19Bj
//                ; runLevel+1D2j ...
        word_59B92 = gScrollOffsetY;
        word_59B90 = gScrollOffsetX;
        al &= 7;
        gNumberOfDotsToShiftDataLeft = al;

        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (SDL_Color) { 0x3f, 0x3f, 0x3f });
        }

//noFlashing4:              ; CODE XREF: runLevel+2D1j
        gNumberOfDotsToShiftDataLeft = ah;

        drawCurrentLevelViewport(gCurrentPanelHeight); // Added by me

        if (fastMode != 1)
        {
            videoloop(); // 01ED:2142
        }

        handleGameIterationFinished();

//isFastMode2:              ; CODE XREF: runLevel+2E8j
        if (gDebugExtraRenderDelay > 1)
        {
            sound9();
        }

//loc_48DB2:              ; CODE XREF: runLevel+2F2j
        // Extra delays in debug mode
        for (int i = 1; i < gDebugExtraRenderDelay; ++i)
        {
//loc_48DB6:              ; CODE XREF: runLevel+310j
            if (fastMode != 1)
            {
                videoloop(); // 01ED:2160
            }

//isFastMode3:              ; CODE XREF: runLevel+303j
            handleGameUserInput();
        }

//loc_48DCD:              ; CODE XREF: runLevel+2FCj
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });
        }

//noFlashing5:              ; CODE XREF: runLevel+317j
        if (gShouldExitGame != 0)
        {
            break;
        }
        word_5195D++;
        if (gShouldExitLevel == 1)
        {
            break;
        }
        if (gQuitLevelCountdown == 0) // 01ED:218D
        {
            continue;
        }

//loc_48DFA:              ; CODE XREF: runLevel+33Aj
        // 01ED:2197
        gQuitLevelCountdown--;
        if (gQuitLevelCountdown == 0)
        {
            break;
        }
    }
    while(1);

//loc_48E03:              ; CODE XREF: runLevel+328j
//                ; runLevel+333j ...
    word_510A2 = 0;
    if (gIsRecordingDemo != 0)
    {
        somethingspsig();
    }

//loc_48E13:              ; CODE XREF: runLevel+353j
    uint8_t was_byte_5A2F9_Zero = (byte_5A2F9 == 0);
    byte_5A2F9 = 0;
    if (was_byte_5A2F9_Zero
        && byte_510B3 != 0
        && byte_5A323 == 0)
    {
        sub_4A95F();
    }

//loc_48E30:              ; CODE XREF: runLevel+362j
//                ; runLevel+369j ...
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });
}

// TODO: seems to be the function that reads joystick input. worth keeping it or should I just delete it?
void sub_48E59() //   proc near       ; CODE XREF: waitForKeyMouseOrJoystick:loc_47EB8p
//                    ; waitForKeyMouseOrJoystick+4Dp ...
{
    ax = 0;
    UserInput userInput = UserInputNone;
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
            // Joystick axis X
//            ah = 1;
            if (waitForJoystickKeyReleased(1, &ax)) // jnb short loc_48E8D
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
                // Joystick axis Y
//                ah = 2;
                if (waitForJoystickKeyReleased(2, &ax)) // jnb short loc_48EC8
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
//                    userInput = *(uint8_t *)si;
                    if (userInput != UserInputNone)
                    {
//                    mov dx, 201h
//                    in  al, dx      ; Game I/O port
//                                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
//                                ; bits 4-7: Buttons/Triggers (digital inputs)
                        if ((al & 0x10) == 0
                            || (al & 0x20) == 0
                            || byte_519F5 != 0
                            || byte_519F4 != 0)
                        {
                            userInput += kUserInputSpaceAndDirectionOffset;
                        }
                    }
                    else
                    {
//loc_48F40:              ; CODE XREF: sub_48E59+B7j
//                    mov dx, 201h
//                    in  al, dx      ; Game I/O port
//                                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
//                                ; bits 4-7: Buttons/Triggers (digital inputs)
                        if ((al & 0x10) == 0
                            || (al & 0x20) == 0
                            || byte_519F5 != 0
                            || byte_519F4 != 0)
                        {
                            userInput = UserInputSpaceOnly;
                        }
                    }
                }
            }
        }
    }
//loc_48F68:              ; CODE XREF: sub_48E59+Bj
//                ; sub_48E59+27j ...
    gCurrentUserInput = userInput;
}

// Draws the fixed stuff from the level (edges of the screen + tiles from FIXED.DAT)
void drawFixedLevel() // sub_48F6D   proc near       ; CODE XREF: start+335p runLevel+AAp ...
{
    // 01ED:230A

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
//loc_48F86:              ; CODE XREF: drawFixedLevel+20j
            size_t srcAddress = (kMovingBitmapTopLeftCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws top edge
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
            size_t srcAddress = (kMovingBitmapTopEdgeY + y) * kMovingBitmapWidth + kMovingBitmapTopEdgeX + (x % kLevelEdgeSize);
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Top-right corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FC8:              ; CODE XREF: drawFixedLevel+62j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapTopRightCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Right edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
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
//loc_48FFE:              ; CODE XREF: drawFixedLevel+98j
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
//loc_4901C:              ; CODE XREF: drawFixedLevel+BDj
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_4901F:              ; CODE XREF: drawFixedLevel+B4j
            int srcX = x % kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws left edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_49047:              ; CODE XREF: drawFixedLevel+EBj
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_4904A:              ; CODE XREF: drawFixedLevel+E4j
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
//loc_49067:              ; CODE XREF: drawFixedLevel+101j
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
            uint16_t tileValue = gCurrentLevelWord[tileY * kLevelWidth + tileX].tile;
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

//loc_490FD:              ; CODE XREF: drawFixedLevel+18Cj
    // No idea what's this yet...
    bx = gScrollOffsetX;
    cl = 3;
    bx = bx >> cl;
    ax = gScrollOffsetY;
    cx = 0x7A; // 122
    ax = ax * cx;
    bx += ax;
    bx += 0x4D34; // 19764
    word_51967 = bx;
}

void updateUserInputInScrollMovementMode() // sub_4914A   proc near       ; CODE XREF: handleGameUserInput+7p
{
    if (gIsLeftKeyPressed != 0)
    {
        gAdditionalScrollOffsetX--;
        gAdditionalScrollOffsetX--;
    }

//loc_49159:              ; CODE XREF: updateUserInputInScrollMovementMode+5j
    if (gIsRightKeyPressed != 0)
    {
        gAdditionalScrollOffsetX++;
        gAdditionalScrollOffsetX++;
    }

//loc_49168:              ; CODE XREF: updateUserInputInScrollMovementMode+14j
    if (gIsUpKeyPressed != 0)
    {
        gAdditionalScrollOffsetY--;
        gAdditionalScrollOffsetY--;
    }

//loc_49177:              ; CODE XREF: updateUserInputInScrollMovementMode+23j
    if (gIsDownKeyPressed != 0)
    {
        gAdditionalScrollOffsetY++;
        gAdditionalScrollOffsetY++;
    }

//loc_49186:              ; CODE XREF: updateUserInputInScrollMovementMode+32j
    if (gIsNumpad5Pressed != 0)
    {
        gAdditionalScrollOffsetX = 0;
        gAdditionalScrollOffsetY = 0;
    }

//loc_49199:              ; CODE XREF: updateUserInputInScrollMovementMode+41j
    bx = 8;
    bx -= word_59B88;
    ax = word_59B8A;
    ax = ~ax;
    if (gIsNumpad0Pressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }

//loc_491B3:              ; CODE XREF: updateUserInputInScrollMovementMode+60j
    bx += 0x138; // 312
    if (gIsNumpad7Pressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }

//loc_491C5:              ; CODE XREF: updateUserInputInScrollMovementMode+72j
    bx += 0x138; // 312
    if (gIsNumpad9Pressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }

//loc_491D7:              ; CODE XREF: updateUserInputInScrollMovementMode+84j
    bx -= 0x270; // 312
    ax += 0xA8;
    if ((word_510C1 & 0xFF) != 0)
    {
        ax += 0x18;
    }

//loc_491E8:              ; CODE XREF: updateUserInputInScrollMovementMode+99j
    if (gIsNumpadPeriodPressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }

//loc_491F6:              ; CODE XREF: updateUserInputInScrollMovementMode+A3j
    bx += 0x138; // 312
    if (gIsNumpad1Pressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }

//loc_49208:              ; CODE XREF: updateUserInputInScrollMovementMode+B5j
    bx += 0x138; // 312
    if (gIsNumpad3Pressed != 0)
    {
        gAdditionalScrollOffsetX = bx;
        gAdditionalScrollOffsetY = ax;
    }
}

void sub_4921B() //   proc near       ; CODE XREF: readConfig+8Cp
                   // ; handleGameUserInput+31p ...
{
//    push    bp
    ax = 0;
    byte_50946 = 0;
    word_50947 = 0;
    word_50949 = 0;
    word_5094B = 0;
    word_5094D = 0;
    bp = 0;

    if (waitForJoystickKeyReleased(1, &ax) == 0)
    {
        return;
    }
    bp += ax;

    if (waitForJoystickKeyReleased(1, &ax) == 0)
    {
        return;
    }
    bp += ax;

    if (waitForJoystickKeyReleased(1, &ax) == 0)
    {
        return;
    }
    bp += ax;

    if (waitForJoystickKeyReleased(1, &ax) == 0)
    {
        return;
    }
    bp += ax;
    bp = bp >> 1;

    word_50947 = bp;
    dx = 0x10;
    ax = 0;
    if (bp <= dx)
    {
        return;
    }
//    div bp -> 0x01000000 comes from the dx=0x10 ax=0 above
    ax = 0x01000000 / bp;
    dx = 0x01000000 % bp;
    word_5094B = ax;
    bp = 0;
    if (waitForJoystickKeyReleased(2, &ax) == 0)
    {
        return;
    }
    bp += ax;
    if (waitForJoystickKeyReleased(2, &ax) == 0)
    {
        return;
    }
    bp += ax;
    if (waitForJoystickKeyReleased(2, &ax) == 0)
    {
        return;
    }
    bp += ax;
    if (waitForJoystickKeyReleased(2, &ax) == 0)
    {
        return;
    }
    bp += ax;
    bp = bp >> 1;
    word_50949 = bp;
    dx = 0x10;
    ax = 0;
    if (bp <= dx)
    {
        return;
    }
    //    div bp -> 0x01000000 comes from the dx=0x10 ax=0 above
    ax = 0x01000000 / bp;
    dx = 0x01000000 % bp;
    word_5094D = ax;
    al = 1;
    byte_50946 = al;

//loc_492A6:              ; CODE XREF: sub_4921B+19j
//                ; sub_4921B+22j ...
//    pop bp
    return;
}

void simulateDemoInput() // sub_492A8   proc near       ; CODE XREF: handleGameUserInput+27p
                   // ; sub_4A3E9+76p
{
    if (byte_510E2 > 1)
    {
        byte_510E2--;
        return;
    }

//loc_492B3:              ; CODE XREF: simulateDemoInput+5j
    uint8_t *demosAsByteArray = (uint8_t *)&gDemos;

    uint8_t newInput = demosAsByteArray[word_510DF];

    if (newInput == 0xFF)
    {
        gQuitLevelCountdown = 0x64;
        gShouldExitLevel = 1;
    }
    else
    {
        word_510DF++;
    }

//loc_492CA:              ; CODE XREF: simulateDemoInput+47j
    gCurrentUserInput = newInput & 0xF;
    byte_510E2 = (newInput >> 4) + 1;
}

void sub_492F1() //   proc near       ; CODE XREF: handleGameUserInput+1Dp
{
    byte_510E2++;
    bl = gCurrentUserInput;
    if (byte_510E2 == 0xFF)
    {
        byte_510E1 = bl;
        ax = gRandomGeneratorSeed;
        word_5A199 = ax;
        byte_59B5F = (ax >> 8); // ah;
        byte_59B5C = (ax & 0xFF); // al;
    }

//loc_49311:              ; CODE XREF: sub_492F1+Dj
    if (byte_510E1 == bl
        && byte_510E2 != 0xF)
    {
        return;
    }

//loc_4931E:              ; CODE XREF: sub_492F1+24j
    al = byte_510E1;
    ah = byte_510E2;
    ah = ah << 4;
    al |= ah;
    byte_510E1 = al;
    al = byte_510E2;
    byte_59B5C += al;
    byte_59B5C++;
//    mov ax, 4000h
//    mov bx, word_510E4
//    mov cx, 1
//    mov dx, 0DD1h
//    int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
    // TODO: Implement
//    fwrite(NULL, sizeof(uint8_t), 1, word_510E4);
    byte_510E2 = 0xFF;
    bl = gCurrentUserInput;
    byte_510E1 = bl;
}

void somethingspsig() //  proc near       ; CODE XREF: runLevel+355p
                     // ; sub_4945D+30p ...
{
    // TODO: Implement
    /*
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
    mov gIsRecordingDemo, 0
    cmp byte_5A33E, 0
    jz  short loc_4944F
    mov gIsPlayingDemo, 1

loc_4944F:              ; CODE XREF: somethingspsig+EEj
    call    drawGamePanelText
    mov byte_5A33F, 1
    mov gIsPlayingDemo, 0
    return;
     */
}

// TODO: Implement recording demo support
void sub_4945D() //   proc near       ; CODE XREF: handleGameUserInput+294p
                   // ; handleGameUserInput+2A4p ...
{
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;

    replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });

    if (gIsRecordingDemo != 0)
    {
        somethingspsig();
    }

//loc_49490:              ; CODE XREF: sub_4945D+2Ej
//    pop ax
    bx = 0x380A;
    al += 0x30;
//    bx[4] = al; // mov [bx+4], al
    if ((word_59B6E & 0xFF) == 0) // cmp byte ptr word_59B6E, 0
    {
        bx = 0xA014;
//        *(bx + 7) = al; // [bx+7], al
    }

//loc_494A6:              ; CODE XREF: sub_4945D+41j
    // aRecordingDemo0 has "--- RECORDING DEMO0 ---"
    // This code changes the "0" from "DEMO0" with another value
//    mov byte ptr aRecordingDemo0+12h, al ; "0 ---"
//    mov cx, 0
//    mov dx, bx
//    mov ax, 3C00h
//    int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
//                ; CX = attributes for file
//                ; DS:DX -> ASCIZ filename (may include drive and path)
    // TODO: Implement
    FILE *file = openWritableFile("some-name-probably-DEMO0", "w");
    if (file == NULL)
    {
        return;
    }

//loc_494B8:              ; CODE XREF: sub_4945D+56j
    word_510E4 = ax; // file handle
    byte_5A140 = 0x83; // 131
    // TODO: don't know for sure but this probably is related to adjusting the demo time with the speed or something?
    // bl = speed3;
    // cl = 4;
    // bl = bl << cl;
    // bl |= gGameSpeed;
    // speed2 = bl;
//    mov bx, word_510E4
//    mov ax, 4000h
//    mov cx, levelDataLength
//    mov dx, offset levelBuffer
//    int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
    uint8_t buffer[levelDataLength];
    size_t bytes = fwrite(buffer, levelDataLength, 1, file);
    if (bytes < levelDataLength)
    {
        return;
    }
    ax = gCurrentSelectedLevelIndex;
    al |= 0x80; // 128
    byte_510E2 = al;
//    mov ax, 4000h
//    mov bx, word_510E4
//    mov cx, 1
//    mov dx, 0DD2h // TODO: what is in 0xDD2??
//    int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
    uint8_t otherBuffer[1];
    bytes = fwrite(otherBuffer, 1, 1, file);
    if (bytes < 1)
    {
        return;
    }
    byte_510E1 = 0;
    byte_5A2F8 = 1;
    gIsPlayingDemo = 0;
    byte_510E2 = 0xFE; // 254
    gDebugExtraRenderDelay = 1;
    if (byte_599D4 == 0)
    {
        ax = word_58AB8;
        word_58AEA = ax;
        ax = word_58ABA;
        ax &= 0xFF;
        word_58AEC = ax;
    }

//loc_4952A:              ; CODE XREF: sub_4945D+BCj
    gIsRecordingDemo = 1;
    if (byte_5A33E != 0)
    {
        gIsPlayingDemo = 1;
    }

//loc_4953B:              ; CODE XREF: sub_4945D+D7j
    sub_4A463();
    gIsPlayingDemo = 0;
}

void prepareSomeKindOfLevelIdentifier() // sub_49544  proc near       ; CODE XREF: start+3A1p
                   // ; handleOkButtonClick:loc_4B40Fp ...
{
    // 01ED:28E1

    char char8 = gLevelsDatFilename[8];
    char char9 = gLevelsDatFilename[9];

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

void readFromFh1() // proc near       ; CODE XREF: handleGameUserInput+54Ep
                    //; handleGameUserInput+57Cp ...
{
//    mov ax, 3F00h
//    mov bx, fh1
//    int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to read
//                ; DS:DX -> buffer
}

void writeToFh1() //  proc near       ; CODE XREF: handleGameUserInput+486p
                    // ; handleGameUserInput+494p ...
{
//    mov ax, 4000h
//    mov bx, fh1
//    int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
//                ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
}

void handleGameUserInput() // sub_4955B   proc near       ; CODE XREF: runLevel:loc_48B6Bp
                   // ; runLevel+30Cp
{
    // 01ED:28F8

// ; FUNCTION CHUNK AT 2DA8 SIZE 0000038B BYTES

    if (gIsMoveScrollModeEnabled != 0)
    {
        updateUserInputInScrollMovementMode(); // 01ED:28FF
    }
//loc_49567:              ; CODE XREF: handleGameUserInput+5j
    else if (gIsPlayingDemo == 0)
    {
        updateUserInput(); // 01ED:290B
        if (gIsRecordingDemo != 0)
        {
            sub_492F1(); // 01ED:2915
        }
    }

//loc_4957B:              ; CODE XREF: handleGameUserInput+Aj
//                ; handleGameUserInput+11j ...
    if (gIsPlayingDemo != 0)
    {
        simulateDemoInput(); // 01ED:2929
    }

//loc_49585:              ; CODE XREF: handleGameUserInput+25j
    if (gIsJKeyPressed != 0)
    {
        sub_4921B();
    }

//loc_4958F:              ; CODE XREF: handleGameUserInput+2Fj
    if ((word_510C1 >> 8) != 0) // cmp byte ptr word_510C1+1, 0
    {
        // 01ED:293E
        uint8_t highValue = (word_510C1 >> 8);
        highValue--;
        word_510C1 = (highValue << 8) + (word_510C1 & 0xFF);
    }

//loc_4959A:              ; CODE XREF: handleGameUserInput+39j
    if (gIsEnterPressed == 0)
    {
        word_510C1 = word_510C1 & 0xFF; // mov byte ptr word_510C1+1, 0
    }
//loc_495A9:              ; CODE XREF: handleGameUserInput+44j
    else if ((word_510C1 >> 8) == 0) // 01ED:2946
    {
//loc_495B3:              ; CODE XREF: handleGameUserInput+53j
        word_510C1 = 0x2000 + (word_510C1 & 0xFF); // mov byte ptr word_510C1+1, 20h ; ' '
        if ((word_510C1 & 0xFF) != 0)
        {
            word_510C1 = (word_510C1 & 0xFF00); // mov byte ptr word_510C1, 0
            gCurrentPanelHeight = 0;
        }
        else
        {
//loc_495FB:              ; CODE XREF: handleGameUserInput+62j
            word_510C1 = (word_510C1 & 0xFF00) + 1; // mov byte ptr word_510C1, 1
            gCurrentPanelHeight = kPanelBitmapHeight;
        }
    }

//loc_49635:              ; CODE XREF: handleGameUserInput+4Bj
//                ; handleGameUserInput+55j ...
    if (gIsDebugModeEnabled != 1)
    {
        loc_49949();
        return;
    }

//loc_4963F:              ; CODE XREF: handleGameUserInput+DFj
    if (gIsRecordingDemo == 0) // 01ED:29DC
    {
//loc_49649:              ; CODE XREF: handleGameUserInput+E9j
        if (gIsMKeyPressed != 0) // cmp byte ptr gIsMKeyPressed, 0
        {
            gIsMoveScrollModeEnabled = 1;
        }

//loc_49656:              ; CODE XREF: handleGameUserInput+F3j
        if (gIsDKeyPressed != 0)
        {
            gIsFlashingBackgroundModeEnabled = 1;
        }

//loc_49663:              ; CODE XREF: handleGameUserInput+100j
        if (gIsZKeyPressed != 0) // cmp byte ptr gIsZKeyPressed, 0
        {
            // TODO: around 01ED:2A09 (check with ORIGINAL.EXE)
            removeTiles(LevelTileTypeZonk);
        }

//loc_4966F:              ; CODE XREF: handleGameUserInput+10Dj
        if (gIsBKeyPressed != 0) // cmp byte ptr gIsBKeyPressed, 0
        {
            removeTiles(LevelTileTypeBase);
        }

//loc_4967B:              ; CODE XREF: handleGameUserInput+119j
        if (gIsHKeyPressed != 0)
        {
            removeTiles(LevelTileTypeHardware);
        }

//loc_49687:              ; CODE XREF: handleGameUserInput+125j
        if (gIsCKeyPressed != 0) // cmp byte ptr gIsCKeyPressed, 0
        {
            removeTiles(LevelTileTypeChip);
        }

//loc_49693:              ; CODE XREF: handleGameUserInput+131j
        if (gIsSKeyPressed != 0)
        {
            removeTiles(LevelTileTypeSnikSnak);
        }

//loc_4969F:              ; CODE XREF: handleGameUserInput+13Dj
        if (gIsRKeyPressed != 0)
        {
            videoloop();
            sub_4A3E9();
        }

//loc_496AC:              ; CODE XREF: handleGameUserInput+149j
        if (gIsPlayingDemo == 0)
        {
//loc_496C0:              ; CODE XREF: handleGameUserInput+160j
            if (gIs1KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 1;
            }

//loc_496CD:              ; CODE XREF: handleGameUserInput+16Aj
            if (gIs2KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 2;
            }

//loc_496DA:              ; CODE XREF: handleGameUserInput+177j
            if (gIs3KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 3;
            }

//loc_496E7:              ; CODE XREF: handleGameUserInput+184j
            if (gIs4KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 4;
            }

//loc_496F4:              ; CODE XREF: handleGameUserInput+191j
            if (gIs5KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 6;
            }

//loc_49701:              ; CODE XREF: handleGameUserInput+19Ej
            if (gIs6KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 8;
            }

//loc_4970E:              ; CODE XREF: handleGameUserInput+1ABj
            if (gIs7KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 0xC;
            }

//loc_4971B:              ; CODE XREF: handleGameUserInput+1B8j
            if (gIs8KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 0x10;
            }

//loc_49728:              ; CODE XREF: handleGameUserInput+1C5j
            if (gIs9KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 0x18;
            }

//loc_49735:              ; CODE XREF: handleGameUserInput+1D2j
            if (gIs0KeyPressed != 0)
            {
                gDebugExtraRenderDelay = 0x20;
            }
        }
    }

//loc_49742:              ; CODE XREF: handleGameUserInput+EBj
//                ; handleGameUserInput+158j ...
    if (gIsLeftControlPressed == 1)
    {
//loc_497D1:              ; CODE XREF: handleGameUserInput+1EEj
        if (gIsPlayingDemo != 0)
        {
            loc_4988E();
            return;
        }
//loc_497DB:              ; CODE XREF: handleGameUserInput+27Bj
        // TODO: this if is only relevant when the game is autoadjusting the speed (speed3 != 0), right?
//        else if (speed3 < 0)
//        {
//            loc_4988E();
//            return;
//        }
//loc_497E5:              ; CODE XREF: handleGameUserInput+285j
        else if (gIsF1KeyPressed == 1)
        {
            ax = 0;
            sub_4945D();
        }
//loc_497F5:              ; CODE XREF: handleGameUserInput+28Fj
        else if (gIsF2KeyPressed == 1)
        {
            ax = 1;
            sub_4945D();
        }
//loc_49805:              ; CODE XREF: handleGameUserInput+29Fj
        else if (gIsF3KeyPressed == 1)
        {
            ax = 2;
            sub_4945D();
        }
//loc_49814:              ; CODE XREF: handleGameUserInput+2AFj
        else if (gIsF4KeyPressed == 1)
        {
            ax = 3;
            sub_4945D();
        }
//loc_49823:              ; CODE XREF: handleGameUserInput+2BEj
        else if (gIsF5KeyPressed == 1)
        {
            ax = 4;
            sub_4945D();
        }
//loc_49832:              ; CODE XREF: handleGameUserInput+2CDj
        else if (gIsF6KeyPressed == 1)
        {
            ax = 5;
            sub_4945D();
        }
//loc_49841:              ; CODE XREF: handleGameUserInput+2DCj
        else if (gIsF7KeyPressed == 1)
        {
            ax = 6;
            sub_4945D();
        }
//loc_49850:              ; CODE XREF: handleGameUserInput+2EBj
        else if (gIsF8KeyPressed == 1)
        {
            ax = 7;
            sub_4945D();
        }
//loc_4985F:              ; CODE XREF: handleGameUserInput+2FAj
        else if (gIsF9KeyPressed == 1)
        {
            ax = 8;
            sub_4945D();
        }
//loc_4986E:              ; CODE XREF: handleGameUserInput+309j
        else if (gIsF10KeyPressed == 1)
        {
            ax = 9;
            sub_4945D();
        }
//loc_4987D:              ; CODE XREF: handleGameUserInput+318j
        else if (byte_519D5 == 1
            && gIsRecordingDemo != 0)
        {
            somethingspsig();
        }
    }
    else
    {
//loc_4974C:              ; CODE XREF: handleGameUserInput+1ECj
        ax = word_5195D;
        ax &= 7;
        if (ax == 0
            && gIsRecordingDemo == 0)
        {
//loc_49761:              ; CODE XREF: handleGameUserInput+201j
            if (gIsF1KeyPressed == 0)
            {
                byte_59B7C = 0;
            }
//loc_4976F:              ; CODE XREF: handleGameUserInput+20Bj
            else if (byte_59B7C == 0)
            {
                byte_59B7C--;
                gIsGravityEnabled &= 1;
                gIsGravityEnabled = gIsGravityEnabled ^ 1;
            }

            if (gIsF1KeyPressed == 0
                || byte_59B7C != 0)
            {
//loc_49786:              ; CODE XREF: handleGameUserInput+212j
//                ; handleGameUserInput+219j
                if (gIsF2KeyPressed == 0)
                {
                    byte_59B7D = 0;
                }
//loc_49794:              ; CODE XREF: handleGameUserInput+230j
                else if (byte_59B7D == 0)
                {
                    byte_59B7D--;
                    gAreZonksFrozen &= 2;
                    gAreZonksFrozen = gAreZonksFrozen ^ 2;
                }

                if (gIsF2KeyPressed == 0
                    || byte_59B7D != 0)
                {
//loc_497AB:              ; CODE XREF: handleGameUserInput+237j
//                ; handleGameUserInput+23Ej
                    if (gIsF3KeyPressed == 0)
                    {
                        byte_59B7E = 0;
                    }
//loc_497B9:              ; CODE XREF: handleGameUserInput+255j
                    else if (byte_59B7E == 0)
                    {
                        byte_59B7E--;
                        gAreEnemiesFrozen &= 1;
                        gAreEnemiesFrozen = gAreEnemiesFrozen ^ 1;
                    }
                }
            }
        }
    }

    loc_4988E();
}

void loc_4988E() // :              ; CODE XREF: handleGameUserInput+1F9j
{
//                ; handleGameUserInput+203j ...
    if (gIsRecordingDemo != 0
        || gIsPlayingDemo != 0)
    {
        loc_49949();
        return;
    }

//loc_498A2:              ; CODE XREF: handleGameUserInput+342j
    if (gIsMinusKeyPressed == 0)
    {
        byte_59B7F = 0;
        byte_59B80 = 5;
    }
//loc_498B5:              ; CODE XREF: handleGameUserInput+34Cj
    else if (byte_59B7F != 0)
    {
        byte_59B7F--;
    }
    else
    {
//loc_498C2:              ; CODE XREF: handleGameUserInput+35Fj
        if (byte_59B80 != 0)
        {
            byte_59B80--;
            byte_59B7F = 0x10;
        }

//loc_498D2:              ; CODE XREF: handleGameUserInput+36Cj
        if (gCurrentSelectedLevelIndex <= 1)
        {
            gCurrentSelectedLevelIndex = 2;
        }

//loc_498DF:              ; CODE XREF: handleGameUserInput+37Cj
        gCurrentSelectedLevelIndex--;
        if (gCurrentSelectedLevelIndex > kNumberOfLevels)
        {
            gCurrentSelectedLevelIndex = kNumberOfLevels;
        }

//loc_498F0:              ; CODE XREF: handleGameUserInput+38Dj
        // ax = gCurrentSelectedLevelIndex;
        sub_4BF4A(gCurrentSelectedLevelIndex);
        drawLevelList();
        sub_4A3D2();
    }

//loc_498FC:              ; CODE XREF: handleGameUserInput+358j
//                ; handleGameUserInput+365j
    if (gIsEqualsKeyPressed == 0)
    {
        byte_59B81 = 0;
        byte_59B82 = 5;
    }
//loc_4990F:              ; CODE XREF: handleGameUserInput+3A6j
    else if (byte_59B81 != 0)
    {
        byte_59B81--;
    }
    else
    {
//loc_4991C:              ; CODE XREF: handleGameUserInput+3B9j
        if (byte_59B82 != 0)
        {
            byte_59B82--;
            byte_59B81 = 0x10; // 16
        }

//loc_4992C:              ; CODE XREF: handleGameUserInput+3C6j
        if (gCurrentSelectedLevelIndex >= kNumberOfLevels)
        {
            gCurrentSelectedLevelIndex = kNumberOfLevels;
        }

//loc_49939:              ; CODE XREF: handleGameUserInput+3D6j
        gCurrentSelectedLevelIndex++;
    //    ax = gCurrentSelectedLevelIndex;
        sub_4BF4A(gCurrentSelectedLevelIndex); // 01ED:2CDD
        drawLevelList();
        sub_4A3D2();
    }

    loc_49949();
}

void loc_49949() //:              ; CODE XREF: handleGameUserInput+E1j
//                ; handleGameUserInput+33Aj ...
{
    // 01ED:2CE6

    uint8_t was_byte_59B6B_NonZero = (byte_59B6B != 0);
    byte_59B6B = 0;
    if (was_byte_59B6B_NonZero)
    {
        loc_49A89();
        return;
    }

//loc_49958:              ; CODE XREF: handleGameUserInput+3F8j
    if (gIsLeftControlPressed != 1)
    {
        loc_49C41();
        return;
    }

//loc_49962:              ; CODE XREF: handleGameUserInput+402j
    if (byte_519D5 == 1
        && gIsPlayingDemo != 0)
    {
        gIsPlayingDemo = 0;
        byte_510B3 = 0;
        byte_5A2F9 = 1;
        byte_5A33E = 1;
    }

//loc_49984:              ; CODE XREF: handleGameUserInput+40Cj
//                ; handleGameUserInput+413j
    if (gIsScrollLockPressed == 1)
    {
        gIsDebugModeEnabled = 1;
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 304, 14, 6, "DB"); // Debug mode enabled
        byte_5197C = 0x46; // 70
    }

//loc_499AA:              ; CODE XREF: handleGameUserInput+42Ej
//                ; handleGameUserInput+43Bj
    if (gIsWKeyPressed != 1)
    {
//loc_49A7F:              ; CODE XREF: handleGameUserInput+456j
        if (gIsLKeyPressed != 1)
        {
            loc_49C41();
            return;
        }

        loc_49A89();
        return;
    }

//loc_499C8:              ; CODE XREF: handleGameUserInput+454j
    FILE *file = openWritableFile("SAVEGAME.SAV", "w");
    if (file == NULL)
    {
        loc_49C28();
        return;
    }

//loc_499D8:              ; CODE XREF: handleGameUserInput+478j
//    mov dx, 9FF9h // address of data to write
    size_t bytes = fwrite(NULL, 1, 4, file);
    if (bytes < 4)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_499E9:              ; CODE XREF: handleGameUserInput+489j
//    mov cx, 1238h
//    mov dx, offset leveldata // address of data to write
    bytes = fwrite(NULL, 1, 0x1238, file);
    if (bytes < 0x1238)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_499F7:              ; CODE XREF: handleGameUserInput+497j
    if (gIsPlayingDemo == 0)
    {
//        mov dx, 87A8h -> this has the level name, like "005 ------ EASY DEAL ------"
    }
    else
    {
//loc_49A03:              ; CODE XREF: handleGameUserInput+4A1j
//        mov dx, 87DAh -> this address is the ".SP" text
    }

//loc_49A06:              ; CODE XREF: handleGameUserInput+4A6j
//    mov cx, 1Ch
//    push    dx
    bytes = fwrite(NULL, 1, 0x1C, file); // 28, level name?
//    pop bx // recovers dx and stores it into bx
    if (bytes < 0x1C)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49A13:              ; CODE XREF: handleGameUserInput+4B3j
    cx = 6;
    byte_5988D = 0x63; // 99 or 'c'
//    ax = "AT"; // mov ax, word ptr aLevels_dat_0+8 ; "AT"
    word_5988E = ax;
//    al = [bx] // bx here is the value dx took from loc_499F7 or loc_49A03
    if (gIsPlayingDemo != 0)
    {
        al |= 0x80;
    }

//loc_49A2C:              ; CODE XREF: handleGameUserInput+4CDj
    byte_59890 = al;
//    ax = [bx+1];
    word_59891 = ax;
//    mov dx, 957Dh
    bytes = fwrite(NULL, 1, 6, file);
    if (bytes < 6)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49A40:              ; CODE XREF: handleGameUserInput+4E0j
//    mov cx, 0E6h ; '?'
//    mov dx, 0D08h
//    call    writeToFh1
    bytes = fwrite(NULL, 1, 0xE6, file); // 230
    if (bytes < 0xE6)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49A4E:              ; CODE XREF: handleGameUserInput+4EEj
//    mov cx, 23h ; '#'
//    mov dx, 164Ah
    bytes = fwrite(NULL, 1, 0x23, file); // 35
    if (bytes < 0x23)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49A5C:              ; CODE XREF: handleGameUserInput+4FCj
//    mov cx, levelDataLength
//    mov dx, offset levelBuffer
//    call    writeToFh1
    bytes = fwrite(NULL, 1, levelDataLength, file); // 35
    if (bytes < levelDataLength)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49A6A:              ; CODE XREF: handleGameUserInput+50Aj
    if (fclose(file) != 0)
    {
        loc_49C28();
        return;
    }

//loc_49A78:              ; CODE XREF: handleGameUserInput+518j
//    push    si
//    si = 0xA001; // "WR"
    loc_49C2C("WR"); // Means snapshot saved with no issues
    return;
}

void loc_49A89() // :              ; CODE XREF: handleGameUserInput+3FAj
{
//                    ; handleGameUserInput+529j
    FILE *file = openWritableFile("SAVEGAME.SAV", "r");
    if (file == NULL)
    {
        loc_49C28();
        return;
    }

//loc_49A96:              ; CODE XREF: handleGameUserInput+536j
//    mov fh1, ax
    if (gIsRecordingDemo != 0)
    {
        somethingspsig();
    }

//loc_49AA3:              ; CODE XREF: handleGameUserInput+543j
//    mov cx, 4
//    mov dx, 9FFDh
//    call    readFromFh1
    size_t bytes = fread(NULL, 1, 4, file);
    if (bytes < 4)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49AB1:              ; CODE XREF: handleGameUserInput+551j
    cx = word_5A30D;
    if (cx == word_5A309)
    {
        cx = word_5A30F;
        if (cx == word_5A30B)
        {
//loc_49AD1:              ; CODE XREF: handleGameUserInput+568j
//            mov cx, 1238h
//            mov dx, offset leveldata
//            call    readFromFh1
            bytes = fread(NULL, 1, 0x1238, file);
            if (bytes < 0x1238)
            {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
                gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
                fclose(file);
                loc_49C28();
                return;
            }
        }
        else
        {
//loc_49AC5:              ; CODE XREF: handleGameUserInput+55Ej
            fclose(file);
            loc_49C28();
            return;
        }
    }
    else
    {
//loc_49AC5:              ; CODE XREF: handleGameUserInput+55Ej
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49ADF:              ; CODE XREF: handleGameUserInput+57Fj
//    mov cx, 1Ch
//    mov dx, 87A8h
//    call    readFromFh1
    bytes = fread(NULL, 1, 0x1C, file);
    if (bytes < 0x1C)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49AED:              ; CODE XREF: handleGameUserInput+58Dj
//    mov cx, 6
//    mov dx, 957Dh
//    call    readFromFh1
    bytes = fread(NULL, 1, 6, file);
    if (bytes < 6)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49AFB:              ; CODE XREF: handleGameUserInput+59Bj
//    mov cx, 84h ; '?'
//    mov dx, 0D08h
//    call    readFromFh1
    bytes = fread(NULL, 1, 0x84, file);
    if (bytes < 0x84)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B09:              ; CODE XREF: handleGameUserInput+5A9j
//    mov cx, 4
//    mov dx, 0D9Bh
//    call    readFromFh1
    bytes = fread(NULL, 1, 4, file);
    if (bytes < 4)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B17:              ; CODE XREF: handleGameUserInput+5B7j
//    mov cx, 7
//    mov dx, 0D90h
//    call    readFromFh1
    bytes = fread(NULL, 1, 7, file);
    if (bytes < 7)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B25:              ; CODE XREF: handleGameUserInput+5C5j
//    mov cx, 4
//    mov dx, 0D9Bh
//    call    readFromFh1
    bytes = fread(NULL, 1, 4, file);
    if (bytes < 4)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B33:              ; CODE XREF: handleGameUserInput+5D3j
//    mov cx, 53h ; 'S'
//    mov dx, 0D9Bh
//    push    word_510C1
//    call    readFromFh1
    bytes = fread(NULL, 1, 0x53, file); // 83
//    pop word_510C1
    if (bytes < 0x53)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B49:              ; CODE XREF: handleGameUserInput+5E9j
//    mov cx, 23h ; '#'
//    mov dx, 164Ah
//    push    gScrollOffsetY
//    push    word_51967
//    push    gIsDebugModeEnabled
//    call    readFromFh1
    bytes = fread(NULL, 1, 0x23, file); // 35
//    pop gIsDebugModeEnabled
//    pop word_51967
//    pop gScrollOffsetY
    if (bytes < 0x23)
    {
//loc_49C1A:              ; CODE XREF: handleGameUserInput+553j
//                    ; handleGameUserInput+581j ...
        gIsRecordingDemo = 0;

//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        loc_49C28();
        return;
    }

//loc_49B6F:              ; CODE XREF: handleGameUserInput+60Fj
    if (byte_5988D != 0)
    {
//        mov cx, levelDataLength
//        mov dx, offset levelBuffer
//        call    readFromFh1
        bytes = fread(NULL, 1, levelDataLength, file); // 35
        if (bytes == levelDataLength)
        {
            sub_49D53();
        }
    }

//loc_49B84:              ; CODE XREF: handleGameUserInput+619j
//                    ; handleGameUserInput+624j
    fclose(file);
    gIsPlayingDemo = 0;
    gIsRecordingDemo = 0;
    gCurrentUserInput = UserInputNone;
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });
    generateRandomSeedFromClock();
    generateRandomNumber();
//    mov si, 60D5h
    setPalette(gBlackPalette);
    drawFixedLevel();
    drawGamePanel();
    sub_4A2E6();
//    si = gMurphyLocation;
    ax = si;
    ax = ax >> 1;
    sub_4A291();
    ax = 0xFFFF;
    gLastDrawnMinutesAndSeconds = ax;
    gLastDrawnHours = al;
    drawGameTime();
    byte_5A2F9 = 1;
    gIsRecordingDemo = 0;

    drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 304, 14, 6, "LD"); // Means snapshot was loaded successfully
    byte_5197C = 0x46; // 70 or '&'

//loc_49C12:              ; CODE XREF: handleGameUserInput+6A8j
//    mov si, 6015h
    fadeToPalette(gPalettes[1]);

//loc_49C40:              ; CODE XREF: handleGameUserInput+6BDj
//                    ; handleGameUserInput+6D6j
//    pop si

    loc_49C41();
}

void loc_49C28() //:              ; CODE XREF: handleGameUserInput+47Aj
{
//                    ; handleGameUserInput+51Aj ...
//    push    si
//    mov si, 0A007h "XX"
    loc_49C2C("XX"); // Means problem writing/loading snapshot
}

void loc_49C2C(char text[3]) // :              ; CODE XREF: handleGameUserInput+521j
{
    drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 304, 14, 6, text);
    byte_5197C = 0x46; // 70 or '&'

//loc_49C40:              ; CODE XREF: handleGameUserInput+6BDj
//                    ; handleGameUserInput+6D6j
//    pop si

    loc_49C41();
}

void loc_49C41() //              ; CODE XREF: handleGameUserInput+404j
//                    ; handleGameUserInput+52Bj
{
    if (gIsLeftAltPressed == 1
        && gIsScrollLockPressed == 1)
    {
        // 01ED:2FEC
        gIsDebugModeEnabled = 0;
        gIsMoveScrollModeEnabled = 0;
        gAdditionalScrollOffsetX = 0;
        gAdditionalScrollOffsetY = 0;
        gIsFlashingBackgroundModeEnabled = 0;
        gDebugExtraRenderDelay = 1;
        replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });

        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 304, 14, 6, "--"); // Debug mode disabled
        byte_5197C = 0x46; // 70 or '&'
    }

//loc_49C96:              ; CODE XREF: handleGameUserInput+6EBj
//                    ; handleGameUserInput+6F2j ...
    if (gIsPKeyPressed != 0)
    {
        // 01ED:303A
        gIsGamePaused = 0;
//        mov si, 6095h
        fadeToPalette(gPalettes[3]);

        do
        {
//loc_49CA8:              ; CODE XREF: handleGameUserInput+752j
            int9handler(1);
        }
        while (gIsPKeyPressed == 1);

        do
        {
//loc_49CAF:              ; CODE XREF: handleGameUserInput+759j
            int9handler(1);
        }
        while (gIsPKeyPressed == 0);

        do
        {
//loc_49CB6:              ; CODE XREF: handleGameUserInput+760j
            int9handler(1);
        }
        while (gIsPKeyPressed == 1);
//            mov si, 6015h
        fadeToPalette(gPalettes[1]);
        gIsGamePaused = 1;
    }

//loc_49CC8:              ; CODE XREF: handleGameUserInput+740j
    if (gIsNumLockPressed != 0)
    {
        // 01ED:306C
        gIsGamePaused = 0;
//        mov si, 6095h
        fadeToPalette(gPalettes[3]);

        do
        {
//loc_49CDA:              ; CODE XREF: handleGameUserInput+784j
            int9handler(1);
        }
        while (gIsNumLockPressed == 1);

        // From the speed fix mod, but in uppercase so I can use characterForSDLScancode
        static const char kMagicDisableDebugModeCode[] = "CANT STO";
        uint8_t index = 0;

        do
        {
//loc_49CE4:              ; CODE XREF: handleGameUserInput+7A6j
            int9handler(1);

            if (index >= strlen(kMagicDisableDebugModeCode))
            {
                gIsDebugModeEnabled = 0;
                break;
            }
            else
            {
//loc_49CF3:              ; CODE XREF: handleGameUserInput+78Ej
                if (characterForSDLScancode(keyPressed) == kMagicDisableDebugModeCode[index])
                {
                    index++;
                }

//loc_49CFC:              ; CODE XREF: handleGameUserInput+79Dj
                if (gIsNumLockPressed != 0)
                {
                    break;
                }
            }
        }
        while (1);

        do
        {
//loc_49D03:              ; CODE XREF: handleGameUserInput+796j
//                    ; handleGameUserInput+7ADj
            int9handler(1);
        }
        while (gIsNumLockPressed == 1);
//        mov si, 6015h
        fadeToPalette(gPalettes[1]);
        gIsGamePaused = 1;
    }

//loc_49D15:              ; CODE XREF: handleGameUserInput+772j
    if (gIsEscapeKeyPressed != 0
        && gQuitLevelCountdown <= 0)
    {
        // This is called when I press ESC to exit the game, but not when I die
        word_510D1 = 1; // 01ED:30C0
    }

//loc_49D29:              ; CODE XREF: handleGameUserInput+7BFj
//                    ; handleGameUserInput+7C6j
    if (gIsQKeyPressed != 0)
    {
        // 01ED:30CD
        gIsMoveScrollModeEnabled = 0;
        gAdditionalScrollOffsetX = 0;
        gAdditionalScrollOffsetY = 0;
        gIsFlashingBackgroundModeEnabled = 0;
    }

//loc_49D48:              ; CODE XREF: handleGameUserInput+7D3j
    if (gIsRightShiftPressed != 0)
    {
        drawNumberOfRemainingRedDisks(); // 01ED:30EC
    }
}

void sub_49D53() //   proc near       ; CODE XREF: handleGameUserInput+626p
                   // ; removeTiles+21p
{
    // TODO: around 01ED:30F0 (check with ORIGINAL.EXE)
    byte_59B7B = 0;
    levelScanThing(); // added by me, seems like code continues from here? see what happens with the debugger
}

void levelScanThing() //   proc near       ; CODE XREF: runLevel+A7p
{
    // TODO: around 01ED:30F5 (check with ORIGINAL.EXE)

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49D65:              ; CODE XREF: levelScanThing+18j
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        if (tile->tile == LevelTileTypeExplosion) // 31
        {
            tile->tile = 0xF1; // 241
        }
    }
    uint8_t was_byte_59B7B_NonZero = (byte_59B7B != 0);
    byte_59B7B = 0;
    if (was_byte_59B7B_NonZero)
    {
        return;
    }

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49D84:              ; CODE XREF: levelScanThing+4Cj
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        if (tile->movingObject != 0 || tile->tile != LevelTileTypeHardware)
        {
            continue;
        }

        LevelTileType originalTile = gCurrentLevel.tiles[i];

        if (originalTile >= LevelTileTypeHardware2 // 28
            && originalTile <= LevelTileTypeHardware11) // 37
        {
            tile->tile = originalTile;
            tile->movingObject = 0;
        }
    }

//loc_49DA6:              ; CODE XREF: levelScanThing+31j
    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49DAC:              ; CODE XREF: levelScanThing+7Fj
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        if (tile->movingObject != 0 || tile->tile != LevelTileTypeChip)
        {
            continue;
        }

        LevelTileType originalTile = gCurrentLevel.tiles[i];

        if (originalTile >= LevelTileTypeHorizontalChipLeft // 26
            && originalTile <= LevelTileTypeHorizontalChipBottom) // 39
        {
            originalTile -= LevelTileTypeHardware2; // 28
            if (originalTile >= LevelTileTypePortDown) // 10
            {
                originalTile += LevelTileTypeHardware2; // 28
                tile->tile = originalTile;
                tile->movingObject = 0;
            }
        }
    }

//loc_49DD9:              ; CODE XREF: levelScanThing+59j
    byte_59B7B = 1;
}

void updateMovingObjects() // gameloop   proc near       ; CODE XREF: runLevel:noFlashingp
{
    // 01ED:317D

    gMurphyLocation = updateMurphy(gMurphyLocation); // 01ED:318B

    if (gIsFlashingBackgroundModeEnabled != 0)
    {
        replaceCurrentPaletteColor(0, (SDL_Color) { 0x3f, 0x3f, 0x21 });
    }

//loc_49E14:
    if (gIsFlashingBackgroundModeEnabled != 0)
    {
        replaceCurrentPaletteColor(0, (SDL_Color) { 0x3f, 0x21, 0x21 });
    }

//loc_49E33:
    uint16_t numberOfMovingObjects = 0;

    typedef struct {
        MovingFunction function;
        uint16_t tilePosition;
    } MovingObject;

    MovingObject movingObjects[kLevelSize];

    for (uint16_t i = kLevelWidth + 1; i < kLevelSize; ++i) // starts from si, ends in si + cx
    {
//checkCellForMovingObject:              ; CODE XREF: updateMovingObjects+84j
        LevelTileType tile = gCurrentLevelWord[i].tile; //         mov bl, byte ptr leveldata[si]

        // Does this check filter out values except like 0, 2, 16 and 18??
        if ((tile & LevelTileTypeSportRight) == 0)
        {
            continue;
        }

        if (tile >= 0x20) // 32 because there are 32 moving functions, which means this is about moving objects (scissors, the infotrons, etc.)
        {
            continue;
        }

        MovingFunction function = movingFunctions[tile];

        if (function != NULL)
        {
            // There is a list of predefined functions, one per object, and this goes through
            // every tile, looking for objects with a moving function, and then putting the
            // tile and the function in a list that will be iterated later to update those objects.
            //
            MovingObject *object = &movingObjects[numberOfMovingObjects];
            object->function = function;
            object->tilePosition = i;
            numberOfMovingObjects++; //dx++;
        }
//moveToNextCell:
    }

    if (numberOfMovingObjects != 0)
    {
        // Call all the moving functions
        for (uint16_t i = 0; i < numberOfMovingObjects; ++i)
        {
            // 01ED:3214
            movingObjects[i].function(movingObjects[i].tilePosition);
        }
    }

//doneWithupdateMovingObjects:
    // 01ED:321D
//    ; set graphics write mode = 0
//    mov dx, 3CEh
//    al = 5
//    out dx, al
//    inc dx
//    al = 0
//    out dx,

    // 01ED:3227
    if (word_510D1 != 1
        && word_510CF != 0)
    {
        return;
    }

//loc_49E99:              ; CODE XREF: updateMovingObjects+AFj
//                ; updateMovingObjects+B6j
    if (gQuitLevelCountdown == 0) // 01ED:3236
    {
        // 01ED:323D
        word_510D1 = 0;
//        si = word_510C7;
        detonateBigExplosion(word_510C7); // could use gMurphyLocation too?
        gQuitLevelCountdown = 0x40; // 64
    }

//loc_49EB3:
//    ; set graphics write mode = 0
//    mov dx, 3CEh
//    al = 5
//    out dx, al
//    inc dx
//    al = 1
//    out dx, al

    return;
}

void updateScrollOffset() // sub_49EBE   proc near       ; CODE XREF: runLevel+109p
                   // ; sub_4A291+29p
{
    // 01ED:325B

//loc_49ECC:              ; CODE XREF: updateScrollOffset+7j
    int16_t scrollX = gMurphyPositionX;
    int16_t scrollY = gMurphyPositionY;
    scrollX -= kScreenWidth / 2; // 152
    if (scrollX < 0)
    {
        scrollX = 0;
    }

//loc_49EDF:              ; CODE XREF: updateScrollOffset+1Cj
    uint16_t maxScrollX = kLevelBitmapWidth - kScreenWidth;
    if (scrollX > maxScrollX) // 624
    {
        scrollX = maxScrollX; // 624
    }

//loc_49EE8:              ; CODE XREF: updateScrollOffset+25j
    if ((word_510C1 & 0xFF) == 0)
    {
//loc_49EF4:              ; CODE XREF: updateScrollOffset+2Fj
        scrollY -= kScreenHeight / 2;
    }
    else
    {
        scrollY -= (kScreenHeight - kPanelBitmapHeight) / 2;
    }

//loc_49EF7:              ; CODE XREF: updateScrollOffset+34j
    if (scrollY < 0)
    {
        scrollY = 0;
    }

//loc_49EFE:              ; CODE XREF: updateScrollOffset+3Cj
    uint16_t maxScrollY = 0;

    if ((word_510C1 & 0xFF) == 0)
    {
//loc_49F0F:              ; CODE XREF: updateScrollOffset+45j
        maxScrollY = kLevelBitmapHeight - kScreenHeight;
        if (scrollY > maxScrollY)
        {
            scrollY = maxScrollY;
        }
    }
    else
    {
        maxScrollY = kLevelBitmapHeight - kScreenHeight + kPanelBitmapHeight;
        if (scrollY > maxScrollY)
        {
            scrollY = maxScrollY;
        }
    }

//loc_49F17:              ; CODE XREF: updateScrollOffset:loc_49F0Dj
//                ; updateScrollOffset+54j
    if (gIsMoveScrollModeEnabled == 0
        || gIsNumpad5Pressed != 0)
    {
//loc_49F25:              ; CODE XREF: updateScrollOffset+5Ej
        word_59B88 = scrollX;
        word_59B8A = scrollY;
    }
    else
    {
//loc_49F2E:              ; CODE XREF: updateScrollOffset+65j
        scrollX = word_59B88;
        scrollY = word_59B8A;

        int16_t additionalScrollX = scrollX;
        scrollX += gAdditionalScrollOffsetX;
        if (scrollX < 0)
        {
            // 01ED:32DD
            scrollX = 0;
        }
        else
        {
//loc_49F56:              ; CODE XREF: updateScrollOffset+80j
            if (scrollX > maxScrollX)
            {
                scrollX = maxScrollX;
            }
        }

//loc_49F66:              ; CODE XREF: updateScrollOffset+8Cj
//                ; updateScrollOffset+96j
        additionalScrollX -= scrollX;
        additionalScrollX = -additionalScrollX;
        gAdditionalScrollOffsetX = additionalScrollX;

//loc_49F6E:              ; CODE XREF: updateScrollOffset+9Dj
//                ; updateScrollOffset+A3j
        int16_t additionalScrollY = scrollY;
        scrollY += gAdditionalScrollOffsetY;
        if (scrollY < 0) // in asm there wasn't a explicit "cmp", just the "add" above
        {
            scrollY = 0;
        }
        else
        {
//loc_49F99:              ; CODE XREF: updateScrollOffset+CFj
            if (scrollY > maxScrollY) // 168
            {
                scrollY = maxScrollY; // 168
            }
        }

//loc_49FA1:              ; CODE XREF: updateScrollOffset+C1j
//                ; updateScrollOffset+D9j
        additionalScrollY -= scrollY;
        additionalScrollY = -additionalScrollY;
        gAdditionalScrollOffsetY = additionalScrollY;
    }

//loc_49FA9:              ; CODE XREF: updateScrollOffset+6Ej
//                ; updateScrollOffset+BDj ...
    // This makes the screen shake when Murphy dies
    if (byte_59B72 != 0
        || (byte_59B6D == 0
            && (gQuitLevelCountdown & 0xFF) != 0))
    {
//loc_49FBE:              ; CODE XREF: updateScrollOffset+F0j
        uint16_t randomNumber = 0;

        // This makes the shaking effect stop after the explosion is finished
        if (gIsExplosionStarted == 1)
        {
            randomNumber = generateRandomNumber();
        }

        randomNumber = randomNumber & 0x101;

        uint16_t scrollShakeYOffset = randomNumber >> 8;
        uint16_t scrollShakeXOffset = (randomNumber & 0xFF);

        scrollY += scrollShakeYOffset;
        if (scrollX > 0x13C) // 316
        {
            scrollShakeXOffset = ~scrollShakeXOffset;
        }

//loc_49FD0:              ; CODE XREF: updateScrollOffset+10Ej
        scrollX += scrollShakeXOffset;
    }

//loc_49FD2:              ; CODE XREF: updateScrollOffset+F7j
//                ; updateScrollOffset+FEj
    gScrollOffsetX = scrollX;
    gScrollOffsetY = scrollY;
    cl = 3;
    scrollX = scrollX >> cl;
    cx = 0x7A; // 122
    scrollY = scrollY * cx;
    scrollX += scrollY;
    scrollX += 0x4D34;
    word_51967 = scrollX;
}

uint8_t waitForJoystickKeyReleased(uint8_t keyOrAxis, uint16_t *outTime) // sub_49FED  proc near       ; CODE XREF: sub_48E59+2Cp
                   // ; sub_48E59+67p ...
{
    // Maybe it waits for a key in the joystick to be released???
    // Parameters:
    // ah: bitmask of the button/coordiante to test
    // Returns:
    // CF=0 if the key was initially pressed and then released, CF=1 in any other case
    // ax: will contain the number of iterations where the key was pressed?

    if (isJoystickEnabled == 0)
    {
//        ax = 0;
//        cf = 1; // stc
        *outTime = 0;
        return 0;
    }

//loc_49FF8:              ; CODE XREF: waitForJoystickKeyReleased+5j
    dx = 0x201; // 513
//    cli
    cx = 0;

    char keyWasPressed = 1;

    // This loop waits until the key is not pressed?
    do
    {
//loc_49FFF:              ; CODE XREF: waitForJoystickKeyReleased+21j
        // 2 possible joysticks: (X1, Y1, 11, 12) and (X2, Y2, 21, 22)
    //    in  al, dx      ; Game I/O port
    //                ; bits 0-3: Coordinates (resistive, time-dependent inputs) X1, Y1, X2, Y2
    //                ; bits 4-7: Buttons/Triggers (digital inputs) 11, 12, 21, 12
        if ((al & keyOrAxis) == 0)
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
        return 0;
    }

//loc_4A013:              ; CODE XREF: waitForJoystickKeyReleased+1Fj
//    out dx, al      ; Game I/O port
//                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
//                ; bits 4-7: Buttons/Triggers (digital inputs)
    cx = 0;

    do
    {
//loc_4A021:              ; CODE XREF: waitForJoystickKeyReleased+44j
    //    in  al, dx      ; Game I/O port
    //                ; bits 0-3: Coordinates (resistive, time-dependent inputs)
    //                ; bits 4-7: Buttons/Triggers (digital inputs)
        if ((al & keyOrAxis) == 0)
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
        return 0;
    }

//loc_4A036:              ; CODE XREF: waitForJoystickKeyReleased+42j
//    cx = -cx;
//    ax = cx;
//    cf = 0; // clc
    *outTime = ~cx;
    return 1;
}

void updateBugTiles(uint16_t position) // movefun7  proc near       ; DATA XREF: data:163Co
{
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *aboveLeftTile = &gCurrentLevelWord[position - kLevelWidth - 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];
    MovingLevelTile *belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
    MovingLevelTile *belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeBug)
    {
        return;
    }

//loc_4A045:              ; CODE XREF: movefun7+5j
    if ((word_5195D & 3) != 0)
    {
        return;
    }

//loc_4A051:              ; CODE XREF: movefun7+11j
    int8_t frameNumber = currentTile->movingObject;
    frameNumber++;
    if (frameNumber >= 0xE)
    {
        uint8_t value = generateRandomNumber() & 0xFF;
        value &= 0x3F;
        value += 0x20;
        value = ~value;
        frameNumber = value;
    }

//loc_4A067:              ; CODE XREF: movefun7+1Dj
    currentTile->movingObject = frameNumber;
    if (frameNumber < 0)
    {
        return;
    }

//loc_4A071:              ; CODE XREF: movefun7+31j
    if (aboveLeftTile->tile == LevelTileTypeMurphy
        || aboveTile->tile == LevelTileTypeMurphy
        || aboveRightTile->tile == LevelTileTypeMurphy
        || leftTile->tile == LevelTileTypeMurphy
        || rightTile->tile == LevelTileTypeMurphy
        || belowLeftTile->tile == LevelTileTypeMurphy
        || belowTile->tile == LevelTileTypeMurphy
        || belowRightTile->tile == LevelTileTypeMurphy)
    {
//loc_4A0AB:              ; CODE XREF: movefun7+39j
//                ; movefun7+40j ...
        sound8();
    }

//loc_4A0AE:              ; CODE XREF: movefun7+6Cj
    Point frameCoordinates = kBugFrameCoordinates[frameNumber];
    drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
}

void updateTerminalTiles(uint16_t position) // movefun5  proc near       ; DATA XREF: data:1630o
{
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];

    if (currentTile->tile != LevelTileTypeTerminal)
    {
        return;
    }

//loc_4A0DA:              ; CODE XREF: updateTerminalTiles+5j
    int8_t movingObject = currentTile->movingObject;
    movingObject++;
    if (movingObject <= 0)
    {
        currentTile->movingObject = movingObject;
        return;
    }

//loc_4A0EA:              ; CODE XREF: updateTerminalTiles+11j
    uint8_t value = generateRandomNumber() & 0xFF;
    value &= byte_5196A;
    value = ~value;
    currentTile->movingObject = value;

    // This code basically simulates a scroll effect in the terminal:
    // copies the row 2 in the row 10, and then row 3 in 2, 4 in 3...
    //
    uint16_t tileX = (position % kLevelWidth) - 1;
    uint16_t tileY = (position / kLevelWidth) - 1;

    uint16_t x = kLevelEdgeSize + tileX * kTileSize;
    uint16_t y = kLevelEdgeSize + tileY * kTileSize;

    uint32_t source = 0;
    uint32_t dest = 0;

    source = dest = y * kLevelBitmapWidth + x;

    dest += kLevelBitmapWidth * 10;
    source += kLevelBitmapWidth * 2;
    memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);

    dest -= kLevelBitmapWidth * 8;
    source += kLevelBitmapWidth;

    for (int i = 0; i < 9; ++i)
    {
        memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);
        dest += kLevelBitmapWidth;
        source += kLevelBitmapWidth;
    }
}

/// Updates the random seed using the clock
void generateRandomSeedFromClock() // getTime    proc near       ; CODE XREF: start:doesNotHaveCommandLinep
                    // ; handleGameUserInput+669p ...
{
//        mov ax, 0
//        int 1Ah     ; CLOCK - GET TIME OF DAY
//                    ; Return: CX:DX = clock count
//                    ; AL = 00h if clock was read or written (via AH=0,1) since the previous
//                    ; midnight
//                    ; Otherwise, AL > 0
//    xor cx, dx
//    mov gRandomGeneratorSeed, cx
    Uint32 timeInMilliseconds = SDL_GetTicks();
    // In order to keep the same behavior and values, this code will convert
    // the time in milliseconds to the clock count, as described in
    // http://vitaly_filatov.tripod.com/ng/asm/asm_029.1.html
    // If 1 second is 18.2 clock counts, we need to divide the time
    // by 1000 to get the seconds, and then multiply by 18.2.
    //
    uint32_t clockCount = timeInMilliseconds * 18.2 / 1000;
    uint16_t lowValue = (clockCount & 0xFFFF);
    uint16_t highValue = ((clockCount >> 16) & 0xFFFF);
    gRandomGeneratorSeed = highValue ^ lowValue;
}

/// Generates a random number based on time?
uint16_t generateRandomNumber() // sub_4A1AE   proc near       ; CODE XREF: handleGameUserInput+66Cp
                   // ; updateScrollOffset+9p ...
{
    uint16_t someValue = gRandomGeneratorSeed;
    someValue *= 0x5E5; // 1509
    someValue += 0x31; // '1' or 49
    gRandomGeneratorSeed = someValue;
    return someValue / 2;
}

void updateUserInput() // sub_4A1BF   proc near       ; CODE XREF: handleGameUserInput+13p
                   // ; runMainMenu+BDp ...
{
    // 01ED:355C
    uint8_t directionKeyWasPressed = 0;

    int8_t gameControllerX = getGameControllerX();
    int8_t gameControllerY = getGameControllerY();

    if (gIsUpKeyPressed != 0
        || byte_519F9 != 0
        || gameControllerY < 0)
    {
//loc_4A1CF:              ; CODE XREF: updateUserInput+7j
        gCurrentUserInput = UserInputUp;
        directionKeyWasPressed = 1;
    }

//loc_4A1D6:              ; CODE XREF: updateUserInput+Ej
    if (gIsLeftKeyPressed != 0
        || byte_519F7 != 0
        || gameControllerX < 0)
    {
//loc_4A1E4:              ; CODE XREF: updateUserInput+1Cj
        gCurrentUserInput = UserInputLeft;
        directionKeyWasPressed = 1;
    }

//loc_4A1EB:              ; CODE XREF: updateUserInput+23j
    if (gIsDownKeyPressed != 0
        || byte_519F8 != 0
        || gameControllerY > 0)
    {
//loc_4A1F9:              ; CODE XREF: updateUserInput+31j
        gCurrentUserInput = UserInputDown;
        directionKeyWasPressed = 1;
    }

//loc_4A200:              ; CODE XREF: updateUserInput+38j
    if (gIsRightKeyPressed != 0
        || byte_519F6 != 0
        || gameControllerX > 0)
    {
//loc_4A20E:              ; CODE XREF: updateUserInput+46j
        gCurrentUserInput = UserInputRight;
        directionKeyWasPressed = 1;
    }

//loc_4A215:              ; CODE XREF: updateUserInput+4Dj
    if (gIsSpaceKeyPressed != 0
        || byte_519F5 != 0
        || byte_519F4 != 0
        || getGameControllerButton(SDL_CONTROLLER_BUTTON_X))
    {
//loc_4A22A:              ; CODE XREF: updateUserInput+5Bj
//                ; updateUserInput+62j
        if (directionKeyWasPressed == 1)
        {
            gCurrentUserInput += kUserInputSpaceAndDirectionOffset;
        }
        else
        {
//loc_4A236:              ; CODE XREF: updateUserInput+6Ej
            gCurrentUserInput = UserInputSpaceOnly;
        }
    }
}

void removeTiles(LevelTileType tileType) // sub_4A23C   proc near       ; CODE XREF: handleGameUserInput+111p
                   // ; handleGameUserInput+11Dp ...
{
    // TODO: around 01ED:35D9 (check with ORIGINAL.EXE)
    // Looks like this function goes through every tile and clears those that match the parameter
    for (uint16_t i = 0; i < kLevelSize; ++i)
    {
//loc_4A242:              ; CODE XREF: removeTiles+1Fj
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        if (tile->tile != tileType)
        {
            if (tileType != LevelTileTypeSnikSnak
                || tile->tile != 0xBB)
            {
                continue;
            }
        }

//loc_4A253:              ; CODE XREF: removeTiles+Cj
        tile->movingObject = 0;
        tile->tile = LevelTileTypeSpace;
    }
    sub_49D53();
    drawFixedLevel();
    sub_4A2E6();
    byte_510B3 = 0;
    byte_5A2F9 = 1;
}

void findMurphy() //   proc near       ; CODE XREF: start+344p sub_4A463+22p
{
    // 01ED:360E
    for (int i = 0; i < kLevelSize; ++i)
    {
        if (gCurrentLevel.tiles[i] == LevelTileTypeMurphy)
        {
            gMurphyLocation = i;
            break;
        }
    }

    sub_4A291();
}

void sub_4A291() //   proc near       ; CODE XREF: handleGameUserInput+686p
{
    // 01ED:362E
    // Parameters:
    // - si: murphy location * 2
    // - al: murphy location

    gMurphyTileX = gMurphyLocation % kLevelWidth; // stores X coord
    gMurphyTileY = gMurphyLocation / kLevelWidth; // stores Y coord

    gMurphyPositionX = gMurphyTileX * kTileSize;
    gMurphyPositionY = gMurphyTileY * kTileSize;
//    di = si[0x6155];
//    si = kMurphyStillSpriteCoordinates;
    drawMovingFrame(304, 132, gMurphyLocation);
    updateScrollOffset();
    word_59B92 = gScrollOffsetY;
    word_59B90 = gScrollOffsetX;
    al = gScrollOffsetX & 7;
    gNumberOfDotsToShiftDataLeft = al;

    videoloop();
}

void sub_4A2E6() //   proc near       ; CODE XREF: start+33Bp runLevel+ADp ...
{
    // 01ED:3683
    bx = 0;
    uint16_t numberOfInfotrons = 0;
    cx = 0x5A0; // 1440 = 120 * 12 -> width = 120, height = 12
//    mov si, offset leveldata
    uint16_t numberOfSomething = 0; // this is bx, just counts the number of tiles so technically is same as cx at this point… probably a return value but I don't see it used anywhere???

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4A2F0:              ; CODE XREF: sub_4A2E6+D1j
        MovingLevelTile *currentTile = &gCurrentLevelWord[i];
        numberOfSomething++;

        if (currentTile->tile == 0xF1)
        {
            currentTile->tile = LevelTileTypeExplosion;
            continue; // jmp short loc_4A3B0
        }

//loc_4A2FC:              ; CODE XREF: sub_4A2E6+Ej
        if (byte_5A33F != 1)
        {
            if (currentTile->tile == LevelTileTypeInfotron)
            {
//loc_4A33C:              ; CODE XREF: sub_4A2E6+20j
                numberOfInfotrons++;
                continue; // jmp short loc_4A3B0
            }
        }
        if (byte_5A33F == 1 || currentTile->movingObject != 0 || currentTile->tile != LevelTileTypeSnikSnak) //jz  short loc_4A34B
        {
            if (byte_5A33F == 1 || currentTile->movingObject != 0 || currentTile->tile != LevelTileTypeElectron) //jz  short loc_4A379
            {
//loc_4A312:              ; CODE XREF: sub_4A2E6+1Bj
                if ((currentTile->movingObject == 0 && currentTile->tile == LevelTileTypeHorizontalChipLeft)
                    || (currentTile->movingObject == 0 && currentTile->tile == LevelTileTypeHorizontalChipRight)
                    || (currentTile->movingObject == 0 && currentTile->tile == LevelTileTypeHorizontalChipTop)
                    || (currentTile->movingObject == 0 && currentTile->tile == LevelTileTypeHorizontalChipBottom))
                {
//loc_4A33F:              ; CODE XREF: sub_4A2E6+2Fj
//                ; sub_4A2E6+34j ...
                    currentTile->tile = LevelTileTypeChip; // mov word ptr [si], 5
                    currentTile->movingObject = 0;
                    continue; // jmp short loc_4A3B0
                }
                if (currentTile->movingObject == 0
                    && currentTile->tile >= LevelTileTypeHardware2
                    && currentTile->tile <= LevelTileTypeHardware11)
                {
//loc_4A345:              ; CODE XREF: sub_4A2E6+48j
                    currentTile->tile = LevelTileTypeHardware; // mov word ptr [si], 6
                    currentTile->movingObject = 0;
                    continue; // jmp short loc_4A3B0
                }

//loc_4A330:              ; CODE XREF: sub_4A2E6+43j
                if (currentTile->movingObject == 0
                    && currentTile->tile >= LevelTileTypeSportRight
                    && currentTile->tile <= LevelTileTypeSportUp)
                {
//loc_4A3A7:              ; CODE XREF: sub_4A2E6+52j
                    currentTile->tile -= 4; // Converts Sport[Direction] to Port[Direction]
                    currentTile->movingObject = 1;
//                    si[0] -= 4; // sub byte ptr [si], 4
//                    si[1] = 1; // mov byte ptr [si+1], 1
        //        jmp short $+2 // wtf this was right above loc_4A3B0
                    continue;
                }

//loc_4A33A:              ; CODE XREF: sub_4A2E6+4Dj
                continue; // jmp short loc_4A3B0
            }
        }

        MovingLevelTile *leftTile = &gCurrentLevelWord[i - 1];
        MovingLevelTile *aboveTile = &gCurrentLevelWord[i - kLevelWidth];
        MovingLevelTile *rightTile = &gCurrentLevelWord[i + 1];

        if (currentTile->movingObject != 0 || currentTile->tile != LevelTileTypeElectron) //jz  short loc_4A379
        {
//loc_4A34B:              ; CODE XREF: sub_4A2E6+25j
            if (leftTile->tile == LevelTileTypeSpace && leftTile->movingObject == 0) //cmp word ptr [si-2], 0
            {
                currentTile->movingObject = 1;
//                si[1] = 1; //mov byte ptr [si+1], 1
                continue; // jmp short loc_4A3B0
            }
//loc_4A357:              ; CODE XREF: sub_4A2E6+69j
        // 0x78 = 120
            if (aboveTile->tile == LevelTileTypeSpace && aboveTile->movingObject == 0) //cmp word ptr [si-78h], 0
            {
                // 01ED:36FA
                // mov word ptr [si-78h], 1011h
                aboveTile->movingObject = 0x10;
                aboveTile->tile = LevelTileTypeSnikSnak;
                // mov word ptr [si], 0FFFFh
                currentTile->movingObject = 0xFF;
                currentTile->tile = 0xFF;
                continue; // jmp short loc_4A3B0
            }
//loc_4A368:              ; CODE XREF: sub_4A2E6+75j
            if (rightTile->tile == LevelTileTypeSpace && rightTile->movingObject == 0) //cmp word ptr [si+2], 0
            {
                // 01ED:370B
                // mov word ptr [si+2], 2811h
                rightTile->movingObject = 0x28;
                rightTile->tile = LevelTileTypeSnikSnak;
                // mov word ptr [si], 0FFFFh
                currentTile->movingObject = 0xFF;
                currentTile->tile = 0xFF;
                continue; // jmp short loc_4A3B0
            }

            continue;
        }
//loc_4A379:              ; CODE XREF: sub_4A2E6+2Aj
        if (leftTile->tile == LevelTileTypeSpace && leftTile->movingObject == 0) //cmp word ptr [si-2], 0
        {
            currentTile->movingObject = 1; //mov byte ptr [si+1], 1
            continue; // jmp short loc_4A3B0
        }
//loc_4A385:              ; CODE XREF: sub_4A2E6+97j
        if (aboveTile->tile == LevelTileTypeSpace && aboveTile->movingObject == 0) //cmp word ptr [si-78h], 0
        {
            // mov word ptr [si-78h], 1018h
            aboveTile->movingObject = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            // mov word ptr [si], 0FFFFh
            currentTile->movingObject = 0xFF;
            currentTile->tile = 0xFF;
            continue; // jmp short loc_4A3B0
        }
//loc_4A396:              ; CODE XREF: sub_4A2E6+A3j
        if (rightTile->tile == LevelTileTypeSpace && rightTile->movingObject == 0) //cmp word ptr [si+2], 0
        {
            // mov word ptr [si+2], 2818h
            rightTile->movingObject = 0x28;
            rightTile->tile = LevelTileTypeElectron;
            // mov word ptr [si], 0FFFFh
            currentTile->movingObject = 0xFF;
            currentTile->tile = 0xFF;
            continue; // jmp short loc_4A3B0
        }

//loc_4A3B0:              ; CODE XREF: sub_4A2E6+13j
//                ; sub_4A2E6:loc_4A33Aj ...
        bx++;
    }
}

void resetNumberOfInfotrons() // sub_4A3BB   proc near       ; CODE XREF: start+33Ep sub_4A463+17p
{
    uint8_t numberOfInfotrons = 0;
    if (gCurrentLevel.numberOfInfotrons != 0)
    {
        numberOfInfotrons = gCurrentLevel.numberOfInfotrons;
    }

//loc_4A3C6:              ; CODE XREF: resetNumberOfInfotrons+5j
    gNumberOfRemainingInfotrons = numberOfInfotrons;
    gTotalNumberOfInfotrons = numberOfInfotrons;
    drawNumberOfRemainingInfotrons();
}

void sub_4A3D2() //   proc near       ; CODE XREF: handleGameUserInput+39Ep
                   // ; handleGameUserInput+3EBp
{
    byte_599D4 = 0;
    word_599D8 = 0;
    uint8_t wasNotZero = (byte_5A33E != 0);
    byte_5A33E = 0;
    if (wasNotZero)
    {
        // jnz short $+12
    }

    // continues with sub_4A3E9 ? or where does that jump lead? check with debugger
//sub_4A3D2   endp ; sp-analysis failed
}

void sub_4A3E9() //   proc near       ; CODE XREF: handleGameUserInput+14Ep
{
    if (byte_5A33E == 0)
    {
        sub_4A95F();
    }

//loc_4A3F3:              ; CODE XREF: sub_4A3D2+15j
//                ; sub_4A3E9+5j
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (SDL_Color) { 0, 0, 0 });

    if (byte_5A33E != 0)
    {
        gIsPlayingDemo = 1;
    }

//loc_4A427:              ; CODE XREF: sub_4A3E9+37j
    byte_5A33F = 0;
    sub_4A463();
    byte_5A33F = 1;
    if (byte_5A33E >= 1)
    {
        gIsPlayingDemo = 0;
        if (byte_5A33E == 0) // WTF? this makes no sense...
        {
            byte_5A33E++;
        }
    }

//loc_4A446:              ; CODE XREF: sub_4A3E9+50j
//                ; sub_4A3E9+57j
    gCurrentUserInput = UserInputNone;
    if (gIsPlayingDemo == 0)
    {
        return;
    }

    word_510DF = word_5A33C;
    byte_510E2 = 1;
    simulateDemoInput();
}

void sub_4A463() //   proc near       ; CODE XREF: sub_4945D:loc_4953Bp
                   // ; sub_4A3E9+43p
{
    readLevels();
    drawFixedLevel();
    drawGamePanel();
    byte_5A33F = -byte_5A33F;
    sub_4A2E6();
    byte_5A33F = -byte_5A33F;
    resetNumberOfInfotrons();
    byte_59B7B = 1;
    initializeGameInfo();
    findMurphy();
}

void updateOrangeDiskTiles(uint16_t position) // movefun3  proc near       ; DATA XREF: data:161Ao
{
    // 01ED:3826
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];

    if (currentTile->tile != LevelTileTypeOrangeDisk)
    {
        return;
    }

//loc_4A491:              ; CODE XREF: movefun3+5j
    uint16_t tileValue = ((currentTile->movingObject << 8)
                         | currentTile->tile);

    if (tileValue >= 0x3008)
    {
//loc_4A4D4:              ; CODE XREF: movefun3+Fj
    //    push    si
        uint8_t movingObjectFrame = currentTile->movingObject;
//        bh = 0;
//        al = movingObject;
    //    bx *= 2;
    //    ;and bx, byte ptr 0Fh
    //    db 83h, 0E3h, 0Fh
        movingObjectFrame *= 2;
        movingObjectFrame &= 0xF; // 16 frames?

    //    mov di, [si+6155h]
    //    shl bx, 1
    //    add di, [bx+6C95h]
    //    mov si, 12F6h
    //    mov si, [si]
        uint16_t offset = kFallAnimationGravityOffsets[movingObjectFrame];

        uint8_t tileX = (position % kLevelWidth);
        uint8_t tileY = (position / kLevelWidth);

        uint16_t dstX = tileX * kTileSize + (offset % 122);
        uint16_t dstY = tileY * kTileSize + (offset / 122);

        drawMovingSpriteFrameInLevel(128, 64,
                                     kTileSize,
                                     kTileSize + 2,
                                     dstX, dstY);

        uint8_t movingObject = currentTile->movingObject;
        movingObject++;
        uint8_t otherMovingObject = movingObject;
        otherMovingObject &= 7;
        if (otherMovingObject != 0)
        {
            currentTile->movingObject = movingObject;
            return;
        }

//loc_4A516:              ; CODE XREF: movefun3+86j
        currentTile->movingObject = 0;
        currentTile->tile = LevelTileTypeSpace;
        belowTile->movingObject = 0;
        belowTile->tile = LevelTileTypeOrangeDisk;

        position += kLevelWidth;

        // Update tiles
        currentTile = &gCurrentLevelWord[position];
        belowTile = &gCurrentLevelWord[position + kLevelWidth];

        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
            currentTile->movingObject = 0x30;
            belowTile->movingObject = 8;
        }
//loc_4A537:              ; CODE XREF: movefun3+A1j
        else if (belowTile->tile != LevelTileTypeExplosion)
        {
//loc_4A53F:              ; CODE XREF: movefun3+B3j
            // 01ED:38DC
            detonateBigExplosion(position);
        }

        return;
    }
    else if (tileValue >= 0x2008)
    {
//loc_4A4B4:              ; CODE XREF: movefun3+14j
        if (belowTile->movingObject != 0 || belowTile->tile != LevelTileTypeSpace)
        {
//loc_4A4C2:              ; CODE XREF: movefun3+30j
            uint8_t movingObject = currentTile->movingObject;
            movingObject++;
            if (movingObject == 0x22)
            {
                movingObject = 0x30;
            }

//loc_4A4CF:              ; CODE XREF: movefun3+42j
            currentTile->movingObject = movingObject;
            return;
        }
        currentTile->movingObject = 0;
        currentTile->tile = LevelTileTypeOrangeDisk;
        return;
    }

    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
//loc_4A4A9:              ; CODE XREF: movefun3+1Dj
        currentTile->movingObject = 0x20;
        belowTile->movingObject = 8;
    }
}

void updateExplosionTiles(uint16_t position) //loc_4A543:              ; DATA XREF: data:1648o
{
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        return;
    }

//loc_4A54B:              ; CODE XREF: code:3928j
    if ((word_5195D & 3) != 0)
    {
        return;
    }

//loc_4A557:              ; CODE XREF: code:3934j
    uint8_t movingObject = currentTile->movingObject;
    if ((movingObject & 0x80) != 0)
    {
//loc_4A5A0:              ; CODE XREF: code:393Ej
        movingObject++;
        if (movingObject != 0x89)
        {
//loc_4A5B3:              ; CODE XREF: code:3985j
            currentTile->movingObject = movingObject;
            movingObject--;
            movingObject &= 0xF;
            // 12e6
            Point frameCoordinates = kInfotronExplosionAnimationFrameCoordinates[movingObject];

            drawMovingFrame(frameCoordinates.x,
                            frameCoordinates.y,
                            position);
        }
        else
        {
            currentTile->movingObject = 0;
            currentTile->tile = LevelTileTypeInfotron;
            gIsExplosionStarted = 0;
        }
    }
    else
    {
        movingObject++;
        currentTile->movingObject = movingObject;
        movingObject--;

        // 12d6
        Point frameCoordinates = kRegularExplosionAnimationFrameCoordinates[movingObject];

        drawMovingFrame(frameCoordinates.x,
                        frameCoordinates.y,
                        position);

//loc_4A582:              ; CODE XREF: code:396Aj
        if (currentTile->movingObject == 8)
        {
            currentTile->movingObject = 0;
            currentTile->tile = LevelTileTypeSpace;
            gIsExplosionStarted = 0;
        }
    }
}

void sub_4A5E0() //   proc near       ; CODE XREF: runLevel+106p
{
    // 01ED:397D
    // TODO: revisit to understand... this does something related to electrons?
    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4A5E9:              ; CODE XREF: sub_4A5E0+25j
        int8_t afterWordTile = gCurrentLevelAfterWord.tiles[i];

        if (afterWordTile == 0)
        {
            continue;
        }

        if (afterWordTile < 0)
        {
//loc_4A608:              ; CODE XREF: sub_4A5E0+10j
            gCurrentLevelAfterWord.tiles[i] = afterWordTile + 1;

            if (gCurrentLevelAfterWord.tiles[i] == 0)
            {
                MovingLevelTile *tile = &gCurrentLevelWord[i];
                tile->movingObject = 0xFF;
                tile->tile = LevelTileTypeElectron;
                detonateBigExplosion(i);
            }
        }
        else
        {
            gCurrentLevelAfterWord.tiles[i] = afterWordTile - 1;

            if (gCurrentLevelAfterWord.tiles[i] == 0)
            {
                detonateBigExplosion(i);
            }
        }
    }
}

// Creates an explossion of 3x3 tiles around a position
void detonateBigExplosion(uint16_t position) // sub_4A61F   proc near       ; CODE XREF: movefun+271p
                   // ; movefun2+20Fp ...
{
    // 01ED:39BC
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *aboveLeftTile = &gCurrentLevelWord[position - kLevelWidth - 1];
    MovingLevelTile *aboveRightTile = &gCurrentLevelWord[position - kLevelWidth + 1];
    MovingLevelTile *belowLeftTile = &gCurrentLevelWord[position + kLevelWidth - 1];
    MovingLevelTile *belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];

    // These indicate the kind of the explosion created by this tile.
    // Tiles around may create a different explosion if needed (like Electrons create Infotrons).
    //
    uint8_t mainMovingObject = 0;
    uint8_t mainTile = 0;
    uint8_t mainAfterWordTile = 0;

    if (currentTile->movingObject == 0 && currentTile->tile == LevelTileTypeHardware)
    {
        return;
    }

//loc_4A627:              ; CODE XREF: detonateBigExplosion+5j
    gIsExplosionStarted = 1;
    if (currentTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;
    }

//loc_4A639:              ; CODE XREF: detonateBigExplosion+12j
    if (currentTile->tile == LevelTileTypeElectron)
    {
        mainMovingObject = 0x80;
        mainTile = LevelTileTypeExplosion;
        mainAfterWordTile = 0xF3; // 243
    }
    else
    {
//loc_4A647:              ; CODE XREF: detonateBigExplosion+1Fj
        // cx = 0x1F; // 31
        mainMovingObject = 0;
        mainTile = LevelTileTypeExplosion;
        mainAfterWordTile = LevelTileTypeSportRight; // 13
    }

    // These have the tile-specific explosion info, which might differ (or not) from the main type
    uint8_t movingObject = mainMovingObject;
    uint8_t tile = mainTile;
    uint8_t afterWordTile = mainAfterWordTile;

//loc_4A64C:              ; CODE XREF: detonateBigExplosion+26j
    uint8_t skipHardwareCheck7 = 0;

    if (aboveLeftTile->tile == LevelTileTypeOrangeDisk
        || aboveLeftTile->tile == LevelTileTypeYellowDisk
        || aboveLeftTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (aboveLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth - 1] = afterWordTile; // mov [bx+23F7h], dh
        }
    }
    else if (aboveLeftTile->tile == LevelTileTypeZonk)
    {
//loc_4A69C:              ; CODE XREF: detonateBigExplosion+46j
        // 01ED:3A39
        detonateZonk(position - kLevelWidth - 1, movingObject, tile);
        skipHardwareCheck7 = 1; // to emulate jmp loc_4A6A6
    }
    else if (aboveLeftTile->tile == LevelTileTypeInfotron)
    {
//loc_4A692:              ; CODE XREF: detonateBigExplosion+4Aj
        sub_4AA34(position - kLevelWidth - 1, movingObject, tile);
        skipHardwareCheck7 = 1; // to emulate jmp loc_4A6A6
    }
    else if (aboveLeftTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (aboveLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth - 1] = afterWordTile; // mov [bx+23F7h], dh
        }
    }
//loc_4A676:              ; CODE XREF: detonateBigExplosion+4Ej
    else if (aboveLeftTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (aboveLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth - 1] = afterWordTile; // mov [bx+23F7h], dh
        }
    }

    if (skipHardwareCheck7 == 0)
    {
//loc_4A688:              ; CODE XREF: detonateBigExplosion+59j
//                ; detonateBigExplosion+63j
        if (aboveLeftTile->tile != LevelTileTypeHardware)
        {
            // mov [si+17BAh], cx
            aboveLeftTile->movingObject = movingObject;
            aboveLeftTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A6A6:              ; CODE XREF: detonateBigExplosion:loc_4A690j
//                ; detonateBigExplosion+7Bj ...
    uint8_t skipHardwareCheck6 = 0;

    if (aboveTile->tile == LevelTileTypeOrangeDisk
        || aboveTile->tile == LevelTileTypeYellowDisk
        || aboveTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A6D7:              ; CODE XREF: detonateBigExplosion+91j
//                ; detonateBigExplosion+95j ...
        if (aboveTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth] = afterWordTile; // mov [bx+23F8h], dh
        }
    }
    else if (aboveTile->tile == LevelTileTypeZonk)
    {
//loc_4A6F3:              ; CODE XREF: detonateBigExplosion+9Dj
        detonateZonk(position - kLevelWidth, movingObject, tile);
        skipHardwareCheck6 = 1; // to emulate jmp loc_4A6FD
    }
    else if (aboveTile->tile == LevelTileTypeInfotron)
    {
//loc_4A6E9:              ; CODE XREF: detonateBigExplosion+A1j
        sub_4AA34(position - kLevelWidth, movingObject, tile);
        skipHardwareCheck6 = 1; // to emulate jmp loc_4A6FD
    }
    else if (aboveTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A6D7:              ; CODE XREF: detonateBigExplosion+91j
//                ; detonateBigExplosion+95j ...
        if (aboveTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth] = afterWordTile; // mov [bx+23F8h], dh
        }
    }
//loc_4A6CD:              ; CODE XREF: detonateBigExplosion+A5j
    else if (aboveTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A6D7:              ; CODE XREF: detonateBigExplosion+91j
//                ; detonateBigExplosion+95j ...
        if (aboveTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth] = afterWordTile; // mov [bx+23F8h], dh
        }
    }

    if (skipHardwareCheck6 == 0)
    {
//loc_4A6DF:              ; CODE XREF: detonateBigExplosion+B0j
//                ; detonateBigExplosion+BAj
        if (aboveTile->tile != LevelTileTypeHardware)
        {
            // mov [si+17BCh], cx
            aboveTile->movingObject = movingObject;
            aboveTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A6FD:              ; CODE XREF: detonateBigExplosion:loc_4A6E7j
//                ; detonateBigExplosion+D2j ...
    uint8_t skipHardwareCheck5 = 0;

    if (aboveRightTile->tile == LevelTileTypeOrangeDisk
        || aboveRightTile->tile == LevelTileTypeYellowDisk
        || aboveRightTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A72E:              ; CODE XREF: detonateBigExplosion+E8j
//                ; detonateBigExplosion+ECj ...
        if (aboveRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth + 1] = afterWordTile; // mov [bx+23F9h], dh
        }
    }
    else if (aboveRightTile->tile == LevelTileTypeZonk)
    {
//loc_4A74A:              ; CODE XREF: detonateBigExplosion+F4j
        detonateZonk(position - kLevelWidth + 1, movingObject, tile);
        skipHardwareCheck5 = 1; // to emulate jmp loc_4A754
    }
    else if (aboveRightTile->tile == LevelTileTypeInfotron)
    {
//loc_4A740:              ; CODE XREF: detonateBigExplosion+F8j
        sub_4AA34(position - kLevelWidth + 1, movingObject, tile);
        skipHardwareCheck5 = 1; // to emulate jmp loc_4A754
    }
    else if (aboveRightTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A72E:              ; CODE XREF: detonateBigExplosion+E8j
//                ; detonateBigExplosion+ECj ...
        if (aboveRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth + 1] = afterWordTile; // mov [bx+23F9h], dh
        }
    }
//loc_4A724:              ; CODE XREF: detonateBigExplosion+FCj
    else if (aboveRightTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A72E:              ; CODE XREF: detonateBigExplosion+E8j
//                ; detonateBigExplosion+ECj ...
        if (aboveRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - kLevelWidth + 1] = afterWordTile; // mov [bx+23F9h], dh
        }
    }

    if (skipHardwareCheck5 == 0)
    {
//loc_4A736:              ; CODE XREF: detonateBigExplosion+107j
//                ; detonateBigExplosion+111j
        if (aboveRightTile->tile != LevelTileTypeHardware)
        {
            // mov [si+17BEh], cx
            aboveRightTile->movingObject = movingObject;
            aboveRightTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A754:              ; CODE XREF: detonateBigExplosion:loc_4A73Ej
//                ; detonateBigExplosion+129j ...
    uint8_t skipHardwareCheck4 = 0;

    if (leftTile->tile == LevelTileTypeOrangeDisk
        || leftTile->tile == LevelTileTypeYellowDisk
        || leftTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A785:              ; CODE XREF: detonateBigExplosion+13Fj
//                ; detonateBigExplosion+143j ...
        if (leftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - 1] = afterWordTile; // mov [bx+2433h], dh
        }
    }
    else if (leftTile->tile == LevelTileTypeZonk)
    {
//loc_4A7A1:              ; CODE XREF: detonateBigExplosion+14Bj
        detonateZonk(position - 1, movingObject, tile);
        skipHardwareCheck4 = 1; // to emulate jmp loc_4A7AB
    }
    else if (leftTile->tile == LevelTileTypeInfotron)
    {
//loc_4A797:              ; CODE XREF: detonateBigExplosion+14Fj
        sub_4AA34(position - 1, movingObject, tile);
        skipHardwareCheck4 = 1; // to emulate jmp loc_4A7AB
    }
    else if (leftTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A785:              ; CODE XREF: detonateBigExplosion+13Fj
//                ; detonateBigExplosion+143j ...
        if (leftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - 1] = afterWordTile; // mov [bx+2433h], dh
        }
    }
//loc_4A77B:              ; CODE XREF: detonateBigExplosion+153j
    else if (leftTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A785:              ; CODE XREF: detonateBigExplosion+13Fj
//                ; detonateBigExplosion+143j ...
        if (leftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position - 1] = afterWordTile; // mov [bx+2433h], dh
        }
    }

    if (skipHardwareCheck4 == 0)
    {
//loc_4A78D:              ; CODE XREF: detonateBigExplosion+15Ej
//                ; detonateBigExplosion+168j
        if (leftTile->tile != LevelTileTypeHardware)
        {
            // mov [si+1832h], cx
            leftTile->movingObject = movingObject;
            leftTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A7AB:              ; CODE XREF: detonateBigExplosion:loc_4A795j
//                ; detonateBigExplosion+180j ...
    // mov [si+1834h], cx
    currentTile->movingObject = movingObject;
    currentTile->tile = tile;

    uint8_t skipHardwareCheck3 = 0;

    if (rightTile->tile == LevelTileTypeOrangeDisk
        || rightTile->tile == LevelTileTypeYellowDisk
        || rightTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A7E0:              ; CODE XREF: detonateBigExplosion+19Aj
//                ; detonateBigExplosion+19Ej ...
        if (rightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + 1] = afterWordTile; // mov [bx+2435h], dh
        }
    }
    else if (rightTile->tile == LevelTileTypeZonk)
    {
//loc_4A7FC:              ; CODE XREF: detonateBigExplosion+1A6j
        detonateZonk(position + 1, movingObject, tile);
        skipHardwareCheck3 = 1; // to emulate jmp loc_4A806
    }
    else if (rightTile->tile == LevelTileTypeInfotron)
    {
//loc_4A7F2:              ; CODE XREF: detonateBigExplosion+1AAj
        sub_4AA34(position + 1, movingObject, tile);
        skipHardwareCheck3 = 1; // to emulate jmp loc_4A806
    }
    else if (rightTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A7E0:              ; CODE XREF: detonateBigExplosion+19Aj
//                ; detonateBigExplosion+19Ej ...
        if (rightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + 1] = afterWordTile; // mov [bx+2435h], dh
        }
    }
//loc_4A7D6:              ; CODE XREF: detonateBigExplosion+1AEj
    else if (rightTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A7E0:              ; CODE XREF: detonateBigExplosion+19Aj
//                ; detonateBigExplosion+19Ej ...
        if (rightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + 1] = afterWordTile; // mov [bx+2435h], dh
        }
    }

    if (skipHardwareCheck3 == 0)
    {
//loc_4A7E8:              ; CODE XREF: detonateBigExplosion+1B9j
//                ; detonateBigExplosion+1C3j
        if (rightTile->tile != LevelTileTypeHardware)
        {
            // mov [si+1836h], cx
            rightTile->movingObject = movingObject;
            rightTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A806:              ; CODE XREF: detonateBigExplosion:loc_4A7F0j
//                ; detonateBigExplosion+1DBj ...
    // 01ED:3BA7
    uint8_t skipHardwareCheck2 = 0;

    if (belowLeftTile->tile == LevelTileTypeOrangeDisk
        || belowLeftTile->tile == LevelTileTypeYellowDisk
        || belowLeftTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A837:              ; CODE XREF: detonateBigExplosion+1F1j
//                ; detonateBigExplosion+1F5j ...
        if (belowLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth - 1] = afterWordTile; // mov [bx+246Fh], dh
        }
    }
    else if (belowLeftTile->tile == LevelTileTypeZonk)
    {
//loc_4A853:              ; CODE XREF: detonateBigExplosion+1FDj
        detonateZonk(position + kLevelWidth - 1, movingObject, tile);
        skipHardwareCheck2 = 1; // to emulate jmp loc_4A85D
    }
    else if (belowLeftTile->tile == LevelTileTypeInfotron)
    {
//loc_4A849:              ; CODE XREF: detonateBigExplosion+201j
        sub_4AA34(position + kLevelWidth - 1, movingObject, tile);
        skipHardwareCheck2 = 1; // to emulate jmp loc_4A85D
    }
    else if (belowLeftTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A837:              ; CODE XREF: detonateBigExplosion+1F1j
//                ; detonateBigExplosion+1F5j ...
        if (belowLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth - 1] = afterWordTile; // mov [bx+246Fh], dh
        }
    }
//loc_4A82D:              ; CODE XREF: detonateBigExplosion+205j
    else if (belowLeftTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;
//loc_4A837:              ; CODE XREF: detonateBigExplosion+1F1j
//                ; detonateBigExplosion+1F5j ...
        if (belowLeftTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth - 1] = afterWordTile; // mov [bx+246Fh], dh
        }
    }

    if (skipHardwareCheck2 == 0)
    {
//loc_4A83F:              ; CODE XREF: detonateBigExplosion+210j
//                ; detonateBigExplosion+21Aj
        if (belowLeftTile->tile != LevelTileTypeHardware)
        {
            // mov [si+18AAh], cx
            belowLeftTile->movingObject = movingObject;
            belowLeftTile->tile = tile;
        }
    }

    // Restore to the main explosion type before evaluating the new tile
    movingObject = mainMovingObject;
    tile = mainTile;
    afterWordTile = mainAfterWordTile;

//loc_4A85D:              ; CODE XREF: detonateBigExplosion:loc_4A847j
//                ; detonateBigExplosion+232j ...
    // 01ED:3BE0
    uint8_t skipHardwareCheck1 = 0;

    if (belowTile->tile == LevelTileTypeOrangeDisk
        || belowTile->tile == LevelTileTypeYellowDisk
        || belowTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A88E:              ; CODE XREF: detonateBigExplosion+248j
//                ; detonateBigExplosion+24Cj ...
        if (belowTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth] = afterWordTile; // mov [bx+2470h], dh
        }
    }
    else if (belowTile->tile == LevelTileTypeZonk)
    {
//loc_4A8AA:              ; CODE XREF: detonateBigExplosion+254j
        detonateZonk(position + kLevelWidth, movingObject, tile);
        skipHardwareCheck1 = 1; // to emulate jmp loc_4A8B4
    }
    else if (belowTile->tile == LevelTileTypeInfotron)
    {
//loc_4A8A0:              ; CODE XREF: detonateBigExplosion+258j
        sub_4AA34(position + kLevelWidth, movingObject, tile);
        skipHardwareCheck1 = 1; // to emulate jmp loc_4A8B4
    }
    else if (belowTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A88E:              ; CODE XREF: detonateBigExplosion+248j
//                ; detonateBigExplosion+24Cj ...
        if (belowTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth] = afterWordTile; // mov [bx+2470h], dh
        }
    }
//loc_4A884:              ; CODE XREF: detonateBigExplosion+25Cj
    else if (belowTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A88E:              ; CODE XREF: detonateBigExplosion+248j
//                ; detonateBigExplosion+24Cj ...
        if (belowTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth] = afterWordTile; // mov [bx+2470h], dh
        }
    }

    if (skipHardwareCheck1 == 0) // to emulate jmp loc_4A8B4
    {
//loc_4A896:              ; CODE XREF: detonateBigExplosion+267j
//                ; detonateBigExplosion+271j
        if (belowTile->tile != LevelTileTypeHardware)
        {
            // mov [si+18ACh], cx
            belowTile->movingObject = movingObject;
            belowTile->tile = tile;
        }
    }

//loc_4A8B4:              ; CODE XREF: detonateBigExplosion:loc_4A89Ej
//                ; detonateBigExplosion+289j ...
    // 01ED:3BFE
    if (belowRightTile->tile == LevelTileTypeOrangeDisk
        || belowRightTile->tile == LevelTileTypeYellowDisk
        || belowRightTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A8E5:              ; CODE XREF: detonateBigExplosion+29Fj
//                ; detonateBigExplosion+2A3j ...
        if (belowRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth + 1] = afterWordTile; // mov [bx+2471h], dh
        }
    }
    else if (belowRightTile->tile == LevelTileTypeZonk)
    {
//loc_4A901:              ; CODE XREF: detonateBigExplosion+2ABj
        detonateZonk(position + kLevelWidth + 1, movingObject, tile);
//loc_4A90B:              ; CODE XREF: detonateBigExplosion:loc_4A8F5j
//                ; detonateBigExplosion+2E0j ...
        sound4();
        return;
    }
    else if (belowRightTile->tile == LevelTileTypeInfotron)
    {
//loc_4A8F7:              ; CODE XREF: detonateBigExplosion+2AFj
        sub_4AA34(position + kLevelWidth + 1, movingObject, tile);
//loc_4A90B:              ; CODE XREF: detonateBigExplosion:loc_4A8F5j
//                ; detonateBigExplosion+2E0j ...
        sound4();
        return;
    }
    else if (belowRightTile->tile == LevelTileTypeElectron)
    {
        afterWordTile = 256 - afterWordTile; // dh = -dh;
        movingObject = 0x80;
        tile = LevelTileTypeExplosion;
//loc_4A8E5:              ; CODE XREF: detonateBigExplosion+29Fj
//                ; detonateBigExplosion+2A3j ...
        if (belowRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth + 1] = afterWordTile; // mov [bx+2471h], dh
        }
    }
//loc_4A8DB:              ; CODE XREF: detonateBigExplosion+2B3j
    else if (belowRightTile->tile == LevelTileTypeMurphy)
    {
        word_510D1 = 1;

//loc_4A8E5:              ; CODE XREF: detonateBigExplosion+29Fj
//                ; detonateBigExplosion+2A3j ...
        if (belowRightTile->tile != LevelTileTypeHardware)
        {
            gCurrentLevelAfterWord.tiles[position + kLevelWidth + 1] = afterWordTile; // mov [bx+2471h], dh
        }
    }

//loc_4A8ED:              ; CODE XREF: detonateBigExplosion+2BEj
//                ; detonateBigExplosion+2C8j
    if (belowRightTile->tile != LevelTileTypeHardware)
    {
        // mov [si+18AEh], cx
        belowRightTile->movingObject = movingObject;
        belowRightTile->tile = tile;
    }

//loc_4A90B:              ; CODE XREF: detonateBigExplosion:loc_4A8F5j
//                ; detonateBigExplosion+2E0j ...
    sound4();
    // 01ED:3CAC
}

void updatePlantedRedDisk() // sub_4A910   proc near       ; CODE XREF: runLevel:noFlashing3p
{
    // 01ED:3CAD
    if (gPlantedRedDiskCountdown <= 1)
    {
        return;
    }

    MovingLevelTile *tile = &gCurrentLevelWord[gPlantedRedDiskPosition];

    if (tile->movingObject == 0 && tile->tile == LevelTileTypeSpace)
    {
        tile->movingObject = 0;
        tile->tile = LevelTileTypeRedDisk;
    }

//loc_4A932:              ; CODE XREF: updatePlantedRedDisk+1Aj
    // si = word_5177E;
    drawMovingFrame(256, 164, gPlantedRedDiskPosition);
    gPlantedRedDiskCountdown++;
    if (gPlantedRedDiskCountdown >= 0x28)
    {
        detonateBigExplosion(gPlantedRedDiskPosition);
        gPlantedRedDiskCountdown = 0;
    }
}

void sub_4A95F() //   proc near       ; CODE XREF: runLevel+372p
                   // ; sub_4A3E9+7p ...
{
    /*
    al = gGameSeconds
    mov byte_510B4, al
    al = gGameMinutes
    mov byte_510B5, al
    al = gGameHours
    mov byte_510B6, al
    cmp gIsPlayingDemo, 0
    jnz short locret_4A97F
    cmp byte_599D4, 0
    jz  short loc_4A980

locret_4A97F:               ; CODE XREF: sub_4A95F+17j
    return;

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

loc_4A9B3:              ; CODE XREF: sub_4A95F+4Bj
    mov [si+0Ah], al
    al = byte_510B6
    add al, [si+9]
    jnb short loc_4A9C0
    al = 0FFh

loc_4A9C0:              ; CODE XREF: sub_4A95F+5Dj
    mov [si+9], al
    return;
     */
}

void detonateZonk(uint16_t position, uint8_t movingObject, uint8_t tile) // sub_4A9C4   proc near       ; CODE XREF: detonateBigExplosion+81p
                  //  ; detonateBigExplosion+D8p ...
{
    // 01ED:3D61
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];

    currentTile->movingObject = movingObject;
    currentTile->tile = tile;

    uint8_t movingObjectType = currentTile->movingObject & 0xF0;

    if (movingObjectType == 0x10
        || movingObjectType == 0x70)
    {
//loc_4A9EF:              ; CODE XREF: detonateZonk+Aj detonateZonk+Fj
        sub_4AAB4(position - kLevelWidth);
        if (belowTile->movingObject == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (movingObjectType == 0x20)
    {
//loc_4AA05:              ; CODE XREF: detonateZonk+14j
        sub_4AAB4(position + 1);
        sub_4AAB4(position + kLevelWidth);
    }
    else if (movingObjectType == 0x30)
    {
//loc_4AA12:              ; CODE XREF: detonateZonk+19j
        sub_4AAB4(position - 1);
        sub_4AAB4(position + kLevelWidth);
    }
    else if (movingObjectType == 0x50)
    {
//loc_4AA1F:              ; CODE XREF: detonateZonk+1Ej
        sub_4AAB4(position - 1);
    }
    else if (movingObjectType == 0x60)
    {
//loc_4AA26:              ; CODE XREF: detonateZonk+23j
        sub_4AAB4(position + 1);
    }
    else if (movingObjectType == 0x70)
    {
//loc_4AA2D:              ; CODE XREF: detonateZonk+28j
        sub_4AAB4(position + kLevelWidth);
    }
}

void sub_4AA34(uint16_t position, uint8_t movingObject, uint8_t tile) //   proc near       ; CODE XREF: detonateBigExplosion+77p
                   // ; detonateBigExplosion+CEp ...
{
    // Parameters:
    // - si: position
    // - cx: movingObject (ch) and tile (cl)

    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];

    currentTile->movingObject = movingObject;
    currentTile->tile = tile;

    ah = currentTile->movingObject & 0xF0;

    if (ah == 0x10
        || ah == 0x70)
    {
//loc_4AA5F:              ; CODE XREF: sub_4AA34+Aj sub_4AA34+Fj
        sub_4AAB4(position - kLevelWidth);
        if (belowTile->movingObject == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (ah == 0x20)
    {
//loc_4AA75:              ; CODE XREF: sub_4AA34+14j
        sub_4AAB4(position + 1);
        if (belowTile->movingObject == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (ah == 0x30)
    {
//loc_4AA8A:              ; CODE XREF: sub_4AA34+19j
        sub_4AAB4(position - 1);
        if (belowTile->movingObject == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (ah == 0x50)
    {
//loc_4AA9F:              ; CODE XREF: sub_4AA34+1Ej
        sub_4AAB4(position - 1);
    }
    else if (ah == 0x60)
    {
//loc_4AAA6:              ; CODE XREF: sub_4AA34+23j
        sub_4AAB4(position + 1);
    }
    else if (ah == 0x70)
    {
//loc_4AAAD:              ; CODE XREF: sub_4AA34+28j
        sub_4AAB4(position + kLevelWidth);
    }
}

void sub_4AAB4(uint16_t position) //   proc near       ; CODE XREF: detonateZonk+2Ep
                   // ; detonateZonk+3Dp ...
{
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];

    if (currentTile->tile == LevelTileTypeExplosion)
    {
        return;
    }

//loc_4AABC:              ; CODE XREF: sub_4AAB4+5j
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeSpace;

//    push    si
//    push    ds
//    mov di, [si+6155h]
    si = word_51580; // probably point to the coordinates of the frame in MOVING.DAT
    drawMovingSpriteFrameInLevel(0, 0, 0, 0, 0, 0);
    /*
    mov ax, es
    mov ds, ax
    mov cx, 10h

//loc_4AAD3:              ; CODE XREF: sub_4AAB4+27j
    movsb
    movsb
    add si, 78h ; 'x'
    add di, 78h ; 'x'
    loop    loc_4AAD3
    pop ds
    pop si
     */
    return;
}

void readMenuDat() // proc near       ; CODE XREF: readEverything+9p
{
    FILE *file = openReadonlyFile("MENU.DAT", "r");
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

    char newPlayerName[kPlayerNameLength + 1] = "        ";
    gNewPlayerNameLength = 0;
    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    restoreLastMouseAreaBitmap();

    if (isRealKeyboardSupported())
    {
        // mov di, 89F7h
        drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, "YOUR NAME:             ");

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

            int9handler(0);
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
    }
    else if (isVirtualKeyboardSupported())
    {
        char inputBuffer[kPlayerNameLength + 1] = "";
        uint8_t result = inputVirtualKeyboardText("Enter your name", kPlayerNameLength, inputBuffer);

        if (result == 0)
        {
            drawTextWithChars6FontWithOpaqueBackground(168, 127, 8, "                       ");
            saveLastMouseAreaBitmap();
            drawMouseCursor();
            return;
        }

        const char *allowedCharacters = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM -";

        // Fill name with spaces, convert it to uppercase, and replace invalid characters with '-'
        size_t numberOfSpaces = kPlayerNameLength - strlen(inputBuffer);

        for (size_t idx = 0; idx < kPlayerNameLength; ++idx)
        {
            char inputCharacter = ' ';

            if (idx >= numberOfSpaces)
            {
                inputCharacter = toupper(inputBuffer[idx - numberOfSpaces]);

                if (strchr(allowedCharacters, inputCharacter) == NULL)
                {
                    inputCharacter = '-';
                }
            }

            newPlayerName[idx] = inputCharacter;
        }
    }
    else
    {
        do
        {
//loc_4AB7F:              ; CODE XREF: handleNewPlayerOptionClick+6Aj
            getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        }
        while (mouseButtonStatus != 0);

        snprintf(newPlayerName, kPlayerNameLength, "PLAYER%2d", gNewPlayerEntryIndex + 1);
        gNewPlayerNameLength = strlen(newPlayerName);
    }

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
    gShouldAutoselectNextLevelToPlay = 1;
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
    gShouldAutoselectNextLevelToPlay = 1;
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
    if (gIsDebugModeEnabled == 0)
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
        gShouldAutoselectNextLevelToPlay = 0;
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

void handleDemoOptionClick() // sub_4B159   proc near       ; CODE XREF: runMainMenu+6Fp
{
    // 01ED:44F6
    if (readDemoFiles() == 0)
    {
        return;
    }

//loc_4B163:              ; CODE XREF: handleDemoOptionClick+5j
    word_5196C = 1;
    gIsPlayingDemo = 1;

    uint8_t numberOfDemos = 0;

    uint8_t idx = 0;
    do
    {
//loc_4B17A:              ; CODE XREF: handleDemoOptionClick+2Dj
        if (gDemos.demoFirstIndices[idx] == 0xFFFF)
        {
            break;
        }
        idx++;
        numberOfDemos++;
    }
    while (1);
    // 01ED:4525
    uint8_t *demosAsByteArray = (uint8_t *)&gDemos;

//loc_4B188:              ; CODE XREF: handleDemoOptionClick+2Aj
    // This picks a random demo
    generateRandomSeedFromClock();
    uint16_t demoIndex = generateRandomNumber() % numberOfDemos;
    uint16_t demoFirstIndex = gDemos.demoFirstIndices[demoIndex];

    // This only happens if there are no demos...
    if (demoFirstIndex == 0xFFFF)
    {
        word_5196C = 0;
        gIsPlayingDemo = 0;
    }

//loc_4B1AE:              ; CODE XREF: handleDemoOptionClick+48j
    uint8_t demoLevelNumber = demosAsByteArray[demoFirstIndex];
    uint8_t finalLevelNumber = demoIndex;

    word_599D6 = demoIndex;
    word_599D8 = 0;

    // This checks if the level number has its MSB to 0 and is a valid level number (1-111) for the original DEMO format
    if (demoLevelNumber <= 0x6F // 111
        && demoLevelNumber != 0)
    {
        // TODO: test demos from the original game and recheck anything involving word_599D8
        word_599D8 = (word_599D8 & 0xFF00) | demoLevelNumber; // mov byte ptr word_599D8, al
        finalLevelNumber = demoLevelNumber;
        //dl = al; // TODO: does this mean gDemoIndexOrDemoLevelNumber might store the level number of the demo or the demo number depending on the value of word_599D8 ???
    }

//loc_4B1CF:              ; CODE XREF: handleDemoOptionClick+6Bj
//                ; handleDemoOptionClick+6Fj
    // al = dl;  // TODO: does this mean gDemoIndexOrDemoLevelNumber might store the level number of the demo or the demo number depending on the value of word_599D8 ???
    gRandomGeneratorSeed = gDemoRandomSeeds[demoIndex];
    gDemoIndexOrDemoLevelNumber = finalLevelNumber;

    demoFirstIndex++; // To skip the level number
    word_510DF = demoFirstIndex;
    word_5A33C = demoFirstIndex;
    byte_510E1 = 0;
    byte_510E2 = 1;
}

void playDemo(uint16_t demoIndex) // demoSomething  proc near       ; CODE XREF: start+3BAp
                    // ; runMainMenu+12Ep ...
{
    uint8_t *demosAsByteArray = (uint8_t *)&gDemos;

    readDemoFiles();

    gRandomGeneratorSeed = gDemoRandomSeeds[demoIndex];
    word_5196C = 1;
    gIsPlayingDemo = 1;

    uint16_t demoFirstIndex = gDemos.demoFirstIndices[demoIndex];
    if (demoFirstIndex == 0xFFFF)
    {
        word_5196C = 0;
        gIsPlayingDemo = 0;
    }

//loc_4B22F:              ; CODE XREF: playDemo+30j
    word_599D8 = 0;

    uint8_t demoLevelNumber = demosAsByteArray[demoFirstIndex];
    uint8_t finalLevelNumber = demoIndex;

    if (demoLevelNumber <= 0x6F // 111
        && demoLevelNumber != 0)
    {
        // TODO: test demos from the original game and recheck anything involving word_599D8
        finalLevelNumber = demoLevelNumber;
        word_599D8 = (word_599D8 & 0xFF00) | finalLevelNumber; // mov byte ptr word_599D8, al
    }

//loc_4B248:              ; CODE XREF: playDemo+4Bj
//                ; playDemo+4Fj
    gDemoIndexOrDemoLevelNumber = finalLevelNumber;

    demoFirstIndex++; // To skip the level number
    word_510DF = demoFirstIndex;
    word_5A33C = demoFirstIndex;
    byte_510E1 = 0;
    byte_510E2 = 1;
}

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
    gIsPlayingDemo = 0;

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
    // 01ED:47B6
//loc_4B433:              ; CODE XREF: sub_4B419+15j
    ax = word_5195D;
    ax -= word_59B8C;
    if (ax < word_59B8E)
    {
        return;
    }

//loc_4B443:              ; CODE XREF: sub_4B419+25j
    word_59B8E = word_5195D;
    if (word_59B8C > 1)
    {
        word_59B8C--;
    }

    FILE *file = NULL;
    char currentSuffix[3] = "AT";

    do
    {
//loc_4B454:              ; CODE XREF: sub_4B419+35j
//                ; sub_4B419+9Aj
        strcpy(currentSuffix, &gLevelsDatFilename[8]);

        // Pressing the shift key will show the level sets in descending order
        uint8_t shouldSwitchInDescendingOrder = (gIsRightShiftPressed || gIsLeftShiftPressed);

        if (shouldSwitchInDescendingOrder)
        {
//loc_4B482:              ; CODE XREF: sub_4B419+46j
            if (strcmp(currentSuffix, "AT") == 0) // "AT"
            {
                strcpy(currentSuffix, "99");
            }
//loc_4B48C:              ; CODE XREF: sub_4B419+6Cj
            else if (strcmp(currentSuffix, "00") == 0) // "00"
            {
                strcpy(currentSuffix, "AT");
            }
            else
            {
//loc_4B496:              ; CODE XREF: sub_4B419+76j
                currentSuffix[1]--;
                if (currentSuffix[1] < '0'
                    && currentSuffix[0] > '0')
                {
                    currentSuffix[1] = '9'; // '9'
                    currentSuffix[0]--;
                }
            }
        }
        else
        {
            if (strcmp(currentSuffix, "AT") == 0) // "AT"
            {
                strcpy(currentSuffix, "00");
            }
//loc_4B46B:              ; CODE XREF: sub_4B419+4Bj
            else if (strcmp(currentSuffix, "99") == 0) // "99"
            {
                strcpy(currentSuffix, "AT");
            }
            else
            {
//loc_4B475:              ; CODE XREF: sub_4B419+55j
                currentSuffix[1]++;
                if (currentSuffix[1] > '9'
                    && currentSuffix[0] < '9')
                {
                    currentSuffix[1] = '0'; // '0'
                    currentSuffix[0]++;
                }
            }
        }

//loc_4B4A3:              ; CODE XREF: sub_4B419+50j
//                ; sub_4B419+5Aj ...
        strcpy(&gLevelsDatFilename[8], currentSuffix);

        file = openReadonlyFile(gLevelsDatFilename, "r");
        if (file == NULL)
        {
            if (errno != ENOENT)
            {
                exitWithError("Error opening %s\n", gLevelsDatFilename);
            }
        }
    }
    while (file == NULL);

//loc_4B4B8:              ; CODE XREF: sub_4B419+95j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing %s\n", gLevelsDatFilename);
    }

//loc_4B4C6:              ; CODE XREF: sub_4B419+A8j
    char message[] = "     LEVEL SET ??      ";
    memcpy(&message[0xF], currentSuffix, 2);

    if (strcmp(currentSuffix, "AT") == 0)
    {
        strcpy(currentSuffix, "ST");
    }

//loc_4B4D3:              ; CODE XREF: sub_4B419+B6j
    strcpy(&gLevelLstFilename[7], currentSuffix);
    strcpy(&gPlayerLstFilename[8], currentSuffix);
    strcpy(&gHallfameLstFilename[0xA], currentSuffix);

    if (strcmp(currentSuffix, "ST") == 0)
    {
        strcpy(currentSuffix, "IN");
    }

//loc_4B4E4:              ; CODE XREF: sub_4B419+C6j
    strcpy(&gDemo0BinFilename[7], currentSuffix);

    if (strcmp(currentSuffix, "IN") == 0)
    {
        strcpy(currentSuffix, "AV");
    }

//loc_4B4EF:              ; CODE XREF: sub_4B419+D1j
    if ((dword_59B76 & 0xFF) == 0)
    {
        strcpy(&gSavegameSavFilename[0xA], currentSuffix);
    }

//loc_4B4F9:              ; CODE XREF: sub_4B419+DBj
    if (strcmp(currentSuffix, "AV") == 0)
    {
        strcpy(message, "  SUPAPLEX LEVEL SET   ");
    }

//loc_4B504:              ; CODE XREF: sub_4B419+E6j
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, message);
    readLevelsLst();
    readDemoFiles();
//    push    es
//    push    ds
//    pop es
//    assume es:data
    // 01ED:48B2
    if (byte_59B85 != 0)
    {
        // TODO: no idea what byte_59B85 is for yet, but seems to be related to some command line option
        assert(0);
    //    lea di, byte_58DB4+4
        cx = 0x6F; // 11
        al = 2;
        memset(NULL, kNotCompletedLevelEntryColor, 111); // rep stosb, I think this resets the player level state to skipped
    }
    else
    {
//loc_4B52A:              ; CODE XREF: sub_4B419+101j
        for (int i = 0; i < kNumberOfPlayers; ++i)
        {
            PlayerEntry *entry = &gPlayerListData[i];
//loc_4B531:              ; CODE XREF: sub_4B419+129j
            memset(entry, 0, sizeof(PlayerEntry));
            strcpy(entry->name, "--------");
        }

        for (int i = 0; i < kNumberOfHallOfFameEntries; ++i)
        {
            HallOfFameEntry *entry = &gHallOfFameData[i];

//loc_4B54B:              ; CODE XREF: sub_4B419+143j
            memset(entry, 0, sizeof(HallOfFameEntry));
            strcpy(entry->playerName, "        ");
        }

        readHallfameLst();
        readPlayersLst();
    }

//loc_4B565:              ; CODE XREF: sub_4B419+10Fj
    gShouldAutoselectNextLevelToPlay = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    drawLevelList();
    drawHallOfFame();
    drawRankings();
    restoreLastMouseAreaBitmap();
    saveLastMouseAreaBitmap();
    drawMouseCursor();
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
    gShouldAutoselectNextLevelToPlay = 1;
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
    gShouldAutoselectNextLevelToPlay = 1;
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
            for (uint8_t x = 0; x < kBitmapFontCharacter6Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter6Width + destX + x);
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
            for (uint8_t x = 0; x < kBitmapFontCharacter6Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                if (pixelValue == 1)
                {
                    // 6 is the wide (in pixels) of this font
                    size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter6Width + destX + x);
                    gScreenPixels[destAddress] = color;
                }
            }
        }
    }
}

void sub_4BF4A(uint8_t number) //   proc near       ; CODE XREF: start+3F7p handleGameUserInput+398p ...
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

void drawRankings() // sub_4C0DD   proc near       ; CODE XREF: handleNewPlayerOptionClick+1E9p
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
    convertNumberTo3DigitStringWithPadding0(byte_58D46 + 1, numberString);
    drawTextWithChars6FontWithOpaqueBackground(144, 110, 6, &numberString[1]); // Remove the first (left most) digit
}

void drawLevelList() // sub_4C141  proc near       ; CODE XREF: start+41Ap handleGameUserInput+39Bp ...
{
    // 01ED:54DE
    byte_59821 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 2];
    byte_59822 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1];
    byte_59823 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex];

    char *previousLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 2) * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 155, byte_59821, previousLevelName);

    char *currentLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 1) * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 164, byte_59822, currentLevelName);

    memcpy(gCurrentLevelName, currentLevelName, kListLevelNameLength);

    char *nextLevelName = (char *)&gLevelListData[gCurrentSelectedLevelIndex * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackground(144, 173, byte_59823, nextLevelName);
}

void drawHallOfFame() // sub_4C1A9   proc near       ; CODE XREF: handleFloppyDiskButtonClick+15Ap
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

        uint8_t playerNameLength = MIN(strlen(entry.playerName), sizeof(entry.playerName) - 1);
        memcpy(text, entry.playerName, playerNameLength);

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

void drawMenuTitleAndDemoLevelResult() // sub_4C2F2   proc near       ; CODE XREF: handleGfxTutorOptionClick+Cp
                    // ; sub_4C407+1Fp ...
{
    // 01ED:568F
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, "  WELCOME TO SUPAPLEX  ");
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
            message = "     LEVEL FAILED      ";
        }
        else
        {
            message = "      DEMO FAILED      ";
        }
    }
    else
    {
        if (byte_5A19C == 0)
        {
            message = "   LEVEL SUCCESSFUL    ";
        }
        else
        {
            message = "    DEMO SUCCESSFUL    ";
        }
    }

//loc_4C33C:              // ; CODE XREF: drawMenuTitleAndDemoLevelResult+34j
                // ; drawMenuTitleAndDemoLevelResult+39j ...
    drawTextWithChars6FontWithOpaqueBackground(168, 127, 4, message);
    byte_5A19B = 0;
}

void prepareLevelDataForCurrentPlayer() // sub_4C34A   proc near       ; CODE XREF: start+404p handleNewPlayerOptionClick+1E0p ...
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
        if (gShouldAutoselectNextLevelToPlay != 0)
        {
            gCurrentSelectedLevelIndex = kLastLevelIndex;
        }

//loc_4C3D1:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+7Fj
        currentPlayerEntry.nextLevelToPlay = kLastLevelIndex;
        return;
    }

//loc_4C3D6:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+61j
                // ; prepareLevelDataForCurrentPlayer+74j
    if (gShouldAutoselectNextLevelToPlay != 0)
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
    // 01ED:57A4
    gNumberOfDotsToShiftDataLeft = 0;
    if (gShouldShowFailedLevelResultScreen != 0)
    {
        gShouldShowFailedLevelResultScreen = 0;
        drawFailedLevelResultScreen(); // 01ED:57B5
        drawMenuBackground(); // 01ED:57B8
        gShouldAutoselectNextLevelToPlay = 0;
        prepareLevelDataForCurrentPlayer();
        drawMenuTitleAndDemoLevelResult();
    //    mov si, 6015h
        fadeToPalette(gPalettes[1]);

        // gNumberOfDotsToShiftDataLeft = 0;
        videoloop();

        // This will prevent to leave traces of the options menu
        // area in the main menu.
        //
        saveLastMouseAreaBitmap();
    }
    else
    {
//loc_4C449:              ; CODE XREF: sub_4C407+3Aj
        scrollLeftToMainMenu();
    }
}

void scrollLeftToMainMenu() //loc_4C44F:              ; CODE XREF: handleGfxTutorOptionClick+9p
{
    uint8_t currentScreenPixels[kFullScreenFramebufferLength];
    memcpy(currentScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    drawMenuBackground();
    gShouldAutoselectNextLevelToPlay = 0;

    prepareLevelDataForCurrentPlayer();
    drawMenuTitleAndDemoLevelResult();

    uint8_t menuScreenPixels[kFullScreenFramebufferLength];
    memcpy(menuScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 80;

    static const uint32_t kAnimationDuration = kNumberOfSteps * 1000 / 70; // ~571 ms
    uint32_t animationTime = 0;

    startTrackingRenderDeltaTime();

    // Draws the current scroll animation step
    while (animationTime < kAnimationDuration)
    {
        animationTime += updateRenderDeltaTime();
        animationTime = MIN(animationTime, kAnimationDuration);

        float animationFactor = (float)animationTime / kAnimationDuration;

        int limitFromLeft = animationFactor * kScreenWidth;
        int limitFromRight = kScreenWidth - limitFromLeft;

        for (int y = 0; y < kScreenHeight; ++y)
        {
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
        videoloop();
    }

//loc_4C499:              ; CODE XREF: sub_4C407+28j
    // This will prevent to leave traces of the options menu
    // area in the main menu.
    //
    saveLastMouseAreaBitmap();
}

void drawFailedLevelResultScreen() // sub_4C4F9   proc near       ; CODE XREF: sub_4C407+11p
{
    setPalette(gBlackPalette);
    drawBackBackground();

    drawTextWithChars6FontWithTransparentBackground(128, 60, 0xF, "HARD LUCK!");
    if (gNumberOfRemainingInfotrons == 0)
    {
        drawTextWithChars6FontWithTransparentBackground(40, 80, 0xF, "YOU COMPLETED ALL THE NECESSARY INFOTRONS");
        drawTextWithChars6FontWithTransparentBackground(72, 100, 0xF, "BUT FAILED TO REACH THE EXIT");
    }
    else
    {
//loc_4C52C:              ; CODE XREF: drawFailedLevelResultScreen+19j
        char message[] = "YOU HAVE COLLECTED ??? OUT OF THE ???";

        uint8_t collectedInfotrons = gTotalNumberOfInfotrons - gNumberOfRemainingInfotrons;
        convertNumberTo3DigitPaddedString(collectedInfotrons, &message[19], 1);

        convertNumberTo3DigitPaddedString(gTotalNumberOfInfotrons, &message[34], 1);

        drawTextWithChars6FontWithTransparentBackground(40, 80, 0xF, message);
        drawTextWithChars6FontWithTransparentBackground(104, 100, 0xF, "INFOTRONS NEEDED");
    }

//loc_4C55C:              ; CODE XREF: drawFailedLevelResultScreen+31j
    drawTextWithChars6FontWithTransparentBackground(72, 120, 0xF, "WHY NOT GIVE IT ANOTHER TRY?");

    videoloop();
    setPalette(gPalettes[0]);
    if (gShouldExitGame != 1)
    {
        waitForKeyMouseOrJoystick();
    }

//loc_4C591:              ; CODE XREF: drawFailedLevelResultScreen+93j
    setPalette(gBlackPalette);
}

void scrollRightToNewScreen() // sub_4C5AF   proc near       ; CODE XREF: handleGfxTutorOptionClick+3p
{
//    gNumberOfDotsToShiftDataLeft = 0;
    videoloop();

    uint8_t screenPixelsBackup[kFullScreenFramebufferLength];
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 80;

    static const uint32_t kAnimationDuration = kNumberOfSteps * 1000 / 70; // ~571 ms
    uint32_t animationTime = 0;

    startTrackingRenderDeltaTime();

    // Draws the current scroll animation step
    while (animationTime < kAnimationDuration)
    {
        animationTime += updateRenderDeltaTime();
        animationTime = MIN(animationTime, kAnimationDuration);

        float animationFactor = (float)animationTime / kAnimationDuration;

        int limitFromRight = animationFactor * kScreenWidth;
        int limitFromLeft = kScreenWidth - limitFromRight;

        for (int y = 0; y < kScreenHeight; ++y)
        {
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
        videoloop();
    }
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
        gShouldAutoselectNextLevelToPlay = 1;
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
        int9handler(0);

//loc_4C7FD:              // ; CODE XREF: runMainMenu+121j
                   // ; runMainMenu+219j ...
        word_58465++;
        if (word_58465 == 0)
        {
            handleDemoOptionClick();
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
        updateUserInput();
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
        if (gCurrentUserInput > kUserInputSpaceAndDirectionOffset)
        {
            handleOkButtonClick();
        }
//loc_4C8B8:              // ; CODE XREF: runMainMenu+11Cj
        else if (gIsF1KeyPressed == 1)
        {
            playDemo(0);
        }
//loc_4C8C8:              // ; CODE XREF: runMainMenu+129j
        else if (gIsF2KeyPressed == 1)
        {
            playDemo(1);
        }
//loc_4C8D8:              // ; CODE XREF: runMainMenu+139j
        else if (gIsF3KeyPressed == 1)
        {
            playDemo(2);
        }
//loc_4C8E8:              // ; CODE XREF: runMainMenu+149j
        else if (gIsF4KeyPressed == 1)
        {
            playDemo(3);
        }
//loc_4C8F8:              // ; CODE XREF: runMainMenu+159j
        else if (gIsF5KeyPressed == 1)
        {
            playDemo(4);
        }
//loc_4C908:              // ; CODE XREF: runMainMenu+169j
        else if (gIsF6KeyPressed == 1)
        {
            playDemo(5);
        }
//loc_4C918:              // ; CODE XREF: runMainMenu+179j
        else if (gIsF7KeyPressed == 1)
        {
            playDemo(6);
        }
//loc_4C928:              // ; CODE XREF: runMainMenu+189j
        else if (gIsF8KeyPressed == 1)
        {
            playDemo(7);
        }
//loc_4C937:              // ; CODE XREF: runMainMenu+199j
        else if (gIsF9KeyPressed == 1)
        {
            playDemo(8);
        }
//loc_4C946:              // ; CODE XREF: runMainMenu+1A8j
        else if (gIsF10KeyPressed == 1)
        {
            playDemo(9);
        }
//loc_4C955:              // ; CODE XREF: runMainMenu+1B7j
        else if (gIsNumpadDividePressed == 1
                 && demoFileName != 0
                 && fileIsDemo == 1)
        {
            byte_599D4 = 1;
            playDemo(0);
        }
//loc_4C977:              // ; CODE XREF: runMainMenu+1C6j
                    // ; runMainMenu+1CDj ...
        else if (byte_519D5 == 1
                 && demoFileName != 0)
        {
            byte_599D4 = 1;
            word_5196C = 1;
            gIsPlayingDemo = 0;
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
        if (gIsEscapeKeyPressed == 1)
        {
            break;
        }
        if (gShouldExitGame == 1)
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
    gShouldExitGame = 1;
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
        int9handler(0);

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
        if (gIsEscapeKeyPressed == 1)
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
    // 01ED:60D9
    sub_48E59();
    updateUserInput();
    if (gCurrentUserInput == byte_50919)
    {
        return;
    }

//loc_4CD4D:              ; CODE XREF: updateOptionsMenuState+Ej
    byte_50919 = gCurrentUserInput;
    if (gCurrentUserInput == UserInputNone)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[12], 6, destBuffer);
        drawOptionsMenuLine(kOptionsMenuBorders[17], 4, destBuffer);
    }
    else
    {
//loc_4CD6A:              ; CODE XREF: updateOptionsMenuState+1Aj
        if (gCurrentUserInput <= kUserInputSpaceAndDirectionOffset)
        {
//loc_4CD9E:              ; CODE XREF: updateOptionsMenuState+33j
            drawOptionsMenuLine(kOptionsMenuBorders[12], 4, destBuffer);
            drawOptionsMenuLine(kOptionsMenuBorders[17], 4, destBuffer);
        }
        else
        {
            drawOptionsMenuLine(kOptionsMenuBorders[17], 6, destBuffer);
            if (gCurrentUserInput != UserInputSpaceOnly)
            {
//loc_4CD8F:              ; CODE XREF: updateOptionsMenuState+42j
                gCurrentUserInput -= kUserInputSpaceAndDirectionOffset;
                drawOptionsMenuLine(kOptionsMenuBorders[12], 4, destBuffer);
            }
            else
            {
                gCurrentUserInput = UserInputNone;
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
    if (gCurrentUserInput == UserInputUp)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[13], 6, destBuffer);
    }
//loc_4CDDF:              ; CODE XREF: updateOptionsMenuState+97j
    else if (gCurrentUserInput == UserInputLeft)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[14], 6, destBuffer);
    }
//loc_4CDF0:              ; CODE XREF: updateOptionsMenuState+A8j
    else if (gCurrentUserInput == UserInputDown)
    {
        drawOptionsMenuLine(kOptionsMenuBorders[15], 6, destBuffer);
    }
//loc_4CE01:              ; CODE XREF: updateOptionsMenuState+B9j
    else if (gCurrentUserInput == UserInputRight)
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

    FILE *file = openWritableFile(gPlayerLstFilename, "w");
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

    FILE *file = openWritableFile(gHallfameLstFilename, "w");
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
    if (gIsPlayingDemo != 0)
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
    if (gIsPlayingDemo != 0)
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

void initializeFadePalette() //   proc near       ; CODE XREF: start+296p
{
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
    gMouseX = kScreenWidth / 2;
    gMouseY = kScreenHeight / 2;
    SDL_ShowCursor(SDL_DISABLE);
    handleSDLEvents();
}

void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus) //   proc near       ; CODE XREF: waitForKeyMouseOrJoystick:mouseIsClickedp
//                    ; waitForKeyMouseOrJoystick+3Ep ...
{
    // Returns coordinate X in CX (0-320) and coordinate Y in DX (0-200).
    // Also button status in BX.

    if (gIsMouseAvailable != 0)
    {
        handleSDLEvents();

        int x, y;
        Uint32 state = SDL_GetMouseState(&x, &y);

        int windowWidth, windowHeight;
        SDL_GetWindowSize(gWindow, &windowWidth, &windowHeight);

        float controllerX = 0, controllerY = 0;
        uint8_t controllerLeftButton = 0;
        uint8_t controllerRightButton = 0;
        gameControllerEmulateMouse(&controllerX,
                                   &controllerY,
                                   &controllerLeftButton,
                                   &controllerRightButton);

        if (controllerX != 0.0 || controllerY != 0.0)
        {
            float speed = (float) windowWidth / 1280;

            x += speed * controllerX;
            y += speed * controllerY;

            // Correct mouse position for future events
            SDL_WarpMouseInWindow(gWindow, x, y);
        }

        // Read touch screen where available
        float touchScreenX, touchScreenY;
        uint8_t touchScreenPressed = readTouchScreen(&touchScreenX, &touchScreenY);
        if (touchScreenPressed)
        {
            x = touchScreenX * windowWidth;
            y = touchScreenY * windowHeight;

            // Correct mouse position for future events
            SDL_WarpMouseInWindow(gWindow, x, y);
        }

        x = x * kScreenWidth / windowWidth;
        y = y * kScreenHeight / windowHeight;

        uint8_t leftButtonPressed = (state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        uint8_t rightButtonPressed = (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

        leftButtonPressed = (leftButtonPressed
                             || controllerLeftButton
                             || touchScreenPressed);
        rightButtonPressed = (rightButtonPressed
                              || controllerRightButton);

//        uint8_t shouldCorrectMousePosition = 0;


        // Limit coordinates as in the original game
        x = CLAMP(x, 16, 304);
        y = CLAMP(y, 8, 192);

//        if (shouldCorrectMousePosition)
        {
//            SDL_WarpMouseInWindow(gWindow, x, y);
        }

        if (mouseX != NULL)
        {
            *mouseX = x;
        }
        if (mouseY != NULL)
        {
            *mouseY = y;
        }

        if (mouseButtonStatus != NULL)
        {
            *mouseButtonStatus = (rightButtonPressed << 1
                                  | leftButtonPressed);
        }
    }
}

void limitFPS()
{
    static const double kMaximumFPS = 70.0;
    static const double kFrameDuration = 1000.0 / kMaximumFPS;
    static Uint32 sLastFrameTime = 0;

    if (sLastFrameTime != 0)
    {
        Uint32 duration = (SDL_GetTicks() - sLastFrameTime);
        if (duration < kFrameDuration)
        {
            SDL_Delay(kFrameDuration - duration);
        }
    }

    sLastFrameTime = SDL_GetTicks();
}

void videoloop() //   proc near       ; CODE XREF: crt?2+52p crt?1+3Ep ...
{
    if (gShouldShowFPS)
    {
        char frameRateString[5] = "";
        sprintf(frameRateString, "%4.1f", MIN(gFrameRate, 99.9)); // Don't show more than 99.9 FPS, not necessary

        // TODO: No idea what this is _yet_ but I can't print on the screen if it's 1
        uint8_t previousValue = byte_5A33F;
        byte_5A33F = 0;
        drawTextWithChars6FontWithOpaqueBackground(0, 0, 6, frameRateString);
        byte_5A33F = previousValue;
    }

    handleSDLEvents(); // Make sure the app stays responsive

    SDL_BlitSurface(gScreenSurface, NULL, gTextureSurface, NULL);

    SDL_UpdateTexture(gTexture, NULL, gTextureSurface->pixels, gTextureSurface->pitch);
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, &gWindowViewport);
    SDL_RenderPresent(gRenderer);

    limitFPS();

    static Uint32 sNumberOfFrames = 0;

    sNumberOfFrames++;

    if (gFrameRateReferenceTime == 0)
    {
        gFrameRateReferenceTime = SDL_GetTicks();
    }
    else
    {
        Uint32 difference = SDL_GetTicks() - gFrameRateReferenceTime;

        if (difference > 1000)
        {
            gFrameRate = sNumberOfFrames * 1000.f / difference;
            sNumberOfFrames = 0;
            gFrameRateReferenceTime = SDL_GetTicks();
        }
    }
}

void readLevels() //  proc near       ; CODE XREF: start:loc_46F3Ep
                    // ; sub_4A463p
{
    // 01ED:68E5
    char *filename = "";
    FILE *file = NULL;
    Level fileLevelData;

    if (gIsPlayingDemo != 0
        && (word_599D8 & 0xFF) == 0
        && byte_599D4 == 0)
    {
        // Demos with the new format
        Level *level = &gDemos.level[gDemoIndexOrDemoLevelNumber];

        memcpy(&fileLevelData, level, levelDataLength);

        strcpy(gCurrentDemoLevelIdentifier, ".SP");

        memcpy(gCurrentDemoLevelName, level->name, sizeof(level->name));
    }
    else
    {
        if (gIsPlayingDemo == 0
            || byte_599D4 != 0)
        {
//loc_4D59F:              ; CODE XREF: readLevels+5j
//                ; readLevels+13j
            filename = gLevelsDatFilename; // lea dx, aLevels_dat_0 ; "LEVELS.DAT"
        }

        if (gIsPlayingDemo != 0
            && (word_599D8 & 0xFF) != 0) //cmp byte ptr word_599D8, 0
        {
//loc_4D599:              ; CODE XREF: readLevels+Cj
            filename = gLevelsDatFilename; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }
//loc_4D5A3:              ; CODE XREF: readLevels+55j
        else if (byte_599D4 != 0)
        {
            // TODO: add support for demos run from command line
            // filename = demoFileName; //    mov dx, offset demoFileName
        }
        else if (word_599DA != 0)
        {
            filename = gLevelsDatFilename; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }

//loc_4D5BB:              ; CODE XREF: readLevels+63j
        file = openReadonlyFile(filename, "r");
        if (file == NULL)
        {
            exitWithError("Error opening %s\n", filename);
        }

        uint8_t levelIndex = 0;

//loc_4D5C2:              ; CODE XREF: readLevels+75j
        if (gIsPlayingDemo != 0)
        {
            levelIndex = gDemoIndexOrDemoLevelNumber;
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

            gDemoIndexOrDemoLevelNumber = word_599D6;

            Level *level = &gDemos.level[gDemoIndexOrDemoLevelNumber];

            memcpy(level, &fileLevelData, levelDataLength);
        }
    }

    char *levelName = NULL;

//loc_4D64B:              ; CODE XREF: readLevels+4Ej
//                ; readLevels+D5j
    if (gIsPlayingDemo != 0)
    {
        gRandomGeneratorSeed = word_51076;
        levelName = gCurrentDemoLevelName;
    }
    else
    {
//loc_4D65D:              ; CODE XREF: readLevels+108j
        levelName = gCurrentLevelName;
    }

//loc_4D660:              ; CODE XREF: readLevels+113j
    if (word_599D8 != 0
        || (byte_599D4 != 0
            && word_599DA != 0))
    {
//loc_4D679:              ; CODE XREF: readLevels+121j
        strcpy(gCurrentDemoLevelIdentifier, "BIN");
    }
    else if (byte_599D4 == 0)
    {
//loc_4D68C:              ; CODE XREF: readLevels+128j
        levelName += 4; // Skips the number directly to the title (from pointing "005 ------- EASY DEAL -------" to pointing "------- EASY DEAL -------")
    }
    else if (word_599DA == 0)
    {
//loc_4D682:              ; CODE XREF: readLevels+12Fj
        // TODO: not sure if this code is executed only for demos
        strcpy(gCurrentDemoLevelIdentifier, ".SP"); // TODO: not sure if this code is executed only for demos
    }

//loc_4D68F:              ; CODE XREF: readLevels+142j
    memcpy(levelName, fileLevelData.name, sizeof(fileLevelData.name));

    memcpy(&gCurrentLevel, &fileLevelData, sizeof(gCurrentLevel));

    for (int i = 0; i < kLevelSize; ++i) // originally was levelDataLength but sounds like a bug
    {
//loc_4D6B8:              ; CODE XREF: readLevels+172j
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        tile->tile = fileLevelData.tiles[i];
        tile->movingObject = 0;
    }

    memset(&gCurrentLevelAfterWord, 0, sizeof(gCurrentLevelAfterWord)); // rep stosb

    if (gIsPlayingDemo == 0
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

    // The original animation had 64 steps, and the game was written to run in 70Hz displays
    static const uint32_t kFadeDuration = 64 * 1000 / 70; // ~914 ms
    uint32_t fadeTime = 0;

    startTrackingRenderDeltaTime();

    // for (uint8_t step = 0; step < totalSteps; ++step)
    while (fadeTime < kFadeDuration)
    {
        fadeTime += updateRenderDeltaTime();
        fadeTime = MIN(fadeTime, kFadeDuration);

        float animationFactor = (float)fadeTime / kFadeDuration;
        float complementaryAnimationFactor = 1.0 - animationFactor;

        for (uint8_t i = 0; i < kNumberOfColors; ++i)
        {
            uint8_t r = (palette[i].r * animationFactor) + (gCurrentPalette[i].r * complementaryAnimationFactor);
            uint8_t g = (palette[i].g * animationFactor) + (gCurrentPalette[i].g * complementaryAnimationFactor);
            uint8_t b = (palette[i].b * animationFactor) + (gCurrentPalette[i].b * complementaryAnimationFactor);

            intermediatePalette[i] = (SDL_Color) { r, g, b, 255};
        }

        SDL_SetPaletteColors(gScreenSurface->format->palette, intermediatePalette, 0, kNumberOfColors);

        videoloop();
    }

    setPalette(palette);
//        word_510A2 = old_word_510A2;
}

void replaceCurrentPaletteColor(uint8_t index, SDL_Color color)
{
    gCurrentPalette[index] = color;
    setPalette(gCurrentPalette);
}

void setPalette(ColorPalette palette) // sub_4D836   proc near       ; CODE XREF: start+2B8p
                   // ; loadScreen2+B5p ...
{
//    int old_word_510A2; //       = word ptr -2

//    old_word_510A2 = word_510A2;
//    word_510A2 = 0;
#ifdef __vita__
    // For some reason (SDL bug?) in PS Vita using SDL_SetPaletteColors here
    // specifically breaks the game colors. This seems to work, will investigate later...
    //
    for (uint8_t i = 0; i < kNumberOfColors; ++i)
    {
        gScreenSurface->format->palette->colors[i].r = palette[i].r;
        gScreenSurface->format->palette->colors[i].g = palette[i].g;
        gScreenSurface->format->palette->colors[i].b = palette[i].b;
    }
    gScreenSurface->format->palette->version++;
    if (gScreenSurface->format->palette->version == 0)
    {
        gScreenSurface->format->palette->version = 1;
    }
#else
    SDL_SetPaletteColors(gScreenSurface->format->palette, palette, 0, kNumberOfColors);
#endif

    memcpy(gCurrentPalette, palette, sizeof(ColorPalette));
//        word_510A2 = old_word_510A2;
}
/*
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
    FILE *file = openReadonlyFile(filename, "r");
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
    FILE *file = openReadonlyFile(filename, "r");
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

void sound4() //     proc near       ; CODE XREF: detonateBigExplosion+2EDp code:5ADEp ...
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

void sound5() //     proc near       ; CODE XREF: update?:loc_4E55Cp
                //    ; update?:loc_4E588p ...
{
    /*
    cmp isFXEnabled, 1
    jz  short loc_4DBE8
    return;

loc_4DBE8:              ; CODE XREF: sound5+5j
    cmp byte_59889, 5
    jl  short loc_4DBF0
    return;

loc_4DBF0:              ; CODE XREF: sound5+Dj
    mov byte_5988B, 0Fh
    mov byte_59889, 4
    cmp sndType, 1
    jnz short loc_4DC08
    mov ax, 401h
    int 80h     ; LINUX -
    jmp short locret_4DC48

loc_4DC08:              ; CODE XREF: sound5+1Fj
    cmp sndType, 2
    jnz short loc_4DC1D
    mov dx, 5D38h
    mov ah, 3
    int 81h
    mov ax, 1
    int 81h
    jmp short locret_4DC48

loc_4DC1D:              ; CODE XREF: sound5+2Dj
    cmp sndType, 3
    jnz short loc_4DC2E
    mov ax, 401h
    mov dx, 388h
    int 80h     ; LINUX -
    jmp short locret_4DC48

loc_4DC2E:              ; CODE XREF: sound5+42j
    cmp sndType, 4
    jnz short loc_4DC3C
    mov ax, 1
    int 81h
    jmp short locret_4DC48

loc_4DC3C:              ; CODE XREF: sound5+53j
    cmp sndType, 5
    jnz short locret_4DC48
    mov ax, 401h
    int 80h     ; LINUX -

locret_4DC48:               ; CODE XREF: sound5+26j sound5+3Bj ...
    return;
     */
}

void sound6() //     proc near       ; CODE XREF: update?+B8Bp
                //    ; update?+136Cp
{
    /*
    cmp isFXEnabled, 1
    jz  short loc_4DC51
    return;

loc_4DC51:              ; CODE XREF: sound6+5j
    cmp byte_59889, 2
    jl  short loc_4DC59
    return;

loc_4DC59:              ; CODE XREF: sound6+Dj
    mov byte_5988B, 7
    mov byte_59889, 2
    cmp sndType, 1
    jnz short loc_4DC71
    mov ax, 402h
    int 80h     ; LINUX -
    jmp short locret_4DCB1

loc_4DC71:              ; CODE XREF: sound6+1Fj
    cmp sndType, 2
    jnz short loc_4DC86
    mov dx, 5D38h
    mov ah, 3
    int 81h
    mov ax, 2
    int 81h
    jmp short locret_4DCB1

loc_4DC86:              ; CODE XREF: sound6+2Dj
    cmp sndType, 3
    jnz short loc_4DC97
    mov ax, 402h
    mov dx, 388h
    int 80h     ; LINUX -
    jmp short locret_4DCB1

loc_4DC97:              ; CODE XREF: sound6+42j
    cmp sndType, 4
    jnz short loc_4DCA5
    mov ax, 2
    int 81h
    jmp short locret_4DCB1

loc_4DCA5:              ; CODE XREF: sound6+53j
    cmp sndType, 5
    jnz short locret_4DCB1
    mov ax, 402h
    int 80h     ; LINUX -

locret_4DCB1:               ; CODE XREF: sound6+26j sound6+3Bj ...
    return;
    */
}

void sound7() //     proc near       ; CODE XREF: movefun:loc_48125p
//                    ; movefun2:loc_48573p
{
    /*
        cmp isFXEnabled, 1
        jz  short loc_4DCBA
        return;

loc_4DCBA:              ; CODE XREF: sound7+5j
        cmp byte_59889, 2
        jl  short loc_4DCC2
        return;

loc_4DCC2:              ; CODE XREF: sound7+Dj
        mov byte_5988B, 7
        mov byte_59889, 2
        cmp sndType, 1
        jnz short loc_4DCDA
        mov ax, 403h
        int 80h     ; LINUX -
        jmp short locret_4DD1A

loc_4DCDA:              ; CODE XREF: sound7+1Fj
        cmp sndType, 2
        jnz short loc_4DCEF
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 3
        int 81h
        jmp short locret_4DD1A

loc_4DCEF:              ; CODE XREF: sound7+2Dj
        cmp sndType, 3
        jnz short loc_4DD00
        mov ax, 403h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DD1A

loc_4DD00:              ; CODE XREF: sound7+42j
        cmp sndType, 4
        jnz short loc_4DD0E
        mov ax, 3
        int 81h
        jmp short locret_4DD1A

loc_4DD0E:              ; CODE XREF: sound7+53j
        cmp sndType, 5
        jnz short locret_4DD1A
        mov ax, 403h
        int 80h     ; LINUX -

locret_4DD1A:               ; CODE XREF: sound7+26j sound7+3Bj ...
        return;
     */
}

void sound8() //     proc near       ; CODE XREF: movefun7:loc_4A0ABp
{
    /*
    cmp isFXEnabled, 1
    jz  short loc_4DD23
    return;

loc_4DD23:              ; CODE XREF: sound8+5j
    cmp byte_59889, 3
    jl  short loc_4DD2B
    return;

loc_4DD2B:              ; CODE XREF: sound8+Dj
    mov byte_5988B, 3
    mov byte_59889, 3
    cmp sndType, 1
    jnz short loc_4DD43
    mov ax, 404h
    int 80h     ; LINUX -
    jmp short locret_4DD83

loc_4DD43:              ; CODE XREF: sound8+1Fj
    cmp sndType, 2
    jnz short loc_4DD58
    mov dx, 5D38h
    mov ah, 3
    int 81h
    mov ax, 4
    int 81h
    jmp short locret_4DD83

loc_4DD58:              ; CODE XREF: sound8+2Dj
    cmp sndType, 3
    jnz short loc_4DD69
    mov ax, 404h
    mov dx, 388h
    int 80h     ; LINUX -
    jmp short locret_4DD83

loc_4DD69:              ; CODE XREF: sound8+42j
    cmp sndType, 4
    jnz short loc_4DD77
    mov ax, 4
    int 81h
    jmp short locret_4DD83

loc_4DD77:              ; CODE XREF: sound8+53j
    cmp sndType, 5
    jnz short locret_4DD83
    mov ax, 404h
    int 80h     ; LINUX -

locret_4DD83:               ; CODE XREF: sound8+26j sound8+3Bj ...
    return;
*/
}

void sound9() //     proc near       ; CODE XREF: runLevel+2F4p
                //    ; update?:loc_4E3E1p ...
{
    /*
    cmp isFXEnabled, 1
    jz  short xxxxxxxxdcdc
    return;

xxxxxxxxdcdc:               ; CODE XREF: sound9+5j
    cmp byte_59889, 1
    jl  short loc_4DD94
    return;

loc_4DD94:              ; CODE XREF: sound9+Dj
    mov byte_5988B, 3
    mov byte_59889, 1
    cmp sndType, 1
    jnz short loc_4DDAC
    mov ax, 405h
    int 80h     ; LINUX -
    jmp short locret_4DDEC

loc_4DDAC:              ; CODE XREF: sound9+1Fj
    cmp sndType, 2
    jnz short loc_4DDC1
    mov dx, 5D38h
    mov ah, 3
    int 81h
    mov ax, 5
    int 81h
    jmp short locret_4DDEC

loc_4DDC1:              ; CODE XREF: sound9+2Dj
    cmp sndType, 3
    jnz short loc_4DDD2
    mov ax, 405h
    mov dx, 388h
    int 80h     ; LINUX -
    jmp short locret_4DDEC

loc_4DDD2:              ; CODE XREF: sound9+42j
    cmp sndType, 4
    jnz short loc_4DDE0
    mov ax, 5
    int 81h
    jmp short locret_4DDEC

loc_4DDE0:              ; CODE XREF: sound9+53j
    cmp sndType, 5
    jnz short locret_4DDEC
    mov ax, 405h
    int 80h     ; LINUX -

locret_4DDEC:               ; CODE XREF: sound9+26j sound9+3Bj ...
    return;
    */
}

void sound10() //    proc near       ; CODE XREF: update?+7EBp
{
    /*
    cmp isFXEnabled, 1
    jz  short loc_4DDF5
    return;

loc_4DDF5:              ; CODE XREF: sound10+5j
    mov byte_5988B, 0FAh ; '?'
    mov byte_59889, 0Ah
    call    sound3
    cmp sndType, 1
    jnz short loc_4DE10
    mov ax, 1
    int 80h     ; LINUX - sys_exit
    jmp short locret_4DE5E

loc_4DE10:              ; CODE XREF: sound10+1Aj
    cmp sndType, 2
    jnz short loc_4DE25
    mov dx, 5D38h
    mov ah, 3
    int 81h
    mov ax, 6
    int 81h
    jmp short locret_4DE5E

loc_4DE25:              ; CODE XREF: sound10+28j
    cmp sndType, 3
    jnz short loc_4DE36
    mov ax, 1
    mov dx, 388h
    int 80h     ; LINUX - sys_exit
    jmp short locret_4DE5E

loc_4DE36:              ; CODE XREF: sound10+3Dj
    cmp sndType, 4
    jnz short loc_4DE52
    cmp musType, 5
    jnz short loc_4DE4B
    mov ax, 1
    int 80h     ; LINUX - sys_exit
    jmp short locret_4DE5E

loc_4DE4B:              ; CODE XREF: sound10+55j
    mov ax, 6
    int 81h
    jmp short locret_4DE5E

loc_4DE52:              ; CODE XREF: sound10+4Ej
    cmp sndType, 5
    jnz short locret_4DE5E
    mov ax, 1
    int 80h     ; LINUX - sys_exit

locret_4DE5E:               ; CODE XREF: sound10+21j sound10+36j ...
    return;
     */
}

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
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================
*/

 uint16_t updateMurphy(uint16_t position) // update?     proc near       ; CODE XREF: updateMovingObjects+Ep
{
    // 01ED:722D

    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];

    if (murphyTile->tile != LevelTileTypeMurphy)
    {
        word_510CF = 0;
        return position;
    }

//hasValidMurphy:              ; CODE XREF: update?+5j
    word_510CF = 1;
    word_510C7 = position;
    if (murphyTile->movingObject != 0 || murphyTile->tile != LevelTileTypeMurphy)
    {
        return updateMurphyAnimation(position);
    }

//loc_4DEB4:              ; CODE XREF: update?+1Fj
    byte_510D8 = 0;

    if (gIsGravityEnabled != 0
        && aboveTile->tile != LevelTileTypePortUp
        && aboveTile->tile != LevelTileTypePortVertical
        && aboveTile->tile != LevelTileTypePort4Way
        && (belowTile->movingObject == 0
            && belowTile->tile == LevelTileTypeSpace))
    {
        byte_510D8 = 1;
    }

//loc_4DEE1:              ; CODE XREF: update?+2Ej update?+35j ...
    if (gCurrentUserInput == UserInputNone)
    {
//loc_4DEED:              ; CODE XREF: update?+58j
        byte_510D3 =  1;
        if (byte_510D8 != 0)
        {
            MurphyAnimationDescriptor unknownMurphyData;
//loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
            if (word_510CB != 0)
            {
                unknownMurphyData = someBinaryData_5110E[3]; // dx = 0x0E2E;
            }
            else
            {
//loc_4E396:              ; CODE XREF: update?+4FFj
                unknownMurphyData = someBinaryData_5110E[4]; // dx = 0x0E3E;
            }

//loc_4E399:              ; CODE XREF: update?+504j
            belowTile->movingObject = 3;
            belowTile->tile = LevelTileTypeMurphy;
            murphyTile->movingObject = 3;
            murphyTile->tile = LevelTileTypeSpace;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
        }

//loc_4DEFC:              ; CODE XREF: update?+67j
        ax = word_5195D;
        ax &= 3;
        if (ax != 0)
        {
            return position;
        }

//loc_4DF05:              ; CODE XREF: update?+72j
        word_510CD++;
        if (word_510CD == 4)
        {
            // si = kMurphyStillSpriteCoordinates;
            drawMovingFrame(304, 132, position);

            return position;
        }
//loc_4DF1E:              ; CODE XREF: update?+7Ej
        else if (word_510CD <= 0x01F4)
        {
            return position;
        }
//loc_4DF27:              ; CODE XREF: update?+94j
        else if (word_510CD <= 0x020A)
        {
            // Yawning animation
            uint16_t currentFrame = word_510CD - 0x1F4;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DF4A:              ; CODE XREF: update?+9Dj
        else if (word_510CD <= 0x03E8)
        {
            return position;
        }
//loc_4DF53:              ; CODE XREF: update?+C0j
        else if (word_510CD <= 0x03FE)
        {
            // Yawning animation
            uint16_t currentFrame = word_510CD - 0x3E8;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DF76:              ; CODE XREF: update?+C9j
        else if (word_510CD <= 0x0640)
        {
            return position;
        }
//loc_4DF7F:              ; CODE XREF: update?+ECj
        else if (word_510CD <= 0x0656)
        {
            // Yawning animation
            uint16_t currentFrame = word_510CD - 0x640;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DFA2:              ; CODE XREF: update?+F5j
        else if (word_510CD > 0x0676)
        {
            return position;
        }
        else if (leftTile->movingObject != 0 || leftTile->tile != LevelTileTypeSpace)
        {
//loc_4DFBF:              ; CODE XREF: update?+11Fj
            // Sleep to left animation
            uint16_t currentFrame = word_510CD - 0x656;
            currentFrame = currentFrame >> 4;

            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[35];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
        else if (rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeSpace)
        {
//loc_4DFE0:              ; CODE XREF: update?+126j
            // Sleep to right animation
            uint16_t currentFrame = word_510CD - 0x656;
            currentFrame = currentFrame >> 4;

            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[36];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
        else
        {
            word_510CD = 0x24; // 36
            return position;
        }
    }

//loc_4E001:              ; CODE XREF: update?+5Aj
    // 01ED:739E
    if (byte_510D8 != 0
        && (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace))
    {
        if (gCurrentUserInput != UserInputUp
            || (aboveTile->movingObject != 0 || aboveTile->tile != LevelTileTypeBase))
        {
//loc_4E01B:              ; CODE XREF: update?+182j
            if (gCurrentUserInput != UserInputLeft
                || (leftTile->movingObject != 0 || leftTile->tile != LevelTileTypeBase))
            {
//loc_4E027:              ; CODE XREF: update?+18Ej
                if (gCurrentUserInput != UserInputRight
                    || (rightTile->movingObject != 0 || rightTile->tile != LevelTileTypeBase))
                {
//loc_4E033:              ; CODE XREF: update?+19Aj
                    gCurrentUserInput = UserInputDown;
                }
            }
        }
    }

//loc_4E035:              ; CODE XREF: update?+176j update?+17Dj ...
    // 01ED:73D2
    if (gCurrentUserInput == UserInputUp)
    {
        byte_510D3 = 0;
        return handleMurphyDirectionUp(position);
    }
//loc_4E041:              ; CODE XREF: update?+1A8j
    else if (gCurrentUserInput == UserInputLeft)
    {
        byte_510D3 = 0;
        return handleMurphyDirectionLeft(position);
    }
//loc_4E04E:              ; CODE XREF: update?+1B4j
    else if (gCurrentUserInput == UserInputDown)
    {
        byte_510D3 = 0;
        return handleMurphyDirectionDown(position);
    }
//loc_4E05B:              ; CODE XREF: update?+1C1j
    else if (gCurrentUserInput == UserInputRight)
    {
        byte_510D3 = 0;
        return handleMurphyDirectionRight(position);
    }
//loc_4E068:              ; CODE XREF: update?+1CEj
    else if (gCurrentUserInput == UserInputSpaceUp)
    {
        byte_510D3 = 0;
//loc_4E260:              ; CODE XREF: update?+1E2j
    //    mov ax, leveldata[si-78h]
        if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeBase)
        {
//loc_4E4BD:              ; CODE XREF: update?+3D9j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            sound9();
//            dx = 0x0ECE;
            murphyTile->movingObject = 0x10;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[13]);
        }
//loc_4E26C:              ; CODE XREF: update?+3D7j
        else if (aboveTile->tile == LevelTileTypeBug)
        {
//loc_4E4AC:              ; CODE XREF: update?+3E0j
            if (aboveTile->movingObject < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E4B7:              ; CODE XREF: update?+621j
            aboveTile->movingObject = 0;
            aboveTile->tile = LevelTileTypeBase;

//loc_4E4BD:              ; CODE XREF: update?+3D9j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            sound9();
//            dx = 0x0ECE;
            murphyTile->movingObject = 0x10;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[13]);
        }
//loc_4E273:              ; CODE XREF: update?+3DEj
        else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeInfotron)
        {
//loc_4E5F4:              ; CODE XREF: update?+3E8j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            sound5();
//            dx = 0x0F6E;
            murphyTile->movingObject = 0x14;
            aboveTile->movingObject = 0xFF;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[23]);
        }
//loc_4E27B:              ; CODE XREF: update?+3E6j
        else if (aboveTile->tile == LevelTileTypeTerminal)
        {
//loc_4E712:              ; CODE XREF: update?+249j update?+3EFj
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            if (byte_5196B != 0)
            {
                word_510CD = 0xA;
                return position;
            }

//loc_4E72D:              ; CODE XREF: update?+894j
        //    push    si
        //    mov di, [si+60DDh]
//            si = kTerminalOnSpriteCoordinates;
            drawMovingFrame(256, 388, position - kLevelWidth);
        //    pop si
            detonateYellowDisks();
            return position;
        }
//loc_4E282:              ; CODE XREF: update?+3EDj
        else if (aboveTile->tile == LevelTileTypeRedDisk)
        {
//loc_4E8B6:              ; CODE XREF: update?+3F6j
//            dx = 0x106E;
            murphyTile->movingObject = 0x20;
            aboveTile->movingObject = 3;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[39]);
        }
        else
        {
            return position;
        }
    }
//loc_4E075:              ; CODE XREF: update?+1DBj
    else if (gCurrentUserInput == UserInputSpaceLeft)
    {
        byte_510D3 = 0;
//loc_4E28A:              ; CODE XREF: update?+1EFj
        word_510CB = 1;
    //    mov ax, [si+1832h]
        if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeBase)
        {
//loc_4E4E9:              ; CODE XREF: update?+409j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            sound9();
//            dx = 0x0EDE;
            murphyTile->movingObject = 0x11;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[14]);
        }
//loc_4E29C:              ; CODE XREF: update?+407j
        else if (leftTile->tile == LevelTileTypeBug)
        {
//loc_4E4D8:              ; CODE XREF: update?+410j
            if (leftTile->movingObject < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E4E3:              ; CODE XREF: update?+64Dj
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeBase;

//loc_4E4E9:              ; CODE XREF: update?+409j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            sound9();
//            dx = 0x0EDE;
            murphyTile->movingObject = 0x11;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[14]);
        }
//loc_4E2A3:              ; CODE XREF: update?+40Ej
        else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeInfotron)
        {
//loc_4E614:              ; CODE XREF: update?+418j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            sound5();
//            dx = 0x0F7E;
            murphyTile->movingObject = 0x15;
            leftTile->movingObject = 0xFF;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[24]);
        }
//loc_4E2AB:              ; CODE XREF: update?+416j
        else if (leftTile->tile == LevelTileTypeTerminal)
        {
//loc_4E73C:              ; CODE XREF: update?+2B9j update?+41Fj
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            if (byte_5196B != 0)
            {
                word_510CD = 0xA;
                return position;
            }

//loc_4E757:              ; CODE XREF: update?+8BEj
        //    push    si
        //    mov di, [si+6153h]
//            si = kTerminalOnSpriteCoordinates;
            drawMovingFrame(256, 388, position - 1);
        //    pop si
            detonateYellowDisks();
            return position;
        }
//loc_4E2B2:              ; CODE XREF: update?+41Dj
        else if (leftTile->tile == LevelTileTypeRedDisk)
        {
//loc_4E8C5:              ; CODE XREF: update?+426j
//            dx = 0x107E;
            murphyTile->movingObject = 0x21;
            leftTile->movingObject = 3;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[40]);
        }
        else
        {
            return position;
        }
    }
//loc_4E082:              ; CODE XREF: update?+1E8j
    else if (gCurrentUserInput == UserInputSpaceDown)
    {
        byte_510D3 = 0;
//loc_4E2BA:              ; CODE XREF: update?+1FCj
    //    mov ax, [si+18ACh]
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeBase)
        {
//loc_4E515:              ; CODE XREF: update?+433j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            sound9();
//            dx = 0x0EEE;
            murphyTile->movingObject = 0x12;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[15]);
        }
//loc_4E2C6:              ; CODE XREF: update?+431j
        else if (belowTile->tile == LevelTileTypeBug)
        {
//loc_4E504:              ; CODE XREF: update?+43Aj
            if (belowTile->movingObject < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E50F:              ; CODE XREF: update?+679j
            belowTile->movingObject = 0;
            belowTile->tile = LevelTileTypeBase;

//loc_4E515:              ; CODE XREF: update?+433j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            sound9();
//            dx = 0x0EEE;
            murphyTile->movingObject = 0x12;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[15]);
        }
//loc_4E2CD:              ; CODE XREF: update?+438j
        else if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeInfotron)
        {
//loc_4E634:              ; CODE XREF: update?+442j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            sound5();
//            dx = 0x0F8E;
            murphyTile->movingObject = 0x16;
            belowTile->movingObject = 0xFF;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[25]);
        }
//loc_4E2D5:              ; CODE XREF: update?+440j
        else if (belowTile->tile == LevelTileTypeTerminal)
        {
//loc_4E766:              ; CODE XREF: update?+325j update?+449j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            if (byte_5196B != 0)
            {
                word_510CD = 0xA;
                return position;
            }

//loc_4E781:              ; CODE XREF: update?+8E8j
        //    push    si
        //    mov di, [si+61CDh]
//            si = kTerminalOnSpriteCoordinates;
            drawMovingFrame(256, 388, position + kLevelWidth);
        //    pop si
            detonateYellowDisks();
            return position;
        }
//loc_4E2DC:              ; CODE XREF: update?+447j
        else if (belowTile->tile == LevelTileTypeRedDisk)
        {
//loc_4E8D4:              ; CODE XREF: update?+450j
//            dx = 0x108E;
            murphyTile->movingObject = 0x22;
            belowTile->movingObject = 3;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[41]);
        }
        else
        {
            return position;
        }
    }
//loc_4E08F:              ; CODE XREF: update?+1F5j
    else if (gCurrentUserInput == UserInputSpaceRight)
    {
        byte_510D3 = 0;
//loc_4E2E4:              ; CODE XREF: update?+209j
        word_510CB = 0;
    //    mov ax, [si+1836h]
        if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeBase)
        {
//loc_4E541:              ; CODE XREF: update?+463j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            sound9();
//            dx = 0x0EFE;
            murphyTile->movingObject = 0x13;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[16]);
        }
//loc_4E2F6:              ; CODE XREF: update?+461j
        else if (rightTile->tile == LevelTileTypeBug)
        {
//loc_4E530:              ; CODE XREF: update?+46Aj
            if (rightTile->movingObject < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E53B:              ; CODE XREF: update?+6A5j
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeBase;

//loc_4E541:              ; CODE XREF: update?+463j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            sound9();
//            dx = 0x0EFE;
            murphyTile->movingObject = 0x13;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[16]);
        }
//loc_4E2FD:              ; CODE XREF: update?+468j
        else if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeInfotron)
        {
//loc_4E654:              ; CODE XREF: update?+472j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            sound5();
//            dx = 0x0F9E;
            murphyTile->movingObject = 0x17;
            rightTile->movingObject = 0xFF;
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[26]);
        }
//loc_4E305:              ; CODE XREF: update?+470j
        else if (rightTile->tile != LevelTileTypeTerminal)
        {
//loc_4E30C:              ; CODE XREF: update?+477j
            if (rightTile->tile != LevelTileTypeRedDisk)
            {
                return position;
            }
//loc_4E8E3:              ; CODE XREF: update?+480j
//            dx = 0x109E;
            murphyTile->movingObject = 0x23;
            rightTile->movingObject = 3;

//loc_4E8F0:              ; CODE XREF: update?+4DAj update?+4F7j ...
            word_510EE = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, someBinaryData_5110E[42]);
        }
        else
        {
//loc_4E790:              ; CODE XREF: update?+395j update?+479j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            if (byte_5196B != 0)
            {
                word_510CD = 0xA;
                return position;
            }

//loc_4E7AB:              ; CODE XREF: update?+912j
        //    push    si
        //    mov di, [si+6157h]
//            si = kTerminalOnSpriteCoordinates;
            drawMovingFrame(256, 388, position + 1);
        //    pop si

            detonateYellowDisks();
            return position;
        }
    }
//loc_4E09C:              ; CODE XREF: update?+202j
    else if (gCurrentUserInput == UserInputSpaceOnly)
    {
//loc_4E314:              ; CODE XREF: update?+211j
        if (gNumberOfRemainingRedDisks == 0
            || gPlantedRedDiskCountdown != 0
            || byte_510D3 != 1)
        {
            return position;
        }
        murphyTile->movingObject = 0x2A;
        word_510EE = 0x40; // 64
//        dx = 0x110E;
        gPlantedRedDiskCountdown = 1;
        gPlantedRedDiskPosition = position;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[49]);
    }
    else
    {
//loc_4E0A4:              ; CODE XREF: update?+20Fj
        byte_510D3 = 0;
        return position;
    }
}

uint16_t handleMurphyDirectionUp(uint16_t position)
{
    // 01ED:7447
    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *aboveAboveTile = &gCurrentLevelWord[position - kLevelWidth * 2];

//loc_4E0AA:              ; CODE XREF: update?+1AFj update?+279j
    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E344:              ; CODE XREF: update?+223j
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[0]; // dx = 0x0DFE;
        }
        else
        {
//loc_4E350:              ; CODE XREF: update?+4B9j
            unknownMurphyData = someBinaryData_5110E[1]; // dx = 0x0E0E;
        }

//loc_4E353:              ; CODE XREF: update?+4BEj
        aboveTile->movingObject = 1;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;

        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, unknownMurphyData);
    }
//loc_4E0B6:              ; CODE XREF: update?+221j
    else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeBase)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E3E1:              ; CODE XREF: update?+22Bj
        sound9();
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[7]; // dx = 0x0E6E;
        }
        else
        {
//loc_4E3F0:              ; CODE XREF: update?+559j
            unknownMurphyData = someBinaryData_5110E[8]; // dx = 0x0E7E;
        }

//loc_4E3F3:              ; CODE XREF: update?+55Ej

        aboveTile->movingObject = 5;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, unknownMurphyData);
    }
//loc_4E0BE:              ; CODE XREF: update?+229j
    else if (aboveTile->tile == LevelTileTypeBug)
    {
//loc_4E3D0:              ; CODE XREF: update?+232j
        //    cmp byte ptr [si+17BDh], 0
        //    jl  short loc_4E3DB
        if (aboveTile->movingObject < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E3DB:              ; CODE XREF: update?+545j
        aboveTile->movingObject = 0;
        aboveTile->tile = LevelTileTypeBase;

//loc_4E3E1:              ; CODE XREF: update?+22Bj
        MurphyAnimationDescriptor animationDescriptor;
        sound9();
        if (word_510CB != 0)
        {
            animationDescriptor = someBinaryData_5110E[7]; // dx = 0x0E6E;
        }
        else
        {
//loc_4E3F0:              ; CODE XREF: update?+559j
            animationDescriptor = someBinaryData_5110E[8]; // dx = 0x0E7E;
        }

//loc_4E3F3:              ; CODE XREF: update?+55Ej

        aboveTile->movingObject = 5;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, animationDescriptor);
    }
//loc_4E0C5:              ; CODE XREF: update?+230j
    else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeInfotron)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E55C:              ; CODE XREF: update?+23Aj
        sound5();
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[17]; // dx = 0x0F0E;
        }
        else
        {
//loc_4E56B:              ; CODE XREF: update?+6D4j
            unknownMurphyData = someBinaryData_5110E[18]; // dx = 0x0F1E;
        }

//loc_4E56E:              ; CODE XREF: update?+6D9j
        aboveTile->movingObject = 9;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, unknownMurphyData);
    }
//loc_4E0CD:              ; CODE XREF: update?+238j
    else if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        sound10();
    //    push    si
        byte_5A19B = 1;
        byte_510BB = 1;
        gShouldShowFailedLevelResultScreen = 0;
        if (byte_5A2F9 == 0
            && byte_510B3 != 0)
        {
            byte_5A323 = 1;
            sub_4A95F();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->movingObject = 0xD;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[6]);
    }
//loc_4E0D5:              ; CODE XREF: update?+240j
    else if (aboveTile->tile == LevelTileTypeTerminal)
    {
//loc_4E712:              ; CODE XREF: update?+249j update?+3EFj
        // 01ED:7AAF
    //    push    si
    //    mov di, [si+6155h]
//        si = word_51840;
        drawMovingFrame(160, 64, position);
    //    pop si
        if (byte_5196B != 0)
        {
            word_510CD = 0xA;
            return position;
        }

//loc_4E72D:              ; CODE XREF: update?+894j
    //    push    si
    //    mov di, [si+60DDh]
//        si = kTerminalOnSpriteCoordinates;
        drawMovingFrame(256, 388, position - kLevelWidth);
    //    pop si
        detonateYellowDisks();
        return position;
    }
//loc_4E0DC:              ; CODE XREF: update?+247j
    else if (aboveTile->tile == LevelTileTypePortUp
             || aboveTile->tile == LevelTileTypePortVertical
             || aboveTile->tile == LevelTileTypePort4Way)
    {
//loc_4E7DE:              ; CODE XREF: update?+250j update?+257j ...
        if (aboveAboveTile->movingObject != 0 || aboveAboveTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E7E6:              ; CODE XREF: update?+953j
//        dx = 0x0FCE;
        murphyTile->movingObject = 0x18;
        aboveAboveTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[29]);
    }
//loc_4E0F1:              ; CODE XREF: update?+25Cj
    else if (aboveTile->tile == LevelTileTypeRedDisk)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E847:              ; CODE XREF: update?+265j
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[33]; // dx = 0x100E;
        }
        else
        {
//loc_4E853:              ; CODE XREF: update?+9BCj
            unknownMurphyData = someBinaryData_5110E[34]; // dx = 0x101E;
        }

//loc_4E856:              ; CODE XREF: update?+9C1j
        murphyTile->movingObject = 0x1C;
        aboveTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, unknownMurphyData);
    }
//loc_4E0F8:              ; CODE XREF: update?+263j
    else if (aboveTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E8F9:              ; CODE XREF: update?+26Cj
        if (aboveAboveTile->movingObject != 0 || aboveAboveTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E903:              ; CODE XREF: update?+A70j
        aboveAboveTile->movingObject = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10AE;
        murphyTile->movingObject = 0x24;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[43]);
    }
//loc_4E0FF:              ; CODE XREF: update?+26Aj
    else if (checkMurphyMovementToPosition(position - kLevelWidth) != 1)
    {
        return handleMurphyDirectionUp(position);
    }
    else
    {
        return position;
    }
}

uint16_t handleMurphyDirectionLeft(uint16_t position)
{
    // 01ED:74A9
    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *leftLeftTile = &gCurrentLevelWord[position - 2];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];

//loc_4E10C:              ; CODE XREF: update?+1BBj update?+2F3j
    word_510CB = 1;
//    mov ax, [si+1832h]
    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
//loc_4E36D:              ; CODE XREF: update?+28Bj
//        dx = 0x0E1E;
        leftTile->movingObject = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, someBinaryData_5110E[2]);
    }
//loc_4E11E:              ; CODE XREF: update?+289j
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeBase) // 01ED:7634
    {
//loc_4E41E:              ; CODE XREF: update?+293j
        // 01ED:77BB
        sound9();
//        dx = 0x0E8E;
        leftTile->movingObject = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, someBinaryData_5110E[9]);
    }
//loc_4E126:              ; CODE XREF: update?+291j
    else if (leftTile->tile == LevelTileTypeBug) // 01ED:763B
    {
//loc_4E40D:              ; CODE XREF: update?+29Aj
        if (leftTile->movingObject < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E418:              ; CODE XREF: update?+582j
        // 01ED:77B5
        leftTile->movingObject = 0;
        leftTile->tile = LevelTileTypeBase;

//loc_4E41E:              ; CODE XREF: update?+293j
        sound9();
//        dx = 0x0E8E;
        leftTile->movingObject = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, someBinaryData_5110E[9]);
    }
//loc_4E12D:              ; CODE XREF: update?+298j
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeInfotron)
    {
//loc_4E588:              ; CODE XREF: update?+2A2j
        sound5();
//        dx = 0x0F2E;
        leftTile->movingObject = 10;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, someBinaryData_5110E[19]);
    }
//loc_4E135:              ; CODE XREF: update?+2A0j
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        sound10();
    //    push    si
        byte_5A19B = 1;
        byte_510BB = 1;
        gShouldShowFailedLevelResultScreen = 0;
        if (byte_5A2F9 == 0
            && byte_510B3 != 0)
        {
            byte_5A323 = 1;
            sub_4A95F();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->movingObject = 0xD;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[6]);
    }
//loc_4E13D:              ; CODE XREF: update?+2A8j
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeZonk)
    {
//loc_4E6BA:              ; CODE XREF: update?+2B2j
    //    mov ax, [si+1830h]
        if (leftLeftTile->movingObject != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6C4:              ; CODE XREF: update?+831j
        leftLeftTile->movingObject = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x0FAE;
        murphyTile->movingObject = 0xE;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[27]);
    }
//loc_4E145:              ; CODE XREF: update?+2B0j
    else if (leftTile->tile == LevelTileTypeTerminal)
    {
//loc_4E73C:              ; CODE XREF: update?+2B9j update?+41Fj
    //    push    si
    //    mov di, [si+6155h]
//        si = word_51842;
        drawMovingFrame(208, 16, position);
    //    pop si
        if (byte_5196B != 0)
        {
            word_510CD = 0xA;
            return position;
        }

//loc_4E757:              ; CODE XREF: update?+8BEj
    //    push    si
    //    mov di, [si+6153h]
//        si = kTerminalOnSpriteCoordinates;
        drawMovingFrame(256, 388, position - 1);
    //    pop si
        detonateYellowDisks();
        return position;
    }
//loc_4E14C:              ; CODE XREF: update?+2B7j
    else if (leftTile->tile == LevelTileTypePortLeft
        || leftTile->tile == LevelTileTypePortHorizontal
        || leftTile->tile == LevelTileTypePort4Way)
    {
//loc_4E7F5:              ; CODE XREF: update?+2C0j update?+2C7j ...
        if (leftLeftTile->movingObject != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E7FD:              ; CODE XREF: update?+96Aj
//        dx = 0x0FDE;
        murphyTile->movingObject = 0x19;
        leftLeftTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[30]);
    }
//loc_4E161:              ; CODE XREF: update?+2CCj
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeRedDisk)
    {
//loc_4E863:              ; CODE XREF: update?+2D6j
//        dx = 0x102E;
        leftTile->movingObject = 0x1D;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, someBinaryData_5110E[35]);
    }
//loc_4E169:              ; CODE XREF: update?+2D4j
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E920:              ; CODE XREF: update?+2DEj
        if (leftLeftTile->movingObject != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E92A:              ; CODE XREF: update?+A97j
        leftLeftTile->movingObject = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x10BE;
        murphyTile->movingObject = 0x25;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[44]);
    }
//loc_4E171:              ; CODE XREF: update?+2DCj
    else if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeOrangeDisk)
    {
//loc_4E993:              ; CODE XREF: update?+2E6j
        if (leftLeftTile->movingObject != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E99D:              ; CODE XREF: update?+B0Aj
        leftLeftTile->movingObject = 8;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x10EE;
        murphyTile->movingObject = 0x28;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[47]);
    }
//loc_4E179:              ; CODE XREF: update?+2E4j
    else if (checkMurphyMovementToPosition(position - 1) != 1)
    {
        return handleMurphyDirectionLeft(position);
    }
    else
    {
        return position;
    }
}

uint16_t handleMurphyDirectionDown(uint16_t position)
{
    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *belowBelowTile = &gCurrentLevelWord[position + kLevelWidth * 2];

//loc_4E186:              ; CODE XREF: update?+1C8j update?+355j
//    mov ax, leveldata[si+78h]
    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[3]; // dx = 0x0E2E;
        }
        else
        {
//loc_4E396:              ; CODE XREF: update?+4FFj
            unknownMurphyData = someBinaryData_5110E[4]; // dx = 0x0E3E;
        }

//loc_4E399:              ; CODE XREF: update?+504j
        belowTile->movingObject = 3;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E192:              ; CODE XREF: update?+2FDj
    else if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeBase)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E44F:              ; CODE XREF: update?+307j
        sound9();
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[10]; // dx = 0x0E9E;
        }
        else
        {
//loc_4E45E:              ; CODE XREF: update?+5C7j
            unknownMurphyData = someBinaryData_5110E[11]; // dx = 0x0EAE;
        }

//loc_4E461:              ; CODE XREF: update?+5CCj
        belowTile->movingObject = 7;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E19A:              ; CODE XREF: update?+305j
    else if (belowTile->tile == LevelTileTypeBug)
    {
//loc_4E43E:              ; CODE XREF: update?+30Ej
        if (belowTile->movingObject < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E449:              ; CODE XREF: update?+5B3j
        belowTile->movingObject = 0;
        belowTile->tile = LevelTileTypeBase;

        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E44F:              ; CODE XREF: update?+307j
        sound9();
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[10]; // dx = 0x0E9E;
        }
        else
        {
//loc_4E45E:              ; CODE XREF: update?+5C7j
            unknownMurphyData = someBinaryData_5110E[11]; // dx = 0x0EAE;
        }

//loc_4E461:              ; CODE XREF: update?+5CCj
        belowTile->movingObject = 7;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E1A1:              ; CODE XREF: update?+30Cj
    else if (belowTile->tile == LevelTileTypeInfotron)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E5A8:              ; CODE XREF: update?+316j
        sound5();
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[20]; // dx = 0x0F3E;
        }
        else
        {
//loc_4E5B7:              ; CODE XREF: update?+720j
            unknownMurphyData = someBinaryData_5110E[21]; // dx = 0x0F4E;
        }

//loc_4E5BA:              ; CODE XREF: update?+725j
        belowTile->movingObject = 11;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeBase;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E1A9:              ; CODE XREF: update?+314j
    else if (belowTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        sound10();
    //    push    si
        byte_5A19B = 1;
        byte_510BB = 1;
        gShouldShowFailedLevelResultScreen = 0;
        if (byte_5A2F9 == 0
            && byte_510B3 != 0)
        {
            byte_5A323 = 1;
            sub_4A95F();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->movingObject = 0xD;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[6]);
    }
//loc_4E1B1:              ; CODE XREF: update?+31Cj
    else if (belowTile->tile == LevelTileTypeTerminal)
    {
//loc_4E766:              ; CODE XREF: update?+325j update?+449j
    //    push    si
    //    mov di, [si+6155h]
//        si = word_51844;
        drawMovingFrame(176, 64, position);
    //    pop si
        if (byte_5196B != 0)
        {
            word_510CD = 0xA;
            return position;
        }

//loc_4E781:              ; CODE XREF: update?+8E8j
    //    push    si
    //    mov di, [si+61CDh]
//        si = kTerminalOnSpriteCoordinates;
        drawMovingFrame(256, 388, position + kLevelWidth);
    //    pop si
        detonateYellowDisks();
        return position;
    }
//loc_4E1B8:              ; CODE XREF: update?+323j
    else if (belowTile->tile == LevelTileTypePortDown
             || belowTile->tile == LevelTileTypePortVertical
             || belowTile->tile == LevelTileTypePort4Way)
    {
//loc_4E80C:              ; CODE XREF: update?+32Cj update?+333j ...
        if (belowBelowTile->movingObject != 0 || belowBelowTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E814:              ; CODE XREF: update?+981j
//        dx = 0x0FEE;
        murphyTile->movingObject = 0x1A;
        belowBelowTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[31]);
    }
//loc_4E1CD:              ; CODE XREF: update?+338j
    else if (belowTile->tile == LevelTileTypeRedDisk)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E87F:              ; CODE XREF: update?+341j
        if (word_510CB != 0)
        {
            unknownMurphyData = someBinaryData_5110E[36]; // dx = 0x103E;
        }
        else
        {
//loc_4E88B:              ; CODE XREF: update?+9F4j
            unknownMurphyData = someBinaryData_5110E[37]; // dx = 0x104E;
        }

//loc_4E88E:              ; CODE XREF: update?+9F9j
        murphyTile->movingObject = 0x1E;
        belowTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, unknownMurphyData);
    }
//loc_4E1D4:              ; CODE XREF: update?+33Fj
    else if (belowTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E947:              ; CODE XREF: update?+348j
        if (belowBelowTile->movingObject != 0 || belowBelowTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E951:              ; CODE XREF: update?+ABEj
        belowBelowTile->movingObject = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10CE;
        murphyTile->movingObject = 0x27;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[45]);
    }
//loc_4E1DB:              ; CODE XREF: update?+346j
    else if (checkMurphyMovementToPosition(position + kLevelWidth) != 1)
    {
        return handleMurphyDirectionDown(position);
    }
    else
    {
        return position;
    }
}

uint16_t handleMurphyDirectionRight(uint16_t position)
{
    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *rightRightTile = &gCurrentLevelWord[position + 2];
    MovingLevelTile *belowRightTile = &gCurrentLevelWord[position + kLevelWidth + 1];

//loc_4E1E8:              ; CODE XREF: update?+1D5j update?+3CDj
    word_510CB = 0;
//    mov ax, leveldata[si+2]
    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
//loc_4E3B3:              ; CODE XREF: update?+367j
//        dx = 0x0E4E;
        rightTile->movingObject = 4;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, someBinaryData_5110E[5]);
    }
//loc_4E1FA:              ; CODE XREF: update?+365j
    else if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeBase)
    {
//loc_4E48C:              ; CODE XREF: update?+36Fj
        sound9();
//        dx = 0x0EBE;
        rightTile->movingObject = 8;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, someBinaryData_5110E[12]);
    }
//loc_4E202:              ; CODE XREF: update?+36Dj
    else if (rightTile->tile == LevelTileTypeBug)
    {
//loc_4E47B:              ; CODE XREF: update?+376j
        if (rightTile->movingObject < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E486:              ; CODE XREF: update?+5F0j
        rightTile->movingObject = 0;
        rightTile->tile = LevelTileTypeBase;

//loc_4E48C:              ; CODE XREF: update?+36Fj
        sound9();
//        dx = 0x0EBE;
        rightTile->movingObject = 8;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, someBinaryData_5110E[12]);
    }
//loc_4E209:              ; CODE XREF: update?+374j
    else if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeInfotron)
    {
//loc_4E5D4:              ; CODE XREF: update?+37Ej
        sound5();
//        dx = 0x0F5E;
        rightTile->movingObject = 12;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, someBinaryData_5110E[22]);
    }
//loc_4E211:              ; CODE XREF: update?+37Cj
    else if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        sound10();
    //    push    si
        byte_5A19B = 1;
        byte_510BB = 1;
        gShouldShowFailedLevelResultScreen = 0;
        if (byte_5A2F9 == 0
            && byte_510B3 != 0)
        {
            byte_5A323 = 1;
            sub_4A95F();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->movingObject = 0xD;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[6]);
    }
//loc_4E219:              ; CODE XREF: update?+384j
    else if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeZonk)
    {
//loc_4E6E1:              ; CODE XREF: update?+38Ej
    //    mov ax, [si+1838h]
        if (rightRightTile->movingObject != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6EB:              ; CODE XREF: update?+858j
    //    mov ax, [si+18AEh]
        if (belowRightTile->movingObject == 0 && belowRightTile->tile == LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6F5:              ; CODE XREF: update?+862j
        rightRightTile->movingObject = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x0FBE;
        murphyTile->movingObject = 0xF;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[28]);
    }
//loc_4E221:              ; CODE XREF: update?+38Cj
    else if (rightTile->tile == LevelTileTypeTerminal)
    {
//loc_4E790:              ; CODE XREF: update?+395j update?+479j
    //    push    si
    //    mov di, [si+6155h]
//        si = word_51846;
        drawMovingFrame(192, 16, position);
    //    pop si
        if (byte_5196B != 0)
        {
            word_510CD = 0xA;
            return position;
        }

//loc_4E7AB:              ; CODE XREF: update?+912j
    //    push    si
    //    mov di, [si+6157h]
//        si = kTerminalOnSpriteCoordinates;
        drawMovingFrame(256, 388, position + 1);
    //    pop si

        detonateYellowDisks();
        return position;
    }
//loc_4E228:              ; CODE XREF: update?+393j
    else if (rightTile->tile == LevelTileTypePortRight
        || rightTile->tile == LevelTileTypePortHorizontal
        || rightTile->tile == LevelTileTypePort4Way)
    {
//loc_4E823:              ; CODE XREF: update?+39Cj update?+3A3j ...
        if (rightRightTile->movingObject != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E82B:              ; CODE XREF: update?+998j
//        dx = 0x0FFE;
        murphyTile->movingObject = 0x1B;
        rightRightTile->movingObject = 3;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[32]);
    }
//loc_4E23D:              ; CODE XREF: update?+3A8j
    else if (rightTile->tile == LevelTileTypeRedDisk)
    {
//loc_4E89A:              ; CODE XREF: update?+3B1j
//        dx = 0x105E;
        rightTile->movingObject = 0x1F;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->movingObject = 3;
        murphyTile->tile = LevelTileTypeSpace;
        word_510EE = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, someBinaryData_5110E[38]);
    }
//loc_4E244:              ; CODE XREF: update?+3AFj
    else if (rightTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E96D:              ; CODE XREF: update?+3B8j
        if (rightRightTile->movingObject != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E977:              ; CODE XREF: update?+AE4j
        rightRightTile->movingObject = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10DE;
        murphyTile->movingObject = 0x26;
        word_510EE = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, someBinaryData_5110E[46]);
    }
//loc_4E24B:              ; CODE XREF: update?+3B6j
    else if (rightTile->tile == LevelTileTypeOrangeDisk)
    {
//loc_4E9B9:              ; CODE XREF: update?+3C0j
        if (rightRightTile->movingObject != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E9C3:              ; CODE XREF: update?+B30j
        if (belowRightTile->movingObject == 0 && belowRightTile->tile == LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E9CD:              ; CODE XREF: update?+B3Aj
        rightRightTile->movingObject = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10FE;
        murphyTile->movingObject = 0x29;

//loc_4E9E7:              ; CODE XREF: update?+84Ej update?+87Fj ...
        word_510EE = 8;

//loc_4E9ED:              ; CODE XREF: update?+A66j
        gIsMurphyGoingThroughPortal = 0;

        return updateMurphyAnimationInfo(position, someBinaryData_5110E[48]);
    }
//loc_4E253:              ; CODE XREF: update?+3BEj
    else if (checkMurphyMovementToPosition(position + 1) != 1)
    {
        return handleMurphyDirectionRight(position);
    }
    else
    {
        return position;
    }
}

uint16_t updateMurphyAnimationInfo(uint16_t position, MurphyAnimationDescriptor unknownMurphyData)
{
    // 01ED:7D9F

//loc_4E9F3:              ; CODE XREF: update?+4B0j update?+9B4j
//    di = 0x0DE0;
//    memcpy(di, si, 7 * 2); // rep movsw

    gCurrentMurphyAnimation = unknownMurphyData;

    return updateMurphyAnimation(position);
}

uint16_t updateMurphyAnimation(uint16_t position)
{
    // 01ED:7DA4

    MovingLevelTile *murphyTile = &gCurrentLevelWord[position];
    MovingLevelTile *leftLeftTile = &gCurrentLevelWord[position - 2];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *rightRightTile = &gCurrentLevelWord[position + 2];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *belowBelowTile = &gCurrentLevelWord[position + kLevelWidth * 2];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *aboveAboveTile = &gCurrentLevelWord[position - kLevelWidth * 2];
    MovingLevelTile *belowRightRightTile = &gCurrentLevelWord[position + kLevelWidth + 2];

//loc_4EA07:              ; CODE XREF: update?+21j
    word_510CD = 0;

    if (word_510EE == 0)
    {
        // 01ED:7E08
        uint8_t currentFrame = gCurrentMurphyAnimation.currentFrame;
        AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[gCurrentMurphyAnimation.animationIndex];
        Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

//loc_4EA6B:              ; CODE XREF: update?+B83j
    //    push    si
    //    push(di);
        gMurphyPositionX += gCurrentMurphyAnimation.speedX;
        gMurphyPositionY += gCurrentMurphyAnimation.speedY;
    //    mov di, [si+6155h]
//        di += gSomeUnknownMurphyData.word_510F0; // destination offset
//        si = gSomeUnknownMurphyData.word_510F8; // coordinates in MOVING.DAT
//        ax = si;
        si += 2; // advance a frame (2 bytes = 1 word in the list of frames)
//        gSomeUnknownMurphyData.word_510F8 = si;
        gCurrentMurphyAnimation.currentFrame++;
        si = ax;
        bx = gCurrentMurphyAnimation.width * 8;
        dx = gCurrentMurphyAnimation.height;
    //    push    ds
    //    mov ax, es
    //    mov ds, ax

//        uint16_t srcX = (si % kMovingBitmapWidth);
//        uint16_t srcY = (si / kMovingBitmapWidth);
        uint16_t dstX = (position % kLevelWidth) * kTileSize;
        uint16_t dstY = (position / kLevelWidth) * kTileSize;

        int16_t offsetX = (gCurrentMurphyAnimation.word_510F0 % 122) * 8;
        int16_t offsetY = (gCurrentMurphyAnimation.word_510F0 / 122);

//loc_4EA9F:              ; CODE XREF: update?+C28j
        drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                     frameCoordinates.y,
                                     gCurrentMurphyAnimation.width * 8,
                                     gCurrentMurphyAnimation.height,
                                     dstX + offsetX,
                                     dstY + offsetY);

        if (gIsMurphyGoingThroughPortal != 0)
        {
            // This +1 is because the "opposite" portal animation is always the next one
            AnimationFrameCoordinates animationFrameCoordinates = frameCoordinates_5142E[gCurrentMurphyAnimation.animationIndex + 1];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            int16_t offsetX = (gCurrentMurphyAnimation.word_510F2 % 122) * 8;
            int16_t offsetY = (gCurrentMurphyAnimation.word_510F2 / 122);

            drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                         frameCoordinates.y,
                                         gCurrentMurphyAnimation.width * 8,
                                         gCurrentMurphyAnimation.height,
                                         dstX + offsetX,
                                         dstY + offsetY);
        }
        else
        {
//loc_4EAFA:              ; CODE XREF: update?+C32j
            gCurrentMurphyAnimation.word_510F0 += gCurrentMurphyAnimation.word_510F2;
        }

//loc_4EB04:              ; CODE XREF: update?+C68j
        if (gCurrentMurphyAnimation.currentFrame < animationFrameCoordinates.numberOfCoordinates)
        {
              return position;
        }

//loc_4EB10:              ; CODE XREF: update?+C7Bj
        // 01ED:7EAD
        gMurphyTileX += gCurrentMurphyAnimation.speedX / 2;
        gMurphyTileY += gCurrentMurphyAnimation.speedY / 2;
        uint8_t previousMurphyMovingObject = murphyTile->movingObject;
        murphyTile->movingObject = 0;
        if (previousMurphyMovingObject == 1)
        {
//loc_4EC93:              ; CODE XREF: update?+CA3j update?+CC3j
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + kLevelWidth);
            return position;
        }
//loc_4EB36:              ; CODE XREF: update?+CA1j
        else if (previousMurphyMovingObject == 2)
        {
//loc_4ECB1:              ; CODE XREF: update?+CABj update?+CCBj
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + 1);
            return position;
        }
//loc_4EB3E:              ; CODE XREF: update?+CA9j
        else if (previousMurphyMovingObject == 3)
        {
//loc_4ECCF:              ; CODE XREF: update?+CB3j update?+CD3j
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB46:              ; CODE XREF: update?+CB1j
        else if (previousMurphyMovingObject == 4)
        {
//loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
            handleMurphyCollisionAfterMovement(position - 1);
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB4E:              ; CODE XREF: update?+CB9j
        else if (previousMurphyMovingObject == 5)
        {
//loc_4EC93:              ; CODE XREF: update?+CA3j update?+CC3j
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + kLevelWidth);
            return position;
        }
//loc_4EB56:              ; CODE XREF: update?+CC1j
        else if (previousMurphyMovingObject == 6)
        {
//loc_4ECB1:              ; CODE XREF: update?+CABj update?+CCBj
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + 1);
            return position;
        }
//loc_4EB5E:              ; CODE XREF: update?+CC9j
        else if (previousMurphyMovingObject == 7)
        {
//loc_4ECCF:              ; CODE XREF: update?+CB3j update?+CD3j
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB66:              ; CODE XREF: update?+CD1j
        else if (previousMurphyMovingObject == 8)
        {
//loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
            // 01ED:82F0
            handleMurphyCollisionAfterMovement(position - 1);
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB6E:              ; CODE XREF: update?+CD9j
        else if (previousMurphyMovingObject == 9)
        {
//loc_4EC85:              ; CODE XREF: update?+CE3j
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EC90:              ; CODE XREF: update?+DFAj
            drawNumberOfRemainingInfotrons();

//loc_4EC93:              ; CODE XREF: update?+CA3j update?+CC3j
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + kLevelWidth);
            return position;
        }
//loc_4EB76:              ; CODE XREF: update?+CE1j
        else if (previousMurphyMovingObject == 10)
        {
//loc_4ECA3:              ; CODE XREF: update?+CEBj
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4ECAE:              ; CODE XREF: update?+E18j
            drawNumberOfRemainingInfotrons();

//loc_4ECB1:              ; CODE XREF: update?+CABj update?+CCBj
            murphyTile->tile = LevelTileTypeMurphy;
            handleMurphyCollisionAfterMovement(position + 1);
            return position;
        }
//loc_4EB7E:              ; CODE XREF: update?+CE9j
        else if (previousMurphyMovingObject == 11)
        {
//loc_4ECC1:              ; CODE XREF: update?+CF3j
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4ECCC:              ; CODE XREF: update?+E36j
            drawNumberOfRemainingInfotrons();

//loc_4ECCF:              ; CODE XREF: update?+CB3j update?+CD3j
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB86:              ; CODE XREF: update?+CF1j
        else if (previousMurphyMovingObject == 12)
        {
//loc_4EF45:              ; CODE XREF: update?+CFBj
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EF50:              ; CODE XREF: update?+10BAj
            drawNumberOfRemainingInfotrons();

//loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
            handleMurphyCollisionAfterMovement(position - 1);
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB8E:              ; CODE XREF: update?+CF9j
        else if (previousMurphyMovingObject == 14)
        {
//loc_4ECE3:              ; CODE XREF: update?+D03j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4ECF0:              ; CODE XREF: update?+E58j
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeZonk;
            handleZonkPushedByMurphy(position - 2);
            return position - 1;
        }
//loc_4EB96:              ; CODE XREF: update?+D01j
        else if (previousMurphyMovingObject == 15)
        {
//loc_4ED06:              ; CODE XREF: update?+D0Bj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4ED13:              ; CODE XREF: update?+E7Bj
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeZonk;
            handleZonkPushedByMurphy(position + 2);
            return position + 1;
        }
//loc_4EB9E:              ; CODE XREF: update?+D09j
        else if (previousMurphyMovingObject == 16)
        {
//loc_4EF71:              ; CODE XREF: update?+D13j
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBA6:              ; CODE XREF: update?+D11j
        else if (previousMurphyMovingObject == 17)
        {
//loc_4EF8D:              ; CODE XREF: update?+D1Bj
            if (leftTile->tile != LevelTileTypeExplosion)
            {
                leftTile->movingObject = 0;
                leftTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBAE:              ; CODE XREF: update?+D19j
        else if (previousMurphyMovingObject == 19)
        {
//loc_4EFC5:              ; CODE XREF: update?+D23j
            if (rightTile->tile != LevelTileTypeExplosion)
            {
                rightTile->movingObject = 0;
                rightTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBB6:              ; CODE XREF: update?+D21j
        else if (previousMurphyMovingObject == 18)
        {
//loc_4EFA9:              ; CODE XREF: update?+D2Bj
            if (belowTile->tile != LevelTileTypeExplosion)
            {
                belowTile->movingObject = 0;
                belowTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBBE:              ; CODE XREF: update?+D29j
        else if (previousMurphyMovingObject == 20)
        {
//loc_4EF63:              ; CODE XREF: update?+D33j
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EF6E:              ; CODE XREF: update?+10D8j
            drawNumberOfRemainingInfotrons();

//loc_4EF71:              ; CODE XREF: update?+D13j
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBC6:              ; CODE XREF: update?+D31j
        else if (previousMurphyMovingObject == 21)
        {
//loc_4EF7F:              ; CODE XREF: update?+D3Bj
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EF8A:              ; CODE XREF: update?+10F4j
            drawNumberOfRemainingInfotrons();

//loc_4EF8D:              ; CODE XREF: update?+D1Bj
            if (leftTile->tile != LevelTileTypeExplosion)
            {
                leftTile->movingObject = 0;
                leftTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBCE:              ; CODE XREF: update?+D39j
        else if (previousMurphyMovingObject == 23)
        {
//loc_4EFB7:              ; CODE XREF: update?+D43j
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EFC2:              ; CODE XREF: update?+112Cj
            drawNumberOfRemainingInfotrons();

//loc_4EFC5:              ; CODE XREF: update?+D23j
            if (rightTile->tile != LevelTileTypeExplosion)
            {
                rightTile->movingObject = 0;
                rightTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBD6:              ; CODE XREF: update?+D41j
        else if (previousMurphyMovingObject == 22)
        {
//loc_4EF9B:              ; CODE XREF: update?+D4Bj
            if (gNumberOfRemainingInfotrons > 0)
            {
                gNumberOfRemainingInfotrons--;
            }

//loc_4EFA6:              ; CODE XREF: update?+1110j
            drawNumberOfRemainingInfotrons();

//loc_4EFA9:              ; CODE XREF: update?+D2Bj
            if (belowTile->tile != LevelTileTypeExplosion)
            {
                belowTile->movingObject = 0;
                belowTile->tile = LevelTileTypeSpace;
            }

            return position;
        }
//loc_4EBDE:              ; CODE XREF: update?+D49j
        else if (previousMurphyMovingObject == 13)
        {
//loc_4ED42:              ; CODE XREF: update?+D53j
            gShouldExitLevel = 1;
            return position;
        }
//loc_4EBE6:              ; CODE XREF: update?+D51j
        else if (previousMurphyMovingObject == 24)
        {
//loc_4EFD3:              ; CODE XREF: update?+D5Bj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4EFE0:              ; CODE XREF: update?+1148j
            aboveAboveTile->movingObject = 0;
            aboveAboveTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position -= kLevelWidth * 2;
            if (aboveTile->movingObject == 1)
            {
                updateSpecialPort(position + kLevelWidth);
            }

            return position;
        }
//loc_4EBEE:              ; CODE XREF: update?+D59j
        else if (previousMurphyMovingObject == 25)
        {
//loc_4F001:              ; CODE XREF: update?+D63j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F00E:              ; CODE XREF: update?+1176j
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position -= 2;
            if (leftTile->movingObject == 1)
            {
                updateSpecialPort(position + 1);
            }

            return position;
        }
//loc_4EBF6:              ; CODE XREF: update?+D61j
        else if (previousMurphyMovingObject == 26)
        {
//loc_4F02E:              ; CODE XREF: update?+D6Bj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F03B:              ; CODE XREF: update?+11A3j
            belowBelowTile->movingObject = 0;
            belowBelowTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position += kLevelWidth * 2;
            if (belowTile->movingObject == 1)
            {
                updateSpecialPort(position - kLevelWidth);
            }

            return position;
        }
//loc_4EBFE:              ; CODE XREF: update?+D69j
        else if (previousMurphyMovingObject == 27)
        {
//loc_4F05C:              ; CODE XREF: update?+D73j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F069:              ; CODE XREF: update?+11D1j
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position += 2;
            if (rightTile->movingObject == 1)
            {
                updateSpecialPort(position - 1);
            }

            return position;
        }
//loc_4EC06:              ; CODE XREF: update?+D71j
        else if (previousMurphyMovingObject == 28)
        {
//loc_4F089:              ; CODE XREF: update?+D7Bj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F096:              ; CODE XREF: update?+11FEj
            position -= kLevelWidth;

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            aboveTile->movingObject = 0;
            aboveTile->tile = LevelTileTypeMurphy;
            decreaseRemainingRedDisksIfNeeded(position);
            return position;
        }
//loc_4EC0E:              ; CODE XREF: update?+D79j
        else if (previousMurphyMovingObject == 29)
        {
//loc_4F09C:              ; CODE XREF: update?+D83j
            if (rightTile->tile != LevelTileTypeExplosion)
            {
                rightTile->movingObject = 0;
                rightTile->tile = LevelTileTypeSpace;
            }

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            decreaseRemainingRedDisksIfNeeded(position);
            return position;
        }
//loc_4EC16:              ; CODE XREF: update?+D81j
        else if (previousMurphyMovingObject == 30)
        {
//loc_4F0AC:              ; CODE XREF: update?+D8Bj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F0B9:              ; CODE XREF: update?+1221j
            position += kLevelWidth;

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            belowTile->movingObject = 0;
            belowTile->tile = LevelTileTypeMurphy;
            decreaseRemainingRedDisksIfNeeded(position);
            return position;
        }
//loc_4EC1E:              ; CODE XREF: update?+D89j
        else if (previousMurphyMovingObject == 31)
        {
//loc_4F0BF:              ; CODE XREF: update?+D93j
            if (leftTile->tile != LevelTileTypeExplosion)
            {
                leftTile->movingObject = 0;
                leftTile->tile = LevelTileTypeSpace;
            }

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            decreaseRemainingRedDisksIfNeeded(position);
            return position;
        }
//loc_4EC26:              ; CODE XREF: update?+D91j
        else if (previousMurphyMovingObject == 32)
        {
//loc_4F0CF:              ; CODE XREF: update?+D9Bj
            if (aboveTile->tile != LevelTileTypeExplosion)
            {
                aboveTile->movingObject = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4F0DC:              ; CODE XREF: update?+1244j
            decreaseRemainingRedDisksIfNeeded(position - kLevelWidth);
            return position;
        }
//loc_4EC2E:              ; CODE XREF: update?+D99j
        else if (previousMurphyMovingObject == 33)
        {
//loc_4F0E6:              ; CODE XREF: update?+DA3j
            if (leftTile->tile != LevelTileTypeExplosion)
            {
                leftTile->movingObject = 0;
                leftTile->tile = LevelTileTypeSpace;
            }

//loc_4F0F3:              ; CODE XREF: update?+125Bj
            decreaseRemainingRedDisksIfNeeded(position - 1);
            return position;
        }
//loc_4EC36:              ; CODE XREF: update?+DA1j
        else if (previousMurphyMovingObject == 34)
        {
//loc_4F0FD:              ; CODE XREF: update?+DABj
            if (belowTile->tile != LevelTileTypeExplosion)
            {
                belowTile->movingObject = 0;
                belowTile->tile = LevelTileTypeSpace;
            }

//loc_4F10A:              ; CODE XREF: update?+1272j
            decreaseRemainingRedDisksIfNeeded(position + kLevelWidth);
            return position;
        }
//loc_4EC3E:              ; CODE XREF: update?+DA9j
        else if (previousMurphyMovingObject == 35)
        {
//loc_4F114:              ; CODE XREF: update?+DB3j
            if (rightTile->tile != LevelTileTypeExplosion)
            {
                rightTile->movingObject = 0;
                rightTile->tile = LevelTileTypeSpace;
            }

//loc_4F121:              ; CODE XREF: update?+1289j
            decreaseRemainingRedDisksIfNeeded(position + 1);
            return position;
        }
//loc_4EC46:              ; CODE XREF: update?+DB1j
        else if (previousMurphyMovingObject == 36)
        {
//loc_4F12B:              ; CODE XREF: update?+DBBj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F138:              ; CODE XREF: update?+12A0j
            aboveTile->movingObject = 0;
            aboveTile->tile = LevelTileTypeMurphy;
            aboveAboveTile->movingObject = 0;
            aboveAboveTile->tile = LevelTileTypeYellowDisk;
            return position - kLevelWidth;
        }
//loc_4EC4E:              ; CODE XREF: update?+DB9j
        else if (previousMurphyMovingObject == 37)
        {
//loc_4F148:              ; CODE XREF: update?+DC3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F155:              ; CODE XREF: update?+12BDj
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeYellowDisk;
            return position - 1;
        }
//loc_4EC56:              ; CODE XREF: update?+DC1j
        else if (previousMurphyMovingObject == 39)
        {
//loc_4F165:              ; CODE XREF: update?+DCBj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F172:              ; CODE XREF: update?+12DAj
            belowTile->movingObject = 0;
            belowTile->tile = LevelTileTypeMurphy;
            belowBelowTile->movingObject = 0;
            belowBelowTile->tile = LevelTileTypeYellowDisk;
            return position + kLevelWidth;
        }
//loc_4EC5E:              ; CODE XREF: update?+DC9j
        else if (previousMurphyMovingObject == 38)
        {
//loc_4F182:              ; CODE XREF: update?+DD3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F18F:              ; CODE XREF: update?+12F7j
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeYellowDisk;
            return position + 1;
        }
//loc_4EC66:              ; CODE XREF: update?+DD1j
        else if (previousMurphyMovingObject == 40)
        {
//loc_4F19F:              ; CODE XREF: update?+DDBj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F1AC:              ; CODE XREF: update?+1314j
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeOrangeDisk;
            return position - 1;
        }
//loc_4EC6E:              ; CODE XREF: update?+DD9j
        else if (previousMurphyMovingObject == 41)
        {
//loc_4F1BC:              ; CODE XREF: update?+DE3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->movingObject = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F1C9:              ; CODE XREF: update?+1331j
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeOrangeDisk;
            if (belowRightRightTile->movingObject == 0 && belowRightRightTile->tile == LevelTileTypeSpace)
            {
                rightRightTile->movingObject = 0x20;
                belowRightRightTile->movingObject = 8;
            }

//loc_4F1E6:              ; CODE XREF: update?+134Aj
            return position + 1;
        }
//loc_4EC76:              ; CODE XREF: update?+DE1j
        else if (previousMurphyMovingObject == 42)
        {
//loc_4F1EA:              ; CODE XREF: update?+DEBj
            murphyTile->movingObject = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            gPlantedRedDiskCountdown = 2;
            gNumberOfRemainingRedDisks--;
            drawNumberOfRemainingRedDisks();
            sound6();
            return position;
        }
        else
        {
//loc_4EC7E:              ; CODE XREF: update?+DE9j
            gShouldExitLevel = 1;
            return position;
        }
    }

    word_510EE--;
    if (word_510EE == 0)
    {
        sound6();
    }

//loc_4EA1E:              ; CODE XREF: update?+B89j
    if (murphyTile->movingObject == 0xE)
    {
//loc_4ED49:              ; CODE XREF: update?+B97j
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeZonk))
        {
            return position;
        }

//loc_4ED5A:              ; CODE XREF: update?+EC0j update?+EC7j
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->movingObject = 0;
        leftTile->tile = LevelTileTypeZonk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4ED73:              ; CODE XREF: update?+EDBj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA2A:              ; CODE XREF: update?+B95j
    else if (murphyTile->movingObject == 0xF)
    {
//loc_4ED81:              ; CODE XREF: update?+B9Fj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeZonk))
        {
            return position;
        }

//loc_4ED92:              ; CODE XREF: update?+EF8j update?+EFFj
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->movingObject = 0;
        rightTile->tile = LevelTileTypeZonk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EDAB:              ; CODE XREF: update?+F13j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA32:              ; CODE XREF: update?+B9Dj
    else if (murphyTile->movingObject == 0x28)
    {
//loc_4EDB9:              ; CODE XREF: update?+BA7j
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeOrangeDisk))
        {
            return position;
        }

//loc_4EDCA:              ; CODE XREF: update?+F30j update?+F37j
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->movingObject = 0;
        leftTile->tile = LevelTileTypeOrangeDisk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4EDE3:              ; CODE XREF: update?+F4Bj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA3A:              ; CODE XREF: update?+BA5j
    else if (murphyTile->movingObject == 0x29)
    {
//loc_4EDF1:              ; CODE XREF: update?+BAFj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeOrangeDisk))
        {
            return position;
        }

//loc_4EE02:              ; CODE XREF: update?+F68j update?+F6Fj
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->movingObject = 0;
        rightTile->tile = LevelTileTypeOrangeDisk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EE1B:              ; CODE XREF: update?+F83j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA42:              ; CODE XREF: update?+BADj
    else if (murphyTile->movingObject == 0x24)
    {
//loc_4EE29:              ; CODE XREF: update?+BB7j
        if (gCurrentUserInput == UserInputUp
            && (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EE3A:              ; CODE XREF: update?+FA0j update?+FA7j
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        aboveTile->movingObject = 0;
        aboveTile->tile = LevelTileTypeYellowDisk;
        if (aboveAboveTile->tile != LevelTileTypeExplosion)
        {
            aboveAboveTile->movingObject = 0;
            aboveAboveTile->tile = LevelTileTypeSpace;
        }

//loc_4EE53:              ; CODE XREF: update?+FBBj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA4A:              ; CODE XREF: update?+BB5j
    else if (murphyTile->movingObject == 0x25)
    {
//loc_4EE61:              ; CODE XREF: update?+BBFj
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EE72:              ; CODE XREF: update?+FD8j update?+FDFj
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->movingObject = 0;
        leftTile->tile = LevelTileTypeYellowDisk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->movingObject = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4EE8B:              ; CODE XREF: update?+FF3j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA52:              ; CODE XREF: update?+BBDj
    else if (murphyTile->movingObject == 0x27)
    {
//loc_4EE99:              ; CODE XREF: update?+BC7j
        if (gCurrentUserInput == UserInputDown
            && (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EEAA:              ; CODE XREF: update?+1010j
//                ; update?+1017j
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        belowTile->movingObject = 0;
        belowTile->tile = LevelTileTypeYellowDisk;
        if (belowBelowTile->tile != LevelTileTypeExplosion)
        {
            belowBelowTile->movingObject = 0;
            belowBelowTile->tile = LevelTileTypeSpace;
        }

//loc_4EEC3:              ; CODE XREF: update?+102Bj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA5A:              ; CODE XREF: update?+BC5j
    else if (murphyTile->movingObject == 0x26)
    {
//loc_4EED1:              ; CODE XREF: update?+BCFj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EEE2:              ; CODE XREF: update?+1048j
//                ; update?+104Fj
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->movingObject = 0;
        rightTile->tile = LevelTileTypeYellowDisk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->movingObject = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EEFB:              ; CODE XREF: update?+1063j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA62:              ; CODE XREF: update?+BCDj
    else if (murphyTile->movingObject == 0x2A)
    {
//loc_4EF09:              ; CODE XREF: update?+BD7j
        if (gCurrentUserInput == UserInputSpaceOnly)
        {
            if (word_510EE > 0x20)
            {
                return position;
            }

            // si = word_51790;
            drawMovingFrame(288, 132, position);
            gPlantedRedDiskCountdown = 1;

            return position;
        }

//loc_4EF2C:              ; CODE XREF: update?+1080j
        murphyTile->movingObject = 0;
        murphyTile->tile = LevelTileTypeMurphy;

        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);
        gPlantedRedDiskCountdown = 0;

        return position;
    }
    else
    {
        return position;
    }
}

void detonateYellowDisks()
{
//loc_4E7B8:              ; CODE XREF: update?+8AAj update?+8D4j ...
    byte_5196A = 7;
    byte_5196B = 1;

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4E7C9:              ; CODE XREF: update?+94Aj
        MovingLevelTile *tile = &gCurrentLevelWord[i];
        if (tile->movingObject == 0 && tile->tile == LevelTileTypeYellowDisk)
        {
            detonateBigExplosion(i);
        }
    }
}

void handleZonkPushedByMurphy(uint16_t position) // sub_4ED29   proc near       ; CODE XREF: update?+E6Fp update?+E92p
{
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];

    if (belowTile->tile == LevelTileTypeSnikSnak
        || belowTile->tile == 0xBB)
    {
//loc_4ED38:              ; CODE XREF: handleZonkPushedByMurphy+5j handleZonkPushedByMurphy+Cj
        handleZonkStateAfterFallingOneTile(position + kLevelWidth);
    }
}

// srcX and srcY are the coordinates of the frame to draw in MOVING.DAT
void drawMovingFrame(uint16_t srcX, uint16_t srcY, uint16_t destPosition) // sub_4F200   proc near       ; CODE XREF: sub_4A291+26p
                   // ; updatePlantedRedDisk+2Ap ...
{
    // 01ED:859D
    // Draws Murphy idle (from MOVING.DAT) on the screen
    // Parameters:
    // - di: coordinates on the screen
    // - si: coordinates on the MOVING.DAT bitmap to draw from?

    uint16_t destX = (destPosition % kLevelWidth) * kTileSize;
    uint16_t destY = (destPosition / kLevelWidth) * kTileSize;

    for (int y = 0; y < kTileSize; ++y)
    {
        for (int x = 0; x < kTileSize; ++x)
        {
//loc_4F208:              ; CODE XREF: drawMovingFrame+1Bj
            size_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX + x;
            size_t dstAddress = (destY + y - kLevelEdgeSize) * kLevelBitmapWidth + destX + x - kLevelEdgeSize;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }
}

uint8_t checkMurphyMovementToPosition(uint16_t position) // sub_4F21F   proc near       ; CODE XREF: update?+273p update?+2EDp ...
{
    // 01ED:85BC
    // Parameters:
    // - si: position
    // - ax: value of that position (movingObject + tile)
    // - bl: gCurrentUserInput
    MovingLevelTile *tile = &gCurrentLevelWord[position];

    if ((tile->movingObject == 0xFF && tile->tile == 0xFF)
        || (tile->movingObject == 0xAA && tile->tile == 0xAA)
        || (tile->movingObject == 0))
    {
//loc_4F296:              ; CODE XREF: checkMurphyMovementToPosition+3j checkMurphyMovementToPosition+8j ...
        return 1;
    }
    else if (tile->tile == LevelTileTypeZonk)
    {
//loc_4F24F:              ; CODE XREF: checkMurphyMovementToPosition+11j
        if (gCurrentUserInput == UserInputLeft)
        {
//loc_4F25E:              ; CODE XREF: checkMurphyMovementToPosition+33j
            uint8_t movingObjectType = (tile->movingObject & 0xF0);
            if (movingObjectType == 0x20
                || movingObjectType == 0x40
                || movingObjectType == 0x50
                || movingObjectType == 0x70)
            {
//loc_4F278:              ; CODE XREF: checkMurphyMovementToPosition+45j
//                ; checkMurphyMovementToPosition+4Aj ...
                return 1;
            }
            detonateBigExplosion(position);

//loc_4F278:              ; CODE XREF: checkMurphyMovementToPosition+45j
//                ; checkMurphyMovementToPosition+4Aj ...
            return 1;
        }
        else if (gCurrentUserInput != UserInputRight)
        {
            detonateBigExplosion(position);
            return 1;
        }

//loc_4F27A:              ; CODE XREF: checkMurphyMovementToPosition+38j
        uint8_t movingObjectType = (tile->movingObject & 0xF0);
        if (movingObjectType == 0x30
            || movingObjectType == 0x40
            || movingObjectType == 0x60
            || movingObjectType == 0x70)
        {
//loc_4F294:              ; CODE XREF: checkMurphyMovementToPosition+61j
//                ; checkMurphyMovementToPosition+66j ...
            return 1;
        }
        detonateBigExplosion(position);

//loc_4F294:              ; CODE XREF: checkMurphyMovementToPosition+61j
//                ; checkMurphyMovementToPosition+66j ...
        return 1;
    }
    else if (tile->tile == LevelTileTypeExplosion)
    {
//loc_4F298:              ; CODE XREF: checkMurphyMovementToPosition+15j
        // 01ED:8635
        if ((tile->movingObject & 0x80) != 0
            || tile->movingObject < 4)
        {
//loc_4F2A2:              ; CODE XREF: checkMurphyMovementToPosition+7Cj
            detonateBigExplosion(position);
            return 1;
        }
        else
        {
//loc_4F2A7:              ; CODE XREF: checkMurphyMovementToPosition+81j
            tile->movingObject = 0;
            tile->tile = LevelTileTypeSpace;
            return 0;
        }
    }
    else if (tile->tile == LevelTileTypeOrangeDisk
             || tile->tile == LevelTileTypePortRight
             || tile->tile == LevelTileTypePortDown
             || tile->tile == LevelTileTypePortLeft
             || tile->tile == LevelTileTypePortUp)
    {
//loc_4F296:              ; CODE XREF: checkMurphyMovementToPosition+3j checkMurphyMovementToPosition+8j ...
        return 1;
    }
    else
    {
        detonateBigExplosion(position);
        return 1;
    }
}

void updateSpecialPort(uint16_t position) // sub_4F2AF   proc near       ; CODE XREF: update?+116Ap
                   // ; update?+1197p ...
{
    // 01ED:864C
    if (gCurrentLevel.numberOfSpecialPorts == 0)
    {
        return;
    }

    uint8_t isPortInPosition = 0;
    uint8_t portIndex = 0;

    for (uint8_t i = 0; i < gCurrentLevel.numberOfSpecialPorts; ++i)
    {
//loc_4F2BD:              ; CODE XREF: updateSpecialPort+19j
        SpecialPortInfo portInfo = gCurrentLevel.specialPortsInfo[i];
        // For _reasons_ the port position has its bytes inverted (first high, then low), so we must reverse them
        uint16_t portPosition = SDL_Swap16(portInfo.position);
        portPosition /= 2; // We must divide by 2 because the level format works with words

        if (portPosition == position)
        {
            isPortInPosition = 1;
            portIndex = i;
            break;
        }
    }

    if (isPortInPosition == 0)
    {
        return;
    }

//loc_4F2CB:              ; CODE XREF: updateSpecialPort+14j
    SpecialPortInfo portInfo = gCurrentLevel.specialPortsInfo[portIndex];
    gIsGravityEnabled = portInfo.gravity;
    gAreZonksFrozen = portInfo.freezeZonks;
    gAreEnemiesFrozen = portInfo.freezeEnemies;
    // TODO: I still don't know where word_510AC is read :fearful:
    // I tried with a breakpoint on memory read and it was never accessed :shrug:
    word_510AC = word_510AC ^ gRandomGeneratorSeed;
}
/*
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +
*/

void updateSnikSnakTiles(uint16_t position) // movefun4  proc near       ; DATA XREF: data:162Co
{
    // 01ED:868D
    if (gAreEnemiesFrozen == 1)
    {
        return;
    }

    MovingLevelTile *currentTile = &gCurrentLevelWord[position];

    if (currentTile->tile != LevelTileTypeSnikSnak)
    {
        return;
    }

    uint8_t frame = currentTile->movingObject;

    FrameBasedMovingFunction function = kSnikSnakMovingFunctions[frame];

    if (function != NULL)
    {
        // 01ED:86AC
        function(position, frame);
    }
}

void updateSnikSnakTurnLeft(uint16_t position, uint8_t frame) // sub_4F312   proc near       ; DATA XREF: data:movingFunctions3o
{
    // 01ED:86AF
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t value = word_5195D & 3;

    if (value == 0)
    {
//loc_4F320:              ; CODE XREF: updateSnikSnakTurnLeft+6j
        Point frameCoordinates = frameCoordinates_51654[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        currentTile->movingObject = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F362:              ; CODE XREF: updateSnikSnakTurnLeft+Bj
    uint8_t movingObject = currentTile->movingObject;
    uint8_t nextMovingObject = 0;

    if (movingObject == 0)
    {
//loc_4F37B:              ; CODE XREF: updateSnikSnakTurnLeft+57j
        if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F38E:              ; CODE XREF: updateSnikSnakTurnLeft+6Ej
            currentTile->movingObject = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->movingObject = 0x10;
            aboveTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = aboveTile->movingObject;
        if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 2)
    {
//loc_4F39E:              ; CODE XREF: updateSnikSnakTurnLeft+5Cj
        if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F3B1:              ; CODE XREF: updateSnikSnakTurnLeft+91j
            currentTile->movingObject = 0x2;
            currentTile->tile = 0xBB;
            leftTile->movingObject = 0x18;
            leftTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = leftTile->movingObject;
        if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 4)
    {
//loc_4F3C1:              ; CODE XREF: updateSnikSnakTurnLeft+61j
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F3D7:              ; CODE XREF: updateSnikSnakTurnLeft+B4j
            currentTile->movingObject = 0x3;
            currentTile->tile = 0xBB;
            belowTile->movingObject = 0x20;
            belowTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = belowTile->movingObject;
        if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 6)
    {
//loc_4F3E7:              ; CODE XREF: updateSnikSnakTurnLeft+66j
        if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F3FD:              ; CODE XREF: updateSnikSnakTurnLeft+DAj
            currentTile->movingObject = 0x4;
            currentTile->tile = 0xBB;
            rightTile->movingObject = 0x28;
            rightTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = rightTile->movingObject;
        if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else
    {
        return;
    }

//loc_4F34A:              ; CODE XREF: updateSnikSnakTurnLeft+79j
//                ; updateSnikSnakTurnLeft+9Cj ...
    if (nextMovingObject != 0x1B
        && nextMovingObject != 0x19
        && nextMovingObject != 0x18
        && nextMovingObject != 0x1A)
    {
        detonateBigExplosion(position);
    }
}

void updateSnikSnakTurnRight(uint16_t position, uint8_t frame) // sub_4F40D   proc near       ; DATA XREF: data:155Ao
{
    // 01ED:87AA
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];

    uint16_t value = word_5195D & 3;

    if (value == 0)
    {
//loc_4F41B:              ; CODE XREF: updateSnikSnakTurnRight+6j
        Point frameCoordinates = frameCoordinates_51654[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        frame |= 8;
        currentTile->movingObject = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F45F:              ; CODE XREF: updateSnikSnakTurnRight+Bj
    uint8_t movingObject = currentTile->movingObject;
    uint8_t nextMovingObject = 0;
    if (movingObject == 8)
    {
//loc_4F478:              ; CODE XREF: updateSnikSnakTurnRight+59j
        if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F48B:              ; CODE XREF: updateSnikSnakTurnRight+70j
            currentTile->movingObject = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->movingObject = 0x10;
            aboveTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = aboveTile->movingObject;
        if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xA)
    {
//loc_4F4E4:              ; CODE XREF: updateSnikSnakTurnRight+5Ej
        if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F4FA:              ; CODE XREF: updateSnikSnakTurnLeft+DAj
            currentTile->movingObject = 0x4;
            currentTile->tile = 0xBB;
            rightTile->movingObject = 0x28;
            rightTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = rightTile->movingObject;
        if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xC)
    {
//loc_4F4BE:              ; CODE XREF: updateSnikSnakTurnRight+63j
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F4D4:              ; CODE XREF: updateSnikSnakTurnRight+B6j
            currentTile->movingObject = 0x3;
            currentTile->tile = 0xBB;
            belowTile->movingObject = 0x20;
            belowTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = belowTile->movingObject;
        if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xE)
    {
//loc_4F49B:              ; CODE XREF: updateSnikSnakTurnRight+68j
        if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F4AE:              ; CODE XREF: updateSnikSnakTurnRight+93j
            currentTile->movingObject = 0x2;
            currentTile->tile = 0xBB;
            leftTile->movingObject = 0x18;
            leftTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = leftTile->movingObject;
        if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else
    {
        return;
    }

//loc_4F447:              ; CODE XREF: updateSnikSnakTurnRight+7Bj
//                ; updateSnikSnakTurnRight+9Ej ...
    if (nextMovingObject != 0x1B
        && nextMovingObject != 0x19
        && nextMovingObject != 0x18
        && nextMovingObject != 0x1A)
    {
        detonateBigExplosion(position);
    }
}

void updateSnikSnakMovementUp(uint16_t position, uint8_t frame) // sub_4F50A    proc near       ; DATA XREF: data:156Ao
{
    // 01ED:88A7
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t finalPosition = position + kLevelWidth;
    Point frameCoordinates = frameCoordinates_51654[frame];
    // sub bx, 1Eh
    frame -= 15; // 0x1E / 2
    uint16_t offset = kFallAnimationGravityOffsets[frame];

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize - (offset % 122) * 2;
    uint16_t dstY = tileY * kTileSize - (offset / 122) * 2;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize,
                                 kTileSize + 2,
                                 dstX, dstY);
    if (frame == 7)
    {
        if (belowTile->tile != LevelTileTypeExplosion)
        {
            belowTile->movingObject = 0;
            belowTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F546:              ; CODE XREF: updateSnikSnakMovementUp+2Dj
//                ; updateSnikSnakMovementUp+34j
    if (frame < 8)
    {
        frame += 0x10;
        currentTile->movingObject = frame;
        return;
    }

//loc_4F553:              ; CODE XREF: updateSnikSnakMovementUp+3Fj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 1;
        return;
    }

//loc_4F566:              ; CODE XREF: updateSnikSnakMovementUp+54j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 1;
        return;
    }

//loc_4F573:              ; CODE XREF: updateSnikSnakMovementUp+61j
    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 1;
        currentTile->tile = 0xBB;
        aboveTile->movingObject = 0x10;
        aboveTile->tile = LevelTileTypeSnikSnak;
        return;
    }

//loc_4F58A:              ; CODE XREF: updateSnikSnakMovementUp+6Ej
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4F595:              ; CODE XREF: updateSnikSnakMovementUp+85j
    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8939
        currentTile->movingObject = 9;
        return;
    }

//loc_4F5A2:              ; CODE XREF: updateSnikSnakMovementUp+90j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 9;
        return;
    }

//loc_4F5AF:              ; CODE XREF: updateSnikSnakMovementUp+9Dj
    currentTile->movingObject = 1;
}

void updateSnikSnakMovementLeft(uint16_t position, uint8_t frame) // sub_4F5B5   proc near       ; DATA XREF: data:157Ao
{
    // 01ED:8952
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    Point frameCoordinates = frameCoordinates_51654[frame];

    uint8_t tileX = (position % kLevelWidth);
    uint8_t tileY = (position / kLevelWidth);

    uint16_t dstX = tileX * kTileSize;
    uint16_t dstY = tileY * kTileSize;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize * 2,
                                 kTileSize,
                                 dstX, dstY);

    frame &= 7;
    frame++;
    if (frame == 7)
    {
        if (rightTile->tile != LevelTileTypeExplosion)
        {
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F5EC:              ; CODE XREF: updateSnikSnakMovementLeft+28j
//                ; updateSnikSnakMovementLeft+2Fj
    if (frame < 8)
    {
        frame += 0x18;
        currentTile->movingObject = frame;
        return;
    }

//loc_4F5F9:              ; CODE XREF: updateSnikSnakMovementLeft+3Aj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 3;
        return;
    }

//loc_4F60C:              ; CODE XREF: updateSnikSnakMovementLeft+4Fj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 3;
        return;
    }

//loc_4F619:              ; CODE XREF: updateSnikSnakMovementLeft+5Cj
    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 2;
        currentTile->tile = 0xBB;
        leftTile->movingObject = 0x18;
        leftTile->tile = LevelTileTypeSnikSnak;
        return;
    }

//loc_4F630:              ; CODE XREF: updateSnikSnakMovementLeft+69j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4F63B:              ; CODE XREF: updateSnikSnakMovementLeft+80j
    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 0xF;
        return;
    }

//loc_4F648:              ; CODE XREF: updateSnikSnakMovementLeft+8Bj
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xF;
        return;
    }

//loc_4F655:              ; CODE XREF: updateSnikSnakMovementLeft+98j
    currentTile->movingObject = 3;
}

void updateSnikSnakMovementDown(uint16_t position, uint8_t frame) // sub_4F65B   proc near       ; DATA XREF: data:158Ao
{
    // 01ED:89F8
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t finalPosition = position - kLevelWidth;
    Point frameCoordinates = frameCoordinates_51654[frame];
    //sub bx, 40h ; '@'
    frame -= 0x20; // 0x40 / 2

    uint16_t offset = kFallAnimationGravityOffsets[frame];

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize + (offset % 122) * 2;
    uint16_t dstY = tileY * kTileSize + (offset / 122) * 2;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize,
                                 kTileSize + 2,
                                 dstX, dstY);

    frame++;
    if (frame == 7)
    {
        if (aboveTile->tile != LevelTileTypeExplosion)
        {
            aboveTile->movingObject = 0;
            aboveTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F699:              ; CODE XREF: sub_4F66B+1Fj
//                ; sub_4F66B+26j
    if (frame < 8)
    {
        frame += 0x20;
        currentTile->movingObject = frame;
        return;
    }

//loc_4F6A6:              ; CODE XREF: sub_4F66B+31j
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 5;
        return;
    }

//loc_4F6B9:              ; CODE XREF: sub_4F66B+46j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 5;
        return;
    }

//loc_4F6C6:              ; CODE XREF: sub_4F66B+53j
    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 3;
        currentTile->tile = 0xBB;
        belowTile->movingObject = 0x20;
        belowTile->tile = LevelTileTypeSnikSnak;
        return;
    }

//loc_4F6DD:              ; CODE XREF: sub_4F66B+60j
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4F6E8:              ; CODE XREF: sub_4F66B+77j
    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8A8C
        currentTile->movingObject = 0xD;
        return;
    }

//loc_4F6F5:              ; CODE XREF: sub_4F66B+82j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xD;
        return;
    }

//loc_4F702:              ; CODE XREF: sub_4F66B+8Fj
    currentTile->movingObject = 5;
}

void updateSnikSnakMovementRight(uint16_t position, uint8_t frame) // sub_4F708   proc near       ; DATA XREF: data:159Ao
{
    // 01ED:8AA5
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    Point frameCoordinates = frameCoordinates_51654[frame];

    uint16_t finalPosition = position - 1;

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize;
    uint16_t dstY = tileY * kTileSize;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize * 2,
                                 kTileSize,
                                 dstX, dstY);
    frame &= 7;
    frame++;
    if (frame == 7)
    {
        if (leftTile->tile != LevelTileTypeExplosion)
        {
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F740:              ; CODE XREF: updateSnikSnakMovementRight+29j
//                ; updateSnikSnakMovementRight+30j
    if (frame < 8)
    {
        frame += 0x28;
        currentTile->movingObject = frame;
        return;
    }

//loc_4F74D:              ; CODE XREF: updateSnikSnakMovementRight+3Bj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 7;
        return;
    }

//loc_4F760:              ; CODE XREF: updateSnikSnakMovementRight+50j
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 7;
        return;
    }

//loc_4F76D:              ; CODE XREF: updateSnikSnakMovementRight+5Dj
    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 4;
        currentTile->tile = 0xBB;
        rightTile->movingObject = 0x28;
        rightTile->tile = LevelTileTypeSnikSnak;
        return;
    }

//loc_4F784:              ; CODE XREF: updateSnikSnakMovementRight+6Aj
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4F78F:              ; CODE XREF: updateSnikSnakMovementRight+81j
    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 0xB;
        return;
    }

//loc_4F79C:              ; CODE XREF: updateSnikSnakMovementRight+8Cj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xB;
        return;
    }

//loc_4F7A9:              ; CODE XREF: updateSnikSnakMovementRight+99j
    currentTile->movingObject = 7;
}

void updateElectronTiles(uint16_t position) // movefun6  proc near       ; DATA XREF: data:163Ao
{
    // 01ED:8B4C
    if (gAreEnemiesFrozen == 1)
    {
        return;
    }
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];

    if (currentTile->tile != LevelTileTypeElectron)
    {
        return;
    }

    uint8_t frame = currentTile->movingObject;

    FrameBasedMovingFunction function = kElectronMovingFunctions[frame];

    if (function != NULL)
    {
        // 01ED:8B6B
        function(position, frame);
    }
}

void updateElectronTurnLeft(uint16_t position, uint8_t frame) // sub_4F7D1   proc near       ; DATA XREF: data:movingFunctions2o
{
    // 01ED:8B6E
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t value = word_5195D & 3;

    if (value == 0)
    {
//loc_4F7DF:              ; CODE XREF: updateElectronTurnLeft+6j
        Point frameCoordinates = frameCoordinates_516B4[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        currentTile->movingObject = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F80D:              ; CODE XREF: updateElectronTurnLeft+Bj
    uint8_t movingObject = currentTile->movingObject;
    if (movingObject == 0)
    {
//loc_4F826:              ; CODE XREF: updateElectronTurnLeft+43j
        if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F835:              ; CODE XREF: updateElectronTurnLeft+5Aj
            currentTile->movingObject = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->movingObject = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 2)
    {
//loc_4F845:              ; CODE XREF: updateElectronTurnLeft+48j
        if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F854:              ; CODE XREF: updateElectronTurnLeft+79j
            currentTile->movingObject = 0x2;
            currentTile->tile = 0xBB;
            leftTile->movingObject = 0x18;
            leftTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 4)
    {
//loc_4F864:              ; CODE XREF: updateElectronTurnLeft+4Dj
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F873:              ; CODE XREF: updateElectronTurnLeft+98j
            currentTile->movingObject = 0x3;
            currentTile->tile = 0xBB;
            belowTile->movingObject = 0x20;
            belowTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 6)
    {
//loc_4F883:              ; CODE XREF: updateElectronTurnLeft+52j
        if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F895:              ; CODE XREF: updateElectronTurnLeft+B7j
            currentTile->movingObject = 0x4;
            currentTile->tile = 0xBB;
            rightTile->movingObject = 0x28;
            rightTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else
    {
        return;
    }

//loc_4F809:              ; CODE XREF: updateElectronTurnLeft+61j
//                ; updateElectronTurnLeft+80j ...
    detonateBigExplosion(position);
}

void updateElectronTurnRight(uint16_t position, uint8_t frame) // sub_4F8A5   proc near       ; DATA XREF: data:15BAo
{
    // 01ED:8C42
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t value = word_5195D & 3;

    if (value == 0)
    {
//loc_4F8B3:              ; CODE XREF: updateElectronTurnRight+6j
        Point frameCoordinates = frameCoordinates_516B4[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        frame |= 8;
        currentTile->movingObject = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F8E3:              ; CODE XREF: updateElectronTurnRight+Bj
    uint8_t movingObject = currentTile->movingObject;
    if (movingObject == 8)
    {
//loc_4F8FC:              ; CODE XREF: updateElectronTurnRight+45j
        if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F90B:              ; CODE XREF: updateElectronTurnRight+5Cj
            currentTile->movingObject = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->movingObject = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xA)
    {
//loc_4F959:              ; CODE XREF: updateElectronTurnRight+4Aj
        if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F96B:              ; CODE XREF: updateElectronTurnRight+B9j
            currentTile->movingObject = 0x4;
            currentTile->tile = 0xBB;
            rightTile->movingObject = 0x28;
            rightTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xC)
    {
//loc_4F93A:              ; CODE XREF: updateElectronTurnRight+4Fj
        if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F949:              ; CODE XREF: updateElectronTurnRight+9Aj
            currentTile->movingObject = 0x3;
            currentTile->tile = 0xBB;
            belowTile->movingObject = 0x20;
            belowTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (movingObject == 0xE)
    {
//loc_4F91B:              ; CODE XREF: updateElectronTurnRight+54j
        if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F92A:              ; CODE XREF: updateElectronTurnRight+7Bj
            currentTile->movingObject = 0x2;
            currentTile->tile = 0xBB;
            leftTile->movingObject = 0x18;
            leftTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else
    {
        return;
    }

//loc_4F8DF:              ; CODE XREF: updateElectronTurnRight+63j
//                ; updateElectronTurnRight+82j ...
    detonateBigExplosion(position);
}

void updateElectronMovementUp(uint16_t position, uint8_t frame) // sub_4F97B   proc near       ; DATA XREF: data:15CAo
{
    // 01ED:8D18
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t finalPosition = position + kLevelWidth;
    Point frameCoordinates = frameCoordinates_516B4[frame];
    // sub bx, 1Eh
    frame -= 15; // 0x1E / 2
    uint16_t offset = kFallAnimationGravityOffsets[frame];

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize - (offset % 122) * 2;
    uint16_t dstY = tileY * kTileSize - (offset / 122) * 2;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize,
                                 kTileSize + 2,
                                 dstX, dstY);

    if (frame == 7)
    {
        if (belowTile->tile != LevelTileTypeExplosion)
        {
            belowTile->movingObject = 0;
            belowTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4F9B7:              ; CODE XREF: updateElectronMovementUp+2Dj
//                ; updateElectronMovementUp+34j
    if (frame < 8)
    {
        frame += 0x10;
        currentTile->movingObject = frame;
        return;
    }

//loc_4F9C4:              ; CODE XREF: updateElectronMovementUp+3Fj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 1;
        return;
    }

//loc_4F9D7:              ; CODE XREF: updateElectronMovementUp+54j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 1;
        return;
    }

//loc_4F9E4:              ; CODE XREF: updateElectronMovementUp+61j
    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 1;
        currentTile->tile = 0xBB;
        aboveTile->movingObject = 0x10;
        aboveTile->tile = LevelTileTypeElectron;
        return;
    }

//loc_4F9FB:              ; CODE XREF: updateElectronMovementUp+6Ej
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4FA06:              ; CODE XREF: updateElectronMovementUp+85j
    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 9;
        return;
    }

//loc_4FA13:              ; CODE XREF: updateElectronMovementUp+90j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 9;
        return;
    }

//loc_4FA20:              ; CODE XREF: updateElectronMovementUp+9Dj
    currentTile->movingObject = 1;
}

void updateElectronMovementDown(uint16_t position, uint8_t frame) // sub_4FA26   proc near       ; DATA XREF: data:15DAo
{
    // 01ED:8DC3
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    Point frameCoordinates = frameCoordinates_516B4[frame];

    uint8_t tileX = (position % kLevelWidth);
    uint8_t tileY = (position / kLevelWidth);

    uint16_t dstX = tileX * kTileSize;
    uint16_t dstY = tileY * kTileSize;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize * 2,
                                 kTileSize,
                                 dstX, dstY);

    frame &= 7;
    frame++;
    if (frame == 7)
    {
        if (rightTile->tile != LevelTileTypeExplosion)
        {
            rightTile->movingObject = 0;
            rightTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FA5D:              ; CODE XREF: updateElectronMovementDown+28j
//                ; updateElectronMovementDown+2Fj
    if (frame < 8)
    {
        frame += 0x18;
        currentTile->movingObject = frame;
        return;
    }

//loc_4FA6A:              ; CODE XREF: updateElectronMovementDown+3Aj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 3;
        return;
    }

//loc_4FA7D:              ; CODE XREF: updateElectronMovementDown+4Fj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 3;
        return;
    }

//loc_4FA8A:              ; CODE XREF: updateElectronMovementDown+5Cj
    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 2;
        currentTile->tile = 0xBB;
        leftTile->movingObject = 0x18;
        leftTile->tile = LevelTileTypeElectron;
        return;
    }

//loc_4FAA1:              ; CODE XREF: updateElectronMovementDown+69j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4FAAC:              ; CODE XREF: updateElectronMovementDown+80j
    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 0xF;
        return;
    }

//loc_4FAB9:              ; CODE XREF: updateElectronMovementDown+8Bj
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xF;
        return;
    }

//loc_4FAC6:              ; CODE XREF: updateElectronMovementDown+98j
    currentTile->movingObject = 3;
}

void updateElectronMovementRight(uint16_t position, uint8_t frame) // sub_4FACC   proc near       ; DATA XREF: data:15EAo
{
    // 01ED:8E69
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    uint16_t finalPosition = position - kLevelWidth;
    Point frameCoordinates = frameCoordinates_516B4[frame];
    //sub bx, 40h ; '@'
    frame -= 0x20; // 0x40 / 2

    uint16_t offset = kFallAnimationGravityOffsets[frame];

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize + (offset % 122) * 2;
    uint16_t dstY = tileY * kTileSize + (offset / 122) * 2;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize,
                                 kTileSize + 2,
                                 dstX, dstY);

    frame++;
    if (frame == 7)
    {
        if (aboveTile->tile != LevelTileTypeExplosion)
        {
            aboveTile->movingObject = 0;
            aboveTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FB0A:              ; CODE XREF: updateElectronMovementRight+2Fj
//                ; updateElectronMovementRight+36j
    if (frame < 8)
    {
        frame += 0x20;
        currentTile->movingObject = frame;
        return;
    }

//loc_4FB17:              ; CODE XREF: updateElectronMovementRight+41j
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 5;
        return;
    }

//loc_4FB2A:              ; CODE XREF: updateElectronMovementRight+56j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 5;
        return;
    }

//loc_4FB37:              ; CODE XREF: updateElectronMovementRight+63j
    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 3;
        currentTile->tile = 0xBB;
        belowTile->movingObject = 0x20;
        belowTile->tile = LevelTileTypeElectron;
        return;
    }

//loc_4FB4E:              ; CODE XREF: updateElectronMovementRight+70j
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4FB59:              ; CODE XREF: updateElectronMovementRight+87j
    if (leftTile->movingObject == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8A8C
        currentTile->movingObject = 0xD;
        return;
    }

//loc_4FB66:              ; CODE XREF: updateElectronMovementRight+92j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xD;
        return;
    }

//loc_4FB73:              ; CODE XREF: updateElectronMovementRight+9Fj
    currentTile->movingObject = 5;
}

void updateElectronMovementLeft(uint16_t position, uint8_t frame) // sub_4FB79   proc near       ; DATA XREF: data:15FAo
{
    // 01ED:8F16
    MovingLevelTile *currentTile = &gCurrentLevelWord[position];
    MovingLevelTile *belowTile = &gCurrentLevelWord[position + kLevelWidth];
    MovingLevelTile *leftTile = &gCurrentLevelWord[position - 1];
    MovingLevelTile *aboveTile = &gCurrentLevelWord[position - kLevelWidth];
    MovingLevelTile *rightTile = &gCurrentLevelWord[position + 1];

    Point frameCoordinates = frameCoordinates_516B4[frame];

    uint16_t finalPosition = position - 1;

    uint8_t tileX = (finalPosition % kLevelWidth);
    uint8_t tileY = (finalPosition / kLevelWidth);

    uint16_t dstX = tileX * kTileSize;
    uint16_t dstY = tileY * kTileSize;

    drawMovingSpriteFrameInLevel(frameCoordinates.x, frameCoordinates.y,
                                 kTileSize * 2,
                                 kTileSize,
                                 dstX, dstY);
    frame &= 7;
    frame++;
    if (frame == 7)
    {
        if (leftTile->tile != LevelTileTypeExplosion)
        {
            leftTile->movingObject = 0;
            leftTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FBB1:              ; CODE XREF: updateElectronMovementLeft+29j
//                ; updateElectronMovementLeft+30j
    if (frame < 8)
    {
        frame += 0x28;
        currentTile->movingObject = frame;
        return;
    }

//loc_4FBBE:              ; CODE XREF: updateElectronMovementLeft+3Bj
    currentTile->movingObject = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (aboveTile->movingObject == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 7;
        return;
    }

//loc_4FBD1:              ; CODE XREF: updateElectronMovementLeft+50j
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 7;
        return;
    }

//loc_4FBDE:              ; CODE XREF: updateElectronMovementLeft+5Dj
    if (rightTile->movingObject == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 4;
        currentTile->tile = 0xBB;
        rightTile->movingObject = 0x28;
        rightTile->tile = LevelTileTypeElectron;
        return;
    }

//loc_4FBF5:              ; CODE XREF: updateElectronMovementLeft+6Aj
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        detonateBigExplosion(position);
        return;
    }

//loc_4FC00:              ; CODE XREF: updateElectronMovementLeft+81j
    if (belowTile->movingObject == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->movingObject = 0xB;
        return;
    }

//loc_4FC0D:              ; CODE XREF: updateElectronMovementLeft+8Cj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->movingObject = 0xB;
        return;
    }

//loc_4FC1A:              ; CODE XREF: updateElectronMovementLeft+99j
    currentTile->movingObject = 7;
}

void drawGamePanelText() // sub_4FC20  proc near       ; CODE XREF: somethingspsig:loc_4944Fp
                   // ; drawGamePanel+22p ...
{
    // 01ED:8FBD
    if (gIsRecordingDemo != 0) // Recording demo?
    {
//    mov si, 87D1h // "  DEMO  "
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 72, 3, 8, "  DEMO  ");
//    mov si, 87DAh // "000" -> this address is the ".SP" text
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 16, 14, 8, gCurrentDemoLevelIdentifier);
//        mov si, 87F6h // "--- RECORDING DEMO0 ---"
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 64, 14, 8, "--- RECORDING DEMO0 ---");
    }
//loc_4FC6F:              ; CODE XREF: drawGamePanelText+5j
    else if (gIsPlayingDemo != 0) // Playing demo?
    {
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 72, 3, 8, "  DEMO  ");
//      mov si, 87DAh // "000" -> this address is the ".SP" text
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 16, 14, 8, gCurrentDemoLevelIdentifier);
//      mov si, 87DEh // "----- DEMO LEVEL! -----"
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 64, 14, 8, &gCurrentDemoLevelName[4]);
    }
    else
    {
//loc_4FCD6:              ; CODE XREF: drawGamePanelText+B1j
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 72, 3, 6, gPlayerName);
        char levelNumber[4] = "000";
        memcpy(levelNumber, gCurrentLevelName, 3);
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 16, 14, 8, levelNumber);
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 64, 14, 8, &gCurrentLevelName[4]);
    }

//loc_4FD1A:              ; CODE XREF: drawGamePanelText+4Cj
//                ; drawGamePanelText+A0j ...
    drawNumberOfRemainingInfotrons();
    drawGameTime();
}

void drawNumberOfRemainingInfotrons() // sub_4FD21   proc near       ; CODE XREF: resetNumberOfInfotrons+13p
                   // ; update?:loc_4EC90p ...
{
    if (gNumberOfRemainingInfotrons < 1)
    {
        gNumberOfRemainingInfotrons = 0; // WTF? Can this be negative? In theory not...
    }

//loc_4FD2E:              ; CODE XREF: drawNumberOfRemainingInfotrons+6j
    char number[4] = "000";
    convertNumberTo3DigitStringWithPadding0(gNumberOfRemainingInfotrons, number);

//loc_4FD46:              ; CODE XREF: drawNumberOfRemainingInfotrons+20j
    uint8_t color = (gNumberOfRemainingInfotrons == 0
                     ? 6
                     : 8);

//loc_4FD56:              ; CODE XREF: drawNumberOfRemainingInfotrons+31j
    drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 272, 14, color, number);
}

void clearNumberOfRemainingRedDisks() // sub_4FD65   proc near       ; CODE XREF: runLevel+E9p
{
//loc_4FD6D:              ; CODE XREF: clearNumberOfRemainingRedDisks+5j
    al = byte_5197C;
    if (al == 0)
    {
        return;
    }

//loc_4FD75:              ; CODE XREF: clearNumberOfRemainingRedDisks+Dj
    al--;
    if (al != 0)
    {
        byte_5197C = al;
        return;
    }

//loc_4FD7D:              ; CODE XREF: clearNumberOfRemainingRedDisks+12j
    byte_5197C = al;

    // Only draws 7 pixel height? That sprite is 8 pixel height.
    // (A few days later...) it's 7 because this function just clears the text written
    // in drawNumberOfRemainingRedDisks, and the text is just 7 pixel height, so no need for the 8th line.
    //
    uint8_t spriteHeight = 7;

    uint16_t srcX = 272;
    uint16_t srcY = 388;

    uint16_t dstX = 304;
    uint16_t dstY = 14;

//loc_4FD99:              ; CODE XREF: clearNumberOfRemainingRedDisks+3Cj
    for (int y = 0; y < spriteHeight; ++y)
    {
        uint32_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX;
        uint32_t dstAddress = (dstY + y) * kScreenWidth + dstX;
        memcpy(&gPanelRenderedBitmapData[dstAddress], &gMovingDecodedBitmapData[srcAddress], kTileSize);
    }
}

void decreaseRemainingRedDisksIfNeeded(uint16_t position) // sub_4FDB5   proc near       ; CODE XREF: update?+124Fp
                    // ; update?+1266p ...
{
    if ((word_59B73 & 0xFF) == 0
        && gPlantedRedDiskCountdown != 0
        && gPlantedRedDiskPosition == position)
    {
        return;
    }

//loc_4FDCA:              ; CODE XREF: decreaseRemainingRedDisksIfNeeded+5j decreaseRemainingRedDisksIfNeeded+Cj ...
    gNumberOfRemainingRedDisks++;
    drawNumberOfRemainingRedDisks();
}

void drawNumberOfRemainingRedDisks() // sub_4FDCE   proc near       ; CODE XREF: handleGameUserInput+7F4p
                   // ; update?+1369p
{
//loc_4FDD6:              ; CODE XREF: drawNumberOfRemainingRedDisks+5j
    char numberString[4] = "000";
    convertNumberTo3DigitStringWithPadding0(gNumberOfRemainingRedDisks, numberString);
//    mov di, 6D2h
//    mov si, 87CAh
    uint8_t color = 0;
    if (gNumberOfRemainingRedDisks != 0)
    {
//loc_4FDF1:              ; CODE XREF: drawNumberOfRemainingRedDisks+1Dj
        color = 6;
    }
    else
    {
        color = 8;
    }

//loc_4FDF3:              ; CODE XREF: drawNumberOfRemainingRedDisks+21j
    drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 304, 14, color, &numberString[1]);
    byte_5197C = 0x46; // 70
}

void drawGameTime() // sub_4FDFD   proc near       ; CODE XREF: runLevel+29p
                   // ; runLevel:noFlashing2p ...
{
    // Only the 2 last digits will be printed, hence why it will be used with &number[1] everywhere
    char number[4] = "000";

    if ((gLastDrawnMinutesAndSeconds & 0xFF) != gGameSeconds) // byte
    {
        gLastDrawnMinutesAndSeconds = (gLastDrawnMinutesAndSeconds & 0xFF00) + gGameSeconds; // byte
        convertNumberTo3DigitStringWithPadding0(gGameSeconds, number);
//loc_4FE2C:              ; CODE XREF: drawGameTime+2Aj
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 208, 3, 6, &number[1]); // seconds
    }

//loc_4FE36:              ; CODE XREF: drawGameTime+12j
    if ((gLastDrawnMinutesAndSeconds >> 8) != gGameMinutes) // byte
    {
        gLastDrawnMinutesAndSeconds = (gGameMinutes << 8) + (gLastDrawnMinutesAndSeconds & 0x00FF); // byte
        convertNumberTo3DigitStringWithPadding0(gGameMinutes, number);
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 184, 3, 6, &number[1]); // minutes
    }

//loc_4FE5F:              ; CODE XREF: drawGameTime+40j
    if (gLastDrawnHours != gGameHours)
    {
        gLastDrawnHours = gGameHours;
        convertNumberTo3DigitStringWithPadding0(gGameHours, number);
        drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, 160, 3, 6, &number[1]); // hours
    }
}

void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text) //   proc near       ; CODE XREF: drawTextWithChars8Font+7p
{
    drawTextWithChars8FontToBuffer(gScreenPixels, destX, destY, color, text);
}

void drawTextWithChars8FontToBuffer(uint8_t *buffer, size_t destX, size_t destY, uint8_t color, const char *text)
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
            for (uint8_t x = 0; x < kBitmapFontCharacter8Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars8BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter8Width + destX + x);
                buffer[destAddress] = color * pixelValue;
            }
        }
    }
}

void drawGamePanel() // sub_501C0   proc near       ; CODE XREF: start+338p handleGameUserInput+678p ...
{
    memcpy(&gPanelRenderedBitmapData, &gPanelDecodedBitmapData, sizeof(gPanelRenderedBitmapData));
    drawGamePanelText();
}

void drawSpeedFixTitleAndVersion() //   proc near       ; CODE XREF: start+2E6p
{
    drawTextWithChars6FontWithOpaqueBackground(102, 11, 1, "SUPAPLEX VERSION 7.0");
}

void drawSpeedFixCredits() // showNewCredits  proc near       ; CODE XREF: start+2ECp
{
    drawTextWithChars6FontWithOpaqueBackground(60, 168, 0xE, "VERSIONS 1-4 + 6.X BY HERMAN PERK");
    drawTextWithChars6FontWithOpaqueBackground(60, 176, 0xE, "VERSIONS 5.X BY ELMER PRODUCTIONS");
    drawTextWithChars6FontWithOpaqueBackground(60, 184, 0xE, "  VERSION 7.0 BY SERGIO PADRINO  ");

    videoloop();

    do
    {
//loc_502F1:             // ; CODE XREF: drawSpeedFixCredits+28j
        int9handler(1);

        if (gIsScrollLockPressed == 1)
        {
            gIsDebugModeEnabled = 1;
        }
//loc_50301:             // ; CODE XREF: drawSpeedFixCredits+1Ej
    }
    while (keyPressed == SDL_SCANCODE_UNKNOWN
           && isAnyGameControllerButtonPressed() == 0);

    byte_510AB = 1;
}

void exitWithError(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    SDL_Quit();
    exit(errno);
}

void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    int scrollX = MAX(0, MIN(x, kLevelBitmapWidth - width));
    int scrollY = MAX(0, MIN(y, kLevelBitmapHeight - height));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            gScreenPixels[y * kScreenWidth + x] = gLevelBitmapData[(scrollY + y) * kLevelBitmapWidth + x + scrollX];
        }
    }
}

void drawCurrentLevelViewport(uint16_t panelHeight)
{
    uint16_t viewportHeight = kScreenHeight - panelHeight;

    drawLevelViewport(gScrollOffsetX, gScrollOffsetY, kScreenWidth, viewportHeight);

    for (int y = 0; y < panelHeight; ++y)
    {
        uint32_t srcAddress = y * kPanelBitmapWidth;
        uint32_t dstAddress = (viewportHeight + y) * kScreenWidth;
        memcpy(&gScreenPixels[dstAddress], &gPanelRenderedBitmapData[srcAddress], kPanelBitmapWidth);
    }
}

void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, uint16_t dstX, uint16_t dstY)
{
    assert((width % kTileSize) == 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            size_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX + x;
            size_t dstAddress = (dstY + y - kLevelEdgeSize) * kLevelBitmapWidth + dstX + x - kLevelEdgeSize;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }
}

void updateWindowViewport()
{
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(gRenderer, &windowWidth, &windowHeight);
    float textureAspectRatio = (float)kScreenWidth / kScreenHeight;
    float screenAspectRatio = (float)windowWidth / windowHeight;

    if (textureAspectRatio > screenAspectRatio) {
        gWindowViewport.x = 0;
        gWindowViewport.w = windowWidth;
        gWindowViewport.h = gWindowViewport.w / textureAspectRatio;
        gWindowViewport.y = (windowHeight - gWindowViewport.h) >> 1;
    }
    else {
        gWindowViewport.y = 0;
        gWindowViewport.h = windowHeight;
        gWindowViewport.w = gWindowViewport.h * textureAspectRatio;
        gWindowViewport.x = (windowWidth - gWindowViewport.w) >> 1;
    }
}

int windowResizingEventWatcher(void* data, SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT
        && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        updateWindowViewport();
        videoloop();
    }
    return 0;
}

void emulateClock()
{
    static Uint32 sLastTickCount = 0;
    static Uint32 sRemainingTicks = 0;
    static const Uint32 sClockInterval = 20; // 20 ms is what the original game seemed to use

    if (sLastTickCount == 0)
    {
        sLastTickCount = SDL_GetTicks();
        return;
    }

    sRemainingTicks += SDL_GetTicks() - sLastTickCount;

    while (sRemainingTicks > sClockInterval)
    {
        int8handler();
        sRemainingTicks -= sClockInterval;
    }

    sLastTickCount = SDL_GetTicks();
}

void handleSDLEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            gShouldExitLevel = 1;
            gShouldExitGame = 1;
        }
    }

    emulateClock();
}

void startTrackingRenderDeltaTime()
{
    gRenderDeltaTime = SDL_GetTicks();
}

uint32_t updateRenderDeltaTime()
{
    uint32_t duration = SDL_GetTicks() - gRenderDeltaTime;
    gRenderDeltaTime = SDL_GetTicks();
    return duration;
}


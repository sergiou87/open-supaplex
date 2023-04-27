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
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "animations.h"
#include "audio.h"
#include "buttonBorders.h"
#include "commandLineParser.h"
#include "conditionals.h"
#include "config.h"
#include "controller.h"
#include "demo.h"
#include "file.h"
#include "globals.h"
#include "graphics.h"
#include "input.h"
#include "keyboard.h"
#include "logging.h"
#include "menu.h"
#include "savegame.h"
#include "touchscreen.h"
#include "utils.h"
#include "video.h"
#include "virtualKeyboard.h"
#include "system.h"

#ifdef __PSP__
#include <pspkernel.h>

// PSP_MODULE_INFO("OpenSupaplex", 0, 7, 1); -> SDL_main sets this for us, for now at least
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-1024);
#endif

// title1DataBuffer -> A000:4DAC - A000:CAAC
// title2DataBuffer -> 0x4DD4 - 0xCAD4

// maps are 58 x 22 tiles

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

uint8_t byte_50919 = 0;
uint8_t byte_5091A = 0;
UserInput gCurrentUserInput = 0; // byte_50941 -> 0x0631
uint8_t byte_50946 = 0;
uint16_t word_50947 = 0;
uint16_t word_50949 = 0;
uint8_t byte_50953 = 0;
uint8_t byte_50954 = 0;
uint8_t gShouldAutoselectNextLevelToPlay = 0; // byte_51ABE
uint8_t gHasChangedLevelSetFromAdvancedMenu = 0;
uint8_t byte_58D47 = 0; //
uint8_t byte_59821 = 0; //
uint8_t byte_59822 = 0; //
uint8_t byte_59823 = 0; //
uint8_t gDemoRecordingRandomGeneratorSeedHigh = 0; // byte_59B5C
uint8_t gDemoRecordingRandomGeneratorSeedLow = 0; // byte_59B5F
uint8_t gToggleFancyEasyTilesThrottleCounter = 0; // byte_59B7A -> data_subrest
uint8_t gIsShowingFancyTiles = 1; // byte_59B7B -> data_subrstflg
uint8_t gToggleGravityAutorepeatFlag = 0; // byte_59B7C
uint8_t gToggleZonksFrozenAutorepeatFlag = 0; // byte_59B7D
uint8_t gToggleEnemiesFrozenAutorepeatFlag = 0; // byte_59B7E
uint8_t gDebugSkipPreviousLevelAutorepeatFlag_1 = 0; // byte_59B7F
uint8_t gDebugSkipPreviousLevelAutorepeatFlag_2 = 0; // byte_59B80
uint8_t gDebugSkipNextLevelAutorepeatFlag_1 = 0; // byte_59B81
uint8_t gDebugSkipNextLevelAutorepeatFlag_2 = 0; // byte_59B82
uint8_t byte_59B83 = 0;
uint8_t byte_59B86 = 0;
uint16_t gDemoRecordingRandomGeneratorSeed = 0; // word_5A199
// uint8_t byte_5A140 = 0; // speedFixMagicNumber inside of level
uint8_t byte_5A19B = 0;
uint8_t gIsLevelStartedAsDemo = 0; // byte_5A19C
uint8_t gDemoRecordingJustStarted = 0; // byte_5A2F8
uint8_t gHasUserCheated = 0; // byte_5A2F9
uint8_t byte_5A323 = 0;
uint16_t word_5A33C = 0;
uint8_t gHasUserInterruptedDemo = 0; // byte_5A33E
uint8_t gIsGameBusy = 0; // byte_5A33F -> this was used mainly to avoid some graphic glitches when some text from the main menu was written on the game field
// uint8_t gIsMouseAvailable = 0; // byte_58487
uint8_t gLevelListButtonPressed = 0; // byte_50918
uint8_t gLevelListDownButtonPressed = 0; // byte_50916
uint8_t gLevelListUpButtonPressed = 0; // byte_50917
uint8_t gNewPlayerEntryIndex = 0; // byte_59820
uint8_t gPlayerListButtonPressed = 0; // byte_50912
uint8_t gPlayerListDownButtonPressed = 0; // byte_50910
uint8_t gPlayerListUpButtonPressed = 0; // byte_50911
uint8_t gRankingListButtonPressed = 0; // byte_50915
uint8_t gRankingListDownButtonPressed = 0; // byte_50913
uint8_t gRankingListUpButtonPressed = 0; // byte_50914
uint16_t gCurrentSelectedLevelIndex = 0; // word_51ABC
uint16_t gNewPlayerNameLength = 0; // word_58475

uint16_t word_5157A = 0x4A62; // -> 0x126A -> (64, 132)
uint16_t word_5157C = 0x0502; // -> 0x126C -> (97, 132)
uint16_t kMurphyStillSpriteCoordinates = 0x4A80; // word_5157E -> 0x126E -> (304, 132)
uint16_t word_51580 = 0x1AB2; // -> 0x1270 -> (0, 32)
uint16_t word_515A2 = 0x32A2; // -> 0x1292 -> (224, 82) ??

uint16_t word_515C4 = 0x1358; // (240, 178) -> 0x12B4

// { 128, 64 }, // -> 0x2a02 -> 12f6 -> orange disk falling

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
uint16_t gIsMoveScrollModeEnabled = 0; // word_51A01
uint16_t gDebugExtraRenderDelay = 1; // this was used to add an extra delay in debug mode using keys 1-9
uint16_t word_58463 = 0;
uint8_t gIsInMainMenu = 0;
uint16_t gAutomaticDemoPlaybackCountdown = 0; // word_58465
uint16_t word_58467 = 0;
uint16_t gLevelListThrottleCurrentCounter = 0; // word_58469
uint16_t gLevelListThrottleNextCounter = 0;
uint16_t gPlayerListThrottleCurrentCounter = 0; // word_5846D
uint16_t gPlayerListThrottleNextCounter = 0; // word_5846F
uint16_t gRankingListThrottleCurrentCounter = 0; // word_58471
uint16_t gRankingListThrottleNextCounter = 0; // word_58473
uint16_t gSelectedOriginalDemoIndex = 0; // word_599D6 -> used loading old demo files demo
uint16_t gSelectedOriginalDemoLevelNumber = 0; // word_599D8 -> used loading old demo files demo -> the high byte is set to -1 in readLevels for some unknown reason
// These two store the scroll offset to get back to Murphy when we're in "free mode"
uint16_t gMurphyScrollOffsetX = 0; // word_59B88
uint16_t gMurphyScrollOffsetY = 0; // word_59B8A
uint16_t gLevelSetRotationThrottleCurrentCounter = 0; // word_59B8C
uint16_t gLevelSetRotationThrottleNextCounter = 0; // word_59B8E
uint16_t gLastDrawnMinutesAndSeconds; // word_510B7
uint8_t gLastDrawnHours; // byte_510B9
FILE *gCurrentRecordingDemoFile; // word_510E4
uint8_t gDemoRecordingLowestSpeed; // speed?2
int16_t gAdditionalScrollOffsetX; // word_51963
int16_t gAdditionalScrollOffsetY; // word_51965
uint8_t isJoystickEnabled = 0; // byte_50940
uint8_t isMusicEnabled = 0; // byte_59886
uint8_t isFXEnabled = 0; // byte_59885

uint8_t gIsFlashingBackgroundModeEnabled = 0; // flashingbackgroundon
const float kSpeedTimeFactors[kNumberOfGameSpeeds] = { 3.5, 3.0, 2.5, 2.0, 1.5, 1.0, 0.75, 2.0 / 3.0, 5.0 / 8.0, 3.0 / 5.0, 1.0 / 70.0 };

// Used to measure game speed (reference is 35 iterations per second)
float gGameIterationRate = 0.f;
uint32_t gGameIterationRateReferenceTime = 0;

// Used to limit game speed
uint32_t gGameIterationStartTime = 0;
uint32_t gNumberOfGameIterations = 0;

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

static const int kConfigDataLength = 4;

uint16_t gRandomGeneratorSeed = 0;

enum MouseButton
{
    MouseButtonLeft = 1 << 0,
    MouseButtonRight = 1 << 1,
};

uint8_t gShouldCloseAdvancedMenu = 0;
uint8_t gAdvancedMenuRecordDemoIndex = 0;
uint8_t gAdvancedMenuPlayDemoIndex = 0;

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

typedef void (*MovingFunction)(int16_t);
typedef void (*FrameBasedMovingFunction)(int16_t, uint8_t);

void updateZonkTiles(int16_t position);
void updateInfotronTiles(int16_t position);
void updateOrangeDiskTiles(int16_t position);
void updateSnikSnakTiles(int16_t position);
void updateTerminalTiles(int16_t position);
void updateElectronTiles(int16_t position);
void updateBugTiles(int16_t position);
void updateExplosionTiles(int16_t position);

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

void updateElectronTurnLeft(int16_t position, uint8_t frame);
void updateElectronTurnRight(int16_t position, uint8_t frame);
void updateElectronMovementUp(int16_t position, uint8_t frame);
void updateElectronMovementDown(int16_t position, uint8_t frame);
void updateElectronMovementRight(int16_t position, uint8_t frame);
void updateElectronMovementLeft(int16_t position, uint8_t frame);

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

void updateSnikSnakTurnLeft(int16_t position, uint8_t frame);
void updateSnikSnakTurnRight(int16_t position, uint8_t frame);
void updateSnikSnakMovementUp(int16_t position, uint8_t frame);
void updateSnikSnakMovementLeft(int16_t position, uint8_t frame);
void updateSnikSnakMovementDown(int16_t position, uint8_t frame);
void updateSnikSnakMovementRight(int16_t position, uint8_t frame);

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

void throttledRotateLevelSet(uint8_t descending);
void rotateLevelSet(uint8_t descending);
void updateMenuAfterLevelSetChanged(void);
void initializeGameStateData(void);
void startDirectlyFromLevel(uint8_t levelNumber);
void stopDemoAndPlay(void);
void emulateClock(void);
void loadScreen2(void);
void readEverything(void);
void drawSpeedFixTitleAndVersion(void);
void openCreditsBlock(void);
void drawSpeedFixCredits(void);
void readConfig(void);
void activateAdlibSound(void);
void activateSoundBlasterSound(void);
void activateRolandSound(void);
void activateCombinedSound(void);
void activateInternalStandardSound(void);
void activateInternalSamplesSound(void);
void prepareDemoRecordingFilename(void);
void runMainMenu(void);
void convertNumberTo3DigitPaddedString(uint8_t number, char numberString[3], char useSpacesForPadding);
void stopMusicAndSounds(void);
void playMusicIfNeeded(void);
void stopMusic(void);
void playExplosionSound(void);
void playInfotronSound(void);
void playPushSound(void);
void playFallSound(void);
void playBugSound(void);
void playBaseSound(void);
void playExitSound(void);
void sound11(void);
void savePlayerListData(void);
void saveHallOfFameData(void);
void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus);
void drawMainMenuButtonBorders(void);
void drawMainMenuButtonBorder(ButtonBorderDescriptor border, uint8_t color);
void generateRandomSeedFromClock(void);
void initializeFadePalette(void);
void initializeMouse(void);
void prepareLevelDataForCurrentPlayer(void);
void drawPlayerList(void);
void drawLevelList(void);
void drawHallOfFame(void);
void drawRankings(void);
void drawTextWithChars6FontWithOpaqueBackgroundIfPossible(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars6FontWithTransparentBackgroundIfPossible(size_t destX, size_t destY, uint8_t color, const char *text);
void waitForKeyMouseOrJoystick(void);
void drawMenuTitleAndDemoLevelResult(void);
void scrollRightToNewScreen(void);
void scrollLeftToMainMenu(void);
void convertNumberTo3DigitStringWithPadding0(uint8_t number, char numberString[3]);
void changePlayerCurrentLevelState(void);
void updateHallOfFameEntries(void);
void drawSoundTypeOptionsSelection(uint8_t *destBuffer);
void dimOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer);
void drawOptionsMenuLine(ButtonBorderDescriptor border, uint8_t color, uint8_t *destBuffer);
void highlightOptionsButtonText(size_t startX, size_t startY, size_t width, size_t height, uint8_t *destBuffer);
void drawAudioOptionsSelection(uint8_t *destBuffer);
void drawInputOptionsSelection(uint8_t *destBuffer);
void updateOptionsMenuState(uint8_t *destBuffer);
void convertLevelNumberTo3DigitStringWithPadding0(uint8_t number);
void readLevels(void);
void initializeGameInfo(void);
void drawGamePanel(void);
void drawNumberOfRemainingInfotrons(void);
void drawGameTime(void);
uint16_t convertToEasyTiles(void);
void resetNumberOfInfotrons(uint16_t numberOfInfotronsInGameField);
void findMurphy(void);
void drawGamePanelText(void);
void scrollToMurphy(void);
void runLevel(void);
void slideDownGameDash(void);
void updateScrollOffset(void);
uint16_t generateRandomNumber(void);
void handleGameUserInput(void);
void loc_49C41(void);
void loc_49C2C(char text[3]);
void showSavegameOperationError(void);
void saveGameSnapshot(void);
void loadGameSnapshot(void);
void checkDebugKeys(void);
void loc_4988E(void);
void restoreOriginalFancyTiles(void);
void updateMovingObjects(void);
int16_t updateMurphy(int16_t position);
int16_t updateMurphyAnimation(int16_t position);
int16_t updateMurphyAnimationInfo(int16_t position, MurphyAnimationDescriptor unknownMurphyData);
int16_t handleMurphyDirectionRight(int16_t position);
int16_t handleMurphyDirectionDown(int16_t position);
int16_t handleMurphyDirectionLeft(int16_t position);
int16_t handleMurphyDirectionUp(int16_t position);
void detonateYellowDisks(void);
void updateUserInputInScrollMovementMode(void);
void updateUserInput(void);
void saveInputForDemo(void);
void simulateDemoInput(void);
void fetchAndInitializeLevel(void);
void removeTiles(LevelTileType tileType);
void restartLevel(void);
void restartLevelWithoutAddingCurrentGameTimeToPlayer(void);
void recordDemo(uint16_t demoIndex);
void stopRecordingDemo(void);
void debugSkipLevel(void);
void forceRestoreOriginalFancyTiles(void);
void drawNumberOfRemainingRedDisks(void);
void clearAdditionalInfoInGamePanelIfNeeded(void);
void updatePlantedRedDisk(void);
void updateExplosionTimers(void);
void addCurrentGameTimeToPlayer(void);
void drawFailedLevelResultScreen(void);
void handleZonkStateAfterFallingOneTile(int16_t position);
void detonateBigExplosion(int16_t position);
void detonateZonk(int16_t position, uint8_t state, uint8_t tile);
void sub_4AA34(int16_t position, uint8_t state, uint8_t tile);
void sub_4AAB4(int16_t position);
uint8_t checkMurphyMovementToPosition(int16_t position, UserInput userInput);
void handleZonkPushedByMurphy(int16_t position);
void decreaseRemainingRedDisksIfNeeded(int16_t position);
void updateSpecialPort(int16_t position);
void handleInfotronStateAfterFallingOneTile(int16_t position);
void int9handler(uint8_t shouldYieldCpu);
void updateDemoRecordingLowestSpeed(void);
void playDemo(uint16_t demoIndex);

static const char *kAdvancedConfigGeneralSection = "general";
static const char *kAdvancedConfigDebugSection = "debug";

static const char *kAdvancedConfigPlayerKey = "player";
static const char *kAdvancedConfigLevelSetKey = "levelSet";
static const char *kAdvancedConfigGameSpeedKey = "gameSpeed";
static const char *kAdvancedConfigMusicVolumeKey = "musicVolume";
static const char *kAdvancedConfigFXVolumeKey = "fxVolume";
static const char *kAdvancedConfigScalingModeKey = "scalingMode";
static const char *kAdvancedConfigFullscreenKey = "fullscreen";
static const char *kAdvancedConfigDisplayFPSKey = "displayFPS";
static const char *kAdvancedConfigLimitFPSKey = "limitFPS";

void readAdvancedConfig()
{
    Config *config = initializeConfigForReading("ADVANCED.CFG");

    if (config == NULL)
    {
        spLogInfo("Couldn't read advanced config");
        return;
    }

    char currentSuffix[3] = "AT";
    strcpy(currentSuffix, &gLevelsDatFilename[8]);

    int player = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigPlayerKey, 0);
    player = CLAMP(player, 0, kNumberOfPlayers - 1);
    gCurrentPlayerIndex = player;

    // Only apply level set from config if it wasn't overriden before (by command line)
    if (strcmp(currentSuffix, "AT") == 0)
    {
        int levelSet = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigLevelSetKey, 0);

        if (levelSet != 0)
        {
            char newSuffix[3] = "00";
            snprintf(newSuffix, 3, "%02d", levelSet);

            strcpy(&gLevelsDatFilename[8], newSuffix);
            strcpy(&gLevelLstFilename[7], newSuffix);
            strcpy(&gDemo0BinFilename[7], newSuffix);
            strcpy(&gPlayerLstFilename[8], newSuffix);
            strcpy(&gHallfameLstFilename[10], newSuffix);

            if (gShouldAlwaysWriteSavegameSav == 0) // cmp byte ptr gShouldAlwaysWriteSavegameSav, 0
            {
                strcpy(&gSavegameSavFilename[10], newSuffix);
            }
        }
    }

    gGameSpeed = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigGameSpeedKey, kDefaultGameSpeed);

    int volume = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigMusicVolumeKey, getMusicVolume());
    setMusicVolume(volume);

    volume = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigFXVolumeKey, getSoundEffectsVolume());
    setSoundEffectsVolume(volume);

    int scalingMode = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigScalingModeKey, getScalingMode());
    setScalingMode(scalingMode);

    gShouldShowFPS = readConfigInt(config, kAdvancedConfigDebugSection, kAdvancedConfigDisplayFPSKey, gShouldShowFPS);
    gShouldLimitFPS = readConfigInt(config, kAdvancedConfigDebugSection, kAdvancedConfigLimitFPSKey, gShouldLimitFPS);

    int fullscreen = readConfigInt(config, kAdvancedConfigGeneralSection, kAdvancedConfigFullscreenKey, getFullscreenMode());
    setFullscreenMode(fullscreen);

    destroyConfig(config);
}

void writeAdvancedConfig()
{
    Config *config = initializeConfigForWriting("ADVANCED.CFG");

    if (config == NULL)
    {
        spLogInfo("Couldn't write advanced config");
        return;
    }

    writeConfigSection(config, kAdvancedConfigGeneralSection);

    char currentSuffix[3] = "AT";
    strcpy(currentSuffix, &gLevelsDatFilename[8]);

    int levelSet = 0;

    if (strcmp(currentSuffix, "AT") != 0)
    {
        levelSet = atoi(currentSuffix);
    }

    writeConfigInt(config, kAdvancedConfigPlayerKey, gCurrentPlayerIndex);
    writeConfigInt(config, kAdvancedConfigLevelSetKey, levelSet);
    writeConfigInt(config, kAdvancedConfigGameSpeedKey, gGameSpeed);
    writeConfigInt(config, kAdvancedConfigMusicVolumeKey, getMusicVolume());
    writeConfigInt(config, kAdvancedConfigFXVolumeKey, getSoundEffectsVolume());
    writeConfigInt(config, kAdvancedConfigScalingModeKey, getScalingMode());
    writeConfigInt(config, kAdvancedConfigFullscreenKey, getFullscreenMode());

    writeConfigSection(config, kAdvancedConfigDebugSection);
    writeConfigInt(config, kAdvancedConfigDisplayFPSKey, gShouldShowFPS);
    writeConfigInt(config, kAdvancedConfigLimitFPSKey, gShouldLimitFPS);

    destroyConfig(config);
}

/// @return 1 if the action was to go back / close the menu
uint8_t handleAdvancedOptionsMenuInput(AdvancedOptionsMenu *menu)
{
    if (isUpButtonPressed())
    {
        playBaseSound();
        moveUpAdvancedOptionsSelectedEntry(menu);
    }

    if (isDownButtonPressed())
    {
        playBaseSound();
        moveDownAdvancedOptionsSelectedEntry(menu);
    }

    if (isLeftButtonPressed())
    {
        playBaseSound();
        decreaseAdvancedOptionsSelectedEntry(menu);
    }

    if (isRightButtonPressed())
    {
        playBaseSound();
        increaseAdvancedOptionsSelectedEntry(menu);
    }

    if (isMenuConfirmButtonPressed())
    {
        playInfotronSound();
        selectAdvancedOptionsSelectedEntry(menu);
    }

    if (isMenuCancelButtonPressed())
    {
        playPushSound();
        return 1;
    }

    if (isPauseButtonPressed())
    {
        playPushSound();
        gShouldCloseAdvancedMenu = 1;
        return 1;
    }

    if (gShouldCloseAdvancedMenu)
    {
        return 1;
    }

    return 0;
}

void advancedOptionsMenuWaitForKeyPress()
{
    do
    {
        int9handler(1);
    }
    while (isUpButtonPressed() == 0
           && isDownButtonPressed() == 0
           && isLeftButtonPressed() == 0
           && isRightButtonPressed() == 0
           && isMenuBackButtonPressed() == 0
           && isMenuConfirmButtonPressed() == 0
           && isMenuCancelButtonPressed() == 0
           && isPauseButtonPressed() == 0);
}

void advancedOptionsMenuWaitForKeyRelease()
{
    do
    {
        int9handler(1);
    }
    while (isUpButtonPressed()
           || isDownButtonPressed()
           || isLeftButtonPressed()
           || isRightButtonPressed()
           || isMenuBackButtonPressed()
           || isMenuConfirmButtonPressed()
           || isMenuCancelButtonPressed()
           || isPauseButtonPressed());
}

void runAdvancedOptionsMenu(AdvancedOptionsMenu *menu)
{
    playFallSound();

    advancedOptionsMenuWaitForKeyRelease();

    do
    {
        if (handleAdvancedOptionsMenuInput(menu))
        {
            break;
        }

        renderAdvancedOptionsMenu(menu);
        advancedOptionsMenuWaitForKeyRelease();
        advancedOptionsMenuWaitForKeyPress();
    }
    while (1);

    advancedOptionsMenuWaitForKeyRelease();
}

void buildGameSpeedOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "GAME SPEED: %d", gGameSpeed);
}

void buildMusicVolumeOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "MUSIC VOLUME: %d", getMusicVolume());
}

void buildFXVolumeOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "FX VOLUME: %d", getSoundEffectsVolume());
}

void buildScalingModeOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    static const char *kAspectFitScalingModeString = "NORMAL";
    static const char *kAspectFillScalingModeString = "ZOOM";
    static const char *kIntegerFactorScalingModeString = "INTEGER FACTOR";
    static const char *kFullscreenScalingModeString = "FULLSCREEN";
    static const char *kAspectCorrectScalingModeString = "4:3";

    const char *mode = kAspectFitScalingModeString;

    switch(getScalingMode())
    {
        case ScalingModeAspectFill:
            mode = kAspectFillScalingModeString;
            break;
        case ScalingModeIntegerFactor:
            mode = kIntegerFactorScalingModeString;
            break;
        case ScalingModeFullscreen:
            mode = kFullscreenScalingModeString;
            break;
        case ScalingModeAspectCorrect:
            mode = kAspectCorrectScalingModeString;
            break;
        default:
            mode = kAspectFitScalingModeString;
            break;
    }

    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "SCALING MODE: %s", mode);
}

void buildBooleanOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength], char title[kMaxAdvancedOptionsMenuEntryTitleLength], uint8_t value)
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "%s: %s", title, (value == 0 ? "OFF" : "ON"));
}

void buildDisplayFPSOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    buildBooleanOptionTitle(output, "DISPLAY FPS", gShouldShowFPS);
}

void buildLimitFPSOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    buildBooleanOptionTitle(output, "LIMIT FPS", gShouldLimitFPS);
}

void buildPlayDemoOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "PLAY DEMO: %d", gAdvancedMenuPlayDemoIndex);
}

void buildRecordDemoOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "RECORD DEMO: %d", gAdvancedMenuRecordDemoIndex);
}

void increaseGameSpeed()
{
    if (gGameSpeed < kNumberOfGameSpeeds - 1)
    {
        gGameSpeed++;
    }

    updateDemoRecordingLowestSpeed();
}

void decreaseGameSpeed()
{
    if (gGameSpeed > 0)
    {
        gGameSpeed--;
    }

    updateDemoRecordingLowestSpeed();
}

void updateDemoRecordingLowestSpeed()
{
    if (gGameSpeed < gDemoRecordingLowestSpeed)
    {
        gDemoRecordingLowestSpeed = gGameSpeed;
    }
}

void increaseMusicVolume()
{
    uint8_t volume = getMusicVolume();

    if (volume < kMaxVolume)
    {
        setMusicVolume(volume + 1);
    }
}

void decreaseMusicVolume()
{
    uint8_t volume = getMusicVolume();

    if (volume > 0)
    {
        setMusicVolume(volume - 1);
    }
}

void increaseFXVolume()
{
    uint8_t volume = getSoundEffectsVolume();

    if (volume < kMaxVolume)
    {
        setSoundEffectsVolume(volume + 1);
    }
}

void decreaseAdvancedMenuRecordDemoIndex()
{
    if (gAdvancedMenuRecordDemoIndex > 0)
    {
        gAdvancedMenuRecordDemoIndex--;
    }
}

void increaseAdvancedMenuRecordDemoIndex()
{
    if (gAdvancedMenuRecordDemoIndex < kNumberOfDemos - 1)
    {
        gAdvancedMenuRecordDemoIndex++;
    }
}

void decreaseAdvancedMenuPlayDemoIndex()
{
    if (gAdvancedMenuPlayDemoIndex > 0)
    {
        gAdvancedMenuPlayDemoIndex--;
    }
}

void increaseAdvancedMenuPlayDemoIndex()
{
    if (gAdvancedMenuPlayDemoIndex < kNumberOfDemos - 1)
    {
        gAdvancedMenuPlayDemoIndex++;
    }
}

void decreaseAdvancedMenuScalingMode()
{
    ScalingMode mode = getScalingMode();
    if (mode > 0)
    {
        setScalingMode(mode - 1);
    }
    else
    {
        setScalingMode(ScalingModeCount - 1);
    }
}

void increaseAdvancedMenuScalingMode()
{
    ScalingMode mode = getScalingMode();
    if (mode < ScalingModeCount - 1)
    {
        setScalingMode(mode + 1);
    }
    else
    {
        setScalingMode(0);
    }
}

void decreaseFXVolume()
{
    uint8_t volume = getSoundEffectsVolume();

    if (volume > 0)
    {
        setSoundEffectsVolume(volume - 1);
    }
}

void toggleDisplayFPSOption()
{
    TOGGLE_BOOL(gShouldShowFPS);
}

void toggleLimitFPSOption()
{
    TOGGLE_BOOL(gShouldLimitFPS);
}

void handleResumeOptionSelection()
{
    gShouldCloseAdvancedMenu = 1;
}

void handleRestartLevelOptionSelection()
{
    restartLevel();
    gShouldCloseAdvancedMenu = 1;
}

void handleRemoveZonksOptionSelection()
{
    removeTiles(LevelTileTypeZonk);
    gShouldCloseAdvancedMenu = 1;
}

void handleRemoveHardwareOptionSelection()
{
    removeTiles(LevelTileTypeHardware);
    gShouldCloseAdvancedMenu = 1;
}

void handleRemoveBaseOptionSelection()
{
    removeTiles(LevelTileTypeBase);
    gShouldCloseAdvancedMenu = 1;
}

void handleRemoveChipsOptionSelection()
{
    removeTiles(LevelTileTypeChip);
    gShouldCloseAdvancedMenu = 1;
}

void handleRemoveSnikSnakOptionSelection()
{
    removeTiles(LevelTileTypeSnikSnak);
    gShouldCloseAdvancedMenu = 1;
}

void handleMoveScrollOptionSelection()
{
    gIsMoveScrollModeEnabled = 1;
    gShouldCloseAdvancedMenu = 1;
}

void handleRecordDemoOptionSelection()
{
    recordDemo(gAdvancedMenuRecordDemoIndex);
    gShouldCloseAdvancedMenu = 1;
}

void handlePlayDemoOptionSelection()
{
    playDemo(gAdvancedMenuPlayDemoIndex);
    gShouldCloseAdvancedMenu = 1;
}

void handleStopDemoAndPlayOptionSelection()
{
    stopDemoAndPlay();
    gShouldCloseAdvancedMenu = 1;
}

void handleStopRecordingDemoOptionSelection()
{
    stopRecordingDemo();
    gShouldCloseAdvancedMenu = 1;
}

void handleExitLevelOptionSelection()
{
    gShouldKillMurphy = 1;
    gShouldCloseAdvancedMenu = 1;
}

void handleExitGameOptionSelection()
{
    gShouldExitGame = 1;
    gShouldCloseAdvancedMenu = 1;
}

void runAdvancedOptionsSubMenu(AdvancedOptionsMenu menu)
{
    runAdvancedOptionsMenu(&menu);
}

void handleDebugOptionSelection()
{
    AdvancedOptionsMenu menu;
    initializeAdvancedOptionsMenu(&menu);

    strncpy(menu.title, "DEBUG (DANGER)", kMaxAdvancedOptionsMenuEntryTitleLength);

    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "",
        buildDisplayFPSOptionTitle,
        toggleDisplayFPSOption,
        toggleDisplayFPSOption,
        toggleDisplayFPSOption,
    });
    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "",
        buildLimitFPSOptionTitle,
        toggleLimitFPSOption,
        toggleLimitFPSOption,
        toggleLimitFPSOption,
    });
    if (gIsInMainMenu == 0)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "LOAD GAME STATE",
            NULL,
            loadGameSnapshot,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "SAVE GAME STATE",
            NULL,
            saveGameSnapshot,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "MOVE FREELY",
            NULL,
            handleMoveScrollOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE ZONKS",
            NULL,
            handleRemoveZonksOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE HARDWARE",
            NULL,
            handleRemoveHardwareOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE SNIK SNAKS",
            NULL,
            handleRemoveSnikSnakOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE CHIPS",
            NULL,
            handleRemoveChipsOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE HARDWARE",
            NULL,
            handleRemoveHardwareOptionSelection,
            NULL,
            NULL,
        });
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "REMOVE BASE",
            NULL,
            handleRemoveBaseOptionSelection,
            NULL,
            NULL,
        });
    }

    runAdvancedOptionsSubMenu(menu);
}

void buildLevelSetOptionTitle(char output[kMaxAdvancedOptionsMenuEntryTitleLength])
{
    const char *kOriginalLevelSetName = "ORIGINAL";
    char currentSuffix[3] = "AT";
    strcpy(currentSuffix, &gLevelsDatFilename[8]);

    const char *levelSetName = kOriginalLevelSetName;
    if (strcmp(currentSuffix, "AT") != 0)
    {
        levelSetName = currentSuffix;
    }

    snprintf(output, kMaxAdvancedOptionsMenuEntryTitleLength, "LEVEL SET: %s", levelSetName);
}

void decreaseLevelSet()
{
    rotateLevelSet(1);
    gHasChangedLevelSetFromAdvancedMenu = 1;
}

void increaseLevelSet()
{
    rotateLevelSet(0);
    gHasChangedLevelSetFromAdvancedMenu = 1;
}

void runAdvancedOptionsRootMenu()
{
    AdvancedOptionsMenu menu;
    initializeAdvancedOptionsMenu(&menu);

    strncpy(menu.title, "OPENSUPAPLEX " VERSION_STRING, kMaxAdvancedOptionsMenuEntryTitleLength);

    if (gIsPlayingDemo)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "RESUME DEMO",
            NULL,
            handleResumeOptionSelection,
            NULL,
            NULL,
        });
    }
    else
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "RESUME GAME",
            NULL,
            handleResumeOptionSelection,
            NULL,
            NULL,
        });
    }

    if (gIsInMainMenu)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "",
            buildLevelSetOptionTitle,
            NULL,
            decreaseLevelSet,
            increaseLevelSet,
        });
    }

    if (gIsPlayingDemo && gIsInMainMenu == 0)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "STOP DEMO AND PLAY",
            NULL,
            handleStopDemoAndPlayOptionSelection,
            NULL,
            NULL,
        });
    }

    if (gIsInMainMenu == 0)
    {
        if (gIsPlayingDemo)
        {
            addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
                "RESTART DEMO",
                NULL,
                handleRestartLevelOptionSelection,
                NULL,
                NULL,
            });
        }
        else
        {
            addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
                "RESTART LEVEL",
                NULL,
                handleRestartLevelOptionSelection,
                NULL,
                NULL,
            });
        }
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "",
            buildGameSpeedOptionTitle,
            NULL,
            decreaseGameSpeed,
            increaseGameSpeed,
        });
    }
    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "",
        buildMusicVolumeOptionTitle,
        NULL,
        decreaseMusicVolume,
        increaseMusicVolume,
    });
    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "",
        buildFXVolumeOptionTitle,
        NULL,
        decreaseFXVolume,
        increaseFXVolume,
    });
    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "",
        buildScalingModeOptionTitle,
        NULL,
        decreaseAdvancedMenuScalingMode,
        increaseAdvancedMenuScalingMode,
    });
    if (gIsInMainMenu)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "",
            buildPlayDemoOptionTitle,
            handlePlayDemoOptionSelection,
            decreaseAdvancedMenuPlayDemoIndex,
            increaseAdvancedMenuPlayDemoIndex,
        });
    }
    if (gIsInMainMenu == 0 && gIsPlayingDemo == 0)
    {
        if (gIsRecordingDemo)
        {
            addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
                "STOP RECORDING DEMO",
                NULL,
                handleStopRecordingDemoOptionSelection,
                NULL,
                NULL,
            });
        }
        else
        {
            addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
                "",
                buildRecordDemoOptionTitle,
                handleRecordDemoOptionSelection,
                decreaseAdvancedMenuRecordDemoIndex,
                increaseAdvancedMenuRecordDemoIndex,
            });
        }
    }
    addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
        "DEBUG (DANGER)",
        NULL,
        handleDebugOptionSelection,
        NULL,
        NULL,
    });
    if (gIsInMainMenu == 0)
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "EXIT LEVEL",
            NULL,
            handleExitLevelOptionSelection,
            NULL,
            NULL,
        });
    }
    else
    {
        addAdvancedOptionsEntry(&menu, (AdvancedOptionsMenuEntry) {
            "EXIT GAME",
            NULL,
            handleExitGameOptionSelection,
            NULL,
            NULL,
        });
    }

    gShouldCloseAdvancedMenu = 0;

    saveScreenForAdvancedMenu();

    setPalette(gGameDimmedPalette);

    runAdvancedOptionsMenu(&menu);

    writeAdvancedConfig();

    restoreScreenFromAdvancedMenu();
    videoLoop();

    setPalette(gGamePalette);

    if (gHasChangedLevelSetFromAdvancedMenu)
    {
        updateMenuAfterLevelSetChanged();
        gHasChangedLevelSetFromAdvancedMenu = 0;
    }
}

int main(int argc, char *argv[])
{
    parseCommandLineOptions(argc, argv);

    if (gFastMode == FastModeTypeUltra)
    {
        setLogLevel(LogLevelDemo);
    }

    initializeLogging();
    initializeSystem();
    initializeVideo(gFastMode);
    initializeControllers();

    if (gFastMode != FastModeTypeUltra)
    {
        initializeAudio();
        readAdvancedConfig();
    }

    // Override the initial game speed with the one from the command line if needed
    if (gForcedInitialGameSpeed != kInvalidForcedInitialGameSpeed)
    {
        gGameSpeed = gForcedInitialGameSpeed;
    }

    handleSystemEvents();

    initializeGameStateData();

//doesNotHaveCommandLine:         //; CODE XREF: start+13j start+23Fj ...
    generateRandomSeedFromClock();
    // checkVideo();

//leaveVideoStatus:           //; CODE XREF: start+28Aj
    initializeFadePalette(); // 01ED:026F
    initializeMouse();

    if (gFastMode == FastModeTypeNone)
    {
        setPalette(gBlackPalette);
        videoLoop();
        readAndRenderTitleDat();
        ColorPalette titleDatPalette; // si = 0x5F15;
        convertPaletteDataToPalette(gTitlePaletteData, titleDatPalette);
        fadeToPalette(titleDatPalette);
    }

//isFastMode:              //; CODE XREF: start+2ADj
    loadMurphySprites(); // 01ED:029D
    // Conditions to whether show
    if (gShouldStartFromSavedSnapshot
        || gIsForcedLevel
        || gIsSPDemoAvailableToRun
        || gFastMode)
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
    if (gFastMode == FastModeTypeNone)
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
            gIsGameBusy = 0;
            drawPlayerList();
            initializeGameInfo();
            drawFixedLevel();
            drawGamePanel(); // 01ED:0311
            uint16_t numberOfInfotrons = convertToEasyTiles();
            resetNumberOfInfotrons(numberOfInfotrons);
            findMurphy();
            gCurrentPanelHeight = kPanelBitmapHeight;
            drawCurrentLevelViewport(gCurrentPanelHeight); // Added by me
            if (gFastMode != FastModeTypeUltra)
            {
                fadeToPalette(gGamePalette); // At this point the screen fades in and shows the game
            }

            if (isMusicEnabled == 0)
            {
                stopMusic();
            }

//loc_46F77:              //; CODE XREF: start+352j
            gIsGameBusy = 1;
            runLevel();
            gIsSPDemoAvailableToRun = 0;
            if (gShouldExitGame != 0)
            {
                break; // goto loc_47067;
            }

//loc_46F8E:              //; CODE XREF: start+369j
            if (gFastMode != FastModeTypeNone)
            {
                break;
            }

//isNotFastMode2:              //; CODE XREF: start+373j
            slideDownGameDash(); // 01ED:0351
            if (byte_59B71 != 0)
            {
                loadMurphySprites();
            }

//loc_46FA5:              //; CODE XREF: start+380j
            gIsGameBusy = 0;
            if (gShouldExitGame != 0)
            {
                break; // goto loc_47067;
            }

//loc_46FB4:              //; CODE XREF: start+38Fj
            if (isMusicEnabled == 0)
            {
                playMusicIfNeeded();
            }
        }

        shouldSkipFirstPart = 0;

//loc_46FBE:              //; CODE XREF: start+30Cj start+31Bj ...
        prepareDemoRecordingFilename(); // 01ED:037A

        uint8_t levelNumberForcedToLoad = 0;

        if (gIsSPDemoAvailableToRun == 2)
        {
            gIsSPDemoAvailableToRun = 1;
            if (fileIsDemo == 1)
            {
                playDemo(0);
            }
            else
            {
//loc_46FDF:              //; CODE XREF: start+3B5j
                gIsPlayingDemo = 0;
            }

//loc_46FE4:              //; CODE XREF: start+3BDj
            gShouldUpdateTotalLevelTime = 0;
            gHasUserCheated = 1;
            memcpy(&gSPDemoFileName[3], "---", 3);
//loc_4701A:              //; CODE XREF: start+3DDj start+433j
            startDirectlyFromLevel(1);
            continue;
        }
        else
        {
//loc_46FFF:              //; CODE XREF: start+3A9j
            levelNumberForcedToLoad = gIsForcedLevel;
            gIsForcedLevel = 0;
            gIsPlayingDemo = 0;

            if (levelNumberForcedToLoad > 0)
            {
                convertLevelNumberTo3DigitStringWithPadding0(levelNumberForcedToLoad);
            }
        }

        if (levelNumberForcedToLoad > 0)
        {
//loc_4701A:              //; CODE XREF: start+3DDj start+433j
            startDirectlyFromLevel(levelNumberForcedToLoad);
            continue;
        }

//loc_4704B:              //; CODE XREF: start+3EEj start+3F2j
        if (gShouldStartFromSavedSnapshot != 0)
        {
//loc_4701A:              //; CODE XREF: start+3DDj start+433j
            startDirectlyFromLevel(1);
            continue;
        }
        gHasUserCheated = 0;
        runMainMenu();
    }
    while (gShouldExitGame == 0);

    int runResult = 0;

    if (gFastMode != FastModeTypeNone)
    {
        char *message = "";
        if (gIsLevelStartedAsDemo == 0)
        {
            //loc_47094:              //; CODE XREF: start+45Fj
            if (byte_5A19B == 0)
            {
//loc_470A1:              //; CODE XREF: start+479j
                message = "\"@\"-ERROR: Level(?) failed:     ";
                runResult = 1;
            }
            else
            {
                message = "\"@\"-ERROR: Level(?) successful: ";
            }
        }
        else if (byte_5A19B == 0)
        {
//loc_4708E:              //; CODE XREF: start+466j
            message = "Demo failed:     ";
            runResult = 1;
        }
        else
        {
            message = "Demo successful: ";
        }

//printMessageAfterward:          //; CODE XREF: start+46Cj start+472j ...
        spLogDemo("%s%s", message, demoFileName);
    }
    else
    {
//loc_47067:              //; CODE XREF: start+36Bj start+391j ...
        fadeToPalette(gBlackPalette); // 0x60D5

        writeAdvancedConfig();
    }

    // Tidy up
    destroyAudio();
    destroyLogging();
    destroyVideo();
    destroySystem();

    return runResult;
}

void initializeGameStateData()
{
    // Initialize game state with the same values as in the original game
    static const uint16_t kLevelStatePrecedingPadding[kSizeOfLevelStatePrecedingPadding] = {
        0x8995 , 0x8995 , 0x8995 , 0x8a3b , 0x8a3b , 0x8a3b , 0x8a3b , 0x8a3b ,
        0x8a3b , 0x8a3b , 0x8a3b , 0x8ae8 , 0x8ae8 , 0x8ae8 , 0x8ae8 , 0x8ae8 ,
        0x8ae8 , 0x8ae8 , 0x8ae8 , 0x8bb1 , 0x8bb1 , 0x8bb1 , 0x8bb1 , 0x8bb1 ,
        0x8bb1 , 0x8bb1 , 0x8bb1 , 0x8c85 , 0x8c85 , 0x8c85 , 0x8c85 , 0x8c85 ,
        0x8c85 , 0x8c85 , 0x8c85 , 0x8d5b , 0x8d5b , 0x8d5b , 0x8d5b , 0x8d5b ,
        0x8d5b , 0x8d5b , 0x8d5b , 0x8e06 , 0x8e06 , 0x8e06 , 0x8e06 , 0x8e06 ,
        0x8e06 , 0x8e06 , 0x8e06 , 0x8eac , 0x8eac , 0x8eac , 0x8eac , 0x8eac ,
        0x8eac , 0x8eac , 0x8eac , 0x8f59 , 0x8f59 , 0x8f59 , 0x8f59 , 0x8f59 ,
        0x8f59 , 0x8f59 , 0x8f59 , 0x0000 , 0x1370 , 0x0000 , 0x0000 , 0x17e8 ,
        0x0000 , 0x0000 , 0x0000 , 0x3869 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x86d0 , 0x0000 , 0x34b2 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x8b8f , 0x341d , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x3923 , 0x0909 , 0x0c00 , 0x0800 , 0x5800 , 0x0000 ,
        0x0000 , 0x2500 , 0x0677 , 0x007f , 0x0000 , 0x0001 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0xec00 , 0x2606 , 0x0005 , 0x0000 ,
        0x0000 , 0x0100 , 0x0000 , 0x0000 , 0x3231 , 0x3433 , 0x3635 , 0x3837 ,
        0x3039 , 0x002d , 0x0008 , 0x5751 , 0x5245 , 0x5954 , 0x4955 , 0x504f ,
        0x0000 , 0x000a , 0x5341 , 0x4644 , 0x4847 , 0x4b4a , 0x004c , 0x0000 ,
        0x0000 , 0x585a , 0x5643 , 0x4e42 , 0x004d , 0x0000 , 0x0000 , 0x2000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x002e , 0x001e , 0x0031 , 0x0014 , 0x0039 ,
        0x001f , 0x0014 , 0x0018 , 0xffff , 0x0001 , 0x4c01 , 0x5645 , 0x4c45 ,
        0x2e53 , 0x4144 , 0x0054 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 ,
        0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000 , 0x0000
    };

    for (int idx = 0; idx < kSizeOfLevelStatePrecedingPadding; ++idx)
    {
        uint16_t value = kLevelStatePrecedingPadding[idx];
        value = convert16LE(value);
        StatefulLevelTile *tile = &gCurrentLevelStateWithPadding[idx];
        tile->tile = (value & 0xFF);
        tile->state = (value >> 8);
    }

    gFrameCounter = 0xF000;
}

void startDirectlyFromLevel(uint8_t levelNumber)
{
    gIsGameBusy = 1;
    gShouldAutoselectNextLevelToPlay = 1;
    prepareLevelDataForCurrentPlayer();
    drawPlayerList();
    word_58467 = 0;
    playMusicIfNeeded();
    gCurrentSelectedLevelIndex = levelNumber;
    restoreLastMouseAreaBitmap();
    drawLevelList();
    gShouldLeaveMainMenu = 0;
    byte_5A19B = 0;
}

void slideDownGameDash() // proc near     ; CODE XREF: start:isNotFastMode2p
{
    // 01ED:04ED
    if (gShouldShowGamePanel == 0)
    {
        return;
    }

    for (int panelHeight = kPanelBitmapHeight; panelHeight > 0; --panelHeight)
    {
        // Move the bottom panel line by line to the bottom of the screen
        for (int y = kScreenHeight - 2; y >= kScreenHeight - panelHeight; --y)
        {
            uint16_t srcAddress = y * kScreenWidth;
            uint16_t dstAddress = (y + 1) * kScreenWidth;
            memcpy(&gScreenPixels[dstAddress], &gScreenPixels[srcAddress], kScreenWidth);
        }

        drawCurrentLevelViewport(panelHeight);
        videoLoop();
    }
}

/// This alternative int9 handler seems to control the keys +, -, *, / in the numpad
/// to alter the game speed, and also the key X for something else.
/// @param shouldYieldCpu If 1, will sleep the thread for a bit to prevent 100% CPU usage.
void int9handler(uint8_t shouldYieldCpu) // proc far        ; DATA XREF: setint9+1Fo
{
    updateKeyboardState();

    // 01ED:0659

    if (gIsLeftAltPressed && gIsEnterPressed)
    {
        toggleFullscreen();
    }

//storeKey:               ; CODE XREF: int9handler+2Bj
    if (gIsInMainMenu == 0)
    {
        if (gIsNumpadMultiplyPressed) // Key * in the numpad, restore speed
        {
            gGameSpeed = kDefaultGameSpeed;
            updateDemoRecordingLowestSpeed();
        }
//checkSlash:             ; CODE XREF: int9handler+3Ej
//                ; int9handler+45j
        else if (gIsNumpadDividePressed) // Keypad / -> fastest playback seed
        {
            gGameSpeed = kNumberOfGameSpeeds - 1;
            updateDemoRecordingLowestSpeed();
        }
//checkPlus:              ; CODE XREF: int9handler+54j
        else if (gIsGameSpeedChangeButtonPressed == 0)
        {
            if (isIncreaseGameSpeedButtonPressed())
            {
                increaseGameSpeed();
            }
//checkMinus:             ; CODE XREF: int9handler+65j
//                ; int9handler+71j
            else if (isDecreaseGameSpeedButtonPressed())
            {
                decreaseGameSpeed();
            }
        }

        gIsGameSpeedChangeButtonPressed = (isIncreaseGameSpeedButtonPressed()
                                           || isDecreaseGameSpeedButtonPressed());
    }

//checkX:                 ; CODE XREF: int9handler+39j
//                ; int9handler+60j ...
    if (gIsXKeyPressed && gIsLeftAltPressed != 0)
    {
        gShouldExitLevel = 1;
        gShouldExitGame = 1;
    }

    if (shouldYieldCpu)
    {
        waitTime(10); // Avoid burning the CPU
    }
}

void readConfig() //  proc near       ; CODE XREF: start:loc_46F0Fp
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openWritableFile("SUPAPLEX.CFG", "rb");
    if (file == NULL)
    {
    if (errno == ENOENT || errno == ENOSYS || errno == EIO) // ax == 2? ax has error code, 2 is file not found (http://stanislavs.org/helppc/dos_error_codes.html)
        {
//loc_47551:              //; CODE XREF: readConfig+Fj
                       // ; readConfig+17j
            activateCombinedSound();
            isMusicEnabled = 1;
            isFXEnabled = 1;
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

    size_t bytes = fileReadBytes(configData, sizeof(configData), file);

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
        // calibrateJoystick(); not needed anymore
    }

//loc_47530:              //; CODE XREF: readConfig+85j
    isMusicEnabled = (configData[2] == 'm');

//loc_47540:              //; CODE XREF: readConfig+98j
    isFXEnabled = (configData[3] == 'x');
}

void saveConfiguration() // sub_4755A      proc near               ; CODE XREF: code:loc_4CAECp
{
    FILE *file = openWritableFile("SUPAPLEX.CFG", "wb");
    if (file == NULL)
    {
        spLogInfo("Error opening SUPAPLEX.CFG\n");
        return;
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
    size_t bytes = fileWriteBytes(configData, sizeof(configData), file);
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

    gDemoCurrentInputIndex = 0;
    word_5A33C = 0;

    memset(&gDemos.demoFirstIndices, 0xFF, sizeof(gDemos.demoFirstIndices)); // fills 11 words (22 bytes) with 0xFFFF

    for (int i = 0; i < kNumberOfDemos; ++i)
    {
//loc_47629:              //; CODE XREF: readDemoFiles+175j
        gSelectedOriginalDemoLevelNumber = 0;
        char *filename = gDemo0BinFilename;

        if (gIsSPDemoAvailableToRun == 1)
        {
            filename = demoFileName;
        }
        else
        {
//loc_4763C:             // ; CODE XREF: readDemoFiles+2Cj
            gDemo0BinFilename[4] = '0' + i; // Replaces the number in "DEMO0.BIN" with the right value
        }

//loc_47647:             // ; CODE XREF: readDemoFiles+31j
        FILE *file = openWritableFileWithReadonlyFallback(filename, "rb");
        if (file == NULL)
        {
            return i;
        }

//loc_47651:              //; CODE XREF: readDemoFiles+43j
        if (gIsSPDemoAvailableToRun == 1)
        {
            if (gSelectedOriginalDemoFromCommandLineLevelNumber == 0)
            {
                fseek(file, kLevelDataLength, SEEK_SET);
            }
        }
        else
        {
//loc_47674:             // ; CODE XREF: readDemoFiles+52j
            int result = fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);

            // this is probably to support old level formats
            if (result == 0
                && fileSize < kLevelDataLength)
            {
                gSelectedOriginalDemoLevelNumber = getLevelNumberFromOriginalDemoFile(file, fileSize);
            }

//loc_47690:             // ; CODE XREF: readDemoFiles+76j readDemoFiles+7Aj ...
            fseek(file, 0, SEEK_SET);

            if (gSelectedOriginalDemoLevelNumber == 0)
            {
                Level *level = &gDemos.level[i];
                size_t bytes = fileReadBytes(level, kLevelDataLength, file);

                if (bytes < kLevelDataLength)
                {
                    return i;
                }

//loc_476D3:           //   ; CODE XREF: readDemoFiles+C5j
                gDemoRandomSeeds[i] = level->randomSeed;
            }
        }

//loc_476DB:             // ; CODE XREF: readDemoFiles+59j readDemoFiles+69j ...
        uint16_t maxNumberOfBytesToRead = kMaxDemoInputSteps + 1; // 48649
        maxNumberOfBytesToRead -= gDemoCurrentInputIndex;

        if (maxNumberOfBytesToRead > kMaxDemoInputSteps + 1) // weird way of checking if gDemoCurrentInputIndex < 0 ????
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
            numberOfDemoBytesRead = fileReadBytes(&gDemos.demoData[gDemoCurrentInputIndex], maxNumberOfBytesToRead, file);

            if (numberOfDemoBytesRead == 0)
            {
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
        gDemos.demoData[gDemoCurrentInputIndex] = gDemos.demoData[gDemoCurrentInputIndex] & 0x7F; // this removes the MSB from the levelNumber that was added in the speed fix mods
        int isZero = (gSelectedOriginalDemoLevelNumber == 0);
        gSelectedOriginalDemoLevelNumber = 0;
        if (isZero)
        {
            gDemos.demoData[gDemoCurrentInputIndex] = gDemos.demoData[gDemoCurrentInputIndex] | 0x80; // This sets the MSB?? maybe the "interpreter" later needs it
        }

//loc_47743:             // ; CODE XREF: readDemoFiles+134j
        uint16_t demoLastByteIndex = gDemoCurrentInputIndex + numberOfDemoBytesRead - 1;
        // cx = bx; // bx here has the value of gDemoCurrentInputIndex
        // bx += numberOfDemoBytesRead; // ax here has the number of bytes read regarding the level itself (levelNumber + inputSteps)
        // push(ds);
        // push(es);
        // pop(ds);
        // assume ds:nothing
        // bx--;
        if (demoLastByteIndex == 0xFFFF // this would mean bx was 0. is this possible?
            || numberOfDemoBytesRead <= 1 // this means the demo is empty (only has levelNumber or nothing)
            || gDemos.demoData[demoLastByteIndex] != 0xFF)
        {
//loc_4775A:             // ; CODE XREF: readDemoFiles+145j
           // ; readDemoFiles+14Aj
            if (demoLastByteIndex < sizeof(BaseDemo))
            {
                numberOfDemoBytesRead++;
                gDemos.demoData[demoLastByteIndex + 1] = 0xFF;
            }
        }

//loc_47765:             // ; CODE XREF: readDemoFiles+14Fj
                   // ; readDemoFiles+155j
        gDemos.demoFirstIndices[i] = gDemoCurrentInputIndex;
        gDemoCurrentInputIndex += numberOfDemoBytesRead;
    }

    return kNumberOfDemos;
}

void openCreditsBlock() // proc near      ; CODE XREF: start+2E9p
{
    static const int kEdgeWidth = 13;
    static const int kEdgeHeight = 148;
    static const int kEdgeStep = 4;
    static const int kEdgeTopY = 26;
    static const int kNumberOfFrames = 60;

    const uint32_t kAnimationDuration = kNumberOfFrames * 1000 / 70; // ~429 ms

    uint32_t animationTime = 0;

    static const int kInitialLeftEdgeX = 147;
    const int kInitialRightEdgeX = kInitialLeftEdgeX + kEdgeWidth + 1;

    const int kEdgeAnimationDistance = (kEdgeStep * kNumberOfFrames) / 2 + 1;

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

        videoLoop();
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
    readAndRenderTitle1Dat();

//loc_4792E:              //; CODE XREF: loadScreen2+76j
    ColorPalette title1DatPalette;
    convertPaletteDataToPalette(gTitle1PaletteData, title1DatPalette);
    setPalette(title1DatPalette);
    videoLoop();

    readTitle2Dat();
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

    FILE *file = openWritableFile(gLevelLstFilename, "rb");
    if (file == NULL)
    {
//errorOpeningLevelLst:             // ; CODE XREF: readLevelsLst+8j
        FILE *file = openReadonlyFile(gLevelsDatFilename, "rb");
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

            int seekOffset = 0x5A6 + i * kLevelDataLength;

            fseek(file, seekOffset, SEEK_SET); // position 1446
            size_t bytes = fileReadBytes(gLevelListData + i * kListLevelNameLength + 4, kLevelNameLength - 1, file);

            if (bytes < kLevelNameLength - 1)
            {
                fclose(file);
                exitWithError("Error reading LEVELS.DAT\n");
            }
        }
        if (fclose(file) != 0)
        {
            exitWithError("Error closing LEVELS.DAT\n");
        }

        if (gShouldRecreateLevelLstIfNeeded == 0)
        {
            return;
        }

//    loc_47D35:             // ; CODE XREF: readLevelsLst+95j
        file = openWritableFile(gLevelLstFilename, "wb");
        if (file == NULL)
        {
            exitWithError("Error opening %s\n", gLevelLstFilename);
        }

//    writeLevelLstData:             // ; CODE XREF: readLevelsLst+A5j
        size_t bytes = fileWriteBytes(gLevelListData, kLevelListDataLength, file);
        if (bytes < kLevelListDataLength)
        {
            exitWithError("Error writing %s\n", gLevelLstFilename);
        }

//    loc_47D5B:             // ; CODE XREF: readLevelsLst+BBj
        if (fclose(file) != 0)
        {
            exitWithError("Error closing %s\n", gLevelLstFilename);
        }
        return;
    }

//successOpeningLevelLst:             // ; CODE XREF: readLevelsLst+Aj
    size_t bytes = fileReadBytes(gLevelListData, kLevelListDataLength, file);
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

void readPlayersLst() //  proc near       ; CODE XREF: readEverything+1Bp
                    // ; handleFloppyDiskButtonClick+149p
{
    if (gIsForcedCheatMode != 0)
    {
        return;
    }

    for (int i = 0; i < kNumberOfPlayers; ++i)
    {
        strcpy(gPlayerListData[i].name, "--------");
    }

    FILE *file = openWritableFile(gPlayerLstFilename, "rb");
    if (file == NULL)
    {
        return;
    }

    size_t bytes = fileReadBytes(gPlayerListData, sizeof(gPlayerListData), file);
    if (bytes == 0)
    {
        return;
    }

    fclose(file);
}

void readHallfameLst() // proc near       ; CODE XREF: readEverything+18p
                    // ; handleFloppyDiskButtonClick+146p
{
    if (gIsForcedCheatMode != 0)
    {
        return;
    }

    FILE *file = openWritableFile(gHallfameLstFilename, "rb");
    if (file == NULL)
    {
        return;
    }

    size_t bytes = fileReadBytes(gHallOfFameData, sizeof(gHallOfFameData), file);
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
    while (isAnyKeyPressed());

    uint16_t mouseButtonsStatus = 0;

    do
    {
//mouseIsClicked:              ; CODE XREF: waitForKeyMouseOrJoystick+1Ej
        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
    }
    while (mouseButtonsStatus != 0);

    for (int i = 0; i < 4200; ++i)
    {
//loc_47EC6:              ; CODE XREF: waitForKeyMouseOrJoystick+57j
        videoLoop();

        getMouseStatus(NULL, NULL, &mouseButtonsStatus);
        int9handler(0);
        updateUserInput();

        if (mouseButtonsStatus != 0)
        {
            break;
        }

        if (isAnyKeyPressed())
        {
            break;
        }

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
    else if (isAnyKeyPressed())
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
        }
        while (isAnyKeyPressed());
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

            int9handler(1);
            updateUserInput();
        }
        while (gCurrentUserInput > kUserInputSpaceAndDirectionOffset);
    }
}

void updateZonkTiles(int16_t position) //   proc near       ; DATA XREF: data:160Co
{
    // 01ED:132D

    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *belowLeftTile = &gCurrentLevelState[position + kLevelWidth - 1];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeZonk) // cmp byte ptr leveldata[si], 1
    {
        return;
    }

//loc_47F98:              ; CODE XREF: movefun+5j
    uint8_t shouldSkipFirstPartOfLoop = 0;

    if (currentTile->tile != LevelTileTypeZonk
        || currentTile->state != 0)
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
        if (belowTile->tile == LevelTileTypeSpace && belowTile->state == 0)
        {
//loc_47FF4:              ; CODE XREF: movefun+23j
            currentTile->state = 0x40;
            shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48035"
        }
        else
        {
            // Check if below the zonk is another object that could be used to slide down left or right
            if (belowTile->state != 0
                || (belowTile->tile != LevelTileTypeZonk
                    && belowTile->tile != LevelTileTypeInfotron
                    && belowTile->tile != LevelTileTypeChip))
            {
                return;
            }

//loc_47FC5:              ; CODE XREF: movefun+28j
//                ; movefun+2Dj ...
            // Check if it can fall to the left side...
            if ((belowLeftTile->tile == LevelTileTypeSpace && belowLeftTile->state == 0)
                || (belowLeftTile->tile == 0x88 && belowLeftTile->state == 0x88)
                || (belowLeftTile->tile == 0xAA && belowLeftTile->state == 0xAA))
            {
//loc_47FFB:              ; CODE XREF: movefun+3Aj
//                ; movefun+42j ...
                // ...but only if the left tile is empty
                if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
                {
//loc_48004:              ; CODE XREF: movefun+70j
                    currentTile->state = 0x50;
                    leftTile->state = 0x88;
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
            if ((belowRightTile->state != 0 || belowRightTile->tile != LevelTileTypeSpace)
                && (belowRightTile->state != 0x88 || belowRightTile->tile != 0x88)
                && (belowRightTile->state != 0xAA || belowRightTile->tile != 0xAA))
            {
                return;
            }
            else
            {
//loc_48011:              ; CODE XREF: movefun+51j
//                ; movefun+59j ...
                // Only if the right tile is empty or... other circumstances?
                if ((rightTile->state != 0 || rightTile->tile != LevelTileTypeSpace)
                    && ((rightTile->state != 0x99 || rightTile->tile != 0x99)
                        || (aboveRightTile->state != 0 || aboveRightTile->tile != LevelTileTypeZonk)))
                {
                    return;
                }

//loc_48028:              ; CODE XREF: movefun+86j
//                ; movefun+95j
                currentTile->state = 0x60;
                rightTile->state = 0x88;
                rightTile->tile = 0x88;
            }
        }

        shouldSkipFirstPartOfLoop = 0; // don't skip the first part in the next iteration

//loc_48035:              ; CODE XREF: movefun+11j
//                ; movefun+69j ...
        uint8_t state = currentTile->state;
        uint8_t stateType = state & 0xF0;

        if (stateType != 0x10) // 16
        {
//loc_48045:              ; CODE XREF: movefun+B1j
            if (stateType == 0x20) // 32
            {
//loc_48212:              ; CODE XREF: movefun+B9j
                // 01ED:15AF
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 1294h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideLeftAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                // 01ED:15D6
                state = currentTile->state; // mov bl, [si+1835h]
                state++;

                if (state == 0x24) // 36
                {
                    rightTile->state = 0xAA;
                    rightTile->tile = 0xAA;
                }
//loc_4824A:              ; CODE XREF: movefun+2B2j
                if (state == 0x26) // 38
                {
                    currentTile->state = state;
                    handleZonkStateAfterFallingOneTile(position + 1);
                    return;
                }
//loc_4825D:              ; CODE XREF: movefun+2BDj
                else if (state < 0x28) // 40
                {
                    currentTile->state = state;
                    return;
                }
//loc_48267:              ; CODE XREF: movefun+2D0j
                else
                {
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->state = 0x10;
                    belowTile->tile = LevelTileTypeZonk;
                    return;
                }
            }
//loc_4804C:              ; CODE XREF: movefun+B7j
            else if (stateType == 0x30) // 48
            {
//loc_48277:              ; CODE XREF: movefun+C0j
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12A4h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideRightAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                state = currentTile->state;
                state++;
                if (state == 0x34) // 52
                {
                    leftTile->state = 0xAA;
                    leftTile->tile = 0xAA;
                }
//loc_482AF:              ; CODE XREF: movefun+317j
                if (state == 0x36) // 54
                {
                    currentTile->state = state;
                    handleZonkStateAfterFallingOneTile(position - 1); // left tile
                    return;
                }
//loc_482C2:              ; CODE XREF: movefun+322j
                else if (state < 0x38) // 54
                {
                    currentTile->state = state;
                    return;
                }
//loc_482CC:              ; CODE XREF: movefun+335j
                else
                {
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->state = 0x10;
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
            else if (stateType == 0x40) // 64
            {
//loc_482DC:              ; CODE XREF: movefun+CFj
                state++;
                if (state < 0x42) // 66
                {
                    currentTile->state = state;
                    return;
                }
//loc_482E8:              ; CODE XREF: movefun+351j
                else if (belowTile->tile != LevelTileTypeSpace || belowTile->state != 0) // cmp word ptr [si+18ACh], 0
                {
                    state--;
                    currentTile->state = state;
                    return;
                }
//loc_482F6:              ; CODE XREF: movefun+35Dj
                else
                {
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    belowTile->state = 0x10;
                    belowTile->tile = LevelTileTypeZonk;

                    return;
                }
            }
//loc_48062:              ; CODE XREF: movefun+CDj
            else if (stateType == 0x50) // Zonk sliding left
            {
//loc_4830A:              ; CODE XREF: movefun+D6j
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 1294h
                // mov si, [bx+si]
                Point frameCoordinates = kZonkSlideLeftAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                state = currentTile->state;
                state++;

                if (state < 0x52) // 82
                {
                    currentTile->state = state;
                    return;
                }
//loc_48341:              ; CODE XREF: movefun+3AAj
                else if (belowLeftTile->state != 0 || belowLeftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
                {
//loc_48371:              ; CODE XREF: movefun+3B6j
    //                ; movefun+3C5j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else if ((leftTile->state != 0 || leftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1832h], 0
                         && (leftTile->state != 0x88 || leftTile->tile != 0x88)) // cmp word ptr [si+1832h], 8888h
                {
//loc_48371:              ; CODE XREF: movefun+3B6j
    //                ; movefun+3C5j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else
                {
//loc_48357:              ; CODE XREF: movefun+3BDj
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    leftTile->state = 0x22;
                    leftTile->tile = LevelTileTypeZonk;
                    belowLeftTile->state = 0xFF;
                    belowLeftTile->tile = 0xFF;
                    return;
                }
            }
//loc_48069:              ; CODE XREF: movefun+D4j
            else if (stateType == 0x60) // 96
            {
//loc_48378:              ; CODE XREF: movefun+DDj
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12A4h
                Point frameCoordinates = kZonkSlideRightAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                state = currentTile->state;
                state++;

                if (state < 0x62) // 98
                {
                    currentTile->state = state;
                    return;
                }
//loc_483AF:              ; CODE XREF: movefun+418j
                else if (belowRightTile->state != 0 || belowRightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
                {
//loc_483DF:              ; CODE XREF: movefun+424j
    //                ; movefun+433j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else if ((rightTile->state != 0 || rightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1836h], 0
                         && (rightTile->state != 0x88 || rightTile->tile != 0x88)) // cmp word ptr [si+1836h], 8888h
                {
//loc_483DF:              ; CODE XREF: movefun+424j
    //                ; movefun+433j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else
                {
//loc_483C5:              ; CODE XREF: movefun+42Bj
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    rightTile->state = 0x32;
                    rightTile->tile = LevelTileTypeZonk;
                    belowRightTile->state = 0xFF;
                    belowRightTile->tile = 0xFF;
                    return;
                }
            }
//loc_48070:              ; CODE XREF: movefun+DBj
            else if (stateType == 0x70) // 112
            {
//loc_483E6:              ; CODE XREF: movefun+E4j
                if ((belowTile->state != 0 || belowTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18ACh], 0
                    && (belowTile->state != 0x99 && belowTile->tile != 0x99)) // cmp word ptr [si+18ACh], 9999h
                {
                    return;
                }

//loc_483F6:              ; CODE XREF: movefun+45Bj
//                ; movefun+463j
                currentTile->state = 0xFF;
                currentTile->tile = 0xFF;

                // Move down and update tiles
                position += kLevelWidth;

                currentTile = &gCurrentLevelState[position];
                belowTile = &gCurrentLevelState[position + kLevelWidth];
                belowLeftTile = &gCurrentLevelState[position + kLevelWidth - 1];
                leftTile = &gCurrentLevelState[position - 1];
                belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];
                rightTile = &gCurrentLevelState[position + 1];
                aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];

                currentTile->state = 0x10;
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
        uint8_t somePositionThing = state;
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

        uint8_t newState = currentTile->state;
        newState++;
        if (newState == 0x16) // 22
        {
            currentTile->state = newState;
            handleZonkStateAfterFallingOneTile(position - kLevelWidth); // Tile above
            return;
        }
//loc_480BB:              ; CODE XREF: movefun+11Bj
        else if (newState < 0x18) // 24
        {
            currentTile->state = newState;
            return;
        }

//loc_480C5:              ; CODE XREF: movefun+12Ej
        // This part handles what to do when the zonk finished falling 1 tile
        // 01ED:1462
        currentTile->state = 0;
        if (gAreZonksFrozen == 2)
        {
            return;
        }

//loc_480D2:              ; CODE XREF: movefun+13Fj
        if ((belowTile->tile == LevelTileTypeSpace && belowTile->state == 0) // cmp word ptr [si+18ACh], 0
            || (belowTile->tile == 0x99 && belowTile->state == 0x99)) // cmp word ptr [si+18ACh], 9999h
        {
//loc_4816D:              ; CODE XREF: movefun+149j
//                ; movefun+154j
            currentTile->state = 0x70;
            currentTile->tile = LevelTileTypeZonk;
            belowTile->state = 0x99;
            belowTile->tile = 0x99;
            return;
        }

//loc_480E7:              ; CODE XREF: movefun+152j
        if (belowTile->tile == LevelTileTypeMurphy) // cmp byte ptr [si+18ACh], 3
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
        if (belowTile->tile == 0xBB && belowTile->state == 0x2) // cmp word ptr [si+18ACh], 2BBh
        {
            shouldSkipTo_loc_481C6 = 1;
            break;
        }

//loc_48106:              ; CODE XREF: movefun+171j
        if (belowTile->tile == 0xBB && belowTile->state == 0x4) // cmp word ptr [si+18ACh], 4BBh
        {
//loc_481E2:              ; CODE XREF: movefun+17Ej
            if (belowRightTile->tile == LevelTileTypeElectron) // cmp byte ptr [si+18AEh], 18h
            {
                belowTile->tile = LevelTileTypeElectron;
                belowTile->state = 0;
            }
//loc_481EF:              ; CODE XREF: movefun+257j
            if (belowRightTile->tile != LevelTileTypeExplosion) // cmp byte ptr [si+18AEh], 1Fh
            {
                belowRightTile->tile = LevelTileTypeSpace;
                belowRightTile->state = 0;
            }
//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
            detonateBigExplosion(position + kLevelWidth); // Tile below
            return;
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
        if (belowTile->tile == LevelTileTypeOrangeDisk && belowTile->state == 0) // cmp word ptr [si+18ACh], 8
        {
//loc_48205:              ; CODE XREF: movefun+192j
            gExplosionTimers[position + kLevelWidth] = 6;
            return;
        }

//loc_48125:              ; CODE XREF: movefun+190j
        playFallSound();
        if ((belowTile->tile != LevelTileTypeZonk || belowTile->state != 0) // cmp word ptr [si+18ACh], 1
            && (belowTile->tile != LevelTileTypeInfotron || belowTile->state != 0) // cmp word ptr [si+18ACh], 4
            && (belowTile->tile != LevelTileTypeChip || belowTile->state != 0)) // cmp word ptr [si+18ACh], 5
        {
            return;
        }

//loc_4813E:              ; CODE XREF: movefun+19Dj
//                ; movefun+1A4j ...
        if ((belowLeftTile->state == 0 && belowLeftTile->tile == LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
            || (belowLeftTile->state == 0x88 && belowLeftTile->tile == 0x88) // cmp word ptr [si+18AAh], 8888h
            || (belowLeftTile->state == 0xAA && belowLeftTile->tile == 0xAA)) // cmp word ptr [si+18AAh], 0AAAAh
        {
//loc_4817A:              ; CODE XREF: movefun+1B3j
//                ; movefun+1BBj ...
            if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace) //cmp word ptr [si+1832h], 0
            {
//loc_48184:              ; CODE XREF: movefun+1EFj
                currentTile->state = 0x50;
                leftTile->state = 0x88;
                leftTile->tile = 0x88;
                return;
            }
            else
            {
                continue;
            }
        }
        if ((belowRightTile->state == 0 && belowRightTile->tile == LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
            || (belowRightTile->state == 0x88 && belowRightTile->tile == 0x88) // cmp word ptr [si+18AEh], 8888h
            || (belowRightTile->state == 0xAA && belowRightTile->tile == 0xAA)) // cmp word ptr [si+18AEh], 0AAAAh
        {
//loc_48190:              ; CODE XREF: movefun+1CAj
//                ; movefun+1D2j ...
            if (rightTile->tile == LevelTileTypeSpace && rightTile->state == 0) //cmp word ptr [si+1836h], 0
            {
//loc_48198:              ; CODE XREF: movefun+205j
                currentTile->state = 0x60;
                rightTile->state = 0x88;
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
        if (belowTile->state == 0xE
            || belowTile->state == 0xF
            || belowTile->state == 0x28
            || belowTile->state == 0x29
            || belowTile->state == 0x25
            || belowTile->state == 0x26)
        {
            return;
        }
    }

//loc_481C6:              ; CODE XREF: movefun+173j
    if (belowLeftTile->tile == LevelTileTypeElectron) // cmp byte ptr [si+18AAh], 18h
    {
        belowTile->tile = LevelTileTypeElectron;
        belowTile->state = 0;
    }
//loc_481D3:              ; CODE XREF: movefun+23Bj
    if (belowLeftTile->tile != LevelTileTypeExplosion) // cmp byte ptr [si+18AAh], 1Fh
    {
        belowLeftTile->tile = LevelTileTypeSpace;
        belowLeftTile->state = 0;
    }

//loc_481FE:              ; CODE XREF: movefun+168j
//                ; movefun+188j ...
    detonateBigExplosion(position + kLevelWidth); // Tile below
}

void updateInfotronTiles(int16_t position) // movefun2  proc near       ; DATA XREF: data:1612o
{
    // 01ED:17A5

    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *belowLeftTile = &gCurrentLevelState[position + kLevelWidth - 1];
    StatefulLevelTile *belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    if (currentTile->tile != LevelTileTypeInfotron)
    {
        return;
    }

//loc_48410:              ; CODE XREF: movefun2+5j
    uint8_t shouldSkipFirstPartOfLoop = 0;

    if (currentTile->state != 0 || currentTile->tile != LevelTileTypeInfotron)
    {
        shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48495"
    }
    else
    {
//loc_4841B:              ; CODE XREF: movefun2+Fj
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_48463:              ; CODE XREF: movefun2+1Aj
            currentTile->state = 0x40;
            shouldSkipFirstPartOfLoop = 1; // used to emulate "jmp loc_48495"
        }
        else
        {
            if ((belowTile->state != 0 || belowTile->tile != LevelTileTypeZonk)
                && (belowTile->state != 0 || belowTile->tile != LevelTileTypeInfotron)
                && (belowTile->state != 0 || belowTile->tile != LevelTileTypeChip))
            {
                return;
            }

//loc_48434:              ; CODE XREF: movefun2+1Fj
//                ; movefun2+24j ...
            if ((belowLeftTile->state == 0 && belowLeftTile->tile == LevelTileTypeSpace)
                || (belowLeftTile->state == 0x88 && belowLeftTile->tile == 0x88)
                || (belowLeftTile->state == 0xAA && belowLeftTile->tile == 0xAA))
            {
//loc_4846A:              ; CODE XREF: movefun2+31j
//                ; movefun2+39j ...
                if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
                {
//loc_48473:              ; CODE XREF: movefun2+67j
                    currentTile->state = 0x50;
                    leftTile->state = 0x88;
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
            if ((belowRightTile->state != 0 || belowRightTile->tile != LevelTileTypeSpace)
                && (belowRightTile->state != 0x88 || belowRightTile->tile != 0x88)
                && (belowRightTile->state != 0xAA || belowRightTile->tile != 0xAA))
            {
                return;
            }

//loc_48480:              ; CODE XREF: movefun2+48j
//                ; movefun2+50j ...
            if (rightTile->state != 0 || rightTile->tile != LevelTileTypeSpace)
            {
                return;
            }

//loc_48488:              ; CODE XREF: movefun2+7Dj
            currentTile->state = 0x60;
            rightTile->state = 0x88;
            rightTile->tile = 0x88;
        }

        shouldSkipFirstPartOfLoop = 0;

//loc_48495:              ; CODE XREF: movefun2+11j
//                ; movefun2+60j ...
        uint8_t state = currentTile->state;
        uint8_t stateType = state & 0xF0;

        if (stateType != 0x10)
        {
//loc_484A5:              ; CODE XREF: movefun2+99j
            if (stateType == 0x20)
            {
//loc_4861B:              ; CODE XREF: movefun2+A1j
                uint8_t stateFrame = state & 0x7; // module 8?

                // mov si, 12B6h
                Point frameCoordinates = kInfotronSlideLeftAnimationFrameCoordinates[stateFrame];

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

                state = currentTile->state;
                state++;
                if (state == 0x24) // 36
                {
                    rightTile->state = 0xAA;
                    rightTile->tile = 0xAA;
                }
//loc_48653:              ; CODE XREF: movefun2+243j
                if (state == 0x26) // 38
                {
                    currentTile->state = state;
                    handleInfotronStateAfterFallingOneTile(position + 1);
                    return;
                }
//loc_48666:              ; CODE XREF: movefun2+24Ej
                else if (state < 0x28) // 40
                {
                    currentTile->state = state;
                    return;
                }
//loc_48670:              ; CODE XREF: movefun2+261j
                else
                {
                    currentTile->state = 0x70;
                    currentTile->tile = LevelTileTypeInfotron;
                    return;
                }
            }
//loc_484AC:              ; CODE XREF: movefun2+9Fj
            else if (stateType == 0x30)
            {
//loc_48677:              ; CODE XREF: movefun2+A8j
                uint8_t stateFrame = state & 0x7;

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12C6h
                Point frameCoordinates = kInfotronSlideRightAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                state = currentTile->state;
                state++;
                if (state == 0x34) // 52
                {
                    leftTile->state = 0xAA;
                    leftTile->tile = 0xAA;
                }
//loc_486AF:              ; CODE XREF: movefun2+29Fj
                if (state == 0x36) // 54
                {
                    currentTile->state = state;
                    handleInfotronStateAfterFallingOneTile(position - 1); // left tile
                }
//loc_486C1:              ; CODE XREF: movefun2+2AAj
                if (state < 0x38) // 54
                {
                    currentTile->state = state;
                    return;
                }
                else
                {
                    currentTile->state = 0x70;
                    currentTile->tile = LevelTileTypeInfotron;
                    return;
                }
            }
//loc_484B3:              ; CODE XREF: movefun2+A6j
            else if (stateType == 0x40)
            {
//loc_486D2:              ; CODE XREF: movefun2+AFj
                state++;
                if (state < 0x42)
                {
                    currentTile->state = state;
                    return;
                }

//loc_486DE:              ; CODE XREF: movefun2+2CFj
                if (belowTile->state != 0 || belowTile->tile != LevelTileTypeSpace)
                {
                    state--;
                    currentTile->state = state;
                    return;
                }

//loc_486EC:              ; CODE XREF: movefun2+2DBj
                currentTile->state = 0xFF;
                currentTile->tile = 0xFF;
                belowTile->state = 0x10;
                belowTile->tile = LevelTileTypeInfotron;
                return;
            }
//loc_484BA:              ; CODE XREF: movefun2+ADj
            else if (stateType == 0x50)
            {
//loc_48700:              ; CODE XREF: movefun2+B6j
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = ((position - 1) % kLevelWidth);
                uint8_t tileY = ((position - 1) / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12B6h
                Point frameCoordinates = kInfotronSlideLeftAnimationFrameCoordinates[stateFrame];
                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);
                state = currentTile->state;
                state++;

                if (state < 0x52) // 82
                {
                    currentTile->state = state;
                    return;
                }
//loc_48737:              ; CODE XREF: movefun2+328j
                else if (belowLeftTile->state != 0 || belowLeftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AAh], 0
                {
//loc_48767:              ; CODE XREF: movefun2+334j
//                ; movefun2+343j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else if ((leftTile->state != 0 || leftTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1832h], 0
                         && (leftTile->state != 0x88 || leftTile->tile != 0x88)) // cmp word ptr [si+1832h], 8888h
                {
//loc_48767:              ; CODE XREF: movefun2+334j
//                ; movefun2+343j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else
                {
//loc_4874D:              ; CODE XREF: movefun2+33Bj
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    leftTile->state = 0x22;
                    leftTile->tile = LevelTileTypeInfotron;
                    belowLeftTile->state = 0x99;
                    belowLeftTile->tile = 0x99;
                    return;
                }
            }
//loc_484C1:              ; CODE XREF: movefun2+B4j
            else if (stateType == 0x60)
            {
//loc_4876E:              ; CODE XREF: movefun2+BDj
                uint8_t stateFrame = state & 0x7; // module 8?

                uint8_t tileX = (position % kLevelWidth);
                uint8_t tileY = (position / kLevelWidth);

                uint16_t dstX = tileX * kTileSize;
                uint16_t dstY = tileY * kTileSize;

                // mov si, 12C6h
                Point frameCoordinates = kInfotronSlideRightAnimationFrameCoordinates[stateFrame];

                drawMovingSpriteFrameInLevel(frameCoordinates.x,
                                             frameCoordinates.y,
                                             kTileSize * 2,
                                             kTileSize,
                                             dstX, dstY);

                state = currentTile->state;
                state++;

                if (state < 0x62) // 98
                {
                    currentTile->state = state;
                    return;
                }
//loc_487A5:              ; CODE XREF: movefun2+396j
                else if (belowRightTile->state != 0 || belowRightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+18AEh], 0
                {
//loc_487D5:              ; CODE XREF: movefun2+3A2j
//                ; movefun2+3B1j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else if ((rightTile->state != 0 || rightTile->tile != LevelTileTypeSpace) // cmp word ptr [si+1836h], 0
                         && (rightTile->state != 0x88 || rightTile->tile != 0x88)) // cmp word ptr [si+1836h], 8888h
                {
//loc_487D5:              ; CODE XREF: movefun2+3A2j
//                ; movefun2+3B1j
                    state--;
                    currentTile->state = state;
                    return;
                }
                else
                {
//loc_487BB:              ; CODE XREF: movefun2+3A9j
                    currentTile->state = 0xFF;
                    currentTile->tile = 0xFF;
                    rightTile->state = 0x32;
                    rightTile->tile = LevelTileTypeInfotron;
                    belowRightTile->state = 0x99;
                    belowRightTile->tile = 0x99;
                    return;
                }
            }
//loc_484C8:              ; CODE XREF: movefun2+BBj
            else if (stateType == 0x70)
            {
//loc_487DC:              ; CODE XREF: movefun2:loc_484CCj
                if ((belowTile->state != 0 || belowTile->tile != LevelTileTypeSpace)
                    && (belowTile->state != 0x99 || belowTile->tile != 0x99))
                {
                    return;
                }

//loc_487EC:              ; CODE XREF: movefun2+3D9j
//                    ; movefun2+3E1j
                currentTile->state = 0xFF;
                currentTile->tile = 0xFF;

                // add si, 78h ; 'x'
                position += kLevelWidth;

                currentTile = &gCurrentLevelState[position];
                belowTile = &gCurrentLevelState[position + kLevelWidth];
                belowLeftTile = &gCurrentLevelState[position + kLevelWidth - 1];
                leftTile = &gCurrentLevelState[position - 1];
                belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];
                rightTile = &gCurrentLevelState[position + 1];

                currentTile->state = 0x10;
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
        uint8_t somePositionThing = state;
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

        uint8_t newState = currentTile->state;
        newState++;
        if (newState == 0x16) // 22
        {
            currentTile->state = newState;
            handleInfotronStateAfterFallingOneTile(position - kLevelWidth); // Tile above
            return;
        }
//loc_48513:              ; CODE XREF: movefun2+FBj
        else if (newState < 0x18) // 24
        {
            currentTile->state = newState;
            return;
        }

//loc_4851D:              ; CODE XREF: movefun2+10Ej
        // This part handles what to do when the Infotron finished falling 1 tile
        currentTile->state = 0;

        if ((belowTile->tile == LevelTileTypeSpace && belowTile->state == 0) // cmp word ptr [si+18ACh], 0
            || (belowTile->tile == 0x99 && belowTile->state == 0x99)) // cmp word ptr [si+18ACh], 9999h
        {
//loc_485BB:              ; CODE XREF: movefun2+121j
//                ; movefun2+12Cj
            currentTile->state = 0x70;
            currentTile->tile = LevelTileTypeInfotron;
            belowTile->state = 0x99;
            belowTile->tile = 0x99;
            return;
        }

//loc_48537:              ; CODE XREF: movefun2+12Aj
        if (belowTile->tile == LevelTileTypeMurphy) // cmp byte ptr [si+18ACh], 3
        {
//loc_485F2:              ; CODE XREF: movefun2+136j
            if (belowTile->state == 0xE
                || belowTile->state == 0xF
                || belowTile->state == 0x28
                || belowTile->state == 0x29
                || belowTile->state == 0x25
                || belowTile->state == 0x26)
            {
                return;
            }

//loc_48614:              ; CODE XREF: movefun2+140j
//                ; movefun2+14Aj ...
            detonateBigExplosion(position + kLevelWidth);
            return;
        }

//loc_48541:              ; CODE XREF: movefun2+134j
        if ((belowTile->tile == LevelTileTypeRedDisk && belowTile->state == 0) // cmp word ptr [si+18ACh], 14h
            || belowTile->tile == LevelTileTypeSnikSnak // cmp byte ptr [si+18ACh], 11h
            || belowTile->tile == LevelTileTypeElectron // cmp byte ptr [si+18ACh], 18h
            || (belowTile->tile == LevelTileTypeYellowDisk && belowTile->state == 0) // cmp word ptr [si+18ACh], 12h
            || (belowTile->tile == LevelTileTypeOrangeDisk && belowTile->state == 0)) // cmp word ptr [si+18ACh], 8
        {
//loc_48614:              ; CODE XREF: movefun2+140j
//                ; movefun2+14Aj ...
            detonateBigExplosion(position + kLevelWidth);
            return;
        }

//loc_48573:              ; CODE XREF: movefun2+166j
        playFallSound();
        if ((belowTile->tile != LevelTileTypeZonk || belowTile->state != 0) // cmp word ptr [si+18ACh], 1
            && (belowTile->tile != LevelTileTypeInfotron || belowTile->state != 0) // cmp word ptr [si+18ACh], 4
            && (belowTile->tile != LevelTileTypeChip || belowTile->state != 0)) // cmp word ptr [si+18ACh], 5
        {
            return;
        }

//loc_4858C:              ; CODE XREF: movefun2+173j
//                ; movefun2+17Aj ...
        if ((belowLeftTile->tile == LevelTileTypeSpace && belowLeftTile->state == 0) // cmp word ptr [si+18AAh], 0
            || (belowLeftTile->tile == 0x88 && belowLeftTile->state == 0x88) // cmp word ptr [si+18AAh], 8888h
            || (belowLeftTile->tile == 0xAA && belowLeftTile->state == 0xAA)) // cmp word ptr [si+18AAh], 0AAAAh
        {
//loc_485C8:              ; CODE XREF: movefun2+189j
//                ; movefun2+191j ...
            if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
            {
//loc_485D2:              ; CODE XREF: movefun2+1C5j
                currentTile->state = 0x50;
                leftTile->state = 0x88;
                leftTile->tile = 0x88;
                return;
            }
            else
            {
                continue; // jmp loc_4844B
            }
        }
        if ((belowRightTile->tile == LevelTileTypeSpace && belowRightTile->state == 0) // cmp word ptr [si+18AEh], 0
            || (belowRightTile->tile == 0x88 && belowRightTile->state == 0x88) // cmp word ptr [si+18AEh], 8888h
            || (belowRightTile->tile == 0xAA && belowRightTile->state == 0xAA)) // cmp word ptr [si+18AEh], 0AAAAh
        {
//loc_485DE:              ; CODE XREF: movefun2+1A0j
//                ; movefun2+1A8j ...
            if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
            {
//loc_485E6:              ; CODE XREF: movefun2+1DBj
                currentTile->state = 0x60;
                rightTile->state = 0x88;
                rightTile->tile = 0x88;
                return;
            }
            return;
        }
        return;
    }
    while (1);
}

void handleMurphyCollisionAfterMovement(int16_t position) // sub_487FE   proc near       ; CODE XREF: update?+E0Cp update?+E2Ap ...
{
    // 01ED:1B9B
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];
    StatefulLevelTile *aboveLeftTile = &gCurrentLevelState[position - kLevelWidth - 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->state = 0;
        currentTile->tile = LevelTileTypeSpace;
    }

//loc_4880B:              ; CODE XREF: handleMurphyCollisionAfterMovement+5j
    if ((aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
        || (aboveTile->state == 0x99 && aboveTile->tile == 0x99))
    {
//loc_48835:              ; CODE XREF: handleMurphyCollisionAfterMovement+12j
//                ; handleMurphyCollisionAfterMovement+1Aj
        if (aboveLeftTile->state == 0 && aboveLeftTile->tile == LevelTileTypeZonk)
        {
//loc_48852:              ; CODE XREF: handleMurphyCollisionAfterMovement+3Cj
            if ((leftTile->state == 0 && leftTile->tile == LevelTileTypeZonk)
                || (leftTile->state == 0 && leftTile->tile == LevelTileTypeInfotron)
                || (leftTile->state == 0 && leftTile->tile == LevelTileTypeChip))
            {
//loc_48869:              ; CODE XREF: handleMurphyCollisionAfterMovement+59j
//                ; handleMurphyCollisionAfterMovement+60j ...
                aboveLeftTile->state = 0x60;
                aboveTile->state = 0x88;
                aboveTile->tile = 0x88;
                return;
            }
        }
        else if (aboveLeftTile->state == 0 && aboveLeftTile->tile == LevelTileTypeInfotron)
        {
//loc_48897:              ; CODE XREF: handleMurphyCollisionAfterMovement+43j
            if ((leftTile->state == 0 && leftTile->tile == LevelTileTypeZonk)
                || (leftTile->state == 0 && leftTile->tile == LevelTileTypeInfotron)
                || (leftTile->state == 0 && leftTile->tile == LevelTileTypeChip))
            {
//loc_488AE:              ; CODE XREF: handleMurphyCollisionAfterMovement+9Ej
//                ; handleMurphyCollisionAfterMovement+A5j ...
                aboveLeftTile->state = 0x60;
                aboveTile->state = 0x88;
                aboveTile->tile = 0x88;
                return;
            }
        }
//loc_48843:              ; CODE XREF: handleMurphyCollisionAfterMovement+69j
//                ; handleMurphyCollisionAfterMovement+AEj
        if (aboveRightTile->state == 0 && aboveRightTile->tile == LevelTileTypeZonk)
        {
//loc_48875:              ; CODE XREF: handleMurphyCollisionAfterMovement+4Aj
            if ((rightTile->state == 0 && rightTile->tile == LevelTileTypeZonk)
                || (rightTile->state == 0 && rightTile->tile == LevelTileTypeInfotron)
                || (rightTile->state == 0 && rightTile->tile == LevelTileTypeChip))
            {
//loc_4888B:              ; CODE XREF: handleMurphyCollisionAfterMovement+7Cj
//                ; handleMurphyCollisionAfterMovement+83j ...
                aboveRightTile->state = 0x50;
                aboveTile->state = 0x88;
                aboveTile->tile = 0x88;
            }
        }
        else if (aboveRightTile->state == 0 && aboveRightTile->tile == LevelTileTypeInfotron)
        {
//loc_488BA:              ; CODE XREF: handleMurphyCollisionAfterMovement+51j
            if ((rightTile->state == 0 && rightTile->tile == LevelTileTypeZonk)
                || (rightTile->state == 0 && rightTile->tile == LevelTileTypeInfotron)
                || (rightTile->state == 0 && rightTile->tile == LevelTileTypeChip))
            {
//loc_488D0:              ; CODE XREF: handleMurphyCollisionAfterMovement+C1j
//                ; handleMurphyCollisionAfterMovement+C8j ...
                aboveRightTile->state = 0x50;
                aboveTile->state = 0x88;
                aboveTile->tile = 0x88;
            }
        }
    }
    else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeZonk)
    {
//loc_48829:              ; CODE XREF: handleMurphyCollisionAfterMovement+21j
        aboveTile->state = 0x40;
    }
    else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeInfotron)
    {
//loc_4882F:              ; CODE XREF: handleMurphyCollisionAfterMovement+28j
        aboveTile->state = 0x40;
    }
}

void handleZonkStateAfterFallingOneTile(int16_t position) // sub_488DC   proc near       ; CODE XREF: movefun+124p
                   // ; movefun+2C6p ...
{
    // 01ED:1C79
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *aboveAboveTile = &gCurrentLevelState[position - kLevelWidth * 2];
    StatefulLevelTile *aboveLeftTile = &gCurrentLevelState[position - kLevelWidth - 1];
    StatefulLevelTile *aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->state = 0;
        currentTile->tile = LevelTileTypeSpace;
    }

//loc_488E9:              ; CODE XREF: handleZonkStateAfterFallingOneTile+5j
    if (aboveTile->state != 0 || aboveTile->tile != LevelTileTypeSpace) // cmp word ptr [si+17BCh], 0
    {
        if (aboveTile->state != 0x99 || aboveTile->tile != 0x99) // cmp word ptr [si+17BCh], 9999h
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
    if (aboveLeftTile->state == 0 && aboveLeftTile->tile == LevelTileTypeZonk) // cmp word ptr [si+17BAh], 1
    {
//loc_48910:              ; CODE XREF: handleZonkStateAfterFallingOneTile+2Aj
        if (leftTile->state == 0
            && (leftTile->tile == LevelTileTypeZonk
                || leftTile->tile == LevelTileTypeInfotron
                || leftTile->tile == LevelTileTypeChip))
        {
//loc_48927:              ; CODE XREF: handleZonkStateAfterFallingOneTile+39j
//                ; handleZonkStateAfterFallingOneTile+40j ...
            // mov word ptr [si+17BAh], 6001h
            aboveLeftTile->state = 0x60;
            aboveLeftTile->tile = LevelTileTypeZonk;
            // mov word ptr [si+17BCh], 8888h
            aboveTile->state = 0x88;
            aboveTile->tile = 0x88;
            return;
        }
    }

//loc_48908:              ; CODE XREF: handleZonkStateAfterFallingOneTile+49j
    if (aboveRightTile->state == 0 && aboveRightTile->tile == LevelTileTypeZonk) // cmp word ptr [si+17BEh], 1
    {
//loc_48934:              ; CODE XREF: handleZonkStateAfterFallingOneTile+31j
        if (rightTile->state != 0
            || (rightTile->tile != LevelTileTypeZonk
                && rightTile->tile != LevelTileTypeInfotron
                && rightTile->tile != LevelTileTypeChip))
        {
            return;
        }

//loc_4894A:              ; CODE XREF: handleZonkStateAfterFallingOneTile+5Dj
//                ; handleZonkStateAfterFallingOneTile+64j ...
        // mov word ptr [si+17BEh], 5001h
        aboveRightTile->state = 0x50;
        aboveRightTile->tile = LevelTileTypeZonk;
        // mov word ptr [si+17BCh], 8888h
        aboveTile->state = 0x88;
        aboveTile->tile = 0x88;
        return;
    }
}

void handleInfotronStateAfterFallingOneTile(int16_t position) // sub_48957   proc near       ; CODE XREF: movefun2+104p
//                    ; movefun2+257p ...
{
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *aboveAboveTile = &gCurrentLevelState[position - kLevelWidth * 2];
    StatefulLevelTile *aboveLeftTile = &gCurrentLevelState[position - kLevelWidth - 1];
    StatefulLevelTile *aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        currentTile->state = 0;
        currentTile->tile = LevelTileTypeSpace;
    }
//loc_48964:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+5j
    if (aboveTile->state != 0 || aboveTile->tile != LevelTileTypeSpace)
    {
        if (aboveTile->state == 0x99 && aboveTile->tile == 0x99)
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
    if (aboveLeftTile->state == 0 && aboveLeftTile->tile == LevelTileTypeInfotron)
    {
//loc_4898B:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+2Aj
        if ((leftTile->state == 0 && leftTile->tile == LevelTileTypeZonk)
            || (leftTile->state == 0 && leftTile->tile == LevelTileTypeInfotron)
            || (leftTile->state == 0 && leftTile->tile == LevelTileTypeChip))
        {
//loc_489A2:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+39j
//                ; handleInfotronStateAfterFallingOneTile+40j ...
            aboveLeftTile->state = 0x60;
            aboveLeftTile->tile = LevelTileTypeInfotron;
            aboveTile->state = 0x88;
            aboveTile->tile = 0x88;
            return;
        }
    }

//loc_48983:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+49j
    if (aboveRightTile->state != 0 || aboveRightTile->tile != LevelTileTypeInfotron)
    {
        return;
    }

//loc_489AF:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+31j
    if ((rightTile->state != 0 || rightTile->tile != LevelTileTypeZonk)
        && (rightTile->state != 0 || rightTile->tile != LevelTileTypeInfotron)
        && (rightTile->state != 0 || rightTile->tile != LevelTileTypeChip))
    {
        return;
    }

//loc_489C5:              ; CODE XREF: handleInfotronStateAfterFallingOneTile+5Dj
//                ; handleInfotronStateAfterFallingOneTile+64j ...
    aboveRightTile->state = 0x50;
    aboveRightTile->tile = LevelTileTypeInfotron;
    aboveTile->state = 0x88;
    aboveTile->tile = 0x88;
}

void initializeGameInfo() // sub_48A20   proc near       ; CODE XREF: start+32Fp
                // ; runLevel:notFunctionKeyp ...
{
    // 01ED:1DBD
    // word_510BC = gMurphyTileX;
    // word_510BE = gMurphyTileY;
    gIsMurphyLookingLeft = 0;
    gShouldKillMurphy = 0;
    gShouldExitLevel = 0;
    gQuitLevelCountdown = 0;
    gNumberOfRemainingRedDisks = 0;
    gAdditionalInfoInGamePanelFrameCounter = 0;
    gMurphyYawnAndSleepCounter = 0;
    gLastDrawnMinutesAndSeconds = 0xFFFF;
    gLastDrawnHours = 0xFF; // 255
    gIsGameRunning = 1;
    gAuxGameSeconds20msAccumulator = 0;
    gGameSeconds = 0;
    gGameMinutes = 0;
    gGameHours = 0;
    gIsExplosionStarted = 0;
    gTerminalMaxFramesToNextScroll = 0x7F; // 127
    gAreYellowDisksDetonated = 0;
    gFrameCounter = 0;
//    mov byte ptr word_510C1, 1
//    mov byte ptr word_510C1+1, 0
    gShouldShowGamePanel = 1;
    gCurrentPanelHeight = kPanelBitmapHeight;
    gToggleGamePanelKeyAutoRepeatCounter = 0;
    gAreEnemiesFrozen = 0;
    gIsMurphyGoingThroughPortal &= 0xFF00; // mov byte ptr gIsMurphyGoingThroughPortal, 0
    gPlantedRedDiskCountdown = 0;
    gPlantedRedDiskPosition = 0;
}


void handleGameIterationStarted()
{
    gGameIterationStartTime = getTime();
}

void handleGameIterationFinished()
{
    const float kOriginalIterationDuration = 1000.0 / 35; // 35 iterations per second in the original game
    float currentIterationDuration = getTime() - gGameIterationStartTime;

    float targetIterationDuration = kOriginalIterationDuration * kSpeedTimeFactors[gGameSpeed];

    if (gFastMode != FastModeTypeNone)
    {
        targetIterationDuration = 0;
    }

    if (currentIterationDuration < targetIterationDuration)
    {
        waitTime(targetIterationDuration - currentIterationDuration);
    }

    gNumberOfGameIterations++;

    if (gGameIterationRateReferenceTime == 0)
    {
        gGameIterationRateReferenceTime = getTime();
    }
    else
    {
        uint32_t difference = getTime() - gGameIterationRateReferenceTime;

        if (difference > 1000)
        {
            gGameIterationRate = gNumberOfGameIterations * 1000.f / difference;
            gNumberOfGameIterations = 0;
            gGameIterationRateReferenceTime = getTime();
        }
    }
}

void runLevel() //    proc near       ; CODE XREF: start+35Cp
{
    // 01ED:1E58
    if (gIsPlayingDemo == 0)
    {
//loc_48ACE:              ; CODE XREF: runLevel+5j
        gIsLevelStartedAsDemo = 0;
        gLevelFailed = 1;
    }
    else
    {
        gIsLevelStartedAsDemo = 1;
        gLevelFailed = 0;
    }

//loc_48AD8:              ; CODE XREF: runLevel+11j
    if (gDemoRecordingJustStarted == 1)
    {
//loc_48ADF:              ; CODE XREF: runLevel+BAj
        gDemoRecordingJustStarted = 0;
        drawGameTime();

        do
        {
//isFunctionKey:              ; CODE XREF: runLevel+35j
            int9handler(1);
        }
        while (areAnyF1ToF10KeysPressed());

//notFunctionKey:             ; CODE XREF: runLevel+31j
        initializeGameInfo();
        if (isMusicEnabled == 0)
        {
            stopMusic();
        }

//loc_48AFF:              ; CODE XREF: runLevel+3Fj
        gIsLevelStartedAsDemo = 0;
        gLevelFailed = 1;
    }

//loc_48B09:              ; CODE XREF: runLevel+22j
    gPlantedRedDiskCountdown = 0;
    byte_5A323 = 0;

    do
    {
        handleGameIterationStarted();

        int9handler(0);

        uint16_t mouseButtonsStatus;

//loc_48B23:              ; CODE XREF: runLevel+63j
        getMouseStatus(NULL, NULL, &mouseButtonsStatus);

//loc_48B38:              ; CODE XREF: runLevel+6Ej runLevel+75j
        if (gIsDebugModeEnabled != 0)
        {
            if (gToggleFancyEasyTilesThrottleCounter != 0)
            {
                gToggleFancyEasyTilesThrottleCounter--;
            }

//loc_48B4A:              ; CODE XREF: runLevel+89j
            if (gIsEnterPressed == 0
                && mouseButtonsStatus == MouseButtonLeft //cmp bx, 1
                && gToggleFancyEasyTilesThrottleCounter == 0)
            {
                gToggleFancyEasyTilesThrottleCounter = 0xA;
                restoreOriginalFancyTiles(); // 01ED:1EFF
                drawFixedLevel();
                convertToEasyTiles();
            }
        }

//loc_48B6B:              ; CODE XREF: runLevel+82j runLevel+94j ...
        handleGameUserInput(); // 01ED:1F08
        if (gDemoRecordingJustStarted == 1)
        {
            // Restart the demo
//loc_48ADF:              ; CODE XREF: runLevel+BAj
            gDemoRecordingJustStarted = 0;
            drawGameTime();

            do
            {
//isFunctionKey:              ; CODE XREF: runLevel+35j
                int9handler(1);
            }
            while (areAnyF1ToF10KeysPressed());

//notFunctionKey:             ; CODE XREF: runLevel+31j
            initializeGameInfo();
            if (isMusicEnabled == 0)
            {
                stopMusic();
            }

//loc_48AFF:              ; CODE XREF: runLevel+3Fj
            gIsLevelStartedAsDemo = 0;
            gLevelFailed = 1;

//loc_48B09:              ; CODE XREF: runLevel+22j
            gPlantedRedDiskCountdown = 0;
            byte_5A323 = 0;

            continue;

            // All the code in this "if" is equivalent to "jmp loc_48ADF"
        }

//loc_48B78:              ; CODE XREF: runLevel+B8j
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (Color) { 0x35, 0x35, 0x35 });
        }

//noFlashing:              ; CODE XREF: runLevel+C2j
        updateMovingObjects(); // 01ED:1F28
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (Color) { 0x21, 0x21, 0x21 });
        }

//noFlashing2:              ; CODE XREF: runLevel+D8j
        drawGameTime();
        clearAdditionalInfoInGamePanelIfNeeded();
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (Color) { 0x2d, 0x21, 0x0f });
        }

//noFlashing3:              ; CODE XREF: runLevel+F1j
        // 01ED:1F5B
        updatePlantedRedDisk();
        updateExplosionTimers();
        updateScrollOffset();

//loc_48D59:              ; CODE XREF: runLevel+19Bj
//                ; runLevel+1D2j ...
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (Color) { 0x3f, 0x3f, 0x3f });
        }

//noFlashing4:              ; CODE XREF: runLevel+2D1j
        drawCurrentLevelViewport(gCurrentPanelHeight); // Added by me
        if (gFastMode != FastModeTypeUltra)
        {
            videoLoop(); // 01ED:2142
        }
        handleGameIterationFinished();

//isFastMode2:              ; CODE XREF: runLevel+2E8j
        if (gDebugExtraRenderDelay > 1)
        {
            playBaseSound();
        }

//loc_48DB2:              ; CODE XREF: runLevel+2F2j
        // Extra delays in debug mode
        for (int i = 1; i < gDebugExtraRenderDelay; ++i)
        {
//loc_48DB6:              ; CODE XREF: runLevel+310j
            if (gFastMode == FastModeTypeNone)
            {
                videoLoop(); // 01ED:2160
            }

//isFastMode3:              ; CODE XREF: runLevel+303j
            handleGameUserInput();
        }

//loc_48DCD:              ; CODE XREF: runLevel+2FCj
        if (gIsFlashingBackgroundModeEnabled != 0)
        {
            replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });
        }

//noFlashing5:              ; CODE XREF: runLevel+317j
        if (gShouldExitGame != 0)
        {
            break;
        }
        gFrameCounter++;
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
    if (gIsRecordingDemo != 0)
    {
        stopRecordingDemo();
    }

//loc_48E13:              ; CODE XREF: runLevel+353j
    uint8_t userDidNotCheat = (gHasUserCheated == 0);
    gHasUserCheated = 0;
    if (userDidNotCheat
        && gShouldUpdateTotalLevelTime != 0
        && byte_5A323 == 0)
    {
        addCurrentGameTimeToPlayer();
    }

//loc_48E30:              ; CODE XREF: runLevel+362j
//                ; runLevel+369j ...
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });
}


void updateUserInputInScrollMovementMode() // sub_4914A   proc near       ; CODE XREF: handleGameUserInput+7p
{
    if (isLeftButtonPressed())
    {
        gAdditionalScrollOffsetX--;
        gAdditionalScrollOffsetX--;
    }

//loc_49159:              ; CODE XREF: updateUserInputInScrollMovementMode+5j
    if (isRightButtonPressed())
    {
        gAdditionalScrollOffsetX++;
        gAdditionalScrollOffsetX++;
    }

//loc_49168:              ; CODE XREF: updateUserInputInScrollMovementMode+14j
    if (isUpButtonPressed())
    {
        gAdditionalScrollOffsetY--;
        gAdditionalScrollOffsetY--;
    }

//loc_49177:              ; CODE XREF: updateUserInputInScrollMovementMode+23j
    if (isDownButtonPressed())
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
    if (gIsInsertKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = -gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = -gMurphyScrollOffsetY;
    }

//loc_491B3:              ; CODE XREF: updateUserInputInScrollMovementMode+60j
    if (gIsHomeKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = (kLevelBitmapWidth / 2) - gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = -gMurphyScrollOffsetY;
    }

//loc_491C5:              ; CODE XREF: updateUserInputInScrollMovementMode+72j
    if (gIsRePagKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = kLevelBitmapWidth - gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = -gMurphyScrollOffsetY;
    }

//loc_491E8:              ; CODE XREF: updateUserInputInScrollMovementMode+99j
    if (gIsDelKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = -gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = kLevelBitmapHeight - gMurphyScrollOffsetY;
    }

//loc_491F6:              ; CODE XREF: updateUserInputInScrollMovementMode+A3j
    if (gIsEndKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = (kLevelBitmapWidth / 2) - gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = kLevelBitmapHeight - gMurphyScrollOffsetY;
    }

//loc_49208:              ; CODE XREF: updateUserInputInScrollMovementMode+B5j
    if (gIsAvPagKeyPressed != 0)
    {
        gAdditionalScrollOffsetX = kLevelBitmapWidth - gMurphyScrollOffsetX;
        gAdditionalScrollOffsetY = kLevelBitmapHeight - gMurphyScrollOffsetY;
    }
}

void simulateDemoInput() // sub_492A8   proc near       ; CODE XREF: handleGameUserInput+27p
                   // ; restartLevel+76p
{
    // 01ED:2645
    if (gDemoCurrentInputRepeatCounter > 1)
    {
        gDemoCurrentInputRepeatCounter--;
        return;
    }

//loc_492B3:              ; CODE XREF: simulateDemoInput+5j
    uint8_t newInput = gDemos.demoData[gDemoCurrentInputIndex];

    if (newInput == 0xFF)
    {
        gQuitLevelCountdown = 0x64;
        gShouldExitLevel = 1;
    }
    else
    {
        gDemoCurrentInputIndex++;
    }

//loc_492CA:              ; CODE XREF: simulateDemoInput+47j
    gCurrentUserInput = newInput & 0xF;
    gDemoCurrentInputRepeatCounter = (newInput >> 4) + 1;
}

void saveInputForDemo() // sub_492F1   proc near       ; CODE XREF: handleGameUserInput+1Dp
{
    gDemoCurrentInputRepeatCounter++;

    if (gDemoCurrentInputRepeatCounter == 0xFF)
    {
        gDemoCurrentInput = gCurrentUserInput;
        gDemoRecordingRandomGeneratorSeed = gRandomGeneratorSeed;
        gDemoRecordingRandomGeneratorSeedLow = (gDemoRecordingRandomGeneratorSeed >> 8); // ah;
        gDemoRecordingRandomGeneratorSeedHigh = (gDemoRecordingRandomGeneratorSeed & 0xFF); // al;
    }

//loc_49311:              ; CODE XREF: saveInputForDemo+Dj
    if (gDemoCurrentInput == gCurrentUserInput
        && gDemoCurrentInputRepeatCounter != 0xF)
    {
        return;
    }

//loc_4931E:              ; CODE XREF: saveInputForDemo+24j
    gDemoCurrentInput = (gDemoCurrentInput
                                    | (gDemoCurrentInputRepeatCounter << 4));

    gDemoRecordingRandomGeneratorSeedHigh += gDemoCurrentInputRepeatCounter;
    gDemoRecordingRandomGeneratorSeedHigh++;

    fileWriteUInt8(gDemoCurrentInput, gCurrentRecordingDemoFile);
    gDemoCurrentInputRepeatCounter = 0xFF;
    gDemoCurrentInput = gCurrentUserInput;
}

void stopRecordingDemo() // somethingspsig  proc near       ; CODE XREF: runLevel+355p
                     // ; recordDemo+30p ...
{
    uint8_t scrambleSpeedLow = (gDemoRecordingLowestSpeed ^ gDemoRecordingRandomGeneratorSeedLow);
    uint8_t scrambleSpeedHigh = (gDemoRecordingLowestSpeed ^ gDemoRecordingRandomGeneratorSeedHigh);
    uint16_t scrambleSpeed = ((scrambleSpeedHigh << 8)
                              | scrambleSpeedLow);

    fseek(gCurrentRecordingDemoFile, 1532, SEEK_SET);
    fileWriteUInt16(scrambleSpeed, gCurrentRecordingDemoFile);

    fileWriteUInt16(gDemoRecordingRandomGeneratorSeed, gCurrentRecordingDemoFile);

    fseek(gCurrentRecordingDemoFile, 0, SEEK_END);

    gDemoCurrentInput = 0xFF;

    fileWriteUInt8(gDemoCurrentInput, gCurrentRecordingDemoFile);
    if (byte_5A19B != 0)
    {
        FILE *sigFile = openWritableFileWithReadonlyFallback("MYSPSIG.TXT", "rb");
        if (sigFile != NULL)
        {
            if (fseek(sigFile, 0, SEEK_END) == 0)
            {
                long sigFileSize = ftell(sigFile);
                sigFileSize = MIN(sigFileSize, kMaxDemoSignatureLength);

                if (sigFileSize > 0)
                {
//loc_493EB:              ; CODE XREF: stopRecordingDemo+85j
//                ; stopRecordingDemo+8Dj
                    if (fseek(sigFile, 0, SEEK_SET) == 0)
                    {
                        uint8_t signature[kMaxDemoSignatureLength + 1];
                        size_t bytes = fileReadBytes(signature, sigFileSize, sigFile);

                        if (bytes == sigFileSize)
                        {
                            int idx = 0;
                            for (idx = 0; idx < sigFileSize; ++idx)
                            {
                                if (signature[idx] == 0xFF)
                                {
                                    break;
                                }
                            }

                            // Make sure the signature is terminated with 0xFF
                            signature[idx] = 0xFF;
                            sigFileSize = idx;

//loc_4941C:              ; CODE XREF: stopRecordingDemo+BCj
                            fileWriteBytes(signature, sigFileSize, sigFile);
                        }
                    }
                }
            }

//loc_49430:              ; CODE XREF: stopRecordingDemo+7Ej
//                ; stopRecordingDemo+89j ...
            fclose(sigFile);
        }
    }
//loc_49435:              ; CODE XREF: stopRecordingDemo+65j
//                ; stopRecordingDemo+6Fj
    fclose(gCurrentRecordingDemoFile);
    gIsRecordingDemo = 0;
    if (gHasUserInterruptedDemo != 0)
    {
        gIsPlayingDemo = 1;
    }

//loc_4944F:              ; CODE XREF: stopRecordingDemo+EEj
    drawGamePanelText();
    gIsGameBusy = 1;
    gIsPlayingDemo = 0;
}

size_t writeCurrentLevelToFile(FILE *file)
{
    size_t bytes = fileWriteBytes(gCurrentLevel.tiles, sizeof(gCurrentLevel.tiles), file);

    bytes += fileWriteBytes(gCurrentLevel.unused, sizeof(gCurrentLevel.unused), file);
    bytes += fileWriteUInt8(gCurrentLevel.initialGravitation, file);
    bytes += fileWriteUInt8(gCurrentLevel.speedFixMagicNumber, file);
    bytes += fileWriteBytes(gCurrentLevel.name, sizeof(gCurrentLevel.name), file);
    bytes += fileWriteUInt8(gCurrentLevel.freezeZonks, file);
    bytes += fileWriteUInt8(gCurrentLevel.numberOfInfotrons, file);
    bytes += fileWriteUInt8(gCurrentLevel.numberOfSpecialPorts, file);
    for (int idx = 0; idx < kLevelMaxNumberOfSpecialPorts; ++idx)
    {
        bytes += fileWriteUInt16(gCurrentLevel.specialPortsInfo[idx].position, file);
        bytes += fileWriteUInt8(gCurrentLevel.specialPortsInfo[idx].gravity, file);
        bytes += fileWriteUInt8(gCurrentLevel.specialPortsInfo[idx].freezeZonks, file);
        bytes += fileWriteUInt8(gCurrentLevel.specialPortsInfo[idx].freezeEnemies, file);
        bytes += fileWriteUInt8(gCurrentLevel.specialPortsInfo[idx].unused, file);
    }
    bytes += fileWriteUInt8(gCurrentLevel.scrambledSpeed, file);
    bytes += fileWriteUInt8(gCurrentLevel.scrambledChecksum, file);
    bytes += fileWriteUInt16(gCurrentLevel.randomSeed, file);

    return bytes;
}

void recordDemo(uint16_t demoIndex) // sub_4945D   proc near       ; CODE XREF: handleGameUserInput+294p
                   // ; handleGameUserInput+2A4p ...
{
    // 01ED:27FA
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;

    replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });

    if (gIsRecordingDemo != 0)
    {
        stopRecordingDemo();
    }

//loc_49490:              ; CODE XREF: recordDemo+2Ej
    char demoIndexCharacter = '0' + demoIndex;
    gDemo0BinFilename[4] = demoIndexCharacter;

    char *filename = gDemo0BinFilename;

    if (supportsSPFileDemoPlayback()
        && (gShouldRecordWithOriginalDemoFilenames & 0xFF) == 0) // cmp byte ptr gShouldRecordWithOriginalDemoFilenames, 0
    {
        gSPDemoFileName[7] = demoIndexCharacter;
        filename = gSPDemoFileName;
    }

//loc_494A6:              ; CODE XREF: recordDemo+41j
    gRecordingDemoMessage[18] = demoIndexCharacter;

    FILE *file = openWritableFile(filename, "wb");
    if (file == NULL)
    {
        return;
    }

//loc_494B8:              ; CODE XREF: recordDemo+56j
    gCurrentRecordingDemoFile = file; // file handle
    gCurrentLevel.speedFixMagicNumber = 0x20 + kGameVersion;
    // TODO: don't know for sure but this probably is related to adjusting the demo time with the speed or something?
    // bl = speed3;
    // cl = 4;
    // bl = bl << cl;
    // bl |= gGameSpeed;
    // speed2 = bl;
    gDemoRecordingLowestSpeed = gGameSpeed;

    size_t bytes = writeCurrentLevelToFile(file);
    if (bytes != kLevelDataLength)
    {
        return;
    }

    // The original code sets index | 0x80 in demoCurrentInputRepeatCounter and then writes it to the file
    // but seems to be useless because that value is overriden later.
    //
    uint8_t levelNumber = gCurrentSelectedLevelIndex | 0x80;
    bytes = fileWriteUInt8(levelNumber, file);
    if (bytes < 1)
    {
        return;
    }
    gDemoCurrentInput = UserInputNone;
    gDemoRecordingJustStarted = 1;
    gIsPlayingDemo = 0;
    gDemoCurrentInputRepeatCounter = 0xFE; // 254
    gDebugExtraRenderDelay = 1;
    if (gIsSPDemoAvailableToRun == 0)
    {
        memcpy(gCurrentDemoLevelName, gCurrentLevelName, 3);
    }

//loc_4952A:              ; CODE XREF: recordDemo+BCj
    gIsRecordingDemo = 1;
    if (gHasUserInterruptedDemo != 0)
    {
        gIsPlayingDemo = 1;
    }

//loc_4953B:              ; CODE XREF: recordDemo+D7j
    fetchAndInitializeLevel();
    gIsPlayingDemo = 0;
}

void prepareDemoRecordingFilename() // sub_49544  proc near       ; CODE XREF: start+3A1p
                   // ; handleOkButtonClick:loc_4B40Fp ...
{
    // 01ED:28E1

    char currentSuffix[3] = "AT";
    strcpy(currentSuffix, &gLevelsDatFilename[8]);

    // Checks if the last two chars are "00" like LEVELS.D00?
    if (strcmp(currentSuffix, "00") == 0)
    {
        // replaces the content with "--"
        strcpy(currentSuffix, "--");
    }

//loc_4954F:             // ; CODE XREF: prepareDemoRecordingFilename+6j
    // Now checks if the last two chars are "AT" like LEVELS.DAT?
    if (strcmp(currentSuffix, "AT") == 0)
    {
        // replaces the content with "00"
        strcpy(currentSuffix, "00");
    }

//loc_49557:             // ; CODE XREF: prepareDemoRecordingFilename+Ej
    memcpy(gSPDemoFileName, currentSuffix, 2);
}

void handleGameUserInput() // sub_4955B   proc near       ; CODE XREF: runLevel:loc_48B6Bp
                   // ; runLevel+30Cp
{
    // 01ED:28F8

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
            saveInputForDemo(); // 01ED:2915
        }
    }

//loc_4957B:              ; CODE XREF: handleGameUserInput+Aj
//                ; handleGameUserInput+11j ...
    if (gIsPlayingDemo != 0)
    {
        simulateDemoInput(); // 01ED:2929
    }

//loc_4958F:              ; CODE XREF: handleGameUserInput+2Fj
    if (gToggleGamePanelKeyAutoRepeatCounter != 0) // cmp byte ptr word_510C1+1, 0
    {
        // 01ED:293E
        gToggleGamePanelKeyAutoRepeatCounter--;
    }

//loc_4959A:              ; CODE XREF: handleGameUserInput+39j
    if (isToggleGamePanelButtonPressed() == 0)
    {
        gToggleGamePanelKeyAutoRepeatCounter = 0; // mov byte ptr word_510C1+1, 0
    }
//loc_495A9:              ; CODE XREF: handleGameUserInput+44j
    else if (gToggleGamePanelKeyAutoRepeatCounter == 0) // 01ED:2946
    {
//loc_495B3:              ; CODE XREF: handleGameUserInput+53j
        gToggleGamePanelKeyAutoRepeatCounter = 0x20; // mov byte ptr word_510C1+1, 20h ; ' '
        if (gShouldShowGamePanel != 0)
        {
            gShouldShowGamePanel = 0; // mov byte ptr word_510C1, 0
            gCurrentPanelHeight = 0;
        }
        else
        {
//loc_495FB:              ; CODE XREF: handleGameUserInput+62j
            gShouldShowGamePanel = 1; // mov byte ptr word_510C1, 1
            gCurrentPanelHeight = kPanelBitmapHeight;
        }
    }

//loc_49635:              ; CODE XREF: handleGameUserInput+4Bj
//                ; handleGameUserInput+55j ...
    if (gIsDebugModeEnabled != 1)
    {
        checkDebugKeys();
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
            // 01ED:2A07
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
            videoLoop();
            restartLevel();
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
    // 01ED:2ADF
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
            // 01ED:2B89
            recordDemo(0);
        }
//loc_497F5:              ; CODE XREF: handleGameUserInput+28Fj
        else if (gIsF2KeyPressed == 1)
        {
            // 01ED:2B99
            recordDemo(1);
        }
//loc_49805:              ; CODE XREF: handleGameUserInput+29Fj
        else if (gIsF3KeyPressed == 1)
        {
            recordDemo(2);
        }
//loc_49814:              ; CODE XREF: handleGameUserInput+2AFj
        else if (gIsF4KeyPressed == 1)
        {
            recordDemo(3);
        }
//loc_49823:              ; CODE XREF: handleGameUserInput+2BEj
        else if (gIsF5KeyPressed == 1)
        {
            recordDemo(4);
        }
//loc_49832:              ; CODE XREF: handleGameUserInput+2CDj
        else if (gIsF6KeyPressed == 1)
        {
            recordDemo(5);
        }
//loc_49841:              ; CODE XREF: handleGameUserInput+2DCj
        else if (gIsF7KeyPressed == 1)
        {
            recordDemo(6);
        }
//loc_49850:              ; CODE XREF: handleGameUserInput+2EBj
        else if (gIsF8KeyPressed == 1)
        {
            recordDemo(7);
        }
//loc_4985F:              ; CODE XREF: handleGameUserInput+2FAj
        else if (gIsF9KeyPressed == 1)
        {
            recordDemo(8);
        }
//loc_4986E:              ; CODE XREF: handleGameUserInput+309j
        else if (gIsF10KeyPressed == 1)
        {
            recordDemo(9);
        }
//loc_4987D:              ; CODE XREF: handleGameUserInput+318j
        else if (gIsF12KeyPressed == 1
            && gIsRecordingDemo != 0)
        {
            stopRecordingDemo();
        }
    }
    else
    {
//loc_4974C:              ; CODE XREF: handleGameUserInput+1ECj
        if ((gFrameCounter & 7) == 0
            && gIsRecordingDemo == 0)
        {
//loc_49761:              ; CODE XREF: handleGameUserInput+201j
            if (gIsF1KeyPressed == 0)
            {
                gToggleGravityAutorepeatFlag = 0;
            }
//loc_4976F:              ; CODE XREF: handleGameUserInput+20Bj
            else if (gToggleGravityAutorepeatFlag == 0)
            {
                gToggleGravityAutorepeatFlag--;
                gIsGravityEnabled &= 1;
                gIsGravityEnabled = gIsGravityEnabled ^ 1;
            }

            if (gIsF1KeyPressed == 0
                || gToggleGravityAutorepeatFlag != 0)
            {
//loc_49786:              ; CODE XREF: handleGameUserInput+212j
//                ; handleGameUserInput+219j
                if (gIsF2KeyPressed == 0)
                {
                    gToggleZonksFrozenAutorepeatFlag = 0;
                }
//loc_49794:              ; CODE XREF: handleGameUserInput+230j
                else if (gToggleZonksFrozenAutorepeatFlag == 0)
                {
                    gToggleZonksFrozenAutorepeatFlag--;
                    gAreZonksFrozen &= 2;
                    gAreZonksFrozen = gAreZonksFrozen ^ 2;
                }

                if (gIsF2KeyPressed == 0
                    || gToggleZonksFrozenAutorepeatFlag != 0)
                {
//loc_497AB:              ; CODE XREF: handleGameUserInput+237j
//                ; handleGameUserInput+23Ej
                    if (gIsF3KeyPressed == 0)
                    {
                        gToggleEnemiesFrozenAutorepeatFlag = 0;
                    }
//loc_497B9:              ; CODE XREF: handleGameUserInput+255j
                    else if (gToggleEnemiesFrozenAutorepeatFlag == 0)
                    {
                        gToggleEnemiesFrozenAutorepeatFlag--;
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
        checkDebugKeys();
        return;
    }

//loc_498A2:              ; CODE XREF: handleGameUserInput+342j
    if (gIsMinusKeyPressed == 0)
    {
        gDebugSkipPreviousLevelAutorepeatFlag_1 = 0;
        gDebugSkipPreviousLevelAutorepeatFlag_2 = 5;
    }
//loc_498B5:              ; CODE XREF: handleGameUserInput+34Cj
    else if (gDebugSkipPreviousLevelAutorepeatFlag_1 != 0)
    {
        gDebugSkipPreviousLevelAutorepeatFlag_1--;
    }
    else
    {
//loc_498C2:              ; CODE XREF: handleGameUserInput+35Fj
        if (gDebugSkipPreviousLevelAutorepeatFlag_2 != 0)
        {
            gDebugSkipPreviousLevelAutorepeatFlag_2--;
            gDebugSkipPreviousLevelAutorepeatFlag_1 = 0x10;
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
        convertLevelNumberTo3DigitStringWithPadding0(gCurrentSelectedLevelIndex);
        drawLevelList();
        debugSkipLevel();
    }

//loc_498FC:              ; CODE XREF: handleGameUserInput+358j
//                ; handleGameUserInput+365j
    if (gIsEqualsKeyPressed == 0)
    {
        gDebugSkipNextLevelAutorepeatFlag_1 = 0;
        gDebugSkipNextLevelAutorepeatFlag_2 = 5;
    }
//loc_4990F:              ; CODE XREF: handleGameUserInput+3A6j
    else if (gDebugSkipNextLevelAutorepeatFlag_1 != 0)
    {
        gDebugSkipNextLevelAutorepeatFlag_1--;
    }
    else
    {
//loc_4991C:              ; CODE XREF: handleGameUserInput+3B9j
        if (gDebugSkipNextLevelAutorepeatFlag_2 != 0)
        {
            gDebugSkipNextLevelAutorepeatFlag_2--;
            gDebugSkipNextLevelAutorepeatFlag_1 = 0x10; // 16
        }

//loc_4992C:              ; CODE XREF: handleGameUserInput+3C6j
        if (gCurrentSelectedLevelIndex >= kNumberOfLevels)
        {
            gCurrentSelectedLevelIndex = kNumberOfLevels - 1;
        }

//loc_49939:              ; CODE XREF: handleGameUserInput+3D6j
        gCurrentSelectedLevelIndex++;
        convertLevelNumberTo3DigitStringWithPadding0(gCurrentSelectedLevelIndex); // 01ED:2CDD
        drawLevelList();
        debugSkipLevel();
    }

    checkDebugKeys();
}

void stopDemoAndPlay()
{
    gIsPlayingDemo = 0;
    gShouldUpdateTotalLevelTime = 0;
    gHasUserCheated = 1;
    gHasUserInterruptedDemo = 1;
}

void checkDebugKeys() //loc_49949:              ; CODE XREF: handleGameUserInput+E1j
//                ; handleGameUserInput+33Aj ...
{
    // 01ED:2CE6

    uint8_t shouldStartFromSavedSnapshot = (gShouldStartFromSavedSnapshot != 0);
    gShouldStartFromSavedSnapshot = 0;
    if (shouldStartFromSavedSnapshot)
    {
        loadGameSnapshot();
        return;
    }

//loc_49958:              ; CODE XREF: handleGameUserInput+3F8j
    if (gIsLeftControlPressed != 1)
    {
        loc_49C41();
        return;
    }

//loc_49962:              ; CODE XREF: handleGameUserInput+402j
    // Control + F12: Interrupt demo and continue playing
    if (gIsF12KeyPressed == 1
        && gIsPlayingDemo != 0)
    {
        stopDemoAndPlay();
    }

//loc_49984:              ; CODE XREF: handleGameUserInput+40Cj
//                ; handleGameUserInput+413j
    if (gIsScrollLockPressed == 1)
    {
        gIsDebugModeEnabled = 1;
        drawTextWithChars8FontToGamePanel(304, 14, 6, "DB"); // Debug mode enabled
        gAdditionalInfoInGamePanelFrameCounter = 0x46; // 70
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

        loadGameSnapshot();
        return;
    }

    saveGameSnapshot();
}

void saveGameSnapshot() //loc_499C8:              ; CODE XREF: handleGameUserInput+454j
{
    gShouldCloseAdvancedMenu = 1;

    if (saveGameState() != 0)
    {
        showSavegameOperationError();
        return;
    }

//loc_49A78:              ; CODE XREF: handleGameUserInput+518j
    loc_49C2C("WR"); // Means snapshot saved with no issues
}

void loadGameSnapshot() // loc_49A89:              ; CODE XREF: handleGameUserInput+3FAj
{
    gShouldCloseAdvancedMenu = 1;

    if (canLoadGameState() == 0)
    {
        showSavegameOperationError();
        return;
    }

//loc_49A96:              ; CODE XREF: handleGameUserInput+536j
    if (gIsRecordingDemo != 0)
    {
        stopRecordingDemo();
    }

    gIsRecordingDemo = 0;

    if (loadGameState() != 0)
    {
        showSavegameOperationError();
        return;
    }

    forceRestoreOriginalFancyTiles();

//loc_49B84:              ; CODE XREF: handleGameUserInput+619j
//                    ; handleGameUserInput+624j
    gIsPlayingDemo = 0;
    gIsRecordingDemo = 0;
    gCurrentUserInput = UserInputNone;
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });
    generateRandomSeedFromClock();
    generateRandomNumber();
    // I commented out all these video transitions because they're not needed in the reimplementation. They were here
    // just to prevent graphical glitches. Also they made loading much slower.
    //
    // setPalette(gBlackPalette);
    // videoLoop();
    drawFixedLevel();
    drawGamePanel();
    convertToEasyTiles();
    scrollToMurphy();
    gLastDrawnMinutesAndSeconds = 0xFFFF;
    gLastDrawnHours = 0xFF;
    drawGameTime();
    gHasUserCheated = 1;
    gIsRecordingDemo = 0;

    drawTextWithChars8FontToGamePanel(304, 14, 6, "LD"); // Means snapshot was loaded successfully
    gAdditionalInfoInGamePanelFrameCounter = 0x46; // 70 or '&'

    drawCurrentLevelViewport(gCurrentPanelHeight);
    // videoLoop();

//loc_49C12:              ; CODE XREF: handleGameUserInput+6A8j
    // fadeToPalette(gGamePalette);

//loc_49C40:              ; CODE XREF: handleGameUserInput+6BDj
//                    ; handleGameUserInput+6D6j
    loc_49C41();
}

void showSavegameOperationError() //loc_49C28:              ; CODE XREF: handleGameUserInput+47Aj
{
//                    ; handleGameUserInput+51Aj ...
//    push    si
//    mov si, 0A007h "XX"
    loc_49C2C("XX"); // Means problem writing/loading snapshot
}

void loc_49C2C(char text[3]) // :              ; CODE XREF: handleGameUserInput+521j
{
    drawTextWithChars8FontToGamePanel(304, 14, 6, text);
    gAdditionalInfoInGamePanelFrameCounter = 0x46; // 70 or '&'

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
        replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });

        drawTextWithChars8FontToGamePanel(304, 14, 6, "--"); // Debug mode disabled
        gAdditionalInfoInGamePanelFrameCounter = 0x46; // 70 or '&'
    }

//loc_49C96:              ; CODE XREF: handleGameUserInput+6EBj
//                    ; handleGameUserInput+6F2j ...
    if (isPauseButtonPressed())
    {
        // 01ED:303A
        gIsGameRunning = 0;
        runAdvancedOptionsRootMenu();
        gIsGameRunning = 1;
    }

//loc_49CC8:              ; CODE XREF: handleGameUserInput+740j
    if (gIsNumLockPressed != 0)
    {
        // 01ED:306C
        gIsGameRunning = 0;
//        mov si, 6095h
        fadeToPalette(gGameDimmedPalette);

        do
        {
//loc_49CDA:              ; CODE XREF: handleGameUserInput+784j
            int9handler(1);
        }
        while (gIsNumLockPressed == 1);

        // From the speed fix mod, but in uppercase so I can use characterForLastKeyPressed
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
                if (characterForLastKeyPressed() == kMagicDisableDebugModeCode[index])
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
        fadeToPalette(gGamePalette);
        gIsGameRunning = 1;
    }

//loc_49D15:              ; CODE XREF: handleGameUserInput+772j
    if (isExitLevelButtonPressed() // Select/Back/- controller button -> exit game
        && gQuitLevelCountdown <= 0)
    {
        // This is called when I press ESC to exit the game, but not when I die
        gShouldKillMurphy = 1; // 01ED:30C0
    }

//loc_49D29:              ; CODE XREF: handleGameUserInput+7BFj
//                    ; handleGameUserInput+7C6j
    if (gIsQKeyPressed != 0
        || getGameControllerCancelButton())
    {
        // 01ED:30CD
        gIsMoveScrollModeEnabled = 0;
        gAdditionalScrollOffsetX = 0;
        gAdditionalScrollOffsetY = 0;
        gIsFlashingBackgroundModeEnabled = 0;
    }

//loc_49D48:              ; CODE XREF: handleGameUserInput+7D3j
    if (isShowNumberOfRedDisksButtonPressed())
    {
        drawNumberOfRemainingRedDisks(); // 01ED:30EC
    }
}

void forceRestoreOriginalFancyTiles() // sub_49D53   proc near       ; CODE XREF: handleGameUserInput+626p
                   // ; removeTiles+21p
{
    // 01ED:30F0
    gIsShowingFancyTiles = 0;
    restoreOriginalFancyTiles();
}

void restoreOriginalFancyTiles() //   proc near       ; CODE XREF: runLevel+A7p
{
    // 01ED:30F5

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49D65:              ; CODE XREF: restoreOriginalFancyTiles+18j
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        if (tile->tile == LevelTileTypeExplosion) // 31
        {
            tile->tile = 0xF1; // 241
        }
    }

    uint8_t was_gIsShowingFancyTiles_NonZero = (gIsShowingFancyTiles != 0);
    gIsShowingFancyTiles = 0;
    if (was_gIsShowingFancyTiles_NonZero)
    {
        return;
    }

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49D84:              ; CODE XREF: levelScanThing+4Cj
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        if (tile->state != 0 || tile->tile != LevelTileTypeHardware)
        {
            continue;
        }

        LevelTileType originalTile = gCurrentLevel.tiles[i];

        if (originalTile >= LevelTileTypeHardware2 // 28
            && originalTile <= LevelTileTypeHardware11) // 37
        {
            tile->tile = originalTile;
            tile->state = 0;
        }
    }

//loc_49DA6:              ; CODE XREF: levelScanThing+31j
    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_49DAC:              ; CODE XREF: levelScanThing+7Fj
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        if (tile->state != 0 || tile->tile != LevelTileTypeChip)
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
                tile->state = 0;
            }
        }
    }

//loc_49DD9:              ; CODE XREF: levelScanThing+59j
    gIsShowingFancyTiles = 1;
}

void updateMovingObjects() // gameloop   proc near       ; CODE XREF: runLevel:noFlashingp
{
    // 01ED:317D

    gMurphyLocation = updateMurphy(gMurphyLocation); // 01ED:318B

    if (gIsFlashingBackgroundModeEnabled != 0)
    {
        replaceCurrentPaletteColor(0, (Color) { 0x3f, 0x3f, 0x21 });
    }

//loc_49E14:
    if (gIsFlashingBackgroundModeEnabled != 0)
    {
        replaceCurrentPaletteColor(0, (Color) { 0x3f, 0x21, 0x21 });
    }

//loc_49E33:
    uint16_t numberOfMovingObjects = 0;

    typedef struct {
        MovingFunction function;
        uint16_t tilePosition;
    } MovingObject;

    MovingObject movingObjects[kLevelSize];

    // This loop doesn't iterate through _every tile_. Instead it only goes from the first tile in the gamefield
    // (located at levelWidth + 1) to the last tile in the gamefield (located at levelSize - levelWidth - 2).
    // From what I've seen some demos depend on this. Demo 00/01s028-1.SP is an example because there is a
    // non-hardware tile at the bottom edge that explodes, and a SnikSnak goes in there.
    //
    for (uint16_t i = kLevelWidth + 1; i < kLevelSize - kLevelWidth - 1; ++i) // starts from si, ends in si + cx
    {
//checkCellForMovingObject:              ; CODE XREF: updateMovingObjects+84j
        LevelTileType tile = gCurrentLevelState[i].tile; //         mov bl, byte ptr leveldata[si]

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

    // 01ED:3227
    if (gShouldKillMurphy != 1
        && gIsMurphyUpdated != 0)
    {
        return;
    }

//loc_49E99:              ; CODE XREF: updateMovingObjects+AFj
//                ; updateMovingObjects+B6j
    if (gQuitLevelCountdown == 0) // 01ED:3236
    {
        // 01ED:323D
        gShouldKillMurphy = 0;
        detonateBigExplosion(gMurphyPreviousLocation);
        gQuitLevelCountdown = 0x40; // 64
    }

    return;
}

void updateScrollOffset() // sub_49EBE   proc near       ; CODE XREF: runLevel+109p
                   // ; scrollToMurphy+29p
{
    // 01ED:325B
    uint16_t randomNumber = 0;

    // This random number is used to generate the shaking effect on explosions.
    // The original game generates this random number here for _every_ explosion, even if
    // normally only Murphy's explosion will make the screen shake. However it's necessary
    // to do this here to make sure the right sequence of random numbers is generated when
    // there are explosions in the level.
    //
    if (gIsExplosionStarted == 1)
    {
        randomNumber = generateRandomNumber();
    }

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
    if (gShouldShowGamePanel == 0)
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

    if (gShouldShowGamePanel == 0)
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
        gMurphyScrollOffsetX = scrollX;
        gMurphyScrollOffsetY = scrollY;
    }
    else
    {
//loc_49F2E:              ; CODE XREF: updateScrollOffset+65j
        scrollX = gMurphyScrollOffsetX;
        scrollY = gMurphyScrollOffsetY;

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
    // This makes the screen shake on an explosion
    if (gShouldShakeWithAllExplosions != 0
        || (gShakeWithExplosionsDisabled == 0
            && (gQuitLevelCountdown & 0xFF) != 0))
    {
//loc_49FBE:              ; CODE XREF: updateScrollOffset+F0j
        randomNumber = randomNumber & 0x101;

        uint16_t scrollShakeYOffset = randomNumber >> 8;
        uint16_t scrollShakeXOffset = (randomNumber & 0xFF);

        scrollY += scrollShakeYOffset;
        if (scrollX > 0x13C) // 316
        {
            scrollShakeXOffset = -scrollShakeXOffset;
        }

//loc_49FD0:              ; CODE XREF: updateScrollOffset+10Ej
        scrollX += scrollShakeXOffset;
    }

//loc_49FD2:              ; CODE XREF: updateScrollOffset+F7j
//                ; updateScrollOffset+FEj
    gScrollOffsetX = scrollX;
    gScrollOffsetY = scrollY;
}

void updateBugTiles(int16_t position) // movefun7  proc near       ; DATA XREF: data:163Co
{
    // 01ED:33DA
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *aboveLeftTile = &gCurrentLevelState[position - kLevelWidth - 1];
    StatefulLevelTile *aboveRightTile = &gCurrentLevelState[position - kLevelWidth + 1];
    StatefulLevelTile *belowLeftTile = &gCurrentLevelState[position + kLevelWidth - 1];
    StatefulLevelTile *belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];

    if (currentTile->tile != LevelTileTypeBug)
    {
        return;
    }

//loc_4A045:              ; CODE XREF: movefun7+5j
    if ((gFrameCounter & 3) != 0)
    {
        return;
    }

//loc_4A051:              ; CODE XREF: movefun7+11j
    int8_t frameNumber = currentTile->state;
    frameNumber++;
    if (frameNumber >= 0xE)
    {
        // 01ED:33F9
        uint8_t value = generateRandomNumber() & 0xFF;
        value &= 0x3F;
        value += 0x20;
        value = -value;
        frameNumber = value;
    }

//loc_4A067:              ; CODE XREF: movefun7+1Dj
    currentTile->state = frameNumber;
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
        playBugSound();
    }

//loc_4A0AE:              ; CODE XREF: movefun7+6Cj
    Point frameCoordinates = kBugFrameCoordinates[frameNumber];
    drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
}

void updateTerminalTiles(int16_t position) // movefun5  proc near       ; DATA XREF: data:1630o
{
    // 01ED:346F
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->tile != LevelTileTypeTerminal)
    {
        return;
    }

//loc_4A0DA:              ; CODE XREF: updateTerminalTiles+5j
    int8_t state = currentTile->state;
    state++;
    if (state <= 0)
    {
        currentTile->state = state;
        return;
    }

//loc_4A0EA:              ; CODE XREF: updateTerminalTiles+11j
    uint8_t value = generateRandomNumber() & 0xFF;
    value &= gTerminalMaxFramesToNextScroll;
    value = -value;
    currentTile->state = value;

    scrollTerminalScreen(position);
}

/// Updates the random seed using the clock
void generateRandomSeedFromClock() // getTime    proc near       ; CODE XREF: start:doesNotHaveCommandLinep
                    // ; handleGameUserInput+669p ...
{
    uint32_t timeInMilliseconds = getTime();
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

    gCurrentUserInput = UserInputNone;

    if (isUpButtonPressed())
    {
//loc_4A1CF:              ; CODE XREF: updateUserInput+7j
        gCurrentUserInput = UserInputUp;
        directionKeyWasPressed = 1;
    }

//loc_4A1D6:              ; CODE XREF: updateUserInput+Ej
    if (isLeftButtonPressed())
    {
//loc_4A1E4:              ; CODE XREF: updateUserInput+1Cj
        gCurrentUserInput = UserInputLeft;
        directionKeyWasPressed = 1;
    }

//loc_4A1EB:              ; CODE XREF: updateUserInput+23j
    if (isDownButtonPressed())
    {
//loc_4A1F9:              ; CODE XREF: updateUserInput+31j
        gCurrentUserInput = UserInputDown;
        directionKeyWasPressed = 1;
    }

//loc_4A200:              ; CODE XREF: updateUserInput+38j
    if (isRightButtonPressed())
    {
//loc_4A20E:              ; CODE XREF: updateUserInput+46j
        gCurrentUserInput = UserInputRight;
        directionKeyWasPressed = 1;
    }

//loc_4A215:              ; CODE XREF: updateUserInput+4Dj
    if (isActionButtonPressed())
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
    // 01ED:35D9
    // Looks like this function goes through every tile and clears those that match the parameter
    for (uint16_t i = 0; i < kLevelSize; ++i)
    {
//loc_4A242:              ; CODE XREF: removeTiles+1Fj
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        if (tile->tile != tileType)
        {
            if (tileType != LevelTileTypeSnikSnak
                || tile->tile != 0xBB)
            {
                continue;
            }
        }

//loc_4A253:              ; CODE XREF: removeTiles+Cj
        tile->state = 0;
        tile->tile = LevelTileTypeSpace;
    }
    forceRestoreOriginalFancyTiles();
    drawFixedLevel();
    convertToEasyTiles();
    gShouldUpdateTotalLevelTime = 0;
    gHasUserCheated = 1;
}

void findMurphy() //   proc near       ; CODE XREF: start+344p fetchAndInitializeLevel+22p
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

    scrollToMurphy();
}

void scrollToMurphy() // sub_4A291   proc near       ; CODE XREF: handleGameUserInput+686p
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

    videoLoop();
}

uint16_t convertToEasyTiles() // sub_4A2E6   proc near       ; CODE XREF: start+33Bp runLevel+ADp ...
{
    // 01ED:3683
    uint16_t numberOfInfotrons = 0;
    uint16_t numberOfSomething = 0; // this is bx, just counts the number of tiles so technically is same as cx at this point… probably a return value but I don't see it used anywhere???

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4A2F0:              ; CODE XREF: convertToEasyTiles+D1j
        StatefulLevelTile *currentTile = &gCurrentLevelState[i];
        numberOfSomething++;

        if (currentTile->tile == 0xF1)
        {
            currentTile->tile = LevelTileTypeExplosion;
            continue; // jmp short loc_4A3B0
        }

//loc_4A2FC:              ; CODE XREF: convertToEasyTiles+Ej
        if (gIsGameBusy != 1)
        {
            if (currentTile->tile == LevelTileTypeInfotron)
            {
//loc_4A33C:              ; CODE XREF: convertToEasyTiles+20j
                numberOfInfotrons++;
                continue; // jmp short loc_4A3B0
            }
        }
        // TODO: what are these gIsGameBusy for??
        if (gIsGameBusy == 1 || currentTile->state != 0 || currentTile->tile != LevelTileTypeSnikSnak) //jz  short loc_4A34B
        {
            if (gIsGameBusy == 1 || currentTile->state != 0 || currentTile->tile != LevelTileTypeElectron) //jz  short loc_4A379
            {
//loc_4A312:              ; CODE XREF: convertToEasyTiles+1Bj
                if ((currentTile->state == 0 && currentTile->tile == LevelTileTypeHorizontalChipLeft)
                    || (currentTile->state == 0 && currentTile->tile == LevelTileTypeHorizontalChipRight)
                    || (currentTile->state == 0 && currentTile->tile == LevelTileTypeHorizontalChipTop)
                    || (currentTile->state == 0 && currentTile->tile == LevelTileTypeHorizontalChipBottom))
                {
//loc_4A33F:              ; CODE XREF: convertToEasyTiles+2Fj
//                ; convertToEasyTiles+34j ...
                    currentTile->tile = LevelTileTypeChip; // mov word ptr [si], 5
                    currentTile->state = 0;
                    continue; // jmp short loc_4A3B0
                }
                if (currentTile->state == 0
                    && currentTile->tile >= LevelTileTypeHardware2
                    && currentTile->tile <= LevelTileTypeHardware11)
                {
//loc_4A345:              ; CODE XREF: convertToEasyTiles+48j
                    currentTile->tile = LevelTileTypeHardware; // mov word ptr [si], 6
                    currentTile->state = 0;
                    continue; // jmp short loc_4A3B0
                }

//loc_4A330:              ; CODE XREF: convertToEasyTiles+43j
                if (currentTile->state == 0
                    && currentTile->tile >= LevelTileTypeSportRight
                    && currentTile->tile <= LevelTileTypeSportUp)
                {
//loc_4A3A7:              ; CODE XREF: convertToEasyTiles+52j
                    currentTile->tile -= 4; // Converts Sport[Direction] to Port[Direction]
                    currentTile->state = 1;
                    continue;
                }

//loc_4A33A:              ; CODE XREF: convertToEasyTiles+4Dj
                continue;
            }
        }

        StatefulLevelTile *leftTile = &gCurrentLevelState[i - 1];
        StatefulLevelTile *aboveTile = &gCurrentLevelState[i - kLevelWidth];
        StatefulLevelTile *rightTile = &gCurrentLevelState[i + 1];

        if (currentTile->state != 0 || currentTile->tile != LevelTileTypeElectron) //jz  short loc_4A379
        {
//loc_4A34B:              ; CODE XREF: convertToEasyTiles+25j
            if (leftTile->tile == LevelTileTypeSpace && leftTile->state == 0) //cmp word ptr [si-2], 0
            {
                currentTile->state = 1;
//                si[1] = 1; //mov byte ptr [si+1], 1
                continue; // jmp short loc_4A3B0
            }
//loc_4A357:              ; CODE XREF: convertToEasyTiles+69j
        // 0x78 = 120
            if (aboveTile->tile == LevelTileTypeSpace && aboveTile->state == 0) //cmp word ptr [si-78h], 0
            {
                // 01ED:36FA
                // mov word ptr [si-78h], 1011h
                aboveTile->state = 0x10;
                aboveTile->tile = LevelTileTypeSnikSnak;
                // mov word ptr [si], 0FFFFh
                currentTile->state = 0xFF;
                currentTile->tile = 0xFF;
                continue; // jmp short loc_4A3B0
            }
//loc_4A368:              ; CODE XREF: convertToEasyTiles+75j
            if (rightTile->tile == LevelTileTypeSpace && rightTile->state == 0) //cmp word ptr [si+2], 0
            {
                // 01ED:370B
                // mov word ptr [si+2], 2811h
                rightTile->state = 0x28;
                rightTile->tile = LevelTileTypeSnikSnak;
                // mov word ptr [si], 0FFFFh
                currentTile->state = 0xFF;
                currentTile->tile = 0xFF;
                continue; // jmp short loc_4A3B0
            }

            continue;
        }
//loc_4A379:              ; CODE XREF: convertToEasyTiles+2Aj
        if (leftTile->tile == LevelTileTypeSpace && leftTile->state == 0) //cmp word ptr [si-2], 0
        {
            currentTile->state = 1; //mov byte ptr [si+1], 1
            continue; // jmp short loc_4A3B0
        }
//loc_4A385:              ; CODE XREF: convertToEasyTiles+97j
        if (aboveTile->tile == LevelTileTypeSpace && aboveTile->state == 0) //cmp word ptr [si-78h], 0
        {
            // mov word ptr [si-78h], 1018h
            aboveTile->state = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            // mov word ptr [si], 0FFFFh
            currentTile->state = 0xFF;
            currentTile->tile = 0xFF;
            continue; // jmp short loc_4A3B0
        }
//loc_4A396:              ; CODE XREF: convertToEasyTiles+A3j
        if (rightTile->tile == LevelTileTypeSpace && rightTile->state == 0) //cmp word ptr [si+2], 0
        {
            // mov word ptr [si+2], 2818h
            rightTile->state = 0x28;
            rightTile->tile = LevelTileTypeElectron;
            // mov word ptr [si], 0FFFFh
            currentTile->state = 0xFF;
            currentTile->tile = 0xFF;
            continue; // jmp short loc_4A3B0
        }
    }

    return numberOfInfotrons;
}

void resetNumberOfInfotrons(uint16_t numberOfInfotronsFoundInLevel) // sub_4A3BB   proc near       ; CODE XREF: start+33Ep fetchAndInitializeLevel+17p
{
    // In the original game, the number of infotrons found in a level is stored in a 2-bytes variable,
    // however, when stored for its use in the game, it's stored in a 1-byte variable.
    //
    uint8_t numberOfInfotrons = (numberOfInfotronsFoundInLevel & 0xFF);
    if (gNumberOfInfoTrons != 0)
    {
        numberOfInfotrons = gNumberOfInfoTrons;
    }

//loc_4A3C6:              ; CODE XREF: resetNumberOfInfotrons+5j
    gNumberOfRemainingInfotrons = numberOfInfotrons;
    gTotalNumberOfInfotrons = numberOfInfotrons;
    drawNumberOfRemainingInfotrons();
}

void debugSkipLevel() // sub_4A3D2  proc near       ; CODE XREF: handleGameUserInput+39Ep
                   // ; handleGameUserInput+3EBp
{
    gIsSPDemoAvailableToRun = 0;
    gSelectedOriginalDemoLevelNumber = 0;
    uint8_t wasNotZero = (gHasUserInterruptedDemo != 0);
    gHasUserInterruptedDemo = 0;
    gHasUserCheated = 1;
    if (wasNotZero)
    {
        restartLevelWithoutAddingCurrentGameTimeToPlayer();
    }

    restartLevel();
}

void restartLevel() // sub_4A3E9   proc near       ; CODE XREF: handleGameUserInput+14Ep
{
    if (gHasUserInterruptedDemo == 0)
    {
        addCurrentGameTimeToPlayer();
    }

    if (gIsRecordingDemo != 0)
    {
        stopRecordingDemo();
    }

    restartLevelWithoutAddingCurrentGameTimeToPlayer();
}

void restartLevelWithoutAddingCurrentGameTimeToPlayer() //loc_4A3F3:              ; CODE XREF: debugSkipLevel+15j
//                ; restartLevel+5j
{
    gIsMoveScrollModeEnabled = 0;
    gAdditionalScrollOffsetX = 0;
    gAdditionalScrollOffsetY = 0;
    gIsFlashingBackgroundModeEnabled = 0;
    gDebugExtraRenderDelay = 1;
    replaceCurrentPaletteColor(0, (Color) { 0, 0, 0 });

    if (gHasUserInterruptedDemo != 0)
    {
        gIsPlayingDemo = 1;
    }

//loc_4A427:              ; CODE XREF: restartLevel+37j
    gIsGameBusy = 0;
    fetchAndInitializeLevel();
    gIsGameBusy = 1;
    if (gHasUserInterruptedDemo >= 1)
    {
        gIsPlayingDemo = 0;
        if (gHasUserInterruptedDemo == 0) // WTF? this makes no sense...
        {
            gHasUserInterruptedDemo++;
        }
    }

//loc_4A446:              ; CODE XREF: restartLevel+50j
//                ; restartLevel+57j
    gCurrentUserInput = UserInputNone;
    if (gIsPlayingDemo == 0)
    {
        return;
    }

    gDemoCurrentInputIndex = word_5A33C;
    gDemoCurrentInputRepeatCounter = 1;
    simulateDemoInput();
}

void fetchAndInitializeLevel() // sub_4A463   proc near       ; CODE XREF: recordDemo:loc_4953Bp
                   // ; restartLevel+43p
{
    readLevels();
    drawFixedLevel();
    drawGamePanel();
    gIsGameBusy = -gIsGameBusy;
    uint16_t numberOfInfotrons = convertToEasyTiles();
    gIsGameBusy = -gIsGameBusy;
    resetNumberOfInfotrons(numberOfInfotrons);
    gIsShowingFancyTiles = 1;
    initializeGameInfo();
    findMurphy();
}

void updateOrangeDiskTiles(int16_t position) // movefun3  proc near       ; DATA XREF: data:161Ao
{
    // 01ED:3826
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];

    if (currentTile->tile != LevelTileTypeOrangeDisk)
    {
        return;
    }

//loc_4A491:              ; CODE XREF: movefun3+5j
    uint16_t tileValue = ((currentTile->state << 8)
                         | currentTile->tile);

    if (tileValue >= 0x3008)
    {
//loc_4A4D4:              ; CODE XREF: movefun3+Fj
    //    push    si
        uint8_t stateFrame = currentTile->state;
//        bh = 0;
//        al = state;
    //    bx *= 2;
    //    ;and bx, byte ptr 0Fh
    //    db 83h, 0E3h, 0Fh
        stateFrame *= 2;
        stateFrame &= 0xF; // 16 frames?

    //    mov di, [si+6155h]
    //    shl bx, 1
    //    add di, [bx+6C95h]
    //    mov si, 12F6h
    //    mov si, [si]
        uint16_t offset = kFallAnimationGravityOffsets[stateFrame];

        uint8_t tileX = (position % kLevelWidth);
        uint8_t tileY = (position / kLevelWidth);

        uint16_t dstX = tileX * kTileSize + (offset % 122);
        uint16_t dstY = tileY * kTileSize + (offset / 122);

        drawMovingSpriteFrameInLevel(128, 64,
                                     kTileSize,
                                     kTileSize + 2,
                                     dstX, dstY);

        uint8_t state = currentTile->state;
        state++;
        uint8_t otherMovingObject = state;
        otherMovingObject &= 7;
        if (otherMovingObject != 0)
        {
            currentTile->state = state;
            return;
        }

//loc_4A516:              ; CODE XREF: movefun3+86j
        currentTile->state = 0;
        currentTile->tile = LevelTileTypeSpace;
        belowTile->state = 0;
        belowTile->tile = LevelTileTypeOrangeDisk;

        position += kLevelWidth;

        // Update tiles
        currentTile = &gCurrentLevelState[position];
        belowTile = &gCurrentLevelState[position + kLevelWidth];

        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
            currentTile->state = 0x30;
            belowTile->state = 8;
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
        if (belowTile->state != 0 || belowTile->tile != LevelTileTypeSpace)
        {
//loc_4A4C2:              ; CODE XREF: movefun3+30j
            uint8_t state = currentTile->state;
            state++;
            if (state == 0x22)
            {
                state = 0x30;
            }

//loc_4A4CF:              ; CODE XREF: movefun3+42j
            currentTile->state = state;
            return;
        }
        currentTile->state = 0;
        currentTile->tile = LevelTileTypeOrangeDisk;
        return;
    }

    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
//loc_4A4A9:              ; CODE XREF: movefun3+1Dj
        currentTile->state = 0x20;
        belowTile->state = 8;
    }
}

void updateExplosionTiles(int16_t position) //loc_4A543:              ; DATA XREF: data:1648o
{
    // 01ED:38E0
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->tile != LevelTileTypeExplosion)
    {
        return;
    }

//loc_4A54B:              ; CODE XREF: code:3928j
    if ((gFrameCounter & 3) != 0)
    {
        return;
    }

//loc_4A557:              ; CODE XREF: code:3934j
    uint8_t state = currentTile->state;
    if ((state & 0x80) != 0)
    {
//loc_4A5A0:              ; CODE XREF: code:393Ej
        state++;
        if (state != 0x89)
        {
//loc_4A5B3:              ; CODE XREF: code:3985j
            currentTile->state = state;
            state--;
            state &= 0xF;
            // 12e6
            Point frameCoordinates = kInfotronExplosionAnimationFrameCoordinates[state];

            drawMovingFrame(frameCoordinates.x,
                            frameCoordinates.y,
                            position);
        }
        else
        {
            currentTile->state = 0;
            currentTile->tile = LevelTileTypeInfotron;
            gIsExplosionStarted = 0;
        }
    }
    else
    {
        state++;
        currentTile->state = state;
        state--;

        // 12d6
        Point frameCoordinates = kRegularExplosionAnimationFrameCoordinates[state];

        drawMovingFrame(frameCoordinates.x,
                        frameCoordinates.y,
                        position);

//loc_4A582:              ; CODE XREF: code:396Aj
        if (currentTile->state == 8)
        {
            currentTile->state = 0;
            currentTile->tile = LevelTileTypeSpace;
            gIsExplosionStarted = 0;
        }
    }
}

void updateExplosionTimers() // sub_4A5E0   proc near       ; CODE XREF: runLevel+106p
{
    // 01ED:397D
    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4A5E9:              ; CODE XREF: updateExplosionTimers+25j
        int8_t timer = gExplosionTimers[i];

        if (timer == 0)
        {
            continue;
        }

        if (timer < 0)
        {
//loc_4A608:              ; CODE XREF: updateExplosionTimers+10j
            gExplosionTimers[i] = timer + 1;

            if (gExplosionTimers[i] == 0)
            {
                StatefulLevelTile *tile = &gCurrentLevelState[i];
                tile->state = 0xFF;
                tile->tile = LevelTileTypeElectron;
                detonateBigExplosion(i);
            }
        }
        else
        {
            gExplosionTimers[i] = timer - 1;

            if (gExplosionTimers[i] == 0)
            {
                detonateBigExplosion(i);
            }
        }
    }
}

void detonateBigExplosionTile(int16_t position, uint8_t newTile, uint8_t newState, uint8_t newExplosionTimer)
{
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

//loc_4A64C:              ; CODE XREF: detonateBigExplosion+26j
    uint8_t hasChangedCurrentTile = 0;

    if (currentTile->tile == LevelTileTypeOrangeDisk
        || currentTile->tile == LevelTileTypeYellowDisk
        || currentTile->tile == LevelTileTypeSnikSnak)
    {
//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (currentTile->tile != LevelTileTypeHardware)
        {
            gExplosionTimers[position] = newExplosionTimer; // mov [bx+23F7h], dh
        }
    }
    else if (currentTile->tile == LevelTileTypeZonk)
    {
//loc_4A69C:              ; CODE XREF: detonateBigExplosion+46j
        // 01ED:3A39
        detonateZonk(position, newState, newTile);
        hasChangedCurrentTile = 1; // to emulate jmp loc_4A6A6
    }
    else if (currentTile->tile == LevelTileTypeInfotron)
    {
//loc_4A692:              ; CODE XREF: detonateBigExplosion+4Aj 01ED:3A2F
        sub_4AA34(position, newState, newTile);
        hasChangedCurrentTile = 1; // to emulate jmp loc_4A6A6
    }
    else if (currentTile->tile == LevelTileTypeElectron)
    {
        newExplosionTimer = -newExplosionTimer; // dh = -dh;
        newState = 0x80;
        newTile = LevelTileTypeExplosion;
//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (currentTile->tile != LevelTileTypeHardware)
        {
            gExplosionTimers[position] = newExplosionTimer; // mov [bx+23F7h], dh
        }
    }
//loc_4A676:              ; CODE XREF: detonateBigExplosion+4Ej
    else if (currentTile->tile == LevelTileTypeMurphy)
    {
        gShouldKillMurphy = 1;

//loc_4A680:              ; CODE XREF: detonateBigExplosion+3Aj
//                ; detonateBigExplosion+3Ej ...
        if (currentTile->tile != LevelTileTypeHardware)
        {
            gExplosionTimers[position] = newExplosionTimer; // mov [bx+23F7h], dh
        }
    }

    if (hasChangedCurrentTile == 0)
    {
//loc_4A688:              ; CODE XREF: detonateBigExplosion+59j
//                ; detonateBigExplosion+63j
        if (currentTile->tile != LevelTileTypeHardware)
        {
            // mov [si+17BAh], cx
            currentTile->state = newState;
            currentTile->tile = newTile;
        }
    }
}

// Creates an explossion of 3x3 tiles around a position
void detonateBigExplosion(int16_t position) // sub_4A61F   proc near       ; CODE XREF: movefun+271p
                   // ; movefun2+20Fp ...
{
    // 01ED:39BC
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->state == 0 && currentTile->tile == LevelTileTypeHardware)
    {
        return;
    }

    // These indicate the kind of the explosion created by this tile.
    // Tiles around may create a different explosion if needed (like Electrons create Infotrons).
    //
    uint8_t newState = 0;
    uint8_t newTile = 0;
    uint8_t newExplosionTimer = 0;

//loc_4A627:              ; CODE XREF: detonateBigExplosion+5j
    gIsExplosionStarted = 1;
    if (currentTile->tile == LevelTileTypeMurphy)
    {
        gShouldKillMurphy = 1;
    }

//loc_4A639:              ; CODE XREF: detonateBigExplosion+12j
    if (currentTile->tile == LevelTileTypeElectron)
    {
        newState = 0x80;
        newTile = LevelTileTypeExplosion;
        newExplosionTimer = -13;
    }
    else
    {
//loc_4A647:              ; CODE XREF: detonateBigExplosion+1Fj
        // cx = 0x1F; // 31
        newState = 0;
        newTile = LevelTileTypeExplosion;
        newExplosionTimer = 13;
    }

    detonateBigExplosionTile(position - kLevelWidth - 1, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position - kLevelWidth, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position - kLevelWidth + 1, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position - 1, newTile, newState, newExplosionTimer);

//loc_4A7AB:              ; CODE XREF: detonateBigExplosion:loc_4A795j
//                ; detonateBigExplosion+180j ...
    currentTile->state = newState;
    currentTile->tile = newTile;

    detonateBigExplosionTile(position + 1, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position + kLevelWidth - 1, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position + kLevelWidth, newTile, newState, newExplosionTimer);
    detonateBigExplosionTile(position + kLevelWidth + 1, newTile, newState, newExplosionTimer);

//loc_4A90B:              ; CODE XREF: detonateBigExplosion:loc_4A8F5j
//                ; detonateBigExplosion+2E0j ...
    playExplosionSound();
    // 01ED:3CAC
}

void updatePlantedRedDisk() // sub_4A910   proc near       ; CODE XREF: runLevel:noFlashing3p
{
    // 01ED:3CAD
    if (gPlantedRedDiskCountdown <= 1)
    {
        return;
    }

    StatefulLevelTile *tile = &gCurrentLevelState[gPlantedRedDiskPosition];

    if (tile->state == 0 && tile->tile == LevelTileTypeSpace)
    {
        tile->state = 0;
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

void addCurrentGameTimeToPlayer() // sub_4A95F   proc near       ; CODE XREF: runLevel+372p
                   // ; restartLevel+7p ...
{
    uint8_t seconds = gGameSeconds;
    uint8_t minutes = gGameMinutes;
    uint16_t hours = gGameHours;
    if (gIsPlayingDemo != 0)
    {
        return;
    }

    if (gIsSPDemoAvailableToRun != 0)
    {
        return;
    }

//loc_4A980:              ; CODE XREF: addCurrentGameTimeToPlayer+1Ej
    PlayerEntry *playerEntry = &gPlayerListData[gCurrentPlayerIndex];
    seconds += playerEntry->seconds;

    do
    {
//loc_4A994:              ; CODE XREF: addCurrentGameTimeToPlayer+3Ej
        if (seconds < 60)
        {
            break;
        }
        seconds -= 60;
        playerEntry->minutes++;
    }
    while (1);

//loc_4A99F:              ; CODE XREF: addCurrentGameTimeToPlayer+37j
    playerEntry->seconds = seconds;

    minutes += playerEntry->minutes;

    do
    {
//loc_4A9A8:              ; CODE XREF: addCurrentGameTimeToPlayer+52j
        if (minutes < 60)
        {
            break;
        }
        minutes -= 60;
        playerEntry->hours++;
    }
    while (1);

//loc_4A9B3:              ; CODE XREF: addCurrentGameTimeToPlayer+4Bj
    playerEntry->minutes = minutes;

    hours += playerEntry->hours;

    if (hours > 0xFF)
    {
        hours = 0xFF;
    }

//loc_4A9C0:              ; CODE XREF: addCurrentGameTimeToPlayer+5Dj
    playerEntry->hours = hours;
}

void detonateZonk(int16_t position, uint8_t state, uint8_t tile) // sub_4A9C4   proc near       ; CODE XREF: detonateBigExplosion+81p
                  //  ; detonateBigExplosion+D8p ...
{
    // 01ED:3D61
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];

    uint8_t stateType = currentTile->state & 0xF0;

    currentTile->state = state;
    currentTile->tile = tile;

    if (stateType == 0x10
        || stateType == 0x70)
    {
//loc_4A9EF:              ; CODE XREF: detonateZonk+Aj detonateZonk+Fj
        sub_4AAB4(position - kLevelWidth);
        if (belowTile->state == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (stateType == 0x20)
    {
//loc_4AA05:              ; CODE XREF: detonateZonk+14j
        sub_4AAB4(position + 1);
        sub_4AAB4(position + kLevelWidth);
    }
    else if (stateType == 0x30)
    {
//loc_4AA12:              ; CODE XREF: detonateZonk+19j
        sub_4AAB4(position - 1);
        sub_4AAB4(position + kLevelWidth);
    }
    else if (stateType == 0x50)
    {
//loc_4AA1F:              ; CODE XREF: detonateZonk+1Ej
        sub_4AAB4(position - 1);
    }
    else if (stateType == 0x60)
    {
//loc_4AA26:              ; CODE XREF: detonateZonk+23j
        sub_4AAB4(position + 1);
    }
    else if (stateType == 0x70)
    {
//loc_4AA2D:              ; CODE XREF: detonateZonk+28j
        sub_4AAB4(position + kLevelWidth);
    }
}

void sub_4AA34(int16_t position, uint8_t state, uint8_t tile) //   proc near       ; CODE XREF: detonateBigExplosion+77p
                   // ; detonateBigExplosion+CEp ...
{
    // Parameters:
    // - si: position
    // - cx: state (ch) and tile (cl)

    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];

    uint8_t stateType = currentTile->state & 0xF0;

    currentTile->state = state;
    currentTile->tile = tile;

    if (stateType == 0x10
        || stateType == 0x70)
    {
//loc_4AA5F:              ; CODE XREF: sub_4AA34+Aj sub_4AA34+Fj
        sub_4AAB4(position - kLevelWidth);
        if (belowTile->state == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (stateType == 0x20)
    {
//loc_4AA75:              ; CODE XREF: sub_4AA34+14j
        sub_4AAB4(position + 1);
        if (belowTile->state == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (stateType == 0x30)
    {
//loc_4AA8A:              ; CODE XREF: sub_4AA34+19j
        sub_4AAB4(position - 1);
        if (belowTile->state == 0x99 && belowTile->tile == 0x99)
        {
            sub_4AAB4(position + kLevelWidth);
        }
    }
    else if (stateType == 0x50)
    {
//loc_4AA9F:              ; CODE XREF: sub_4AA34+1Ej
        sub_4AAB4(position - 1);
    }
    else if (stateType == 0x60)
    {
//loc_4AAA6:              ; CODE XREF: sub_4AA34+23j
        sub_4AAB4(position + 1);
    }
    else if (stateType == 0x70)
    {
//loc_4AAAD:              ; CODE XREF: sub_4AA34+28j
        sub_4AAB4(position + kLevelWidth);
    }
}

void sub_4AAB4(int16_t position) //   proc near       ; CODE XREF: detonateZonk+2Ep
                   // ; detonateZonk+3Dp ...
{
    // 01ED:3DD1
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->tile == LevelTileTypeExplosion)
    {
        return;
    }

//loc_4AABC:              ; CODE XREF: sub_4AAB4+5j
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeSpace;

    // si = word_51580;
    uint16_t dstX = (position % kLevelWidth) * kTileSize;
    uint16_t dstY = (position / kLevelWidth) * kTileSize;

    drawMovingSpriteFrameInLevel(0, 32, kTileSize, kTileSize, dstX, dstY);
}

void handleNewPlayerOptionClick() // sub_4AB1B  proc near       ; CODE XREF: runMainMenu+28Fp
//                    ; DATA XREF: data:off_50318o
{
    // 01ED:3EB8
    if (gIsForcedCheatMode != 0)
    {
        //jnz short loc_4AB4A
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 6, "PLAYER LIST FULL       ");
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
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 6, "PLAYER LIST FULL       ");
        return;
    }

//loc_4AB56:              ; CODE XREF: handleNewPlayerOptionClick+25j
    gNewPlayerEntryIndex = newPlayerIndex;

    char newPlayerName[kPlayerNameLength + 1] = "        ";
    gNewPlayerNameLength = 0;
    uint16_t mouseX, mouseY;
    uint16_t mouseButtonStatus;

    restoreLastMouseAreaBitmap();

    if (supportsRealKeyboard())
    {
        // mov di, 89F7h
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 4, "YOUR NAME:             ");

        do
        {
//loc_4AB7F:              ; CODE XREF: handleNewPlayerOptionClick+6Aj
            getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        }
        while (mouseButtonStatus != 0);

        char lastPressedCharacter = '\0';

        do
        {
//noKeyPressed:               ; CODE XREF: handleNewPlayerOptionClick+79j
//                ; handleNewPlayerOptionClick+8Aj ...
            videoLoop();

            int9handler(0);
            getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
            if (mouseButtonStatus != 0)
            {
                break;
            }
            if (isAnyKeyPressed() == 0)
            {
                lastPressedCharacter = '\0';
                continue;
            }

            char character = characterForLastKeyPressed();

            if (lastPressedCharacter == character)
            {
                continue;
            }

            lastPressedCharacter = character;

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
                drawTextWithChars6FontWithOpaqueBackgroundIfPossible(232, 127, 6, newPlayerName);
                continue;
            }
            if (gNewPlayerNameLength >= 8) // when more than 8 chars were entered, ignore the rest?
            {
                continue;
            }
            newPlayerName[gNewPlayerNameLength] = character; // mov [bx+si], al
            gNewPlayerNameLength++;
            drawTextWithChars6FontWithOpaqueBackgroundIfPossible(232, 127, 6, newPlayerName);
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
    else if (supportsVirtualKeyboard())
    {
        char inputBuffer[kPlayerNameLength + 1] = "";
        uint8_t result = inputVirtualKeyboardText("Enter your name", kPlayerNameLength, inputBuffer);

        if (result == 0)
        {
            drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "                       ");
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

        // Limit the player number value to avoid -Wformat-truncation warning
        snprintf(newPlayerName, sizeof(newPlayerName), "PLAYER%2d", MIN(gNewPlayerEntryIndex + 1, kNumberOfPlayers));
        gNewPlayerNameLength = strlen(newPlayerName);
    }

    // Completely empty name: ignore
    if (strcmp(newPlayerName, "        ") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "                       ");
        saveLastMouseAreaBitmap();
        drawMouseCursor();
        return;
    }

//loc_4AC1E:              ; CODE XREF: handleNewPlayerOptionClick+E0j
//                ; handleNewPlayerOptionClick+E5j ...
    // Name with all dashes: invalid
    if (strcmp(newPlayerName, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 6, "INVALID NAME           ");
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
            drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 6, "PLAYER EXISTS          ");
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

    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "                       ");
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
    if (gIsForcedCheatMode != 0)
    {
//loc_4AD3C:              ; CODE XREF: handleDeletePlayerOptionClick+5j
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
//    *dword_58477 = currentPlayerEntry; // mov word ptr dword_58477, si
    if (strcmp(currentPlayerEntry->name, "--------") == 0)
    {
//loc_4AD3C:              ; CODE XREF: handleDeletePlayerOptionClick+5j
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4AD48:              ; CODE XREF: handleDeletePlayerOptionClick+1Dj
//                ; handleDeletePlayerOptionClick+22j ...
    char message[24] = "";
    sprintf(message, "DELETE '%s' ???  ", currentPlayerEntry->name);

    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, message);

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
        videoLoop();
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
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "                       ");
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
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "NO PLAYER SELECTED     ");
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
            drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 6, "SKIP NOT POSSIBLE      ");
            return;
        }
    }

//loc_4AE5B:              ; CODE XREF: handleSkipLevelOptionClick+49j
//                ; handleSkipLevelOptionClick+4Ej
    if (gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1] != kNotCompletedLevelEntryColor)
    {
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 4, "COLORBLIND I GUESS     ");
        return;
    }

//loc_4AE75:              ; CODE XREF: handleSkipLevelOptionClick+68j
    char levelNumber[4] = "000";
    convertNumberTo3DigitStringWithPadding0(gCurrentSelectedLevelIndex, levelNumber);

    char message[24];
    sprintf(message, "SKIP LEVEL %s ???     ", levelNumber);
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, message);

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
        videoLoop();
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
        gCurrentPlayerLevelState = PlayerLevelStateSkipped;
        changePlayerCurrentLevelState(); // 01ED:4275
        gShouldAutoselectNextLevelToPlay = 0;
        prepareLevelDataForCurrentPlayer();
    }

//loc_4AEE9:              ; CODE XREF: handleSkipLevelOptionClick+C3j
//                ; handleSkipLevelOptionClick+C8j ...
    restoreLastMouseAreaBitmap();
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "                       ");
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
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4AFE3:              ; CODE XREF: handleStatisticsOptionClick+58j
    fadeToPalette(gBlackPalette);

    uint8_t *screenPixelsBackup = malloc(kFullScreenFramebufferLength);
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();

    byte_5091A = 0;
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 20, 15, "SUPAPLEX  BY DREAM FACTORY");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(64, 50, 15, "(C) DIGITAL INTEGRATION LTD 1991");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(16, 60, 15, "________________________________________________");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 80, 15, "SUPAPLEX PLAYER STATISTICS");

    char currentPlayerText[27] = "";
    sprintf(currentPlayerText, "CURRENT PLAYER :  %s", currentPlayerEntry.name);
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 100, 15, currentPlayerText);

    if (currentPlayerEntry.nextLevelToPlay == kLastLevelIndex)
    {
        byte_5091A = 1;
    }

//loc_4B046:              ; CODE XREF: handleStatisticsOptionClick+133j
    char levelNumberString[4] = "000";
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.nextLevelToPlay, levelNumberString);

    char currentLevelText[27] = "";
    sprintf(currentLevelText, "CURRENT LEVEL  :       %s", levelNumberString);
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 110, 15, currentLevelText);

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

    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 120, 15, usedTimeText);

    uint32_t totalMinutes = currentPlayerEntry.hours * 60 + currentPlayerEntry.minutes;

    if (currentPlayerEntry.seconds >= 30)
    {
        totalMinutes++;
    }

//loc_4B0A1:              ; CODE XREF: handleStatisticsOptionClick+192j

    char averageTimeString[6] = "000.0";
    uint16_t averageMinutesWhole = totalMinutes / currentPlayerEntry.nextLevelToPlay;
    uint16_t averageMinutesFraction = (totalMinutes % currentPlayerEntry.nextLevelToPlay);
    averageMinutesFraction = averageMinutesFraction * 10 / currentPlayerEntry.nextLevelToPlay;
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
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(24, 140, 15, "YOU'VE COMPLETED ALL LEVELS! CONGRATULATIONS!!!");
    }
//loc_4B0E2:              ; CODE XREF: handleStatisticsOptionClick+1C7j
    else if (byte_5091A == 2)
    {
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(40, 140, 15, "STILL UNDER ONE MINUTE (KEEP IT UP...)");
    }
//loc_4B0F6:              ; CODE XREF: handleStatisticsOptionClick+1DBj
    else
    {
        char averageTimeMessage[44] = "";
        sprintf(averageTimeMessage, "AVERAGE TIME USED PER LEVEL  %s MINUTES", averageTimeString);
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(32, 140, 15, averageTimeMessage);
    }
//loc_4B105:              ; CODE XREF: handleStatisticsOptionClick+1D4j
//                ; handleStatisticsOptionClick+1E8j
    fadeToPalette(gInformationScreenPalette);
    waitForKeyMouseOrJoystick();
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);
    fadeToPalette(gGamePalette);

    free(screenPixelsBackup);
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
    gShouldLeaveMainMenu = 1;
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

//loc_4B188:              ; CODE XREF: handleDemoOptionClick+2Aj
    // This picks a random demo
    generateRandomSeedFromClock();
    uint16_t demoIndex = generateRandomNumber() % numberOfDemos;
    uint16_t demoFirstIndex = gDemos.demoFirstIndices[demoIndex];

    // This only happens if there are no demos...
    if (demoFirstIndex == 0xFFFF)
    {
        gShouldLeaveMainMenu = 0;
        gIsPlayingDemo = 0;
    }

//loc_4B1AE:              ; CODE XREF: handleDemoOptionClick+48j
    uint8_t demoLevelNumber = gDemos.demoData[demoFirstIndex];
    uint8_t finalLevelNumber = demoIndex;

    gSelectedOriginalDemoIndex = demoIndex;
    gSelectedOriginalDemoLevelNumber = 0;

    // This checks if the level number has its MSB to 0 and is a valid level number (1-111) for the original DEMO format
    if (demoLevelNumber <= 0x6F // 111
        && demoLevelNumber != 0)
    {
        gSelectedOriginalDemoLevelNumber = (gSelectedOriginalDemoLevelNumber & 0xFF00) | demoLevelNumber; // mov byte ptr gSelectedOriginalDemoLevelNumber, al
        finalLevelNumber = demoLevelNumber;
    }

//loc_4B1CF:              ; CODE XREF: handleDemoOptionClick+6Bj
//                ; handleDemoOptionClick+6Fj
    gRandomGeneratorSeed = gDemoRandomSeeds[demoIndex];
    gDemoIndexOrDemoLevelNumber = finalLevelNumber;

    demoFirstIndex++; // To skip the level number
    gDemoCurrentInputIndex = demoFirstIndex;
    word_5A33C = demoFirstIndex;
    gDemoCurrentInput = UserInputNone;
    gDemoCurrentInputRepeatCounter = 1;
}

void playDemo(uint16_t demoIndex) // demoSomething  proc near       ; CODE XREF: start+3BAp
                    // ; runMainMenu+12Ep ...
{
    readDemoFiles();

    gRandomGeneratorSeed = gDemoRandomSeeds[demoIndex];
    gShouldLeaveMainMenu = 1;
    gIsPlayingDemo = 1;

    uint16_t demoFirstIndex = gDemos.demoFirstIndices[demoIndex];
    if (demoFirstIndex == 0xFFFF)
    {
        gShouldLeaveMainMenu = 0;
        gIsPlayingDemo = 0;
    }

//loc_4B22F:              ; CODE XREF: playDemo+30j
    gSelectedOriginalDemoLevelNumber = 0;

    uint8_t demoLevelNumber = gDemos.demoData[demoFirstIndex];
    uint8_t finalLevelNumber = demoIndex;

    if (demoLevelNumber <= kNumberOfLevels // 111
        && demoLevelNumber != 0)
    {
        finalLevelNumber = demoLevelNumber;
        gSelectedOriginalDemoLevelNumber = (gSelectedOriginalDemoLevelNumber & 0xFF00) | finalLevelNumber; // mov byte ptr gSelectedOriginalDemoLevelNumber, al
    }

//loc_4B248:              ; CODE XREF: playDemo+4Bj
//                ; playDemo+4Fj
    gDemoIndexOrDemoLevelNumber = finalLevelNumber;

    demoFirstIndex++; // To skip the level number
    gDemoCurrentInputIndex = demoFirstIndex;
    word_5A33C = demoFirstIndex;
    gDemoCurrentInput = UserInputNone;
    gDemoCurrentInputRepeatCounter = 1;
}

void handleRankingListScrollUp() // loc_4B262
{
    gRankingListButtonPressed = 1;
    gRankingListDownButtonPressed = 0;
    gRankingListUpButtonPressed = 1;

    if (gFrameCounter - gRankingListThrottleCurrentCounter < gRankingListThrottleNextCounter)
    {
        return;
    }

//loc_4B27F:              ; CODE XREF: code:465Cj
    restoreLastMouseAreaBitmap();
    gRankingListThrottleNextCounter = gFrameCounter;
    if (gRankingListThrottleCurrentCounter > 1)
    {
        gRankingListThrottleCurrentCounter--;
    }

//loc_4B293:              ; CODE XREF: code:466Dj
    if (gIsForcedCheatMode == 0
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

    if (gFrameCounter - gRankingListThrottleCurrentCounter < gRankingListThrottleNextCounter)
    {
        return;
    }

//loc_4B2CC:              ; CODE XREF: code:46A9j
    restoreLastMouseAreaBitmap();
    gRankingListThrottleNextCounter = gFrameCounter;
    if (gRankingListThrottleCurrentCounter > 1)
    {
        gRankingListThrottleCurrentCounter--;
    }

//loc_4B2E0:              ; CODE XREF: code:46BAj
    if (gIsForcedCheatMode == 0
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

    uint8_t *screenPixelsBackup = malloc(kFullScreenFramebufferLength);
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(120, 30, 15, "CONGRATULATIONS");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(24, 70, 15, "YOU HAVE COMPLETED ALL 111 LEVELS OF SUPAPLEX");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(64, 85, 15, "YOUR BRAIN IS IN FANTASTIC SHAPE");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(40, 100, 15, "NOT MANY PEOPLE ARE ABLE TO MANAGE THIS");
    fadeToPalette(gInformationScreenPalette);
    waitForKeyMouseOrJoystick();
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);

    fadeToPalette(gGamePalette); // 6015h

    free(screenPixelsBackup);
}

void handleOkButtonClick() // sub_4B375  proc near       ; CODE XREF: runMainMenu+11Ep
{
    // 01ED:4712
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];

    if (strcmp(currentPlayerEntry.name, "--------") == 0)
    {
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "NO PLAYER SELECTED     ");
        return;
    }

//loc_4B3A4:              ; CODE XREF: handleOkButtonClick+12j
//                ; handleOkButtonClick+17j ...
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
            drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 2, "COLORBLIND I GUESS     ");
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
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 8, "COLORBLIND I GUESS     ");
        return;
    }
    gShouldLeaveMainMenu = 1;
    gIsPlayingDemo = 0;

    if (currentLevelColor == kCompletedLevelEntryColor)
    {
        gShouldUpdateTotalLevelTime = 0;
    }
    else
    {
//loc_4B3FD:              ; CODE XREF: handleOkButtonClick+7Fj
        gShouldUpdateTotalLevelTime = 1;
    }

//loc_4B40F:              ; CODE XREF: handleOkButtonClick+86j
//                ; handleOkButtonClick+8Dj
    prepareDemoRecordingFilename(); // 01ED:47AC
    convertLevelNumberTo3DigitStringWithPadding0(gCurrentSelectedLevelIndex); // 01ED:47B2
}

void throttledRotateLevelSet(uint8_t descending) // sub_4B419  proc near
{
    // 01ED:47B6
//loc_4B433:              ; CODE XREF: sub_4B419+15j
    if (gFrameCounter - gLevelSetRotationThrottleCurrentCounter < gLevelSetRotationThrottleNextCounter)
    {
        return;
    }

//loc_4B443:              ; CODE XREF: sub_4B419+25j
    gLevelSetRotationThrottleNextCounter = gFrameCounter;
    if (gLevelSetRotationThrottleCurrentCounter > 1)
    {
        gLevelSetRotationThrottleCurrentCounter--;
    }

    rotateLevelSet(descending);
}

void rotateLevelSet(uint8_t descending) // sub_4B419  proc near
{
    FILE *file = NULL;
    char currentSuffix[3] = "AT";

    do
    {
//loc_4B454:              ; CODE XREF: sub_4B419+35j
//                ; sub_4B419+9Aj
        strcpy(currentSuffix, &gLevelsDatFilename[8]);

        if (descending)
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

        file = openReadonlyFile(gLevelsDatFilename, "rb");
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
    if (gShouldAlwaysWriteSavegameSav == 0)
    {
        strcpy(&gSavegameSavFilename[0xA], currentSuffix);
    }

//loc_4B504:              ; CODE XREF: sub_4B419+E6j
    readLevelsLst();
    readDemoFiles();

    // 01ED:48B2
    if (gIsForcedCheatMode != 0)
    {
        PlayerEntry *entry = &gPlayerListData[0];
        memset(entry->levelState, PlayerLevelStateSkipped, sizeof(entry->levelState));
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

    updateMenuAfterLevelSetChanged();
}

void updateMenuAfterLevelSetChanged() // loc_4B565:              ; CODE XREF: sub_4B419+10Fj
{
//loc_4B4C6:              ; CODE XREF: sub_4B419+A8j
    char message[] = "     LEVEL SET ??      ";
    char currentSuffix[3] = "AT";
    strcpy(currentSuffix, &gLevelsDatFilename[8]);

    memcpy(&message[0xF], currentSuffix, 2);

//loc_4B4F9:              ; CODE XREF: sub_4B419+DBj
    if (strcmp(currentSuffix, "AT") == 0)
    {
        strcpy(message, "  SUPAPLEX LEVEL SET   ");
    }

    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 4, message);

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

void handleFloppyDiskButtonClick()
{
    // Pressing the shift key will show the level sets in descending order
    throttledRotateLevelSet(gIsRightShiftPressed || gIsLeftShiftPressed);
}

void handlePlayerListScrollDown() // sub_4B671  proc near
{
    gPlayerListButtonPressed = 1;
    gPlayerListDownButtonPressed = 1;
    gPlayerListUpButtonPressed = 0;

    if (gFrameCounter - gPlayerListThrottleCurrentCounter < gPlayerListThrottleNextCounter)
    {
        return;
    }

//loc_4B68E:              ; CODE XREF: handlePlayerListScrollDown+1Aj
    gPlayerListThrottleNextCounter = gFrameCounter;
    if (gPlayerListThrottleCurrentCounter > 1)
    {
        gPlayerListThrottleCurrentCounter--;
    }

//loc_4B69F:              ; CODE XREF: handlePlayerListScrollDown+28j
    if (gIsForcedCheatMode == 0
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

    if (gFrameCounter - gPlayerListThrottleCurrentCounter < gPlayerListThrottleNextCounter)
    {
        return;
    }

//loc_4B6E6:              ; CODE XREF: handlePlayerListScrollUp+1Aj
    gPlayerListThrottleNextCounter = gFrameCounter;
    if (gPlayerListThrottleCurrentCounter > 1)
    {
        gPlayerListThrottleCurrentCounter--;
    }

//loc_4B6F7:              ; CODE XREF: handlePlayerListScrollUp+28j
    if (gIsForcedCheatMode == 0
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

    if (gFrameCounter - gLevelListThrottleCurrentCounter < gLevelListThrottleNextCounter)
    {
        return;
    }

//loc_4B748:              ; CODE XREF: handleLevelListScrollDown+1Aj
    gLevelListThrottleNextCounter = gFrameCounter;
    if (gLevelListThrottleCurrentCounter > 1)
    {
        gLevelListThrottleCurrentCounter--;
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

    if (gFrameCounter - gLevelListThrottleCurrentCounter < gLevelListThrottleNextCounter)
    {
        return;
    }

//loc_4B78E:              ; CODE XREF: handleLevelListScrollUp+1Aj
    gLevelListThrottleNextCounter = gFrameCounter;
    if (gLevelListThrottleCurrentCounter > 1)
    {
        gLevelListThrottleCurrentCounter--;
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

    uint8_t *screenPixelsBackup = malloc(kFullScreenFramebufferLength);
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    drawBackBackground();

    drawTextWithChars6FontWithTransparentBackgroundIfPossible(80, 10, 15, "SUPAPLEX  BY DREAM FACTORY");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(56, 40, 15, "ORIGINAL DESIGN BY PHILIP JESPERSEN");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(88, 50, 15, "AND MICHAEL STOPP");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(56, 90, 15, "NEARLY ALL LEVELS BY MICHEAL STOPP");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(64, 100, 15, "A FEW LEVELS BY PHILIP JESPERSEN");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(56, 110, 15, "HARDLY ANY LEVELS BY BARBARA STOPP");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(64, 170, 15, "NOTE: PRESS ENTER TO REMOVE PANEL");
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(64, 190, 15, "(C) DIGITAL INTEGRATION LTD 1991");
    fadeToPalette(gInformationScreenPalette);
    waitForKeyMouseOrJoystick();
    fadeToPalette(gBlackPalette);
    memcpy(gScreenPixels, screenPixelsBackup, kFullScreenFramebufferLength);
    fadeToPalette(gGamePalette);

    free(screenPixelsBackup);
}

void drawTextWithChars6FontWithOpaqueBackgroundIfPossible(size_t destX, size_t destY, uint8_t color, const char *text) // sub_4BA5F  proc near       ; CODE XREF: handleNewPlayerOptionClick+37p
                  //  ; handleNewPlayerOptionClick+4Ap ...
{
    // Parameters:
    // - di is the destination surface
    // - si is the text to be rendered
    // - ah is the color index in the current palette

    // Address: 01ED:4DFC
    if (gIsGameBusy == 1)
    {
        return;
    }

    drawTextWithChars6FontWithOpaqueBackground(destX, destY, color, text);
}

void drawTextWithChars6FontWithTransparentBackgroundIfPossible(size_t destX, size_t destY, uint8_t color, const char *text)  // sub_4BDF0 proc near       ; CODE XREF: recoverFilesFromFloppyDisk+2Ap
                   // ; handleStatisticsOptionClick+EDp ...
{
    if (gIsGameBusy == 1)
    {
        return;
    }

    drawTextWithChars6FontWithTransparentBackground(destX, destY, color, text);
}

void convertLevelNumberTo3DigitStringWithPadding0(uint8_t number) // sub_4BF4A   proc near       ; CODE XREF: start+3F7p handleGameUserInput+398p ...
{
    convertNumberTo3DigitStringWithPadding0(number, &gSPDemoFileName[3]);
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
    if (gIsForcedCheatMode != 0)
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
        PlayerEntry *player = &gPlayerListData[i];

        rankingEntry->playerIndex = i;
        rankingEntry->nextLevelToPlay = player->nextLevelToPlay;
        rankingEntry->hours = player->hours;
        rankingEntry->minutes = player->minutes;
        rankingEntry->seconds = player->seconds;
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
        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(8, y, color, gRankingTextEntries[byte_58D46 + i]);
    }

    char numberString[4] = "001"; // 0x8359
    convertNumberTo3DigitStringWithPadding0(byte_58D46 + 1, numberString);
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(144, 110, 6, &numberString[1]); // Remove the first (left most) digit
}

void drawLevelList() // sub_4C141  proc near       ; CODE XREF: start+41Ap handleGameUserInput+39Bp ...
{
    // 01ED:54DE
    byte_59821 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 2];
    byte_59822 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex - 1];
    byte_59823 = gCurrentPlayerLevelData[gCurrentSelectedLevelIndex];

    char *previousLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 2) * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(144, 155, byte_59821, previousLevelName);

    char *currentLevelName = (char *)&gLevelListData[(gCurrentSelectedLevelIndex - 1) * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(144, 164, byte_59822, currentLevelName);

    memcpy(gCurrentLevelName, currentLevelName, kListLevelNameLength);

    char *nextLevelName = (char *)&gLevelListData[gCurrentSelectedLevelIndex * kListLevelNameLength];
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(144, 173, byte_59823, nextLevelName);
}

void drawHallOfFame() // sub_4C1A9   proc near       ; CODE XREF: handleFloppyDiskButtonClick+15Ap
//                    ; drawMenuTitleAndDemoLevelResult+11p
{
    // 01ED:5546
    char text[19] = { '\0' };

    for (int i = 0; i < kNumberOfHallOfFameEntries; ++i)
    {
//loc_4C1B7:              ; CODE XREF: drawHallOfFame+56j
        strcpy(text, "                  ");
        HallOfFameEntry entry = gHallOfFameData[i];

        convertNumberTo3DigitStringWithPadding0(entry.seconds, &text[15]);
        text[15] = ':';

        convertNumberTo3DigitStringWithPadding0(entry.minutes, &text[12]);
        text[12] = ':';

        convertNumberTo3DigitPaddedString(entry.hours, &text[9], 1);

        uint8_t playerNameLength = MIN(strlen(entry.playerName), sizeof(entry.playerName) - 1);
        memcpy(text, entry.playerName, playerNameLength);

        drawTextWithChars6FontWithOpaqueBackgroundIfPossible(184, 28 + i * 9, 8, text);
    }
}

void drawCurrentPlayerRanking() //   proc near       ; CODE XREF: drawPlayerList+5Bp // sub_4C224
{
    // 01ED:55C1
    PlayerEntry currentPlayerEntry = gPlayerListData[gCurrentPlayerIndex];
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 93, 8, currentPlayerEntry.name);

    char timeText[10] = "000:00:00";

    // Seconds
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.seconds, &timeText[6]);
    timeText[6] = ':';

    // Minutes
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.minutes, &timeText[3]);
    timeText[3] = ':';

    // Hours
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.hours, timeText);

    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(224, 93, 8, timeText);

    char nextLevelText[4] = "000";
    convertNumberTo3DigitStringWithPadding0(currentPlayerEntry.nextLevelToPlay, nextLevelText);
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(288, 93, 8, nextLevelText);
}

void drawPlayerList() // sub_4C293  proc near       ; CODE XREF: start+32Cp start+407p ...
{
    // 01ED:5630
    PlayerEntry currentPlayer = gPlayerListData[gCurrentPlayerIndex];
    memcpy(gPlayerName, currentPlayer.name, sizeof(currentPlayer.name) - 1);
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(16, 164, 6, currentPlayer.name);

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
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(16, 155, 8, prevPlayerName);

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
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(16, 173, 8, nextPlayerName);
    drawCurrentPlayerRanking();
}

void drawMenuTitleAndDemoLevelResult() // sub_4C2F2   proc near       ; CODE XREF: handleGfxTutorOptionClick+Cp
                    // ; sub_4C407+1Fp ...
{
    // 01ED:568F
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 4, "  WELCOME TO SUPAPLEX  ");
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
        if (gIsLevelStartedAsDemo == 0)
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
        if (gIsLevelStartedAsDemo == 0)
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
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(168, 127, 4, message);
    byte_5A19B = 0;
}

void prepareLevelDataForCurrentPlayer() // sub_4C34A   proc near       ; CODE XREF: start+404p handleNewPlayerOptionClick+1E0p ...
{
    // 01ED:56E7
    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];

    uint8_t *currentPlayerLevelState = currentPlayerEntry->levelState;

    // Sets everything to 6 which seems to mean all levels are blocked
    memset(gCurrentPlayerPaddedLevelData, kSkippedLevelEntryColor, kNumberOfLevelsWithPadding);
    memset(gCurrentPlayerLevelData, kBlockedLevelEntryColor, kCurrentPlayerkLevelDataLength);

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
        currentPlayerEntry->nextLevelToPlay = kLastLevelIndex;
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
        if (strcmp(currentPlayerEntry->name, "--------") == 0)
        {
            nextLevelToPlay = 0;
        }
    }

//loc_4C403:              // ; CODE XREF: prepareLevelDataForCurrentPlayer+9Aj
                // ; prepareLevelDataForCurrentPlayer+A0j ...
    currentPlayerEntry->nextLevelToPlay = nextLevelToPlay; // 0x7e = 126
}

void sub_4C407() //   proc near       ; CODE XREF: runMainMenu+5Dp
{
    // 01ED:57A4
    if (gLevelFailed != 0)
    {
        gLevelFailed = 0;
        drawFailedLevelResultScreen(); // 01ED:57B5
        drawMenuBackground(); // 01ED:57B8
        gShouldAutoselectNextLevelToPlay = 0;
        prepareLevelDataForCurrentPlayer();
        drawMenuTitleAndDemoLevelResult();
    //    mov si, 6015h
        fadeToPalette(gGamePalette);

        videoLoop();

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
    uint8_t *currentScreenPixels = malloc(kFullScreenFramebufferLength);
    memcpy(currentScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    drawMenuBackground();
    gShouldAutoselectNextLevelToPlay = 0;

    prepareLevelDataForCurrentPlayer();
    drawMenuTitleAndDemoLevelResult();

    uint8_t *menuScreenPixels = malloc(kFullScreenFramebufferLength);
    memcpy(menuScreenPixels, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 80;

    const uint32_t kAnimationDuration = kNumberOfSteps * 1000 / 70; // ~571 ms
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
        videoLoop();
    }

    free(currentScreenPixels);
    free(menuScreenPixels);

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

    drawTextWithChars6FontWithTransparentBackgroundIfPossible(128, 60, 0xF, "HARD LUCK!");
    if (gNumberOfRemainingInfotrons == 0)
    {
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(40, 80, 0xF, "YOU COMPLETED ALL THE NECESSARY INFOTRONS");
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(72, 100, 0xF, "BUT FAILED TO REACH THE EXIT");
    }
    else
    {
//loc_4C52C:              ; CODE XREF: drawFailedLevelResultScreen+19j
        char message[] = "YOU HAVE COLLECTED ??? OUT OF THE ???";

        uint8_t collectedInfotrons = gTotalNumberOfInfotrons - gNumberOfRemainingInfotrons;
        convertNumberTo3DigitPaddedString(collectedInfotrons, &message[19], 1);

        convertNumberTo3DigitPaddedString(gTotalNumberOfInfotrons, &message[34], 1);

        drawTextWithChars6FontWithTransparentBackgroundIfPossible(40, 80, 0xF, message);
        drawTextWithChars6FontWithTransparentBackgroundIfPossible(104, 100, 0xF, "INFOTRONS NEEDED");
    }

//loc_4C55C:              ; CODE XREF: drawFailedLevelResultScreen+31j
    drawTextWithChars6FontWithTransparentBackgroundIfPossible(72, 120, 0xF, "WHY NOT GIVE IT ANOTHER TRY?");

    videoLoop();
    setPalette(gInformationScreenPalette);
    if (gShouldExitGame != 1)
    {
        waitForKeyMouseOrJoystick();
    }

//loc_4C591:              ; CODE XREF: drawFailedLevelResultScreen+93j
    setPalette(gBlackPalette);
}

void scrollRightToNewScreen() // sub_4C5AF   proc near       ; CODE XREF: handleGfxTutorOptionClick+3p
{
    videoLoop();

    uint8_t *screenPixelsBackup = malloc(kFullScreenFramebufferLength);
    memcpy(screenPixelsBackup, gScreenPixels, kFullScreenFramebufferLength);

    const int kNumberOfSteps = 80;

    const uint32_t kAnimationDuration = kNumberOfSteps * 1000 / 70; // ~571 ms
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
        videoLoop();
    }

    free(screenPixelsBackup);
}

void handleOptionsStandardClick() // sub_4C705  proc near       ; CODE XREF: code:5ADBp
{
    activateInternalStandardSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsInternalClick() // loc_4C6FB
{
    handleOptionsStandardClick();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsSamplesClick() // sub_4C70F  proc near
{
    activateInternalSamplesSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsSoundBlasterClick() // sub_4C719  proc near
{
    activateSoundBlasterSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsAdlibClick() // sub_4C723  proc near
{
    activateAdlibSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsRolandClick() // sub_4C72D  proc near
{
    activateRolandSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsCombinedClick() // sub_4C737  proc near
{
    activateCombinedSound();
    playExplosionSound();
    drawSoundTypeOptionsSelection(gScreenPixels);
}

void handleOptionsMusicClick() // sub_4C741   proc near
{
    if (isMusicEnabled == 1)
    {
        stopMusic();
        isMusicEnabled = 0;
    }
    else
    {
//loc_4C752:              ; CODE XREF: handleOptionsAdlibClick+5j
        isMusicEnabled = 1;
        playMusicIfNeeded();
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
        playExplosionSound();
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
    // calibrateJoystick(); not needed anymore
    drawInputOptionsSelection(gScreenPixels);
}

void handleOptionsExitAreaClick() // loc_4C78D
{
    word_58463 = 1;
}

void runMainMenu() // proc near       ; CODE XREF: start+43Ap
{
    // 01ED:5B31
    gIsInMainMenu = 1;
    gHasUserInterruptedDemo = 0;
    gSelectedOriginalDemoLevelNumber = 0;
    gIsSPDemoAvailableToRun = 0;
    gAutomaticDemoPlaybackCountdown = 4200;
    if (word_58467 != 0)
    {
        drawMenuBackground(); // 01ED:5B4E
        gShouldAutoselectNextLevelToPlay = 1;
        prepareLevelDataForCurrentPlayer(); // 01ED:5B56
        drawMenuTitleAndDemoLevelResult(); // 01ED:5B59

        videoLoop();
        fadeToPalette(gGamePalette); // 6015h
        word_58467 = 0;
    }
    else
    {
//loc_4C7EC:              // ; CODE XREF: runMainMenu+1Bj
        byte_59B83 = 1;
        sub_4C407(); // 01ED:5B8E
    }

//loc_4C7F4:              // ; CODE XREF: runMainMenu+56j
    playMusicIfNeeded(); // 01ED:5B91
    saveLastMouseAreaBitmap();
    drawMouseCursor();

    while (1)
    {
        int9handler(0);

//loc_4C7FD:              // ; CODE XREF: runMainMenu+121j
                   // ; runMainMenu+219j ...
        gAutomaticDemoPlaybackCountdown--;
        if (gAutomaticDemoPlaybackCountdown == 0)
        {
            handleDemoOptionClick();
        }

//loc_4C806:              // ; CODE XREF: runMainMenu+6Dj
        if (gShouldLeaveMainMenu != 0)
        {
            gShouldLeaveMainMenu = 0;
            break;
        }

//loc_4C81A:              // ; CODE XREF: runMainMenu+77j
        videoLoop();

        gFrameCounter++;
        uint16_t mouseX, mouseY;
        uint16_t mouseButtonStatus;
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);
        gMouseButtonStatus = mouseButtonStatus;
        if (gMouseX != mouseX
            || gMouseY != mouseY)
        {
//loc_4C834:              // ; CODE XREF: runMainMenu+98j
            gAutomaticDemoPlaybackCountdown = 4200;
        }

//loc_4C83A:              // ; CODE XREF: runMainMenu+9Ej
        gMouseX = mouseX;
        gMouseY = mouseY;
        restoreLastMouseAreaBitmap(); // 01ED:5BDF
        saveLastMouseAreaBitmap(); // 01ED:5BE2
        drawMouseCursor(); // 01ED:5BE5 Draws mouse cursor too?
        drawMainMenuButtonBorders(); // 01ED:5BE8
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
        if (gCurrentUserInput > kUserInputSpaceAndDirectionOffset
            || isStartButtonPressed())
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
                 && strlen(demoFileName) != 0
                 && fileIsDemo == 1)
        {
            gIsSPDemoAvailableToRun = 1;
            playDemo(0);
        }
//loc_4C977:              // ; CODE XREF: runMainMenu+1C6j
                    // ; runMainMenu+1CDj ...
        else if (gIsF12KeyPressed == 1
                 && strlen(demoFileName) != 0)
        {
            gIsSPDemoAvailableToRun = 1;
            gShouldLeaveMainMenu = 1;
            gIsPlayingDemo = 0;
            gShouldUpdateTotalLevelTime = 0;
            gHasUserCheated = 1;
            prepareDemoRecordingFilename();
            // This adds dashes to the level name or something?
            gSPDemoFileName[3] = 0x2D; // '-' ; "001$0.SP"
            gSPDemoFileName[4] = 0x2D; // '-' ; "01$0.SP"
            gSPDemoFileName[5] = 0x2D; // '-' ; "1$0.SP"
            continue;
        }
//loc_4C9B0:              // ; CODE XREF: runMainMenu+131j
                   // ; runMainMenu+141j ...
        // if (gMouseButtonStatus == MouseButtonRight) // Right button -> exit game
        // {
        //     gShouldExitGame = 1;
        //     break;
        // }
        else if (getGameControllerButtonBack() // Select/Back/- controller button -> exit game
                 || gIsEscapeKeyPressed == 1)
        {
            runAdvancedOptionsRootMenu();
        }
        else if (isRotateLevelSetAscendingButtonPressed())
        {
            throttledRotateLevelSet(0);
            continue; // This allows the throttling effect to act
        }
        else if (isRotateLevelSetDescendingButtonPressed())
        {
            throttledRotateLevelSet(1);
            continue; // This allows the throttling effect to act
        }
        if (gShouldExitGame == 1)
        {
            break;
        }

        if (gMouseButtonStatus == MouseButtonLeft)
        {
//loc_4C9FF:              // ; CODE XREF: runMainMenu+236j
            gAutomaticDemoPlaybackCountdown = 4200;

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
            }
        }
        else
        {
            // Reset throttle counters
            gLevelListThrottleCurrentCounter = 0x10;
            gLevelListThrottleNextCounter = 0;
            gPlayerListThrottleCurrentCounter = 0x10;
            gPlayerListThrottleNextCounter = 0;
            gRankingListThrottleCurrentCounter = 0x10;
            gRankingListThrottleNextCounter = 0;
            gLevelSetRotationThrottleCurrentCounter = 0x10;
            gLevelSetRotationThrottleNextCounter = 0;
        }
    }

//loc_4CA34:              // ; CODE XREF: runMainMenu+223j
               // ; runMainMenu+22Aj ...
    gIsInMainMenu = 0;
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
    setPalette(gControlsScreenPalette);
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
        videoLoop(); // 01ED:5E04
        updateOptionsMenuState(gScreenPixels);
        gFrameCounter++;
        getMouseStatus(&mouseX, &mouseY, &mouseButtonStatus);

        gMouseButtonStatus = mouseButtonStatus;
        gMouseX = mouseX;
        gMouseY = mouseY;

        restoreLastMouseAreaBitmap();
        saveLastMouseAreaBitmap();
        drawMouseCursor();
        if (gMouseButtonStatus == MouseButtonRight)
        {
            break;
        }
        if (isMenuBackButtonPressed()) // Select/Back/- controller button -> go back
        {
            break;
        }
        if (word_58463 == 1)
        {
            break;
        }
        if (gMouseButtonStatus == MouseButtonLeft)
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
                        videoLoop();
                        gFrameCounter++;
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
    setPalette(gGamePalette);
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

void savePlayerListData() // sub_4CFB2   proc near       ; CODE XREF: handleNewPlayerOptionClick+1D5p
//                    ; handleDeletePlayerOptionClick+CEp ...
{
    if (gIsForcedCheatMode != 0)
    {
        return;
    }

    FILE *file = openWritableFile(gPlayerLstFilename, "wb");
    if (file == NULL)
    {
        return;
    }

    assert(sizeof(gPlayerListData) == 0xA00);

    fileWriteBytes(gPlayerListData, sizeof(gPlayerListData), file);

    fclose(file);
}

void saveHallOfFameData() //   proc near       ; CODE XREF: handleNewPlayerOptionClick+1D8p
//                    ; handleDeletePlayerOptionClick+D1p ...
{
    if (gIsForcedCheatMode != 0)
    {
        return;
    }

    FILE *file = openWritableFile(gHallfameLstFilename, "wb");
    if (file == NULL)
    {
        return;
    }

    assert(sizeof(gHallOfFameData) == 0x24);

    fileWriteBytes(gHallOfFameData, sizeof(gHallOfFameData), file);

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
    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
    if (currentPlayerEntry->completedAllLevels != 0)
    {
        return;
    }

//loc_4D1D2:              ; CODE XREF: updateHallOfFameEntries+19j
    int numberOfCompletedLevels = 0;

    for (int i = 0; i < kNumberOfLevels; ++i)
    {
//loc_4D1E2:              ; CODE XREF: updateHallOfFameEntries+33j
        if (currentPlayerEntry->levelState[i] == PlayerLevelStateCompleted)
        {
            numberOfCompletedLevels++;
        }
//loc_4D1E8:              ; CODE XREF: updateHallOfFameEntries+2Fj
    }

    if (numberOfCompletedLevels != kNumberOfLevels)
    {
        return;
    }

    currentPlayerEntry->completedAllLevels = 1;

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
        if (currentPlayerEntry->hours < entry.hours)
        {
            newEntryInsertIndex = i;
            break;
        }
        else if (currentPlayerEntry->hours == entry.hours)
        {
            if (currentPlayerEntry->minutes < entry.minutes)
            {
                newEntryInsertIndex = i;
                break;
            }
            else if (currentPlayerEntry->minutes == entry.minutes)
            {
                if (currentPlayerEntry->seconds < entry.seconds)
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
               currentPlayerEntry->name,
               sizeof(currentPlayerEntry->name));
        newEntry->hours = currentPlayerEntry->hours;
        newEntry->minutes = currentPlayerEntry->minutes;
        newEntry->seconds = currentPlayerEntry->seconds;
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
    if (gHasUserCheated != 0)
    {
        return;
    }
    uint8_t previousState = gCurrentPlayerLevelState;
    gCurrentPlayerLevelState = PlayerLevelStateNotCompleted;

    PlayerEntry *currentPlayerEntry = &gPlayerListData[gCurrentPlayerIndex];
    currentPlayerEntry->levelState[gCurrentSelectedLevelIndex - 1] = previousState;
    gCurrentSelectedLevelIndex++;
    updateHallOfFameEntries(); // 01ED:6618

    // Added by me to prevent losing progress when switching levelsets after finishing a level
    savePlayerListData();
    saveHallOfFameData();
}

void initializeFadePalette() //   proc near       ; CODE XREF: start+296p
{
    setPalette(gBlackPalette);
}

void initializeMouse() //   proc near       ; CODE XREF: start+299p
{
    gMouseX = kScreenWidth / 2;
    gMouseY = kScreenHeight / 2;
    if (getFullscreenMode())
    {
        centerMouse();
    }
    hideMouse();
    handleSystemEvents();
}

void getMouseStatus(uint16_t *mouseX, uint16_t *mouseY, uint16_t *mouseButtonStatus) //   proc near       ; CODE XREF: waitForKeyMouseOrJoystick:mouseIsClickedp
//                    ; waitForKeyMouseOrJoystick+3Ep ...
{
    // Returns coordinate X in CX (0-320) and coordinate Y in DX (0-200).
    // Also button status in BX.

    handleSystemEvents();

    int x, y;
    uint8_t leftButtonPressed,rightButtonPressed;

    getMouseState(&x, &y, &leftButtonPressed, &rightButtonPressed);

    int windowWidth, windowHeight;
    getWindowSize(&windowWidth, &windowHeight);

    float controllerX = 0, controllerY = 0;
    uint8_t controllerLeftButton = 0;
    uint8_t controllerRightButton = 0;
    gameControllerEmulateMouse(&controllerX,
                                &controllerY,
                                &controllerLeftButton,
                                &controllerRightButton);

    uint8_t shouldCorrectMousePosition = 0;

    if (controllerX != 0.0 || controllerY != 0.0)
    {
        float speed = (float) windowWidth / 1280;

        x += speed * controllerX;
        y += speed * controllerY;

        shouldCorrectMousePosition = 1;
    }

    // Read touch screen where available
    float touchScreenX, touchScreenY;
    uint8_t touchScreenPressed = readTouchScreen(&touchScreenX, &touchScreenY);
    if (touchScreenPressed)
    {
        x = touchScreenX * windowWidth;
        y = touchScreenY * windowHeight;

        shouldCorrectMousePosition = 1;
    }

    if (shouldCorrectMousePosition)
    {
        x = CLAMP(x, 0, windowWidth);
        y = CLAMP(y, 0, windowHeight);

        // Correct mouse position for future events
        moveMouse(x, y);
    }

    if (windowWidth != 0 && windowHeight != 0)
    {
        x = x * kScreenWidth / windowWidth;
        y = y * kScreenHeight / windowHeight;
    }

    leftButtonPressed = (leftButtonPressed
                            || controllerLeftButton
                            || touchScreenPressed);
    rightButtonPressed = (rightButtonPressed
                            || controllerRightButton);

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

    if (mouseButtonStatus != NULL)
    {
        *mouseButtonStatus = (rightButtonPressed << 1
                                | leftButtonPressed);
    }
}

#define COPY_LEVEL_DATA(__dest, __size) \
    do \
    { \
        memcpy(__dest, &levelFileData[pointer], __size); \
        pointer += __size; \
    } \
    while (0)

#define COPY_LEVEL_DATA_UINT16(__dest) \
    do \
    { \
        COPY_LEVEL_DATA(&__dest, sizeof(uint16_t)); \
        __dest = convert16LE(__dest); \
    } \
    while (0)

void mapLevelFileData(char *levelFileData, Level *level)
{
    size_t pointer = 0;

    COPY_LEVEL_DATA(level->tiles, sizeof(level->tiles));
    COPY_LEVEL_DATA(level->unused, sizeof(level->unused));
    COPY_LEVEL_DATA(&level->initialGravitation, sizeof(level->initialGravitation));
    COPY_LEVEL_DATA(&level->speedFixMagicNumber, sizeof(level->speedFixMagicNumber));
    COPY_LEVEL_DATA(level->name, sizeof(level->name));
    COPY_LEVEL_DATA(&level->freezeZonks, sizeof(level->freezeZonks));
    COPY_LEVEL_DATA(&level->numberOfInfotrons, sizeof(level->numberOfInfotrons));
    COPY_LEVEL_DATA(&level->numberOfSpecialPorts, sizeof(level->numberOfSpecialPorts));
    for (int idx = 0; idx < kLevelMaxNumberOfSpecialPorts; ++idx)
    {
        SpecialPortInfo *specialPortInfo = &level->specialPortsInfo[idx];
        COPY_LEVEL_DATA_UINT16(specialPortInfo->position);
        COPY_LEVEL_DATA(&specialPortInfo->gravity, sizeof(specialPortInfo->gravity));
        COPY_LEVEL_DATA(&specialPortInfo->freezeZonks, sizeof(specialPortInfo->freezeZonks));
        COPY_LEVEL_DATA(&specialPortInfo->freezeEnemies, sizeof(specialPortInfo->freezeEnemies));
        COPY_LEVEL_DATA(&specialPortInfo->unused, sizeof(specialPortInfo->unused));
    }
    COPY_LEVEL_DATA(&level->scrambledSpeed, sizeof(level->scrambledSpeed));
    COPY_LEVEL_DATA(&level->scrambledChecksum, sizeof(level->scrambledChecksum));
    COPY_LEVEL_DATA_UINT16(level->randomSeed);

    assert(pointer == kLevelDataLength);
}

#undef COPY_LEVEL_DATA_UINT16
#undef COPY_LEVEL_DATA

void readLevels() //  proc near       ; CODE XREF: start:loc_46F3Ep
                    // ; fetchAndInitializeLevelp
{
    // 01ED:68E5
    char *filename = "";
    FILE *file = NULL;
    char levelFileData[kLevelDataLength];

    if (gIsPlayingDemo != 0
        && (gSelectedOriginalDemoLevelNumber & 0xFF) == 0
        && gIsSPDemoAvailableToRun == 0)
    {
        // Demos with the new format
        Level *level = &gDemos.level[gDemoIndexOrDemoLevelNumber];

        memcpy(&levelFileData, level, kLevelDataLength);

        strcpy(gCurrentDemoLevelName, ".SP");

        memcpy(&gCurrentDemoLevelName[4], level->name, sizeof(level->name));
    }
    else
    {
        if (gIsPlayingDemo == 0
            || gIsSPDemoAvailableToRun != 0)
        {
//loc_4D59F:              ; CODE XREF: readLevels+5j
//                ; readLevels+13j
            filename = gLevelsDatFilename; // lea dx, aLevels_dat_0 ; "LEVELS.DAT"
        }

        if (gIsPlayingDemo != 0
            && (gSelectedOriginalDemoLevelNumber & 0xFF) != 0) //cmp byte ptr gSelectedOriginalDemoLevelNumber, 0
        {
//loc_4D599:              ; CODE XREF: readLevels+Cj
            filename = gLevelsDatFilename; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }
//loc_4D5A3:              ; CODE XREF: readLevels+55j
        else if (gIsSPDemoAvailableToRun != 0)
        {
            filename = demoFileName;
        }
        else if (gSelectedOriginalDemoFromCommandLineLevelNumber != 0)
        {
            filename = gLevelsDatFilename; // lea dx, aLevels_dat ; "LEVELS.DAT"
        }

//loc_4D5BB:              ; CODE XREF: readLevels+63j
        file = openWritableFileWithReadonlyFallback(filename, "rb");
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
        if (gIsSPDemoAvailableToRun != 0)
        {
            levelIndex = gSelectedOriginalDemoFromCommandLineLevelNumber;
            if (levelIndex == 0)
            {
                levelIndex++;
            }
        }

//loc_4D5E3:              ; CODE XREF: readLevels+91j
//                ; readLevels+98j
        levelIndex--; // Levels anywhere else are 1-index, we need them to start from 0 here
        size_t fileOffset = levelIndex * kLevelDataLength;
        // 01ED:699A
        int result = fseek(file, fileOffset, SEEK_SET);
        if (result != 0)
        {
            exitWithError("Error seeking %s\n", filename);
        }

//loc_4D604:              ; CODE XREF: readLevels+B7j
        // 01ED:69AE
        size_t bytes = fileReadBytes(levelFileData, kLevelDataLength, file);
        if (bytes < kLevelDataLength)
        {
            exitWithError("Error reading %s\n", filename);
        }

        Level tmpLevel;
        mapLevelFileData(levelFileData, &tmpLevel);
        gIsGravityEnabled = tmpLevel.initialGravitation;
        gAreZonksFrozen = tmpLevel.freezeZonks;
        gNumberOfInfoTrons = tmpLevel.numberOfInfotrons;
        gNumberOfSpecialPorts = tmpLevel.numberOfSpecialPorts;
        gRandomSeed = tmpLevel.randomSeed;

//loc_4D618:              ; CODE XREF: readLevels+CBj
        if ((gSelectedOriginalDemoLevelNumber & 0xFF) != 0) // cmp byte ptr gSelectedOriginalDemoLevelNumber, 0
        {
            gSelectedOriginalDemoLevelNumber |= 0xFF00; // mov byte ptr gSelectedOriginalDemoLevelNumber+1, 0FFh

            gDemoIndexOrDemoLevelNumber = gSelectedOriginalDemoIndex;

            Level *level = &gDemos.level[gDemoIndexOrDemoLevelNumber];
            mapLevelFileData(levelFileData, level);
        }
    }

    char *levelName = NULL;

//loc_4D64B:              ; CODE XREF: readLevels+4Ej
//                ; readLevels+D5j
    if (gIsPlayingDemo != 0)
    {
        gRandomGeneratorSeed = gRandomSeed;
        levelName = gCurrentDemoLevelName;
    }
    else
    {
//loc_4D65D:              ; CODE XREF: readLevels+108j
        levelName = gCurrentLevelName;
    }

//loc_4D660:              ; CODE XREF: readLevels+113j
    if (gSelectedOriginalDemoLevelNumber != 0
        || (gIsSPDemoAvailableToRun != 0
            && gSelectedOriginalDemoFromCommandLineLevelNumber != 0))
    {
//loc_4D679:              ; CODE XREF: readLevels+121j
        strcpy(gCurrentDemoLevelName, "BIN");
        levelName += 4;
    }
    else if (gIsSPDemoAvailableToRun == 0)
    {
//loc_4D68C:              ; CODE XREF: readLevels+128j
        levelName += 4; // Skips the number directly to the title (from pointing "005 ------- EASY DEAL -------" to pointing "------- EASY DEAL -------")
    }
    else if (gSelectedOriginalDemoFromCommandLineLevelNumber == 0)
    {
//loc_4D682:              ; CODE XREF: readLevels+12Fj
        strcpy(gCurrentDemoLevelName, ".SP");
        levelName += 4;
    }

//loc_4D68F:              ; CODE XREF: readLevels+142j
    mapLevelFileData(levelFileData, &gCurrentLevel);
    memcpy(levelName, gCurrentLevel.name, sizeof(gCurrentLevel.name));

    // The reason this is 1536 (level file size) and not 1440 (actual gamefield size of 60 * 24 tiles) is because
    // the game was written like this, and some levels rely on this behavior by removing the bottom border of the level
    // and using some unused bytes from those 1536 to store tile data.
    // An example of this is "HIDDEN TRACK" (07/07s062-1.sp)
    //
    for (int i = 0; i < kLevelDataLength; ++i)
    {
//loc_4D6B8:              ; CODE XREF: readLevels+172j
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        tile->tile = levelFileData[i];
        tile->state = 0;
    }

    memset(&gExplosionTimers, 0, sizeof(gExplosionTimers)); // rep stosb

    if (gIsPlayingDemo == 0
        || (gSelectedOriginalDemoLevelNumber & 0xFF) != 0
        || gIsSPDemoAvailableToRun != 0)
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
    gSelectedOriginalDemoLevelNumber &= 0xFF00; // mov byte ptr gSelectedOriginalDemoLevelNumber, 0
}

void soundShutdown() //  proc near       ; CODE XREF: start+48Ep
//                    ; loadScreen2-7DAp
{
    stopMusicAndSounds();
}

void activateInternalStandardSound() // loadBeep   proc near       ; CODE XREF: readConfig:loc_4751Ap
//                    ; readConfig:loc_47551p ...
{
    stopMusicAndSounds();
    setSoundType(SoundTypeInternalStandard, SoundTypeInternalStandard);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void activateInternalSamplesSound() // loadBeep2  proc near       ; CODE XREF: readConfig+4Cp handleOptionsSamplesClickp
{
    stopMusicAndSounds();
    setSoundType(SoundTypeInternalStandard, SoundTypeInternalSamples);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void activateAdlibSound() // loadAdlib  proc near       ; CODE XREF: readConfig+56p handleOptionsAdlibClickp
{
    stopMusicAndSounds(); // 01ED:6D06
    setSoundType(SoundTypeAdlib, SoundTypeAdlib);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void activateSoundBlasterSound() // loadBlaster  proc near       ; CODE XREF: readConfig+60p handleOptionsSoundBlasterClickp
{
    // 01ED:6D39
    stopMusicAndSounds();
    setSoundType(SoundTypeAdlib, SoundTypeSoundBlaster);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void activateRolandSound() // loadRoland  proc near       ; CODE XREF: readConfig+6Ap handleOptionsRolandClickp
{
    stopMusicAndSounds();
    setSoundType(SoundTypeRoland, SoundTypeRoland);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void activateCombinedSound() // loadCombined proc near       ; CODE XREF: readConfig+74p handleOptionsCombinedClickp
{
    stopMusicAndSounds();
    setSoundType(SoundTypeRoland, SoundTypeSoundBlaster);
    playMusicIfNeeded();
    gCurrentSoundPriority = 0;
    gCurrentSoundDuration = 0;
}

void stopMusicAndSounds() // sound1     proc near       ; CODE XREF: soundShutdown?+5p
                 //   ; code:6CC7p ...
{
    // 01ED:6E4E
    setSoundType(SoundTypeNone, SoundTypeNone);
}

void playMusicIfNeeded() // sound2    proc near       ; CODE XREF: start+39Bp start+410p ...
{
    // 01ED:6EA8
    if (isMusicEnabled != 1)
    {
        return;
    }

    playMusic();
}

void playExplosionSound() // sound4    proc near       ; CODE XREF: detonateBigExplosion+2EDp code:5ADEp ...
{
    if (isFXEnabled != 1)
    {
        return;
    }

//loc_4DB7F:              ; CODE XREF: playExplosionSound+5j
    if (gCurrentSoundPriority >= 5)
    {
        return;
    }

//loc_4DB87:              ; CODE XREF: playExplosionSound+Dj
    gCurrentSoundDuration = 0xF;
    gCurrentSoundPriority = 5;

    playSoundEffect(SoundEffectExplosion);
}

void playInfotronSound() // sound5    proc near       ; CODE XREF: update?:loc_4E55Cp
                //    ; update?:loc_4E588p ...
{
    if (isFXEnabled == 0)
    {
        return;
    }

//loc_4DBE8:              ; CODE XREF: playInfotronSound+5j
    if (gCurrentSoundPriority >= 5)
    {
        return;
    }

//loc_4DBF0:              ; CODE XREF: playInfotronSound+Dj
    gCurrentSoundDuration = 0xF;
    gCurrentSoundPriority = 4;

    playSoundEffect(SoundEffectInfotron);
}

void playPushSound() // sound6    proc near       ; CODE XREF: update?+B8Bp
                //    ; update?+136Cp
{
    if (isFXEnabled == 0)
    {
        return;
    }

//loc_4DC51:              ; CODE XREF: playPushSound+5j
    if (gCurrentSoundPriority >= 2)
    {
        return;
    }

//loc_4DC59:              ; CODE XREF: playPushSound+Dj
    gCurrentSoundDuration = 7;
    gCurrentSoundPriority = 2;
    playSoundEffect(SoundEffectPush);
}

void playFallSound() // sound7     proc near       ; CODE XREF: movefun:loc_48125p
//                    ; movefun2:loc_48573p
{
    if (isFXEnabled == 0)
    {
        return;
    }

//loc_4DCBA:              ; CODE XREF: playFallSound+5j
    if (gCurrentSoundPriority >= 2)
    {
        return;
    }

//loc_4DCC2:              ; CODE XREF: playFallSound+Dj
    gCurrentSoundDuration = 7;
    gCurrentSoundPriority = 2;
    playSoundEffect(SoundEffectFall);
}

void playBugSound() // sound8     proc near       ; CODE XREF: movefun7:loc_4A0ABp
{
    if (isFXEnabled == 0)
    {
        return;
    }

// loc_4DD23:              ; CODE XREF: playBugSound+5j
    if (gCurrentSoundPriority >= 3)
    {
        return;
    }

//loc_4DD2B:              ; CODE XREF: playBugSound+Dj
    gCurrentSoundDuration = 3;
    gCurrentSoundPriority = 3;

    playSoundEffect(SoundEffectBug);
}

void playBaseSound() // sound9    proc near       ; CODE XREF: runLevel+2F4p
                //    ; update?:loc_4E3E1p ...
{
    if (isFXEnabled == 0)
    {
        return;
    }

//xxxxxxxxdcdc:               ; CODE XREF: playBaseSound+5j
    if (gCurrentSoundPriority >= 1)
    {
        return;
    }

//loc_4DD94:              ; CODE XREF: playBaseSound+Dj
    gCurrentSoundDuration = 3;
    gCurrentSoundPriority = 1;

    playSoundEffect(SoundEffectBase);
}

void playExitSound() // sound10   proc near       ; CODE XREF: update?+7EBp
{
    if (isFXEnabled == 0)
    {
        return;
    }

//loc_4DDF5:              ; CODE XREF: playExitSound+5j
    gCurrentSoundDuration = 0xFA;
    gCurrentSoundPriority = 0xA;
    stopMusic();

    playSoundEffect(SoundEffectExit);
}

int16_t updateMurphy(int16_t position) // update?     proc near       ; CODE XREF: updateMovingObjects+Ep
{
    // 01ED:722D

    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];

    if (murphyTile->tile != LevelTileTypeMurphy)
    {
        gIsMurphyUpdated = 0;
        return position;
    }

//hasValidMurphy:              ; CODE XREF: update?+5j
    gIsMurphyUpdated = 1;
    gMurphyPreviousLocation = position;
    if (murphyTile->state != 0 || murphyTile->tile != LevelTileTypeMurphy)
    {
        return updateMurphyAnimation(position);
    }

//loc_4DEB4:              ; CODE XREF: update?+1Fj
    gScratchGravity = 0;

    if (gIsGravityEnabled != 0
        && aboveTile->tile != LevelTileTypePortUp
        && aboveTile->tile != LevelTileTypePortVertical
        && aboveTile->tile != LevelTileTypePort4Way
        && (belowTile->state == 0
            && belowTile->tile == LevelTileTypeSpace))
    {
        gScratchGravity = 1;
    }

//loc_4DEE1:              ; CODE XREF: update?+2Ej update?+35j ...
    UserInput userInput = gCurrentUserInput;

    if (userInput == UserInputNone)
    {
//loc_4DEED:              ; CODE XREF: update?+58j
        gPreviousUserInputWasNone =  1;
        if (gScratchGravity != 0)
        {
            MurphyAnimationDescriptor unknownMurphyData;
//loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
            if (gIsMurphyLookingLeft != 0)
            {
                unknownMurphyData = kMurphyAnimationDescriptors[3]; // dx = 0x0E2E;
            }
            else
            {
//loc_4E396:              ; CODE XREF: update?+4FFj
                unknownMurphyData = kMurphyAnimationDescriptors[4]; // dx = 0x0E3E;
            }

//loc_4E399:              ; CODE XREF: update?+504j
            belowTile->state = 3;
            belowTile->tile = LevelTileTypeMurphy;
            murphyTile->state = 3;
            murphyTile->tile = LevelTileTypeSpace;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
        }

//loc_4DEFC:              ; CODE XREF: update?+67j
        if ((gFrameCounter & 3) != 0)
        {
            return position;
        }

//loc_4DF05:              ; CODE XREF: update?+72j
        gMurphyYawnAndSleepCounter++;
        if (gMurphyYawnAndSleepCounter == 4)
        {
            // si = kMurphyStillSpriteCoordinates;
            drawMovingFrame(304, 132, position);

            return position;
        }
//loc_4DF1E:              ; CODE XREF: update?+7Ej
        else if (gMurphyYawnAndSleepCounter <= 0x01F4)
        {
            return position;
        }
//loc_4DF27:              ; CODE XREF: update?+94j
        else if (gMurphyYawnAndSleepCounter <= 0x020A)
        {
            // Yawning animation
            uint16_t currentFrame = gMurphyYawnAndSleepCounter - 0x1F4;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DF4A:              ; CODE XREF: update?+9Dj
        else if (gMurphyYawnAndSleepCounter <= 0x03E8)
        {
            return position;
        }
//loc_4DF53:              ; CODE XREF: update?+C0j
        else if (gMurphyYawnAndSleepCounter <= 0x03FE)
        {
            // Yawning animation
            uint16_t currentFrame = gMurphyYawnAndSleepCounter - 0x3E8;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DF76:              ; CODE XREF: update?+C9j
        else if (gMurphyYawnAndSleepCounter <= 0x0640)
        {
            return position;
        }
//loc_4DF7F:              ; CODE XREF: update?+ECj
        else if (gMurphyYawnAndSleepCounter <= 0x0656)
        {
            // Yawning animation
            uint16_t currentFrame = gMurphyYawnAndSleepCounter - 0x640;
            currentFrame = currentFrame >> 1;

            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[34];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
//loc_4DFA2:              ; CODE XREF: update?+F5j
        else if (gMurphyYawnAndSleepCounter > 0x0676)
        {
            return position;
        }
        else if (leftTile->state != 0 || leftTile->tile != LevelTileTypeSpace)
        {
//loc_4DFBF:              ; CODE XREF: update?+11Fj
            // Sleep to left animation
            uint16_t currentFrame = gMurphyYawnAndSleepCounter - 0x656;
            currentFrame = currentFrame >> 4;

            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[35];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
        else if (rightTile->state != 0 || rightTile->tile != LevelTileTypeSpace)
        {
//loc_4DFE0:              ; CODE XREF: update?+126j
            // Sleep to right animation
            uint16_t currentFrame = gMurphyYawnAndSleepCounter - 0x656;
            currentFrame = currentFrame >> 4;

            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[36];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);

            return position;
        }
        else
        {
            gMurphyYawnAndSleepCounter = 0x24; // 36
            return position;
        }
    }

//loc_4E001:              ; CODE XREF: update?+5Aj
    // 01ED:739E
    if (gScratchGravity != 0
        && (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace))
    {
        if (userInput != UserInputUp
            || (aboveTile->state != 0 || aboveTile->tile != LevelTileTypeBase))
        {
//loc_4E01B:              ; CODE XREF: update?+182j
            if (userInput != UserInputLeft
                || (leftTile->state != 0 || leftTile->tile != LevelTileTypeBase))
            {
//loc_4E027:              ; CODE XREF: update?+18Ej
                if (userInput != UserInputRight
                    || (rightTile->state != 0 || rightTile->tile != LevelTileTypeBase))
                {
//loc_4E033:              ; CODE XREF: update?+19Aj
                    userInput = UserInputDown;
                }
            }
        }
    }

//loc_4E035:              ; CODE XREF: update?+176j update?+17Dj ...
    // 01ED:73D2
    if (userInput == UserInputUp)
    {
        gPreviousUserInputWasNone = 0;
        return handleMurphyDirectionUp(position);
    }
//loc_4E041:              ; CODE XREF: update?+1A8j
    else if (userInput == UserInputLeft)
    {
        gPreviousUserInputWasNone = 0;
        return handleMurphyDirectionLeft(position);
    }
//loc_4E04E:              ; CODE XREF: update?+1B4j
    else if (userInput == UserInputDown)
    {
        gPreviousUserInputWasNone = 0;
        return handleMurphyDirectionDown(position);
    }
//loc_4E05B:              ; CODE XREF: update?+1C1j
    else if (userInput == UserInputRight)
    {
        gPreviousUserInputWasNone = 0;
        return handleMurphyDirectionRight(position);
    }
//loc_4E068:              ; CODE XREF: update?+1CEj
    else if (userInput == UserInputSpaceUp)
    {
        gPreviousUserInputWasNone = 0;
//loc_4E260:              ; CODE XREF: update?+1E2j
    //    mov ax, leveldata[si-78h]
        if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeBase)
        {
//loc_4E4BD:              ; CODE XREF: update?+3D9j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            playBaseSound();
//            dx = 0x0ECE;
            murphyTile->state = 0x10;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[13]);
        }
//loc_4E26C:              ; CODE XREF: update?+3D7j
        else if (aboveTile->tile == LevelTileTypeBug)
        {
//loc_4E4AC:              ; CODE XREF: update?+3E0j
            if (aboveTile->state < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E4B7:              ; CODE XREF: update?+621j
            aboveTile->state = 0;
            aboveTile->tile = LevelTileTypeBase;

//loc_4E4BD:              ; CODE XREF: update?+3D9j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            playBaseSound();
//            dx = 0x0ECE;
            murphyTile->state = 0x10;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[13]);
        }
//loc_4E273:              ; CODE XREF: update?+3DEj
        else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeInfotron)
        {
//loc_4E5F4:              ; CODE XREF: update?+3E8j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51840;
            drawMovingFrame(160, 64, position);
        //    pop si
            playInfotronSound();
//            dx = 0x0F6E;
            murphyTile->state = 0x14;
            aboveTile->state = 0xFF;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[23]);
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
            if (gAreYellowDisksDetonated != 0)
            {
                gMurphyYawnAndSleepCounter = 0xA;
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
            murphyTile->state = 0x20;
            aboveTile->state = 3;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[39]);
        }
        else
        {
            return position;
        }
    }
//loc_4E075:              ; CODE XREF: update?+1DBj
    else if (userInput == UserInputSpaceLeft)
    {
        gPreviousUserInputWasNone = 0;
//loc_4E28A:              ; CODE XREF: update?+1EFj
        gIsMurphyLookingLeft = 1;
    //    mov ax, [si+1832h]
        if (leftTile->state == 0 && leftTile->tile == LevelTileTypeBase)
        {
//loc_4E4E9:              ; CODE XREF: update?+409j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EDE;
            murphyTile->state = 0x11;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[14]);
        }
//loc_4E29C:              ; CODE XREF: update?+407j
        else if (leftTile->tile == LevelTileTypeBug)
        {
//loc_4E4D8:              ; CODE XREF: update?+410j
            if (leftTile->state < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E4E3:              ; CODE XREF: update?+64Dj
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeBase;

//loc_4E4E9:              ; CODE XREF: update?+409j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EDE;
            murphyTile->state = 0x11;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[14]);
        }
//loc_4E2A3:              ; CODE XREF: update?+40Ej
        else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeInfotron)
        {
//loc_4E614:              ; CODE XREF: update?+418j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51842;
            drawMovingFrame(208, 16, position);
        //    pop si
            playInfotronSound();
//            dx = 0x0F7E;
            murphyTile->state = 0x15;
            leftTile->state = 0xFF;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[24]);
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
            if (gAreYellowDisksDetonated != 0)
            {
                gMurphyYawnAndSleepCounter = 0xA;
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
            murphyTile->state = 0x21;
            leftTile->state = 3;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[40]);
        }
        else
        {
            return position;
        }
    }
//loc_4E082:              ; CODE XREF: update?+1E8j
    else if (userInput == UserInputSpaceDown)
    {
        gPreviousUserInputWasNone = 0;
//loc_4E2BA:              ; CODE XREF: update?+1FCj
    //    mov ax, [si+18ACh]
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeBase)
        {
//loc_4E515:              ; CODE XREF: update?+433j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EEE;
            murphyTile->state = 0x12;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[15]);
        }
//loc_4E2C6:              ; CODE XREF: update?+431j
        else if (belowTile->tile == LevelTileTypeBug)
        {
//loc_4E504:              ; CODE XREF: update?+43Aj
            if (belowTile->state < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E50F:              ; CODE XREF: update?+679j
            belowTile->state = 0;
            belowTile->tile = LevelTileTypeBase;

//loc_4E515:              ; CODE XREF: update?+433j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EEE;
            murphyTile->state = 0x12;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[15]);
        }
//loc_4E2CD:              ; CODE XREF: update?+438j
        else if (belowTile->state == 0 && belowTile->tile == LevelTileTypeInfotron)
        {
//loc_4E634:              ; CODE XREF: update?+442j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51844;
            drawMovingFrame(176, 64, position);
        //    pop si
            playInfotronSound();
//            dx = 0x0F8E;
            murphyTile->state = 0x16;
            belowTile->state = 0xFF;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[25]);
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
            if (gAreYellowDisksDetonated != 0)
            {
                gMurphyYawnAndSleepCounter = 0xA;
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
            murphyTile->state = 0x22;
            belowTile->state = 3;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[41]);
        }
        else
        {
            return position;
        }
    }
//loc_4E08F:              ; CODE XREF: update?+1F5j
    else if (userInput == UserInputSpaceRight)
    {
        gPreviousUserInputWasNone = 0;
//loc_4E2E4:              ; CODE XREF: update?+209j
        gIsMurphyLookingLeft = 0;
    //    mov ax, [si+1836h]
        if (rightTile->state == 0 && rightTile->tile == LevelTileTypeBase)
        {
//loc_4E541:              ; CODE XREF: update?+463j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EFE;
            murphyTile->state = 0x13;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[16]);
        }
//loc_4E2F6:              ; CODE XREF: update?+461j
        else if (rightTile->tile == LevelTileTypeBug)
        {
//loc_4E530:              ; CODE XREF: update?+46Aj
            if (rightTile->state < 0x80)
            {
                detonateBigExplosion(position);
                return position;
            }

//loc_4E53B:              ; CODE XREF: update?+6A5j
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeBase;

//loc_4E541:              ; CODE XREF: update?+463j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            playBaseSound();
//            dx = 0x0EFE;
            murphyTile->state = 0x13;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[16]);
        }
//loc_4E2FD:              ; CODE XREF: update?+468j
        else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeInfotron)
        {
//loc_4E654:              ; CODE XREF: update?+472j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            playInfotronSound();
//            dx = 0x0F9E;
            murphyTile->state = 0x17;
            rightTile->state = 0xFF;
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[26]);
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
            murphyTile->state = 0x23;
            rightTile->state = 3;

//loc_4E8F0:              ; CODE XREF: update?+4DAj update?+4F7j ...
            gMurphyCounterToStartPushAnimation = 0;
            gIsMurphyGoingThroughPortal = 0;
            return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[42]);
        }
        else
        {
//loc_4E790:              ; CODE XREF: update?+395j update?+479j
        //    push    si
        //    mov di, [si+6155h]
//            si = word_51846;
            drawMovingFrame(192, 16, position);
        //    pop si
            if (gAreYellowDisksDetonated != 0)
            {
                gMurphyYawnAndSleepCounter = 0xA;
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
    else if (userInput == UserInputSpaceOnly)
    {
//loc_4E314:              ; CODE XREF: update?+211j
        if (gNumberOfRemainingRedDisks == 0
            || gPlantedRedDiskCountdown != 0
            || gPreviousUserInputWasNone != 1)
        {
            return position;
        }
        murphyTile->state = 0x2A;
        gMurphyCounterToStartPushAnimation = 0x40; // 64
//        dx = 0x110E;
        gPlantedRedDiskCountdown = 1;
        gPlantedRedDiskPosition = position;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[49]);
    }
    else
    {
//loc_4E0A4:              ; CODE XREF: update?+20Fj
        gPreviousUserInputWasNone = 0;
        return position;
    }
}

int16_t handleMurphyDirectionUp(int16_t position)
{
    // 01ED:7447
    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *aboveAboveTile = &gCurrentLevelState[position - kLevelWidth * 2];

//loc_4E0AA:              ; CODE XREF: update?+1AFj update?+279j
    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        MurphyAnimationDescriptor animationDescriptor;

//loc_4E344:              ; CODE XREF: update?+223j
        if (gIsMurphyLookingLeft != 0)
        {
            animationDescriptor = kMurphyAnimationDescriptors[0]; // dx = 0x0DFE;
        }
        else
        {
//loc_4E350:              ; CODE XREF: update?+4B9j
            animationDescriptor = kMurphyAnimationDescriptors[1]; // dx = 0x0E0E;
        }

//loc_4E353:              ; CODE XREF: update?+4BEj
        aboveTile->state = 1;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;

        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, animationDescriptor);
    }
//loc_4E0B6:              ; CODE XREF: update?+221j
    else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeBase)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E3E1:              ; CODE XREF: update?+22Bj
        playBaseSound();
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[7]; // dx = 0x0E6E;
        }
        else
        {
//loc_4E3F0:              ; CODE XREF: update?+559j
            unknownMurphyData = kMurphyAnimationDescriptors[8]; // dx = 0x0E7E;
        }

//loc_4E3F3:              ; CODE XREF: update?+55Ej

        aboveTile->state = 5;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, unknownMurphyData);
    }
//loc_4E0BE:              ; CODE XREF: update?+229j
    else if (aboveTile->tile == LevelTileTypeBug)
    {
//loc_4E3D0:              ; CODE XREF: update?+232j
        //    cmp byte ptr [si+17BDh], 0
        //    jl  short loc_4E3DB
        if (aboveTile->state < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E3DB:              ; CODE XREF: update?+545j
        aboveTile->state = 0;
        aboveTile->tile = LevelTileTypeBase;

//loc_4E3E1:              ; CODE XREF: update?+22Bj
        MurphyAnimationDescriptor animationDescriptor;
        playBaseSound();
        if (gIsMurphyLookingLeft != 0)
        {
            animationDescriptor = kMurphyAnimationDescriptors[7]; // dx = 0x0E6E;
        }
        else
        {
//loc_4E3F0:              ; CODE XREF: update?+559j
            animationDescriptor = kMurphyAnimationDescriptors[8]; // dx = 0x0E7E;
        }

//loc_4E3F3:              ; CODE XREF: update?+55Ej

        aboveTile->state = 5;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, animationDescriptor);
    }
//loc_4E0C5:              ; CODE XREF: update?+230j
    else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeInfotron)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E55C:              ; CODE XREF: update?+23Aj
        playInfotronSound();
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[17]; // dx = 0x0F0E;
        }
        else
        {
//loc_4E56B:              ; CODE XREF: update?+6D4j
            unknownMurphyData = kMurphyAnimationDescriptors[18]; // dx = 0x0F1E;
        }

//loc_4E56E:              ; CODE XREF: update?+6D9j
        aboveTile->state = 9;
        aboveTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - kLevelWidth, unknownMurphyData);
    }
//loc_4E0CD:              ; CODE XREF: update?+238j
    else if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        playExitSound();
        byte_5A19B = 1;
        gCurrentPlayerLevelState = PlayerLevelStateCompleted;
        gLevelFailed = 0;
        if (gHasUserCheated == 0
            && gShouldUpdateTotalLevelTime != 0)
        {
            byte_5A323 = 1;
            addCurrentGameTimeToPlayer();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->state = 0xD;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[6]);
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
        if (gAreYellowDisksDetonated != 0)
        {
            gMurphyYawnAndSleepCounter = 0xA;
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
        if (aboveAboveTile->state != 0 || aboveAboveTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E7E6:              ; CODE XREF: update?+953j
//        dx = 0x0FCE;
        murphyTile->state = 0x18;
        aboveAboveTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[29]);
    }
//loc_4E0F1:              ; CODE XREF: update?+25Cj
    else if (aboveTile->tile == LevelTileTypeRedDisk)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E847:              ; CODE XREF: update?+265j
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[33]; // dx = 0x100E;
        }
        else
        {
//loc_4E853:              ; CODE XREF: update?+9BCj
            unknownMurphyData = kMurphyAnimationDescriptors[34]; // dx = 0x101E;
        }

//loc_4E856:              ; CODE XREF: update?+9C1j
        murphyTile->state = 0x1C;
        aboveTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, unknownMurphyData);
    }
//loc_4E0F8:              ; CODE XREF: update?+263j
    else if (aboveTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E8F9:              ; CODE XREF: update?+26Cj
        if (aboveAboveTile->state != 0 || aboveAboveTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E903:              ; CODE XREF: update?+A70j
        aboveAboveTile->state = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10AE;
        murphyTile->state = 0x24;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[43]);
    }
//loc_4E0FF:              ; CODE XREF: update?+26Aj
    else if (checkMurphyMovementToPosition(position - kLevelWidth, UserInputUp) != 1)
    {
        return handleMurphyDirectionUp(position);
    }
    else
    {
        return position;
    }
}

int16_t handleMurphyDirectionLeft(int16_t position)
{
    // 01ED:74A9
    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *leftLeftTile = &gCurrentLevelState[position - 2];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];

//loc_4E10C:              ; CODE XREF: update?+1BBj update?+2F3j
    gIsMurphyLookingLeft = 1;
//    mov ax, [si+1832h]
    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
//loc_4E36D:              ; CODE XREF: update?+28Bj
//        dx = 0x0E1E;
        leftTile->state = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, kMurphyAnimationDescriptors[2]);
    }
//loc_4E11E:              ; CODE XREF: update?+289j
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeBase) // 01ED:7634
    {
//loc_4E41E:              ; CODE XREF: update?+293j
        // 01ED:77BB
        playBaseSound();
//        dx = 0x0E8E;
        leftTile->state = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, kMurphyAnimationDescriptors[9]);
    }
//loc_4E126:              ; CODE XREF: update?+291j
    else if (leftTile->tile == LevelTileTypeBug) // 01ED:763B
    {
//loc_4E40D:              ; CODE XREF: update?+29Aj
        if (leftTile->state < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E418:              ; CODE XREF: update?+582j
        // 01ED:77B5
        leftTile->state = 0;
        leftTile->tile = LevelTileTypeBase;

//loc_4E41E:              ; CODE XREF: update?+293j
        playBaseSound();
//        dx = 0x0E8E;
        leftTile->state = 2;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, kMurphyAnimationDescriptors[9]);
    }
//loc_4E12D:              ; CODE XREF: update?+298j
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeInfotron)
    {
//loc_4E588:              ; CODE XREF: update?+2A2j
        playInfotronSound();
//        dx = 0x0F2E;
        leftTile->state = 10;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, kMurphyAnimationDescriptors[19]);
    }
//loc_4E135:              ; CODE XREF: update?+2A0j
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        playExitSound();
        byte_5A19B = 1;
        gCurrentPlayerLevelState = PlayerLevelStateCompleted;
        gLevelFailed = 0;
        if (gHasUserCheated == 0
            && gShouldUpdateTotalLevelTime != 0)
        {
            byte_5A323 = 1;
            addCurrentGameTimeToPlayer();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->state = 0xD;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[6]);
    }
//loc_4E13D:              ; CODE XREF: update?+2A8j
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeZonk)
    {
//loc_4E6BA:              ; CODE XREF: update?+2B2j
    //    mov ax, [si+1830h]
        if (leftLeftTile->state != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6C4:              ; CODE XREF: update?+831j
        leftLeftTile->state = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x0FAE;
        murphyTile->state = 0xE;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[27]);
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
        if (gAreYellowDisksDetonated != 0)
        {
            gMurphyYawnAndSleepCounter = 0xA;
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
        if (leftLeftTile->state != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E7FD:              ; CODE XREF: update?+96Aj
//        dx = 0x0FDE;
        murphyTile->state = 0x19;
        leftLeftTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[30]);
    }
//loc_4E161:              ; CODE XREF: update?+2CCj
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeRedDisk)
    {
//loc_4E863:              ; CODE XREF: update?+2D6j
//        dx = 0x102E;
        leftTile->state = 0x1D;
        leftTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position - 1, kMurphyAnimationDescriptors[35]);
    }
//loc_4E169:              ; CODE XREF: update?+2D4j
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E920:              ; CODE XREF: update?+2DEj
        if (leftLeftTile->state != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E92A:              ; CODE XREF: update?+A97j
        leftLeftTile->state = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x10BE;
        murphyTile->state = 0x25;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[44]);
    }
//loc_4E171:              ; CODE XREF: update?+2DCj
    else if (leftTile->state == 0 && leftTile->tile == LevelTileTypeOrangeDisk)
    {
//loc_4E993:              ; CODE XREF: update?+2E6j
        if (leftLeftTile->state != 0 || leftLeftTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E99D:              ; CODE XREF: update?+B0Aj
        leftLeftTile->state = 8;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157A;
        drawMovingFrame(64, 132, position);
    //    pop si
//        dx = 0x10EE;
        murphyTile->state = 0x28;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[47]);
    }
//loc_4E179:              ; CODE XREF: update?+2E4j
    else if (checkMurphyMovementToPosition(position - 1, UserInputLeft) != 1)
    {
        return handleMurphyDirectionLeft(position);
    }
    else
    {
        return position;
    }
}

int16_t handleMurphyDirectionDown(int16_t position)
{
    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *belowBelowTile = &gCurrentLevelState[position + kLevelWidth * 2];

//loc_4E186:              ; CODE XREF: update?+1C8j update?+355j
//    mov ax, leveldata[si+78h]
    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[3]; // dx = 0x0E2E;
        }
        else
        {
//loc_4E396:              ; CODE XREF: update?+4FFj
            unknownMurphyData = kMurphyAnimationDescriptors[4]; // dx = 0x0E3E;
        }

//loc_4E399:              ; CODE XREF: update?+504j
        belowTile->state = 3;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E192:              ; CODE XREF: update?+2FDj
    else if (belowTile->state == 0 && belowTile->tile == LevelTileTypeBase)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E44F:              ; CODE XREF: update?+307j
        playBaseSound();
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[10]; // dx = 0x0E9E;
        }
        else
        {
//loc_4E45E:              ; CODE XREF: update?+5C7j
            unknownMurphyData = kMurphyAnimationDescriptors[11]; // dx = 0x0EAE;
        }

//loc_4E461:              ; CODE XREF: update?+5CCj
        belowTile->state = 7;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E19A:              ; CODE XREF: update?+305j
    else if (belowTile->tile == LevelTileTypeBug)
    {
//loc_4E43E:              ; CODE XREF: update?+30Ej
        if (belowTile->state < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E449:              ; CODE XREF: update?+5B3j
        belowTile->state = 0;
        belowTile->tile = LevelTileTypeBase;

        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E44F:              ; CODE XREF: update?+307j
        playBaseSound();
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[10]; // dx = 0x0E9E;
        }
        else
        {
//loc_4E45E:              ; CODE XREF: update?+5C7j
            unknownMurphyData = kMurphyAnimationDescriptors[11]; // dx = 0x0EAE;
        }

//loc_4E461:              ; CODE XREF: update?+5CCj
        belowTile->state = 7;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + kLevelWidth, unknownMurphyData);
    }
//loc_4E1A1:              ; CODE XREF: update?+30Cj
    else if (belowTile->tile == LevelTileTypeInfotron)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E5A8:              ; CODE XREF: update?+316j
        playInfotronSound();
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[20]; // dx = 0x0F3E;
        }
        else
        {
//loc_4E5B7:              ; CODE XREF: update?+720j
            unknownMurphyData = kMurphyAnimationDescriptors[21]; // dx = 0x0F4E;
        }

//loc_4E5BA:              ; CODE XREF: update?+725j
        belowTile->state = 11;
        belowTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeBase;
        gMurphyCounterToStartPushAnimation = 0;
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
        playExitSound();
        byte_5A19B = 1;
        gCurrentPlayerLevelState = PlayerLevelStateCompleted;
        gLevelFailed = 0;
        if (gHasUserCheated == 0
            && gShouldUpdateTotalLevelTime != 0)
        {
            byte_5A323 = 1;
            addCurrentGameTimeToPlayer();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
    //    pop si
//        dx = 0x0E5E;
        murphyTile->state = 0xD;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[6]);
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
        if (gAreYellowDisksDetonated != 0)
        {
            gMurphyYawnAndSleepCounter = 0xA;
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
        if (belowBelowTile->state != 0 || belowBelowTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E814:              ; CODE XREF: update?+981j
//        dx = 0x0FEE;
        murphyTile->state = 0x1A;
        belowBelowTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[31]);
    }
//loc_4E1CD:              ; CODE XREF: update?+338j
    else if (belowTile->tile == LevelTileTypeRedDisk)
    {
        MurphyAnimationDescriptor unknownMurphyData;

//loc_4E87F:              ; CODE XREF: update?+341j
        if (gIsMurphyLookingLeft != 0)
        {
            unknownMurphyData = kMurphyAnimationDescriptors[36]; // dx = 0x103E;
        }
        else
        {
//loc_4E88B:              ; CODE XREF: update?+9F4j
            unknownMurphyData = kMurphyAnimationDescriptors[37]; // dx = 0x104E;
        }

//loc_4E88E:              ; CODE XREF: update?+9F9j
        murphyTile->state = 0x1E;
        belowTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, unknownMurphyData);
    }
//loc_4E1D4:              ; CODE XREF: update?+33Fj
    else if (belowTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E947:              ; CODE XREF: update?+348j
        if (belowBelowTile->state != 0 || belowBelowTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E951:              ; CODE XREF: update?+ABEj
        belowBelowTile->state = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10CE;
        murphyTile->state = 0x27;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[45]);
    }
//loc_4E1DB:              ; CODE XREF: update?+346j
    else if (checkMurphyMovementToPosition(position + kLevelWidth, UserInputDown) != 1)
    {
        return handleMurphyDirectionDown(position);
    }
    else
    {
        return position;
    }
}

int16_t handleMurphyDirectionRight(int16_t position)
{
    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *rightRightTile = &gCurrentLevelState[position + 2];
    StatefulLevelTile *belowRightTile = &gCurrentLevelState[position + kLevelWidth + 1];

//loc_4E1E8:              ; CODE XREF: update?+1D5j update?+3CDj
    gIsMurphyLookingLeft = 0;
//    mov ax, leveldata[si+2]
    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
//loc_4E3B3:              ; CODE XREF: update?+367j
//        dx = 0x0E4E;
        rightTile->state = 4;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, kMurphyAnimationDescriptors[5]);
    }
//loc_4E1FA:              ; CODE XREF: update?+365j
    else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeBase)
    {
//loc_4E48C:              ; CODE XREF: update?+36Fj
        playBaseSound();
//        dx = 0x0EBE;
        rightTile->state = 8;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, kMurphyAnimationDescriptors[12]);
    }
//loc_4E202:              ; CODE XREF: update?+36Dj
    else if (rightTile->tile == LevelTileTypeBug)
    {
//loc_4E47B:              ; CODE XREF: update?+376j
        if (rightTile->state < 0x80)
        {
            detonateBigExplosion(position);
            return position;
        }

//loc_4E486:              ; CODE XREF: update?+5F0j
        rightTile->state = 0;
        rightTile->tile = LevelTileTypeBase;

//loc_4E48C:              ; CODE XREF: update?+36Fj
        playBaseSound();
//        dx = 0x0EBE;
        rightTile->state = 8;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, kMurphyAnimationDescriptors[12]);
    }
//loc_4E209:              ; CODE XREF: update?+374j
    else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeInfotron)
    {
//loc_4E5D4:              ; CODE XREF: update?+37Ej
        playInfotronSound();
//        dx = 0x0F5E;
        rightTile->state = 12;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, kMurphyAnimationDescriptors[22]);
    }
//loc_4E211:              ; CODE XREF: update?+37Cj
    else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeExit)
    {
//loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        if (gNumberOfRemainingInfotrons != 0)
        {
            return position;
        }
        playExitSound();
        byte_5A19B = 1;
        gCurrentPlayerLevelState = PlayerLevelStateCompleted;
        gLevelFailed = 0;
        if (gHasUserCheated == 0
            && gShouldUpdateTotalLevelTime != 0)
        {
            byte_5A323 = 1;
            addCurrentGameTimeToPlayer();
        }

//loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        changePlayerCurrentLevelState();
        gQuitLevelCountdown = 0x40;
        murphyTile->state = 0xD;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[6]);
    }
//loc_4E219:              ; CODE XREF: update?+384j
    else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeZonk)
    {
//loc_4E6E1:              ; CODE XREF: update?+38Ej
    //    mov ax, [si+1838h]
        if (rightRightTile->state != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6EB:              ; CODE XREF: update?+858j
    //    mov ax, [si+18AEh]
        if (belowRightTile->state == 0 && belowRightTile->tile == LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E6F5:              ; CODE XREF: update?+862j
        rightRightTile->state = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x0FBE;
        murphyTile->state = 0xF;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[28]);
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
        if (gAreYellowDisksDetonated != 0)
        {
            gMurphyYawnAndSleepCounter = 0xA;
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
        if (rightRightTile->state != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E82B:              ; CODE XREF: update?+998j
//        dx = 0x0FFE;
        murphyTile->state = 0x1B;
        rightRightTile->state = 3;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 1;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[32]);
    }
//loc_4E23D:              ; CODE XREF: update?+3A8j
    else if (rightTile->tile == LevelTileTypeRedDisk)
    {
//loc_4E89A:              ; CODE XREF: update?+3B1j
//        dx = 0x105E;
        rightTile->state = 0x1F;
        rightTile->tile = LevelTileTypeMurphy;
        murphyTile->state = 3;
        murphyTile->tile = LevelTileTypeSpace;
        gMurphyCounterToStartPushAnimation = 0;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position + 1, kMurphyAnimationDescriptors[38]);
    }
//loc_4E244:              ; CODE XREF: update?+3AFj
    else if (rightTile->tile == LevelTileTypeYellowDisk)
    {
//loc_4E96D:              ; CODE XREF: update?+3B8j
        if (rightRightTile->state != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E977:              ; CODE XREF: update?+AE4j
        rightRightTile->state = 0x12;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10DE;
        murphyTile->state = 0x26;
        gMurphyCounterToStartPushAnimation = 8;
        gIsMurphyGoingThroughPortal = 0;
        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[46]);
    }
//loc_4E24B:              ; CODE XREF: update?+3B6j
    else if (rightTile->state == 0 && rightTile->tile == LevelTileTypeOrangeDisk)
    {
//loc_4E9B9:              ; CODE XREF: update?+3C0j
        if (rightRightTile->state != 0 || rightRightTile->tile != LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E9C3:              ; CODE XREF: update?+B30j
        if (belowRightTile->state == 0 && belowRightTile->tile == LevelTileTypeSpace)
        {
            return position;
        }

//loc_4E9CD:              ; CODE XREF: update?+B3Aj
        rightRightTile->state = 1;
    //    push    si
    //    mov di, [si+6155h]
//        si = word_5157C;
        drawMovingFrame(97, 132, position);
    //    pop si
//        dx = 0x10FE;
        murphyTile->state = 0x29;

//loc_4E9E7:              ; CODE XREF: update?+84Ej update?+87Fj ...
        gMurphyCounterToStartPushAnimation = 8;

//loc_4E9ED:              ; CODE XREF: update?+A66j
        gIsMurphyGoingThroughPortal = 0;

        return updateMurphyAnimationInfo(position, kMurphyAnimationDescriptors[48]);
    }
//loc_4E253:              ; CODE XREF: update?+3BEj
    else if (checkMurphyMovementToPosition(position + 1, UserInputRight) != 1)
    {
        return handleMurphyDirectionRight(position);
    }
    else
    {
        return position;
    }
}

int16_t updateMurphyAnimationInfo(int16_t position, MurphyAnimationDescriptor unknownMurphyData)
{
    // 01ED:7D9F

//loc_4E9F3:              ; CODE XREF: update?+4B0j update?+9B4j
//    di = 0x0DE0;
//    memcpy(di, si, 7 * 2); // rep movsw

    gCurrentMurphyAnimation = unknownMurphyData;

    return updateMurphyAnimation(position);
}

int16_t updateMurphyAnimation(int16_t position)
{
    // 01ED:7DA4

    StatefulLevelTile *murphyTile = &gCurrentLevelState[position];
    StatefulLevelTile *leftLeftTile = &gCurrentLevelState[position - 2];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *rightRightTile = &gCurrentLevelState[position + 2];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *belowBelowTile = &gCurrentLevelState[position + kLevelWidth * 2];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *aboveAboveTile = &gCurrentLevelState[position - kLevelWidth * 2];
    StatefulLevelTile *belowRightRightTile = &gCurrentLevelState[position + kLevelWidth + 2];

//loc_4EA07:              ; CODE XREF: update?+21j
    gMurphyYawnAndSleepCounter = 0;

    if (gMurphyCounterToStartPushAnimation == 0)
    {
        // 01ED:7E08
        uint8_t currentFrame = gCurrentMurphyAnimation.currentFrame;
        AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[gCurrentMurphyAnimation.animationIndex];
        Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

//loc_4EA6B:              ; CODE XREF: update?+B83j
        gMurphyPositionX += gCurrentMurphyAnimation.speedX;
        gMurphyPositionY += gCurrentMurphyAnimation.speedY;
        gCurrentMurphyAnimation.currentFrame++;

        uint16_t dstX = (position % kLevelWidth) * kTileSize;
        uint16_t dstY = (position / kLevelWidth) * kTileSize;

        int16_t offsetX = (gCurrentMurphyAnimation.animationCoordinatesOffset % 122) * 8;
        int16_t offsetY = (gCurrentMurphyAnimation.animationCoordinatesOffset / 122);

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
            AnimationFrameCoordinates animationFrameCoordinates = kMurphyAnimationFrameCoordinates[gCurrentMurphyAnimation.animationIndex + 1];
            Point frameCoordinates = animationFrameCoordinates.coordinates[currentFrame];

            int16_t offsetX = (gCurrentMurphyAnimation.animationCoordinatesOffsetIncrement % 122) * 8;
            int16_t offsetY = (gCurrentMurphyAnimation.animationCoordinatesOffsetIncrement / 122);

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
            gCurrentMurphyAnimation.animationCoordinatesOffset += gCurrentMurphyAnimation.animationCoordinatesOffsetIncrement;
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
        uint8_t previousMurphyMovingObject = murphyTile->state;
        murphyTile->state = 0;
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
                aboveTile->state = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->state = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB46:              ; CODE XREF: update?+CB1j
        else if (previousMurphyMovingObject == 4)
        {
//loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
            handleMurphyCollisionAfterMovement(position - 1);
            murphyTile->state = 0;
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
                aboveTile->state = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->state = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB66:              ; CODE XREF: update?+CD1j
        else if (previousMurphyMovingObject == 8)
        {
//loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
            // 01ED:82F0
            handleMurphyCollisionAfterMovement(position - 1);
            murphyTile->state = 0;
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
                aboveTile->state = 0;
                aboveTile->tile = LevelTileTypeSpace;
            }

//loc_4ECDC:              ; CODE XREF: update?+E44j
            murphyTile->state = 0;
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
            murphyTile->state = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            return position;
        }
//loc_4EB8E:              ; CODE XREF: update?+CF9j
        else if (previousMurphyMovingObject == 14)
        {
//loc_4ECE3:              ; CODE XREF: update?+D03j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4ECF0:              ; CODE XREF: update?+E58j
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->state = 0;
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4ED13:              ; CODE XREF: update?+E7Bj
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->state = 0;
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
                aboveTile->state = 0;
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
                leftTile->state = 0;
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
                rightTile->state = 0;
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
                belowTile->state = 0;
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
                aboveTile->state = 0;
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
                leftTile->state = 0;
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
                rightTile->state = 0;
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
                belowTile->state = 0;
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4EFE0:              ; CODE XREF: update?+1148j
            aboveAboveTile->state = 0;
            aboveAboveTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position -= kLevelWidth * 2;
            if (aboveTile->state == 1)
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F00E:              ; CODE XREF: update?+1176j
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position -= 2;
            if (leftTile->state == 1)
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F03B:              ; CODE XREF: update?+11A3j
            belowBelowTile->state = 0;
            belowBelowTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position += kLevelWidth * 2;
            if (belowTile->state == 1)
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F069:              ; CODE XREF: update?+11D1j
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeMurphy;
            gIsMurphyGoingThroughPortal = 0;
            position += 2;
            if (rightTile->state == 1)
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F096:              ; CODE XREF: update?+11FEj
            position -= kLevelWidth;

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            aboveTile->state = 0;
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
                rightTile->state = 0;
                rightTile->tile = LevelTileTypeSpace;
            }

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            murphyTile->state = 0;
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F0B9:              ; CODE XREF: update?+1221j
            position += kLevelWidth;

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            belowTile->state = 0;
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
                leftTile->state = 0;
                leftTile->tile = LevelTileTypeSpace;
            }

//loc_4FDAF:              ; CODE XREF: update?+1209j
//            ; update?:loc_4F0A9j ...
            murphyTile->state = 0;
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
                aboveTile->state = 0;
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
                leftTile->state = 0;
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
                belowTile->state = 0;
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
                rightTile->state = 0;
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
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F138:              ; CODE XREF: update?+12A0j
            aboveTile->state = 0;
            aboveTile->tile = LevelTileTypeMurphy;
            aboveAboveTile->state = 0;
            aboveAboveTile->tile = LevelTileTypeYellowDisk;
            return position - kLevelWidth;
        }
//loc_4EC4E:              ; CODE XREF: update?+DB9j
        else if (previousMurphyMovingObject == 37)
        {
//loc_4F148:              ; CODE XREF: update?+DC3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F155:              ; CODE XREF: update?+12BDj
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeYellowDisk;
            return position - 1;
        }
//loc_4EC56:              ; CODE XREF: update?+DC1j
        else if (previousMurphyMovingObject == 39)
        {
//loc_4F165:              ; CODE XREF: update?+DCBj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F172:              ; CODE XREF: update?+12DAj
            belowTile->state = 0;
            belowTile->tile = LevelTileTypeMurphy;
            belowBelowTile->state = 0;
            belowBelowTile->tile = LevelTileTypeYellowDisk;
            return position + kLevelWidth;
        }
//loc_4EC5E:              ; CODE XREF: update?+DC9j
        else if (previousMurphyMovingObject == 38)
        {
//loc_4F182:              ; CODE XREF: update?+DD3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F18F:              ; CODE XREF: update?+12F7j
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeYellowDisk;
            return position + 1;
        }
//loc_4EC66:              ; CODE XREF: update?+DD1j
        else if (previousMurphyMovingObject == 40)
        {
//loc_4F19F:              ; CODE XREF: update?+DDBj
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F1AC:              ; CODE XREF: update?+1314j
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeMurphy;
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeOrangeDisk;
            return position - 1;
        }
//loc_4EC6E:              ; CODE XREF: update?+DD9j
        else if (previousMurphyMovingObject == 41)
        {
//loc_4F1BC:              ; CODE XREF: update?+DE3j
            if (murphyTile->tile != LevelTileTypeExplosion)
            {
                murphyTile->state = 0;
                murphyTile->tile = LevelTileTypeSpace;
            }

//loc_4F1C9:              ; CODE XREF: update?+1331j
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeMurphy;
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeOrangeDisk;
            if (belowRightRightTile->state == 0 && belowRightRightTile->tile == LevelTileTypeSpace)
            {
                rightRightTile->state = 0x20;
                belowRightRightTile->state = 8;
            }

//loc_4F1E6:              ; CODE XREF: update?+134Aj
            return position + 1;
        }
//loc_4EC76:              ; CODE XREF: update?+DE1j
        else if (previousMurphyMovingObject == 42)
        {
//loc_4F1EA:              ; CODE XREF: update?+DEBj
            murphyTile->state = 0;
            murphyTile->tile = LevelTileTypeMurphy;
            gPlantedRedDiskCountdown = 2;
            gNumberOfRemainingRedDisks--;
            drawNumberOfRemainingRedDisks();
            playPushSound();
            return position;
        }
        else
        {
//loc_4EC7E:              ; CODE XREF: update?+DE9j
            gShouldExitLevel = 1;
            return position;
        }
    }

    gMurphyCounterToStartPushAnimation--;
    if (gMurphyCounterToStartPushAnimation == 0)
    {
        playPushSound();
    }

//loc_4EA1E:              ; CODE XREF: update?+B89j
    if (murphyTile->state == 0xE)
    {
//loc_4ED49:              ; CODE XREF: update?+B97j
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->state == 0 && leftTile->tile == LevelTileTypeZonk))
        {
            return position;
        }

//loc_4ED5A:              ; CODE XREF: update?+EC0j update?+EC7j
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->state = 0;
        leftTile->tile = LevelTileTypeZonk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4ED73:              ; CODE XREF: update?+EDBj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA2A:              ; CODE XREF: update?+B95j
    else if (murphyTile->state == 0xF)
    {
//loc_4ED81:              ; CODE XREF: update?+B9Fj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->state == 0 && rightTile->tile == LevelTileTypeZonk))
        {
            return position;
        }

//loc_4ED92:              ; CODE XREF: update?+EF8j update?+EFFj
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->state = 0;
        rightTile->tile = LevelTileTypeZonk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EDAB:              ; CODE XREF: update?+F13j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA32:              ; CODE XREF: update?+B9Dj
    else if (murphyTile->state == 0x28)
    {
//loc_4EDB9:              ; CODE XREF: update?+BA7j
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->state == 0 && leftTile->tile == LevelTileTypeOrangeDisk))
        {
            return position;
        }

//loc_4EDCA:              ; CODE XREF: update?+F30j update?+F37j
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->state = 0;
        leftTile->tile = LevelTileTypeOrangeDisk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4EDE3:              ; CODE XREF: update?+F4Bj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA3A:              ; CODE XREF: update?+BA5j
    else if (murphyTile->state == 0x29)
    {
//loc_4EDF1:              ; CODE XREF: update?+BAFj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->state == 0 && rightTile->tile == LevelTileTypeOrangeDisk))
        {
            return position;
        }

//loc_4EE02:              ; CODE XREF: update?+F68j update?+F6Fj
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->state = 0;
        rightTile->tile = LevelTileTypeOrangeDisk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EE1B:              ; CODE XREF: update?+F83j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA42:              ; CODE XREF: update?+BADj
    else if (murphyTile->state == 0x24)
    {
//loc_4EE29:              ; CODE XREF: update?+BB7j
        if (gCurrentUserInput == UserInputUp
            && (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EE3A:              ; CODE XREF: update?+FA0j update?+FA7j
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        aboveTile->state = 0;
        aboveTile->tile = LevelTileTypeYellowDisk;
        if (aboveAboveTile->tile != LevelTileTypeExplosion)
        {
            aboveAboveTile->state = 0;
            aboveAboveTile->tile = LevelTileTypeSpace;
        }

//loc_4EE53:              ; CODE XREF: update?+FBBj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA4A:              ; CODE XREF: update?+BB5j
    else if (murphyTile->state == 0x25)
    {
//loc_4EE61:              ; CODE XREF: update?+BBFj
        if (gCurrentUserInput == UserInputLeft
            && (leftTile->state == 0 && leftTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EE72:              ; CODE XREF: update?+FD8j update?+FDFj
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        leftTile->state = 0;
        leftTile->tile = LevelTileTypeYellowDisk;
        if (leftLeftTile->tile != LevelTileTypeExplosion)
        {
            leftLeftTile->state = 0;
            leftLeftTile->tile = LevelTileTypeSpace;
        }

//loc_4EE8B:              ; CODE XREF: update?+FF3j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA52:              ; CODE XREF: update?+BBDj
    else if (murphyTile->state == 0x27)
    {
//loc_4EE99:              ; CODE XREF: update?+BC7j
        if (gCurrentUserInput == UserInputDown
            && (belowTile->state == 0 && belowTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EEAA:              ; CODE XREF: update?+1010j
//                ; update?+1017j
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        belowTile->state = 0;
        belowTile->tile = LevelTileTypeYellowDisk;
        if (belowBelowTile->tile != LevelTileTypeExplosion)
        {
            belowBelowTile->state = 0;
            belowBelowTile->tile = LevelTileTypeSpace;
        }

//loc_4EEC3:              ; CODE XREF: update?+102Bj
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA5A:              ; CODE XREF: update?+BC5j
    else if (murphyTile->state == 0x26)
    {
//loc_4EED1:              ; CODE XREF: update?+BCFj
        if (gCurrentUserInput == UserInputRight
            && (rightTile->state == 0 && rightTile->tile == LevelTileTypeYellowDisk))
        {
            return position;
        }

//loc_4EEE2:              ; CODE XREF: update?+1048j
//                ; update?+104Fj
        murphyTile->state = 0;
        murphyTile->tile = LevelTileTypeMurphy;
        rightTile->state = 0;
        rightTile->tile = LevelTileTypeYellowDisk;
        if (rightRightTile->tile != LevelTileTypeExplosion)
        {
            rightRightTile->state = 0;
            rightRightTile->tile = LevelTileTypeSpace;
        }

//loc_4EEFB:              ; CODE XREF: update?+1063j
        // si = kMurphyStillSpriteCoordinates;
        drawMovingFrame(304, 132, position);

        return position;
    }
//loc_4EA62:              ; CODE XREF: update?+BCDj
    else if (murphyTile->state == 0x2A)
    {
//loc_4EF09:              ; CODE XREF: update?+BD7j
        if (gCurrentUserInput == UserInputSpaceOnly)
        {
            if (gMurphyCounterToStartPushAnimation > 0x20)
            {
                return position;
            }

            // si = word_51790;
            drawMovingFrame(288, 132, position);
            gPlantedRedDiskCountdown = 1;

            return position;
        }

//loc_4EF2C:              ; CODE XREF: update?+1080j
        murphyTile->state = 0;
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
    gTerminalMaxFramesToNextScroll = 7;
    gAreYellowDisksDetonated = 1;

    for (int i = 0; i < kLevelSize; ++i)
    {
//loc_4E7C9:              ; CODE XREF: update?+94Aj
        StatefulLevelTile *tile = &gCurrentLevelState[i];
        if (tile->state == 0 && tile->tile == LevelTileTypeYellowDisk)
        {
            detonateBigExplosion(i);
        }
    }
}

void handleZonkPushedByMurphy(int16_t position) // sub_4ED29   proc near       ; CODE XREF: update?+E6Fp update?+E92p
{
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];

    if (belowTile->tile == LevelTileTypeSnikSnak
        || belowTile->tile == 0xBB)
    {
//loc_4ED38:              ; CODE XREF: handleZonkPushedByMurphy+5j handleZonkPushedByMurphy+Cj
        detonateBigExplosion(position + kLevelWidth);
    }
}

uint8_t checkMurphyMovementToPosition(int16_t position, UserInput userInput) // sub_4F21F   proc near       ; CODE XREF: update?+273p update?+2EDp ...
{
    // 01ED:85BC
    // Parameters:
    // - si: position
    // - ax: value of that position (state + tile)
    // - bl: user input to process
    StatefulLevelTile *tile = &gCurrentLevelState[position];

    if ((tile->state == 0xFF && tile->tile == 0xFF)
        || (tile->state == 0xAA && tile->tile == 0xAA)
        || (tile->state == 0))
    {
//loc_4F296:              ; CODE XREF: checkMurphyMovementToPosition+3j checkMurphyMovementToPosition+8j ...
        return 1;
    }
    else if (tile->tile == LevelTileTypeZonk)
    {
//loc_4F24F:              ; CODE XREF: checkMurphyMovementToPosition+11j
        if (userInput == UserInputLeft)
        {
//loc_4F25E:              ; CODE XREF: checkMurphyMovementToPosition+33j
            uint8_t stateType = (tile->state & 0xF0);
            if (stateType == 0x20
                || stateType == 0x40
                || stateType == 0x50
                || stateType == 0x70)
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
        else if (userInput != UserInputRight)
        {
            detonateBigExplosion(position);
            return 1;
        }

//loc_4F27A:              ; CODE XREF: checkMurphyMovementToPosition+38j
        uint8_t stateType = (tile->state & 0xF0);
        if (stateType == 0x30
            || stateType == 0x40
            || stateType == 0x60
            || stateType == 0x70)
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
        if ((tile->state & 0x80) != 0
            || tile->state < 4)
        {
//loc_4F2A2:              ; CODE XREF: checkMurphyMovementToPosition+7Cj
            detonateBigExplosion(position);
            return 1;
        }
        else
        {
//loc_4F2A7:              ; CODE XREF: checkMurphyMovementToPosition+81j
            tile->state = 0;
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

void updateSpecialPort(int16_t position) // sub_4F2AF   proc near       ; CODE XREF: update?+116Ap
                   // ; update?+1197p ...
{
    // 01ED:864C
    if (gNumberOfSpecialPorts == 0)
    {
        return;
    }

    uint8_t isPortInPosition = 0;
    uint8_t portIndex = 0;

    for (uint8_t i = 0; i < gNumberOfSpecialPorts; ++i)
    {
//loc_4F2BD:              ; CODE XREF: updateSpecialPort+19j
        SpecialPortInfo portInfo = gCurrentLevel.specialPortsInfo[i];
        // For _reasons_ the port position has its bytes inverted (first high, then low), so we must reverse them
        uint16_t portPosition = swap16(portInfo.position);
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
    // I still don't know where word_510AC is read :fearful:
    // I tried with a breakpoint on memory read and it was never accessed :shrug:
    // I can probably forget about it...
    // word_510AC = word_510AC ^ gRandomGeneratorSeed;
}

void updateSnikSnakTiles(int16_t position) // movefun4  proc near       ; DATA XREF: data:162Co
{
    // 01ED:868D
    if (gAreEnemiesFrozen == 1)
    {
        return;
    }

    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->tile != LevelTileTypeSnikSnak)
    {
        return;
    }

    uint8_t frame = currentTile->state;

    FrameBasedMovingFunction function = kSnikSnakMovingFunctions[frame];

    if (function != NULL)
    {
        // 01ED:86AC
        function(position, frame);
    }
}

void updateSnikSnakTurnLeft(int16_t position, uint8_t frame) // sub_4F312   proc near       ; DATA XREF: data:movingFunctions3o
{
    // 01ED:86AF
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t value = gFrameCounter & 3;

    if (value == 0)
    {
//loc_4F320:              ; CODE XREF: updateSnikSnakTurnLeft+6j
        Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        currentTile->state = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F362:              ; CODE XREF: updateSnikSnakTurnLeft+Bj
    uint8_t state = currentTile->state;
    uint8_t nextMovingObject = 0;

    if (state == 0)
    {
//loc_4F37B:              ; CODE XREF: updateSnikSnakTurnLeft+57j
        if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F38E:              ; CODE XREF: updateSnikSnakTurnLeft+6Ej
            currentTile->state = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->state = 0x10;
            aboveTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = aboveTile->state;
        if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 2)
    {
//loc_4F39E:              ; CODE XREF: updateSnikSnakTurnLeft+5Cj
        if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F3B1:              ; CODE XREF: updateSnikSnakTurnLeft+91j
            currentTile->state = 0x2;
            currentTile->tile = 0xBB;
            leftTile->state = 0x18;
            leftTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = leftTile->state;
        if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 4)
    {
//loc_4F3C1:              ; CODE XREF: updateSnikSnakTurnLeft+61j
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F3D7:              ; CODE XREF: updateSnikSnakTurnLeft+B4j
            currentTile->state = 0x3;
            currentTile->tile = 0xBB;
            belowTile->state = 0x20;
            belowTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = belowTile->state;
        if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 6)
    {
//loc_4F3E7:              ; CODE XREF: updateSnikSnakTurnLeft+66j
        if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F3FD:              ; CODE XREF: updateSnikSnakTurnLeft+DAj
            currentTile->state = 0x4;
            currentTile->tile = 0xBB;
            rightTile->state = 0x28;
            rightTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = rightTile->state;
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

void updateSnikSnakTurnRight(int16_t position, uint8_t frame) // sub_4F40D   proc near       ; DATA XREF: data:155Ao
{
    // 01ED:87AA
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];

    uint16_t value = gFrameCounter & 3;

    if (value == 0)
    {
//loc_4F41B:              ; CODE XREF: updateSnikSnakTurnRight+6j
        Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        frame |= 8;
        currentTile->state = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F45F:              ; CODE XREF: updateSnikSnakTurnRight+Bj
    uint8_t state = currentTile->state;
    uint8_t nextMovingObject = 0;
    if (state == 8)
    {
//loc_4F478:              ; CODE XREF: updateSnikSnakTurnRight+59j
        if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F48B:              ; CODE XREF: updateSnikSnakTurnRight+70j
            currentTile->state = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->state = 0x10;
            aboveTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = aboveTile->state;
        if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xA)
    {
//loc_4F4E4:              ; CODE XREF: updateSnikSnakTurnRight+5Ej
        if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F4FA:              ; CODE XREF: updateSnikSnakTurnLeft+DAj
            currentTile->state = 0x4;
            currentTile->tile = 0xBB;
            rightTile->state = 0x28;
            rightTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = rightTile->state;
        if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xC)
    {
//loc_4F4BE:              ; CODE XREF: updateSnikSnakTurnRight+63j
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F4D4:              ; CODE XREF: updateSnikSnakTurnRight+B6j
            currentTile->state = 0x3;
            currentTile->tile = 0xBB;
            belowTile->state = 0x20;
            belowTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = belowTile->state;
        if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xE)
    {
//loc_4F49B:              ; CODE XREF: updateSnikSnakTurnRight+68j
        if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F4AE:              ; CODE XREF: updateSnikSnakTurnRight+93j
            currentTile->state = 0x2;
            currentTile->tile = 0xBB;
            leftTile->state = 0x18;
            leftTile->tile = LevelTileTypeSnikSnak;
            return;
        }
        nextMovingObject = leftTile->state;
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

void updateSnikSnakMovementUp(int16_t position, uint8_t frame) // sub_4F50A    proc near       ; DATA XREF: data:156Ao
{
    // 01ED:88A7
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t finalPosition = position + kLevelWidth;
    Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];
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
            belowTile->state = 0;
            belowTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F546:              ; CODE XREF: updateSnikSnakMovementUp+2Dj
//                ; updateSnikSnakMovementUp+34j
    if (frame < 8)
    {
        frame += 0x10;
        currentTile->state = frame;
        return;
    }

//loc_4F553:              ; CODE XREF: updateSnikSnakMovementUp+3Fj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 1;
        return;
    }

//loc_4F566:              ; CODE XREF: updateSnikSnakMovementUp+54j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 1;
        return;
    }

//loc_4F573:              ; CODE XREF: updateSnikSnakMovementUp+61j
    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 1;
        currentTile->tile = 0xBB;
        aboveTile->state = 0x10;
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
    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8939
        currentTile->state = 9;
        return;
    }

//loc_4F5A2:              ; CODE XREF: updateSnikSnakMovementUp+90j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 9;
        return;
    }

//loc_4F5AF:              ; CODE XREF: updateSnikSnakMovementUp+9Dj
    currentTile->state = 1;
}

void updateSnikSnakMovementLeft(int16_t position, uint8_t frame) // sub_4F5B5   proc near       ; DATA XREF: data:157Ao
{
    // 01ED:8952
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];

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
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F5EC:              ; CODE XREF: updateSnikSnakMovementLeft+28j
//                ; updateSnikSnakMovementLeft+2Fj
    if (frame < 8)
    {
        frame += 0x18;
        currentTile->state = frame;
        return;
    }

//loc_4F5F9:              ; CODE XREF: updateSnikSnakMovementLeft+3Aj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 3;
        return;
    }

//loc_4F60C:              ; CODE XREF: updateSnikSnakMovementLeft+4Fj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 3;
        return;
    }

//loc_4F619:              ; CODE XREF: updateSnikSnakMovementLeft+5Cj
    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 2;
        currentTile->tile = 0xBB;
        leftTile->state = 0x18;
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
    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 0xF;
        return;
    }

//loc_4F648:              ; CODE XREF: updateSnikSnakMovementLeft+8Bj
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xF;
        return;
    }

//loc_4F655:              ; CODE XREF: updateSnikSnakMovementLeft+98j
    currentTile->state = 3;
}

void updateSnikSnakMovementDown(int16_t position, uint8_t frame) // sub_4F65B   proc near       ; DATA XREF: data:158Ao
{
    // 01ED:89F8
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t finalPosition = position - kLevelWidth;
    Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];
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
            aboveTile->state = 0;
            aboveTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F699:              ; CODE XREF: sub_4F66B+1Fj
//                ; sub_4F66B+26j
    if (frame < 8)
    {
        frame += 0x20;
        currentTile->state = frame;
        return;
    }

//loc_4F6A6:              ; CODE XREF: sub_4F66B+31j
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 5;
        return;
    }

//loc_4F6B9:              ; CODE XREF: sub_4F66B+46j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 5;
        return;
    }

//loc_4F6C6:              ; CODE XREF: sub_4F66B+53j
    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 3;
        currentTile->tile = 0xBB;
        belowTile->state = 0x20;
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
    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8A8C
        currentTile->state = 0xD;
        return;
    }

//loc_4F6F5:              ; CODE XREF: sub_4F66B+82j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xD;
        return;
    }

//loc_4F702:              ; CODE XREF: sub_4F66B+8Fj
    currentTile->state = 5;
}

void updateSnikSnakMovementRight(int16_t position, uint8_t frame) // sub_4F708   proc near       ; DATA XREF: data:159Ao
{
    // 01ED:8AA5
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    Point frameCoordinates = kSnikSnakAnimationFrameCoordinates[frame];

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
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeSpace;
        }
    }
//loc_4F740:              ; CODE XREF: updateSnikSnakMovementRight+29j
//                ; updateSnikSnakMovementRight+30j
    if (frame < 8)
    {
        frame += 0x28;
        currentTile->state = frame;
        return;
    }

//loc_4F74D:              ; CODE XREF: updateSnikSnakMovementRight+3Bj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeSnikSnak;

    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 7;
        return;
    }

//loc_4F760:              ; CODE XREF: updateSnikSnakMovementRight+50j
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 7;
        return;
    }

//loc_4F76D:              ; CODE XREF: updateSnikSnakMovementRight+5Dj
    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 4;
        currentTile->tile = 0xBB;
        rightTile->state = 0x28;
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
    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 0xB;
        return;
    }

//loc_4F79C:              ; CODE XREF: updateSnikSnakMovementRight+8Cj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xB;
        return;
    }

//loc_4F7A9:              ; CODE XREF: updateSnikSnakMovementRight+99j
    currentTile->state = 7;
}

void updateElectronTiles(int16_t position) // movefun6  proc near       ; DATA XREF: data:163Ao
{
    // 01ED:8B4C
    if (gAreEnemiesFrozen == 1)
    {
        return;
    }
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];

    if (currentTile->tile != LevelTileTypeElectron)
    {
        return;
    }

    uint8_t frame = currentTile->state;

    FrameBasedMovingFunction function = kElectronMovingFunctions[frame];

    if (function != NULL)
    {
        // 01ED:8B6B
        function(position, frame);
    }
}

void updateElectronTurnLeft(int16_t position, uint8_t frame) // sub_4F7D1   proc near       ; DATA XREF: data:movingFunctions2o
{
    // 01ED:8B6E
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t value = gFrameCounter & 3;

    if (value == 0)
    {
//loc_4F7DF:              ; CODE XREF: updateElectronTurnLeft+6j
        Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        currentTile->state = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F80D:              ; CODE XREF: updateElectronTurnLeft+Bj
    uint8_t state = currentTile->state;
    if (state == 0)
    {
//loc_4F826:              ; CODE XREF: updateElectronTurnLeft+43j
        if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F835:              ; CODE XREF: updateElectronTurnLeft+5Aj
            currentTile->state = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->state = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 2)
    {
//loc_4F845:              ; CODE XREF: updateElectronTurnLeft+48j
        if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F854:              ; CODE XREF: updateElectronTurnLeft+79j
            currentTile->state = 0x2;
            currentTile->tile = 0xBB;
            leftTile->state = 0x18;
            leftTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (leftTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 4)
    {
//loc_4F864:              ; CODE XREF: updateElectronTurnLeft+4Dj
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F873:              ; CODE XREF: updateElectronTurnLeft+98j
            currentTile->state = 0x3;
            currentTile->tile = 0xBB;
            belowTile->state = 0x20;
            belowTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 6)
    {
//loc_4F883:              ; CODE XREF: updateElectronTurnLeft+52j
        if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F895:              ; CODE XREF: updateElectronTurnLeft+B7j
            currentTile->state = 0x4;
            currentTile->tile = 0xBB;
            rightTile->state = 0x28;
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

void updateElectronTurnRight(int16_t position, uint8_t frame) // sub_4F8A5   proc near       ; DATA XREF: data:15BAo
{
    // 01ED:8C42
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t value = gFrameCounter & 3;

    if (value == 0)
    {
//loc_4F8B3:              ; CODE XREF: updateElectronTurnRight+6j
        Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];
        drawMovingFrame(frameCoordinates.x, frameCoordinates.y, position);
        frame++;
        frame &= 7;
        frame |= 8;
        currentTile->state = frame;
        return;
    }
    else if (value != 3)
    {
        return;
    }

//loc_4F8E3:              ; CODE XREF: updateElectronTurnRight+Bj
    uint8_t state = currentTile->state;
    if (state == 8)
    {
//loc_4F8FC:              ; CODE XREF: updateElectronTurnRight+45j
        if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
        {
//loc_4F90B:              ; CODE XREF: updateElectronTurnRight+5Cj
            currentTile->state = 0x1;
            currentTile->tile = 0xBB;
            aboveTile->state = 0x10;
            aboveTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (aboveTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xA)
    {
//loc_4F959:              ; CODE XREF: updateElectronTurnRight+4Aj
        if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
        {
//loc_4F96B:              ; CODE XREF: updateElectronTurnRight+B9j
            currentTile->state = 0x4;
            currentTile->tile = 0xBB;
            rightTile->state = 0x28;
            rightTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (rightTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xC)
    {
//loc_4F93A:              ; CODE XREF: updateElectronTurnRight+4Fj
        if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
        {
//loc_4F949:              ; CODE XREF: updateElectronTurnRight+9Aj
            currentTile->state = 0x3;
            currentTile->tile = 0xBB;
            belowTile->state = 0x20;
            belowTile->tile = LevelTileTypeElectron;
            return;
        }
        else if (belowTile->tile != LevelTileTypeMurphy)
        {
            return;
        }
    }
    else if (state == 0xE)
    {
//loc_4F91B:              ; CODE XREF: updateElectronTurnRight+54j
        if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
        {
//loc_4F92A:              ; CODE XREF: updateElectronTurnRight+7Bj
            currentTile->state = 0x2;
            currentTile->tile = 0xBB;
            leftTile->state = 0x18;
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

void updateElectronMovementUp(int16_t position, uint8_t frame) // sub_4F97B   proc near       ; DATA XREF: data:15CAo
{
    // 01ED:8D18
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t finalPosition = position + kLevelWidth;
    Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];
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
            belowTile->state = 0;
            belowTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4F9B7:              ; CODE XREF: updateElectronMovementUp+2Dj
//                ; updateElectronMovementUp+34j
    if (frame < 8)
    {
        frame += 0x10;
        currentTile->state = frame;
        return;
    }

//loc_4F9C4:              ; CODE XREF: updateElectronMovementUp+3Fj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 1;
        return;
    }

//loc_4F9D7:              ; CODE XREF: updateElectronMovementUp+54j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 1;
        return;
    }

//loc_4F9E4:              ; CODE XREF: updateElectronMovementUp+61j
    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 1;
        currentTile->tile = 0xBB;
        aboveTile->state = 0x10;
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
    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 9;
        return;
    }

//loc_4FA13:              ; CODE XREF: updateElectronMovementUp+90j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 9;
        return;
    }

//loc_4FA20:              ; CODE XREF: updateElectronMovementUp+9Dj
    currentTile->state = 1;
}

void updateElectronMovementDown(int16_t position, uint8_t frame) // sub_4FA26   proc near       ; DATA XREF: data:15DAo
{
    // 01ED:8DC3
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];

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
            rightTile->state = 0;
            rightTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FA5D:              ; CODE XREF: updateElectronMovementDown+28j
//                ; updateElectronMovementDown+2Fj
    if (frame < 8)
    {
        frame += 0x18;
        currentTile->state = frame;
        return;
    }

//loc_4FA6A:              ; CODE XREF: updateElectronMovementDown+3Aj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 3;
        return;
    }

//loc_4FA7D:              ; CODE XREF: updateElectronMovementDown+4Fj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 3;
        return;
    }

//loc_4FA8A:              ; CODE XREF: updateElectronMovementDown+5Cj
    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 2;
        currentTile->tile = 0xBB;
        leftTile->state = 0x18;
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
    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 0xF;
        return;
    }

//loc_4FAB9:              ; CODE XREF: updateElectronMovementDown+8Bj
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xF;
        return;
    }

//loc_4FAC6:              ; CODE XREF: updateElectronMovementDown+98j
    currentTile->state = 3;
}

void updateElectronMovementRight(int16_t position, uint8_t frame) // sub_4FACC   proc near       ; DATA XREF: data:15EAo
{
    // 01ED:8E69
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    uint16_t finalPosition = position - kLevelWidth;
    Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];
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
            aboveTile->state = 0;
            aboveTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FB0A:              ; CODE XREF: updateElectronMovementRight+2Fj
//                ; updateElectronMovementRight+36j
    if (frame < 8)
    {
        frame += 0x20;
        currentTile->state = frame;
        return;
    }

//loc_4FB17:              ; CODE XREF: updateElectronMovementRight+41j
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 5;
        return;
    }

//loc_4FB2A:              ; CODE XREF: updateElectronMovementRight+56j
    if (rightTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 5;
        return;
    }

//loc_4FB37:              ; CODE XREF: updateElectronMovementRight+63j
    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 3;
        currentTile->tile = 0xBB;
        belowTile->state = 0x20;
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
    if (leftTile->state == 0 && leftTile->tile == LevelTileTypeSpace)
    {
        // 01ED:8A8C
        currentTile->state = 0xD;
        return;
    }

//loc_4FB66:              ; CODE XREF: updateElectronMovementRight+92j
    if (leftTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xD;
        return;
    }

//loc_4FB73:              ; CODE XREF: updateElectronMovementRight+9Fj
    currentTile->state = 5;
}

void updateElectronMovementLeft(int16_t position, uint8_t frame) // sub_4FB79   proc near       ; DATA XREF: data:15FAo
{
    // 01ED:8F16
    StatefulLevelTile *currentTile = &gCurrentLevelState[position];
    StatefulLevelTile *belowTile = &gCurrentLevelState[position + kLevelWidth];
    StatefulLevelTile *leftTile = &gCurrentLevelState[position - 1];
    StatefulLevelTile *aboveTile = &gCurrentLevelState[position - kLevelWidth];
    StatefulLevelTile *rightTile = &gCurrentLevelState[position + 1];

    Point frameCoordinates = kElectronAnimationFrameCoordinates[frame];

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
            leftTile->state = 0;
            leftTile->tile = LevelTileTypeSpace;
        }
    }

//loc_4FBB1:              ; CODE XREF: updateElectronMovementLeft+29j
//                ; updateElectronMovementLeft+30j
    if (frame < 8)
    {
        frame += 0x28;
        currentTile->state = frame;
        return;
    }

//loc_4FBBE:              ; CODE XREF: updateElectronMovementLeft+3Bj
    currentTile->state = 0;
    currentTile->tile = LevelTileTypeElectron;

    if (aboveTile->state == 0 && aboveTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 7;
        return;
    }

//loc_4FBD1:              ; CODE XREF: updateElectronMovementLeft+50j
    if (aboveTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 7;
        return;
    }

//loc_4FBDE:              ; CODE XREF: updateElectronMovementLeft+5Dj
    if (rightTile->state == 0 && rightTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 4;
        currentTile->tile = 0xBB;
        rightTile->state = 0x28;
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
    if (belowTile->state == 0 && belowTile->tile == LevelTileTypeSpace)
    {
        currentTile->state = 0xB;
        return;
    }

//loc_4FC0D:              ; CODE XREF: updateElectronMovementLeft+8Cj
    if (belowTile->tile == LevelTileTypeMurphy)
    {
        currentTile->state = 0xB;
        return;
    }

//loc_4FC1A:              ; CODE XREF: updateElectronMovementLeft+99j
    currentTile->state = 7;
}

void drawGamePanelText() // sub_4FC20  proc near       ; CODE XREF: stopRecordingDemo:loc_4944Fp
                   // ; drawGamePanel+22p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:8FBD
    if (gIsRecordingDemo != 0) // Recording demo?
    {
//    mov si, 87D1h // "  DEMO  "
        drawTextWithChars8FontToGamePanel(72, 3, 8, "  DEMO  ");
//    mov si, 87DAh // "000" -> this address is the ".SP" text
        drawTextWithChars8FontToGamePanel(16, 14, 8, gCurrentDemoLevelName);
//        mov si, 87F6h // "--- RECORDING DEMO0 ---"
        drawTextWithChars8FontToGamePanel(64, 14, 8, gRecordingDemoMessage);
    }
//loc_4FC6F:              ; CODE XREF: drawGamePanelText+5j
    else if (gIsPlayingDemo != 0) // Playing demo?
    {
        drawTextWithChars8FontToGamePanel(72, 3, 8, "  DEMO  ");
//      mov si, 87DAh // "000" -> this address is the ".SP" text
        drawTextWithChars8FontToGamePanel(16, 14, 8, gCurrentDemoLevelName);
//      mov si, 87DEh // "----- DEMO LEVEL! -----"
        drawTextWithChars8FontToGamePanel(64, 14, 8, &gCurrentDemoLevelName[4]);
    }
    else
    {
//loc_4FCD6:              ; CODE XREF: drawGamePanelText+B1j
        drawTextWithChars8FontToGamePanel(72, 3, 6, gPlayerName);
        char levelNumber[4] = "000";
        memcpy(levelNumber, gCurrentLevelName, 3);
        drawTextWithChars8FontToGamePanel(16, 14, 8, levelNumber);
        drawTextWithChars8FontToGamePanel(64, 14, 8, &gCurrentLevelName[4]);
    }

//loc_4FD1A:              ; CODE XREF: drawGamePanelText+4Cj
//                ; drawGamePanelText+A0j ...
    drawNumberOfRemainingInfotrons();
    drawGameTime();
}

void drawNumberOfRemainingInfotrons() // sub_4FD21   proc near       ; CODE XREF: resetNumberOfInfotrons+13p
                   // ; update?:loc_4EC90p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

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
    drawTextWithChars8FontToGamePanel(272, 14, color, number);
}

void clearAdditionalInfoInGamePanelIfNeeded() // sub_4FD65   proc near       ; CODE XREF: runLevel+E9p
{
//loc_4FD6D:              ; CODE XREF: clearAdditionalInfoInGamePanelIfNeeded+5j
    if (gAdditionalInfoInGamePanelFrameCounter == 0)
    {
        return;
    }

//loc_4FD75:              ; CODE XREF: clearAdditionalInfoInGamePanelIfNeeded+Dj
    gAdditionalInfoInGamePanelFrameCounter--;
    if (gAdditionalInfoInGamePanelFrameCounter != 0)
    {
        return;
    }

    clearAdditionalInfoInGamePanel();
}

void decreaseRemainingRedDisksIfNeeded(int16_t position) // sub_4FDB5   proc near       ; CODE XREF: update?+124Fp
                    // ; update?+1266p ...
{
    if (word_59B73 == 0
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
    drawTextWithChars8FontToGamePanel(304, 14, color, &numberString[1]);
    gAdditionalInfoInGamePanelFrameCounter = 0x46; // 70
}

void drawGameTime() // sub_4FDFD   proc near       ; CODE XREF: runLevel+29p
                   // ; runLevel:noFlashing2p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // Only the 2 last digits will be printed, hence why it will be used with &number[1] everywhere
    char number[4] = "000";

    if ((gLastDrawnMinutesAndSeconds & 0xFF) != gGameSeconds) // byte
    {
        gLastDrawnMinutesAndSeconds = (gLastDrawnMinutesAndSeconds & 0xFF00) + gGameSeconds; // byte
        convertNumberTo3DigitStringWithPadding0(gGameSeconds, number);
//loc_4FE2C:              ; CODE XREF: drawGameTime+2Aj
        drawTextWithChars8FontToGamePanel(208, 3, 6, &number[1]); // seconds
    }

//loc_4FE36:              ; CODE XREF: drawGameTime+12j
    if ((gLastDrawnMinutesAndSeconds >> 8) != gGameMinutes) // byte
    {
        gLastDrawnMinutesAndSeconds = (gGameMinutes << 8) + (gLastDrawnMinutesAndSeconds & 0x00FF); // byte
        convertNumberTo3DigitStringWithPadding0(gGameMinutes, number);
        drawTextWithChars8FontToGamePanel(184, 3, 6, &number[1]); // minutes
    }

//loc_4FE5F:              ; CODE XREF: drawGameTime+40j
    if (gLastDrawnHours != gGameHours)
    {
        gLastDrawnHours = gGameHours;
        convertNumberTo3DigitStringWithPadding0(gGameHours, number);
        drawTextWithChars8FontToGamePanel(160, 3, 6, &number[1]); // hours
    }
}

void drawGamePanel() // sub_501C0   proc near       ; CODE XREF: start+338p handleGameUserInput+678p ...
{
    clearGamePanel();
    drawGamePanelText();
}

void drawSpeedFixTitleAndVersion() //   proc near       ; CODE XREF: start+2E6p
{
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(102, 11, 1, "SUPAPLEX VERSION " VERSION_STRING);
}

void drawSpeedFixCredits() // showNewCredits  proc near       ; CODE XREF: start+2ECp
{
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(60, 168, 0xE, "VERSIONS 1-4 + 6.X BY HERMAN PERK");
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(60, 176, 0xE, "VERSIONS 5.X BY ELMER PRODUCTIONS");
    drawTextWithChars6FontWithOpaqueBackgroundIfPossible(60, 184, 0xE, "  VERSION 7.X BY SERGIO PADRINO  ");

    videoLoop();

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
    while (isAnyKeyPressed() == 0
           && isAnyGameControllerButtonPressed() == 0);
}

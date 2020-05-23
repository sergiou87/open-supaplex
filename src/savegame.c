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

#include "savegame.h"

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "demo.h"
#include "utils.h"

// These two are some kind of magic number to identify savegames
// uint16_t word_5A309 = 0x5053;
// uint16_t word_5A30B = 0x1A0D;
char kSaveGameMagicNumber[5] = "OSPX";

// 8KB of buffer should be enough for the strings we'll deal with here
#define kReadConfigStringBufferSize 8192
#define kConfigKeyBufferSize 256

#define SAVE_GAME_STATE_CONFIG_BASE64(__key, __value, __size) \
    do \
    { \
        char *base64Data = encodeBase64((const unsigned char *)__value, __size); \
        if (base64Data == NULL) \
        { \
            return 1; \
        } \
        writeConfigString(config, __key, base64Data); \
        free(base64Data); \
    } \
    while (0)

#define SAVE_GAME_STATE_CONFIG_SUBITEM_INT(__keyBase, __valueBase, __value) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%s", __keyBase, #__value); \
        writeConfigInt(config, configKeyBuffer, __valueBase.__value); \
    } \
    while (0)

#define SAVE_GAME_STATE_CONFIG_SUBITEM_BASE64(__keyBase, __valueBase, __value, __size) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%s", __keyBase, #__value); \
        SAVE_GAME_STATE_CONFIG_BASE64(configKeyBuffer, __valueBase.__value, __size); \
    } \
    while (0)

#define SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(__keyBase, __valueBase, __value) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%d_%s", __keyBase, idx, #__value); \
        writeConfigInt(config, configKeyBuffer, __valueBase.__value); \
    } \
    while (0)

static const char *kVersionKey = "Version";
static const char *kLevelNameKey = "LevelName";
static const char *kLevelStateKey = "LevelState";
static const char *kExplosionTimersKey = "ExplosionTimers";
static const char *kMurphyAnimationKeyBase = "MurphyAnimation";
static const char *kCurrentLevelKeyBase = "CurrentLevel";
static const char *kCurrentLevelSpecialPortsKeyBase = "CurrentLevel_specialPortsInfo";

void saveCurrentMurphyAnimationGameState(Config *config)
{
    char configKeyBuffer[kConfigKeyBufferSize] = "";

    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationCoordinatesOffset);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationCoordinatesOffsetIncrement);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, width);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, height);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationIndex);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, speedX);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, speedY);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, currentFrame);
}

uint8_t saveCurrentLevelGameState(Config *config)
{
    char configKeyBuffer[kConfigKeyBufferSize] = "";

    SAVE_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, tiles, sizeof(gCurrentLevel.tiles));
    SAVE_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, unused, sizeof(gCurrentLevel.unused));
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, initialGravitation);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, speedFixMagicNumber);
    SAVE_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, name, sizeof(gCurrentLevel.name));
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, freezeZonks);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, numberOfInfotrons);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, numberOfSpecialPorts);

    for (int idx = 0; idx < kLevelMaxNumberOfSpecialPorts; ++idx)
    {
        SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], position);
        SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], gravity);
        SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], freezeZonks);
        SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], freezeEnemies);
        SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], unused);
    }

    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledSpeed);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledChecksum);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, randomSeed);

    return 0;
}

#undef SAVE_GAME_STATE_CONFIG_SUBITEM_INT
#undef SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT
#undef SAVE_GAME_STATE_CONFIG_SUBITEM_BASE64

uint8_t saveGameState()
{
    Config *config = initializeConfigForWriting(gSavegameSavFilename);

    if (config == NULL)
    {
        return 1;
    }

//loc_499D8:              ; CODE XREF: handleGameUserInput+478j
    // 01ED:2D7B

    writeConfigSection(config, kSaveGameMagicNumber);

//loc_499F7:              ; CODE XREF: handleGameUserInput+497j
    // 01ED:2D9B
    char *levelName = "";
    if (gIsPlayingDemo == 0)
    {
        levelName = gCurrentLevelName;
    }
    else
    {
//loc_49A03:              ; CODE XREF: handleGameUserInput+4A1j
        levelName = gCurrentDemoLevelName;
    }

//loc_49A06:              ; CODE XREF: handleGameUserInput+4A6j

    writeConfigInt(config, kVersionKey, kGameVersion);
    SAVE_GAME_STATE_CONFIG_BASE64(kLevelNameKey, levelName, kListLevelNameLength);

#define SAVE_GAME_STATE_CONFIG_INT(__value) \
    writeConfigInt(config, #__value, g##__value)

    SAVE_GAME_STATE_CONFIG_BASE64(kLevelStateKey, gCurrentLevelStateWithPadding, sizeof(gCurrentLevelStateWithPadding));
    SAVE_GAME_STATE_CONFIG_BASE64(kExplosionTimersKey, gExplosionTimers, sizeof(gExplosionTimers));
    SAVE_GAME_STATE_CONFIG_INT(IsGravityEnabled);
    SAVE_GAME_STATE_CONFIG_INT(AreZonksFrozen);
    SAVE_GAME_STATE_CONFIG_INT(NumberOfInfoTrons);
    SAVE_GAME_STATE_CONFIG_INT(NumberOfSpecialPorts);
    SAVE_GAME_STATE_CONFIG_INT(RandomSeed);
    SAVE_GAME_STATE_CONFIG_INT(AuxGameSeconds20msAccumulator);
    SAVE_GAME_STATE_CONFIG_INT(GameSeconds);
    SAVE_GAME_STATE_CONFIG_INT(GameMinutes);
    SAVE_GAME_STATE_CONFIG_INT(GameHours);
    SAVE_GAME_STATE_CONFIG_INT(ShouldUpdateTotalLevelTime);
    SAVE_GAME_STATE_CONFIG_INT(LevelFailed);
    SAVE_GAME_STATE_CONFIG_INT(CurrentPlayerLevelState);
    SAVE_GAME_STATE_CONFIG_INT(IsExplosionStarted);
    SAVE_GAME_STATE_CONFIG_INT(ShouldShowGamePanel);
    SAVE_GAME_STATE_CONFIG_INT(ToggleGamePanelKeyAutoRepeatCounter);
    SAVE_GAME_STATE_CONFIG_INT(MurphyTileX);
    SAVE_GAME_STATE_CONFIG_INT(MurphyTileY);
    SAVE_GAME_STATE_CONFIG_INT(MurphyPreviousLocation);
    SAVE_GAME_STATE_CONFIG_INT(MurphyLocation);
    SAVE_GAME_STATE_CONFIG_INT(IsMurphyLookingLeft);
    SAVE_GAME_STATE_CONFIG_INT(MurphyYawnAndSleepCounter);
    SAVE_GAME_STATE_CONFIG_INT(IsMurphyUpdated);
    SAVE_GAME_STATE_CONFIG_INT(ShouldKillMurphy);
    SAVE_GAME_STATE_CONFIG_INT(PreviousUserInputWasNone);
    SAVE_GAME_STATE_CONFIG_INT(AreEnemiesFrozen);
    SAVE_GAME_STATE_CONFIG_INT(ScratchGravity);
    SAVE_GAME_STATE_CONFIG_INT(IsMurphyGoingThroughPortal);
    SAVE_GAME_STATE_CONFIG_INT(PlantedRedDiskCountdown);
    SAVE_GAME_STATE_CONFIG_INT(PlantedRedDiskPosition);
    SAVE_GAME_STATE_CONFIG_INT(DemoCurrentInputIndex);
    SAVE_GAME_STATE_CONFIG_INT(DemoCurrentInput);
    SAVE_GAME_STATE_CONFIG_INT(DemoCurrentInputRepeatCounter);
    SAVE_GAME_STATE_CONFIG_INT(DemoIndexOrDemoLevelNumber);
    SAVE_GAME_STATE_CONFIG_INT(MurphyPositionX);
    SAVE_GAME_STATE_CONFIG_INT(MurphyPositionY);
    SAVE_GAME_STATE_CONFIG_INT(MurphyCounterToStartPushAnimation);
    saveCurrentMurphyAnimationGameState(config);
    SAVE_GAME_STATE_CONFIG_INT(NumberOfRemainingInfotrons);
    SAVE_GAME_STATE_CONFIG_INT(TotalNumberOfInfotrons);
    SAVE_GAME_STATE_CONFIG_INT(NumberOfRemainingRedDisks);
    SAVE_GAME_STATE_CONFIG_INT(FrameCounter);
    SAVE_GAME_STATE_CONFIG_INT(TerminalMaxFramesToNextScroll);
    SAVE_GAME_STATE_CONFIG_INT(AreYellowDisksDetonated);
    SAVE_GAME_STATE_CONFIG_INT(ShouldLeaveMainMenu);
    SAVE_GAME_STATE_CONFIG_INT(ShouldExitLevel);
    SAVE_GAME_STATE_CONFIG_INT(QuitLevelCountdown);
    SAVE_GAME_STATE_CONFIG_INT(AdditionalInfoInGamePanelFrameCounter);
    saveCurrentLevelGameState(config);
#undef SAVE_GAME_STATE_CONFIG_INT

    destroyConfig(config);

    return 0;
}

uint8_t canLoadGameState(void)
{
    FILE *file = openWritableFile(gSavegameSavFilename, "r");
    if (file == NULL)
    {
        return 0;
    }

    fclose(file);

    return 1;
}

#define LOAD_GAME_STATE_CONFIG_BASE64(__key, __value, __size) \
    do \
    { \
        if (readConfigString(config, kSaveGameMagicNumber, __key, readConfigStringBuffer) != 0) \
        { \
            return 1; \
        } \
        if (decodeBase64(readConfigStringBuffer, (unsigned char *)__value, __size) == 0) \
        { \
            return 1; \
        } \
    } \
    while (0)

#define LOAD_GAME_STATE_CONFIG_SUBITEM_INT(__keyBase, __valueBase, __value) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%s", __keyBase, #__value); \
        __valueBase.__value = readConfigInt(config, kSaveGameMagicNumber, configKeyBuffer, __valueBase.__value); \
    } \
    while (0)

#define LOAD_GAME_STATE_CONFIG_SUBITEM_BASE64(__keyBase, __valueBase, __value, __size) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%s", __keyBase, #__value); \
        LOAD_GAME_STATE_CONFIG_BASE64(configKeyBuffer, __valueBase.__value, __size); \
    } \
    while (0)

#define LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(__keyBase, __valueBase, __value) \
    do \
    { \
        sprintf(configKeyBuffer, "%s_%d_%s", __keyBase, idx, #__value); \
        __valueBase.__value = readConfigInt(config, kSaveGameMagicNumber, configKeyBuffer, __valueBase.__value); \
    } \
    while (0)

void loadCurrentMurphyAnimationGameState(Config *config)
{
    char configKeyBuffer[kConfigKeyBufferSize] = "";

    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationCoordinatesOffset);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationCoordinatesOffsetIncrement);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, width);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, height);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, animationIndex);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, speedX);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, speedY);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kMurphyAnimationKeyBase, gCurrentMurphyAnimation, currentFrame);
}

uint8_t loadCurrentLevelGameState(Config *config)
{
    char configKeyBuffer[kConfigKeyBufferSize] = "";
    char readConfigStringBuffer[kReadConfigStringBufferSize];

    LOAD_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, tiles, sizeof(gCurrentLevel.tiles));
    LOAD_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, unused, sizeof(gCurrentLevel.unused));
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, initialGravitation);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, speedFixMagicNumber);
    LOAD_GAME_STATE_CONFIG_SUBITEM_BASE64(kCurrentLevelKeyBase, gCurrentLevel, name, sizeof(gCurrentLevel.name));
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, freezeZonks);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, numberOfInfotrons);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, numberOfSpecialPorts);

    for (int idx = 0; idx < kLevelMaxNumberOfSpecialPorts; ++idx)
    {
        LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], position);
        LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], gravity);
        LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], freezeZonks);
        LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], freezeEnemies);
        LOAD_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT(kCurrentLevelSpecialPortsKeyBase, gCurrentLevel.specialPortsInfo[idx], unused);
    }

    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledSpeed);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledChecksum);
    LOAD_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, randomSeed);

    return 0;
}

uint8_t loadGameState()
{
    Config *config = initializeConfigForReading(gSavegameSavFilename);

    if (config == NULL)
    {
        return 1;
    }

    int value = readConfigInt(config, kSaveGameMagicNumber, kVersionKey, 0);
    if (value != kGameVersion)
    {
        destroyConfig(config);
        return 1;
    }

    char readConfigStringBuffer[kReadConfigStringBufferSize];

    LOAD_GAME_STATE_CONFIG_BASE64(kLevelNameKey, gCurrentLevelName, sizeof(gCurrentLevelName));
    LOAD_GAME_STATE_CONFIG_BASE64(kLevelStateKey, gCurrentLevelStateWithPadding, sizeof(gCurrentLevelStateWithPadding));
    LOAD_GAME_STATE_CONFIG_BASE64(kExplosionTimersKey, gExplosionTimers, sizeof(gExplosionTimers));

#define LOAD_GAME_STATE_CONFIG_INT(__value) \
    g##__value = readConfigInt(config, kSaveGameMagicNumber, #__value, g##__value)

    LOAD_GAME_STATE_CONFIG_INT(IsGravityEnabled);
    LOAD_GAME_STATE_CONFIG_INT(AreZonksFrozen);
    LOAD_GAME_STATE_CONFIG_INT(NumberOfInfoTrons);
    LOAD_GAME_STATE_CONFIG_INT(NumberOfSpecialPorts);
    LOAD_GAME_STATE_CONFIG_INT(RandomSeed);
    LOAD_GAME_STATE_CONFIG_INT(AuxGameSeconds20msAccumulator);
    LOAD_GAME_STATE_CONFIG_INT(GameSeconds);
    LOAD_GAME_STATE_CONFIG_INT(GameMinutes);
    LOAD_GAME_STATE_CONFIG_INT(GameHours);
    LOAD_GAME_STATE_CONFIG_INT(ShouldUpdateTotalLevelTime);
    LOAD_GAME_STATE_CONFIG_INT(LevelFailed);
    LOAD_GAME_STATE_CONFIG_INT(CurrentPlayerLevelState);
    LOAD_GAME_STATE_CONFIG_INT(IsExplosionStarted);
    LOAD_GAME_STATE_CONFIG_INT(ShouldShowGamePanel);
    LOAD_GAME_STATE_CONFIG_INT(ToggleGamePanelKeyAutoRepeatCounter);
    LOAD_GAME_STATE_CONFIG_INT(MurphyTileX);
    LOAD_GAME_STATE_CONFIG_INT(MurphyTileY);
    LOAD_GAME_STATE_CONFIG_INT(MurphyPreviousLocation);
    LOAD_GAME_STATE_CONFIG_INT(MurphyLocation);
    LOAD_GAME_STATE_CONFIG_INT(IsMurphyLookingLeft);
    LOAD_GAME_STATE_CONFIG_INT(MurphyYawnAndSleepCounter);
    LOAD_GAME_STATE_CONFIG_INT(IsMurphyUpdated);
    LOAD_GAME_STATE_CONFIG_INT(ShouldKillMurphy);
    LOAD_GAME_STATE_CONFIG_INT(PreviousUserInputWasNone);
    LOAD_GAME_STATE_CONFIG_INT(AreEnemiesFrozen);
    LOAD_GAME_STATE_CONFIG_INT(ScratchGravity);
    LOAD_GAME_STATE_CONFIG_INT(IsMurphyGoingThroughPortal);
    LOAD_GAME_STATE_CONFIG_INT(PlantedRedDiskCountdown);
    LOAD_GAME_STATE_CONFIG_INT(PlantedRedDiskPosition);
    LOAD_GAME_STATE_CONFIG_INT(DemoCurrentInputIndex);
    LOAD_GAME_STATE_CONFIG_INT(DemoCurrentInput);
    LOAD_GAME_STATE_CONFIG_INT(DemoCurrentInputRepeatCounter);
    LOAD_GAME_STATE_CONFIG_INT(DemoIndexOrDemoLevelNumber);
    LOAD_GAME_STATE_CONFIG_INT(MurphyPositionX);
    LOAD_GAME_STATE_CONFIG_INT(MurphyPositionY);
    LOAD_GAME_STATE_CONFIG_INT(MurphyCounterToStartPushAnimation);
    loadCurrentMurphyAnimationGameState(config);
    LOAD_GAME_STATE_CONFIG_INT(NumberOfRemainingInfotrons);
    LOAD_GAME_STATE_CONFIG_INT(TotalNumberOfInfotrons);
    LOAD_GAME_STATE_CONFIG_INT(NumberOfRemainingRedDisks);
    LOAD_GAME_STATE_CONFIG_INT(FrameCounter);
    LOAD_GAME_STATE_CONFIG_INT(TerminalMaxFramesToNextScroll);
    LOAD_GAME_STATE_CONFIG_INT(AreYellowDisksDetonated);
    LOAD_GAME_STATE_CONFIG_INT(ShouldLeaveMainMenu);
    LOAD_GAME_STATE_CONFIG_INT(ShouldExitLevel);
    LOAD_GAME_STATE_CONFIG_INT(QuitLevelCountdown);
    LOAD_GAME_STATE_CONFIG_INT(AdditionalInfoInGamePanelFrameCounter);
    loadCurrentLevelGameState(config);
#undef LOAD_GAME_STATE_CONFIG_INT

    destroyConfig(config);

    return 0;
}


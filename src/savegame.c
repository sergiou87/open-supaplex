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
    char configKeyBuffer[256] = "";

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
    char configKeyBuffer[256] = "";

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

#undef SAVE_GAME_STATE_CONFIG_INDEXED_SUBITEM_INT

    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledSpeed);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, scrambledChecksum);
    SAVE_GAME_STATE_CONFIG_SUBITEM_INT(kCurrentLevelKeyBase, gCurrentLevel, randomSeed);

    return 0;
}

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

//    Savegame savegame;
//    memset(&savegame, 0, sizeof(Savegame));
//    memcpy(savegame.magicNumber, kSaveGameMagicNumber, sizeof(savegame.magicNumber));

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

uint8_t loadGameState()
{
    FILE *file = openWritableFile(gSavegameSavFilename, "r");
    if (file == NULL)
    {
        return 1;
    }
/*
    Savegame savegame;
    size_t bytes = fread(&savegame, 1, sizeof(Savegame), file);
    if (bytes < sizeof(Savegame))
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        return 1;
    }

//loc_49AB1:              ; CODE XREF: handleGameUserInput+551j
    if (memcmp(savegame.magicNumber, kSaveGameMagicNumber, sizeof(savegame.magicNumber)) != 0)
    {
//loc_49AC5:              ; CODE XREF: handleGameUserInput+55Ej
        fclose(file);
        return 1;
    }

//loc_49B6F:              ; CODE XREF: handleGameUserInput+60Fj
    if (savegame.savegameVersion != kGameVersion)
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        return 1;
    }
*/
    fclose(file);

//loc_49ADF:              ; CODE XREF: handleGameUserInput+57Fj
    //memcpy(gCurrentLevelName, savegame.levelName, sizeof(gCurrentLevelName));

    // TODO: implement savegames properly
    // gCurrentGameState = savegame.gameState;

    return 0;
}


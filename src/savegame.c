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

void saveCurrentMurphyAnimationGameState(Config *config)
{
    writeConfigInt(config, "MurphyAnimationAnimationCoordinatesOffset", gCurrentMurphyAnimation.animationCoordinatesOffset);
    writeConfigInt(config, "MurphyAnimationAnimationCoordinatesOffsetIncrement", gCurrentMurphyAnimation.animationCoordinatesOffsetIncrement);
    writeConfigInt(config, "MurphyAnimationWidth", gCurrentMurphyAnimation.width);
    writeConfigInt(config, "MurphyAnimationHeight", gCurrentMurphyAnimation.height);
    writeConfigInt(config, "MurphyAnimationAnimationIndex", gCurrentMurphyAnimation.animationIndex);
    writeConfigInt(config, "MurphyAnimationSpeedX", gCurrentMurphyAnimation.speedX);
    writeConfigInt(config, "MurphyAnimationSpeedY", gCurrentMurphyAnimation.speedY);
    writeConfigInt(config, "MurphyAnimationCurrentFrame", gCurrentMurphyAnimation.currentFrame);
}

uint8_t saveCurrentLevelGameState(Config *config)
{
    SAVE_GAME_STATE_CONFIG_BASE64("CurrentLevelTiles", gCurrentLevel.tiles, sizeof(gCurrentLevel.tiles));
    SAVE_GAME_STATE_CONFIG_BASE64("CurrentLevelUnused", gCurrentLevel.unused, sizeof(gCurrentLevel.unused));
    writeConfigInt(config, "CurrentLevelInitialGravitation", gCurrentLevel.initialGravitation);
    writeConfigInt(config, "CurrentLevelSpeedFixMagicNumber", gCurrentLevel.speedFixMagicNumber);
    SAVE_GAME_STATE_CONFIG_BASE64("CurrentLevelName", gCurrentLevel.name, sizeof(gCurrentLevel.name));
    writeConfigInt(config, "CurrentLevelFreezeZonks", gCurrentLevel.freezeZonks);
    writeConfigInt(config, "CurrentLevelNumberOfInfotrons", gCurrentLevel.numberOfInfotrons);
    writeConfigInt(config, "CurrentLevelNumberOfSpecialPorts", gCurrentLevel.numberOfSpecialPorts);

#define SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(__value) \
    do \
    { \
        sprintf(specialPortKeyBuffer, "CurrentLevelSpecialPortsInfo_%d_%s", idx, #__value); \
        writeConfigInt(config, specialPortKeyBuffer, gCurrentLevel.specialPortsInfo[idx].__value); \
    } \
    while (0)

    char specialPortKeyBuffer[256] = "";
    for (int idx = 0; idx < kLevelMaxNumberOfSpecialPorts; ++idx)
    {
        SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(position);
        SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(gravity);
        SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(freezeZonks);
        SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(freezeEnemies);
        SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT(unused);
    }

#undef SAVE_GAME_STATE_CONFIG_SPECIAL_PORT_INT

    writeConfigInt(config, "CurrentLevelScrambledSpeed", gCurrentLevel.scrambledSpeed);
    writeConfigInt(config, "CurrentLevelScrambledChecksum", gCurrentLevel.scrambledChecksum);
    writeConfigInt(config, "CurrentLevelRandomSeed", gCurrentLevel.randomSeed);

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

    writeConfigInt(config, "Version", kGameVersion);
    SAVE_GAME_STATE_CONFIG_BASE64("LevelName", levelName, kListLevelNameLength);
//    writeConfigString(config, "LevelSet", &gLevelsDatFilename[8]);
//    writeConfigString(config, "LevelIdentifier", levelName);

//    memcpy(savegame.levelName, levelName, sizeof(savegame.levelName));
//    memcpy(savegame.levelsetSuffix, &gLevelsDatFilename[8], sizeof(savegame.levelsetSuffix));
//    memcpy(savegame.levelIdentifier, levelName, sizeof(savegame.levelIdentifier));

#define SAVE_GAME_STATE_CONFIG_INT(__value) \
    writeConfigInt(config, #__value, g##__value)

    SAVE_GAME_STATE_CONFIG_BASE64("LevelState", gCurrentLevelStateWithPadding, sizeof(gCurrentLevelStateWithPadding));
    SAVE_GAME_STATE_CONFIG_BASE64("ExplosionTimers", gExplosionTimers, sizeof(gExplosionTimers));
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


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

uint8_t saveGameLevelState(Config *config)
{
    char *base64LevelState = encodeBase64((const unsigned char *)gCurrentLevelStateWithPadding, sizeof(gCurrentLevelStateWithPadding));
    if (base64LevelState == NULL)
    {
        return 1;
    }

    writeConfigString(config, "LevelState", base64LevelState);
    free(base64LevelState);

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
    writeConfigString(config, "LevelName", levelName); // TODO: use base64 to prevent weird characters
//    writeConfigString(config, "LevelSet", &gLevelsDatFilename[8]);
//    writeConfigString(config, "LevelIdentifier", levelName);

//    memcpy(savegame.levelName, levelName, sizeof(savegame.levelName));
//    memcpy(savegame.levelsetSuffix, &gLevelsDatFilename[8], sizeof(savegame.levelsetSuffix));
//    memcpy(savegame.levelIdentifier, levelName, sizeof(savegame.levelIdentifier));

#define SAVE_GAME_STATE_CONFIG(__value) \
    writeConfigInt(config, #__value, g##__value)

    saveGameLevelState(config);
    // SAVE_GAME_STATE_CONFIG(CurrentLevelStateWithPadding);
    // SAVE_GAME_STATE_CONFIG(CurrentLevelState);
    //SAVE_GAME_STATE_CONFIG(ExplosionTimers);
    SAVE_GAME_STATE_CONFIG(IsGravityEnabled);
    SAVE_GAME_STATE_CONFIG(AreZonksFrozen);
    SAVE_GAME_STATE_CONFIG(NumberOfInfoTrons);
    SAVE_GAME_STATE_CONFIG(NumberOfSpecialPorts);
    SAVE_GAME_STATE_CONFIG(RandomSeed);
    SAVE_GAME_STATE_CONFIG(AuxGameSeconds20msAccumulator);
    SAVE_GAME_STATE_CONFIG(GameSeconds);
    SAVE_GAME_STATE_CONFIG(GameMinutes);
    SAVE_GAME_STATE_CONFIG(GameHours);
    SAVE_GAME_STATE_CONFIG(ShouldUpdateTotalLevelTime);
    SAVE_GAME_STATE_CONFIG(LevelFailed);
    SAVE_GAME_STATE_CONFIG(CurrentPlayerLevelState);
    SAVE_GAME_STATE_CONFIG(IsExplosionStarted);
    SAVE_GAME_STATE_CONFIG(ShouldShowGamePanel);
    SAVE_GAME_STATE_CONFIG(ToggleGamePanelKeyAutoRepeatCounter);
    SAVE_GAME_STATE_CONFIG(MurphyTileX);
    SAVE_GAME_STATE_CONFIG(MurphyTileY);
    SAVE_GAME_STATE_CONFIG(MurphyPreviousLocation);
    SAVE_GAME_STATE_CONFIG(MurphyLocation);
    SAVE_GAME_STATE_CONFIG(IsMurphyLookingLeft);
    SAVE_GAME_STATE_CONFIG(MurphyYawnAndSleepCounter);
    SAVE_GAME_STATE_CONFIG(IsMurphyUpdated);
    SAVE_GAME_STATE_CONFIG(ShouldKillMurphy);
    SAVE_GAME_STATE_CONFIG(PreviousUserInputWasNone);
    SAVE_GAME_STATE_CONFIG(AreEnemiesFrozen);
    SAVE_GAME_STATE_CONFIG(ScratchGravity);
    SAVE_GAME_STATE_CONFIG(IsMurphyGoingThroughPortal);
    SAVE_GAME_STATE_CONFIG(PlantedRedDiskCountdown);
    SAVE_GAME_STATE_CONFIG(PlantedRedDiskPosition);
    SAVE_GAME_STATE_CONFIG(DemoCurrentInputIndex);
    SAVE_GAME_STATE_CONFIG(DemoCurrentInput);
    SAVE_GAME_STATE_CONFIG(DemoCurrentInputRepeatCounter);
    SAVE_GAME_STATE_CONFIG(DemoIndexOrDemoLevelNumber);
    SAVE_GAME_STATE_CONFIG(MurphyPositionX);
    SAVE_GAME_STATE_CONFIG(MurphyPositionY);
    SAVE_GAME_STATE_CONFIG(MurphyCounterToStartPushAnimation);
    //SAVE_GAME_STATE_CONFIG(CurrentMurphyAnimation);
    SAVE_GAME_STATE_CONFIG(NumberOfRemainingInfotrons);
    SAVE_GAME_STATE_CONFIG(TotalNumberOfInfotrons);
    SAVE_GAME_STATE_CONFIG(NumberOfRemainingRedDisks);
    SAVE_GAME_STATE_CONFIG(FrameCounter);
    SAVE_GAME_STATE_CONFIG(TerminalMaxFramesToNextScroll);
    SAVE_GAME_STATE_CONFIG(AreYellowDisksDetonated);
    SAVE_GAME_STATE_CONFIG(ShouldLeaveMainMenu);
    SAVE_GAME_STATE_CONFIG(ShouldExitLevel);
    SAVE_GAME_STATE_CONFIG(QuitLevelCountdown);
    SAVE_GAME_STATE_CONFIG(AdditionalInfoInGamePanelFrameCounter);
    //SAVE_GAME_STATE_CONFIG(CurrentLevel);
#undef SAVE_GAME_STATE_CONFIG

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


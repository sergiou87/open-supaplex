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

#include <string.h>

#include "demo.h"

// These two are some kind of magic number to identify savegames
// uint16_t word_5A309 = 0x5053;
// uint16_t word_5A30B = 0x1A0D;
char kSaveGameMagicNumber[4] = "OSPX";

uint8_t saveGameState()
{
    FILE *file = openWritableFile(gSavegameSavFilename, "w");
    if (file == NULL)
    {
        return 1;
    }

//loc_499D8:              ; CODE XREF: handleGameUserInput+478j
    // 01ED:2D7B

    Savegame savegame;
    memset(&savegame, 0, sizeof(Savegame));
    memcpy(savegame.magicNumber, kSaveGameMagicNumber, sizeof(savegame.magicNumber));

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

    savegame.savegameVersion = kGameVersion;

    memcpy(savegame.levelName, levelName, sizeof(savegame.levelName));
    memcpy(savegame.levelsetSuffix, &gLevelsDatFilename[8], sizeof(savegame.levelsetSuffix));
    memcpy(savegame.levelIdentifier, levelName, sizeof(savegame.levelIdentifier));

    // TODO: implement savegames properly
    //savegame.gameState = gCurrentGameState;

    size_t bytes = fwrite(&savegame, 1, sizeof(Savegame), file);
    if (bytes < sizeof(Savegame))
    {
//loc_49C1F:              ; CODE XREF: handleGameUserInput+48Bj
//                    ; handleGameUserInput+499j ...
        fclose(file);
        return 1;
    }

//loc_49A6A:              ; CODE XREF: handleGameUserInput+50Aj
    if (fclose(file) != 0)
    {
        return 1;
    }

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

    fclose(file);

//loc_49ADF:              ; CODE XREF: handleGameUserInput+57Fj
    memcpy(gCurrentLevelName, savegame.levelName, sizeof(gCurrentLevelName));

    // TODO: implement savegames properly
    // gCurrentGameState = savegame.gameState;

    return 0;
}


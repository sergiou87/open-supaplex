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

#include "commandLineParser.h"

#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "demo.h"
#include "globals.h"
#include "logging.h"
#include "utils.h"

//uint8_t word_59B60 = 0; // 'A' command line option
//uint8_t word_59B61 = 0; // 'B' command line option
uint8_t gShouldRecreateLevelLstIfNeeded = 0; // 'C' command line option -> If deleted, Create LEVEL.L?? file out of info from LEVELS.D??
uint8_t byte_59B63 = 0; // 'D' command line option -> Force Debug mode at start: needed to record demo's etc.
//uint8_t byte_59B64 = 0; // 'E' command line option -> Force (buggy) EGA mode on VGA hardware
//uint8_t word_59B65 = 0; // 'F' command line option -> Force original Floppy 1<->2 symbol function (Invert Alt key)
//uint8_t word_59B66 = 0; // 'G' command line option
//uint8_t byte_59B67 = 0; // 'H' command line option -> Force original Supaplex Horizontal smooth-scroll timing
//uint8_t byte_59B68 = 0; // 'I' command line option
//uint8_t byte_59B69 = 0; // 'J' command line option
//uint8_t byte_59B6A = 0; // 'K' command line option
uint8_t gShouldStartFromSavedSnapshot = 0; // byte_59B6B -> 'L' command line option -> Load and play the available saved game at start
//uint8_t byte_59B6C = 0; // 'M' command line option
uint8_t gShakeWithExplosionsDisabled = 0; // 'N' command line option -> Never shake the screen during explosions (See also "S")
uint16_t gShouldRecordWithOriginalDemoFilenames = 0; // word_59B6E -> 0x985E -> 'O' command line option -> Record using Original demo names DEMO?.B?? (not ??S???$?.SP)
//uint8_t byte_59B6F = 0; // 'P' command line option
//uint8_t byte_59B70 = 0; // 'Q' command line option
//uint8_t byte_59B71 = 0; // 'R' command line option -> Refresh video memory after each game: reload MOVING.DAT
uint8_t gShouldShakeWithAllExplosions = 0; // byte_59B72 -> 'S' command line option -> Shake the screen during every explosion (See also "N")
uint8_t word_59B73 = 0; // 'T' command line option -> Allow the use of the original infinite Red Disk (ch)eat Trick
//uint8_t byte_59B74 = 0; // 'U' command line option
//uint8_t byte_59B75 = 0; // 'V' command line option
uint8_t gShouldAlwaysWriteSavegameSav = 0; // dword_59B76 -> 'W' command line option -> Force Writing only one SAVEGAME.SAV (else use SAVEGAME.S??)
//uint8_t byte_59B77 = 0; // 'X' command line option
//uint8_t byte_59B78 = 0; // 'Y' command line option
//uint8_t byte_59B79 = 0; // 'Z' command line option

uint8_t gForcedInitialGameSpeed = kInvalidForcedInitialGameSpeed;

uint8_t gIsSPDemoAvailableToRun = 0;
uint8_t fileIsDemo = 0;
uint16_t gSelectedOriginalDemoFromCommandLineLevelNumber = 0;

typedef struct {
    const char *name;
    const char shortName;
    uint8_t hasArgument;
    const char *description;
} OpenSupaplexCommandLineOption;

#define kNumberOfCommandLineOptions 17

static const OpenSupaplexCommandLineOption kFullCommandLineOptions[kNumberOfCommandLineOptions] = {
    { "help", 'h', 0, "Shows this help" },
    { "level-set", 'z', 1, "Force level SET number at start, else original set. Usage: -z nn (nn=0...99)" },
    { "level", 'l', 1, "Force LEVEL number at start. Usage: -l nnn (nnn=1...111)" },
    { "player", 'n', 1, "Force PLAYER number at start. Usage: -n nn (nn=1...20)" },
    { "game-speed", 'g', 1, "Force SpeedFix SPEED at start. Usage: -g nn (nn=0...10, or empty(=0=slow))" },
    { "test-mode", 't', 0, "Force all levels skipped and no score updates: test mode" },
    { "recreate-levels", 'r', 0, "If deleted, Create LEVEL.L\?\? file out of info from LEVELS.D\?\?" },
    { "debug", 'd', 0, "Force Debug mode at start: needed to record demo's etc." },
    // { "", "", 0, "Force original Floppy 1<->2 symbol function (Invert Alt key)" },
    // { "", "", 0, "Force original Supaplex Horizontal smooth-scroll timing" },
    { "load-savegame", 'v', 0, "Load and play the available saved game at start" },
    // { "", "", 0, "View beyond the game field edges with the debug M key options" },
    { "no-shake-explosions", 'e', 0, "Never shake the screen during explosions (See also \"S\")" },
    { "original-demos", 'o', 0, "Record using Original demo names DEMO\?.B\?\? (not \?\?S\?\?\?$\?.SP)" },
    // { "", "", "Refresh video memory after each game: reload MOVING.DAT" },
    { "shake-explosions", 's', 0, "Shake the screen during every explosion (See also \"N\")" },
    { "red-disk-cheat", 'k', 0, "Allow the use of the original infinite Red Disk (ch)eat Trick" },
    { "force-savegame-sav", 'f', 0, "Force Writing only one SAVEGAME.SAV (else use SAVEGAME.S\?\?)" },
    { "play-sp-file", 'p', 1, "Use SP-file to play at start and from menu with F11 and F12" },
    { "quick-demo", 'q', 1, "Lightning speed SP-demo test, exits the game with message" },
    { "ultra-quick-demo", 'u', 1, "Ludicrous speed SP-demo test, exits the game with message" },
};

static struct option options[kNumberOfCommandLineOptions + 1];

void handlePrintHelpOption(void);
void handleForceLevelSetOption(void);
void handleForceLevelNumberOption(void);
void handleForcePlayerNumberOption(void);
void handleForceGameSpeedOption(void);
void handleEnableTestModeOption(void);
void handlePlayDemoFile(FastModeType fastModeType);

void parseCommandLineOptions(int argc, char *argv[])
{
    char optstring[kNumberOfCommandLineOptions * 2];
    int optstringLength = 0;

    for (int idx = 0; idx < kNumberOfCommandLineOptions; ++idx)
    {
        OpenSupaplexCommandLineOption fullCommandLineOption = kFullCommandLineOptions[idx];
        options[idx] = (struct option) {
            fullCommandLineOption.name,
            (fullCommandLineOption.hasArgument ? required_argument : no_argument),
            0,
            fullCommandLineOption.shortName,
        };

        optstring[optstringLength] = fullCommandLineOption.shortName;
        optstringLength++;
        if (fullCommandLineOption.hasArgument)
        {
            optstring[optstringLength] = ':';
            optstringLength++;
        }
    }

    optstring[optstringLength] = '\0';
    options[kNumberOfCommandLineOptions] = (struct option) { 0, 0, 0, 0 };

    int opt = 0;
    int optionIndex = 0;
    while ((opt = getopt_long(argc, argv, optstring, options, &optionIndex)) != -1)
    {
        switch (opt)
        {
            case 'h':
                handlePrintHelpOption();
                break;
            case 'z':
                handleForceLevelSetOption();
                break;
            case 'l':
                handleForceLevelNumberOption();
                break;
            case 'n':
                handleForcePlayerNumberOption();
                break;
            case 'g':
                handleForceGameSpeedOption();
                break;
            case 't':
                handleEnableTestModeOption();
                break;
            case 'r':
                gShouldRecreateLevelLstIfNeeded = 1;
                break;
            case 'd':
                byte_59B63 = 1;
                break;
            case 'v':
                gShouldStartFromSavedSnapshot = 1;
                break;
            case 'e':
                gShakeWithExplosionsDisabled = 1;
                break;
            case 'o':
                gShouldRecordWithOriginalDemoFilenames = 1;
                break;
            case 's':
                gShouldShakeWithAllExplosions = 1;
                break;
            case 'k':
                word_59B73 = 1;
                break;
            case 'f':
                gShouldAlwaysWriteSavegameSav = 1;
                strcpy(&gSavegameSavFilename[10], "AV");
                break;
            case 'p':
                handlePlayDemoFile(FastModeTypeNone);
                break;
            case 'q':
                handlePlayDemoFile(FastModeTypeNormal);
                break;
            case 'u':
                handlePlayDemoFile(FastModeTypeUltra);
                break;
        }
    }

    if (byte_59B63 != 0)
    {
        gIsDebugModeEnabled = 1;
        spLogInfo("Debug mode enabled");
    }
}

void handlePrintHelpOption(void)
{
    printf("Welcome to OpenSupaplex v" VERSION_STRING "!\n\n");
    printf("You can use the following parameters:\n");
    for (int idx = 0; idx < kNumberOfCommandLineOptions; ++idx)
    {
        OpenSupaplexCommandLineOption fullCommandLineOption = kFullCommandLineOptions[idx];

        printf("-%c, --%-20s\t\t%s\n",
               fullCommandLineOption.shortName,
               fullCommandLineOption.name,
               fullCommandLineOption.description);
    }
    printf("\nWritten by Sergio Padrino (sergiou87)");

    exit(0);
}

void handleForceLevelSetOption(void)
{
    // Force level SET number at start (nn=0...99), else original set
    int numberValue = atoi(optarg);
    if (numberValue < 0 || numberValue > 99)
    {
        return;
    }

    char newSuffix[3] = "00";
    snprintf(newSuffix, 3, "%02d", numberValue);

    spLogInfo("Replacing level set with %s", newSuffix);

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

void handleForceLevelNumberOption(void)
{
//loc_46E59:              //; CODE XREF: start+221j start+230j
    // Force LEVEL number at start (nnn=1...111)
    int levelNumber = atoi(optarg);
    levelNumber = CLAMP(levelNumber, 1, kNumberOfLevels);
    gIsForcedLevel = levelNumber;
    spLogInfo("Playing level %d", levelNumber);
}

void handleForcePlayerNumberOption(void)
{
//loc_46E18:              //; CODE XREF: start+1A7j
    // Force PLAYER number at start (nn=1...20)
    int playerNumber = atoi(optarg);
    playerNumber = CLAMP(playerNumber, 1, 20);

    gCurrentPlayerIndex =
    byte_58D46 = playerNumber - 1;
    spLogInfo("Forcing player %d", playerNumber);
}

void handleForceGameSpeedOption(void)
{
//loc_46E39:              //; CODE XREF: start+1F6j start+200j
    // Force SpeedFix SPEED at start (nn=0...10, or empty(=0=slow))
    int newSpeed = atoi(optarg);
    newSpeed = CLAMP(newSpeed, 0, 10);
    // We can't store this directly in gGameSpeed because it will be overriden by the stored configuration
    gForcedInitialGameSpeed = newSpeed;
    spLogInfo("Starting with speed %d", newSpeed);
}

void handleEnableTestModeOption(void)
{
//loc_46DBF:              //; CODE XREF: start+166j start+16Bj ...
    // Force all levels skipped and no score updates: test mode
    gIsForcedCheatMode = 0x0FF;
    PlayerEntry *firstPlayer = &gPlayerListData[0];
    strcpy(firstPlayer->name, "(FORCED)");
    memset(firstPlayer->levelState, PlayerLevelStateSkipped, kNumberOfLevels);

    for (int i = 1; i < 3; ++i)
    {
        PlayerEntry *player = &gPlayerListData[i];
        strcpy(player->name, "        ");

        strcpy(gRankingTextEntries[kRankingFirstPlayerIndex + i], "                      ");
    }

    spLogInfo("Enabling test mode");
}

void handlePlayDemoFile(FastModeType fastModeType)
{
    strcpy(demoFileName, optarg);

    FILE *file = openWritableFileWithReadonlyFallback(demoFileName, "rb");
    if (file == NULL)
    {
        if (fastModeType != FastModeTypeNone)
        {
            spLogDemo("\"@\"-ERROR: Bad or missing file %s. %s", demoFileName, strerror(errno));
            exit(1);
        }

        strcpy(demoFileName, "");
        gIsSPDemoAvailableToRun = 0;
        return;
    }

    long fileLength = 0;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
    }
    else
    {
        fileLength = ftell(file);

        if (fileLength >= kLevelDataLength)
        {
            fclose(file);
        }
    }

    gSelectedOriginalDemoFromCommandLineLevelNumber = 0;

    if (fileLength > kMaxBaseDemoSize + kMaxDemoSignatureSize + kLevelDataLength)
    {
//lookForAtSignInCommandLine:              //; CODE XREF: start+A6j start+ABj
        if (fastModeType != FastModeTypeNone)
        {
            spLogDemo("!! File >> Demo: %s", demoFileName); // (meaning: file too long)
            exit(1);
        }
        else
        {
            strcpy(demoFileName, "");
            gIsSPDemoAvailableToRun = 0;
            return;
        }
    }

    if (fileLength < kLevelDataLength) // all demo files with the new format are greater than a level (1536 bytes)
    {
// loc_46CB7:              //; CODE XREF: start:loc_46CADj
        gSelectedOriginalDemoFromCommandLineLevelNumber = getLevelNumberFromOriginalDemoFile(file, fileLength);

        fclose(file);

        if (gSelectedOriginalDemoFromCommandLineLevelNumber != 0)
        {
//loc_46CF6:              //; CODE XREF: start+C2j
            fileIsDemo = 1;
            gIsSPDemoAvailableToRun = 2;
        }
        else
        {
            if (fastModeType != FastModeTypeNone)
            {
                spLogDemo("!! File < Level: %s", demoFileName); // (meaning: file too short)
                exit(1);
            }
            else
            {
                strcpy(demoFileName, "");
                gIsSPDemoAvailableToRun = 0;
                return;
            }
        }
    }
//loc_46CF4:              //; CODE XREF: start+B4j
    else if (fileLength == kLevelDataLength) // all demo files are greater than a level (1536 bytes)
    {
//loc_46CFB:              //; CODE XREF: start:loc_46CF4j
        gIsSPDemoAvailableToRun = 2;
    }
    else
    {
//loc_46CF6:              //; CODE XREF: start+C2j
        fileIsDemo = 1;
        gIsSPDemoAvailableToRun = 2;
    }

    if (fastModeType != FastModeTypeNone)
    {
//demoFileNotMissing:              //; CODE XREF: start+10Bj
        if (fileIsDemo != 1)
        {
            spLogDemo("SP without demo: %s", demoFileName); // (meaning: level only)
            exit(1);
        }

//spHasAtAndDemo:              //; CODE XREF: start+11Cj
        gFastMode = fastModeType;
    }
}

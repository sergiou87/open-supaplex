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

#ifndef commandLineParser_h
#define commandLineParser_h

#include <stdio.h>
#include <stdint.h>

//extern uint8_t word_59B60; // 'A' command line option
//extern uint8_t word_59B61; // 'B' command line option
extern uint8_t gShouldRecreateLevelLstIfNeeded; // byte_59B62 -> 'C' command line option -> If deleted, Create LEVEL.L?? file out of info from LEVELS.D??
extern uint8_t byte_59B63; // 'D' command line option -> Force Debug mode at start: needed to record demo's etc.
//extern uint8_t byte_59B64; // 'E' command line option -> Force (buggy) EGA mode on VGA hardware
//extern uint8_t word_59B65; // 'F' command line option -> Force original Floppy 1<->2 symbol function (Invert Alt key)
//extern uint8_t word_59B66; // 'G' command line option
//extern uint8_t byte_59B67; // 'H' command line option -> Force original Supaplex Horizontal smooth-scroll timing
//extern uint8_t byte_59B68; // 'I' command line option
//extern uint8_t byte_59B69; // 'J' command line option
//extern uint8_t byte_59B6A; // 'K' command line option
extern uint8_t gShouldStartFromSavedSnapshot; // byte_59B6B -> 'L' command line option -> Load and play the available saved game at start
//extern uint8_t byte_59B6C; // 'M' command line option
extern uint8_t gShakeWithExplosionsDisabled; // byte_59B6D -> 'N' command line option -> Never shake the screen during explosions (See also "S")
extern uint16_t gShouldRecordWithOriginalDemoFilenames; // word_59B6E -> 0x985E -> 'O' command line option -> Record using Original demo names DEMO?.B?? (not ??S???$?.SP)
//extern uint8_t byte_59B6F; // 'P' command line option
//extern uint8_t byte_59B70; // 'Q' command line option
//extern uint8_t byte_59B71; // 'R' command line option -> Refresh video memory after each game: reload MOVING.DAT
extern uint8_t gShouldShakeWithAllExplosions; // 'S' command line option -> Shake the screen during every explosion (See also "N")
extern uint8_t word_59B73; // 'T' command line option -> Allow the use of the original infinite Red Disk (ch)eat Trick
//extern uint8_t byte_59B74; // 'U' command line option
//extern uint8_t byte_59B75; // 'V' command line option
extern uint8_t gShouldAlwaysWriteSavegameSav; // 'W' command line option -> Force Writing only one SAVEGAME.SAV (else use SAVEGAME.S??)
//extern uint8_t byte_59B77; // 'X' command line option
//extern uint8_t byte_59B78; // 'Y' command line option
//extern uint8_t byte_59B79; // 'Z' command line option

#define kInvalidForcedInitialGameSpeed 0xFF
extern uint8_t gForcedInitialGameSpeed;

extern uint8_t gIsSPDemoAvailableToRun; // byte_599D4 -> 0=don't run .SP, 1=run .SP, 2=run .SP at startup
extern uint8_t fileIsDemo;
extern uint16_t gSelectedOriginalDemoFromCommandLineLevelNumber; // word_599DA -> first demo byte if SP is an old demo

void parseCommandLineOptions(int argc, char *argv[]);

#endif /* commandLineParser_h */

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

#ifndef conditionals_h
#define conditionals_h

#include <stdint.h>

uint8_t supportsRealKeyboard(void);
uint8_t supportsVirtualKeyboard(void);

// This is for platforms that allow in some way to select a 00S000$0.SP demo file for playback. For example, on
// PC the user can use a command line option to play a specific demo. But right now not on consoles using the
// advanced menu.
// TODO: add some kind of demo file selector to advanced menu.
//
uint8_t supportsSPFileDemoPlayback(void);

#endif /* conditionals_h */

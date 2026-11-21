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

#ifndef savegame_h
#define savegame_h

#include "globals.h"

// This is the only file format that is not compatible with the original game: while most of the data is
// still exactly the same, some other things are just useless/pointless in the reimplementation, because
// they weren't game data only, but also info tightly coupled to the rendering/hardware requirements it had.

uint8_t saveGameState(void);
uint8_t canLoadGameState(void);
uint8_t loadGameState(void);

#endif /* savegame_h */

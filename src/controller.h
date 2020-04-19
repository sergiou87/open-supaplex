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

#ifndef controller_h
#define controller_h

#include <SDL2/SDL.h>

// -1, 0, 1
int8_t getGameControllerX(void);
int8_t getGameControllerY(void);
uint8_t getGameControllerButton(SDL_GameControllerButton button);
uint8_t isAnyGameControllerButtonPressed(void);

void gameControllerEmulateMouse(float *x, float *y, uint8_t *leftButton, uint8_t *rightButton);

#endif /* controller_h */

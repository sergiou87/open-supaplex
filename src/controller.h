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

#include <stdint.h>

// -1, 0, 1
int8_t getGameControllerX(void);
int8_t getGameControllerY(void);
uint8_t getGameControllerButtonA(void);
uint8_t getGameControllerButtonB(void);
uint8_t getGameControllerButtonX(void);
uint8_t getGameControllerButtonY(void);
uint8_t getGameControllerButtonBack(void);
uint8_t getGameControllerButtonStart(void);
uint8_t getGameControllerButtonLeftShoulder(void);
uint8_t getGameControllerButtonRightShoulder(void);

uint8_t isAnyGameControllerButtonPressed(void);

uint8_t getGameControllerConfirmButton(void);
uint8_t getGameControllerCancelButton(void);

void gameControllerEmulateMouse(float *x, float *y, uint8_t *leftButton, uint8_t *rightButton);

#endif /* controller_h */

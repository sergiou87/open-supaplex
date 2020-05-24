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

#ifndef input_h
#define input_h

#include <stdint.h>

uint8_t isUpButtonPressed(void);
uint8_t isDownButtonPressed(void);
uint8_t isLeftButtonPressed(void);
uint8_t isRightButtonPressed(void);
uint8_t isActionButtonPressed(void);
uint8_t isMenuBackButtonPressed(void);
uint8_t isMenuConfirmButtonPressed(void);
uint8_t isMenuCancelButtonPressed(void);
uint8_t isExitLevelButtonPressed(void);
uint8_t isStartButtonPressed(void);
uint8_t isPauseButtonPressed(void);
uint8_t isToggleGamePanelButtonPressed(void);

uint8_t isRotateLevelSetAscendingButtonPressed(void);
uint8_t isRotateLevelSetDescendingButtonPressed(void);

#endif /* input_h */

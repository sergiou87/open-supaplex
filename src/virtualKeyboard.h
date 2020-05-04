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

#ifndef virtualKeyboard_h
#define virtualKeyboard_h

#include <stdint.h>

uint8_t isRealKeyboardSupported(void);
uint8_t isVirtualKeyboardSupported(void);
uint8_t inputVirtualKeyboardText(const char *title, uint16_t maxLength, char *outText);

#endif /* virtualKeyboard_h */

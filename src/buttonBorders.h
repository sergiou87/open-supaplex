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

#ifndef buttonBorders_h
#define buttonBorders_h

#include <stdint.h>

enum ButtonBorderLineType {
    ButtonBorderLineTypeHorizontal = 0, // from left to right
    ButtonBorderLineTypeVertical = 1, // from bottom to top
    ButtonBorderLineTypeBottomLeftToTopRightDiagonal = 2,
    ButtonBorderLineTypeTopLeftToBottomRightDiagonal = 3,
};

typedef struct {
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint16_t length;
} ButtonBorderLineDescriptor;

typedef struct {
    ButtonBorderLineDescriptor lines[15];
    uint8_t numberOfLines;
} ButtonBorderDescriptor;

#define kNumberOfOptionsMenuBorders 20
extern const ButtonBorderDescriptor kOptionsMenuBorders[kNumberOfOptionsMenuBorders];

#define kNumberOfMainMenuButtonBorders 12
extern const ButtonBorderDescriptor kMainMenuButtonBorders[kNumberOfMainMenuButtonBorders];

#endif /* buttonBorders_h */

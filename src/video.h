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

#ifndef video_h
#define video_h

#include <stdio.h>

#define kScreenWidth 320
#define kScreenHeight 200

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef enum {
    ScalingModeAspectFit,
    ScalingModeAspectFill,
    ScalingModeIntegerFactor,
    ScalingModeFullscreen,
    ScalingModeCount,
} ScalingMode;

#define kNumberOfColors 16
typedef Color ColorPalette[kNumberOfColors];

extern uint8_t *gScreenPixels;

void initializeVideo(void);
void render(void);
void present(void);
void destroyVideo(void);

ScalingMode getScalingMode(void);
void setScalingMode(ScalingMode mode);

void getWindowSize(int *width, int *height);
void centerMouse(void);
void moveMouse(int x, int y);

void toggleFullscreen(void);

void setColorPalette(const ColorPalette palette);

#endif /* video_h */

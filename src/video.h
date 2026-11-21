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

#include <stdint.h>

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
    ScalingModeAspectCorrect,
    ScalingModeCount,
} ScalingMode;

#define kNumberOfColors 16
typedef Color ColorPalette[kNumberOfColors];

extern uint8_t *gScreenPixels;

void initializeVideo(uint8_t fastMode);
void render(void);
void present(void);
void destroyVideo(void);

ScalingMode getScalingMode(void);
void setScalingMode(ScalingMode mode);

void getWindowSize(int *width, int *height);

void centerMouse(void);
void moveMouse(int x, int y);
void hideMouse(void);
void getMouseState(int *x, int *y, uint8_t *leftButton, uint8_t *rightButton);

void toggleFullscreen(void);
void setFullscreenMode(uint8_t fullscreen);
uint8_t getFullscreenMode(void);

void setGlobalPaletteColor(const uint8_t index, const Color color);
void setColorPalette(const ColorPalette palette);

#endif /* video_h */

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

#include "../video.h"

#include "../logging.h"
#include "../utils.h"


uint8_t kScreenPixels[kScreenWidth * kScreenHeight];
uint8_t *gScreenPixels = kScreenPixels;
ScalingMode gScalingMode = ScalingModeAspectFit;

void initializeVideo(uint8_t fastMode)
{
    //int ret = SDL_Init(SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
    // if (ret)
    // {
    //     spLogInfo("SDL_Init failed with %d", ret);
    //     exit(1);
    // }
}

void render()
{
    // Do nothing
}

void present()
{
    // Do nothing
}

void destroyVideo()
{
    // Do nothing
}

ScalingMode getScalingMode(void)
{
    return gScalingMode;
}

void setScalingMode(ScalingMode mode)
{
    if (mode == gScalingMode)
    {
        return;
    }

    gScalingMode = mode;
}

void getWindowSize(int *width, int *height)
{
    // Do nothing
}

void centerMouse()
{
    // Do nothing
}

void moveMouse(int x, int y)
{
    // Do nothing
}

void hideMouse(void)
{
    // Do nothing
}

void getMouseState(int *x, int *y, uint8_t *leftButton, uint8_t *rightButton)
{
    // Do nothing
}

void toggleFullscreen()
{
    // Do nothing
}

void setFullscreenMode(uint8_t fullscreen)
{
    // Do nothing
}

uint8_t getFullscreenMode(void)
{
    // Do nothing
    return 1;
}

void setGlobalPaletteColor(const uint8_t index, const Color color)
{
    // Do nothing
}

void setColorPalette(const ColorPalette palette)
{
    // Do nothing
}
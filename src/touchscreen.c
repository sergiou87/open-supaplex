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

#include "touchscreen.h"

#include <SDL2/SDL.h>

uint8_t readTouchScreen(float *x, float *y)
{
    if (SDL_GetNumTouchDevices() == 0)
    {
        return 0;
    }

    SDL_TouchID touchID = SDL_GetTouchDevice(0);

    int numTouches = SDL_GetNumTouchFingers(touchID);

    if (numTouches > 0)
    {
        SDL_Finger *finger = SDL_GetTouchFinger(touchID, 0);

        *x = finger->x;
        *y = finger->y;

        return 1;
    }

    return 0;
}

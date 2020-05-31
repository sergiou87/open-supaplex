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

#include "../controller.h"

#include <SDL2/SDL.h>
#include <stdlib.h>

static int sCurrentGameControllerIndex = -1;
static SDL_GameController *sCurrentGameController = NULL;
static const float kJoystickDeadzone = 0.1;

SDL_GameController *getGameController()
{
    // The game controller is not valid anymore, invalidate.
    if (sCurrentGameControllerIndex != -1
        && SDL_IsGameController(sCurrentGameControllerIndex) == 0)
    {
        sCurrentGameController = NULL;
    }

    if (sCurrentGameController != NULL)
    {
        return sCurrentGameController;
    }

    int numberOfJoysticks = SDL_NumJoysticks();

    for (int i = 0; i < numberOfJoysticks; ++i)
    {
        if (SDL_IsGameController(i))
        {
            sCurrentGameController = SDL_GameControllerOpen(i);
            sCurrentGameControllerIndex = i;
            break;
        }
    }

    return sCurrentGameController;
}

int8_t getGameControllerAxis(SDL_GameControllerAxis axis, SDL_GameControllerButton minButton, SDL_GameControllerButton maxButton)
{
    SDL_GameController *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    Sint16 axisValue = SDL_GameControllerGetAxis(controller, axis);

    Sint16 threshold = SDL_MAX_SINT16 * kJoystickDeadzone;

    if (axisValue > threshold)
    {
        return 1;
    }
    else if (axisValue < -threshold)
    {
        return -1;
    }
    else if (SDL_GameControllerGetButton(controller, maxButton))
    {
        return 1;
    }
    else if (SDL_GameControllerGetButton(controller, minButton))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int8_t getGameControllerX(void)
{
    return getGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTX,
                                 SDL_CONTROLLER_BUTTON_DPAD_LEFT,
                                 SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
}

int8_t getGameControllerY(void)
{
    return getGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTY,
                                 SDL_CONTROLLER_BUTTON_DPAD_UP,
                                 SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}

void gameControllerEmulateMouse(float *x, float *y, uint8_t *leftButton, uint8_t *rightButton)
{
    SDL_GameController *controller = getGameController();

    if (controller == NULL)
    {
        return;
    }

    Sint16 yAxis = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    Sint16 xAxis = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);

    Uint8 confirmButton = getGameControllerConfirmButton();
    Uint8 cancelButton = getGameControllerCancelButton();

    static const Sint16 JOYSTICK_MOUSE_THRESHOLD = 8192;

    *x = abs(xAxis) * 8.0 / SDL_MAX_SINT16 + 1.0;
    *y = abs(yAxis) * 8.0 / SDL_MAX_SINT16 + 1.0;

    if (xAxis < 0)
        *x = -*x;
    if (yAxis < 0)
        *y = -*y;

    if (abs(xAxis) < JOYSTICK_MOUSE_THRESHOLD)
        *x = 0;
    if (abs(yAxis) < JOYSTICK_MOUSE_THRESHOLD)
        *y = 0;

    *leftButton = confirmButton;
    *rightButton = cancelButton;
}

uint8_t getGameControllerButton(SDL_GameControllerButton button)
{
    SDL_GameController *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    return SDL_GameControllerGetButton(controller, button);
}

uint8_t getGameControllerButtonA(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_A);
}

uint8_t getGameControllerButtonB(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_B);
}

uint8_t getGameControllerButtonX(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_X);
}

uint8_t getGameControllerButtonY(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_Y);
}

uint8_t getGameControllerButtonBack(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_BACK);
}

uint8_t getGameControllerButtonStart(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_START);
}

uint8_t getGameControllerButtonLeftShoulder(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
}

uint8_t getGameControllerButtonRightShoulder(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

uint8_t getGameControllerConfirmButton(void)
{
#if defined(__SWITCH__)
    return getGameControllerButtonB();
#else
    return getGameControllerButtonA();
#endif
}

uint8_t getGameControllerCancelButton(void)
{
#if defined(__SWITCH__)
    return getGameControllerButtonA();
#else
    return getGameControllerButtonB();
#endif
}

uint8_t isAnyGameControllerButtonPressed(void)
{
    SDL_GameController *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    for (SDL_GameControllerButton button = SDL_CONTROLLER_BUTTON_A;
         button <= SDL_CONTROLLER_BUTTON_START;
         ++button)
    {
        if (SDL_GameControllerGetButton(controller, button))
        {
            return 1;
        }
    }

    return 0;
}

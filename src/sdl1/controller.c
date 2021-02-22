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

#include <SDL/SDL.h>
#include <stdlib.h>

#include "../logging.h"

#if defined(__PSP__)
const uint8_t GAMEPAD_BUTTON_DOWN = 6;
const uint8_t GAMEPAD_BUTTON_LEFT = 7;
const uint8_t GAMEPAD_BUTTON_UP = 8;
const uint8_t GAMEPAD_BUTTON_RIGHT = 9;
const uint8_t GAMEPAD_BUTTON_A = 2;
const uint8_t GAMEPAD_BUTTON_B = 1;
const uint8_t GAMEPAD_BUTTON_X = 3;
const uint8_t GAMEPAD_BUTTON_Y = 0;
const uint8_t GAMEPAD_BUTTON_LSHOULDER = 4;
const uint8_t GAMEPAD_BUTTON_RSHOULDER = 5;
const uint8_t GAMEPAD_BUTTON_START = 11;
const uint8_t GAMEPAD_BUTTON_BACK = 10;
#elif defined(__NDS__) || defined(_3DS)
const uint8_t GAMEPAD_BUTTON_DOWN = 10;
const uint8_t GAMEPAD_BUTTON_LEFT = 10;
const uint8_t GAMEPAD_BUTTON_UP = 10;
const uint8_t GAMEPAD_BUTTON_RIGHT = 10;
const uint8_t GAMEPAD_BUTTON_A = 1;
const uint8_t GAMEPAD_BUTTON_B = 2;
const uint8_t GAMEPAD_BUTTON_X = 3;
const uint8_t GAMEPAD_BUTTON_Y = 4;
const uint8_t GAMEPAD_BUTTON_LSHOULDER = 5;
const uint8_t GAMEPAD_BUTTON_RSHOULDER = 6;
const uint8_t GAMEPAD_BUTTON_START = 0;
const uint8_t GAMEPAD_BUTTON_BACK = 7;
#elif defined(__WII__)
const uint8_t GAMEPAD_BUTTON_DOWN = 20;
const uint8_t GAMEPAD_BUTTON_LEFT = 20;
const uint8_t GAMEPAD_BUTTON_UP = 20;
const uint8_t GAMEPAD_BUTTON_RIGHT = 20;
const uint8_t GAMEPAD_BUTTON_1 = 2;
const uint8_t GAMEPAD_BUTTON_2 = 3;
const uint8_t GAMEPAD_BUTTON_A = 1;
const uint8_t GAMEPAD_BUTTON_B = 0;
const uint8_t GAMEPAD_BUTTON_X = 10;
const uint8_t GAMEPAD_BUTTON_Y = 9;
const uint8_t GAMEPAD_BUTTON_LSHOULDER = 11;
const uint8_t GAMEPAD_BUTTON_RSHOULDER = 12;
const uint8_t GAMEPAD_BUTTON_START = 5;
const uint8_t GAMEPAD_BUTTON_BACK = 4;
#else
const uint8_t GAMEPAD_BUTTON_DOWN = 14;
const uint8_t GAMEPAD_BUTTON_LEFT = 15;
const uint8_t GAMEPAD_BUTTON_UP = 16;
const uint8_t GAMEPAD_BUTTON_RIGHT = 17;
const uint8_t GAMEPAD_BUTTON_A = 1;
const uint8_t GAMEPAD_BUTTON_B = 2;
const uint8_t GAMEPAD_BUTTON_X = 0;
const uint8_t GAMEPAD_BUTTON_Y = 3;
const uint8_t GAMEPAD_BUTTON_LSHOULDER = 4;
const uint8_t GAMEPAD_BUTTON_RSHOULDER = 5;
const uint8_t GAMEPAD_BUTTON_START = 9;
const uint8_t GAMEPAD_BUTTON_BACK = 8;
#endif

static SDL_Joystick *sCurrentGameController = NULL;
static const float kJoystickDeadzone = 0.5;

void initializeControllers(void)
{
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
}

SDL_Joystick *getGameController()
{
    if (sCurrentGameController != NULL)
    {
        return sCurrentGameController;
    }

    int numberOfJoysticks = SDL_NumJoysticks();

    if (numberOfJoysticks > 0)
    {
        SDL_JoystickEventState(SDL_ENABLE);
        sCurrentGameController = SDL_JoystickOpen(0);
    }

    return sCurrentGameController;
}

int8_t getGameControllerAxis(int axis, int minButton, int maxButton, int hat, Uint8 minHat, Uint8 maxHat)
{
    SDL_Joystick *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    Sint16 axisValue = SDL_JoystickGetAxis(controller, axis);
    Uint8 hatValue = SDL_JoystickGetHat(controller, hat);

    Sint16 threshold = INT16_MAX * kJoystickDeadzone;

    if (axisValue > threshold)
    {
        return 1;
    }
    else if (axisValue < -threshold)
    {
        return -1;
    }
    else if (SDL_JoystickGetButton(controller, maxButton))
    {
        return 1;
    }
    else if (SDL_JoystickGetButton(controller, minButton))
    {
        return -1;
    }
    else if (hatValue & maxHat)
    {
        return 1;
    }
    else if (hatValue & minHat)
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
#if defined(__WII__)
    // Wii only supports the Wiimote sideways
    return getGameControllerAxis(0, GAMEPAD_BUTTON_LEFT, GAMEPAD_BUTTON_RIGHT, 0, SDL_HAT_UP, SDL_HAT_DOWN);
#else
    return getGameControllerAxis(0, GAMEPAD_BUTTON_LEFT, GAMEPAD_BUTTON_RIGHT, 0, SDL_HAT_LEFT, SDL_HAT_RIGHT);
#endif
}

int8_t getGameControllerY(void)
{
#if defined(__WII__)
    // Wii only supports the Wiimote sideways
    return getGameControllerAxis(1, GAMEPAD_BUTTON_UP, GAMEPAD_BUTTON_DOWN, 0, SDL_HAT_RIGHT, SDL_HAT_LEFT);
#else
    return getGameControllerAxis(1, GAMEPAD_BUTTON_UP, GAMEPAD_BUTTON_DOWN, 0, SDL_HAT_UP, SDL_HAT_DOWN);
#endif
}

void gameControllerEmulateMouse(float *x, float *y, uint8_t *leftButton, uint8_t *rightButton)
{
    SDL_Joystick *controller = getGameController();

    if (controller == NULL)
    {
        return;
    }

    Sint16 xAxis = SDL_JoystickGetAxis(controller, 0);
    Sint16 yAxis = SDL_JoystickGetAxis(controller, 1);

    Uint8 confirmButton = getGameControllerConfirmButton();
    Uint8 cancelButton = getGameControllerCancelButton();

    static const Sint16 JOYSTICK_MOUSE_THRESHOLD = 8192;

    *x = abs(xAxis) * 8.0 / INT16_MAX + 1.0;
    *y = abs(yAxis) * 8.0 / INT16_MAX + 1.0;

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

uint8_t getGameControllerButton(int button)
{
    SDL_Joystick *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    return SDL_JoystickGetButton(controller, button);
}

uint8_t getGameControllerButtonA(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_A);
}

uint8_t getGameControllerButtonB(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_B);
}

uint8_t getGameControllerButtonX(void)
{
#if defined(__WII__)
    return (getGameControllerButton(GAMEPAD_BUTTON_X)
            || getGameControllerButton(GAMEPAD_BUTTON_1));
#else
    return getGameControllerButton(GAMEPAD_BUTTON_X);
#endif
}

uint8_t getGameControllerButtonY(void)
{
#if defined(__WII__)
    return (getGameControllerButton(GAMEPAD_BUTTON_Y)
            || getGameControllerButton(GAMEPAD_BUTTON_2));
#else
    return getGameControllerButton(GAMEPAD_BUTTON_Y);
#endif
}

uint8_t getGameControllerButtonBack(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_BACK);
}

uint8_t getGameControllerButtonStart(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_START);
}

uint8_t getGameControllerButtonLeftShoulder(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_LSHOULDER);
}

uint8_t getGameControllerButtonRightShoulder(void)
{
    return getGameControllerButton(GAMEPAD_BUTTON_RSHOULDER);
}

uint8_t getGameControllerConfirmButton(void)
{
#if defined(__WII__)
    return (getGameControllerButtonB()
            || getGameControllerButton(GAMEPAD_BUTTON_2));
#else
    return getGameControllerButtonA();
#endif
}

uint8_t getGameControllerCancelButton(void)
{
#if defined(__WII__)
    return (getGameControllerButtonA()
            || getGameControllerButton(GAMEPAD_BUTTON_1));
#else
    return getGameControllerButtonB();
#endif
}

uint8_t isAnyGameControllerButtonPressed(void)
{
    SDL_Joystick *controller = getGameController();

    if (controller == NULL)
    {
        return 0;
    }

    for (int button = 0;
         button <= SDL_JoystickNumButtons(controller);
         ++button)
    {
        if (SDL_JoystickGetButton(controller, button))
        {
            return 1;
        }
    }

    return 0;
}

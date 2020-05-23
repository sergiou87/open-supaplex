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

#include "input.h"

#include "controller.h"

SDL_Scancode keyPressed = 0;

uint8_t gIsEscapeKeyPressed = 0; // byte_5197E -> ESC
uint8_t gIs1KeyPressed = 0; // byte_5197F -> 1
uint8_t gIs2KeyPressed = 0; // byte_51980 -> 2
uint8_t gIs3KeyPressed = 0; // byte_51981 -> 3
uint8_t gIs4KeyPressed = 0; // byte_51982 -> 4
uint8_t gIs5KeyPressed = 0; // byte_51983 -> 5
uint8_t gIs6KeyPressed = 0; // byte_51984 -> 6
uint8_t gIs7KeyPressed = 0; // byte_51985 -> 7
uint8_t gIs8KeyPressed = 0; // byte_51986 -> 8
uint8_t gIs9KeyPressed = 0; // byte_51987 -> 9
uint8_t gIs0KeyPressed = 0; // byte_51988 -> 0
uint8_t gIsMinusKeyPressed = 0; // byte_51989 -> -
uint8_t gIsEqualsKeyPressed = 0; // byte_5198A -> =
// backspace
// tab
uint8_t gIsQKeyPressed = 0; // byte_5198D -> q
uint8_t gIsWKeyPressed = 0; // byte_5198E -> w
// e
uint8_t gIsRKeyPressed = 0; // byte_51990 -> r
// t
// y
// u
// i
// o
uint8_t gIsPKeyPressed = 0; // byte_51996 -> p
// [
// ]
uint8_t gIsEnterPressed = 0; // byte_51999 -> enter
uint8_t gIsLeftControlPressed = 0; // byte_5199A -> left control
//a
uint8_t gIsSKeyPressed = 0; // byte_5199C -> s
uint8_t gIsDKeyPressed = 0; // byte_5199D -> d
//f
//g
uint8_t gIsHKeyPressed = 0; // byte_519A0 -> h
uint8_t gIsJKeyPressed = 0; // byte_519A1 -> j
//k
uint8_t gIsLKeyPressed = 0; // dword_519A3 -> l
// ; ' `
uint8_t gIsLeftShiftPressed = 0; // word_519A7 -> "left shift"
// slash
uint8_t gIsZKeyPressed = 0; // word_519A9 -> z
// x
uint8_t gIsCKeyPressed = 0; // word_519AB -> c
// v
uint8_t gIsBKeyPressed = 0; // word_519AD -> b
// n
uint8_t gIsMKeyPressed = 0; // dword_519AF -> m
// , . /
uint8_t gIsRightShiftPressed = 0; // word_519B3 -> "right shift"
// "numpad *"
uint8_t gIsLeftAltPressed = 0; // byte_519B5 -> "left alt"
uint8_t gIsSpaceKeyPressed = 0; // byte_519B6 -> space
// caps lock
uint8_t gIsF1KeyPressed = 0; // byte_519B8 -> F1
uint8_t gIsF2KeyPressed = 0; // byte_519B9 -> F2
uint8_t gIsF3KeyPressed = 0; // byte_519BA -> F3
uint8_t gIsF4KeyPressed = 0; // byte_519BB -> F4
uint8_t gIsF5KeyPressed = 0; // byte_519BC -> F5
uint8_t gIsF6KeyPressed = 0; // byte_519BD -> F6
uint8_t gIsF7KeyPressed = 0; // byte_519BE -> F7
uint8_t gIsF8KeyPressed = 0; // byte_519BF -> F8
uint8_t gIsF9KeyPressed = 0; // byte_519C0 -> F9
uint8_t gIsF10KeyPressed = 0; // byte_519C1 -> F10
uint8_t gIsNumLockPressed = 0; // byte_519C2 -> 16B2 -> Num lock
uint8_t gIsScrollLockPressed = 0; // byte_519C3 -> Scroll lock
uint8_t gIsHomeKeyPressed = 0; // byte_519C4 -> numpad 7
uint8_t gIsUpKeyPressed = 0; // byte_519C5 -> 16B5 -> numpad 8 or up arrow
uint8_t gIsRePagKeyPressed = 0; // byte_519C6 -> numpad 9
// numpad -
uint8_t gIsLeftKeyPressed = 0; // byte_519C8 -> numpad 4
uint8_t gIsNumpad5Pressed = 0; // byte_519C9 -> numpad 5
uint8_t gIsRightKeyPressed = 0; // byte_519CA -> numpad 6
// numpad +
uint8_t gIsEndKeyPressed = 0; // byte_519CC -> numpad 1
uint8_t gIsDownKeyPressed = 0; // byte_519CD -> numpad 2
uint8_t gIsAvPagKeyPressed = 0; // byte_519CE -> numpad 3
uint8_t gIsInsertKeyPressed = 0; // byte_519CF -> numpad 0
uint8_t gIsDelKeyPressed = 0; // byte_519D0 -> numpad .
// numpad SysReq
// Key 45
// numpad Enter
uint8_t gIsNumpadDividePressed = 0; // byte_519D4 -> numpad /
uint8_t gIsF12KeyPressed = 0; // byte_519D5 -> F12
// These are joystick keys
// uint8_t byte_519F4 = 0; // button A
// uint8_t byte_519F5 = 0; // button B
// uint8_t byte_519F6 = 0; // right
// uint8_t byte_519F7 = 0; // left
// uint8_t byte_519F8 = 0; // down
// uint8_t byte_519F9 = 0; // up

uint8_t isUpButtonPressed(void)
{
    return (gIsUpKeyPressed
            || getGameControllerY() < 0);
}

uint8_t isDownButtonPressed(void)
{
    return (gIsDownKeyPressed
            || getGameControllerY() > 0);
}

uint8_t isLeftButtonPressed(void)
{
    return (gIsLeftKeyPressed
            || getGameControllerX() < 0);
}

uint8_t isRightButtonPressed(void)
{
    return (gIsRightKeyPressed
            || getGameControllerX() > 0);
}

uint8_t isActionButtonPressed(void)
{
    return (gIsSpaceKeyPressed
            || getGameControllerButton(SDL_CONTROLLER_BUTTON_X));
}

uint8_t isMenuBackButtonPressed(void)
{
    return (gIsEscapeKeyPressed
            || getGameControllerButton(SDL_CONTROLLER_BUTTON_BACK));
}

uint8_t isMenuConfirmButtonPressed(void)
{
    return (gIsEnterPressed || getGameControllerConfirmButton());
}

uint8_t isMenuCancelButtonPressed(void)
{
    return (gIsEscapeKeyPressed
            || getGameControllerCancelButton()
            || getGameControllerButton(SDL_CONTROLLER_BUTTON_BACK));
}

uint8_t isExitLevelButtonPressed(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_BACK);
}

uint8_t isStartButtonPressed(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_START);
}

uint8_t isPauseButtonPressed(void)
{
    return (gIsPKeyPressed
            || gIsEscapeKeyPressed
            || getGameControllerButton(SDL_CONTROLLER_BUTTON_START));
}

uint8_t isToggleGamePanelButtonPressed(void)
{
    return (gIsEnterPressed
            || getGameControllerButton(SDL_CONTROLLER_BUTTON_Y));
}

uint8_t isRotateLevelSetAscendingButtonPressed(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

uint8_t isRotateLevelSetDescendingButtonPressed(void)
{
    return getGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
}

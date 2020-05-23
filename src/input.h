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

#include <SDL2/SDL.h>

extern SDL_Scancode keyPressed;

extern uint8_t gIsEscapeKeyPressed; // byte_5197E -> ESC
extern uint8_t gIs1KeyPressed; // byte_5197F -> 1
extern uint8_t gIs2KeyPressed; // byte_51980 -> 2
extern uint8_t gIs3KeyPressed; // byte_51981 -> 3
extern uint8_t gIs4KeyPressed; // byte_51982 -> 4
extern uint8_t gIs5KeyPressed; // byte_51983 -> 5
extern uint8_t gIs6KeyPressed; // byte_51984 -> 6
extern uint8_t gIs7KeyPressed; // byte_51985 -> 7
extern uint8_t gIs8KeyPressed; // byte_51986 -> 8
extern uint8_t gIs9KeyPressed; // byte_51987 -> 9
extern uint8_t gIs0KeyPressed; // byte_51988 -> 0
extern uint8_t gIsMinusKeyPressed; // byte_51989 -> -
extern uint8_t gIsEqualsKeyPressed; // byte_5198A -> =
// backspace
// tab
extern uint8_t gIsQKeyPressed; // byte_5198D -> q
extern uint8_t gIsWKeyPressed; // byte_5198E -> w
// e
extern uint8_t gIsRKeyPressed; // byte_51990 -> r
// t
// y
// u
// i
// o
extern uint8_t gIsPKeyPressed; // byte_51996 -> p
// [
// ]
extern uint8_t gIsEnterPressed; // byte_51999 -> enter
extern uint8_t gIsLeftControlPressed; // byte_5199A -> left control
//a
extern uint8_t gIsSKeyPressed; // byte_5199C -> s
extern uint8_t gIsDKeyPressed; // byte_5199D -> d
//f
//g
extern uint8_t gIsHKeyPressed; // byte_519A0 -> h
extern uint8_t gIsJKeyPressed; // byte_519A1 -> j
//k
extern uint8_t gIsLKeyPressed; // uint32_t dword_519A3 -> l
// ; ' `
extern uint8_t gIsLeftShiftPressed; // word_519A7 -> "left shift"
// slash
extern uint8_t gIsZKeyPressed; // word_519A9 -> z
// x
extern uint8_t gIsCKeyPressed; // word_519AB -> c
// v
extern uint8_t gIsBKeyPressed; // word_519AD -> b
// n
extern uint8_t gIsMKeyPressed; // dword_519AF -> m
// , . /
extern uint8_t gIsRightShiftPressed; // word_519B3 -> "right shift"
// "numpad *"
extern uint8_t gIsLeftAltPressed; // byte_519B5 -> "left alt"
extern uint8_t gIsSpaceKeyPressed; // byte_519B6 -> space
// caps lock
extern uint8_t gIsF1KeyPressed; // byte_519B8 -> F1
extern uint8_t gIsF2KeyPressed; // byte_519B9 -> F2
extern uint8_t gIsF3KeyPressed; // byte_519BA -> F3
extern uint8_t gIsF4KeyPressed; // byte_519BB -> F4
extern uint8_t gIsF5KeyPressed; // byte_519BC -> F5
extern uint8_t gIsF6KeyPressed; // byte_519BD -> F6
extern uint8_t gIsF7KeyPressed; // byte_519BE -> F7
extern uint8_t gIsF8KeyPressed; // byte_519BF -> F8
extern uint8_t gIsF9KeyPressed; // byte_519C0 -> F9
extern uint8_t gIsF10KeyPressed; // byte_519C1 -> F10
extern uint8_t gIsNumLockPressed; // byte_519C2 -> 16B2 -> Num lock
extern uint8_t gIsScrollLockPressed; // byte_519C3 -> Scroll lock
extern uint8_t gIsHomeKeyPressed; // byte_519C4 -> numpad 7
extern uint8_t gIsUpKeyPressed; // byte_519C5 -> 16B5 -> numpad 8 or up arrow
extern uint8_t gIsRePagKeyPressed; // byte_519C6 -> numpad 9
// numpad -
extern uint8_t gIsLeftKeyPressed; // byte_519C8 -> numpad 4
extern uint8_t gIsNumpad5Pressed; // byte_519C9 -> numpad 5
extern uint8_t gIsRightKeyPressed; // byte_519CA -> numpad 6
// numpad +
extern uint8_t gIsEndKeyPressed; // byte_519CC -> numpad 1
extern uint8_t gIsDownKeyPressed; // byte_519CD -> numpad 2
extern uint8_t gIsAvPagKeyPressed; // byte_519CE -> numpad 3
extern uint8_t gIsInsertKeyPressed; // byte_519CF -> numpad 0
extern uint8_t gIsDelKeyPressed; // byte_519D0 -> numpad .
// numpad SysReq
// Key 45
// numpad Enter
extern uint8_t gIsNumpadDividePressed; // byte_519D4 -> numpad /
extern uint8_t gIsF12KeyPressed; // byte_519D5 -> F12
// These are joystick keys
// extern uint8_t byte_519F4; // button A
// extern uint8_t byte_519F5; // button B
// extern uint8_t byte_519F6; // right
// extern uint8_t byte_519F7; // left
// extern uint8_t byte_519F8; // down
// extern uint8_t byte_519F9; // up

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

uint8_t isRotateLevelSetAscendingButtonPressed(void);
uint8_t isRotateLevelSetDescendingButtonPressed(void);

#endif /* input_h */

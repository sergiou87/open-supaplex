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

#include "../keyboard.h"

#include <SDL2/SDL.h>

#include "../system.h"

SDL_Scancode gKeyPressed = 0;

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
uint8_t gIsXKeyPressed = 0; // word_519AA -> x
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
uint8_t gIsNumpadMinusPressed = 0; // byte_519C7 -> numpad -
uint8_t gIsLeftKeyPressed = 0; // byte_519C8 -> numpad 4
uint8_t gIsNumpad5Pressed = 0; // byte_519C9 -> numpad 5
uint8_t gIsRightKeyPressed = 0; // byte_519CA -> numpad 6
uint8_t gIsNumpadPlusPressed = 0; // byte_519CB -> numpad +
uint8_t gIsEndKeyPressed = 0; // byte_519CC -> numpad 1
uint8_t gIsDownKeyPressed = 0; // byte_519CD -> numpad 2
uint8_t gIsAvPagKeyPressed = 0; // byte_519CE -> numpad 3
uint8_t gIsInsertKeyPressed = 0; // byte_519CF -> numpad 0
uint8_t gIsDelKeyPressed = 0; // byte_519D0 -> numpad .
// numpad SysReq
// Key 45
// numpad Enter
uint8_t gIsNumpadDividePressed = 0; // byte_519D4 -> numpad /
uint8_t gIsNumpadMultiplyPressed = 0;
uint8_t gIsF12KeyPressed = 0; // byte_519D5 -> F12
// These are joystick keys
// uint8_t byte_519F4 = 0; // button A
// uint8_t byte_519F5 = 0; // button B
// uint8_t byte_519F6 = 0; // right
// uint8_t byte_519F7 = 0; // left
// uint8_t byte_519F8 = 0; // down
// uint8_t byte_519F9 = 0; // up

void updateKeyboardState()
{
    handleSystemEvents();

    int numberOfKeys = 0;
    const Uint8 *keys = SDL_GetKeyboardState(&numberOfKeys);

    gKeyPressed = SDL_SCANCODE_UNKNOWN;

    for (SDL_Scancode scancode = 0; (int)scancode < numberOfKeys; ++scancode)
    {
        if (keys[scancode])
        {
            gKeyPressed = scancode;
            break;
        }
    }

    gIsEscapeKeyPressed = keys[SDL_SCANCODE_ESCAPE];
    gIsSpaceKeyPressed = keys[SDL_SCANCODE_SPACE];
    gIsUpKeyPressed = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_KP_8];
    gIsDownKeyPressed = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_KP_2];
    gIsLeftKeyPressed = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_KP_4];
    gIsRightKeyPressed = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_KP_6];
    gIsInsertKeyPressed = keys[SDL_SCANCODE_INSERT] || keys[SDL_SCANCODE_KP_0];
    gIsEndKeyPressed = keys[SDL_SCANCODE_END] || keys[SDL_SCANCODE_KP_1];
    gIsAvPagKeyPressed = keys[SDL_SCANCODE_PAGEDOWN] || keys[SDL_SCANCODE_KP_3];
    gIsNumpad5Pressed = keys[SDL_SCANCODE_KP_5];
    gIsHomeKeyPressed = keys[SDL_SCANCODE_HOME] || keys[SDL_SCANCODE_KP_7];
    gIsRePagKeyPressed = keys[SDL_SCANCODE_PAGEUP] || keys[SDL_SCANCODE_KP_9];
    gIsDelKeyPressed = keys[SDL_SCANCODE_DELETE] || keys[SDL_SCANCODE_KP_PERIOD];
    gIsNumpadDividePressed = keys[SDL_SCANCODE_KP_DIVIDE];
    gIsNumpadMultiplyPressed = keys[SDL_SCANCODE_KP_MULTIPLY];
    gIsNumpadPlusPressed = keys[SDL_SCANCODE_KP_PLUS];
    gIsNumpadMinusPressed = keys[SDL_SCANCODE_KP_MINUS];
    gIsNumLockPressed = keys[SDL_SCANCODE_NUMLOCKCLEAR];
    gIsScrollLockPressed = keys[SDL_SCANCODE_SCROLLLOCK];
    gIsLeftAltPressed = keys[SDL_SCANCODE_LALT];
    gIsLeftShiftPressed = keys[SDL_SCANCODE_LSHIFT];
    gIsRightShiftPressed = keys[SDL_SCANCODE_RSHIFT];
    gIsEnterPressed = keys[SDL_SCANCODE_RETURN];
    gIsLeftControlPressed = keys[SDL_SCANCODE_LCTRL];
    gIs1KeyPressed = keys[SDL_SCANCODE_1];
    gIs2KeyPressed = keys[SDL_SCANCODE_2];
    gIs3KeyPressed = keys[SDL_SCANCODE_3];
    gIs4KeyPressed = keys[SDL_SCANCODE_4];
    gIs5KeyPressed = keys[SDL_SCANCODE_5];
    gIs6KeyPressed = keys[SDL_SCANCODE_6];
    gIs7KeyPressed = keys[SDL_SCANCODE_7];
    gIs8KeyPressed = keys[SDL_SCANCODE_8];
    gIs9KeyPressed = keys[SDL_SCANCODE_9];
    gIs0KeyPressed = keys[SDL_SCANCODE_0];
    gIsMinusKeyPressed = keys[SDL_SCANCODE_MINUS];
    gIsEqualsKeyPressed = keys[SDL_SCANCODE_EQUALS];
    gIsQKeyPressed = keys[SDL_SCANCODE_Q];
    gIsWKeyPressed = keys[SDL_SCANCODE_W];
    gIsRKeyPressed = keys[SDL_SCANCODE_R];
    gIsPKeyPressed = keys[SDL_SCANCODE_P];
    gIsSKeyPressed = keys[SDL_SCANCODE_S];
    gIsDKeyPressed = keys[SDL_SCANCODE_D];
    gIsHKeyPressed = keys[SDL_SCANCODE_H];
    gIsJKeyPressed = keys[SDL_SCANCODE_J];
    gIsLKeyPressed = keys[SDL_SCANCODE_L];
    gIsZKeyPressed = keys[SDL_SCANCODE_Z];
    gIsXKeyPressed = keys[SDL_SCANCODE_X];
    gIsCKeyPressed = keys[SDL_SCANCODE_C];
    gIsBKeyPressed = keys[SDL_SCANCODE_B];
    gIsMKeyPressed = keys[SDL_SCANCODE_M];
    gIsF1KeyPressed = keys[SDL_SCANCODE_F1];
    gIsF2KeyPressed = keys[SDL_SCANCODE_F2];
    gIsF3KeyPressed = keys[SDL_SCANCODE_F3];
    gIsF4KeyPressed = keys[SDL_SCANCODE_F4];
    gIsF5KeyPressed = keys[SDL_SCANCODE_F5];
    gIsF6KeyPressed = keys[SDL_SCANCODE_F6];
    gIsF7KeyPressed = keys[SDL_SCANCODE_F7];
    gIsF8KeyPressed = keys[SDL_SCANCODE_F8];
    gIsF9KeyPressed = keys[SDL_SCANCODE_F9];
    gIsF10KeyPressed = keys[SDL_SCANCODE_F10];
    gIsF12KeyPressed = keys[SDL_SCANCODE_F12];
}

uint8_t isAnyKeyPressed()
{
    return (gKeyPressed != SDL_SCANCODE_UNKNOWN);
}

#define SDL_KEYCODE_TO_CHAR_CASE(keycode, char) case keycode: return char

char characterForSDLScancode(SDL_Scancode /*scancode*/)
{
    switch (SDL_GetKeyFromScancode(gKeyPressed))
    {
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_0, '0');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_1, '1');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_2, '2');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_3, '3');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_4, '4');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_5, '5');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_6, '6');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_7, '7');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_8, '8');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_9, '9');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_a, 'A');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_b, 'B');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_c, 'C');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_d, 'D');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_e, 'E');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_f, 'F');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_g, 'G');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_h, 'H');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_i, 'I');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_j, 'J');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_k, 'K');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_l, 'L');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_m, 'M');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_n, 'N');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_o, 'O');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_p, 'P');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_q, 'Q');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_r, 'R');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_s, 'S');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_t, 'T');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_u, 'U');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_v, 'V');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_w, 'W');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_x, 'X');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_y, 'Y');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_z, 'Z');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_SPACE, ' ');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_MINUS, '-');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_BACKSPACE, '\b');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_RETURN, '\n');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_RETURN2, '\n');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_KP_SPACE, ' ');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_KP_MINUS, '-');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_KP_BACKSPACE, '\b');
        SDL_KEYCODE_TO_CHAR_CASE(SDLK_KP_ENTER, '\n');

        default:
            return '\0';
    }
}

char characterForLastKeyPressed()
{
    return characterForSDLScancode(gKeyPressed);
}

uint8_t areAnyF1ToF10KeysPressed(void)
{
    return (gKeyPressed >= SDL_SCANCODE_F1
            && gKeyPressed <= SDL_SCANCODE_F10);
}


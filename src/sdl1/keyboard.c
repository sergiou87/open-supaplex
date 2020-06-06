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

#include <SDL/SDL.h>

#include "../system.h"

SDLKey gKeyPressed = SDLK_UNKNOWN;

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

    // The PS3 port of SDL is not 1.2 nor 2... but 1.3, which seems to have some SDL2-like APIs while other are still SDL 1.2
    // The keyboard API seems to be SDL2 based, so nothing from here will build. However, it's not needed anyway (unless someone wants to
    // play with keyboard instead of a gamepad).
    //
#ifndef __PSL1GHT__
    const Uint8 *keys = SDL_GetKeyState(NULL);

    gKeyPressed = SDLK_UNKNOWN;

    for (SDLKey scancode = 0; scancode < SDLK_LAST; ++scancode)
    {
        if (keys[scancode])
        {
            gKeyPressed = scancode;
            break;
        }
    }

    gIsEscapeKeyPressed = keys[SDLK_ESCAPE];
    gIsSpaceKeyPressed = keys[SDLK_SPACE];
    gIsUpKeyPressed = keys[SDLK_UP] || keys[SDLK_KP8];
    gIsDownKeyPressed = keys[SDLK_DOWN] || keys[SDLK_KP2];
    gIsLeftKeyPressed = keys[SDLK_LEFT] || keys[SDLK_KP4];
    gIsRightKeyPressed = keys[SDLK_RIGHT] || keys[SDLK_KP6];
    gIsInsertKeyPressed = keys[SDLK_INSERT] || keys[SDLK_KP0];
    gIsEndKeyPressed = keys[SDLK_END] || keys[SDLK_KP1];
    gIsAvPagKeyPressed = keys[SDLK_PAGEDOWN] || keys[SDLK_KP3];
    gIsNumpad5Pressed = keys[SDLK_KP5];
    gIsHomeKeyPressed = keys[SDLK_HOME] || keys[SDLK_KP7];
    gIsRePagKeyPressed = keys[SDLK_PAGEUP] || keys[SDLK_KP9];
    gIsDelKeyPressed = keys[SDLK_DELETE] || keys[SDLK_KP_PERIOD];
    gIsNumpadDividePressed = keys[SDLK_KP_DIVIDE];
    gIsNumpadMultiplyPressed = keys[SDLK_KP_MULTIPLY];
    gIsNumpadPlusPressed = keys[SDLK_KP_PLUS];
    gIsNumpadMinusPressed = keys[SDLK_KP_MINUS];
    gIsNumLockPressed = keys[SDLK_NUMLOCK];
    gIsScrollLockPressed = keys[SDLK_SCROLLOCK];
    gIsLeftAltPressed = keys[SDLK_LALT];
    gIsLeftShiftPressed = keys[SDLK_LSHIFT];
    gIsRightShiftPressed = keys[SDLK_RSHIFT];
    gIsEnterPressed = keys[SDLK_RETURN];
    gIsLeftControlPressed = keys[SDLK_LCTRL];
    gIs1KeyPressed = keys[SDLK_1];
    gIs2KeyPressed = keys[SDLK_2];
    gIs3KeyPressed = keys[SDLK_3];
    gIs4KeyPressed = keys[SDLK_4];
    gIs5KeyPressed = keys[SDLK_5];
    gIs6KeyPressed = keys[SDLK_6];
    gIs7KeyPressed = keys[SDLK_7];
    gIs8KeyPressed = keys[SDLK_8];
    gIs9KeyPressed = keys[SDLK_9];
    gIs0KeyPressed = keys[SDLK_0];
    gIsMinusKeyPressed = keys[SDLK_MINUS];
    gIsEqualsKeyPressed = keys[SDLK_EQUALS];
    gIsQKeyPressed = keys[SDLK_q];
    gIsWKeyPressed = keys[SDLK_w];
    gIsRKeyPressed = keys[SDLK_r];
    gIsPKeyPressed = keys[SDLK_p];
    gIsSKeyPressed = keys[SDLK_s];
    gIsDKeyPressed = keys[SDLK_d];
    gIsHKeyPressed = keys[SDLK_h];
    gIsJKeyPressed = keys[SDLK_j];
    gIsLKeyPressed = keys[SDLK_l];
    gIsZKeyPressed = keys[SDLK_z];
    gIsXKeyPressed = keys[SDLK_x];
    gIsCKeyPressed = keys[SDLK_c];
    gIsBKeyPressed = keys[SDLK_b];
    gIsMKeyPressed = keys[SDLK_m];
    gIsF1KeyPressed = keys[SDLK_F1];
    gIsF2KeyPressed = keys[SDLK_F2];
    gIsF3KeyPressed = keys[SDLK_F3];
    gIsF4KeyPressed = keys[SDLK_F4];
    gIsF5KeyPressed = keys[SDLK_F5];
    gIsF6KeyPressed = keys[SDLK_F6];
    gIsF7KeyPressed = keys[SDLK_F7];
    gIsF8KeyPressed = keys[SDLK_F8];
    gIsF9KeyPressed = keys[SDLK_F9];
    gIsF10KeyPressed = keys[SDLK_F10];
    gIsF12KeyPressed = keys[SDLK_F12];
#endif
}

uint8_t isAnyKeyPressed()
{
    return (gKeyPressed != SDLK_UNKNOWN);
}

#define SDL_SCANCODE_TO_CHAR_CASE(scancode, char) case scancode: return char

char characterForSDLScancode(SDLKey scancode)
{
    // 0x16FA points to what seems to be a map from key code (http://stanislavs.org/helppc/make_codes.html) to ASCII
    //        0B5D:16FA     00 00 31 32 33 34 35 36 37 38 39 30 2D 00 08 00  ..1234567890-...
    //        0B5D:170A     51 57 45 52 54 59 55 49 4F 50 00 00 0A 00 41 53  QWERTYUIOP....AS
    //        0B5D:171A     44 46 47 48 4A 4B 4C 00 00 00 00 00 5A 58 43 56  DFGHJKL.....ZXCV
    //        0B5D:172A     42 4E 4D 00 00 00 00 00 00 20 00 00 00 00 00 00  BNM...... ......
    //        0B5D:173A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:174A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:175A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:176A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:177A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
    //        0B5D:178A     00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................

    switch (scancode)
    {
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_0, '0');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_1, '1');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_2, '2');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_3, '3');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_4, '4');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_5, '5');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_6, '6');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_7, '7');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_8, '8');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_9, '9');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_q, 'Q');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_w, 'W');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_e, 'E');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_r, 'R');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_t, 'T');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_y, 'Y');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_u, 'U');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_i, 'I');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_o, 'O');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_p, 'P');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_a, 'A');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_s, 'S');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_d, 'D');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_f, 'F');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_g, 'G');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_h, 'H');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_j, 'J');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_k, 'K');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_l, 'L');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_z, 'Z');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_x, 'X');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_c, 'C');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_v, 'V');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_b, 'B');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_n, 'N');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_m, 'M');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_SPACE, ' ');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_MINUS, '-');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_BACKSPACE, '\b');
        SDL_SCANCODE_TO_CHAR_CASE(SDLK_RETURN, '\n');

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
    return (gKeyPressed >= SDLK_F1
            && gKeyPressed <= SDLK_F10);
}


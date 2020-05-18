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

#include "../virtualKeyboard.h"

#include <SDL2/SDL.h>
#include <switch.h>

#include "../logging.h"

static uint16_t gCurrentKeyboardMaxLength = 0;

static const char *kAllowedCharacters = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM -";

SwkbdTextCheckResult validateInputText(char *inputText, size_t errorTextMaxLength)
{
    // inputText is used for outputting an error if validation fails.
    // errorTextMaxLength is the size of the inputText when used as a buffer to output errors. From what I've seen it's 15... ¬_¬'

    uint8_t containsInvalidCharacters = 0;
    size_t textLength = strlen(inputText);
    for (size_t idx = 0; idx < textLength; ++idx)
    {
        char uppercaseChar = toupper(inputText[idx]);
        if (strchr(kAllowedCharacters, uppercaseChar) == NULL)
        {
            containsInvalidCharacters = 1;
            break;
        }
    }

    if (containsInvalidCharacters)
    {
        snprintf(inputText, errorTextMaxLength, "Invalid symbol");
        return SwkbdTextCheckResult_Bad;
    }

    if (textLength > gCurrentKeyboardMaxLength)
    {
        snprintf(inputText, errorTextMaxLength, "Too long");
        return SwkbdTextCheckResult_Bad;
    }

    return SwkbdTextCheckResult_OK;
}

uint8_t inputVirtualKeyboardText(const char *title, uint16_t maxLength, char *outText)
{
    uint8_t success = 0;

    SwkbdConfig kbd;
    char tmpoutstr[16] = {0};
    Result rc = swkbdCreate(&kbd, 0);
    spLogInfo("swkbdCreate(): 0x%x\n", rc);

    gCurrentKeyboardMaxLength = maxLength;

    if (R_SUCCEEDED(rc)) 
    {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetGuideText(&kbd, title);
        swkbdConfigSetInitialText(&kbd, outText);

        swkbdConfigSetTextCheckCallback(&kbd, validateInputText); 

        // You can also use swkbdConfigSet*() funcs if you want.

        spLogInfo("Running swkbdShow...\n");
        rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
        spLogInfo("swkbdShow(): 0x%x\n", rc);

        if (R_SUCCEEDED(rc))
        {
            tmpoutstr[maxLength] = 0;
            strncpy(outText, tmpoutstr, maxLength);

            success = 1;
        }
        swkbdClose(&kbd);
    }

    return success;
}

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

uint8_t isRealKeyboardSupported(void)
{
    return 0;
}

uint8_t isVirtualKeyboardSupported(void)
{
    return 1;
}

static const char *kAllowedCharacters = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM -";

// TextCheck callback, this can be removed when not using TextCheck.
SwkbdTextCheckResult validate_text(char *tmp_string, size_t tmp_string_size)
{
    uint8_t containsInvalidCharacters = 0;
    size_t textLength = strlen(tmp_string);
    for (size_t idx = 0; idx < textLength; ++idx)
    {
        char uppercaseChar = toupper(tmp_string[idx]);
        if (strchr(kAllowedCharacters, uppercaseChar) == NULL)
        {
            containsInvalidCharacters = 1;
            break;
        }
    }

    if (containsInvalidCharacters)
    {
        snprintf(tmp_string, tmp_string_size, "Invalid symbol");
        return SwkbdTextCheckResult_Bad;
    }

    if (textLength > gCurrentKeyboardMaxLength)
    {
        snprintf(tmp_string, tmp_string_size, "Too long");
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
    spLog("swkbdCreate(): 0x%x\n", rc);

    gCurrentKeyboardMaxLength = maxLength;

    if (R_SUCCEEDED(rc)) 
    {
        // Select a Preset to use, if any.
        swkbdConfigMakePresetDefault(&kbd);
        //swkbdConfigMakePresetPassword(&kbd);
        //swkbdConfigMakePresetUserName(&kbd);
        //swkbdConfigMakePresetDownloadCode(&kbd);

        // Optional, set any text if you want (see swkbd.h).
        //swkbdConfigSetOkButtonText(&kbd, "Submit");
        //swkbdConfigSetLeftOptionalSymbolKey(&kbd, "a");
        //swkbdConfigSetRightOptionalSymbolKey(&kbd, "b");
        //swkbdConfigSetHeaderText(&kbd, "Header");
        //swkbdConfigSetSubText(&kbd, "Sub");
        swkbdConfigSetGuideText(&kbd, title);

        swkbdConfigSetTextCheckCallback(&kbd, validate_text);//Optional, can be removed if not using TextCheck.

        // Set the initial string if you want.
        swkbdConfigSetInitialText(&kbd, outText);

        // You can also use swkbdConfigSet*() funcs if you want.

        spLog("Running swkbdShow...\n");
        rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
        spLog("swkbdShow(): 0x%x\n", rc);

        if (R_SUCCEEDED(rc))
        {
            spLog("out str: %s\n", tmpoutstr);

            tmpoutstr[maxLength] = 0;
            strncpy(outText, tmpoutstr, maxLength);

            success = 1;
        }
        swkbdClose(&kbd);
    }

    return success;
}

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

#include <3ds.h>
#include <ctype.h>
#include <string.h>

#include "../logging.h"

static uint16_t gCurrentKeyboardMaxLength = 0;

static const char *kAllowedCharacters = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM -";

static SwkbdCallbackResult validateInputText(void *user, const char **ppMessage, const char *inputText, size_t textLength)
{
    // inputText is used for outputting an error if validation fails.
    // errorTextMaxLength is the size of the inputText when used as a buffer to output errors. From what I've seen it's 15... ¬_¬'

    uint8_t containsInvalidCharacters = 0;
    //size_t textLength = strlen(inputText);
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
		*ppMessage = "Invalid symbol";
        return SWKBD_CALLBACK_CONTINUE;
    }

    if (textLength > gCurrentKeyboardMaxLength)
    {
		*ppMessage = "Too long";
        return SWKBD_CALLBACK_CONTINUE;
    }

    return SWKBD_CALLBACK_OK;
}

uint8_t inputVirtualKeyboardText(const char *title, uint16_t maxLength, char *outText)
{
    uint8_t success = 0;

	SwkbdState swkbd;
	char mybuf[60];

	swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, maxLength); // TODO: use maxLength?
	swkbdSetInitialText(&swkbd, mybuf);
	swkbdSetHintText(&swkbd, title);
	swkbdSetFilterCallback(&swkbd, validateInputText, NULL);

    gCurrentKeyboardMaxLength = maxLength;

	spLogInfo("Running swkbdInputText...\n");
	SwkbdButton button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
	spLogInfo("Button: %d\n", button);

	if (button == SWKBD_BUTTON_CONFIRM)
	{
		mybuf[maxLength] = 0;
		strncpy(outText, mybuf, maxLength);
		success = 1;
	}

	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();

	gspWaitForVBlank();

    return success;
}

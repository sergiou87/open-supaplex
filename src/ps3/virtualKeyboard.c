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

#include <sys/memory.h>
#include <sysutil/osk.h>
#include <sysutil/sysutil.h>

#include "../logging.h"
#include "../video.h"

static void utf16_to_utf8(const uint16_t *src, uint8_t *dst)
{
    int i;
    for (i = 0; src[i]; i++)
    {
        if ((src[i] & 0xFF80) == 0)
        {
            *(dst++) = src[i] & 0xFF;
        }
        else if((src[i] & 0xF800) == 0)
        {
            *(dst++) = ((src[i] >> 6) & 0xFF) | 0xC0;
            *(dst++) = (src[i] & 0x3F) | 0x80;
        }
        else if((src[i] & 0xFC00) == 0xD800 && (src[i + 1] & 0xFC00) == 0xDC00)
        {
            *(dst++) = (((src[i] + 64) >> 8) & 0x3) | 0xF0;
            *(dst++) = (((src[i] >> 2) + 16) & 0x3F) | 0x80;
            *(dst++) = ((src[i] >> 4) & 0x30) | 0x80 | ((src[i + 1] << 2) & 0xF);
            *(dst++) = (src[i + 1] & 0x3F) | 0x80;
            i += 1;
        }
        else
        {
            *(dst++) = ((src[i] >> 12) & 0xF) | 0xE0;
            *(dst++) = ((src[i] >> 6) & 0x3F) | 0x80;
            *(dst++) = (src[i] & 0x3F) | 0x80;
        }
    }

    *dst = '\0';
}

static void utf8_to_utf16(const uint8_t *src, uint16_t *dst)
{
    int i;
    for (i = 0; src[i];)
    {
        if ((src[i] & 0xE0) == 0xE0)
        {
            *(dst++) = ((src[i] & 0x0F) << 12) | ((src[i + 1] & 0x3F) << 6) | (src[i + 2] & 0x3F);
            i += 3;
        }
        else if ((src[i] & 0xC0) == 0xC0)
        {
            *(dst++) = ((src[i] & 0x1F) << 6) | (src[i + 1] & 0x3F);
            i += 2;
        }
        else
        {
            *(dst++) = src[i];
            i += 1;
        }
    }

    *dst = '\0';
}

// This should be defined in PSL1GHT but they aren't... yet
#define SYSUTIL_OSK_INPUT_ENTERED 0x0505
#define SYSUTIL_OSK_INPUT_CANCELED 0x0506
#define SYSUTIL_OSK_INPUT_DEVICE_CHANGED 0x0507
#define SYSUTIL_OSK_DISPLAY_CHANGED 0x0508

static uint8_t gIsRunningOSK = 0;
static oskInputFieldInfo gInputFieldInfo;
static oskParam gParameters;
static oskCallbackReturnParam gOutputParam;

static void keyboardCallback(u64 status, u64 param, void *usrdata)
{
	switch (status)
	{
		case SYSUTIL_OSK_LOADED:
			spLogInfo("OSK loaded");
			break;
		case SYSUTIL_OSK_INPUT_CANCELED:
			spLogInfo("OSK input canceled");
			oskAbort();
			// fall-through
		case SYSUTIL_OSK_DONE:
			if (status == SYSUTIL_OSK_DONE)
			{
				spLogInfo("OSK done");
			}
			oskUnloadAsync(&gOutputParam);

			if (gOutputParam.res == OSK_OK)
			{
				spLogInfo("OSK result OK");
			}
			else
			{
				spLogInfo("OKS result: %d", gOutputParam.res);
			}

			break;
		case SYSUTIL_OSK_UNLOADED:
			spLogInfo("OSK unloaded");
			gIsRunningOSK = 0;
			break;
	}
}

#define SCE_IME_DIALOG_MAX_TITLE_LENGTH 50
#define SCE_IME_DIALOG_MAX_TEXT_LENGTH 50
#define SCE_IME_DIALOG_MAX_TEXT_LENGTH 50
#define SCE_IME_DIALOG_MAX_TEXT_LENGTH 50

uint8_t inputVirtualKeyboardText(const char *title, uint16_t maxLength, char *outText)
{
    static uint16_t ime_title_utf16[SCE_IME_DIALOG_MAX_TITLE_LENGTH];
    static uint16_t ime_input_text_utf16[SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1];
    static uint16_t ime_initial_text_utf16[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
    static uint8_t ime_input_text_utf8[SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1];

    // Convert UTF8 to UTF16
    memset(ime_title_utf16, 0, sizeof(ime_title_utf16));
    memset(ime_initial_text_utf16, 0, sizeof(ime_initial_text_utf16));
    utf8_to_utf16((uint8_t *)title, ime_title_utf16);
    utf8_to_utf16((uint8_t *)outText, ime_initial_text_utf16);

	gInputFieldInfo.message = ime_title_utf16;
	gInputFieldInfo.startText = ime_initial_text_utf16;
	gInputFieldInfo.maxLength = maxLength;

	gParameters.allowedPanels = OSK_PANEL_TYPE_DEFAULT;
	gParameters.firstViewPanel = OSK_PANEL_TYPE_DEFAULT;
	gParameters.controlPoint = (oskPoint) { 0, 0 };
	gParameters.prohibitFlags = OSK_PROHIBIT_RETURN;
	
	gOutputParam.res = OSK_OK;
	gOutputParam.len = maxLength;
	gOutputParam.str = ime_input_text_utf16;
	
	s32 res = 0;

	sys_mem_container_t containerid;
	res = sysMemContainerCreate(&containerid, 4 * 1024 * 1024);
	if (res != 0)
	{
        spLogInfo("Error sysMemContainerCreate: %08x", res);
		return 0;
	}

	oskSetInitialInputDevice(OSK_DEVICE_PAD);
	oskSetKeyLayoutOption(OSK_FULLKEY_PANEL);
	oskSetLayoutMode(OSK_LAYOUTMODE_HORIZONTAL_ALIGN_CENTER | OSK_LAYOUTMODE_VERTICAL_ALIGN_CENTER);

	res = sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT3, keyboardCallback, NULL);
	if (res != 0)
	{
        spLogInfo("Error sysUtilRegisterCallback: %08x", res);
		sysMemContainerDestroy(containerid);
		return 0;
	}

	res = oskLoadAsync(containerid, &gParameters, &gInputFieldInfo);
	if (res != 0)
	{
        spLogInfo("Error oskLoadAsync: %08x", res);
		sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT3);
		sysMemContainerDestroy(containerid);
		return 0;
	}

	spLogInfo("Running OSK");

	gIsRunningOSK = 1;

	while (gIsRunningOSK)
	{
		render();
		present();
	    sysUtilCheckCallback();
	}

	sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT3);
	sysMemContainerDestroy(containerid);

	if (gOutputParam.res != OSK_OK)
	{
        spLogInfo("Keyboard cancelled");
		return 0;
	}

	// Convert UTF16 to UTF8
	utf16_to_utf8(gOutputParam.str, ime_input_text_utf8);
	ime_input_text_utf8[maxLength] = 0; // Make sure we don't copy more than maxLength bytes
	strcpy(outText, (char *)ime_input_text_utf8);

	return 1;
}

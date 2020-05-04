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

#include "virtualKeyboard.h"

#include <SDL2/SDL.h>

#ifdef __vita__
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/gxt.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/sysmem.h>
#endif

#include "logging.h"

#if defined(__vita__)

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

uint8_t isRealKeyboardSupported(void)
{
    return 0;
}

uint8_t isVirtualKeyboardSupported(void)
{
    return 1;
}

#define ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))
#define DISPLAY_WIDTH            960
#define DISPLAY_HEIGHT            544
#define DISPLAY_STRIDE_IN_PIXELS    1024
#define DISPLAY_BUFFER_COUNT        2
#define DISPLAY_MAX_PENDING_SWAPS    1

typedef struct
{
    void* data;
    SceGxmSyncObject* sync;
    SceGxmColorSurface surf;
    SceUID uid;
} displayBuffer;

unsigned int backBufferIndex = 0;
unsigned int frontBufferIndex = 0;
/* could be converted as struct displayBuffer[] */
displayBuffer dbuf[DISPLAY_BUFFER_COUNT];

void *dram_alloc(unsigned int size, SceUID *uid)
{
    void *mem;
    *uid = sceKernelAllocMemBlock("gpu_mem", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, ALIGN(size,256*1024), NULL);
    sceKernelGetMemBlockBase(*uid, &mem);
    sceGxmMapMemory(mem, ALIGN(size,256*1024), SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE);
    return mem;
}

void gxm_vsync_cb(const void *callback_data)
{
    sceDisplaySetFrameBuf(&(SceDisplayFrameBuf){sizeof(SceDisplayFrameBuf),
        *((void **)callback_data),DISPLAY_STRIDE_IN_PIXELS, 0,
        DISPLAY_WIDTH,DISPLAY_HEIGHT}, SCE_DISPLAY_SETBUF_NEXTFRAME);
}

void gxm_init()
{
    sceGxmInitialize(&(SceGxmInitializeParams){0,DISPLAY_MAX_PENDING_SWAPS,gxm_vsync_cb,sizeof(void *),SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE});
    unsigned int i;
    for (i = 0; i < DISPLAY_BUFFER_COUNT; i++)
    {
        dbuf[i].data = dram_alloc(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, &dbuf[i].uid);
        sceGxmColorSurfaceInit(&dbuf[i].surf,SCE_GXM_COLOR_FORMAT_A8B8G8R8,SCE_GXM_COLOR_SURFACE_LINEAR,SCE_GXM_COLOR_SURFACE_SCALE_NONE,SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS,dbuf[i].data);
        sceGxmSyncObjectCreate(&dbuf[i].sync);
    }
}

void gxm_swap()
{
    sceGxmPadHeartbeat(&dbuf[backBufferIndex].surf, dbuf[backBufferIndex].sync);
    sceGxmDisplayQueueAddEntry(dbuf[frontBufferIndex].sync, dbuf[backBufferIndex].sync, &dbuf[backBufferIndex].data);
    frontBufferIndex = backBufferIndex;
    backBufferIndex = (backBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
}

void gxm_term()
{
    sceGxmTerminate();

    for (int i=0; i<DISPLAY_BUFFER_COUNT; ++i)
        sceKernelFreeMemBlock(dbuf[i].uid);
}

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

    sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
    sceCommonDialogSetConfigParam(&(SceCommonDialogConfigParam){});

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);

    param.supportedLanguages = 0x0001FFFF;
    param.languagesForced = SCE_TRUE;
    param.type = SCE_IME_TYPE_DEFAULT;
    param.option = 0;
    param.textBoxMode = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
    param.title = ime_title_utf16;
    param.maxTextLength = maxLength;
    param.initialText = ime_initial_text_utf16;
    param.inputTextBuffer = ime_input_text_utf16;

    int res = sceImeDialogInit(&param);
    if (res != SCE_OK)
    {
        spLog("Error creating IME dialog for virtual keyboard: %08x", res);
        return 0;
    }

    gxm_init();

    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) { };

        SceCommonDialogStatus status = sceImeDialogGetStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED)
        {
            SceImeDialogResult result;
            memset(&result, 0, sizeof(SceImeDialogResult));
            sceImeDialogGetResult(&result);

            sceImeDialogTerm();

            if (result.button == SCE_IME_DIALOG_BUTTON_ENTER)
            {
                // Convert UTF16 to UTF8
                utf16_to_utf8(ime_input_text_utf16, ime_input_text_utf8);
                strcpy(outText, ime_input_text_utf8);
                return 1;
            }

            break;
        }

        sceCommonDialogUpdate(&(SceCommonDialogUpdateParam){{
            NULL,dbuf[backBufferIndex].data,0,0,
            DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS},
            dbuf[backBufferIndex].sync});

        gxm_swap();
        sceDisplayWaitVblankStart();
    }

    gxm_term();

    return 0;
}
#else
uint8_t isRealKeyboardSupported(void)
{
    return 1;
}

uint8_t isVirtualKeyboardSupported(void)
{
    return 0;
}

uint8_t inputVirtualKeyboardText(const char *title, uint16_t maxLength, char *outText)
{
    return 0;
}
#endif

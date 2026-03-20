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

#include "conditionals.h"
#include "platformCapabilities.h"

#ifdef __SWITCH__
#include <switch.h>
#endif

uint8_t supportsRealKeyboard(void)
{
    return PLATFORM_SUPPORTS_REAL_KEYBOARD;
}

uint8_t supportsVirtualKeyboard(void)
{
#if PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD
    return 1;
#elif PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD
    // If app is ran in applet mode, SwKbd will not work
    AppletType at = appletGetAppletType();
    return at == AppletType_Application || at == AppletType_SystemApplication;
#else
    return 0;
#endif
}

uint8_t supportsSPFileDemoPlayback(void)
{
    return PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK;
}

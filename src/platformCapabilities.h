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

#ifndef platformCapabilities_h
#define platformCapabilities_h

#if defined(__vita__) || defined(__SWITCH__) || defined(__PSP__) || defined(__NDS__) || defined(_3DS) || defined(__PSL1GHT__) || defined(__WII__) || defined(__WIIU__) || defined(__PS2__)
#define PLATFORM_SUPPORTS_REAL_KEYBOARD 0
#define PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK 0
#else
#define PLATFORM_SUPPORTS_REAL_KEYBOARD 1
#define PLATFORM_SUPPORTS_SP_FILE_DEMO_PLAYBACK 1
#endif

#if defined(__vita__) || defined(_3DS) || defined(__PSL1GHT__)
#define PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD 1
#else
#define PLATFORM_HAS_NATIVE_VIRTUAL_KEYBOARD 0
#endif

#if defined(__SWITCH__)
#define PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD 1
#define PLATFORM_NEEDS_SWITCH_SOCKET_LOGGING 1
#else
#define PLATFORM_HAS_RUNTIME_VIRTUAL_KEYBOARD 0
#define PLATFORM_NEEDS_SWITCH_SOCKET_LOGGING 0
#endif

#if (defined(__vita__) || defined(__PSL1GHT__)) && DEBUG
#define PLATFORM_USES_DEBUGNET_LOGGING 1
#else
#define PLATFORM_USES_DEBUGNET_LOGGING 0
#endif

#if defined(__NDS__)
#define PLATFORM_NEEDS_NITRO_FS_INIT 1
#else
#define PLATFORM_NEEDS_NITRO_FS_INIT 0
#endif

#if defined(_3DS)
#define PLATFORM_NEEDS_ROMFS_LIFECYCLE 1
#define PLATFORM_SUPPORTS_3DS_SPEEDUP 1
#else
#define PLATFORM_NEEDS_ROMFS_LIFECYCLE 0
#define PLATFORM_SUPPORTS_3DS_SPEEDUP 0
#endif

#if defined(__WII__)
#define PLATFORM_SDL1_CONTROLLER_SIDEWAYS_LAYOUT 1
#define PLATFORM_SDL1_CONTROLLER_HAS_WII_CONFIRM_CANCEL_ALIASES 1
#else
#define PLATFORM_SDL1_CONTROLLER_SIDEWAYS_LAYOUT 0
#define PLATFORM_SDL1_CONTROLLER_HAS_WII_CONFIRM_CANCEL_ALIASES 0
#endif

#if defined(_3DS)
#define PLATFORM_SDL1_CONFIRM_IS_B 1
#else
#define PLATFORM_SDL1_CONFIRM_IS_B 0
#endif

#if defined(__SWITCH__) || defined(__WIIU__)
#define PLATFORM_SDL2_CONFIRM_IS_B 1
#else
#define PLATFORM_SDL2_CONFIRM_IS_B 0
#endif

#if defined(FILE_DATA_PATH)
#define PLATFORM_PATH_STR_HELPER(x) #x
#define PLATFORM_PATH_STR(x) PLATFORM_PATH_STR_HELPER(x)
#endif

#if defined(FILE_DATA_PATH)
#define PLATFORM_AUDIO_BASE_PATH PLATFORM_PATH_STR(FILE_DATA_PATH) "/audio"
#elif defined(__vita__)
#define PLATFORM_AUDIO_BASE_PATH "app0:/audio"
#elif defined(_3DS)
#define PLATFORM_AUDIO_BASE_PATH "romfs:/audio"
#elif defined(__PSL1GHT__)
#define PLATFORM_AUDIO_BASE_PATH "/dev_hdd0/game/" PS3APPID "/USRDIR/audio"
#elif defined(__riscos__)
#define PLATFORM_AUDIO_BASE_PATH "/<OpenSupaplex$Dir>/audio"
#elif defined(__WII__)
#define PLATFORM_AUDIO_BASE_PATH "/apps/OpenSupaplex/audio"
#elif defined(__WIIU__)
#define PLATFORM_AUDIO_BASE_PATH "fs:/vol/external01/wiiu/apps/OpenSupaplex/audio"
#else
#define PLATFORM_AUDIO_BASE_PATH "audio"
#endif

#if defined(__SWITCH__) || defined(__WIIU__) || defined(__WII__) || defined(_3DS)
#define PLATFORM_AUDIO_BUFFER_SIZE 1024
#else
#define PLATFORM_AUDIO_BUFFER_SIZE 512
#endif

#if defined(_3DS)
#define PLATFORM_AUDIO_SAMPLE_RATE 22050
#define PLATFORM_AUDIO_CHANNEL_COUNT 1
#else
#define PLATFORM_AUDIO_SAMPLE_RATE 44100
#define PLATFORM_AUDIO_CHANNEL_COUNT 2
#endif

#if defined(__WII__) || defined(__WIIU__)
#define PLATFORM_NEEDS_MIX_INIT_MOD 0
#else
#define PLATFORM_NEEDS_MIX_INIT_MOD 1
#endif

#if defined(__PSP__)
#define PLATFORM_SDL2_WINDOW_WIDTH 480
#define PLATFORM_SDL2_WINDOW_HEIGHT 272
#define PLATFORM_SDL2_WINDOW_STARTS_FULLSCREEN 1
#define PLATFORM_SDL2_TEXTURE_FORMAT SDL_PIXELFORMAT_ABGR32
#elif defined(__vita__)
#define PLATFORM_SDL2_WINDOW_WIDTH 960
#define PLATFORM_SDL2_WINDOW_HEIGHT 544
#define PLATFORM_SDL2_WINDOW_STARTS_FULLSCREEN 1
#define PLATFORM_SDL2_TEXTURE_FORMAT SDL_PIXELFORMAT_RGB24
#elif defined(__PSL1GHT__) || defined(__WIIU__)
#define PLATFORM_SDL2_WINDOW_WIDTH 1280
#define PLATFORM_SDL2_WINDOW_HEIGHT 720
#define PLATFORM_SDL2_WINDOW_STARTS_FULLSCREEN 1
#define PLATFORM_SDL2_TEXTURE_FORMAT SDL_PIXELFORMAT_RGB24
#else
#define PLATFORM_SDL2_WINDOW_WIDTH (kScreenWidth * 4)
#define PLATFORM_SDL2_WINDOW_HEIGHT (kScreenHeight * 4)
#if defined(__SWITCH__) || defined(__PS2__)
#define PLATFORM_SDL2_WINDOW_STARTS_FULLSCREEN 1
#else
#define PLATFORM_SDL2_WINDOW_STARTS_FULLSCREEN 0
#endif
#define PLATFORM_SDL2_TEXTURE_FORMAT SDL_PIXELFORMAT_RGB24
#endif

#if defined(__PSP__)
#define PLATFORM_SDL1_WINDOW_WIDTH 480
#define PLATFORM_SDL1_WINDOW_HEIGHT 272
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE)
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 1
#elif defined(_3DS)
#define PLATFORM_SDL1_WINDOW_WIDTH 400
#define PLATFORM_SDL1_WINDOW_HEIGHT 240
#if DEBUG
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_CONSOLEBOTTOM)
#else
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE)
#endif
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 0
#elif defined(__NDS__)
#define PLATFORM_SDL1_WINDOW_WIDTH kScreenWidth
#define PLATFORM_SDL1_WINDOW_HEIGHT kScreenHeight
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_BOTTOMSCR)
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 0
#elif defined(__WII__)
#define PLATFORM_SDL1_WINDOW_WIDTH 640
#define PLATFORM_SDL1_WINDOW_HEIGHT 480
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE)
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 0
#elif defined(__riscos__)
#define PLATFORM_SDL1_WINDOW_WIDTH (kScreenWidth * 2)
#define PLATFORM_SDL1_WINDOW_HEIGHT (kScreenHeight * 2)
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_RESIZABLE | SDL_SWSURFACE | SDL_HWPALETTE)
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 0
#else
#define PLATFORM_SDL1_WINDOW_WIDTH (kScreenWidth * 4)
#define PLATFORM_SDL1_WINDOW_HEIGHT (kScreenHeight * 4)
#define PLATFORM_SDL1_WINDOW_FLAGS (SDL_RESIZABLE | SDL_SWSURFACE | SDL_HWPALETTE)
#define PLATFORM_SDL1_NEEDS_PSP_VBLANK_WAIT 0
#endif

#if defined(_3DS)
#define PLATFORM_FILE_BASE_PATH "romfs:/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH "sdmc:/OpenSupaplex/"
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 1
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#elif defined(__NDS__)
#define PLATFORM_FILE_BASE_PATH "nitro:/resources/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH ""
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#elif defined(__PSL1GHT__)
#define PLATFORM_FILE_BASE_PATH "/dev_hdd0/game/" PS3APPID "/USRDIR/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH PLATFORM_FILE_BASE_PATH
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#elif defined(__riscos__)
#define PLATFORM_FILE_BASE_PATH "/<OpenSupaplex$Dir>/data/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH "/<OpenSupaplex$Saves>/"
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 1
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#elif defined(__vita__)
#define PLATFORM_FILE_BASE_PATH "app0:/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH "ux0:/data/OpenSupaplex/"
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 1
#elif defined(__WII__)
#define PLATFORM_FILE_BASE_PATH "/apps/OpenSupaplex/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH PLATFORM_FILE_BASE_PATH
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#elif defined(__WIIU__)
#define PLATFORM_FILE_BASE_PATH "fs:/vol/external01/wiiu/apps/OpenSupaplex/"
#define PLATFORM_FILE_BASE_WRITABLE_PATH PLATFORM_FILE_BASE_PATH
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#else
#define PLATFORM_FILE_BASE_PATH ""
#define PLATFORM_FILE_BASE_WRITABLE_PATH PLATFORM_FILE_BASE_PATH
#define PLATFORM_NEEDS_POSIX_WRITABLE_DIR_CREATE 0
#define PLATFORM_NEEDS_SCE_WRITABLE_DIR_CREATE 0
#endif

#endif /* platformCapabilities_h */

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

#include "../video.h"

#include <math.h>
#include <SDL/SDL.h>
#include <stdlib.h>

#include "../logging.h"
#include "../utils.h"

#if defined(__PSP__)
#include <pspdisplay.h>

static const int kWindowWidth = 480;
static const int kWindowHeight = 272;
#else
static const int kWindowWidth = kScreenWidth * 4;
static const int kWindowHeight = kScreenHeight * 4;
#endif

SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
SDL_Rect gScreenClipRect;
SDL_Rect gWindowViewport;
SDL_Surface *gWindowSurface = NULL;
ScalingMode gScalingMode = ScalingModeAspectFit;

int windowResizingEventWatcher(void* data, SDL_Event* event);
void updateWindowViewport(void);

void initializeVideo(uint8_t fastMode)
{
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
    if (ret)
    {
        spLogInfo("SDL_Init failed with %d", ret);
        exit(1);
    }

    SDL_WM_SetCaption("OpenSupaplex", "OpenSupaplex");
    gWindowSurface =  SDL_SetVideoMode(kWindowWidth,
                                      kWindowHeight,
                                      8,
#if defined(__PSP__)
                                      SDL_FULLSCREEN |
#endif
                                      SDL_DOUBLEBUF | SDL_HWPALETTE);

    if (gWindowSurface == NULL)
    {
        spLogInfo("Could not create a window surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    gScreenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);

    if (gScreenSurface == NULL)
    {
        spLogInfo("Could not create a screen surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    updateWindowViewport();
}

void render()
{
    //SDL_BlitSurface(gScreenSurface, NULL, gWindowSurface, NULL); // TODO: use only this?
    SDL_SoftStretch(gScreenSurface, &gScreenClipRect, gWindowSurface, &gWindowViewport);
}

void present()
{
    SDL_Flip(gWindowSurface);

#if defined(__PSP__)
    // On PSP, only enable vsync for integer factor scaling. Otherwise, it will kill performance
    if (gScalingMode == ScalingModeIntegerFactor)
    {
        sceDisplayWaitVblankStart();
    }
#endif
}

void destroyVideo()
{
    if (gScreenSurface != NULL)
    {
        SDL_FreeSurface(gScreenSurface);
    }
    if (gWindowSurface != NULL)
    {
        SDL_FreeSurface(gWindowSurface);
    }
    SDL_Quit();
}

ScalingMode getScalingMode(void)
{
    return gScalingMode;
}

void setScalingMode(ScalingMode mode)
{
    if (mode == gScalingMode)
    {
        return;
    }

    gScalingMode = mode;
    updateWindowViewport();

    SDL_FillRect(gWindowSurface, NULL, 0);

    render();
    present();
}

void getWindowSize(int *width, int *height)
{
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    *width = info->current_w;
    *height = info->current_h;
}

void centerMouse()
{
    int kWindowWidth, kWindowHeight;
    getWindowSize(&kWindowWidth, &kWindowHeight);
    moveMouse(kWindowWidth / 2, kWindowHeight / 2);
}

void moveMouse(int x, int y)
{
    SDL_WarpMouse(x, y);
}

void hideMouse(void)
{
    SDL_ShowCursor(SDL_DISABLE);
}

void getMouseState(int *x, int *y, uint8_t *leftButton, uint8_t *rightButton)
{
    Uint32 state = SDL_GetMouseState(x, y);
    *leftButton = (state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    *rightButton = (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

void toggleFullscreen()
{
    // Not supported
}

void setFullscreenMode(uint8_t fullscreen)
{
    // Not supported
}

uint8_t getFullscreenMode(void)
{
    // Not supported
    return 0; // TODO: this should always be 1, actually, because SDL 1.2 is only used in consoles
}

void setGlobalPaletteColor(const uint8_t index, const Color color)
{
    SDL_SetPalette(gScreenSurface, SDL_LOGPAL, (SDL_Color *)&color, index, 1);
    SDL_SetPalette(gWindowSurface, SDL_PHYSPAL, (SDL_Color *)&color, index, 1);
}

void setColorPalette(const ColorPalette palette)
{
    SDL_SetPalette(gScreenSurface, SDL_LOGPAL, (SDL_Color *)palette, 0, kNumberOfColors);
    SDL_SetPalette(gWindowSurface, SDL_PHYSPAL, (SDL_Color *)palette, 0, kNumberOfColors);
}

void updateWindowViewport()
{
    gScreenClipRect.x = 0;
    gScreenClipRect.y = 0;
    gScreenClipRect.w = kScreenWidth;
    gScreenClipRect.h = kScreenHeight;

    // If the scaling mode is fullscreen, use the window size
    if (gScalingMode == ScalingModeFullscreen)
    {
        gWindowViewport.x = 0;
        gWindowViewport.y = 0;
        gWindowViewport.w = kWindowWidth;
        gWindowViewport.h = kWindowHeight;
        return;
    }

    float textureAspectRatio = (float)kScreenWidth / kScreenHeight;

    int maxViewportWidth = kWindowWidth;
    int maxViewportHeight = kWindowHeight;

    // For "integer factor" scaling, pick the highest integer factor that fits into the window
    if (gScalingMode == ScalingModeIntegerFactor)
    {
        maxViewportWidth = floorf(kWindowWidth / kScreenWidth) * kScreenWidth;
        maxViewportHeight = floorf(kWindowHeight / kScreenHeight) * kScreenHeight;
    }

    // If the resulting viewport is too small, do proportional scaling according to the window size
    if (maxViewportWidth == 0)
    {
        maxViewportWidth = kWindowWidth;
    }
    if (maxViewportHeight == 0)
    {
        maxViewportHeight = kWindowHeight;
    }

    float screenAspectRatio = (float)kWindowWidth / kWindowHeight;

    uint8_t shouldPreserveWidth = (textureAspectRatio > screenAspectRatio);

    // The only difference between aspect fill and fit is that fit will leave black bars
    // and fill will crop the image.
    //
    if (gScalingMode == ScalingModeAspectFill)
    {
        shouldPreserveWidth = !shouldPreserveWidth;
    }

    if (shouldPreserveWidth)
    {
        gWindowViewport.x = (kWindowWidth - maxViewportWidth) >> 1;
        gWindowViewport.w = maxViewportWidth;
        gWindowViewport.h = gWindowViewport.w / textureAspectRatio;
        gWindowViewport.y = (kWindowHeight - gWindowViewport.h) >> 1;
    }
    else
    {
        gWindowViewport.y = (kWindowHeight - maxViewportHeight) >> 1;
        gWindowViewport.h = maxViewportHeight;
        gWindowViewport.w = gWindowViewport.h * textureAspectRatio;
        gWindowViewport.x = (kWindowWidth - gWindowViewport.w) >> 1;
    }

    // Blit won't work with wrong values, so we must correct the viewport and adjust the clip rect
    if (gWindowViewport.x < 0)
    {
        float proportion = (float) -gWindowViewport.x / gWindowViewport.w;
        int offset = proportion * kScreenWidth;
        gScreenClipRect.x = offset;
        gScreenClipRect.w -= offset * 2;

        gWindowViewport.x = 0;
        gWindowViewport.w = kWindowWidth;
    }

    if (gWindowViewport.y < 0)
    {
        float proportion = (float) -gWindowViewport.y / gWindowViewport.h;
        int offset = proportion * kScreenHeight;
        gScreenClipRect.y = offset;
        gScreenClipRect.h -= offset * 2;

        gWindowViewport.y = 0;
        gWindowViewport.h = kWindowHeight;
    }
}

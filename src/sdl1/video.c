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
static const int kWindowFlags = SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#elif defined(_3DS)
static const int kWindowWidth = 400;
static const int kWindowHeight = 240;
#if DEBUG
// When building debug mode for 3DS, show stdout in the bottom screen
static const int kWindowFlags = SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_CONSOLEBOTTOM;
#else
static const int kWindowFlags = SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#endif
#elif defined(__NDS__)
static const int kWindowWidth = kScreenWidth;
static const int kWindowHeight = kScreenHeight;
static const int kWindowFlags = SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE | SDL_BOTTOMSCR;
#elif defined(__WII__)
static const int kWindowWidth = 640;
static const int kWindowHeight = 480;
static const int kWindowFlags = SDL_FULLSCREEN | SDL_SWSURFACE | SDL_HWPALETTE;
#elif defined(__riscos__)
static const int kWindowWidth = kScreenWidth * 2;
static const int kWindowHeight = kScreenHeight * 2;
static const int kWindowFlags = SDL_RESIZABLE | SDL_SWSURFACE | SDL_HWPALETTE;
#else
static const int kWindowWidth = kScreenWidth * 4;
static const int kWindowHeight = kScreenHeight * 4;
static const int kWindowFlags = SDL_RESIZABLE | SDL_SWSURFACE | SDL_HWPALETTE;
#endif

SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
SDL_Rect gScreenClipRect;
SDL_Rect gWindowViewport;
SDL_Surface *gWindowSurface = NULL;
ScalingMode gScalingMode = ScalingModeAspectFit;
SDL_Color gLogicalPalette[256], gPhysicalPalette[256];
int fullScreenWidth, fullScreenHeight;
int lastWindowedWidth, lastWindowedHeight;

int windowResizingEventFilter(const SDL_Event* event);
void updateWindowSize(int w, int h, Uint32 flags);
void updateWindowViewport(void);

void initializeVideo(uint8_t fastMode)
{
    int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (ret)
    {
        spLogInfo("SDL_InitSubSystem failed with %d", ret);
        exit(1);
    }

    getWindowSize(&fullScreenWidth, &fullScreenHeight);
    lastWindowedWidth = kWindowWidth;
    lastWindowedHeight = kWindowHeight;

    SDL_WM_SetCaption("OpenSupaplex", "OpenSupaplex");
    gWindowSurface =  SDL_SetVideoMode(kWindowWidth, kWindowHeight, 8, kWindowFlags);
    if (gWindowSurface == NULL)
    {
        spLogInfo("Could not create a window surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    SDL_SetEventFilter(windowResizingEventFilter);

    gScreenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);

    if (gScreenSurface == NULL)
    {
        spLogInfo("Could not create a screen surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    SDL_SetPalette(gScreenSurface, SDL_LOGPAL, gLogicalPalette, 0, SDL_arraysize(gLogicalPalette));
    SDL_SetPalette(gWindowSurface, SDL_LOGPAL, gLogicalPalette, 0, SDL_arraysize(gLogicalPalette));

    updateWindowViewport();
}

void render()
{
    if (gScreenClipRect.w == gWindowViewport.w && gScreenClipRect.h == gWindowViewport.h) {
        SDL_BlitSurface(gScreenSurface, &gScreenClipRect, gWindowSurface, &gWindowViewport);
    } else {
        SDL_SoftStretch(gScreenSurface, &gScreenClipRect, gWindowSurface, &gWindowViewport);
    }
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
    if (info && info->current_w > 0 && info->current_h > 0)
    {
        *width = info->current_w;
        *height = info->current_h;
    }
    else
    {
        *width = kWindowWidth;
        *height = kWindowHeight;
    }
}

void centerMouse()
{
    int windowWidth, windowHeight;
    getWindowSize(&windowWidth, &windowHeight);
    moveMouse(windowWidth / 2, windowHeight / 2);
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
    setFullscreenMode(getFullscreenMode() == 0);
}

void setFullscreenMode(uint8_t fullscreen)
{
    if (fullscreen)
    {
        if ((gWindowSurface->flags & SDL_FULLSCREEN) == 0)
        {
            lastWindowedWidth = gWindowSurface->w;
            lastWindowedHeight = gWindowSurface->h;
        }
        updateWindowSize(fullScreenWidth, fullScreenHeight, gWindowSurface->flags | SDL_FULLSCREEN);
    }
    else
    {
        updateWindowSize(lastWindowedWidth, lastWindowedHeight, gWindowSurface->flags & ~SDL_FULLSCREEN);
    }
}

uint8_t getFullscreenMode(void)
{
    return (gWindowSurface->flags & SDL_FULLSCREEN) != 0;
}

void setGlobalPaletteColor(const uint8_t index, const Color color)
{
    memcpy(&gPhysicalPalette[index], &color, sizeof(Color));
    SDL_SetPalette(gWindowSurface, SDL_PHYSPAL, &gPhysicalPalette[index], index, 1);
}

void setColorPalette(const ColorPalette palette)
{
    memcpy(gPhysicalPalette, palette, sizeof(ColorPalette));
    SDL_SetPalette(gWindowSurface, SDL_PHYSPAL, gPhysicalPalette, 0, kNumberOfColors);
}

int windowResizingEventFilter(const SDL_Event* event)
{
    if (event->type == SDL_VIDEORESIZE)
    {
        updateWindowSize(event->resize.w, event->resize.h, gWindowSurface->flags);
    }

    return 0;
}

void updateWindowSize(int w, int h, Uint32 flags)
{
    gWindowSurface =  SDL_SetVideoMode(w, h, 8, flags);

    SDL_SetPalette(gWindowSurface, SDL_LOGPAL, gLogicalPalette, 0, SDL_arraysize(gLogicalPalette));
    SDL_SetPalette(gWindowSurface, SDL_PHYSPAL, gPhysicalPalette, 0, SDL_arraysize(gPhysicalPalette));

    updateWindowViewport();
    render();
    present();
}

void updateWindowViewport()
{
    int windowWidth, windowHeight;
    getWindowSize(&windowWidth, &windowHeight);

    gScreenClipRect.x = 0;
    gScreenClipRect.y = 0;
    gScreenClipRect.w = kScreenWidth;
    gScreenClipRect.h = kScreenHeight;

    // If the scaling mode is fullscreen, use the window size
    if (gScalingMode == ScalingModeFullscreen)
    {
        gWindowViewport.x = 0;
        gWindowViewport.y = 0;
        gWindowViewport.w = windowWidth;
        gWindowViewport.h = windowHeight;
        return;
    }

    float textureAspectRatio = (float)kScreenWidth / kScreenHeight;

    // The only way in which "aspect correct" differs from "aspect fit"
    // is that 4:3 is used instead of kScreenWidth:kScreenHeight
    if (gScalingMode == ScalingModeAspectCorrect)
    {
        textureAspectRatio = 4.f/3.f;
    }

    int maxViewportWidth = windowWidth;
    int maxViewportHeight = windowHeight;

    // For "integer factor" scaling, pick the highest integer factor that fits into the window
    if (gScalingMode == ScalingModeIntegerFactor)
    {
        maxViewportWidth = floorf(windowWidth / kScreenWidth) * kScreenWidth;
        maxViewportHeight = floorf(windowHeight / kScreenHeight) * kScreenHeight;
    }

    // If the resulting viewport is too small, do proportional scaling according to the window size
    if (maxViewportWidth == 0)
    {
        maxViewportWidth = windowWidth;
    }
    if (maxViewportHeight == 0)
    {
        maxViewportHeight = windowHeight;
    }

    float screenAspectRatio = (float)windowWidth / windowHeight;

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
        gWindowViewport.x = (windowWidth - maxViewportWidth) >> 1;
        gWindowViewport.w = maxViewportWidth;
        gWindowViewport.h = gWindowViewport.w / textureAspectRatio;
        gWindowViewport.y = (windowHeight - gWindowViewport.h) >> 1;
    }
    else
    {
        gWindowViewport.y = (windowHeight - maxViewportHeight) >> 1;
        gWindowViewport.h = maxViewportHeight;
        gWindowViewport.w = gWindowViewport.h * textureAspectRatio;
        gWindowViewport.x = (windowWidth - gWindowViewport.w) >> 1;
    }

    // Blit won't work with wrong values, so we must correct the viewport and adjust the clip rect
    if (gWindowViewport.x < 0)
    {
        float proportion = (float) -gWindowViewport.x / gWindowViewport.w;
        int offset = proportion * kScreenWidth;
        gScreenClipRect.x = offset;
        gScreenClipRect.w -= offset * 2;

        gWindowViewport.x = 0;
        gWindowViewport.w = windowWidth;
    }

    if (gWindowViewport.y < 0)
    {
        float proportion = (float) -gWindowViewport.y / gWindowViewport.h;
        int offset = proportion * kScreenHeight;
        gScreenClipRect.y = offset;
        gScreenClipRect.h -= offset * 2;

        gWindowViewport.y = 0;
        gWindowViewport.h = windowHeight;
    }
}

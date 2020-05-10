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

#include "video.h"

#include <SDL2/SDL.h>

#include "logging.h"
#include "utils.h"

#ifdef __vita__
static const int kWindowWidth = kScreenWidth;
static const int kWindowHeight = kScreenHeight;
#else
static const int kWindowWidth = kScreenWidth * 4;
static const int kWindowHeight = kScreenHeight * 4;
#endif

SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
SDL_Window *gWindow = NULL;
SDL_Rect gWindowViewport;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;
SDL_Surface *gTextureSurface = NULL;
ScalingMode gScalingMode = ScalingModeAspectFit;

int windowResizingEventWatcher(void* data, SDL_Event* event);
void updateWindowViewport(void);

void initializeVideo()
{
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
    if (ret)
    {
        spLog("SDL_Init failed with %d", ret);
        exit(1);
    }

    gWindow = SDL_CreateWindow("OpenSupaplex",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               kWindowWidth,
                               kWindowHeight,
#if defined(__SWITCH__) || defined(__vita__)
                               SDL_WINDOW_FULLSCREEN);
#else
                               0);
#endif

    if (gWindow == NULL)
    {
        spLog("Could not create a window: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_SetWindowResizable(gWindow, SDL_TRUE);
    SDL_AddEventWatch(windowResizingEventWatcher, gWindow);

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);

    gTexture = SDL_CreateTexture(gRenderer,
                                             SDL_PIXELFORMAT_ARGB32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             kScreenWidth, kScreenHeight);

    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_ARGB32);

    gScreenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);
    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    updateWindowViewport();
}

void render()
{
    SDL_BlitSurface(gScreenSurface, NULL, gTextureSurface, NULL);

    SDL_UpdateTexture(gTexture, NULL, gTextureSurface->pixels, gTextureSurface->pitch);
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, &gWindowViewport);
}

void present()
{
    SDL_RenderPresent(gRenderer);
}

void destroyVideo()
{
    SDL_DelEventWatch(windowResizingEventWatcher, gWindow);
    SDL_FreeSurface(gTextureSurface);
    SDL_DestroyTexture(gTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
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
}

void getWindowSize(int *width, int *height)
{
    SDL_GetWindowSize(gWindow, width, height);
}

void centerMouse()
{
    int windowWidth, windowHeight;
    getWindowSize(&windowWidth, &windowHeight);
    moveMouse(windowWidth / 2, windowHeight / 2);
}

void moveMouse(int x, int y)
{
    SDL_WarpMouseInWindow(gWindow, x, y);
}

void toggleFullscreen()
{
    uint8_t isFullscreen = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(gWindow, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
}

void setColorPalette(const ColorPalette palette)
{
#ifdef __vita__
    // For some reason (SDL bug?) in PS Vita using SDL_SetPaletteColors here
    // specifically breaks the game colors. This seems to work, will investigate later...
    //
    for (uint8_t i = 0; i < kNumberOfColors; ++i)
    {
        gScreenSurface->format->palette->colors[i].r = palette[i].r;
        gScreenSurface->format->palette->colors[i].g = palette[i].g;
        gScreenSurface->format->palette->colors[i].b = palette[i].b;
    }
    gScreenSurface->format->palette->version++;
    if (gScreenSurface->format->palette->version == 0)
    {
        gScreenSurface->format->palette->version = 1;
    }
#else
    SDL_SetPaletteColors(gScreenSurface->format->palette, (SDL_Color *)palette, 0, kNumberOfColors);
#endif
}

int windowResizingEventWatcher(void* data, SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT
        && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        updateWindowViewport();
        render();
        present();
    }

    return 0;
}

void updateWindowViewport()
{
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(gRenderer, &windowWidth, &windowHeight);

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
}

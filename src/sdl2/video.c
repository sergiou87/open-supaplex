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
#include <SDL2/SDL.h>
#include <stdlib.h>

#include "../logging.h"
#include "../utils.h"

#if defined(__PSP__)
static const int kWindowWidth = 480;
static const int kWindowHeight = 272;
#elif defined(__vita__)
static const int kWindowWidth = 960;
static const int kWindowHeight = 544;
#elif defined(__PSL1GHT__) || defined(__WIIU__)
static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;
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

void initializeVideo(uint8_t fastMode)
{
    int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (ret)
    {
        spLogInfo("SDL_InitSubSystem failed with %d", ret);
        exit(1);
    }

    gWindow = SDL_CreateWindow("OpenSupaplex",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               kWindowWidth,
                               kWindowHeight,
#if defined(__SWITCH__) || defined(__vita__) || defined(__PSP__) || defined(__PSL1GHT__) || defined(__WIIU__) || defined(__PS2__)
                               SDL_WINDOW_FULLSCREEN);
#else
                               0);
#endif

    if (gWindow == NULL)
    {
        spLogInfo("Could not create a window: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    SDL_SetWindowResizable(gWindow, SDL_TRUE);
    SDL_AddEventWatch(windowResizingEventWatcher, gWindow);

    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;

    // Disable vsync in fast mode to prevent limiting FPS rate
    if (fastMode == 0)
    {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, rendererFlags);

    if (gRenderer == NULL)
    {
        spLogInfo("Could not create a renderer: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    Uint32 format = SDL_PIXELFORMAT_RGB24;

    // HACK: this is needed for my crappy SDL2 implementation (https://github.com/sergiou87/SDL2/commit/962e4e565562c2cd70b877f3d697ad2084d9405b)
    // but this should be fixed on SDL's side (or pspgl's), I think.
    //
#ifdef __PSP__
    format = SDL_PIXELFORMAT_ABGR32;
#endif

    gTexture = SDL_CreateTexture(gRenderer,
                                 format,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 kScreenWidth, kScreenHeight);

    if (gTexture == NULL)
    {
        spLogInfo("Could not create a texture: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_RGB24);

    if (gTextureSurface == NULL)
    {
        spLogInfo("Could not create a texture surface: %s", SDL_GetError());
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
    if (gScreenSurface != NULL)
    {
        SDL_FreeSurface(gScreenSurface);
    }
    if (gTextureSurface != NULL)
    {
        SDL_FreeSurface(gTextureSurface);
    }
    if (gTexture != NULL)
    {
        SDL_DestroyTexture(gTexture);
    }
    if (gRenderer != NULL)
    {
        SDL_DestroyRenderer(gRenderer);
    }
    if (gWindow != NULL)
    {
        SDL_DestroyWindow(gWindow);
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
    SDL_SetWindowFullscreen(gWindow, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

uint8_t getFullscreenMode(void)
{
    uint8_t isFullscreen = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN;
    return (isFullscreen != 0);
}

void setGlobalPaletteColor(const uint8_t index, const Color color)
{
    SDL_SetPaletteColors(gScreenSurface->format->palette, (SDL_Color *)&color, index, 1);
}

void setColorPalette(const ColorPalette palette)
{
    SDL_SetPaletteColors(gScreenSurface->format->palette, (SDL_Color *)palette, 0, kNumberOfColors);
}

int windowResizingEventWatcher(void* /*data*/, SDL_Event* event)
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
}

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
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "../logging.h"
#include "../utils.h"

// Included rather than compiled separately, so the build files of the ports
// that use this backend don't have to list it. See src/lib/hqx/README.md.
#include "../lib/hqx/hq4x.c"

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

// hq4x resources, created on demand. hq4x blends colors, so every frame is
// converted with the current palette first, which keeps fades and color
// flashes working.
PixelFilter gPixelFilter = PixelFilterNone;
static SDL_Texture *sFilteredTexture = NULL;
static uint32_t *sHQ4xSourcePixels = NULL;
static uint32_t *sHQ4xOutputPixels = NULL;

static const int kHQ4xScale = 4;

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
#ifdef __PS2__
        SDL_SetHint(SDL_HINT_PS2_DYNAMIC_VSYNC, "1");
#endif        
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
#ifdef __PS2__
    format = SDL_PIXELFORMAT_ABGR1555;
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

#ifdef __PS2__
    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_ABGR1555);
#else
    gTextureSurface = SDL_CreateRGBSurfaceWithFormat(0, kScreenWidth, kScreenHeight, 8, SDL_PIXELFORMAT_RGB24);
#endif

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

static void applyHQ4xFilter(void)
{
    // Convert the palette indices to 0x00RRGGBB colors with the current palette
    const SDL_Palette *palette = gScreenSurface->format->palette;
    uint32_t colors[256] = {0};
    for (int i = 0; i < palette->ncolors && i < 256; ++i)
    {
        colors[i] = ((uint32_t)palette->colors[i].r << 16)
                    | ((uint32_t)palette->colors[i].g << 8)
                    | palette->colors[i].b;
    }

    const uint8_t *src = gScreenSurface->pixels;
    for (int y = 0; y < kScreenHeight; ++y)
    {
        const uint8_t *row = src + y * gScreenSurface->pitch;
        uint32_t *dst = sHQ4xSourcePixels + y * kScreenWidth;
        for (int x = 0; x < kScreenWidth; ++x)
        {
            dst[x] = colors[row[x]];
        }
    }

    hq4x_32_rb(sHQ4xSourcePixels, kScreenWidth * sizeof(uint32_t),
               sHQ4xOutputPixels, kScreenWidth * kHQ4xScale * sizeof(uint32_t),
               kScreenWidth, kScreenHeight);
}

static void destroyFilteredResources(void)
{
    if (sFilteredTexture != NULL)
    {
        SDL_DestroyTexture(sFilteredTexture);
        sFilteredTexture = NULL;
    }
    free(sHQ4xSourcePixels);
    sHQ4xSourcePixels = NULL;
    free(sHQ4xOutputPixels);
    sHQ4xOutputPixels = NULL;
}

/// @return 1 if the hq4x resources are ready
static uint8_t ensureFilteredResources(void)
{
    if (sFilteredTexture != NULL)
    {
        return 1;
    }

    int width = kScreenWidth * kHQ4xScale;
    int height = kScreenHeight * kHQ4xScale;

    // The filtered frame is larger than the window viewport in most cases, so
    // let it be sampled smoothly instead of dropping or doubling pixel rows.
    char previousScaleQuality[16] = "nearest";
    const char *hint = SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);
    if (hint != NULL)
    {
        snprintf(previousScaleQuality, sizeof(previousScaleQuality), "%s", hint);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    // hq4x outputs 0x00RRGGBB colors. SDL converts them on upload if the
    // renderer has no texture format for them.
    sFilteredTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, previousScaleQuality);

    sHQ4xSourcePixels = malloc(kScreenWidth * kScreenHeight * sizeof(uint32_t));
    sHQ4xOutputPixels = malloc(width * height * sizeof(uint32_t));

    if (sFilteredTexture == NULL || sHQ4xSourcePixels == NULL || sHQ4xOutputPixels == NULL)
    {
        spLogInfo("Could not create pixel filter resources: %s", SDL_GetError());
        destroyFilteredResources();
        gPixelFilter = PixelFilterNone;
        return 0;
    }

    return 1;
}

void render()
{
    // In some cases (like using Wayland on Linux) some SDL events could be triggered
    // and end up triggering a render before initializeVideo is done.
    if (gScreenSurface == NULL)
    {
        return;
    }

    if (gPixelFilter == PixelFilterHQ4x && ensureFilteredResources())
    {
        applyHQ4xFilter();
        SDL_UpdateTexture(sFilteredTexture, NULL, sHQ4xOutputPixels, kScreenWidth * kHQ4xScale * sizeof(uint32_t));
        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, sFilteredTexture, NULL, &gWindowViewport);
        return;
    }

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
    destroyFilteredResources();
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

uint8_t isPixelFilterSupported(void)
{
    // Only offered on desktop and in the browser, where it has been tested.
    // On consoles the 1280x800 filtered texture may not fit in video memory
    // (PS2) or render fast enough.
#if defined(__EMSCRIPTEN__) || defined(__APPLE__) || defined(_WIN32) || defined(__linux__)
    return 1;
#else
    return 0;
#endif
}

PixelFilter getPixelFilter(void)
{
    return gPixelFilter;
}

void setPixelFilter(PixelFilter filter)
{
    if (isPixelFilterSupported() == 0
        || filter < PixelFilterNone
        || filter >= PixelFilterCount)
    {
        filter = PixelFilterNone;
    }

    // Resources are created by the next render that needs them, and kept
    // until the video is destroyed
    gPixelFilter = filter;
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

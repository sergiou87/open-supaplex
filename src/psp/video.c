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
#include <stdlib.h>
#include <SDL/SDL.h>

#include "../logging.h"
#include "../utils.h"

#include <pspdisplay.h>
#include <GL/gl.h>
#include <GL/glu.h>

static const int kWindowWidth = 480;
static const int kWindowHeight = 272;
    
SDL_Surface *gScreenSurface = NULL;
uint8_t *gScreenPixels = NULL;
uint8_t *gScreenPixels2 = NULL;
SDL_Rect gScreenClipRect;
SDL_Rect gWindowViewport;
SDL_Surface *gWindowSurface = NULL;
ScalingMode gScalingMode = ScalingModeAspectFit;

int windowResizingEventWatcher(void* data, SDL_Event* event);
void updateWindowViewport(void);

// This implementation leverages SDL only to initialize the graphics context,
// but then relies on PSP's OpenGL for the fastest performance rendering the
// game graphics.

void initializeVideo(uint8_t fastMode)
{
    int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (ret)
    {
        spLogInfo("SDL_InitSubSystem failed with %d", ret);
        exit(1);
    }

    int flags = SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE | SDL_HWPALETTE;

    SDL_WM_SetCaption("OpenSupaplex", "OpenSupaplex");
    gWindowSurface =  SDL_SetVideoMode(kWindowWidth,
                                      kWindowHeight,
                                      8,
                                      flags);

    if (gWindowSurface == NULL)
    {
        spLogInfo("Could not create a window surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }

    gScreenSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, kScreenWidth, kScreenHeight, 8, 0, 0, 0, 0);
    if (gScreenSurface == NULL)
    {
        spLogInfo("Could not create a screen surface: %s", SDL_GetError());
        destroyVideo();
        exit(1);
    }
    gScreenPixels = (uint8_t *)gScreenSurface->pixels;

    // Prepare for OpenGL rendering
    glEnable(GL_COLOR_TABLE);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, kWindowWidth, kWindowHeight, 0, 1, -1);
    
    updateWindowViewport();
}

void render()
{
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glColorTable(GL_TEXTURE_2D, GL_RGBA, 256 * 4, GL_RGBA, GL_UNSIGNED_BYTE, gScreenSurface->format->palette);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, 512, 256, 0, GL_COLOR_INDEX8_EXT, GL_UNSIGNED_BYTE, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kScreenWidth, kScreenHeight,
                    GL_COLOR_INDEX8_EXT, GL_UNSIGNED_BYTE, gScreenSurface->pixels);

    // Draw image
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2i(gWindowViewport.x - gScreenClipRect.x, 
                   gWindowViewport.y - gScreenClipRect.y);

        glTexCoord2f(0.625, 0);
        glVertex2i(gWindowViewport.x + gWindowViewport.w + gScreenClipRect.x,
                   gWindowViewport.y - gScreenClipRect.y);

        glTexCoord2f(0.625, 0.78125);
        glVertex2i(gWindowViewport.x + gWindowViewport.w + gScreenClipRect.x,
                   gWindowViewport.y + gWindowViewport.h + gScreenClipRect.y);

        glTexCoord2f(0, 0.78125);
        glVertex2i(gWindowViewport.x - gScreenClipRect.x,
                   gWindowViewport.y + gWindowViewport.h + gScreenClipRect.y);
    glEnd();
}

void present()
{
    SDL_GL_SwapBuffers();
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
    // Not supported. Since SDL 1 is always used in consoles, always return 1.
    return 1;
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

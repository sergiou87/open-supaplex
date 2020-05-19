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

#include "graphics.h"

#include <assert.h>
#include <string.h>

#include "globals.h"
#include "utils.h"

uint8_t gChars6BitmapFont[kBitmapFontLength];
uint8_t gChars8BitmapFont[kBitmapFontLength];
uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];
uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];
uint8_t gPanelDecodedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gPanelRenderedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gCurrentPanelHeight = kPanelBitmapHeight;
uint8_t gMenuBitmapData[kFullScreenBitmapLength]; // 0x4D34 ??
uint8_t gControlsBitmapData[kFullScreenBitmapLength];
uint8_t gBackBitmapData[kFullScreenBitmapLength];
uint8_t gGfxBitmapData[kFullScreenBitmapLength];
uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];
uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];
uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

void readMenuDat() // proc near       ; CODE XREF: readEverything+9p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("MENU.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening MENU.DAT\n");
    }

//loc_4AAED:             // ; CODE XREF: readMenuDat+8j

    size_t bytes = fread(gMenuBitmapData, sizeof(uint8_t), sizeof(gMenuBitmapData), file);
    if (bytes < sizeof(gMenuBitmapData))
    {
        exitWithError("Error reading MENU.DAT\n");
    }

//loc_4AB0B:              // ; CODE XREF: readMenuDat+25j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MENU.DAT\n");
    }
}

void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    int scrollX = MAX(0, MIN(x, kLevelBitmapWidth - width));
    int scrollY = MAX(0, MIN(y, kLevelBitmapHeight - height));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            gScreenPixels[y * kScreenWidth + x] = gLevelBitmapData[(scrollY + y) * kLevelBitmapWidth + x + scrollX];
        }
    }
}

void drawCurrentLevelViewport(uint16_t panelHeight)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    uint16_t viewportHeight = kScreenHeight - panelHeight;

    drawLevelViewport(gScrollOffsetX, gScrollOffsetY, kScreenWidth, viewportHeight);

    for (int y = 0; y < panelHeight; ++y)
    {
        uint32_t srcAddress = y * kPanelBitmapWidth;
        uint32_t dstAddress = (viewportHeight + y) * kScreenWidth;
        memcpy(&gScreenPixels[dstAddress], &gPanelRenderedBitmapData[srcAddress], kPanelBitmapWidth);
    }
}

void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, int16_t dstX, int16_t dstY)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    assert((width % kTileSize) == 0);

    for (int y = 0; y < height; ++y)
    {
        int16_t finalY = dstY + y;

        for (int x = 0; x < width; ++x)
        {
            int16_t finalX = dstX + x;

            size_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX + x;
            size_t dstAddress = (finalY - kLevelEdgeSize) * kLevelBitmapWidth + finalX - kLevelEdgeSize;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }
}

// srcX and srcY are the coordinates of the frame to draw in MOVING.DAT
void drawMovingFrame(uint16_t srcX, uint16_t srcY, uint16_t destPosition) // sub_4F200   proc near       ; CODE XREF: scrollToMurphy+26p
                   // ; updatePlantedRedDisk+2Ap ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:859D
    // Draws a frame from MOVING.DAT on the screen
    // Parameters:
    // - di: coordinates on the screen
    // - si: coordinates on the MOVING.DAT bitmap to draw from?

    int16_t destX = (destPosition % kLevelWidth) * kTileSize;
    int16_t destY = (destPosition / kLevelWidth) * kTileSize;

    drawMovingSpriteFrameInLevel(srcX, srcY, kTileSize, kTileSize, destX, destY);
}


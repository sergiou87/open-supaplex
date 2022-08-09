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
#include "system.h"

ColorPalette gCurrentPalette;

// Game palettes:
// - 0: level credits
// - 1: main menu
// - 2: ???
// - 3: ???
//
ColorPalette gPalettes[kNumberOfPalettes];
ColorPalette gBlackPalette = { // 60D5h
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
};

ColorPaletteData gTitlePaletteData = {
    0x02, 0x03, 0x05, 0x00, 0x0D, 0x0A, 0x04, 0x0C, 0x02, 0x06, 0x06, 0x02, 0x03, 0x09, 0x09, 0x03,
    0x0B, 0x08, 0x03, 0x06, 0x02, 0x07, 0x07, 0x0A, 0x08, 0x06, 0x0D, 0x09, 0x06, 0x04, 0x0B, 0x01,
    0x09, 0x01, 0x00, 0x04, 0x0B, 0x01, 0x00, 0x04, 0x0D, 0x01, 0x00, 0x0C, 0x0F, 0x01, 0x00, 0x0C,
    0x0F, 0x06, 0x04, 0x0C, 0x02, 0x05, 0x06, 0x08, 0x0F, 0x0C, 0x06, 0x0E, 0x0C, 0x0C, 0x0D, 0x0E,
};

ColorPaletteData gTitle1PaletteData = {
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
    0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
    0x08, 0x08, 0x08, 0x08, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08,
};

ColorPaletteData gTitle2PaletteData = {
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x06, 0x06, 0x06, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
    0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
    0x01, 0x03, 0x07, 0x00, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
    0x00, 0x02, 0x0A, 0x01, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x07,
};

#define kBitmapFontCharacter8Width 8
#define kNumberOfCharactersInBitmapFont 64
#define kBitmapFontLength (kNumberOfCharactersInBitmapFont * 8) // The size of the bitmap is a round number, not 100% related to the size font, there is some padding :shrug:

uint8_t gChars6BitmapFont[kBitmapFontLength];
uint8_t gChars8BitmapFont[kBitmapFontLength];

#define kFixedBitmapWidth 640
#define kFixedBitmapHeight 16
uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];

#define kMovingBitmapWidth 320
#define kMovingBitmapHeight 462
uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];

uint8_t gPanelDecodedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gPanelRenderedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gCurrentPanelHeight = kPanelBitmapHeight;

#define kFullScreenBitmapLength (kScreenWidth * kScreenHeight / 2) // They use 4 bits to encode pixels

// These buffers contain the raw bitmap data of the corresponding DAT files, separated in 4 bitmaps:
//  - The bitmap has 4 columns of 40 bytes width x 200 bytes height
//  - Every column represents a component (R, G, B and intensity).
//  - In one of these columns/components, every bit represents a pixel (40 bytes x 8 bits = 320 pixels)
//  - The way these are decoded seems to be to pick, for every pixel, the bit of the r column, the bit of the g column,
//    the bit from the b column and the bit from the intensity column, and create a 4 bit number that represents an index
//    in the 16 color palette.
//
uint8_t gMenuBitmapData[kFullScreenBitmapLength]; // 0x4D34 ??
uint8_t gControlsBitmapData[kFullScreenBitmapLength];
uint8_t gBackBitmapData[kFullScreenBitmapLength];
uint8_t gGfxBitmapData[kFullScreenBitmapLength];

uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];
uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];
uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

uint32_t gRenderDeltaTime = 0;

// This points to the address on the screen where the mouse cursor was
// drawn the last time, used to clear the cursor before redrawing it again
//
uint16_t gLastMouseCursorOriginAddress = 0; // word_5847B

// This buffer will hold the bitmap of the area where the cursor is going to be drawn
// with the intention of restoring that area before rendering the mouse in a new
// position, hence clearing the trail the mouse would leave.
//
#define kLastMouseCursorAreaSize 8
uint8_t gLastMouseCursorAreaBitmap[kLastMouseCursorAreaSize * kLastMouseCursorAreaSize];

// Used to measure display frame rate
float gFrameRate = 0.f;
uint32_t gFrameRateReferenceTime = 0;

void drawFullScreenBitmap(uint8_t *bitmapData, uint8_t *dest);

void readMenuDat() // proc near       ; CODE XREF: readEverything+9p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("MENU.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening MENU.DAT\n");
    }

//loc_4AAED:             // ; CODE XREF: readMenuDat+8j

    size_t bytes = fileReadBytes(gMenuBitmapData, sizeof(gMenuBitmapData), file);
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

void loadMurphySprites() // readMoving  proc near       ; CODE XREF: start:isFastModep
                    //; start+382p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:0D84

//loc_479ED:              // ; CODE XREF: loadMurphySprites+27j

// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// MOVING.DAT bitmap size is 320x462
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    FILE *file = openReadonlyFile("MOVING.DAT", "rb");

    if (file == NULL)
    {
//    loc_47A10:              ; CODE XREF: loadMurphySprites+13j
//                    ; loadMurphySprites+25j
        exitWithError("Error opening MOVING.DAT\n");
    }

//loc_47A13:              ; CODE XREF: loadMurphySprites+Ej

    // There is an error in the original code, it uses height 464 (which should be
    // harmless in the original implementation since fileReadBytes doesn't fail there, just
    // returns 0. The new implementation will fail on partial reads.
    for (int y = 0; y < kMovingBitmapHeight; ++y)
    {
        uint8_t fileData[kMovingBitmapWidth / 2];

        size_t bytes = fileReadBytes(fileData, sizeof(fileData), file);
        if (bytes < sizeof(fileData))
        {
            exitWithError("Error reading MOVING.DAT\n");
        }

//loc_47A40:              ; CODE XREF: loadMurphySprites+BAj
        for (int x = 0; x < kMovingBitmapWidth; ++x)
        {
//loc_47A45:              ; CODE XREF: loadMurphySprites+AEj
            uint32_t destPixelAddress = y * kMovingBitmapWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gMovingDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MOVING.DAT\n");
    }

//loc_47AB1:              ; CODE XREF: loadMurphySprites+C5j
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // FIXED.DAT bitmap size is 640x16
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    file = openReadonlyFile("FIXED.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening FIXED.DAT\n");
    }

//loc_47ABE:              //; CODE XREF: loadMurphySprites+D2j
    uint8_t bitmapData[kFixedBitmapWidth * kFixedBitmapHeight / 2];
    size_t bytes = fileReadBytes(bitmapData, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading FIXED.DAT\n");
    }

    for (int y = 0; y < kFixedBitmapHeight; ++y)
    {
        for (int x = 0; x < kFixedBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kFixedBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kFixedBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 160] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 240] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gFixedDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47AD5:              //; CODE XREF: loadMurphySprites+E9j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing FIXED.DAT\n");
    }
}

void readPanelDat() //    proc near       ; CODE XREF: readPanelDat+14j
                    // ; readEverything+6p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("PANEL.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening PANEL.DAT\n");
    }

//loc_47B0A:             // ; CODE XREF: readPanelDat+8j
    uint8_t bitmapData[kPanelBitmapWidth * kPanelBitmapHeight / 2];
    size_t bytes = fileReadBytes(bitmapData, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading PANEL.DAT\n");
    }

    for (int y = 0; y < kPanelBitmapHeight; ++y)
    {
        for (int x = 0; x < kPanelBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kPanelBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kPanelBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gPanelDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47B21:              // ; CODE XREF: readPanelDat+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PANEL.DAT\n");
    }
}

void readBackDat() // proc near       ; CODE XREF: readBackDat+14j
                    // ; readEverything+15p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // address: 01ED:0ECD
    FILE *file = openReadonlyFile("BACK.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening BACK.DAT\n");
    }

//loc_47B49:             // ; CODE XREF: readBackDat+8j
    size_t bytes = fileReadBytes(gBackBitmapData, sizeof(gBackBitmapData), file);
    if (bytes < sizeof(gBackBitmapData))
    {
        exitWithError("Error reading BACK.DAT\n");
    }

//loc_47B67:              //; CODE XREF: readBackDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing BACK.DAT\n");
    }
}

// The bitmap fonts are bitmaps of 512x8 pixels. The way they're encoded is each bit is a pixel that
// is either on or off, so this is encoded in exactly 512 bytes.
// These bits are used as bitmasks to render on top of an existing image combining:
// - enabling the corresponding pixels in the frame buffer
// - setting those pixels from the font in the bitmask to prevent the VGA controller from overriding
//   other pixels (i.e. preserving the content of the original image in the pixels that are "empty"
//   in the font).
//
void readBitmapFonts() //   proc near       ; CODE XREF: readBitmapFonts+14j
                    // ; readEverything+3p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("CHARS6.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS6.DAT\n");
    }

//loc_47B90:              // ; CODE XREF: readBitmapFonts+8j
    size_t bytes = fileReadBytes(gChars6BitmapFont, kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS6.DAT\n");
    }

//loc_47BA7:              // ; CODE XREF: readBitmapFonts+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS6.DAT\n");
    }

//loc_47BB5:              //; CODE XREF: readBitmapFonts+39j
    file = openReadonlyFile("CHARS8.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS8.DAT\n");
    }

//loc_47BC2:              //; CODE XREF: readBitmapFonts+46j
    bytes = fileReadBytes(gChars8BitmapFont, kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS8.DAT\n");
    }

//loc_47BD9:             // ; CODE XREF: readBitmapFonts+5Dj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS8.DAT\n");
    }
}

void readAndRenderTitleDat() // proc near  ; CODE XREF: start+2BBp
{
    FILE *file = openReadonlyFile("TITLE.DAT", "rb");

    if (file == NULL)
    {
        exitWithError("Error opening TITLE.DAT\n");
    }

//loc_47C18:              // ; CODE XREF: readAndRenderTitleDat+2Bj

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
//loc_47C3F:              //; CODE XREF: readAndRenderTitleDat+8Ej
        // read 160 bytes from title.dat
        size_t bytesRead = fileReadBytes(fileData, kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_47C54:             // ; CODE XREF: readAndRenderTitleDat+67j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

//loc_47C59:              ; CODE XREF: readAndRenderTitleDat+88j
            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE.DAT\n");
    }
}

void readAndRenderTitle1Dat(void)
{
//loc_478C0:              // ; CODE XREF: loadScreen2+8j
    FILE *file = openReadonlyFile("TITLE1.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE1.DAT\n");
    }

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
//loc_478E7:              //; CODE XREF: loadScreen2+6Bj
        // read 160 bytes from title.dat
        size_t bytesRead = fileReadBytes(fileData, kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE1.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Copy directly to the screen too
            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE1.DAT\n");
    }
}

void readTitle2Dat()
{
    FILE *file = openReadonlyFile("TITLE2.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE2.DAT\n");
    }

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

// loc_47978:              //; CODE XREF: loadScreen2+C0j
    for (int y = 0; y < kScreenHeight; y++)
    {
        // loc_47995:              //; CODE XREF: loadScreen2+119j
        // read 160 bytes from title.dat
        size_t bytesRead = fileReadBytes(fileData, kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE2.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_479AF:              ; CODE XREF: loadScreen2+113j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gTitle2DecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE2.DAT\n");
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
        size_t dstAddress = y * kScreenWidth;
        size_t srcAddress = (scrollY + y) * kLevelBitmapWidth + scrollX;
        memcpy(&gScreenPixels[dstAddress], &gLevelBitmapData[srcAddress], width);
    }
}

void readGfxDat() //  proc near       ; CODE XREF: readGfxDat+14j
                   // ; readEverything+1Ep
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("GFX.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening GFX.DAT\n");
    }

//loc_47DB5:             // ; CODE XREF: readGfxDat+8j
    size_t bytes = fileReadBytes(gGfxBitmapData, sizeof(gGfxBitmapData), file);
    if (bytes < sizeof(gGfxBitmapData))
    {
        exitWithError("Error reading GFX.DAT\n");
    }

//loc_47DD3:             // ; CODE XREF: readGfxDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing GFX.DAT\n");
    }

// readGfxDat  endp
}

void readControlsDat() // proc near       ; CODE XREF: readControlsDat+14j
                    // ; readEverything+Cp
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("CONTROLS.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening CONTROLS.DAT\n");
    }

//loc_47DFC:             // ; CODE XREF: readControlsDat+8j
    size_t bytes = fileReadBytes(gControlsBitmapData, sizeof(gControlsBitmapData), file);
    if (bytes < sizeof(gControlsBitmapData))
    {
        exitWithError("Error reading CONTROLS.DAT\n");
    }

//loc_47E1A:             // ; CODE XREF: readControlsDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CONTROLS.DAT\n");
    }
}

// This function calculates where in the main menu bitmap the cursor will be drawn, and takes in advance
// the area of the screen so that it can be restored later.
//
void saveLastMouseAreaBitmap() // sub_4B85C  proc near       ; CODE XREF: handleNewPlayerOptionClick+FCp
//                    ; handleNewPlayerOptionClick+124p ...
{
    // 01ED:4BF9
    gLastMouseCursorOriginAddress = gMouseY * kScreenWidth + gMouseX;

    for (int y = 0; y < kLastMouseCursorAreaSize; ++y)
    {
        for (int x = 0; x < kLastMouseCursorAreaSize; ++x)
        {
            uint32_t destAddress = y * kLastMouseCursorAreaSize + x;
            uint32_t sourceAddress = (gLastMouseCursorOriginAddress
                                      + y * kScreenWidth
                                      + x);

            gLastMouseCursorAreaBitmap[destAddress] = gScreenPixels[sourceAddress];
        }
    }
}

// This function will restore the last portion of the background where the mouse was
// hence clearing its trail.
//
void restoreLastMouseAreaBitmap() // sub_4B899  proc near       ; CODE XREF: start+417p handleNewPlayerOptionClick+3Fp ...
{
    for (int y = 0; y < kLastMouseCursorAreaSize; ++y)
    {
        for (int x = 0; x < kLastMouseCursorAreaSize; ++x)
        {
            uint32_t destAddress = y * kLastMouseCursorAreaSize + x;
            uint32_t sourceAddress = (gLastMouseCursorOriginAddress
                                      + y * kScreenWidth
                                      + x);

            gScreenPixels[sourceAddress] = gLastMouseCursorAreaBitmap[destAddress];
        }
    }
}

void drawMouseCursor() // sub_4B8BE  proc near       ; CODE XREF: handleNewPlayerOptionClick+FFp
//                    ; handleNewPlayerOptionClick+127p ...
{
    // gFrameCounter = some kind of counter for animations?
    uint8_t frameNumber = (gFrameCounter / 4) % 8;
    uint8_t frameRow = frameNumber / 4;
    uint8_t frameColumn = frameNumber % 4;

    static const uint8_t kMouseCursorSize = 8;
    static const uint32_t kMouseCursorOriginY = 445;

    // In the original code, the address below was just 0x1520 (+ the offset for the current frame)
    const uint32_t kStartMouseCursorAddr = ((kMouseCursorOriginY * kMovingBitmapWidth) // First pixel of the cursor frames in MOVING.DAT
                                            + (frameRow * (kMouseCursorSize + 1) * kMovingBitmapWidth) // Y offset depending on the row in which the frame we're gonna draw is. The +1 to the size is because there is 1 pixel separation between rows :shrug:
                                            + (frameColumn * kMouseCursorSize)); // X offset depending on the column where the frame is

    for (int y = 0; y < kMouseCursorSize; ++y)
    {
        for (int x = 0; x < kMouseCursorSize; ++x)
        {
            uint8_t sourcePixel = gMovingDecodedBitmapData[kStartMouseCursorAddr + y * kMovingBitmapWidth + x];

            // Only draw non-0 pixels
            if (sourcePixel > 0)
            {
                gScreenPixels[gLastMouseCursorOriginAddress + y * kScreenWidth + x] = sourcePixel;
            }
        }
    }
}

void drawMenuBackground() //   proc near       ; CODE XREF: sub_4C407+14p
                    // ; sub_4C407:scrollLeftToMainMenup ...
{
    for (int y = 0; y < kScreenHeight; y++)
    {
//        loc_4C63E:             // ; CODE XREF: drawMenuBackground+4Dj
        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_4C641:             // ; CODE XREF: drawMenuBackground+47j
            uint32_t destPixelAddress = y * kScreenWidth + x;

            uint32_t sourcePixelAddress = y * kScreenWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (gMenuBitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (gMenuBitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (gMenuBitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (gMenuBitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            gScreenPixels[destPixelAddress] = finalColor;
        }
    }
}

void drawOptionsBackground(uint8_t *dest) // vgaloadcontrolsseg
{
    drawFullScreenBitmap(gControlsBitmapData, dest);
}

void drawBackBackground() // vgaloadbackseg
{
    drawFullScreenBitmap(gBackBitmapData, gScreenPixels);
}

void drawGfxTutorBackground(uint8_t *dest) // vgaloadgfxseg
{
    drawFullScreenBitmap(gGfxBitmapData, dest);
}

void drawFullScreenBitmap(uint8_t *bitmapData, uint8_t *dest)
{
    for (int y = 0; y < kScreenHeight; ++y)
    {
        for (int x = 0; x < kScreenWidth; ++x)
        {
            uint32_t destPixelAddress = y * kScreenWidth + x;

            uint32_t sourcePixelAddress = y * kScreenWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            dest[destPixelAddress] = finalColor;
        }
    }
}

// Draws the fixed stuff from the level (edges of the screen + tiles from FIXED.DAT)
void drawFixedLevel() // sub_48F6D   proc near       ; CODE XREF: start+335p runLevel+AAp ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:230A

    static const uint16_t kMovingBitmapTopLeftCornerX = 288;
    static const uint16_t kMovingBitmapTopLeftCornerY = 388;
    static const uint16_t kMovingBitmapTopRightCornerX = 296;
    static const uint16_t kMovingBitmapTopRightCornerY = 388;
    static const uint16_t kMovingBitmapBottomRightCornerX = 296;
    static const uint16_t kMovingBitmapBottomRightCornerY = 396;
    static const uint16_t kMovingBitmapBottomLeftCornerX = 288;
    static const uint16_t kMovingBitmapBottomLeftCornerY = 396;
    static const uint16_t kMovingBitmapTopEdgeX = 304;
    static const uint16_t kMovingBitmapTopEdgeY = 396;
    static const uint16_t kMovingBitmapRightEdgeX = 304;
    static const uint16_t kMovingBitmapRightEdgeY = 388;
    static const uint16_t kMovingBitmapBottomEdgeX = 304;
    static const uint16_t kMovingBitmapBottomEdgeY = 396;
    static const uint16_t kMovingBitmapLeftEdgeX = 312;
    static const uint16_t kMovingBitmapLeftEdgeY = 388;

    // Draws top-left corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_48F86:              ; CODE XREF: drawFixedLevel+20j
            size_t srcAddress = (kMovingBitmapTopLeftCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws top edge
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
            size_t srcAddress = (kMovingBitmapTopEdgeY + y) * kMovingBitmapWidth + kMovingBitmapTopEdgeX + (x % kLevelEdgeSize);
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Top-right corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FC8:              ; CODE XREF: drawFixedLevel+62j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapTopRightCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Right edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y % kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapRightEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapRightEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-right corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FFE:              ; CODE XREF: drawFixedLevel+98j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomRightCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom edge
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
//loc_4901C:              ; CODE XREF: drawFixedLevel+BDj
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_4901F:              ; CODE XREF: drawFixedLevel+B4j
            int srcX = x % kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws left edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_49047:              ; CODE XREF: drawFixedLevel+EBj
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_4904A:              ; CODE XREF: drawFixedLevel+E4j
            int srcY = y % kLevelEdgeSize;

            size_t srcAddress = (kMovingBitmapLeftEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapLeftEdgeX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-left corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_49067:              ; CODE XREF: drawFixedLevel+101j
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomLeftCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    for (int tileY = 1; tileY < kLevelHeight - 1; ++tileY)
    {
        for (int tileX = 1; tileX < kLevelWidth - 1; ++tileX)
        {
            int bitmapTileX = tileX - 1;
            int bitmapTileY = tileY - 1;

            size_t startDstX = kLevelEdgeSize + bitmapTileX * kTileSize;
            size_t startDstY = kLevelEdgeSize + bitmapTileY * kTileSize;
            uint16_t tileValue = gCurrentLevelState[tileY * kLevelWidth + tileX].tile;

            // Tile values greater than the official ones (including 40, the invisible wall) will be rendered as empty
            // spaces, to prevent issues even with custom graphics.
            //
            if (tileValue >= LevelTileTypeCount)
            {
                tileValue = LevelTileTypeSpace;
            }

            size_t startSrcX = tileValue * kTileSize;

            for (int y = 0; y < kTileSize; ++y)
            {
                for (int x = 0; x < kTileSize; ++x)
                {
                    size_t dstAddress = (startDstY + y) * kLevelBitmapWidth + startDstX + x;
                    size_t srcAddress = (y * kFixedBitmapWidth) + startSrcX + x;
                    gLevelBitmapData[dstAddress] = gFixedDecodedBitmapData[srcAddress];
                }
            }
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
        int16_t finalY = dstY + y - kLevelEdgeSize;

        if (finalY < 0 || finalY >= kLevelBitmapHeight)
        {
            continue;
        }

        int16_t finalX = dstX - kLevelEdgeSize;
        int16_t finalWidth = MIN(width, kLevelBitmapWidth - finalX);
        size_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX;
        size_t dstAddress = finalY * kLevelBitmapWidth + finalX;

        memcpy(&gLevelBitmapData[dstAddress], &gMovingDecodedBitmapData[srcAddress], finalWidth);
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

void scrollTerminalScreen(int16_t position)
{
    // From this point it's all rendering, nothing needed in ultra fast mode
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // This code basically simulates a scroll effect in the terminal:
    // copies the row 2 in the row 10, and then row 3 in 2, 4 in 3...
    //
    uint16_t tileX = (position % kLevelWidth) - 1;
    uint16_t tileY = (position / kLevelWidth) - 1;

    uint16_t x = kLevelEdgeSize + tileX * kTileSize;
    uint16_t y = kLevelEdgeSize + tileY * kTileSize;

    uint32_t source = 0;
    uint32_t dest = 0;

    source = dest = y * kLevelBitmapWidth + x;

    dest += kLevelBitmapWidth * 10;
    source += kLevelBitmapWidth * 2;
    memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);

    dest -= kLevelBitmapWidth * 8;
    source += kLevelBitmapWidth;

    for (int i = 0; i < 9; ++i)
    {
        memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);
        dest += kLevelBitmapWidth;
        source += kLevelBitmapWidth;
    }
}

void clearGamePanel()
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    memcpy(&gPanelRenderedBitmapData, &gPanelDecodedBitmapData, sizeof(gPanelRenderedBitmapData));
}

void clearAdditionalInfoInGamePanel() // loc_4FD7D:              ; CODE XREF: clearAdditionalInfoInGamePanelIfNeeded+12j
{
    // Only draws 7 pixel height? That sprite is 8 pixel height.
    // (A few days later...) it's 7 because this function just clears the text written
    // in drawNumberOfRemainingRedDisks, and the text is just 7 pixel height, so no need for the 8th line.
    //
    uint8_t spriteHeight = 7;

    uint16_t srcX = 272;
    uint16_t srcY = 388;

    uint16_t dstX = 304;
    uint16_t dstY = 14;

//loc_4FD99:              ; CODE XREF: clearAdditionalInfoInGamePanelIfNeeded+3Cj
    for (int y = 0; y < spriteHeight; ++y)
    {
        uint32_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX;
        uint32_t dstAddress = (dstY + y) * kScreenWidth + dstX;
        memcpy(&gPanelRenderedBitmapData[dstAddress], &gMovingDecodedBitmapData[srcAddress], kTileSize);
    }
}

uint8_t gAdvancedOptionsMenuBaseBitmap[kFullScreenFramebufferLength];

void saveScreenForAdvancedMenu()
{
    memcpy(gAdvancedOptionsMenuBaseBitmap, gScreenPixels, sizeof(gAdvancedOptionsMenuBaseBitmap));
}

void restoreScreenFromAdvancedMenu()
{
    memcpy(gScreenPixels, gAdvancedOptionsMenuBaseBitmap, sizeof(gAdvancedOptionsMenuBaseBitmap));
}

void renderAdvancedOptionsMenu(AdvancedOptionsMenu *menu)
{
    memcpy(gScreenPixels, gAdvancedOptionsMenuBaseBitmap, sizeof(gAdvancedOptionsMenuBaseBitmap));

    char entryTitleBuffer[kMaxAdvancedOptionsMenuEntryTitleLength];

    static const uint16_t kInitialMenuX = 104;
    static const uint16_t kInitialMenuY = 16;
    static const uint16_t kLinesBelowTitle = 2;

    static const uint8_t kTitleTextColorIndex = 253;
    static const uint8_t kEntryTextColorIndex = 254;
    static const uint8_t kSelectedEntryTextColorIndex = 255;

    setGlobalPaletteColor(kTitleTextColorIndex, (Color) { 255, 255, 255 });
    setGlobalPaletteColor(kEntryTextColorIndex, (Color) { 180, 180, 180 });
    setGlobalPaletteColor(kSelectedEntryTextColorIndex, (Color) { 224, 16, 16 });

    const uint16_t kMenuTitleX = (kScreenWidth - strlen(menu->title) * kBitmapFontCharacter6Width) / 2;

    drawTextWithChars6FontWithTransparentBackground(kMenuTitleX, kInitialMenuY, kTitleTextColorIndex, menu->title);

    for (int i = 0; i < menu->numberOfEntries; ++i)
    {
        AdvancedOptionsMenuEntry entries = menu->entries[i];

        uint8_t color = (i == menu->selectedEntryIndex
                         ? kSelectedEntryTextColorIndex
                         : kEntryTextColorIndex);

        char *title = entries.title;

        if (entries.titleBuilder)
        {
            entries.titleBuilder(entryTitleBuffer);
            title = entryTitleBuffer;
        }

        drawTextWithChars6FontWithTransparentBackground(kInitialMenuX, kInitialMenuY + (i + kLinesBelowTitle) * (kBitmapFontCharacterHeight + 1), color, title);
    }

    videoLoop();
}

void drawTextWithChars6FontWithOpaqueBackground(size_t destX, size_t destY, uint8_t color, const char *text) // loc_4BA8D:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackgroundIfPossible:loc_4BDECj
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    if (text[0] == '\0')
    {
        return;
    }

//loc_4BA9F:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackgroundIfPossible+3Bj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

//loc_4BA97:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackgroundIfPossible+33j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacter6Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter6Width + destX + x);
                gScreenPixels[destAddress] = color * pixelValue;
            }
        }
    }
}

void drawTextWithChars6FontWithTransparentBackground(size_t destX, size_t destY, uint8_t color, const char *text)  // loc_4BE1E:             // ; CODE XREF: drawTextWithChars6FontWithTransparentBackgroundIfPossible:loc_4BF46j
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    if (text[0] == '\0')
    {
        return;
    }

//loc_4BE30:             // ; CODE XREF: drawTextWithChars6FontWithTransparentBackgroundIfPossible+3Bj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

//loc_4BA97:             // ; CODE XREF: drawTextWithChars6FontWithOpaqueBackgroundIfPossible+33j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacter6Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars6BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                if (pixelValue == 1)
                {
                    // 6 is the wide (in pixels) of this font
                    size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter6Width + destX + x);
                    gScreenPixels[destAddress] = color;
                }
            }
        }
    }
}

void drawTextWithChars8FontToBuffer(uint8_t *buffer, size_t destX, size_t destY, uint8_t color, const char *text)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // Parameters:
    // - di is the destination surface
    // - si is the text to be rendered
    // - ah is the color index in the current palette

//loc_4FEBE:              ; CODE XREF: drawTextWithChars8Font_method1+1C3j
    if (text[0] == '\0')
    {
        return;
    }

//loc_4FED0:              ; CODE XREF: drawTextWithChars8Font_method1+2Fj
    long textLength = strlen(text);

    for (long idx = 0; idx < textLength; ++idx)
    {
        char character = text[idx];

//loc_4FEC8:              ; CODE XREF: drawTextWithChars8Font_method1+27j
        if (character == '\n')
        {
            return;
        }

        // ' ' = 0x20 = 32, and is first ascii that can be represented.
        // This line converts the ascii from the string to the index in the font
        //
        uint8_t bitmapCharacterIndex = character - 0x20;

        for (uint8_t y = 0; y < kBitmapFontCharacterHeight; ++y)
        {
            for (uint8_t x = 0; x < kBitmapFontCharacter8Width; ++x)
            {
                uint8_t bitmapCharacterRow = gChars8BitmapFont[bitmapCharacterIndex + y * kNumberOfCharactersInBitmapFont];
                uint8_t pixelValue = (bitmapCharacterRow >> (7 - x)) & 0x1;

                // 6 is the wide (in pixels) of this font
                size_t destAddress = (destY + y) * kScreenWidth + (idx * kBitmapFontCharacter8Width + destX + x);
                buffer[destAddress] = color * pixelValue;
            }
        }
    }
}

void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text) //   proc near       ; CODE XREF: drawTextWithChars8Font+7p
{
    drawTextWithChars8FontToBuffer(gScreenPixels, destX, destY, color, text);
}

void drawTextWithChars8FontToGamePanel(size_t destX, size_t destY, uint8_t color, const char *text)
{
    drawTextWithChars8FontToBuffer(gPanelRenderedBitmapData, destX, destY, color, text);
}

void limitFPS()
{
    #define kMaximumFPS 70.0
    static const double kFrameDuration = 1000.0 / kMaximumFPS;
    static uint32_t sLastFrameTime = 0;

    if (gFastMode != FastModeTypeNone)
    {
        return;
    }

    if (sLastFrameTime != 0)
    {
        uint32_t duration = (getTime() - sLastFrameTime);
        if (duration < kFrameDuration)
        {
            waitTime(kFrameDuration - duration);
        }
    }

    sLastFrameTime = getTime();
}

void videoLoop() //   proc near       ; CODE XREF: crt?2+52p crt?1+3Ep ...
{
    if (gShouldShowFPS)
    {
        char frameRateString[5] = "";
        sprintf(frameRateString, "%4.1f", MIN(gFrameRate, 99.9)); // Don't show more than 99.9 FPS, not necessary

        drawTextWithChars6FontWithOpaqueBackground(0, 0, 6, frameRateString);
    }

    handleSystemEvents(); // Make sure the app stays responsive

    render();
    present();

    if (gShouldLimitFPS)
    {
        limitFPS();
    }

    static uint32_t sNumberOfFrames = 0;

    sNumberOfFrames++;

    if (gFrameRateReferenceTime == 0)
    {
        gFrameRateReferenceTime = getTime();
    }
    else
    {
        uint32_t difference = getTime() - gFrameRateReferenceTime;

        if (difference > 1000)
        {
            gFrameRate = sNumberOfFrames * 1000.f / difference;
            sNumberOfFrames = 0;
            gFrameRateReferenceTime = getTime();
        }
    }
}

void convertPaletteDataToPalette(ColorPaletteData paletteData, ColorPalette outPalette)
{
    int kExponent = 4; // no idea why (yet)

    for (int i = 0; i < kNumberOfColors; ++i)
    {
        outPalette[i].r = paletteData[i * 4 + 0] << kExponent;
        outPalette[i].g = paletteData[i * 4 + 1] << kExponent;
        outPalette[i].b = paletteData[i * 4 + 2] << kExponent;
        outPalette[i].a = paletteData[i * 4 + 3] << kExponent; // intensity, for now
    }
}

void readPalettes()  // proc near       ; CODE XREF: readEverythingp
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("PALETTES.DAT", "rb");
    if (file == NULL)
    {
        exitWithError("Error opening PALETTES.DAT\n");
    }

//loc_4779F:              // ; CODE XREF: readPalettes+8j
    ColorPaletteData palettesDataBuffer[kNumberOfPalettes];
    size_t bytes = fileReadBytes(palettesDataBuffer, sizeof(ColorPaletteData) * kNumberOfPalettes, file);
    if (bytes == 0)
    {
        exitWithError("Couldn't read PALETTES.DAT\n");
    }

    for (int i = 0; i < kNumberOfPalettes; ++i)
    {
        convertPaletteDataToPalette(palettesDataBuffer[i], gPalettes[i]);
    }

//loc_477B6:              //; CODE XREF: readPalettes+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PALETTES.DAT\n");
    }
}

void fadeToPalette(ColorPalette palette) //        proc near       ; CODE XREF: start+2C1p start+312p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        setPalette(palette);
        return;
    }

    // Parameters:
    // si -> points to the first color of the palette to fade to

    ColorPalette intermediatePalette;

    // The original animation had 64 steps, and the game was written to run in 70Hz displays
    static const uint32_t kFadeDuration = 64 * 1000 / 70; // ~914 ms
    uint32_t fadeTime = 0;

    startTrackingRenderDeltaTime();

    // for (uint8_t step = 0; step < totalSteps; ++step)
    while (fadeTime < kFadeDuration)
    {
        fadeTime += updateRenderDeltaTime();
        fadeTime = MIN(fadeTime, kFadeDuration);

        float animationFactor = (float)fadeTime / kFadeDuration;
        float complementaryAnimationFactor = 1.0 - animationFactor;

        for (uint8_t i = 0; i < kNumberOfColors; ++i)
        {
            uint8_t r = (palette[i].r * animationFactor) + (gCurrentPalette[i].r * complementaryAnimationFactor);
            uint8_t g = (palette[i].g * animationFactor) + (gCurrentPalette[i].g * complementaryAnimationFactor);
            uint8_t b = (palette[i].b * animationFactor) + (gCurrentPalette[i].b * complementaryAnimationFactor);

            intermediatePalette[i] = (Color) { r, g, b, 255};
        }

        setColorPalette(intermediatePalette);

        videoLoop();
    }

    setPalette(palette);
}

void replaceCurrentPaletteColor(uint8_t index, Color color)
{
    gCurrentPalette[index] = color;
    setPalette(gCurrentPalette);
}

void setPalette(ColorPalette palette) // sub_4D836   proc near       ; CODE XREF: start+2B8p
                   // ; loadScreen2+B5p ...
{
    setColorPalette(palette);
    memcpy(gCurrentPalette, palette, sizeof(ColorPalette));
}

void startTrackingRenderDeltaTime()
{
    gRenderDeltaTime = getTime();
}

uint32_t updateRenderDeltaTime()
{
    uint32_t duration = getTime() - gRenderDeltaTime;
    gRenderDeltaTime = getTime();
    return duration;
}

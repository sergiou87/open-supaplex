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

#ifndef graphics_h
#define graphics_h

#include <stdint.h>

#include "globals.h"
#include "video.h"

#define kPaleteDataSize (kNumberOfColors * 4)
#define kNumberOfPalettes 4

typedef uint8_t ColorPaletteData[kPaleteDataSize];

extern ColorPalette gCurrentPalette;

// Game palettes:
// - 0: level credits
// - 1: main menu
// - 2: ???
// - 3: ???
//
extern ColorPalette gPalettes[kNumberOfPalettes];
extern ColorPalette gBlackPalette; // 60D5h

#define gInformationScreenPalette gPalettes[0]
#define gGamePalette gPalettes[1] // 0x6015
#define gControlsScreenPalette gPalettes[2] // 0x6055
#define gGameDimmedPalette gPalettes[3] // 6095h

extern ColorPaletteData gTitlePaletteData;
extern ColorPaletteData gTitle1PaletteData;
extern ColorPaletteData gTitle2PaletteData;

#define kBitmapFontCharacterHeight 7
#define kBitmapFontCharacter6Width 6

// This is a 320x24 bitmap
#define kPanelBitmapWidth 320
#define kPanelBitmapHeight 24
extern uint8_t gCurrentPanelHeight;

#define kFullScreenFramebufferLength (kScreenWidth * kScreenHeight) // We only use 16 colors, but SDL doesn't support that mode, so we use 256 colors

// This buffer has the contents of TITLE2.DAT after it's been "decoded" (i.e. after picking the different channels
// every 40 bytes and forming the 4 bit palette index for each pixel).
//
extern uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];

extern uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];

#define kLevelEdgeSize 8
#define kTileSize 16
#define kLevelBitmapWidth (kTileSize * (kLevelWidth - 2) + kLevelEdgeSize + kLevelEdgeSize)
#define kLevelBitmapHeight (kTileSize * (kLevelHeight - 2) + kLevelEdgeSize + kLevelEdgeSize)

// Time difference between 2 consecutive renders
extern uint32_t gRenderDeltaTime;

void readMenuDat(void);
void loadMurphySprites(void);
void readPanelDat(void);
void readBackDat(void);
void readBitmapFonts(void);
void readAndRenderTitleDat(void);
void readAndRenderTitle1Dat(void);
void readTitle2Dat(void);
void readGfxDat(void);
void readControlsDat(void);

void saveLastMouseAreaBitmap(void);
void restoreLastMouseAreaBitmap(void);
void drawMouseCursor(void);
void drawMenuBackground(void);
void drawOptionsBackground(uint8_t *dest);
void drawBackBackground(void);
void drawGfxTutorBackground(uint8_t *dest);
void drawFixedLevel(void);
void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawCurrentLevelViewport(uint16_t panelHeight);
void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, int16_t dstX, int16_t dstY);
void drawMovingFrame(uint16_t srcX, uint16_t srcY, uint16_t destPosition);

void scrollTerminalScreen(int16_t position);

void clearGamePanel(void);
void clearAdditionalInfoInGamePanel(void);

void drawTextWithChars6FontWithOpaqueBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars6FontWithTransparentBackground(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8Font(size_t destX, size_t destY, uint8_t color, const char *text);
void drawTextWithChars8FontToGamePanel(size_t destX, size_t destY, uint8_t color, const char *text);

void videoLoop(void);

void readPalettes(void);
void replaceCurrentPaletteColor(uint8_t index, Color color);
void setPalette(ColorPalette palette);
void fadeToPalette(ColorPalette palette);
void convertPaletteDataToPalette(ColorPaletteData paletteData, ColorPalette outPalette);

void startTrackingRenderDeltaTime(void);
uint32_t updateRenderDeltaTime(void);

#endif /* graphics_h */

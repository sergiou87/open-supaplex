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

#define kBitmapFontCharacterHeight 7
#define kBitmapFontCharacter6Width 6
#define kBitmapFontCharacter8Width 8
#define kNumberOfCharactersInBitmapFont 64
#define kBitmapFontLength (kNumberOfCharactersInBitmapFont * 8) // The size of the bitmap is a round number, not 100% related to the size font, there is some padding :shrug:

extern uint8_t gChars6BitmapFont[kBitmapFontLength];
extern uint8_t gChars8BitmapFont[kBitmapFontLength];

#define kFixedBitmapWidth 640
#define kFixedBitmapHeight 16
extern uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];

#define kMovingBitmapWidth 320
#define kMovingBitmapHeight 462
extern uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];

// This is a 320x24 bitmap
#define kPanelBitmapWidth 320
#define kPanelBitmapHeight 24

extern uint8_t gPanelDecodedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
extern uint8_t gPanelRenderedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
extern uint8_t gCurrentPanelHeight;

#define kFullScreenBitmapLength (kScreenWidth * kScreenHeight / 2) // They use 4 bits to encode pixels

// These buffers contain the raw bitmap data of the corresponding DAT files, separated in 4 bitmaps:
//  - The bitmap has 4 columns of 40 bytes width x 200 bytes height
//  - Every column represents a component (R, G, B and intensity).
//  - In one of these columns/components, every bit represents a pixel (40 bytes x 8 bits = 320 pixels)
//  - The way these are decoded seems to be to pick, for every pixel, the bit of the r column, the bit of the g column,
//    the bit from the b column and the bit from the intensity column, and create a 4 bit number that represents an index
//    in the 16 color palette.
//
extern uint8_t gMenuBitmapData[kFullScreenBitmapLength]; // 0x4D34 ??
extern uint8_t gControlsBitmapData[kFullScreenBitmapLength];
extern uint8_t gBackBitmapData[kFullScreenBitmapLength];
extern uint8_t gGfxBitmapData[kFullScreenBitmapLength];

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
extern uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

void readMenuDat(void);

void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawCurrentLevelViewport(uint16_t panelHeight);
void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, int16_t dstX, int16_t dstY);

#endif /* graphics_h */

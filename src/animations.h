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

#ifndef animations_h
#define animations_h

#include <stdint.h>

typedef struct {
    // TODO: migrate offset to a (X, Y) coordinate system or to at least something that takes 320 pixel width screen
    // into account instead of the 122 positions of the original game
    int16_t animationCoordinatesOffset; // word_510F0 -> 0x0DE0 -> seems like an offset from the destination position in dimensions of the original game screen (meaning, you have to divide by 122 to get the Y coordinate, and get module 122 and multiply by 8 to get the X coordinate)
    int16_t animationCoordinatesOffsetIncrement; // word_510F2 -> this increases the offset above frame by frame
    uint16_t width; // word_510F4
    uint16_t height; // word_510F6
    uint16_t animationIndex; // word_510F8; -> memory address in someBinaryData_5142E, looks like a list of coordinates of frames in MOVING.DAT
    int16_t speedX; // word_510FA; -> applied to Murphy X position... speedX?
    int16_t speedY; // word_510FC; -> applied to Murphy Y position... speedY?
    uint16_t currentFrame; // Not used in the original code, I will use it to keep track of the current animation frame
} MurphyAnimationDescriptor;

extern const MurphyAnimationDescriptor kMurphyAnimationDescriptors[50];

typedef struct
{
    uint16_t x;
    uint16_t y;
} Point;

typedef struct
{
    Point coordinates[105];
    uint8_t numberOfCoordinates;
} AnimationFrameCoordinates;

extern const AnimationFrameCoordinates kMurphyAnimationFrameCoordinates[37];
extern const Point kBugFrameCoordinates[16]; // binaryData_51582
extern Point kZonkSlideLeftAnimationFrameCoordinates[8]; // binaryData_515A4
extern const Point kZonkSlideRightAnimationFrameCoordinates[8]; // binaryData_515B4
extern const Point kInfotronSlideLeftAnimationFrameCoordinates[8];
extern const Point kInfotronSlideRightAnimationFrameCoordinates[8];
extern const Point kRegularExplosionAnimationFrameCoordinates[8];
extern const Point kInfotronExplosionAnimationFrameCoordinates[8];
extern const Point kSnikSnakAnimationFrameCoordinates[48];
extern const Point kElectronAnimationFrameCoordinates[48];

#endif /* animations_h */

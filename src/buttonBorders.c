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

#include "buttonBorders.h"

const ButtonBorderDescriptor kOptionsMenuBorders[kNumberOfOptionsMenuBorders] = {
    { // 0: 130
        {
            { ButtonBorderLineTypeVertical, 144, 81, 5 },
            { ButtonBorderLineTypeHorizontal, 127, 76, 18 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 122, 71, 5 },
            { ButtonBorderLineTypeVertical, 122, 70, 69 },
            { ButtonBorderLineTypeHorizontal, 76, 2, 46 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 70, 8, 7 },
        },
        6
    },
    { // 1: 161
        {
            { ButtonBorderLineTypeHorizontal, 103, 39, 16 },
            { ButtonBorderLineTypeVertical, 118, 77, 38 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 119, 78, 6 },
            { ButtonBorderLineTypeVertical, 124, 85, 2 },
            { ButtonBorderLineTypeVertical, 124, 112, 8 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 121, 115, 3 },
            { ButtonBorderLineTypeVertical, 120, 117, 2 },
            { ButtonBorderLineTypeHorizontal, 121, 126, 24 },
            { ButtonBorderLineTypeVertical, 144, 127, 2 },
        },
        9
    },
    { // 2: 1A7
        {
            { ButtonBorderLineTypeVertical, 94, 44, 3 },
            { ButtonBorderLineTypeHorizontal, 94, 42, 19 },
            { ButtonBorderLineTypeVertical, 113, 81, 40 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 114, 82, 14 },
        },
        4
    },
    { // 3: 1CA
        {
            { ButtonBorderLineTypeVertical, 94, 78, 2 },
            { ButtonBorderLineTypeHorizontal, 95, 78, 11 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 106, 79, 9 },
            { ButtonBorderLineTypeVertical, 115, 129, 42 },
            { ButtonBorderLineTypeHorizontal, 116, 129, 20 },
        },
        5
    },
    { // 4: 01F4
        {
            { ButtonBorderLineTypeVertical, 88, 115, 3 },
            { ButtonBorderLineTypeHorizontal, 89, 115, 23 },
            { ButtonBorderLineTypeVertical, 112, 115, 3 },
            { ButtonBorderLineTypeVertical, 119, 94, 2 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 120, 93, 8 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 120, 94, 8 },
        },
        6
    },
    { // 5: 225
        {
            { ButtonBorderLineTypeVertical, 22, 114, 2 },
            { ButtonBorderLineTypeHorizontal, 11, 114, 11 },
            { ButtonBorderLineTypeVertical, 10, 151, 38 },
            { ButtonBorderLineTypeHorizontal, 6, 151, 4 },
            { ButtonBorderLineTypeVertical, 5, 156, 6 },
            { ButtonBorderLineTypeVertical, 14, 155, 3 },
            { ButtonBorderLineTypeHorizontal, 15, 153, 6 },
            { ButtonBorderLineTypeVertical, 21, 160, 8 },
            { ButtonBorderLineTypeHorizontal, 22, 160, 20 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 42, 159, 9 },
            { ButtonBorderLineTypeHorizontal, 51, 151, 74 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 125, 152, 10 },
            { ButtonBorderLineTypeHorizontal, 135, 161, 9 },
            { ButtonBorderLineTypeVertical, 144, 161, 5 },
        },
        14
    },
    { // 6: 28E
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 34, 113, 5 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 70, 116, 3 },
            { ButtonBorderLineTypeVertical, 73, 113, 5 },
            { ButtonBorderLineTypeVertical, 73, 84, 7 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 72, 76, 2 },
            { ButtonBorderLineTypeHorizontal, 9, 111, 5 },
            { ButtonBorderLineTypeHorizontal, 9, 112, 6 },
            { ButtonBorderLineTypeVertical, 7, 112, 68 },
            { ButtonBorderLineTypeVertical, 8, 112, 68 },
            { ButtonBorderLineTypeHorizontal, 9, 45, 6 },
            { ButtonBorderLineTypeHorizontal, 9, 46, 5 },
        },
        11
    },
    { // 7: 2E2
        {
            { ButtonBorderLineTypeVertical, 138, 37, 2 },
            { ButtonBorderLineTypeHorizontal, 125, 37, 13 },
            { ButtonBorderLineTypeVertical, 124, 67, 31 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 125, 68, 6 },
            { ButtonBorderLineTypeHorizontal, 131, 73, 19 },
            { ButtonBorderLineTypeVertical, 150, 81, 9 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 151, 36, 4 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 182, 39, 4 },
            { ButtonBorderLineTypeVertical, 192, 38, 3 },
        },
        9
    },
    { // 8: 328
        {
            { ButtonBorderLineTypeVertical, 156, 65, 7 },
            { ButtonBorderLineTypeVertical, 156, 69, 2 },
            { ButtonBorderLineTypeVertical, 157, 69, 2 },
            { ButtonBorderLineTypeHorizontal, 182, 71, 2 },
            { ButtonBorderLineTypeVertical, 183, 85, 14 },
            { ButtonBorderLineTypeVertical, 183, 102, 7 },
            { ButtonBorderLineTypeVertical, 183, 124, 13 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 168, 139, 15 },
            { ButtonBorderLineTypeHorizontal, 165, 139, 3 },
        },
        9
    },
    { // 9: 36E
        {
            { ButtonBorderLineTypeVertical, 180, 65, 7 },
            { ButtonBorderLineTypeVertical, 180, 69, 2 },
            { ButtonBorderLineTypeVertical, 181, 69, 2 },
            { ButtonBorderLineTypeHorizontal, 182, 71, 2 },
            { ButtonBorderLineTypeVertical, 183, 85, 14 },
            { ButtonBorderLineTypeVertical, 183, 102, 7 },
            { ButtonBorderLineTypeVertical, 183, 124, 13 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 168, 139, 15 },
            { ButtonBorderLineTypeHorizontal, 165, 139, 3 },
        },
        9
    },
    { // 10: 3B4
        {
            { ButtonBorderLineTypeHorizontal, 180, 90, 6 },
            { ButtonBorderLineTypeVertical, 186, 90, 24 },
            { ButtonBorderLineTypeHorizontal, 187, 67, 38 },
            { ButtonBorderLineTypeVertical, 225, 67, 33 },
            { ButtonBorderLineTypeHorizontal, 226, 35, 33 },
        },
        5
    },
    { // 11: 3DE
        {
            { ButtonBorderLineTypeVertical, 150, 160, 4 },
            { ButtonBorderLineTypeHorizontal, 150, 161, 31 },
            { ButtonBorderLineTypeVertical, 181, 161, 29 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 181, 133, 8 },
            { ButtonBorderLineTypeVertical, 189, 125, 22 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 189, 103, 4 },
            { ButtonBorderLineTypeVertical, 192, 99, 25 },
            { ButtonBorderLineTypeHorizontal, 192, 74, 51 },
            { ButtonBorderLineTypeBottomLeftToTopRightDiagonal, 243, 74, 17 },
        },
        9
    },
    { // 12: 424
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 126, 10 },
        },
        1
    },
    { // 13: 432
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 131, 10 },
        },
        1
    },
    { // 14: 440
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 136, 10 },
        },
        1
    },
    { // 15: 44E
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 141, 10 },
        },
        1
    },
    { // 16: 45C
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 146, 10 },
        },
        1
    },
    { // 17: 46A
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 151, 10 },
        },
        1
    },
    { // 18: 478
        {
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 116, 4 },
            { ButtonBorderLineTypeHorizontal, 261, 119, 4 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 257, 111, 2 },
            { ButtonBorderLineTypeHorizontal, 259, 112, 7 },
            { ButtonBorderLineTypeHorizontal, 268, 113, 2 },
            { ButtonBorderLineTypeVertical, 272, 114, 2 },
            { ButtonBorderLineTypeVertical, 273, 114, 2 },
            { ButtonBorderLineTypeHorizontal, 280, 113, 7 },
            { ButtonBorderLineTypeHorizontal, 280, 114, 7 },
            { ButtonBorderLineTypeHorizontal, 283, 119, 3 },
            { ButtonBorderLineTypeHorizontal, 292, 119, 2 },
            { ButtonBorderLineTypeVertical, 293, 131, 12 },
            { ButtonBorderLineTypeHorizontal, 289, 131, 4 },
            { ButtonBorderLineTypeTopLeftToBottomRightDiagonal, 283, 126, 4 },
        },
        14
    },
    { // 19: 04E1
        {
            { ButtonBorderLineTypeVertical, 159, 180, 14 },
            { ButtonBorderLineTypeHorizontal, 160, 167, 28 },
            { ButtonBorderLineTypeVertical, 187, 166, 8 },
            { ButtonBorderLineTypeHorizontal, 187, 149, 10 },
        },
        4
    },
};

const ButtonBorderDescriptor kMainMenuButtonBorders[kNumberOfMainMenuButtonBorders] = { // starts on 0x504? or before?
    // Player List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 11, 152, 11 },
            { ButtonBorderLineTypeHorizontal, 12, 142, 56 },
        },
        2
    },
    // Player List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 12, 153, 56 },
            { ButtonBorderLineTypeVertical, 67, 153, 11 },
        },
        2
    },
    // Player List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 11, 191, 11 },
            { ButtonBorderLineTypeHorizontal, 12, 181, 56 },
        },
        2
    },
    // Player List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 12, 192, 56 },
            { ButtonBorderLineTypeVertical, 67, 192, 11 },
        },
        2
    },
    // Ranking List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 141, 105, 16 },
            { ButtonBorderLineTypeHorizontal, 142, 90, 13 },
        },
        2
    },
    // Ranking List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 141, 106, 14 },
            { ButtonBorderLineTypeVertical, 154, 105, 15 },
        },
        2
    },
    // Ranking List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 141, 135, 16 },
            { ButtonBorderLineTypeHorizontal, 142, 120, 13 },
        },
        2
    },
    // Ranking List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 141, 136, 14 },
            { ButtonBorderLineTypeVertical, 154, 135, 15 },
        },
        2
    },
    // Level List - Up Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 142, 152, 11 },
            { ButtonBorderLineTypeHorizontal, 143, 142, 163 },
        },
        2
    },
    // Level List - Up Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 143, 153, 164 },
            { ButtonBorderLineTypeVertical, 306, 152, 11 },
        },
        2
    },
    // Level List - Down Arrow - Left and Top borders
    {
        {
            { ButtonBorderLineTypeVertical, 142, 191, 11 },
            { ButtonBorderLineTypeHorizontal, 143, 181, 164 },
        },
        2
    },
    // Level List - Down Arrow - Bottom and Right borders
    {
        {
            { ButtonBorderLineTypeHorizontal, 142, 192, 164 },
            { ButtonBorderLineTypeVertical, 306, 192, 12 },
        },
        2
    },
};

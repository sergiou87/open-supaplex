/*
 * Copyright (C) 2003 Maxim Stepin ( maxst@hiend3d.com )
 *
 * Copyright (C) 2010 Cameron Zemek ( grom@zeminvaders.net)
 * Copyright (C) 2011 Francois Gannaz <mytskine@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __HQX_COMMON_H_
#define __HQX_COMMON_H_

#include <stdlib.h>
#include <stdint.h>

#define MASK_2     0x0000FF00
#define MASK_13    0x00FF00FF
#define MASK_RGB   0x00FFFFFF
#define MASK_ALPHA 0xFF000000

#define Ymask 0x00FF0000
#define Umask 0x0000FF00
#define Vmask 0x000000FF
#define trY   0x00300000
#define trU   0x00000700
#define trV   0x00000006

/*
 * OpenSupaplex: the original looks colors up in a 16777216-entry (64 MB)
 * RGBtoYUV table built by hqxInit(). That is too much memory for the browser
 * and console ports, and the game only shows 16 colors at a time, so the same
 * conversion is computed on demand and kept in a small cache instead.
 */
#define HQX_YUV_CACHE_SIZE 64

static uint32_t hqxYUVCacheKeys[HQX_YUV_CACHE_SIZE];
static uint32_t hqxYUVCacheValues[HQX_YUV_CACHE_SIZE];
static int hqxYUVCacheInitialized = 0;

static inline uint32_t rgb_to_yuv(uint32_t c)
{
    // Mask against MASK_RGB to discard the alpha channel
    c &= MASK_RGB;

    if (!hqxYUVCacheInitialized)
    {
        // Keys are masked RGB values, so this one never matches
        for (int i = 0; i < HQX_YUV_CACHE_SIZE; ++i)
        {
            hqxYUVCacheKeys[i] = 0xFFFFFFFF;
        }
        hqxYUVCacheInitialized = 1;
    }

    uint32_t slot = (c ^ (c >> 8) ^ (c >> 16)) % HQX_YUV_CACHE_SIZE;
    if (hqxYUVCacheKeys[slot] == c)
    {
        return hqxYUVCacheValues[slot];
    }

    // Same formula as the original hqxInit(), except that the U and V terms
    // are truncated to int before adding 128. The original converted negative
    // doubles straight to uint32_t, which is undefined behavior: x86 wraps
    // around as intended, but ARM clamps to 0 and spoils the comparisons.
    int r = (c & 0xFF0000) >> 16;
    int g = (c & 0x00FF00) >> 8;
    int b = c & 0x0000FF;
    uint32_t y = (uint32_t)(int)(0.299 * r + 0.587 * g + 0.114 * b);
    uint32_t u = (uint32_t)((int)(-0.169 * r - 0.331 * g + 0.5 * b) + 128);
    uint32_t v = (uint32_t)((int)(0.5 * r - 0.419 * g - 0.081 * b) + 128);
    uint32_t yuv = (y << 16) + (u << 8) + v;

    hqxYUVCacheKeys[slot] = c;
    hqxYUVCacheValues[slot] = yuv;
    return yuv;
}

/* Test if there is difference in color */
/* OpenSupaplex: the differences are cast to int explicitly. abs() already
 * received them as int, so behavior is unchanged; this silences -Wabsolute-value. */
static inline int yuv_diff(uint32_t yuv1, uint32_t yuv2) {
    return (( abs((int)((yuv1 & Ymask) - (yuv2 & Ymask))) > trY ) ||
            ( abs((int)((yuv1 & Umask) - (yuv2 & Umask))) > trU ) ||
            ( abs((int)((yuv1 & Vmask) - (yuv2 & Vmask))) > trV ) );
}

static inline int Diff(uint32_t c1, uint32_t c2)
{
    return yuv_diff(rgb_to_yuv(c1), rgb_to_yuv(c2));
}

/* Interpolate functions */
static inline uint32_t Interpolate_2(uint32_t c1, int w1, uint32_t c2, int w2, int s)
{
    if (c1 == c2) {
        return c1;
    }
    return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2) >> s) & MASK_2)	+
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2) >> s) & MASK_13);
}

static inline uint32_t Interpolate_3(uint32_t c1, int w1, uint32_t c2, int w2, uint32_t c3, int w3, int s)
{
    return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2 + ((c3 & MASK_ALPHA) >> 24) * w3) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2 + (c3 & MASK_2) * w3) >> s) & MASK_2) +
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2 + (c3 & MASK_13) * w3) >> s) & MASK_13);
}

static inline void Interp1(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*3+c2) >> 2;
    *pc = Interpolate_2(c1, 3, c2, 1, 2);
}

static inline void Interp2(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+c2+c3) >> 2;
    *pc = Interpolate_3(c1, 2, c2, 1, c3, 1, 2);
}

static inline void Interp3(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*7+c2)/8;
    *pc = Interpolate_2(c1, 7, c2, 1, 3);
}

static inline void Interp4(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+(c2+c3)*7)/16;
    *pc = Interpolate_3(c1, 2, c2, 7, c3, 7, 4);
}

static inline void Interp5(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1+c2) >> 1;
    *pc = Interpolate_2(c1, 1, c2, 1, 1);
}

static inline void Interp6(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*5+c2*2+c3)/8;
    *pc = Interpolate_3(c1, 5, c2, 2, c3, 1, 3);
}

static inline void Interp7(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*6+c2+c3)/8;
    *pc = Interpolate_3(c1, 6, c2, 1, c3, 1, 3);
}

static inline void Interp8(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*5+c2*3)/8;
    *pc = Interpolate_2(c1, 5, c2, 3, 3);
}

static inline void Interp9(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+(c2+c3)*3)/8;
    *pc = Interpolate_3(c1, 2, c2, 3, c3, 3, 3);
}

static inline void Interp10(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*14+c2+c3)/16;
    *pc = Interpolate_3(c1, 14, c2, 1, c3, 1, 4);
}

#endif

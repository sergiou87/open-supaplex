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

#ifndef utils_h
#define utils_h

#include <stdint.h>
#include <stdlib.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(v, a, b) MIN(MAX(a, v), b)
#define SWAP(x, y, __type) do { __type __temp__ = x; x = y; y = __temp__; } while (0)
#define TOGGLE_BOOL(__value) do { __value = !__value; } while (0)

void exitWithError(const char *format, ...);

// Base64
char *encodeBase64(const unsigned char *input, size_t inputLength);
int decodeBase64(const char *input, unsigned char *output, size_t outputLength);

#endif /* utils_h */

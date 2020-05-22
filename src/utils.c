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

#include "utils.h"

#include <errno.h>
#include <SDL2/SDL.h>

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void exitWithError(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    SDL_Quit();
    exit(errno);
}

size_t base64EncodedSize(size_t inputLength)
{
    size_t size = inputLength;

    if (inputLength % 3 != 0)
    {
        size += 3 - (inputLength % 3);
    }

    size /= 3;
    size *= 4;

    return size;
}

char *encodeBase64(const unsigned char *input, size_t inputLength)
{
    size_t  i;
    size_t  j;
    size_t  v;

    if (input == NULL || inputLength == 0)
    {
        return NULL;
    }

    size_t b64Length = base64EncodedSize(inputLength);
    char *output = malloc(b64Length + 1);
    output[b64Length] = '\0';

    for (i = 0, j = 0; i < inputLength; i += 3, j += 4)
    {
        v = input[i];
        v = (i + 1 < inputLength
             ? v << 8 | input[i + 1]
             : v << 8);
        v = (i + 2 < inputLength
             ? v << 8 | input[i + 2]
             : v << 8);

        output[j] = b64chars[(v >> 18) & 0x3F];
        output[j + 1] = b64chars[(v >> 12) & 0x3F];

        if (i + 1 < inputLength)
        {
            output[j + 2] = b64chars[(v >> 6) & 0x3F];
        }
        else
        {
            output[j + 2] = '=';
        }
        if (i + 2 < inputLength)
        {
            output[j + 3] = b64chars[v & 0x3F];
        }
        else
        {
            output[j + 3] = '=';
        }
    }

    return output;
}

int kBase64InverseTable[] = {
    62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
    59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51
};

size_t base64DecodedSize(const char *input)
{
    size_t inputLength;
    size_t decodedLength;
    size_t i;

    if (input == NULL)
    {
        return 0;
    }

    inputLength = strlen(input);
    decodedLength = inputLength / 4 * 3;

    for (i = inputLength; i-- > 0; )
    {
        if (input[i] == '=')
        {
            decodedLength--;
        }
        else
        {
            break;
        }
    }

    return decodedLength;
}

int isValidBase64Char(char c)
{
    if (c >= '0' && c <= '9')
    {
        return 1;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        return 1;
    }
    else if (c >= 'a' && c <= 'z')
    {
        return 1;
    }
    else if (c == '+' || c == '/' || c == '=')
    {
        return 1;
    }

    return 0;
}

int decodeBase64(const char *input, unsigned char *output, size_t outputLength)
{
    size_t i;
    size_t j;
    int v;

    if (input == NULL || output == NULL)
    {
        return 0;
    }

    size_t inputLength = strlen(input);
    if (outputLength < base64DecodedSize(input) || inputLength % 4 != 0)
    {
        return 0;
    }

    for (i = 0; i < inputLength; i++)
    {
        if (isValidBase64Char(input[i]) == 0)
        {
            return 0;
        }
    }

    for (i = 0, j = 0; i < inputLength; i += 4, j += 3)
    {
        v = kBase64InverseTable[input[i] - 43];
        v = (v << 6) | kBase64InverseTable[input[i + 1] - 43];
        v = (input[i + 2] == '='
             ? v << 6
             : (v << 6) | kBase64InverseTable[input[i + 2] - 43]);
        v = (input[i + 3] == '='
             ? v << 6
             : (v << 6) | kBase64InverseTable[input[i + 3] - 43]);

        output[j] = (v >> 16) & 0xFF;
        if (input[i + 2] != '=')
        {
            output[j + 1] = (v >> 8) & 0xFF;
        }
        if (input[i + 3] != '=')
        {
            output[j + 2] = v & 0xFF;
        }
    }

    return 1;
}

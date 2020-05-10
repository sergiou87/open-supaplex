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

#ifndef file_h
#define file_h

#include <stdio.h>

// Some platforms (like PS Vita) won't be able to write in the same folder where the original resources were
// installed. For those original resources bundled with the game, the openReadonlyFile function will be used.
// Other files will be opened with openWritableFile.
//
#define kMaxFilePathLength 256

void getReadonlyFilePath(const char *pathname, char outPath[kMaxFilePathLength]);
void getWritableFilePath(const char *pathname, char outPath[kMaxFilePathLength]);

FILE *openReadonlyFile(const char *pathname, const char *mode);
FILE *openWritableFile(const char *pathname, const char *mode);
FILE *openWritableFileWithReadonlyFallback(const char *pathname, const char *mode);

#endif /* file_h */

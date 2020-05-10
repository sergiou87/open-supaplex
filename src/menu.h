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

#ifndef menu_h
#define menu_h

#include <stdint.h>

#define kMaxAdvancedOptionsMenuEntryTitleLength 50

typedef void (*AdvancedOptionsMenuEntryTitleBuilder)(char output[kMaxAdvancedOptionsMenuEntryTitleLength]);
typedef void (*AdvancedOptionsMenuEntryActionHandler)(void);

typedef struct {
    char title[kMaxAdvancedOptionsMenuEntryTitleLength];
    AdvancedOptionsMenuEntryTitleBuilder titleBuilder;
    AdvancedOptionsMenuEntryActionHandler selectionHandler;
    AdvancedOptionsMenuEntryActionHandler decrementHandler;
    AdvancedOptionsMenuEntryActionHandler incrementHandler;
} AdvancedOptionsMenuEntry;

#endif /* menu_h */

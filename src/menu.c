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

#include "menu.h"

#include <assert.h>

void initializeAdvancedOptionsMenu(AdvancedOptionsMenu *menu)
{
    menu->numberOfEntries = 0;
    menu->selectedEntryIndex = 0;
}

void addAdvancedOptionsEntry(AdvancedOptionsMenu *menu,
                             AdvancedOptionsMenuEntry entry)
{
    assert(menu->numberOfEntries < kMaxAdvancedOptionsMenuEntries);

    menu->entries[menu->numberOfEntries] = entry;
    menu->numberOfEntries++;
}

void moveUpAdvancedOptionsSelectedEntry(AdvancedOptionsMenu *menu)
{
    if (menu->selectedEntryIndex == 0)
    {
        menu->selectedEntryIndex = menu->numberOfEntries - 1;
    }
    else
    {
        menu->selectedEntryIndex--;
    }
}

void moveDownAdvancedOptionsSelectedEntry(AdvancedOptionsMenu *menu)
{
    menu->selectedEntryIndex = (menu->selectedEntryIndex + 1) % menu->numberOfEntries;
}

void increaseAdvancedOptionsSelectedEntry(AdvancedOptionsMenu *menu)
{
    AdvancedOptionsMenuEntry selectedOption = menu->entries[menu->selectedEntryIndex];
    if (selectedOption.incrementHandler)
    {
        selectedOption.incrementHandler();
    }
}

void decreaseAdvancedOptionsSelectedEntry(AdvancedOptionsMenu *menu)
{
    AdvancedOptionsMenuEntry selectedOption = menu->entries[menu->selectedEntryIndex];
    if (selectedOption.decrementHandler)
    {
        selectedOption.decrementHandler();
    }
}

void selectAdvancedOptionsSelectedEntry(AdvancedOptionsMenu *menu)
{
    AdvancedOptionsMenuEntry selectedOption = menu->entries[menu->selectedEntryIndex];
    if (selectedOption.selectionHandler)
    {
        selectedOption.selectionHandler();
    }
}


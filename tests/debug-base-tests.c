/*
 * This file is part of the OpenSupaplex distribution (https://github.com/sergiou87/open-supaplex).
 * Copyright (c) 2026 Sergio Padrino
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

#define updateKeyboardState updateKeyboardStateForBaseTest
#define main opensupaplex_main
#include "../src/supaplex.c"
#undef main
#undef updateKeyboardState

void updateKeyboardState(void);

void updateKeyboardStateForBaseTest(void)
{
    updateKeyboardState();
    // Debug keys are not encoded in the demo stream.
    if (gIsGameRunning && gFrameCounter == 34)
    {
        gIsBKeyPressed = 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "-u") != 0)
    {
        fprintf(stderr, "Usage: debug-base-tests -u <replay>\n");
        return 2;
    }

    char *arguments[] = {argv[0], "-d", argv[1], argv[2], NULL};
    return opensupaplex_main(4, arguments);
}

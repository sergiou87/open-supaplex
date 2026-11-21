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

#define main opensupaplex_main
#include "../src/supaplex.c"
#undef main

typedef struct
{
    const char *name;
    void (*handler)(void);
    uint16_t *interval;
    uint16_t *previous;
    uint8_t checkLevel;
} MenuThrottleCase;

static void rotateLevelSetForTest(void)
{
    throttledRotateLevelSet(0);
}

static int runMenuReplay(FILE *file, const MenuThrottleCase *testCase)
{
    rewind(file);
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL
        || strcmp(line, "frame,pressed,level,interval,previous\n") != 0)
    {
        fprintf(stderr, "Invalid menu replay header\n");
        return 2;
    }

    unsigned row = 0;
    while (fgets(line, sizeof(line), file))
    {
        unsigned frame, pressed, level, interval, previous;
        if (sscanf(line, "%u,%u,%u,%u,%u", &frame, &pressed, &level, &interval, &previous) != 5
            || frame > UINT16_MAX || previous > UINT16_MAX || interval > UINT16_MAX
            || level < 1 || level > 113 || pressed > 2)
        {
            fprintf(stderr, "Invalid menu replay row %u\n", row + 1);
            return 2;
        }

        if (row == 0)
        {
            gCurrentSelectedLevelIndex = level;
            *testCase->interval = interval;
            *testCase->previous = previous;
        }
        gFrameCounter = frame;
        if ((testCase->checkLevel && gCurrentSelectedLevelIndex != level)
            || *testCase->interval != interval || *testCase->previous != previous)
        {
            fprintf(stderr, "%s at frame %u: C level=%u interval=%u previous=%u; original=%u,%u,%u\n",
                    testCase->name, frame, gCurrentSelectedLevelIndex, *testCase->interval,
                    *testCase->previous, level, interval, previous);
            return 1;
        }

        if (pressed == 1)
        {
            testCase->handler();
        }
        else if (pressed == 0)
        {
            *testCase->interval = 0x10;
            *testCase->previous = 0;
        }
        ++row;
    }
    if (ferror(file) || row == 0)
    {
        fprintf(stderr, "Could not read menu replay\n");
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "-u") != 0)
    {
        fprintf(stderr, "Usage: menu-throttle-tests -u <menu-replay.csv>\n");
        return 2;
    }
    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
    {
        perror(argv[2]);
        return 2;
    }

    gFastMode = FastModeTypeUltra;
    gIsForcedCheatMode = 1;
    const MenuThrottleCase cases[] = {
        {"Level down", handleLevelListScrollDown, &gLevelListThrottleCurrentCounter, &gLevelListThrottleNextCounter, 1},
        {"Level up", handleLevelListScrollUp, &gLevelListThrottleCurrentCounter, &gLevelListThrottleNextCounter, 0},
        {"Player down", handlePlayerListScrollDown, &gPlayerListThrottleCurrentCounter, &gPlayerListThrottleNextCounter, 0},
        {"Player up", handlePlayerListScrollUp, &gPlayerListThrottleCurrentCounter, &gPlayerListThrottleNextCounter, 0},
        {"Ranking down", handleRankingListScrollDown, &gRankingListThrottleCurrentCounter, &gRankingListThrottleNextCounter, 0},
        {"Ranking up", handleRankingListScrollUp, &gRankingListThrottleCurrentCounter, &gRankingListThrottleNextCounter, 0},
        {"Level-set rotation", rotateLevelSetForTest, &gLevelSetRotationThrottleCurrentCounter, &gLevelSetRotationThrottleNextCounter, 0},
    };
    int result = 0;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    {
        result = runMenuReplay(file, &cases[i]);
        if (result != 0)
        {
            break;
        }
    }
    if (fclose(file) != 0)
    {
        perror(argv[2]);
        return 2;
    }
    if (result == 0)
    {
        printf("All menu throttles matched original trace: %s\n", argv[2]);
    }
    return result;
}

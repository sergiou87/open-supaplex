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

static int readHallOfFameCase(FILE *file, HallOfFameEntry expected[kNumberOfHallOfFameEntries])
{
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL
        || strcmp(line, "kind,index,name,hours,minutes,seconds\n") != 0)
    {
        fprintf(stderr, "Invalid Hall of Fame case header\n");
        return 2;
    }

    uint8_t seenPlayer = 0;
    uint8_t seenBefore[kNumberOfHallOfFameEntries] = {0};
    uint8_t seenAfter[kNumberOfHallOfFameEntries] = {0};
    unsigned rows = 0;
    while (fgets(line, sizeof(line), file))
    {
        char kind[7], name[kPlayerNameLength + 1];
        unsigned index, hours, minutes, seconds;
        if (sscanf(line, "%6[^,],%u,%8[^,],%u,%u,%u", kind, &index, name, &hours, &minutes, &seconds) != 6
            || hours > UINT8_MAX || minutes >= 60 || seconds >= 60)
        {
            fprintf(stderr, "Invalid Hall of Fame case row %u\n", rows + 1);
            return 2;
        }
        HallOfFameEntry entry = {0};
        memset(entry.playerName, ' ', kPlayerNameLength);
        memcpy(entry.playerName, name, strlen(name));
        entry.hours = hours;
        entry.minutes = minutes;
        entry.seconds = seconds;
        if (strcmp(kind, "player") == 0 && index == 0 && !seenPlayer)
        {
            seenPlayer = 1;
            memcpy(gPlayerListData[0].name, entry.playerName, sizeof(entry.playerName));
            gPlayerListData[0].hours = entry.hours;
            gPlayerListData[0].minutes = entry.minutes;
            gPlayerListData[0].seconds = entry.seconds;
        }
        else if (strcmp(kind, "before") == 0 && index < kNumberOfHallOfFameEntries && !seenBefore[index])
        {
            seenBefore[index] = 1;
            gHallOfFameData[index] = entry;
        }
        else if (strcmp(kind, "after") == 0 && index < kNumberOfHallOfFameEntries && !seenAfter[index])
        {
            seenAfter[index] = 1;
            expected[index] = entry;
        }
        else
        {
            fprintf(stderr, "Invalid or duplicate Hall of Fame record on row %u\n", rows + 1);
            return 2;
        }
        ++rows;
    }
    if (ferror(file) || rows != 1 + kNumberOfHallOfFameEntries * 2)
    {
        fprintf(stderr, "Hall of Fame case must contain a player and all before/after entries\n");
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "-u") != 0)
    {
        fprintf(stderr, "Usage: hall-of-fame-tests -u <hall-of-fame-case.csv>\n");
        return 2;
    }
    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
    {
        perror(argv[2]);
        return 2;
    }
    HallOfFameEntry expected[kNumberOfHallOfFameEntries];
    int result = readHallOfFameCase(file, expected);
    if (fclose(file) != 0)
    {
        perror(argv[2]);
        return 2;
    }
    if (result != 0)
    {
        return result;
    }

    gFastMode = FastModeTypeUltra;
    memset(gPlayerListData[0].levelState, PlayerLevelStateCompleted, sizeof(gPlayerListData[0].levelState));
    updateHallOfFameEntries();
    for (unsigned i = 0; i < kNumberOfHallOfFameEntries; ++i)
    {
        if (memcmp(&gHallOfFameData[i], &expected[i], sizeof(expected[i])) != 0)
        {
            fprintf(stderr, "Hall entry %u: expected '%.8s' %u:%u:%u, got '%.8s' %u:%u:%u\n", i + 1,
                    expected[i].playerName, expected[i].hours, expected[i].minutes, expected[i].seconds,
                    gHallOfFameData[i].playerName, gHallOfFameData[i].hours,
                    gHallOfFameData[i].minutes, gHallOfFameData[i].seconds);
            return 1;
        }
    }
    if (gPlayerListData[0].completedAllLevels != 1)
    {
        fprintf(stderr, "Player completion flag was not set\n");
        return 1;
    }
    printf("Hall of Fame matched original final-level completion: %s\n", argv[2]);
    return 0;
}

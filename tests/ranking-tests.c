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

static int readRankingCase(FILE *file, uint8_t expectedRanks[kNumberOfPlayers])
{
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL
        || strcmp(line, "index,name,next,hours,minutes,seconds,rank\n") != 0)
    {
        fprintf(stderr, "Invalid ranking case header\n");
        return 2;
    }

    uint8_t seenPlayers[kNumberOfPlayers] = {0};
    uint8_t seenRanks[kNumberOfPlayers] = {0};
    unsigned rows = 0;
    while (fgets(line, sizeof(line), file))
    {
        char name[kPlayerNameLength + 1];
        unsigned index, next, hours, minutes, seconds, rank;
        if (sscanf(line, "%u,%8[^,],%u,%u,%u,%u,%u", &index, name, &next, &hours, &minutes, &seconds, &rank) != 7
            || index >= kNumberOfPlayers || rank >= kNumberOfPlayers || next < 1 || next > 113
            || hours > UINT8_MAX || minutes >= 60 || seconds >= 60
            || seenPlayers[index] || seenRanks[rank])
        {
            fprintf(stderr, "Invalid ranking case row %u\n", rows + 1);
            return 2;
        }
        seenPlayers[index] = 1;
        seenRanks[rank] = 1;
        expectedRanks[index] = rank;

        PlayerEntry *player = &gPlayerListData[index];
        memset(player, 0, sizeof(*player));
        memset(player->name, ' ', kPlayerNameLength);
        memcpy(player->name, name, strlen(name));
        player->hours = hours;
        player->minutes = minutes;
        player->seconds = seconds;
        player->nextLevelToPlay = next;
        player->completedAllLevels = (next == 113);
        for (unsigned i = 0; i + 1 < next && i < kNumberOfLevels; ++i)
        {
            player->levelState[i] = PlayerLevelStateCompleted;
        }
        ++rows;
    }
    if (ferror(file) || rows != kNumberOfPlayers)
    {
        fprintf(stderr, "Ranking case must contain all %d players\n", kNumberOfPlayers);
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "-u") != 0)
    {
        fprintf(stderr, "Usage: ranking-tests -u <ranking-case.csv>\n");
        return 2;
    }
    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
    {
        perror(argv[2]);
        return 2;
    }
    uint8_t expectedRanks[kNumberOfPlayers];
    int result = readRankingCase(file, expectedRanks);
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
    prepareRankingTextEntries();
    for (unsigned i = 0; i < kNumberOfPlayers; ++i)
    {
        const PlayerEntry *player = &gPlayerListData[i];
        char expected[kRankingEntryTextLength];
        snprintf(expected, sizeof(expected), "%03d %.8s %03d:%02d:%02d",
                 player->nextLevelToPlay == 113 ? 999 : player->nextLevelToPlay,
                 player->name, player->hours, player->minutes, player->seconds);
        const char *actual = gRankingTextEntries[kRankingFirstPlayerIndex + expectedRanks[i]];
        if (memcmp(actual, expected, sizeof(expected)) != 0)
        {
            fprintf(stderr, "Rank %u: expected '%s', got '%.22s'\n",
                    expectedRanks[i] + 1, expected, actual);
            return 1;
        }
    }
    if (byte_58D47 != expectedRanks[gCurrentPlayerIndex])
    {
        fprintf(stderr, "Current player rank differs: expected %u, got %u\n",
                expectedRanks[gCurrentPlayerIndex] + 1, byte_58D47 + 1);
        return 1;
    }
    printf("Ranking matched original order and displayed times: %s\n", argv[2]);
    return 0;
}

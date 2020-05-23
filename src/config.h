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

#ifndef config_h
#define config_h

#include <stdint.h>

typedef struct {
    void *context;
} Config;

Config *initializeConfigForWriting(const char *pathname);
Config *initializeConfigForReading(const char *pathname);
void destroyConfig(Config *config);

void writeConfigSection(Config *config, const char *section);
void writeConfigInt(Config *config, const char *key, const int value);
void writeConfigString(Config *config, const char *key, const char *value);
int readConfigInt(Config *config, const char *section, const char *key, const int defaultValue);
int readConfigString(Config *config, const char *section, const char *key, char *output);

#endif /* config_h */

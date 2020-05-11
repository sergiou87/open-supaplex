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

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "lib/ini/ini.h"
#include "logging.h"

typedef struct {
    ini_t *iniConfig;
    FILE *file;
} ConfigContextPriv;

void destroyConfig(Config *config)
{
    if (config == NULL)
    {
        return;
    }

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;

    if (context == NULL)
    {
        return;
    }

    if (context->iniConfig != NULL)
    {
        ini_free(context->iniConfig);
        context->iniConfig = NULL;
    }

    if (context->file != NULL)
    {
        fclose(context->file);
        context->file = NULL;
    }

    free(context);
    config->context = NULL;

    free(config);
}

Config *initializeConfig()
{
    Config *config = calloc(1, sizeof(Config));
    if (config == NULL)
    {
        spLog("Something went really wrong. Couldn't allocate config.");
        return NULL;
    }

    config->context = calloc(1, sizeof(ConfigContextPriv));

    if (config->context == NULL)
    {
        spLog("Something went really wrong. Couldn't allocate config context.");
        free(config);
        return NULL;
    }

    return config;
}

Config *initializeConfigForWriting(const char *pathname)
{
    Config *config = initializeConfig();

    if (config == NULL)
    {
        return NULL;
    }

    FILE *file = openWritableFile(pathname, "w");

    if (file == NULL)
    {
        spLog("Couldn't open %s for writing", pathname);
        destroyConfig(config);
        return NULL;
    }

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;
    context->file = file;

    return config;
}

Config *initializeConfigForReading(const char *pathname)
{
    Config *config = initializeConfig();

    if (config == NULL)
    {
        return NULL;
    }

    char configPath[kMaxFilePathLength];
    getWritableFilePath(pathname, configPath);
    ini_t *iniConfig = ini_load(configPath);

    if (iniConfig == NULL)
    {
        spLog("Couldn't open %s for reading", configPath);
        destroyConfig(config);
        return NULL;
    }

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;
    context->iniConfig = iniConfig;

    return config;
}

void writeConfigSection(Config *config, const char *section)
{
    assert(config != NULL);

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;
    assert(context != NULL);

    FILE *file = context->file;
    assert(file != NULL);

    fprintf(file, "\n[%s]\n", section);
}

void writeConfigInt(Config *config, const char *key, const int value)
{
    assert(config != NULL);

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;
    assert(context != NULL);

    FILE *file = context->file;
    assert(file != NULL);

    fprintf(file, "%s = %d\n", key, value);
}

int readConfigInt(Config *config, const char *section, const char *key, const int defaultValue)
{
    assert(config != NULL);

    ConfigContextPriv *context = (ConfigContextPriv *)config->context;
    assert(context != NULL);

    ini_t *iniConfig = context->iniConfig;
    assert(iniConfig != NULL);

    int value = 0;
    
    if (ini_sget(iniConfig, section, key, "%d", &value) == 0)
    {
        return defaultValue;
    }

    return value;
}

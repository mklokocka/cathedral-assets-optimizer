/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "Games.h"

enum bsaType
{
    texturesBsa = 0,
    standardBsa,
    texturesAndStandardBsa
};

PACKED(
        struct PluginHeader
{
            char type[4];
            uint32_t groupSize;
            char label[4];
            int32_t groupType;
            unsigned short stamp;
            char VC1;
            char VC2;
            uint32_t unknown;
        };
)

PACKED(
        struct RecordHeader
{
            char type[4];
            uint32_t dataSize;
            uint32_t flags;
            uint32_t id;
            unsigned short stamp;
            char VC1;
            char VC2;
            uint16_t version;
            uint16_t unknown;
        };
)

PACKED(
        union PluginRecordHeader
{
            RecordHeader record;
            PluginHeader plugin;
        };
        )

PACKED(
        struct PluginFieldHeader
{
            char type[4];
            uint16_t dataSize;
        };)

const char GROUP_TES4[4] = { 'T', 'E', 'S', '4'};
const char GROUP_GRUP[4] = { 'G', 'R', 'U', 'P'};
const char GROUP_HDPT[4] = { 'H', 'D', 'P', 'T'};
const char GROUP_MODL[4] = { 'M', 'O', 'D', 'L'};

class PluginsOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
     * \brief Try to find a plugin (.esp ; .esl ; .esm) in the given folder. It will always return an esp that isn't associated to a BSA OR a textures BSA.
     * \param folderPath The folder to check
     * \param bsaType The type of BSA to load.
     * \return a QString containing the name of the plugin. If no plugin is found, it will return the name of the directory.
     */
    static QString findPlugin(const QString& folderPath, const bsaType &bsaType);
    /*!
     * \brief Create enough plugins to load all BSAs
     * \param folderPath The folder to create plugins into
     */
    static void makeDummyPlugins(const QString& folderPath);
    /*!
     * \brief Check if a bsa already has a plugin to load it
     * \param bsaPath The path of the bsa to check
     */
    static bool checkIfBsaHasPlugin(const QString& bsaPath);
    /*!
     * \brief listHeadparts List all the headparts in a plugin file
     * \param filepath The path of the file to scan
     * \return The list of headparts
     */
    static QStringList listHeadparts(const QString& filepath);
};
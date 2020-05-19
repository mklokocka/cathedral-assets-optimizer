/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/BSACreate.hpp"
#include "Commands/BSA/BSAExtract.hpp"
#include "Commands/CommandBook.hpp"
#include "File/Animations/AnimationFile.hpp"
#include "File/BSA/BSAFile.hpp"
#include "File/Meshes/MeshFile.hpp"
#include "File/Textures/TextureFile.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MainOptimizer)

public:
    explicit MainOptimizer(const Profile &profile);

    void process(const QString &path);
    void packBsa(const QString &folder);

protected:
    bool runCommand(CommandPtr command, File &file);
    bool loadFile(File &file, const QString &path);
    bool saveFile(File &file, const QString &path);

private:
    void processBsa(const QString &file);
    bool processStandardFile(File &file, const QString &path, const Command::CommandType &type);

    Profile profile_;

    BSAFile _bsaFile;
    MeshFile _meshFile;
    TextureFile _textureFile;
    AnimationFile _animFile;
    CommandBook _commandBook;
};
} // namespace CAO
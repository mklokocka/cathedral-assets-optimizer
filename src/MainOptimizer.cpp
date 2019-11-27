/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainOptimizer.hpp"
#include "Plugins/PluginsOperations.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {
MainOptimizer::MainOptimizer(const Settings &optOptions)
    : _optOptions(optOptions)
{}

void MainOptimizer::process(const QString &path)
{
    if (path.endsWith(".dds", Qt::CaseInsensitive)
        || (path.endsWith(".tga") && _optOptions.getMandatoryValue<bool>(AdvancedSettings::bTexturesTGAConvertEnabled)))
        processStandardFile(_textureFile, path, Command::CommandType::Texture);
    else if (path.endsWith(".nif", Qt::CaseInsensitive))
        processStandardFile(_meshFile, path, Command::CommandType::Mesh);
    else if (path.endsWith(_optOptions.getMandatoryValue<QString>(AdvancedSettings::sBSASuffix), Qt::CaseInsensitive))
        processBsa(path);
    else if (path.endsWith(".hkx", Qt::CaseInsensitive))
        processStandardFile(_animFile, path, Command::CommandType::Animation);
}

void MainOptimizer::processBsa(const QString &file)
{
    if (_optOptions.getMandatoryValue<bool>(StandardSettings::bDryRun))
        return; //TODO if "dry run" run dry run on the assets in the BSA

    PLOG_INFO << "Extracting BSA: " + file;
    if (!loadFile(_bsaFile, file))
        return;
    BSAExtract command;
    if (!runCommand(&command, _bsaFile))
        return;

    //TODO if(settings.bBsaOptimizeAssets)
}

void MainOptimizer::packBsa(const QString &folder)
{
    PLOG_INFO << "Creating BSA...";
    BSAFolder bsa;
    if (!loadFile(bsa, folder))
        return;
    BSACreate command;
    if (!runCommand(&command, bsa))
        return;

    PluginsOperations::makeDummyPlugins(folder, _optOptions);
}

bool MainOptimizer::processStandardFile(File &file, const QString &path, const Command::CommandType &type)
{
    if (!loadFile(file, path))
        return false;

    for (auto command : _commandBook.getCommandListByType(type))
        if (!runCommand(command, file))
            return false;

    if (!file.optimizedCurrentFile())
        return false;

    if (!saveFile(file, path))
        return false;

    PLOG_INFO << "Successfully optimized " << path;
    return true;
}

bool MainOptimizer::runCommand(Command *command, File &file)
{
    const auto &result = command->processIfApplicable(file, _optOptions);
    if (result.processedFile)
    {
        PLOG_VERBOSE << QString("Successfully applied module '%1' while processing '%2'")
                            .arg(command->name(), file.getName());
        return true;
    }
    else if (result.errorCode)
    {
        PLOG_ERROR << QString("An error happened in module '%1' while processing '%2': '%3'")
                          .arg(command->name(), file.getName(), result.errorMessage);
        return false;
    }
    else
    {
        PLOG_VERBOSE << QString("Module '%1' was not applied because it was not necessary").arg(command->name());
        return true;
    }
}

bool MainOptimizer::loadFile(File &file, const QString &path)
{
    if (file.loadFromDisk(path))
    {
        PLOG_ERROR << "Cannot load file from disk: " << path;
        return false;
    }
    return true;
}

bool MainOptimizer::saveFile(File &file, const QString &path)
{
    if (file.saveToDisk(path))
    {
        PLOG_ERROR << "Cannot save file to disk: " << path;
        return false;
    }
    return true;
}

} // namespace CAO

/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Command.hpp"
#include "FilesystemOperations.hpp"
#include "Meshes/MeshFile.hpp"
#include "PluginsOperations.hpp"

namespace CAO {
class MeshConvert : public Command
{
public:
    MeshConvert()
    {
        _name = "Convert Mesh";
        _type = CommandType::Mesh;
        _priority = Priority::Medium;
    }

    CommandResult process(File &file, const OptionsCAO &options) override;
    bool isApplicable(File &file, const OptionsCAO &options) override;

protected:
    bool isHeadpart(const QString &filepath);
    void listHeadparts(const OptionsCAO &options);

    static QStringList headpartList;

    std::once_flag _onceHeadpartsFlag;
};
} // namespace CAO

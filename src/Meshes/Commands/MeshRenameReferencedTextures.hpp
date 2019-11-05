/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/Command.hpp"
#include "Meshes/MeshFile.hpp"

namespace CAO {
class MeshRenameReferencedTextures : public Command
{
public:
    MeshRenameReferencedTextures()
    {
        _name = "Rename referenced textures in Mesh";
        _type = CommandType::Mesh;
        _priority = Low;
    }

    CommandResult process(File &file, const OptionsCAO &options) override;
    bool isApplicable(File &file, const OptionsCAO &options) override;
};
} // namespace CAO

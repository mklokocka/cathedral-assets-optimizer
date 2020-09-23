/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandBook.hpp"

namespace CAO {
class BSACreate : public Command
{
public:
    QString name() const override { return "BSA Create"; }
    bool isOptimization() const override { return true; }
    Priority priority() const override { return Low; };
    CommandType type() const override { return CommandType::BSAFolder; };

    CommandResult process(File &file) const override;
    CommandState isApplicable(File &file) const override;

protected:
    bool canBeCompressedFile(const QString &filename);
};
REGISTER_COMMAND(BSACreate)
} // namespace CAO

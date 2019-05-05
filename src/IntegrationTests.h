/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "FilesystemOperations.h"
#include "MainOptimizer.h"

class IntegrationTests
{
public:
    IntegrationTests(const QString& path);

    bool runAllTests();

private:
    QDir m_dir;
};

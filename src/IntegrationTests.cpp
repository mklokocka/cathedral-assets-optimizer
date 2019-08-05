/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "IntegrationTests.h"
#include "FilesystemOperations.h"

IntegrationTests::IntegrationTests(const QString &path)
    : _dir(QDir(path)){};

//TODO fix this class (broken with refactoring and ini changes)

bool IntegrationTests::runAllTests() const
{
    qDebug() << "tests folder: " << _dir.path();

    const QStringList dirs = _dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    const QStringList ignoredTests;

    QVector<bool> results;

    for (const auto &dir : dirs)
    {
        if (!ignoredTests.contains(dir))
        {
            //Assigning folders

            const QString input = _dir.filePath(dir + "/INPUT");
            const QString output = _dir.filePath(dir + "/OUTPUT");
            const QString expected = _dir.filePath(dir + "/EXPECTED");

            //Replacing ini file with predefined config

            QString config = _dir.filePath(dir + "/config.ini");
            QString CathedralIni = QDir::currentPath() + "/Cathedral Assets Optimizer.ini";
            QFile::remove(CathedralIni);
            QFile::copy(config, CathedralIni);

            //Copying input dir to keep it untouched

            FilesystemOperations::copyDir(input, output, true);

            //Running the optimization
            /*
      Manager manager;
      manager.runOptimization();
*/
            results << FilesystemOperations::compareFolders(output, expected, true);

            //Deleting output if test was passed

            QDir reset(output);
            if (results.last())
                reset.removeRecursively();
        }
    }

    qDebug() << results;

    for (const auto &result : results)
    {
        if (!result)
            return false;
    }

    return true;
}

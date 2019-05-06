/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BsaOptimizer.h"

BsaOptimizer::BsaOptimizer() = default;


void BsaOptimizer::bsaExtract(const QString &bsaPath, const bool &makeBackup, const bool &keepFileInBsaFolder)
{
    QDir directory(QFileInfo(bsaPath).path());
    QStringList files(directory.entryList());
    QString bsaFolder = directory.filePath(bsaPath + ".extracted");

    directory.mkdir(bsaFolder);

    QStringList bsarchArgs;

    if(!makeBackup)
        bsarchArgs << "unpack" << bsaPath << bsaFolder ;
    else
        bsarchArgs << "unpack" << backupOldBsa(bsaPath) << bsaFolder ;

    QProcess bsarch;
    bsarch.start(QCoreApplication::applicationDirPath() + "/resources/bsarch.exe", bsarchArgs);
    bsarch.waitForFinished(-1);

    QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolder + "\nBSA Name :" + bsaPath);

    if(bsarch.readAllStandardOutput().contains("Done"))
    {
         if(!keepFileInBsaFolder)
            if(!moveFilesFromBsaFolderToRootFolder(bsaFolder))
            {
                QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));
                QLogger::QLog_Error("Errors", tr("An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory."));
            }

        if(!makeBackup)
            QFile::remove(bsaPath);

        QLogger::QLog_Info("BsaOptimizer", tr("BSA successfully extracted: ") + bsaPath);
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted."));
        QLogger::QLog_Error("Errors", tr("An error occured during the extraction. Please extract it manually. The BSA was not deleted."));

    }
}


void BsaOptimizer::bsaCreate(const QString &bsaFolderPath) //Once all the optimizations are done, create a new BSA
{
    QDir bsaDir(bsaFolderPath);
    QStringList bsaSubDirs(bsaDir.entryList(QDir::Dirs));

    //Detecting if BSA will contain sounds, since compressing BSA breaks sounds. Same for strings, Wrye Bash complains

    bool doNotCompress = false;
    for (int j = 0; j < bsaSubDirs.size(); ++j)
    {
        if(bsaSubDirs.at(j).toLower() == "sound" || bsaSubDirs.at(j).toLower() == "music" || bsaSubDirs.at(j).toLower() == "strings")
            doNotCompress=true;
    }

    //Checking if it a textures BSA

    QString bsaName = bsaFolderPath.chopped(10); //Removing ".extracted"
    QStringList bsarchArgs {"pack", bsaFolderPath, bsaName, "-sse", "-share"};

    if (!doNotCompress) //Compressing BSA breaks sounds
        bsarchArgs << "-z";

    QProcess bsarch;

    if(!QFile(bsaName).exists())
    {
        bsarch.start(QCoreApplication::applicationDirPath() + "/resources/bsarch.exe", bsarchArgs);
        bsarch.waitForFinished(-1);
    }
    else
    {
        QLogger::QLog_Error("BsaOptimizer", tr("Cannot pack existing loose files: a BSA already exists."));
        QLogger::QLog_Error("Errors", tr("Cannot pack existing loose files: a BSA already exists."));
        moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
    }

    QLogger::QLog_Debug("BsaOptimizer", "BSArch Args :" + bsarchArgs.join(" ") + "\nBSA folder :" + bsaFolderPath + "\nBsaName : " + bsaName + "\nBSAsize: " + QString::number(QFile(bsaName).size()));

    QString bsarchOutput = bsarch.readAllStandardOutput();
    QLogger::QLog_Debug("BsaOptimizer", bsarchOutput);

    if(bsarchOutput.contains("Done"))
    {
        if(QFile(bsaName).size() < LONG_MAX)
        {
            QLogger::QLog_Note("BsaOptimizer", tr("BSA successfully compressed: ") + bsaName);
            bsaDir.setPath(bsaFolderPath);
            bsaDir.removeRecursively();
        }
        else
        {
            QLogger::QLog_Error("BsaOptimizer", tr("The BSA was not compressed: it is over 2.15gb: ") + bsaName);
            QLogger::QLog_Error("Errors", tr("The BSA was not compressed: it is over 2.15gb: ") + bsaName);
            moveFilesFromBsaFolderToRootFolder(bsaFolderPath);
            QFile::remove(bsaName);
            if(QFile(bsaName.chopped(3) + "esp").size() == QFile(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp").size())
                QFile::remove(QDir(bsaFolderPath).filePath(bsaName.chopped(3) + "esp"));
        }
    }
}


bool BsaOptimizer::moveFilesFromBsaFolderToRootFolder(const QString &bsaFolderPath)
{
    QString rootFolderPath = QFileInfo(bsaFolderPath).path();

    try
    {
        FilesystemOperations::moveFiles(bsaFolderPath, rootFolderPath, false);
    }
    catch(const QString& e)
    {
        QLogger::QLog_Error("BsaOptimizer", e);
        QLogger::QLog_Error("Errors", e);
        return  false;
    }
    return true;
}


QString BsaOptimizer::backupOldBsa(const QString& bsaPath)
{
    QFile bsaBackupFile(bsaPath + ".bak");
    QFile bsaFile(bsaPath);

    if(!bsaBackupFile.exists())
        QFile::rename(bsaPath, bsaBackupFile.fileName());
    else
    {
        if(bsaFile.size() == bsaBackupFile.size())
            QFile::remove(bsaBackupFile.fileName());
        else
            QFile::rename(bsaBackupFile.fileName(), bsaBackupFile.fileName() + ".bak");
    }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    return bsaBackupFile.fileName();
}

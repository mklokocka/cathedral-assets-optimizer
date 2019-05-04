#include "FilesystemOperations.h"

const QStringList texturesAssets {"png", "dds"};
const QStringList otherAssets {"nif", "seq", "pex", "psc", "lod", "fuz", "waw", "xwm", "swf", "hkx", "wav", "tri", "btr", "bto", "btt", "lip", "txt", "lst"};
const QStringList allAssets = texturesAssets + otherAssets;


FilesystemOperations::FilesystemOperations()
{
    //Reading filesToNotPack to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile filesToNotPackFile(QCoreApplication::applicationDirPath() + "/resources/FilesToNotPack.txt");
    if(filesToNotPackFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&filesToNotPackFile);
        while (!ts.atEnd())
        {
            QString readLine = ts.readLine();
            if(readLine.left(1) != "#" && !readLine.isEmpty())
                filesToNotPack << readLine;
        }
    }
    else
    {
        QLogger::QLog_Warning("FilesystemOperations", tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis."));
        QLogger::QLog_Warning("Errors", tr("FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis."));
    }
}


void FilesystemOperations::prepareBsas(const QString &folderPath) //Split assets between several folders
{
    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");

    QDir directory(folderPath);

    QStringList bsaList;
    QStringList texturesBsaList;
    QStringList dirs(directory.entryList(QDir::Dirs));

    QLogger::QLog_Trace("FilesystemOperations", "Listing all BSA folders and moving files to modpath root directory");

    for (const auto& dir : dirs)
    {
        if(dir.endsWith("bsa.extracted") && dir.contains("- Textures", Qt::CaseInsensitive))
        {
            texturesBsaList << directory.filePath(dir);
            moveFiles(directory.filePath(dir), directory.path(), false);
        }
        else if(dir.endsWith("bsa.extracted"))
        {
            bsaList << directory.filePath(dir);
            moveFiles(directory.filePath(dir), directory.path(), false);
        }
    }

    QString espName = PluginsOperations::findPlugin(folderPath);
    QString bsaName;

    QLogger::QLog_Trace("FilesystemOperations", "Creating enough folders to contain all the files");

    QPair<qint64, qint64> size = assetsSize(directory.path());
    int i = 0;

    while(texturesBsaList.size() < qCeil(size.first/2547483647.0))
    {
        if(i == 0)
            bsaName = espName + " - Textures.bsa.extracted";
        else
            bsaName = espName + QString::number(i) + " - Textures.bsa.extracted";

        texturesBsaList << bsaName;
        texturesBsaList.removeDuplicates();
        ++i;
    }
    i = 0;
    while(bsaList.size() < qCeil(size.second/2107483647.0))
    {
        if(i == 0)
            bsaName = espName + ".bsa.extracted";
        else
            bsaName = espName + QString::number(i) + ".bsa.extracted";

        bsaList << bsaName ;
        bsaList.removeDuplicates();
        ++i;
    }

    QLogger::QLog_Debug("FilesystemOperations", "main folders:\n" + directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot).join("\n"));

    moveAssets(folderPath, bsaList, texturesBsaList);

    QLogger::QLog_Trace("FilesystemOperations", "Total: " + QString::number(bsaList.size()) + " bsa folders:\n" + bsaList.join("\n") + "\n"
                        + QString::number(texturesBsaList.size()) + " textures bsa folders:\n" + texturesBsaList.join("\n"));

    deleteEmptyDirectories(folderPath);

    QLogger::QLog_Trace("FilesystemOperations", "Exiting splitAssets function");
}

void FilesystemOperations::moveFiles(const QString& source, const QString& destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    QStringList oldFiles;

    while (it.hasNext())
    {
        it.next();
        if (!QFileInfo(it.filePath()).isDir()) //Skipping all directories and avoiding to copy from the destination folder
            oldFiles << it.filePath();
    }

    oldFiles.removeDuplicates();

    for (const auto& oldFile : oldFiles)
    {
        QString relativeFilename = sourceDir.relativeFilePath(oldFile);
        QString newFileRelativeFilename = destinationDir.relativeFilePath(QDir::cleanPath(destination + QDir::separator() + relativeFilename));

        if(newFileRelativeFilename.size() >= 255 || oldFile.size() >=255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            QLogger::QLog_Error("Errors", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        //removing the duplicate files from new folder (if overwriteExisting) or from old folder (if !overwriteExisting)

        destinationDir.mkpath(QFileInfo(newFileRelativeFilename).path());

        if(overwriteExisting)
            destinationDir.remove(newFileRelativeFilename);

        destinationDir.rename(oldFile, newFileRelativeFilename);

        if(!overwriteExisting)
            destinationDir.remove(oldFile);
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");
}




void FilesystemOperations::moveAssets(const QString &path, const QStringList &bsaList, const QStringList &texturesBsaList)
{
    QLogger::QLog_Trace("FilesystemOperations", "Splitting files between bsa folders");

    int k = 0;
    int j = 0;

    deleteEmptyDirectories(path);
    QDir directory(path);
    QDirIterator it(directory, QDirIterator::Subdirectories);
    QStringList oldFiles;

    while(it.hasNext())
    {
        it.next();

        //Skipping all directories and avoiding unnecessary files
        QFileInfo info(it.filePath());
        bool isDir = info.isDir();
        bool hasAssets = allAssets.contains(it.fileName().right(3), Qt::CaseInsensitive);
        bool canBePacked = true;

        for(int i = 0 ; i < filesToNotPack.size() ; ++i)
        {
            if(it.filePath().contains(filesToNotPack.at(i)))
                canBePacked = false;
        }

        if(!isDir && hasAssets && canBePacked)
            oldFiles << directory.relativeFilePath(it.filePath());
    }

    oldFiles.removeDuplicates();

    QLogger::QLog_Debug("FilesystemOperations", "oldFiles.size: " + QString::number(oldFiles.size()));

    for (const auto& oldFile : oldFiles)
    {
        QString newFile;
        if(otherAssets.contains(oldFile.right(3), Qt::CaseInsensitive))
        {
            ++k;
            if(k >= bsaList.size() || k < 0)
                k = 0;
            newFile = directory.relativeFilePath(bsaList.at(k) + "/" + oldFile);
        }

        else if(texturesAssets.contains(oldFile.right(3), Qt::CaseInsensitive))
        {
            ++j;
            if(j >= texturesBsaList.size() || j < 0)
                j = 0;
            newFile = directory.relativeFilePath(texturesBsaList.at(j) + "/" + oldFile);
        }

        QLogger::QLog_Debug("FilesystemOperations", "\nOld file: " + oldFile
                            + "\nNew file: " + newFile);

        if(newFile.size() >= 255 || oldFile.size() >=255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            QLogger::QLog_Error("Errors", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        directory.mkpath(QFileInfo(newFile).path());
        directory.rename(oldFile, newFile);
    }
}


void FilesystemOperations::deleteEmptyDirectories(const QString &folderPath)
{
    QDirIterator dirIt(folderPath, QDirIterator::Subdirectories);
    QMap<int, QStringList> dirs;

    while(dirIt.hasNext())
    {
        QString path =  QDir::cleanPath(dirIt.next());
        int size = path.size();
        if(!dirs[size].contains(path))
            dirs[size].append(path);
    }

    QDir dir(folderPath);
    QMapIterator<int, QStringList> i (dirs);

    i.toBack();
    while (i.hasPrevious())
    {
        i.previous();
        for(int j = 0; j < i.value().size(); ++j)
        {
            dir.rmpath(i.value().at(j));
        }
    }
}


QPair <qint64, qint64> FilesystemOperations::assetsSize(const QString& path) // Return textures size and other assets size in a directory
{
    QPair <qint64, qint64> size;
    //First will be textures, second will be other assets

    QDirIterator it(path, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QFile currentFile(it.next());

        if(texturesAssets.contains(it.fileName().right(3), Qt::CaseInsensitive))
            size.first += currentFile.size();
        else if(otherAssets.contains(currentFile.fileName().right(3), Qt::CaseInsensitive))
            size.second += currentFile.size();
    }
    return size;
}


bool FilesystemOperations::compareFolders(const QString &folder1, const QString &folder2, const bool &checkFileSize)
{
    QDirIterator it1 (folder1, QDirIterator::Subdirectories);
    QDirIterator it2 (folder2, QDirIterator::Subdirectories);

    QStringList files1;
    QStringList files2;

    QDir dir1(folder1);
    QDir dir2(folder2);


    QVector<qint64> filesSize1;
    QVector<qint64> filesSize2;

    while(it1.hasNext())
    {
        QString currentFile = dir1.relativeFilePath(it1.next());
        files1 << currentFile;

        if(checkFileSize)
            filesSize1 << QFile(currentFile).size();
    }

    while(it2.hasNext())
    {
        QString currentFile = dir2.relativeFilePath(it2.next());
        files2 << currentFile;

        if(checkFileSize)
            filesSize2 << QFile(currentFile).size();
    }

    if(files1.size() != files2.size())
        return false;

    if(files1 != files2)
        return false;

    if(checkFileSize && filesSize1 != filesSize2)
        return false;

    return true;
}


void FilesystemOperations::copyDir(const QString &source, const QString &destination, bool overwriteExisting)
{
    QDir sourceDir(source);
    QDir destinationDir(destination);
    QDirIterator it(source, QDirIterator::Subdirectories);

    QLogger::QLog_Trace("FilesystemOperations", "Entering " + QString(__FUNCTION__) + " function");
    QLogger::QLog_Debug("FilesystemOperations", "dest folder: " + destination + "\nsource folder: " + source);

    QStringList oldFiles;

    QDir::setCurrent(destination);

    while (it.hasNext())
    {
        it.next();
        if (!QFileInfo(it.filePath()).isDir()) //Skipping all directories
            oldFiles << it.filePath();
    }

    oldFiles.removeDuplicates();

    for (const auto& oldFile : oldFiles)
    {
        QString relativeFilename = sourceDir.relativeFilePath(oldFile);
        QString newFile = QDir::cleanPath(destination + QDir::separator() + relativeFilename);

        if(newFile.size() >= 255)
        {
            QLogger::QLog_Error("FilesystemOperations", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            QLogger::QLog_Error("Errors", tr("An error occurred while moving files. Try reducing path size (260 characters is the maximum)"));
            return;
        }

        destinationDir.mkpath(QFileInfo(newFile).path());

        if(overwriteExisting)
            destinationDir.remove(newFile);

        QFile::copy(oldFile, newFile);
    }
    QLogger::QLog_Trace("FilesystemOperations", "Exiting moveFiles function");

    QDir::setCurrent(QCoreApplication::applicationDirPath());
}



QString FilesystemOperations::findSkyrimDirectory() //Find Skyrim directory using the registry key
{
    QSettings SkyrimReg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat);
    QString SkyrimDir = QDir::cleanPath(SkyrimReg.value("Installed Path").toString());
    return SkyrimDir;
}


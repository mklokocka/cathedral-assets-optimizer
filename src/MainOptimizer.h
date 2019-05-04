#ifndef MAIN_OPTIMIZER_H
#define MAIN_OPTIMIZER_H

#include "pch.h"
#include "QLogger/QLogger.h"
#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "FilesystemOperations.h"
#include "MeshesOptimizer.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

/*!
 * \brief Manages the optimization options
 */
struct optOptions
{
    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bAnimationsOptimization{};
    bool bDryRun{};

    int iMeshesOptimizationLevel{};
    int iTexturesOptimizationLevel{};
    int iMode{};

    QString userPath;
};

/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */
class MainOptimizer : public QObject
{
    Q_OBJECT

public:
    MainOptimizer();

    optOptions options;

    int mainProcess();

    //Settings operations

    /*!
    * \brief Loads settings from the ini file to variables
    */
    void loadSettings();
    /*!
         * \brief Sets the log level to value
         * \param value The value to set
         */
    void setLogLevel(const QLogger::LogLevel &value) { logLevel = value; }
    /*!
     * \brief Resets the settings to default
     */
    static void resetSettings();

private:
    QStringList modDirs;

    QLogger::QLoggerManager *logManager;
    QLogger::LogLevel logLevel{QLogger::LogLevel::Info};

    void init();
    void dryRun();
    bool checkRequirements();
    void fillModsLists();
    void optimizeAssets(const QString& folderPath);
    void dryOptimizeAssets(const QString& folderPath);



signals:
    void progressBarMaximumChanged(int maximum);
    void progressBarIncrease();
    void progressBarReset();
    void progressBarBusy();
    void end();
    void updateLog();
};


#endif // OPTIMISER_H

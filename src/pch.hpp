// Add C includes here

#if defined __cplusplus
// Add C++ includes here

//Win7 compatibility
#include <Windows.h>

/*---CORE---------------*/
#include <QCommandLineParser>
#include <QCompleter>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFile>
#include <QInputDialog>
#include <QMap>
#include <QMutex>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtMath>
/*----------------------*/

/*---GUI----------------*/
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QValidator>
#include <QWidget>
/*----------------------*/

/*--STD-----------------*/
#include <any>
#include <filesystem>
#include <fstream>
#include <optional>
#include <utility>
/*----------------------*/

/*--WINDOWS-------------*/
#include <wrl/client.h>
/*----------------------*/

/*---CUSTOM FILES-----*/
#include "Logger.hpp"
#include "Textures/TextureFormats.hpp"
#include "libs/DirectXTex/DirectXTex.h"
#include "libs/NIF/NifFile.h"
#include "libs/hkxcmd/hkfutils.h"
#include "libs/json/single_include/nlohmann/json.hpp"
#include "libs/libbsarch/BSArchiveAuto.h"
/*----------------------*/

#endif

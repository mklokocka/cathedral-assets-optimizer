/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifdef GUI
#include "MainWindow.h"
#endif

int main(int argc, char *argv[])
{
#ifdef GUI
    QApplication app(argc, argv);
#elif ifndef GUI
    QCoreApplication app(argc, argv);
#endif

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);

#ifdef GUI
    MainWindow *window;
#elif ifndef GUI
    Manager *manger;
#endif

    try
    {
#ifdef GUI
        window = new MainWindow;
        window->show();
#elif ifndef GUI
        Manager manager;
        manager.runOptimization();
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        PLOG_FATAL << e.what();
        return 1;
    }

#ifdef GUI
    return QApplication::exec();
#elif ifndef GUI
    return 0;
#endif
}

/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BSAFilesToPack.hpp"
#include "ListDialog.hpp"
#include "pch.hpp"
#include "ui_mainWindow.h"

namespace Ui {
class MainWindow;
}
namespace CAO {
class Manager;
class GeneralSettings;
class Profile;
class Profiles;

class MainWindow final : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();
    ~MainWindow();

    Ui::MainWindow &mainUI() { return *_ui; }
    Ui::BSAFilesToPack &bsaUI() { return bsaFilesToPackDialog->getUi(); }

    const Ui::MainWindow &mainUI() const { return *_ui; }
    const Ui::BSAFilesToPack &bsaUI() const { return bsaFilesToPackDialog->getUi(); }

private:
    Ui::MainWindow *_ui;

    bool _bLockVariables = false;

    void saveUi();
    void loadUi();
    void refreshProfiles();
    void createProfile();

    void setDarkTheme(const bool &enabled);

    void resetUi() const;

    void setProfile(const Profile &profile);

    void showTutorialWindow(const QString &title, const QString &text);

    void updateLog() const;
    void initProcess();
    void endProcess();
    void readProgress(const QString &text, const int &max, const int &value) const;

    void setAdvancedSettingsEnabled(const bool &value);

    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    void firstStart();

    int _progressBarValue{};

    std::unique_ptr<Profiles> _profiles;

    std::unique_ptr<Manager> _caoProcess;
    bool _settingsChanged;
    bool _alwaysSaveSettings = false;
    bool _showTutorials;
    ListDialog *texturesFormatDialog;
    BSAFilesToPackWidget *bsaFilesToPackDialog;
};
} // namespace CAO
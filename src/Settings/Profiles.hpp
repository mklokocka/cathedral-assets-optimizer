/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profile.hpp"
#include "Settings/CommonSettings.hpp"
#include "Utils/CallOnce.hpp"
#include "pch.hpp"

namespace CAO {
class MainWindow;
class Profiles
{
public:
    static inline const QString commonSettingsFileName   = "CommonSettings.json";
    static inline const QString defaultProfile           = "SSE";
    static inline const QString defaultProfileDir        = "profiles";
    static inline const QString QuickAutoPortProfilesDir = defaultProfileDir + "/__QAP";

    Profiles();
    Profiles(QDir dir);

    /* Profiles operations */
    void create(const QString &name, const QString &baseProfile);
    void create(const QString &name);
    void create(const Profile &profile, const QString &name);
    bool remove(const QString &profile);
    bool rename(const QString &profile, const QString &newName);

    Profile &setCurrent(const QString &profile);

    //Also sets the current profile. Reads the current profile from INI
    Profile &getCurrent();
    Profile &get(const QString &profile);

    QString currentProfileName();

    QStringList list();
    bool exists(const QString &profile);
    void update(bool fullRefresh = false);

    CommonSettings &commonSettings() { return commonSettings_; }
    QString commonSettingsPath();
    void saveCommonSettings();

    void beginRun();
    static void callWhenRunStart(const std::function<void()> &callable);

private:
    std::unordered_map<QString, Profile> profiles_;
    QDir rootProfileDir_;
    CommonSettings commonSettings_;

    static std::vector<std::function<void()>> &callOnceList();
};

inline Profiles &getProfiles()
{
    static Profiles p;
    return p;
}

inline Profile &currentProfile()
{
    return getProfiles().getCurrent();
}

} // namespace CAO

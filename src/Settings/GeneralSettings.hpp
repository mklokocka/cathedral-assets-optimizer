/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QCoreApplication>

#include "BaseTypes.hpp"
#include "Settings.hpp"
#include "Utils/QJSONValueWrapper.hpp"

#define REGISTER_SETTING(type, name, key) \
    QJSONValueWrapper<type> name{json_, nlohmann::json_pointer<nlohmann::json>{key}};

class QDir;

namespace CAO {
class GeneralSettings final : public Settings
{
    Q_DECLARE_TR_FUNCTIONS(GeneralSettings)
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);
    GeneralSettings(const GeneralSettings &other);
    GeneralSettings(GeneralSettings &&other) noexcept;

    GeneralSettings &operator=(const GeneralSettings &other);
    GeneralSettings &operator=(GeneralSettings &&other) noexcept;

    std::optional<QString> isValid() const override;

private:
    static inline const QString forceProcessFolder = "ForceProcess.cao";

    enum class ModManager
    {
        Vortex,
        MO2,
        Kortex,
        ManualForced,
        None
    };
    ModManager findManager(const QDir &dir) const;

public:
    REGISTER_SETTING(bool, bBSACreate, "/BSA/bBSACreate")
    REGISTER_SETTING(bool, bBSAExtract, "/BSA/bBSAExtract")
    REGISTER_SETTING(bool, bBSAProcessContent, "/BSA/bBSAProcessContent")

    REGISTER_SETTING(bool, bBSADontMakeLoaded, "/BSA/bBSADontMakeLoaded")
    REGISTER_SETTING(bool, bBSADontCompress, "/BSA/bBSADontCompress")
    REGISTER_SETTING(bool, bBSADontMergeTextures, "/BSA/bBSADontMergeTextures")
    REGISTER_SETTING(bool, bBSADontMergeIncomp, "/BSA/bBSADontMergeIncomp")

    REGISTER_SETTING(double, iBSAMaxSize, "/Advanced/BSA/iBSAMaxSize")
    REGISTER_SETTING(bool, bBSATexturesEnabled, "/Advanced/BSA/bBSATexturesEnabled")
    REGISTER_SETTING(double, iBSATexturesMaxSize, "/Advanced/BSA/iBSATexturesMaxSize")

    REGISTER_SETTING(bool, isBaseProfile, "/General/isBaseProfile")

    REGISTER_SETTING(bool, bDryRun, "/General/bDryRun")
    REGISTER_SETTING(OptimizationMode, eMode, "/General/eMode")
    REGISTER_SETTING(Games, eGame, "/General/Game")
    REGISTER_SETTING(QString, sCurrentPattern, "/General/sCurrentPattern")

    REGISTER_SETTING(bool, bEnableOutputPath, "/General/bEnableOutputPath")
    REGISTER_SETTING(QString, sInputPath, "/General/sInputPath")
    REGISTER_SETTING(QString, sOutputPath, "/General/sOutputPath")
};
} // namespace CAO

#undef REGISTER_SETTING

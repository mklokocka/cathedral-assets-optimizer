
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "GUI/MainWindow.hpp"
#include "JSON.hpp"
#include "UISync.hpp"
#include "pch.hpp"
#include "ui_BSAFilesToPack.h"

namespace CAO {

namespace { //Macros used to register settings
#define UI_READ_FUNCTION(widget, function, key) \
    []([[maybe_unused]] const MainWindow &window, [[maybe_unused]] nlohmann::json &json) { \
        JSON::setValue(json, key, function(widget)); \
    }

#define UI_SAVE_FUNCTION(widget, function, type, key) \
    []([[maybe_unused]] MainWindow &window, [[maybe_unused]] const nlohmann::json &json) { \
        function(widget, JSON::getValue<type>(json, key)); \
    }

#define SETTING_BUILDER(key, name) \
    SettingList builder_##name{UISync{key, readUI_##name, saveUI_##name}, \
                               uiSyncList_}; //Expects a vector named uiSyncList_

#define SETTING_GETTER(name, type, key) \
    type name() const { return JSON::getValue<type>(json_, key); }

#define REGISTER_SETTING(type, name, key, widget, readFunc, saveFunc) \
public: \
    SETTING_GETTER(name, type, key) \
\
private: \
    UISync::uiRead readUI_##name = UI_READ_FUNCTION(widget, readFunc, key); \
    UISync::uiSave saveUI_##name = UI_SAVE_FUNCTION(widget, saveFunc, type, key); \
    SETTING_BUILDER(key, name)

#define REGISTER_SETTING_2_WIDGETS(type, name, key, widget1, widget2, readFunc, saveFunc) \
public: \
    SETTING_GETTER(name, type, key) \
\
private: \
    UISync::uiRead readUI_##name = [](const MainWindow &window, nlohmann::json &json) { \
        JSON::setValue(json, key, readFunc(widget1, widget2)); \
    }; \
    UISync::uiSave saveUI_##name = UI_SAVE_FUNCTION(widget1, saveFunc, type, key); \
    SETTING_BUILDER(key, name)
} // namespace

// Used to register settings into a list
struct SettingList
{
    //Not a real constructor. It is only used to provide a way to add a UISync to the list
    SettingList(UISync sync, std::vector<UISync> &list)
    {
        if (sync.key_.isEmpty() || std::find(list.begin(), list.end(), sync) != list.end())
            return;
        list.emplace_back(sync);
    }
};
class Settings
{
public:
    /* TODO
    //! \brief Checks if the current settings are allowed virtual std::optional<QString> isValid() = 0; 
    */
    Settings() = default;
    Settings(const nlohmann::json &j)
        : json_(j)
    {}

    virtual ~Settings() = default;

    virtual nlohmann::json getJSON() const { return json_; }
    virtual void setJSON(const nlohmann::json &j) { json_ = j; }

    void saveToUi(MainWindow &window) const;
    void readFromUi(const MainWindow &window);

protected:
    std::vector<UISync> uiSyncList_;
    mutable nlohmann::json json_;
};
class PatternSettings final : public Settings
{
public:
    PatternSettings(const nlohmann::json &json);
    PatternSettings(size_t priority, const std::vector<QRegularExpression> &regex);

    std::vector<QRegularExpression> regexes_;
    size_t priority_{0};

    nlohmann::json getJSON() const override;
    void setJSON(const nlohmann::json &j) override;

    std::optional<QString> isValid();

    static constexpr auto patternKey = "Pattern";
    static constexpr auto regexKey = "Regex";
    static constexpr auto priorityKey = "Priority";

private:
    std::vector<QRegularExpression> getPatternRegexFromJSON(const nlohmann::json &json);
    std::optional<size_t> getPatternPriorityFromJSON(const nlohmann::json &json);

    REGISTER_SETTING(bool,
                     bBSAExtractFromBSA,
                     "BSA/bBSAExtractFromBSA",
                     window.mainUI().bsaExtractCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bBSAAddToBSA,
                     "BSA/bBSAAddToBSA",
                     window.mainUI().bsaCreateCheckbox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bBSACreateDummies,
                     "BSA/bBSACreateDummies",
                     window.mainUI().bsaCreateDummiesCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bTexturesNecessary,
                     "Textures/bTexturesNecessary",
                     window.mainUI().texturesNecessaryOptimizationCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bTexturesCompress,
                     "Textures/bTexturesCompress",
                     window.mainUI().texturesCompressCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bTexturesMipmaps,
                     "Textures/bTexturesMipmaps",
                     window.mainUI().texturesMipmapCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING_2_WIDGETS(bool,
                               bTexturesResizeSize,
                               "Textures/Resizing/BySize/Enabled",
                               window.mainUI().texturesResizingBySizeRadioButton,
                               window.mainUI().texturesResizingGroupBox,
                               UISync::readCheckbox,
                               UISync::saveCheckbox)

    REGISTER_SETTING(int,
                     iTexturesTargetHeight,
                     "Textures/Resizing/BySize/Height",
                     window.mainUI().texturesResizingBySizeHeight,
                     UISync::readValue,
                     UISync::saveValue)

    REGISTER_SETTING(int,
                     iTexturesTargetWidth,
                     "Textures/Resizing/BySize/Width",
                     window.mainUI().texturesResizingBySizeHeight,
                     UISync::readValue,
                     UISync::saveValue)

    REGISTER_SETTING_2_WIDGETS(bool,
                               bTexturesResizeRatio,
                               "Textures/Resizing/ByRatio/Enabled",
                               window.mainUI().texturesResizingByRatioRadioButton,
                               window.mainUI().texturesResizingGroupBox,
                               UISync::readCheckbox,
                               UISync::saveCheckbox)

    REGISTER_SETTING(int,
                     iTexturesTargetWidthRatio,
                     "Textures/Resizing/ByRatio/Width",
                     window.mainUI().texturesResizingByRatioWidth,
                     UISync::readValue,
                     UISync::saveValue)

    REGISTER_SETTING(int,
                     iTexturesTargetHeightRatio,
                     "Textures/Resizing/ByRatio/Height",
                     window.mainUI().texturesResizingByRatioHeight,
                     UISync::readValue,
                     UISync::saveValue)

    REGISTER_SETTING(int,
                     iMeshesOptimizationLevel,
                     "Meshes/iMeshesOptimizationLevel",
                     window,
                     UISync::readMeshesOptLevel,
                     UISync::saveMeshesOptLevel)

    REGISTER_SETTING(bool,
                     bAnimationsOptimization,
                     "Animations/bAnimationsOptimization",
                     window.mainUI().animationsNecessaryOptimizationCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bMeshesHeadparts,
                     "Meshes/bMeshesHeadparts",
                     window.mainUI().meshesHeadpartsCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bMeshesResave,
                     "Meshes/bMeshesResave",
                     window.mainUI().meshesResaveCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(DXGI_FORMAT,
                     eTexturesFormat,
                     "Advanced/Textures/eTexturesFormat",
                     window.mainUI().texturesOutputFormat,
                     UISync::readValue<DXGI_FORMAT>,
                     UISync::saveValue)

    REGISTER_SETTING(bsa_archive_type_t,
                     eBSAFormat,
                     "Advanced/BSA/eBSAFormat",
                     window.mainUI().bsaFormat,
                     UISync::readValue<bsa_archive_type_t>,
                     UISync::saveValue)

    REGISTER_SETTING(double,
                     iBSAMaxSize,
                     "Advanced/BSA/iBSAMaxSize",
                     window.mainUI().bsaMaximumSize,
                     UISync::readGigaByteValue,
                     UISync::saveValueGigabyte)

    REGISTER_SETTING(bool,
                     bBSATexturesEnabled,
                     "Advanced/BSA/bBSATexturesEnabled",
                     window.mainUI().bsaTexturesAdvancedGroupBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bsa_archive_type_t,
                     eBSATexturesFormat,
                     "Advanced/BSA/eBSATexturesFormat",
                     window.mainUI().bsaTexturesFormat,
                     UISync::readValue<bsa_archive_type_t>,
                     UISync::saveValue)

    REGISTER_SETTING(double,
                     iBSATexturesMaxSize,
                     "Advanced/BSA/iBSATexturesMaxSize",
                     window.mainUI().bsaTexturesMaximumSize,
                     UISync::readGigaByteValue,
                     UISync::saveValueGigabyte)

    REGISTER_SETTING(bool,
                     bBSAIsStandard,
                     "Advanced/BSA/bBSAIsStandard",
                     nullptr,
                     UISync::nullReadFunc,
                     UISync::nullSaveFunc)

    REGISTER_SETTING(bool,
                     bBSAIsTexture,
                     "Advanced/BSA/bBSAIsTexture",
                     nullptr,
                     UISync::nullReadFunc,
                     UISync::nullSaveFunc)

    REGISTER_SETTING(bool,
                     bBSAIsUncompressible,
                     "Advanced/BSA/bBSAIsUncompressible",
                     nullptr,
                     UISync::nullReadFunc,
                     UISync::nullSaveFunc)

    REGISTER_SETTING(bool,
                     bTexturesForceConvert,
                     "Advanced/Textures/bTexturesForceConvert",
                     window.mainUI().texturesTgaConversionCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(NiFileVersion,
                     eMeshesFileVersion,
                     "Advanced/Meshes/eMeshesFileVersion",
                     window.mainUI().meshesVersion,
                     UISync::readValue<NiFileVersion>,
                     UISync::saveValue)

    REGISTER_SETTING(uint,
                     iMeshesStream,
                     "Advanced/Meshes/iMeshesStream",
                     window.mainUI().meshesStream,
                     UISync::readValue<uint>,
                     UISync::saveValue)

    REGISTER_SETTING(uint,
                     iMeshesUser,
                     "Advanced/Meshes/iMeshesUser",
                     window.mainUI().meshesUser,
                     UISync::readValue<uint>,
                     UISync::saveValue)

    REGISTER_SETTING(hkPackFormat,
                     eAnimationsFormat,
                     "Advanced/Animations/eAnimationsFormat",
                     nullptr,
                     UISync::nullReadFunc,
                     UISync::nullSaveFunc)
};

class GeneralSettings final : public Settings
{
    Q_GADGET
public:
    GeneralSettings() = default;
    GeneralSettings(nlohmann::json j);

    /*!
   * \brief Checks if the current settings are allowed
   */
    QString isValid() const;

public:
    //All the code below is used to register settings

    REGISTER_SETTING(bool,
                     bDebugLog,
                     "General/bDebugLog",
                     window.mainUI().actionEnable_debug_log,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bDryRun,
                     "General/bDryRun",
                     window.mainUI().dryRunCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(OptimizationMode,
                     eMode,
                     "General/eMode",
                     window.mainUI().modeChooserComboBox,
                     UISync::readValue<OptimizationMode>,
                     UISync::saveValue)

    REGISTER_SETTING(QString,
                     sUserPath,
                     "General/sUserPath",
                     window.mainUI().userPathTextEdit,
                     UISync::readText,
                     UISync::saveText)

    REGISTER_SETTING(bool,
                     bBSACompact,
                     "BSA/bBSACompact",
                     window.mainUI().bsaLeastBsasCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(bool,
                     bBSADeleteBackup,
                     "BSA/bBSADeleteBackup",
                     window.mainUI().bsaDeleteBackupsCheckBox,
                     UISync::readCheckbox,
                     UISync::saveCheckbox)

    REGISTER_SETTING(QString,
                     sBSAExtension,
                     "Advanced/BSA/sBSAExtension",
                     window.mainUI().bsaExtension,
                     UISync::readText,
                     UISync::saveText)

    REGISTER_SETTING(QString,
                     sBSASuffix,
                     "Advanced/BSA/sBSASuffix",
                     window.mainUI().bsaSuffix,
                     UISync::readText,
                     UISync::saveText)

    REGISTER_SETTING(QString,
                     sBSATexturesSuffix,
                     "Advanced/BSA/sBSATexturesSuffix",
                     window.mainUI().bsaTexturesSuffix,
                     UISync::readText,
                     UISync::saveText)

    REGISTER_SETTING(bool,
                     bBSATabEnabled,
                     "Advanced/BSA/bBSATabEnabled",
                     window.mainUI().bsaTab,
                     UISync::readTab,
                     UISync::saveTab)

    REGISTER_SETTING(bool,
                     bTexturesTabEnabled,
                     "Advanced/Textures/bTexturesTabEnabled",
                     window.mainUI().texturesTab,
                     UISync::readTab,
                     UISync::saveTab)

    REGISTER_SETTING(bool,
                     bMeshesTabEnabled,
                     "Advanced/Meshes/bMeshesTabEnabled",
                     window.mainUI().meshesTab,
                     UISync::readTab,
                     UISync::saveTab)

    REGISTER_SETTING(bool,
                     bAnimationsTabEnabled,
                     "Advanced/Animations/bAnimationsTabEnabled",
                     window.mainUI().AnimationsTab,
                     UISync::readTab,
                     UISync::saveTab)

//Cleanup
#undef UI_READ_FUNCTION
#undef UI_SAVE_FUNCTION
#undef REGISTER_SETTING
#undef REGISTER_SETTING_2_WIDGETS
#undef REGISTER_SETTING_NO_UI
};
} // namespace CAO

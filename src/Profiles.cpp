/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Profiles.h"
#include "Manager.h"

Profiles Profiles::_instance = Profiles();

Profiles::Profiles()
{
    findProfiles(QDir("profiles"));
    loadProfile("Default");
};

size_t Profiles::findProfiles(const QDir &dir)
{
    _profileDir = dir;
    size_t counter = 0;
    _profiles.clear();
    for (const auto &subDir : _profileDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString ini = dir.absoluteFilePath(subDir + "/profile.ini");
        if (QFile::exists(ini))
        {
            _profiles << subDir;
            ++counter;
        }
    }
    PLOG_VERBOSE << "Profiles found: " << _profiles.join('\n');
    return counter;
}

void Profiles::loadProfile(const QString &newProfile)
{
    if (!exists(newProfile))
        throw std::runtime_error("This profile does not exist: " + newProfile.toStdString());

    _currentProfile = newProfile;
    QSettings s("profiles/common.ini", QSettings::IniFormat);
    s.setValue("profile", _currentProfile);

    _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/" + _currentProfile + ".html");
    QString folder = _profileDir.absoluteFilePath(_currentProfile);
    _profileSettings = new QSettings(folder + "/profile.ini", QSettings::IniFormat, this);
    _optionsSettings = new QSettings(folder + "/settings.ini", QSettings::IniFormat, this);

    readFromIni();
}

bool Profiles::exists(const QString &profile)
{
    _instance.findProfiles(QDir("profiles"));
    return _instance._profiles.contains(profile);
}

QStringList Profiles::list()
{
    return getInstance()._profiles;
}

void Profiles::create(const QString &name)
{
    FilesystemOperations::copyDir(_instance._profileDir.absoluteFilePath("default"),
                                  _instance._profileDir.absoluteFilePath(name),
                                  false);
    _instance.findProfiles(_instance._profileDir);
}

QFile Profiles::getFile(const QString &filename)
{
    const QDir &currentProfileDir(_instance._profileDir.absoluteFilePath(_instance.currentProfile()));
    const QDir &defaultProfileDir(_instance._profileDir.absoluteFilePath("Default"));

    if (currentProfileDir.exists(filename))
        return QFile(currentProfileDir.filePath(filename));
    else if (defaultProfileDir.exists(filename))
        return QFile(defaultProfileDir.filePath(filename));
    else
        return QFile();
}

void Profiles::saveToIni()
{
    _profileSettings->beginGroup("BSA");
    _profileSettings->setValue("bsaEnabled", _bsaEnabled);
    _profileSettings->setValue("bsaFormat", _bsaFormat);
    _profileSettings->setValue("bsaTexturesFormat", _bsaTexturesFormat);
    _profileSettings->setValue("maxBsaUncompressedSize", _maxBsaUncompressedSize);
    _profileSettings->setValue("hasBsaTextures", _hasBsaTextures);
    _profileSettings->setValue("maxBsaTexturesSize", _maxBsaTexturesSize);
    _profileSettings->setValue("bsaExtension", _bsaExtension);
    _profileSettings->setValue("bsaSuffix", _bsaSuffix);
    _profileSettings->setValue("bsaTexturesSuffix", _bsaTexturesSuffix);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Meshes");
    _profileSettings->setValue("meshesEnabled", _meshesEnabled);
    _profileSettings->setValue("meshesFileVersion", _meshesFileVersion);
    _profileSettings->setValue("meshesStream", _meshesStream);
    _profileSettings->setValue("meshesUser", _meshesUser);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Animations");
    _profileSettings->setValue("animationsEnabled", _animationsEnabled);
    _profileSettings->setValue("animationFormat", _animationFormat);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Textures");
    _profileSettings->setValue("texturesEnabled", _texturesEnabled);
    _profileSettings->setValue("texturesFormat", _texturesFormat);
    _profileSettings->setValue("texturesConvertTga", _texturesConvertTga);
    _profileSettings->setValue("texturesUnwantedFormats", _texturesUnwantedFormats);
    _profileSettings->setValue("texturesCompressInterface", _texturesCompressInterface);
    _profileSettings->endGroup();
}

void Profiles::readFromIni()
{
    _profileSettings->beginGroup("BSA");
    _bsaEnabled = _profileSettings->value("bsaEnabled").toBool();
    _bsaFormat = static_cast<bsa_archive_type_t>(_profileSettings->value("bsaFormat").toInt());
    _bsaTexturesFormat = static_cast<bsa_archive_type_t>(_profileSettings->value("bsaTexturesFormat").toInt());
    _maxBsaUncompressedSize = _profileSettings->value("maxBsaUncompressedSize").toDouble();
    _hasBsaTextures = _profileSettings->value("hasBsaTextures").toBool();
    _maxBsaTexturesSize = _profileSettings->value("maxBsaTexturesSize").toDouble();
    _bsaExtension = _profileSettings->value("bsaExtension").toString();
    _bsaSuffix = _profileSettings->value("bsaSuffix").toString();
    _bsaTexturesSuffix = _profileSettings->value("bsaTexturesSuffix").toString();
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Meshes");
    _meshesEnabled = _profileSettings->value("meshesEnabled").toBool();
    _meshesFileVersion = static_cast<NiFileVersion>(_profileSettings->value("meshesFileVersion").toInt());
    _meshesStream = _profileSettings->value("meshesStream").toUInt();
    _meshesUser = _profileSettings->value("meshesUser").toUInt();
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Animations");
    _animationsEnabled = _profileSettings->value("animationsEnabled").toBool();
    _animationFormat = static_cast<hkPackFormat>(_profileSettings->value("animationFormat").toInt());
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Textures");
    _texturesEnabled = _profileSettings->value("texturesEnabled").toBool();
    _texturesFormat = _profileSettings->value("texturesFormat").value<DXGI_FORMAT>();
    _texturesConvertTga = _profileSettings->value("texturesConvertTga").toBool();
    _texturesUnwantedFormats = _profileSettings->value("texturesUnwantedFormats").toList();
    _texturesCompressInterface = _profileSettings->value("texturesCompressInterface").toBool();
    _profileSettings->endGroup();
}
#ifdef GUI
void Profiles::loadProfile(Ui::MainWindow *ui)
{
    _currentProfile = uiToGame(ui);
    saveToUi(ui);
    loadProfile(_currentProfile);
}

QString Profiles::uiToGame(Ui::MainWindow *ui)
{
    return ui->presets->currentText();
}

void Profiles::saveToUi(Ui::MainWindow *ui)
{
    const auto iterateComboBox = [](QComboBox *box, const QVariant data) {
        for (int i = 0; i < box->count(); ++i)
        {
            if (box->itemData(i) == data)
            {
                box->setCurrentIndex(i);
                break;
            }
        }
    };

    iterateComboBox(ui->bsaFormat, _bsaFormat);
    iterateComboBox(ui->bsaTexturesFormat, _bsaTexturesFormat);
    ui->bsaMaximumSize->setValue(_maxBsaUncompressedSize / GigaByte);
    ui->bsaTexturesAdvancedGroupBox->setChecked(_hasBsaTextures);
    ui->bsaTexturesMaximumSize->setValue(_maxBsaTexturesSize / GigaByte);
    ui->bsaExtension->setText(_bsaExtension);
    ui->bsaSuffix->setText(_bsaSuffix);
    ui->bsaTexturesSuffix->setText(_bsaTexturesSuffix);

    iterateComboBox(ui->meshesUser, _meshesUser);
    iterateComboBox(ui->meshesStream, _meshesStream);
    iterateComboBox(ui->meshesVersion, _meshesFileVersion);

    //Animation format is not working when converting from amd64, thus not added to UI

    iterateComboBox(ui->texturesOutputFormat, _texturesFormat);
    ui->texturesTgaConversionCheckBox->setChecked(_texturesConvertTga);
    ui->texturesCompressInterfaceCheckBox->setChecked(_texturesCompressInterface);

    QStringList unwantedFormats;
    for (const QVariant &variant : _texturesUnwantedFormats)
    {
        DXGI_FORMAT format = variant.value<DXGI_FORMAT>();
        unwantedFormats << QString::fromStdString(dxgiFormatToString(format));
    }
    unwantedFormats.removeDuplicates();
    ui->texturesUnwantedFormats->setPlainText(unwantedFormats.join('\n'));
}

void Profiles::readFromUi(Ui::MainWindow *ui)
{
    _bsaFormat = ui->bsaFormat->currentData().value<bsa_archive_type_e>();
    _bsaTexturesFormat = ui->bsaTexturesFormat->currentData().value<bsa_archive_type_e>();

    _maxBsaUncompressedSize = ui->bsaMaximumSize->value() * GigaByte;
    _hasBsaTextures = ui->bsaTexturesAdvancedGroupBox->isChecked();
    _maxBsaTexturesSize = ui->bsaTexturesMaximumSize->value() * GigaByte;
    _bsaExtension = ui->bsaExtension->text();
    _bsaSuffix = ui->bsaSuffix->text();
    _bsaTexturesSuffix = ui->bsaTexturesSuffix->text();

    _meshesUser = ui->meshesUser->currentData().toUInt();
    _meshesStream = ui->meshesStream->currentData().toUInt();
    _meshesFileVersion = ui->meshesVersion->currentData().value<NiFileVersion>();
    //Animation format is not working currently, thus not added to UI

    _texturesFormat = ui->texturesOutputFormat->currentData().value<DXGI_FORMAT>();
    _texturesConvertTga = ui->texturesTgaConversionCheckBox->isChecked();
    _texturesCompressInterface = ui->texturesCompressInterfaceCheckBox->isChecked();

    _texturesUnwantedFormats.clear();
    for (const auto &line : ui->texturesUnwantedFormats->toPlainText().split('\n'))
    {
        const DXGI_FORMAT format = stringToDxgiFormat(line.toStdString());
        if (!_texturesUnwantedFormats.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            _texturesUnwantedFormats += format;
    }
}
#endif

Profiles &Profiles::getInstance()
{
    return _instance;
}
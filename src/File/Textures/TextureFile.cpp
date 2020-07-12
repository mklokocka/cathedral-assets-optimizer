/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TextureFile.hpp"
#include "Utils/ScopeGuard.hpp"

namespace CAO {

TextureFile::TextureFile()
{
    // Initialize COM (needed for WIC)
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw std::runtime_error("Failed to initialize COM. Textures processing won't work.");
}

int TextureFile::loadFromDisk(const QString &filePath)
{
    ScopeGuard guard([this] { this->reset(); });

    loadHelper<TextureResource>(filePath);

    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(filePath).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    //Trying to guess texture type. DDS is more common
    auto image = static_cast<TextureResource *>(&getFile(false));
    if (FAILED(LoadFromDDSFile(wFilePath, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAFile(wFilePath, &_info, *image)))
            return 1;

    if (!makeTypelessUNORM(*image))
        return 2;
    image->origFormat = _info.format;

    guard.clear();
    return 0;
}

int TextureFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    ScopeGuard guard([this] { this->reset(); });

    loadHelper<TextureResource>(fileName);

    auto image = static_cast<TextureResource *>(&getFile(false));
    if (FAILED(LoadFromDDSMemory(pSource, size, DirectX::DDS_FLAGS_NONE, &_info, *image)))
        if (FAILED(LoadFromTGAMemory(pSource, size, &_info, *image)))
            return 1;

    if (!makeTypelessUNORM(*image))
        return 2;
    image->origFormat = _info.format;

    guard.clear();
    return 0;
}

int TextureFile::saveToMemory(std::iostream &ostr) const
{
    if (!saveToMemoryHelper())
        return 1;

    auto image = static_cast<const TextureResource *>(&getFile());

    const auto img = image->GetImages();
    if (!img)
        return 2;
    const size_t nimg = image->GetImageCount();

    DirectX::Blob blob;
    if (FAILED(DirectX::SaveToDDSMemory(img, nimg, _info, DirectX::DDS_FLAGS_NONE, blob)))
        return 3;

    ostr.write(static_cast<char *>(blob.GetBufferPointer()), blob.GetBufferSize());
    if (!ostr)
        return 4;

    return 0;
}

int TextureFile::saveToDisk(const QString &filePath) const
{
    if (!saveToDiskHelper(filePath))
        return 2;

    auto image     = static_cast<const TextureResource *>(&getFile());
    const auto img = image->GetImages();
    if (!img)
        return 1;
    const size_t nimg = image->GetImageCount();

    QString newName = filePath;
    bool isTGA      = false;
    if (newName.contains(".tga", Qt::CaseInsensitive))
    {
        isTGA = true;
        newName.replace(".tga", ".dds", Qt::CaseInsensitive);
    }

    // Write texture
    wchar_t wFilePath[1024];
    QDir::toNativeSeparators(newName).toWCharArray(wFilePath);
    wFilePath[filePath.length()] = '\0';

    const HRESULT hr = SaveToDDSFile(img, nimg, _info, DirectX::DDS_FLAGS_NONE, wFilePath);
    if (SUCCEEDED(hr) && isTGA)
        QFile(getInputFilePath()).remove();
    return FAILED(hr);
}

bool TextureFile::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    if (!setFileHelper<TextureResource>(std::move(file), optimizedFile))
        return false;
    _info = static_cast<TextureResource *>(&getFile(false))->GetMetadata();
    return true;
}

void TextureFile::reset()
{
    resetHelper();
    _info = DirectX::TexMetadata();
}

bool TextureFile::makeTypelessUNORM(TextureResource &image)
{
    if (DirectX::IsTypeless(_info.format))
    {
        _info.format = DirectX::MakeTypelessUNORM(_info.format);
        if (DirectX::IsTypeless(_info.format))
            return false;

        image.OverrideFormat(_info.format);
    }
    return true;
}
} // namespace CAO

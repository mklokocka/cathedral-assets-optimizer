/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BSA.hpp"
#include "Commands/Plugins/PluginsOperations.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {
BSA BSA::getBSA(const BSAType &type, const GeneralSettings &settings)
{
    BSA bsa;

    switch (type)
    {
        case StandardBsa:
            bsa.type    = BSAType::StandardBsa;
            bsa.maxSize = settings.iBSAMaxSize();
            bsa.format  = settings.eBSAFormat();
            break;
        case TexturesBsa:
            bsa.type    = BSAType::TexturesBsa;
            bsa.maxSize = settings.iBSATexturesMaxSize();
            bsa.format  = settings.eBSATexturesFormat();
            break;
        case UncompressableBsa:
            bsa.type    = BSAType::UncompressableBsa;
            bsa.maxSize = settings.iBSAMaxSize();
            bsa.format  = settings.eBSAFormat();
            break;
    }
    return bsa;
}

BSA::BSA(double maxSize, qint64 size, BSAType type)
    : filesSize(size)
    , maxSize(maxSize)
    , type(type)
{
}

void BSA::name(const QString &folder, const GeneralSettings &settings)
{
    const auto &bsaSuffix    = settings.sBSASuffix();
    const auto &bsaTexSuffix = settings.sBSATexturesSuffix();
    const QString &suffix    = type == TexturesBsa ? bsaTexSuffix : bsaSuffix;
    path                     = folder + "/" + PluginsOperations::findPlugin(folder, settings) + suffix;
    PLOG_VERBOSE << "Named " << type << path;
}

void BSA::mergeBSAs(std::vector<BSA> &list, bool merge)
{
    auto secondBegin = std::partition(list.begin(), list.end(), [](const BSA &val) {
        return val.type == StandardBsa;
    });

    auto thirdBegin = std::partition(secondBegin, list.end(), [](const BSA &val) {
        return val.type == TexturesBsa;
    });

    auto sortBSADescending = [](auto &&one, auto &&two) { return one.filesSize > two.filesSize; };
    //BSAs small enough to be merged
    auto notMaxSize = [](auto &&left, auto &&right) {
        return left.filesSize + right.filesSize < left.maxSize;
    };

    auto sortMerge = [&sortBSADescending, &notMaxSize](auto begin, auto end) {
        std::sort(begin, end, sortBSADescending);
        return merge_if(begin, end, notMaxSize);
    };

    auto firstEnd = sortMerge(list.begin(), secondBegin);
    auto secondEnd = sortMerge(secondBegin, thirdBegin);
    auto thirdEnd = sortMerge(thirdBegin, list.end());

    std::vector<BSA> result;
    result.reserve(list.size());

    std::move(list.begin(), firstEnd, std::back_inserter(result));
    std::move(secondBegin, secondEnd, std::back_inserter(result));
    std::move(thirdBegin, thirdEnd, std::back_inserter(result));

    if (merge)
        result.erase(sortMerge(result.begin(), result.end()), result.end());

    list = result;
}

BSA &BSA::operator+=(const BSA &other)
{
    filesSize += other.filesSize;
    files + other.files;
    if (type != other.type)
        type = StandardBsa;
    return *this;
}

BSA BSA::operator+(const BSA &other) const
{
    BSA copy = *this;
    copy += other;
    return copy;
}

bool BSA::operator==(const BSA &other) const
{
    return path == other.path && filesSize == other.filesSize && files == other.files
           && qFuzzyCompare(maxSize, other.maxSize) && type == other.type && format == other.format;
}
} // namespace CAO

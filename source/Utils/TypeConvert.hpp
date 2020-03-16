/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {

inline QRegularExpression toRegex(const QString &regexString, bool useWildcard)
{
    if (useWildcard)
        return QRegularExpression(QRegularExpression::wildcardToRegularExpression(regexString));
    else
        return QRegularExpression(regexString);
}

inline std::vector<QRegularExpression> toRegexVector(const QStringList &regexStrings, bool useWildcard)
{
    std::vector<QRegularExpression> regexes;
    std::transform(regexStrings.begin(),
                   regexStrings.end(),
                   std::back_inserter(regexes),
                   [useWildcard](const QString &str) { return toRegex(str, useWildcard); });
    return regexes;
}

inline QString toQString(const QRegularExpression &regex)
{
    return regex.pattern();
}

inline QString toQString(const std::string &str)
{
    return QString::fromStdString(str);
}

inline std::string toString(const QString &str)
{
    return str.toStdString();
}

template<typename Container>
inline QStringList toStringList(Container &&cont)
{
    QStringList stringList;
    auto toqs = [](auto &&e) { return toQString(e); };
    std::transform(cont.cbegin(), cont.cend(), std::back_inserter(stringList), toqs);
    return stringList;
} // namespace CAO

template<typename Container>
inline std::vector<std::string> toStringVector(Container &&cont)
{
    std::vector<std::string> stringVec;
    std::transform(cont.cbegin(), cont.cend(), std::back_inserter(stringVec), toString);
    return stringVec;
}
} // namespace CAO

/****************************************************************************
**
** Copyright (C) 2020 Kuba Ober and The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "localeutils.h"

#include <private/qlocale_p.h>
#include <private/qtranslator_p.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

namespace LocaleUtils {

// See numerus.cpp for:
// getNumerusInfo
// getNumerusInfoString

} // namespace LocaleUtils

QLocale LanguageCode::toLocale() const
{
    return QLocale(m_language, m_country);
}

LanguageCode LanguageCode::fromLatin1(const QByteArray &languageCode)
{
    return LanguageCode::fromString(QString::fromLatin1(languageCode));
}

LanguageCode LanguageCode::fromString(QStringRef languageCode)
{
    if (languageCode.isEmpty())
        return {};
    return LanguageCode::fromString(languageCode.toString());
}

LanguageCode LanguageCode::fromString(const QString &languageCode)
{
    if (languageCode.isEmpty())
        return {};
    LanguageCode lc;
    QLocale::Script script;
    QLocalePrivate::getLangAndCountry(languageCode, lc.m_language, script, lc.m_country);
    return lc;
}

QString LanguageCode::toString() const
{
    QString result = QLocalePrivate::languageToCode(m_language);
    if (m_language != QLocale::C && m_country != QLocale::AnyCountry) {
        result.append(QLatin1Char('_'));
        result.append(QLocalePrivate::countryToCode(m_country));
    }
    return result;
}

QString LanguageCode::toLangPart() const
{
    return QLocalePrivate::languageToCode(m_language);
}

QString LanguageCode::toCountryPart() const
{
    if (m_language != QLocale::C && m_country != QLocale::AnyCountry)
        return QLocalePrivate::countryToCode(m_country);
    return {};
}

QT_END_NAMESPACE

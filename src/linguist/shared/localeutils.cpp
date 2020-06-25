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

struct LanguageLess
{
    bool operator()(QLocale::Language lang, const QString &text) const
    {
        return QLocale::languageToString(lang) < text;
    }
    bool operator()(const QString &text, QLocale::Language lang) const
    {
        return text < QLocale::languageToString(lang);
    }
};

struct CountryLess
{
    bool operator()(QLocale::Country country, const QString &text) const
    {
        return QLocale::countryToString(country) < text;
    }
    bool operator()(const QString &text, QLocale::Country country) const
    {
        return text < QLocale::countryToString(country);
    }
};

static QString englishTitleCase(QString str)
{
    str = std::move(str).toLower();
    if (!str.isEmpty())
        str[0] = str[0].toUpper();
    return str;
}

static QList<QLocale> getAllLocales()
{
    QList<QLocale> allLocales =
            QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    return allLocales;
}

/** Returns the list of all languages, sorted in order of their English names */
const QList<QLocale::Language> &getAllLanguages()
{
    static const QList<QLocale::Language> languages = [] {
        QMap<QString, QLocale::Language> languages;
        for (const QLocale &locale : getAllLocales()) {
            QString languageName = QLocale::languageToString(locale.language());
            languages.insert(languageName, locale.language());
        }
        return languages.values();
    }();
    return languages;
}

/** Returns the list of all countries, sorted in order of their English names */
const QList<QLocale::Country> &getAllCountries()
{
    static const QList<QLocale::Country> countries = [] {
        QMap<QString, QLocale::Country> countries;
        for (const QLocale &locale : getAllLocales()) {
            QString countryName = QLocale::countryToString(locale.country());
            countries.insert(countryName, locale.country());
        }
        return countries.values();
    }();
    return countries;
}

/** Converts an English language name, in any capitalization, or a BCP47 code,
 * to QLocale::Lanugage. Returns AnyLanguage if the conversion has failed. */
QLocale::Language stringToLanguage(const QString &text)
{
    if (text.isEmpty())
        return {};
    bool const hasUnderlines = text.contains(QLatin1Char('_'));

    if (!hasUnderlines) {
        // Search for language name
        auto &langs = getAllLanguages();
        auto range = std::equal_range(langs.begin(), langs.end(), englishTitleCase(text),
                                      LanguageLess());
        if (range.first != langs.end())
            return *range.first;
    }

    // Search for language code
    QLocale locale(text);
    if (locale != QLocale::c())
        return locale.language();
    return {};
}

/** Converts an English country name, in any capitalization, or an ISO3166 country
 * code, or a BCP47 code, to QLocale::Country. Returns AnyCountry if the conversion
 * has failed. */
QLocale::Country stringToCountry(const QString &text)
{
    if (text.isEmpty())
        return {};
    bool const hasUnderlines = text.contains(QLatin1Char('_'));

    QLocale locale;
    if (!hasUnderlines) {
        // Search for country name
        auto &countries = getAllCountries();
        auto range = std::equal_range(countries.begin(), countries.end(), englishTitleCase(text),
                                      CountryLess());
        if (range.first != countries.end())
            return *range.first;

        // Search for country code
        locale = QLocale(QStringLiteral("en_%s").arg(text.toUpper()));
    } else {
        // Search for BCP47 code
        locale = QLocale(text);
    }

    if (locale != QLocale::c())
        return locale.country();
    return {};
}

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

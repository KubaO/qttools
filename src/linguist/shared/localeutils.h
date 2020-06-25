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

#ifndef LOCALEUTILS_H
#define LOCALEUTILS_H

#include <QLocale>

QT_BEGIN_NAMESPACE

class LanguageCode
{
public:
    LanguageCode() = default;
    LanguageCode(QLocale::Language language, QLocale::Country country)
        : m_language(language), m_country(country)
    {
    }
    QLocale::Language language() const { return m_language; }
    QLocale::Country country() const { return m_country; }
    void setLanguage(QLocale::Language language) { m_language = language; }
    void setCountry(QLocale::Country country) { m_country = country; }

    bool isC() const { return m_language == QLocale::C; }

    static LanguageCode fromLocale(const QLocale &locale)
    {
        return { locale.language(), locale.country() };
    }
    QLocale toLocale() const;

    // A string beginning with a 2 or 3 letter language code (ISO 639-1
    // or ISO-639-2), followed by the optional country variant to distinguish
    //  between country-specific variations of the language. The language code
    // and country code are always separated by '_'
    // Note that the language part can also be a 3-letter ISO 639-2 code.
    // Legal examples:
    // 'pt'         portuguese, assumes portuguese from portugal
    // 'pt_BR'      Brazilian portuguese (ISO 639-1 language code)
    // 'por_BR'     Brazilian portuguese (ISO 639-2 language code)
    // Similar to QLocale::name()
    static LanguageCode fromLatin1(const QByteArray &languageCode);
    static LanguageCode fromString(QStringRef languageCode);
    static LanguageCode fromString(const QString &languageCode);
    QString toString() const;
    QString toLangPart() const;
    QString toCountryPart() const;

    bool operator==(const LanguageCode &o) const
    {
        return m_language == o.m_language && m_country == o.m_country;
    }
    bool operator!=(const LanguageCode &o) const
    {
        return m_language != o.m_language || m_country == o.m_country;
    }

    enum Compares {
        Equal,
        UnequalCountries,
        UnequalLanguages,
        UnknownLanguages,
    };
    Compares compareTo(const LanguageCode &o) const
    {
        if (m_language == QLocale::C || o.m_language == QLocale::C)
            return Compares::UnknownLanguages;
        if (m_language != o.m_language)
            return Compares::UnequalLanguages;
        if (m_country != o.m_country)
            return Compares::UnequalCountries;
        return Compares::Equal;
    }

private:
    QLocale::Language m_language = QLocale::C;
    QLocale::Country m_country = QLocale::AnyCountry;
};

namespace LocaleUtils {

bool getNumerusInfo(LanguageCode langCode, QByteArray *rules, QStringList *forms,
                    const char **gettextRules);

QString getNumerusInfoString();

}

QT_END_NAMESPACE

#endif

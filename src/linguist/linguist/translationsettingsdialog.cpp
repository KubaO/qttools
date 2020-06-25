/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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


#include "translationsettingsdialog.h"
#include "messagemodel.h"
#include "phrase.h"

#include <QtCore/QLocale>

QT_BEGIN_NAMESPACE

TranslationSettingsDialog::TranslationSettingsDialog(QWidget *parent)
  : QDialog(parent)
{
    m_ui.setupUi(this);

    for (int i = QLocale::C + 1; i < QLocale::LastLanguage; ++i) {
        QString lang = QLocale::languageToString(QLocale::Language(i));
        auto loc = QLocale(QLocale::Language(i));
        if (loc.language() != QLocale::English) {
            QString nln = loc.nativeLanguageName();
            if (!nln.isEmpty()) {
                //: <english> (<endonym>)  (language and country names)
                lang = tr("%1 (%2)").arg(lang, nln);
            }
        }
        m_ui.srcCbLanguageList->addItem(lang, QVariant(i));
    }
    m_ui.srcCbLanguageList->model()->sort(0, Qt::AscendingOrder);
    m_ui.srcCbLanguageList->insertItem(0, QLatin1String("POSIX"), QVariant(QLocale::C));

    m_ui.tgtCbLanguageList->setModel(m_ui.srcCbLanguageList->model());
}

void TranslationSettingsDialog::setDataModel(DataModel *dataModel)
{
    m_dataModel = dataModel;
    m_phraseBook = 0;
    QString fn = QFileInfo(dataModel->srcFileName()).baseName();
    setWindowTitle(tr("Settings for '%1' - Qt Linguist").arg(fn));
}

void TranslationSettingsDialog::setPhraseBook(PhraseBook *phraseBook)
{
    m_phraseBook = phraseBook;
    m_dataModel = 0;
    QString fn = QFileInfo(phraseBook->fileName()).baseName();
    setWindowTitle(tr("Settings for '%1' - Qt Linguist").arg(fn));
}

static void fillCountryCombo(const QVariant &lng, QComboBox *combo)
{
    combo->clear();
    QLocale::Language lang = QLocale::Language(lng.toInt());
    if (lang != QLocale::C) {
        foreach (QLocale::Country cntr, QLocale::countriesForLanguage(lang)) {
            QString country = QLocale::countryToString(cntr);
            auto loc = QLocale(lang, cntr);
            if (loc.language() != QLocale::English) {
                QString ncn = loc.nativeCountryName();
                if (!ncn.isEmpty())
                    country = TranslationSettingsDialog::tr("%1 (%2)").arg(country, ncn);
            }
            combo->addItem(country, QVariant(cntr));
        }
        combo->model()->sort(0, Qt::AscendingOrder);
    }
    combo->insertItem(0, TranslationSettingsDialog::tr("Any Country"), QVariant(QLocale::AnyCountry));
    combo->setCurrentIndex(0);
}

void TranslationSettingsDialog::on_srcCbLanguageList_currentIndexChanged(int idx)
{
    fillCountryCombo(m_ui.srcCbLanguageList->itemData(idx), m_ui.srcCbCountryList);
}

void TranslationSettingsDialog::on_tgtCbLanguageList_currentIndexChanged(int idx)
{
    fillCountryCombo(m_ui.tgtCbLanguageList->itemData(idx), m_ui.tgtCbCountryList);
}

void TranslationSettingsDialog::on_buttonBox_accepted()
{
    LanguageCode toLang, fromLang;

    int itemindex = m_ui.tgtCbLanguageList->currentIndex();
    QVariant var = m_ui.tgtCbLanguageList->itemData(itemindex);
    toLang.setLanguage(QLocale::Language(var.toInt()));

    itemindex = m_ui.tgtCbCountryList->currentIndex();
    var = m_ui.tgtCbCountryList->itemData(itemindex);
    toLang.setCountry(QLocale::Country(var.toInt()));

    itemindex = m_ui.srcCbLanguageList->currentIndex();
    var = m_ui.srcCbLanguageList->itemData(itemindex);
    fromLang.setLanguage(QLocale::Language(var.toInt()));

    itemindex = m_ui.srcCbCountryList->currentIndex();
    var = m_ui.srcCbCountryList->itemData(itemindex);
    fromLang.setCountry(QLocale::Country(var.toInt()));

    if (m_phraseBook) {
        m_phraseBook->setToLanguage(toLang);
        m_phraseBook->setFromLanguage(fromLang);
    } else {
        m_dataModel->setToLanguage(toLang);
        m_dataModel->setFromLanguage(fromLang);
    }

    accept();
}

void TranslationSettingsDialog::showEvent(QShowEvent *)
{
    LanguageCode toLang, fromLang;

    if (m_phraseBook) {
        toLang = m_phraseBook->toLanguage();
        fromLang = m_phraseBook->fromLanguage();
    } else {
        toLang = m_dataModel->toLanguage();
        fromLang = m_dataModel->fromLanguage();
    }

    int itemindex = m_ui.tgtCbLanguageList->findData(QVariant(int(toLang.language())));
    m_ui.tgtCbLanguageList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);

    itemindex = m_ui.tgtCbCountryList->findData(QVariant(int(toLang.country())));
    m_ui.tgtCbCountryList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);

    itemindex = m_ui.srcCbLanguageList->findData(QVariant(int(fromLang.language())));
    m_ui.srcCbLanguageList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);

    itemindex = m_ui.srcCbCountryList->findData(QVariant(int(fromLang.country())));
    m_ui.srcCbCountryList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);
}

QT_END_NAMESPACE

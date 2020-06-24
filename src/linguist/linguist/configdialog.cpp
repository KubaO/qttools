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

/*  TRANSLATOR ConfigDialog */

#include "configdialog.h"
#include "configuration.h"
#include "substitutionmodel.h"

#include <QLineEdit>

QT_BEGIN_NAMESPACE

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent), m_substs(new SubstitutionModel(this))
{
    setupUi(this);

    for (auto size : QFontDatabase::standardSizes()) {
        auto const strSize = QString::number(size);
        cbAppFontSize->addItem(strSize);
        cbEditorFontSize->addItem(strSize);
    }
    QIcon undoIcon(QLatin1String(":/images/win/undo.png"));
    tbAppFont->setIcon(undoIcon);
    tbEditorFont->setIcon(undoIcon);

    substitutionsView->setModel(m_substs);
    substitutionsView->resizeColumnsToContents();
    substitutionsView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto const clicked = &QAbstractButton::clicked;
    connect(tbAppFont, clicked, this, &ConfigDialog::resetAppFont);
    connect(fcbAppFont, QOverload<int>::of(&QFontComboBox::activated), this,
            &ConfigDialog::appFontActivated);
    connect(cbAppFontSize, QOverload<int>::of(&QComboBox::activated), this,
            &ConfigDialog::appFontActivated);
    connect(tbEditorFont, clicked, this, &ConfigDialog::resetEditorFontSize);
    connect(cbEditorFontSize, &QComboBox::textActivated, tbEditorFont,
            [this] { tbEditorFont->setEnabled(true); });
    connect(substitutionsAddRow, clicked, this, &ConfigDialog::addSubstitutionRow);
    connect(substitutionsRemoveRow, clicked, this, &ConfigDialog::removeSubstitutionRow);
}

void ConfigDialog::setFrom(const Configuration *config)
{
    setAppFont(config->appFont());
    setEditorFontSize(config->editorFontSize());
    m_substs->setSubstitutions(config->sourcePathRewrites());
}

void ConfigDialog::applyTo(Configuration *config) const
{
    config->setAppFont(appFont());
    config->setEditorFontSize(editorFontSize());
    config->setSourcePathRewrites(m_substs->substitutions());
}

QFont ConfigDialog::appFont() const
{
    if (!tbAppFont->isEnabled())
        return Configuration::systemAppFont();
    QFont font = fcbAppFont->currentFont();
    bool ok;
    auto size = cbAppFontSize->currentText().toFloat(&ok);
    if (ok)
        font.setPointSizeF(size);
    return font;
}

void ConfigDialog::setAppFont(const QFont &font)
{
    fcbAppFont->setCurrentFont(font);
    cbAppFontSize->setCurrentText(QString::number(font.pointSizeF(), 'g', 2));
    cbAppFontSize->lineEdit()->clearFocus();
    tbAppFont->setEnabled(font != Configuration::systemAppFont());
}

void ConfigDialog::resetAppFont()
{
    setAppFont(Configuration::systemAppFont());
}

void ConfigDialog::appFontActivated()
{
    tbAppFont->setEnabled(true);
}

qreal ConfigDialog::editorFontSize() const
{
    bool ok;
    qreal fontSize = cbEditorFontSize->currentText().toFloat(&ok);
    return ok ? fontSize : qQNaN();
}

void ConfigDialog::setEditorFontSize(qreal size)
{
    if (!qIsNaN(size))
        cbEditorFontSize->setCurrentText(QString::number(size));
    else
        cbEditorFontSize->clearEditText();
    tbEditorFont->setEnabled(!qIsNaN(editorFontSize()));
    cbEditorFontSize->clearFocus();
}

void ConfigDialog::resetEditorFontSize()
{
    setEditorFontSize(qQNaN());
}

void ConfigDialog::editorFontActivated()
{
    tbEditorFont->setEnabled(true);
}

void ConfigDialog::addSubstitutionRow()
{
    substitutionsView->selectionModel();
    int row = substitutionsView->currentIndex().row();
    if (row < 0)
        row = 0;
    m_substs->insertRow(row);
}

void ConfigDialog::removeSubstitutionRow()
{
    int row = substitutionsView->currentIndex().row();
    if (row < 0)
        return;
    m_substs->removeRow(row);
}

QT_END_NAMESPACE

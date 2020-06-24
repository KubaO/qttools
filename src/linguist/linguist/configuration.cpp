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

#include "configuration.h"
#include "globals.h"

#include <QFont>
#include <QApplication>
#include <QScopedPointer>
#include <QSettings>

QT_BEGIN_NAMESPACE

static const char kOptions_EditorFontSize[] = "Options/EditorFontSize";

Configuration::Configuration(QObject *const parent) : QObject(parent)
{
    resetEditorFontSize();
    connect(qApp, &QGuiApplication::fontChanged, this, &Configuration::onAppFontChanged);
}

void Configuration::readConfig()
{
    QSettings config;
    bool ok;

    qreal editorFontSize = config.value(settingPath(kOptions_EditorFontSize)).toReal(&ok);
    if (!ok)
        editorFontSize = qQNaN();
    setEditorFontSize(editorFontSize);
}

void Configuration::writeConfig() const
{
    QSettings config;

    if (!qIsNaN(m_editorFontSize))
        config.setValue(settingPath(kOptions_EditorFontSize), m_editorFontSize);
    else
        config.remove(settingPath(kOptions_EditorFontSize));
}

void Configuration::onAppFontChanged(const QFont &font)
{
    Q_UNUSED(font);
    setEditorFontSize(m_editorFontSize); // notify of resolved size change if needed
}

qreal Configuration::editorFontSize() const
{
    return m_editorFontSize;
}

qreal Configuration::resolvedEditorFontSize() const
{
    return m_resolvedEditorFontSize;
}

void Configuration::setEditorFontSize(qreal const fontSize)
{
    // Change in numeric value - the resolved size won't change
    if (!qIsNaN(fontSize) && fontSize == m_editorFontSize)
        return;

    // Change in value when both values aren't NAN
    if (!qIsNaN(fontSize) || !qIsNaN(m_editorFontSize)) {
        m_editorFontSize = fontSize;
        editorFontSizeChanged(m_editorFontSize);
    }

    auto const newResolvedFontSize = !qIsNaN(fontSize) ? fontSize : qApp->font().pointSizeF();
    if (m_resolvedEditorFontSize != newResolvedFontSize) {
        m_resolvedEditorFontSize = newResolvedFontSize;
        resolvedEditorFontSizeChanged(m_resolvedEditorFontSize);
    }
}

void Configuration::increaseEditorFontSize()
{
    auto fontSize = resolvedEditorFontSize();
    if (fontSize < 32)
        setEditorFontSize(fontSize * 1.2f);
}

void Configuration::decreaseEditorFontSize()
{
    auto fontSize = resolvedEditorFontSize();
    if (fontSize > 8)
        setEditorFontSize(fontSize / 1.2f);
}

void Configuration::resetEditorFontSize()
{
    setEditorFontSize(qQNaN());
}

QT_END_NAMESPACE

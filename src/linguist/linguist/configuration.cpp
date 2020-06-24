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

static const char kSourcePathRewrites[] = "SourcePathRewrites";
static const QLatin1String kRegExp("RegExp");
static const QLatin1String kReplacement("Replacement");
static const char kOptions_ApplicationFont[] = "Options/ApplicationFont";
static const char kOptions_EditorFontSize[] = "Options/EditorFontSize";

static QScopedPointer<QFont> staticSystemAppFont;

Configuration::Configuration(QObject *const parent) : QObject(parent)
{
    staticSystemAppFont.reset(new QFont(qApp->font()));
    resetEditorFontSize();
    connect(qApp, &QGuiApplication::fontChanged, this, &Configuration::onAppFontChanged);
}

Configuration::~Configuration()
{
    staticSystemAppFont.reset();
}

QFont Configuration::systemAppFont()
{
    return *staticSystemAppFont;
}

void Configuration::readConfig()
{
    QSettings config;
    bool ok;

    QString appFontString = config.value(settingPath(kOptions_ApplicationFont)).toString();
    QFont appFont = systemAppFont();
    if (!appFontString.isEmpty()) {
        QFont font;
        if (font.fromString(appFontString))
            appFont = font;
    }
    setAppFont(appFont);

    qreal editorFontSize = config.value(settingPath(kOptions_EditorFontSize)).toReal(&ok);
    if (!ok)
        editorFontSize = qQNaN();
    setEditorFontSize(editorFontSize);

    m_sourcePathRewrites.resize(0);
    int size = config.beginReadArray(settingPath(kSourcePathRewrites));
    for (int i = 0; i < size; ++i) {
        config.setArrayIndex(i);
        Substitution subst;
        subst.regExp.setPattern(config.value(kRegExp).toString());
        subst.replacement = config.value(kReplacement).toString();
        m_sourcePathRewrites.push_back(std::move(subst));
    }
    config.endArray();
}

void Configuration::writeConfig() const
{
    QSettings config;

    if (qApp->font() != systemAppFont())
        config.setValue(settingPath(kOptions_ApplicationFont), appFont().toString());
    else
        config.remove(settingPath(kOptions_ApplicationFont));

    if (!qIsNaN(m_editorFontSize))
        config.setValue(settingPath(kOptions_EditorFontSize), m_editorFontSize);
    else
        config.remove(settingPath(kOptions_EditorFontSize));

    int oldArraySize = config.beginReadArray(settingPath(kSourcePathRewrites));
    config.endArray();
    if (oldArraySize > m_sourcePathRewrites.size())
        // Remove the entire array if the new one has less entries. Otherwise
        // the old entries will remain.
        config.remove(settingPath(kSourcePathRewrites));
    config.beginWriteArray(settingPath(kSourcePathRewrites), m_sourcePathRewrites.size());
    int i = 0;
    for (auto &rewrite : m_sourcePathRewrites) {
        config.setArrayIndex(i++);
        config.setValue(kRegExp, rewrite.regExp.pattern());
        config.setValue(kReplacement, rewrite.replacement);
    }
    config.endArray();
}

QFont Configuration::appFont() const
{
    return qApp->font();
}

void Configuration::setAppFont(const QFont &font)
{
    if (font == appFont())
        return;

    qApp->setFont(font);
}

void Configuration::resetAppFont()
{
    setAppFont(systemAppFont());
}

void Configuration::onAppFontChanged(const QFont &font)
{
    Q_UNUSED(font);
    setEditorFontSize(m_editorFontSize); // notify of resolved size change if needed
    appFontChanged(font);
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

    auto const newResolvedFontSize = !qIsNaN(fontSize) ? fontSize : appFont().pointSizeF();
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

QVector<Substitution> Configuration::sourcePathRewrites() const
{
    return m_sourcePathRewrites;
}

void Configuration::setSourcePathRewrites(const QVector<Substitution> &rewrites)
{
    if (m_sourcePathRewrites == rewrites)
        return;
    m_sourcePathRewrites = rewrites;
    emit sourcePathRewritesChanged(m_sourcePathRewrites);
}

QT_END_NAMESPACE

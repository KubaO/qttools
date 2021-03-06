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

#ifndef LINGUIST_CONFIGURATION_H
#define LINGUIST_CONFIGURATION_H

#include "substitutionmodel.h"

#include <QFont>
#include <QObject>

QT_BEGIN_NAMESPACE

class Configuration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QFont appFont READ appFont WRITE setAppFont RESET resetAppFont NOTIFY appFontChanged)
    Q_PROPERTY(qreal editorFontSize READ editorFontSize WRITE setEditorFontSize RESET
                       resetEditorFontSize NOTIFY editorFontSizeChanged)
    Q_PROPERTY(qreal resolvedEditorFontSize READ resolvedEditorFontSize NOTIFY
                       resolvedEditorFontSizeChanged)
    Q_PROPERTY(QVector<Substitution> sourcePathRewrites READ sourcePathRewrites WRITE
                       setSourcePathRewrites NOTIFY sourcePathRewritesChanged)
public:
    explicit Configuration(QObject *parent = nullptr);
    ~Configuration();

    QFont appFont() const;
    static QFont systemAppFont();

    qreal editorFontSize() const;
    qreal resolvedEditorFontSize() const;

    QVector<Substitution> sourcePathRewrites() const;

    void readConfig();
    void writeConfig() const;

public slots:
    void setAppFont(const QFont &font);
    void resetAppFont();

    void setEditorFontSize(qreal fontSize);
    void increaseEditorFontSize();
    void decreaseEditorFontSize();
    void resetEditorFontSize();

    void setSourcePathRewrites(const QVector<Substitution> &);

signals:
    void appFontChanged(const QFont &font);
    void editorFontSizeChanged(qreal fontSize);
    void resolvedEditorFontSizeChanged(qreal fontSize);
    void sourcePathRewritesChanged(const QVector<Substitution> &);

private:
    qreal m_editorFontSize = 0.0;
    qreal m_resolvedEditorFontSize = 0.0;
    QVector<Substitution> m_sourcePathRewrites;

    void onAppFontChanged(const QFont &font);
};

QT_END_NAMESPACE

#endif

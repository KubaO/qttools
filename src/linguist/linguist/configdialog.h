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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_configdialog.h"

QT_BEGIN_NAMESPACE

class Configuration;

class ConfigDialog : public QDialog, public Ui::ConfigDialog
{
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent = 0);

    void setFrom(const Configuration *config);
    void applyTo(Configuration *config) const;

private slots:
    void appFontActivated();
    void resetAppFont();

    void editorFontActivated();
    void resetEditorFontSize();

private:
    QFont appFont() const;
    void setAppFont(const QFont &font);

    qreal editorFontSize() const;
    void setEditorFontSize(qreal fontSize);
};

QT_END_NAMESPACE

#endif

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

#include "substitutionmodel.h"

#include <QColor>

bool Substitution::operator==(const Substitution &o) const
{
    return regExp == o.regExp && replacement == o.replacement;
}

bool Substitution::applyTo(QString &text) const
{
    auto match = regExp.match(text);
    if (!match.hasMatch())
        return {};

    text.replace(regExp, replacement);
    return true;
}

SubstitutionModel::SubstitutionModel(QObject *parent) : QAbstractTableModel(parent) {}

int SubstitutionModel::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int SubstitutionModel::columnCount(const QModelIndex &) const
{
    return 2;
}

Qt::ItemFlags SubstitutionModel::flags(const QModelIndex &index) const
{
    auto flags =
            Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    if (!index.isValid())
        flags |= Qt::ItemIsDropEnabled;
    return flags;
}

QVariant SubstitutionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 0)
                return tr("Regular Expression", "header");
            if (section == 1)
                return tr("Replacement", "header");
        }
    }
    return {};
}

Qt::DropActions SubstitutionModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QVariant SubstitutionModel::data(const QModelIndex &index, int role) const
{
    auto &row = m_data.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0)
            return row.regExp.pattern();
        if (index.column() == 1)
            return row.replacement;
    } else if (role == Qt::ForegroundRole) {
        if (index.column() == 0)
            return QVariant::fromValue(QColor(row.regExp.isValid() ? Qt::darkGreen : Qt::red));
    }
    return {};
}

bool SubstitutionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &row = m_data[index.row()];
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0) {
            row.regExp.setPattern(value.toString());
            return true;
        }
        if (index.column() == 1) {
            row.replacement = value.toString();
            return true;
        }
    }
    return false;
}

bool SubstitutionModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int,
                                     const QModelIndex &parent)
{
    if (row == -1)
        row = rowCount();
    return QAbstractTableModel::dropMimeData(data, action, row, 0, parent);
}

bool SubstitutionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row > m_data.size())
        return false;
    if (count > 0) {
        beginInsertRows(parent, row, row + count - 1);
        m_data.insert(row, count, {});
        endInsertRows();
    }
    return true;
}

bool SubstitutionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if ((row + count) > m_data.size())
        return false;
    if (count > 0) {
        beginRemoveRows(parent, row, row + count - 1);
        m_data.remove(row, count);
        endRemoveRows();
    }
    return true;
}

bool SubstitutionModel::moveRows(const QModelIndex &, int sourceRow, int count, const QModelIndex &,
                                 int destinationChild)
{
    beginMoveRows({}, sourceRow, sourceRow + count - 1, {}, destinationChild);
    for (int i = 0; i < count; i++) {
        m_data.insert(destinationChild + i, std::move(m_data[sourceRow + i]));
    }
    m_data.remove(destinationChild, count);
    endMoveRows();
    return true;
}

void SubstitutionModel::setSubstitutions(const QVector<Substitution> &substs)
{
    if (substs == m_data)
        return;
    beginResetModel();
    m_data = substs;
    endResetModel();
}

QVector<Substitution> SubstitutionModel::substitutions() const
{
    return m_data;
}

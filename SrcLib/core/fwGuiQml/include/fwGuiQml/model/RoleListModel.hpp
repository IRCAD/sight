/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <QAbstractListModel>
#include <Qt>

namespace fwGuiQml
{
namespace model
{

class RoleListModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit RoleListModel(QObject* parent = nullptr);
    void addData(const QHash<QByteArray, QVariant>&);
    void addRole(const int& enumNb, const QByteArray& role);
    bool isEmpty() noexcept;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
Q_SIGNALS:
public Q_SLOTS:
    void theDataChanged();
private:
    QHash<int, QByteArray> m_roles;
    QList<QHash<QByteArray, QVariant> > m_data;
};
} // namespace model
} // namespace fwGuiQml

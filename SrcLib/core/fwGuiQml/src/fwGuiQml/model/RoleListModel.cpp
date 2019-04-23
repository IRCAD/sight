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

#include "fwGuiQml/model/RoleListModel.hpp"

#include <QDebug>

namespace fwGuiQml
{
namespace model
{
RoleListModel::RoleListModel(QObject* parent) :
    QAbstractListModel(parent)
{

}

//------------------------------------------------------------------------------

int RoleListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

//------------------------------------------------------------------------------

QVariant RoleListModel::data(const QModelIndex& index, int role) const
{
    // check if the role exist
    if (!m_roles.contains(role))
    {
        return QVariant();
    }
    // Check boudaries
    if(index.column() < 0 ||
       index.row() < 0 ||
       rowCount() <= index.row())
    {
        qDebug() << "Warning: " << index.row() << ", " << index.column();
        return QVariant();
    }

    // Nominal case
    qDebug() << "MyModel::data: " << index.column() << "; " << index.row();
    return m_data[index.row()][m_roles.value(role)];
}

//------------------------------------------------------------------------------

QHash<int, QByteArray> RoleListModel::roleNames() const
{
    return m_roles;

}

//------------------------------------------------------------------------------

void RoleListModel::addData(const QHash<QByteArray, QVariant>& data)
{
    m_data.push_front(data);
}

//------------------------------------------------------------------------------

void RoleListModel::addRole(const int& enumNb, const QByteArray& role)
{
    m_roles.insert(enumNb, role);
}

//------------------------------------------------------------------------------

bool RoleListModel::isEmpty() noexcept
{
    return (m_data.isEmpty());
}

//------------------------------------------------------------------------------

void RoleListModel::theDataChanged()
{
    //TODO
}

} // namespace model
} // namespace fwGuiQml

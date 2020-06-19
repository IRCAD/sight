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

#include <fwCore/spyLog.hpp>

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
        return QVariant();
    }

    // Nominal case
    return m_data[index.row()].value(m_roles.value(role));
}

//------------------------------------------------------------------------------

QHash<int, QByteArray> RoleListModel::roleNames() const
{
    return m_roles;

}

//------------------------------------------------------------------------------

void RoleListModel::addData(const QHash<QByteArray, QVariant>& data)
{
    SLM_ASSERT("RoleListModel must have role to add Data", !m_roles.empty());
    m_data.push_back(data);
    // the function that emits that data has changed it's structure
    QAbstractListModel::endResetModel();
}

//------------------------------------------------------------------------------

void RoleListModel::addRole(const int& enumNb, const QByteArray& role)
{
    // each time the user add a role we clear data to be sure everything is alright
    m_roles.insert(enumNb, role);
    m_data.clear();
}

//------------------------------------------------------------------------------

bool RoleListModel::isEmpty() noexcept
{
    return (m_data.isEmpty());
}

//------------------------------------------------------------------------------

} // namespace model
} // namespace fwGuiQml

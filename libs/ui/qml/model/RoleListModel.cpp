/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/model/RoleListModel.hpp"

#include <core/spy_log.hpp>

#include <QDebug>

namespace sight::ui::qml::model
{

RoleListModel::RoleListModel(QObject* _parent) :
    QAbstractListModel(_parent)
{
}

//------------------------------------------------------------------------------

int RoleListModel::rowCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent)
    return m_data.size();
}

//------------------------------------------------------------------------------

QVariant RoleListModel::data(const QModelIndex& _index, int _role) const
{
    // check if the role exist
    if(!m_roles.contains(_role))
    {
        return {};
    }

    // Check boundaries
    if(_index.column() < 0
       || _index.row() < 0
       || rowCount() <= _index.row())
    {
        return {};
    }

    // Nominal case
    return m_data[_index.row()].value(m_roles.value(_role));
}

//------------------------------------------------------------------------------

QHash<int, QByteArray> RoleListModel::roleNames() const
{
    return m_roles;
}

//------------------------------------------------------------------------------

void RoleListModel::addData(const QHash<QByteArray, QVariant>& _data)
{
    SIGHT_ASSERT("RoleListModel must have role to add Data", !m_roles.empty());
    m_data.push_back(_data);
    // the function that emits that data has changed it's structure
    QAbstractListModel::endResetModel();
}

//------------------------------------------------------------------------------

void RoleListModel::addRole(const int& _enum_nb, const QByteArray& _role)
{
    // each time the user add a role we clear data to be sure everything is alright
    m_roles.insert(_enum_nb, _role);
    m_data.clear();
}

//------------------------------------------------------------------------------

bool RoleListModel::isEmpty() noexcept
{
    return m_data.isEmpty();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::model

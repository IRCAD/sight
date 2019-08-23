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

namespace fwGuiQml
{
namespace model
{
/**
 * @brief   RoleListModel allowing the creation of a ListModel with customizable role
 */
class RoleListModel : public QAbstractListModel
{
Q_OBJECT
public:
    /// Constructor
    explicit RoleListModel(QObject* parent = nullptr);

    /**
     * @brief Add data to the ListModel and call the reset function to emit changes to Qml.
     * @param[in] data One data row with role name as key inside the QHash
     */
    void addData(const QHash<QByteArray, QVariant>& data);

    /**
     * @brief Add role to the ListModel that can only be called before any data added
     * @param[in] enumNb the key of the QHash that carry the roles that need to be at least Qt::UserRole +1
     * @param[in] role the value of the string role in qml
     */
    void addRole(const int& enumNb, const QByteArray& role);

    /**
     * @brief Return if data isEmpty
     */
    bool isEmpty() noexcept;

    /// override
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /// override
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /// override
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    /// roles of the qml file to fill
    QHash<int, QByteArray> m_roles;
    /// data to send to the qml file
    QList<QHash<QByteArray, QVariant> > m_data;
};
} // namespace model
} // namespace fwGuiQml

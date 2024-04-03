/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include <data/model_series.hpp>

#include <QAbstractListModel>
#include <QObject>

namespace sight::module::ui::qml::model
{

/**
 * @brief   Model to manage the ModelSeries organs in Qml TableView.
 */
class organ_list_model : public QAbstractListModel
{
Q_OBJECT

public:

    enum organ_roles
    {
        organ_name_role = Qt::UserRole + 1,
        visibility_role,
        structure_type_role
    };

    /// Constructor. Do nothing.
    organ_list_model() noexcept;

    /// Destructor. Do nothing.
    ~organ_list_model() noexcept override;

    /// Return the number of rows under the given parent.
    [[nodiscard]] int rowCount(const QModelIndex& _parent = QModelIndex()) const override;

    /// Return the data stored under the given role for the item referred to by the index.
    [[nodiscard]] QVariant data(
        const QModelIndex& _index,
        int _role = Qt::DisplayRole
    ) const override;

    void update_model_series(const data::model_series::sptr& _model_series);

protected:

    /// Expose the role names, so that they can be accessed via QML
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:

    data::model_series::sptr m_model_series;
};

} // namespace sight::module::ui::qml::model

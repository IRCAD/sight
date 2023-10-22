/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/ui/qml/config.hpp"

#include <data/model_series.hpp>

#include <QAbstractListModel>
#include <QObject>

namespace sight::module::ui::qml::model
{

/**
 * @brief   Model to manage the ModelSeries organs in Qml TableView.
 */
class MODULE_UI_QML_CLASS_API OrganListModel : public QAbstractListModel
{
Q_OBJECT

public:

    enum OrganRoles
    {
        OrganNameRole = Qt::UserRole + 1,
        VisibilityRole,
        StructureTypeRole
    };

    /// Constructor. Do nothing.
    MODULE_UI_QML_API OrganListModel() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API ~OrganListModel() noexcept override;

    /// Return the number of rows under the given parent.
    [[nodiscard]] MODULE_UI_QML_API int rowCount(const QModelIndex& _parent = QModelIndex()) const override;

    /// Return the data stored under the given role for the item referred to by the index.
    [[nodiscard]] MODULE_UI_QML_API QVariant data(
        const QModelIndex& _index,
        int _role = Qt::DisplayRole
    ) const override;

    MODULE_UI_QML_API void updateModelSeries(const data::model_series::sptr& _model_series);

protected:

    /// Expose the role names, so that they can be accessed via QML
    [[nodiscard]] MODULE_UI_QML_API QHash<int, QByteArray> roleNames() const override;

private:

    data::model_series::sptr m_modelSeries;
};

} // namespace sight::module::ui::qml::model

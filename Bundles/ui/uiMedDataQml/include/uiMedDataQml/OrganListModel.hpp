/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "uiMedDataQml/config.hpp"

#include <fwMedData/ModelSeries.hpp>

#include <QAbstractListModel>
#include <QObject>

namespace uiMedDataQml
{

/**
 * @brief   Model to manage the ModelSeries organs in Qml TableView.
 */
class UIMEDDATAQML_CLASS_API OrganListModel : public QAbstractListModel
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
    UIMEDDATAQML_API OrganListModel() noexcept;

    /// Destructor. Do nothing.
    UIMEDDATAQML_API virtual ~OrganListModel() noexcept;

    /// Return the number of rows under the given parent.
    UIMEDDATAQML_API int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /// Return the data stored under the given role for the item referred to by the index.
    UIMEDDATAQML_API QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    UIMEDDATAQML_API void updateModelSeries(const ::fwMedData::ModelSeries::sptr& modelSeries);

protected:
    /// Expose the role names, so that they can be accessed via QML
    UIMEDDATAQML_API virtual QHash<int, QByteArray> roleNames() const;
private:
    ::fwMedData::ModelSeries::sptr m_modelSeries;
};

} // uiMedDataQml

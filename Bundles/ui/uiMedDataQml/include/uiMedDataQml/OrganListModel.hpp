/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

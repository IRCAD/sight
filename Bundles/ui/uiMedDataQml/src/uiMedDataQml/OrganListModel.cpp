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

#include "uiMedDataQml/OrganListModel.hpp"

#include <fwData/Reconstruction.hpp>

namespace uiMedDataQml
{

OrganListModel::OrganListModel() noexcept
{
}

//------------------------------------------------------------------------------

OrganListModel::~OrganListModel() noexcept
{

}

//------------------------------------------------------------------------------

QHash<int, QByteArray> OrganListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[OrganNameRole]     = "organName";
    roles[VisibilityRole]    = "organVisibility";
    roles[StructureTypeRole] = "structureType";
    return roles;
}

//------------------------------------------------------------------------------

void OrganListModel::updateModelSeries(const ::fwMedData::ModelSeries::sptr& modelSeries)
{
    beginResetModel();
    m_modelSeries = modelSeries;
    endResetModel();
}

//------------------------------------------------------------------------------

int OrganListModel::rowCount(const QModelIndex&) const
{
    int count = 0;
    if (m_modelSeries)
    {
        count = static_cast<int>(m_modelSeries->getReconstructionDB().size());
    }
    return count;
}

//------------------------------------------------------------------------------

QVariant OrganListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0)
    {
        return QVariant();
    }

    if (!m_modelSeries)
    {
        return QVariant();
    }

    const auto reconsctructions = m_modelSeries->getReconstructionDB();
    const size_t nbRec          = reconsctructions.size();
    const size_t idx            = static_cast< size_t >(index.row());

    if (idx >= nbRec)
    {
        return QVariant();
    }

    const auto& rec = reconsctructions[idx];
    switch (role)
    {
        case VisibilityRole:
            return rec->getIsVisible();
            break;
        case OrganNameRole:
            return QString::fromStdString(rec->getOrganName());
            break;
        case StructureTypeRole:
            return QString::fromStdString(rec->getStructureType());
            break;
        default:
            break;
    }
    return QVariant();
}

//------------------------------------------------------------------------------

} // namespace uiMedDataQml

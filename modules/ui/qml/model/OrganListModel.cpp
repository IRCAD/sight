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

#include "OrganListModel.hpp"

#include <data/reconstruction.hpp>

namespace sight::module::ui::qml::model
{

OrganListModel::OrganListModel() noexcept =
    default;

//------------------------------------------------------------------------------

OrganListModel::~OrganListModel() noexcept =
    default;

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

void OrganListModel::updateModelSeries(const data::model_series::sptr& _model_series)
{
    beginResetModel();
    m_modelSeries = _model_series;
    endResetModel();
}

//------------------------------------------------------------------------------

int OrganListModel::rowCount(const QModelIndex& /*parent*/) const
{
    int count = 0;
    if(m_modelSeries)
    {
        count = static_cast<int>(m_modelSeries->getReconstructionDB().size());
    }

    return count;
}

//------------------------------------------------------------------------------

QVariant OrganListModel::data(const QModelIndex& _index, int _role) const
{
    if(_index.row() < 0)
    {
        return {};
    }

    if(!m_modelSeries)
    {
        return {};
    }

    const auto reconstructions = m_modelSeries->getReconstructionDB();
    const std::size_t nb_rec   = reconstructions.size();
    const auto idx             = static_cast<size_t>(_index.row());

    if(idx >= nb_rec)
    {
        return {};
    }

    const auto& rec = reconstructions[idx];
    switch(_role)
    {
        case VisibilityRole:
            return rec->getIsVisible();

            break;

        case OrganNameRole:
            return QString::fromStdString(rec->getOrganName());

            break;

        case StructureTypeRole:
            return QString::fromStdString(rec->get_structure_type());

            break;

        default:
            break;
    }

    return {};
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::model

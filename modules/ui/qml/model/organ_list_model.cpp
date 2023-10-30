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

#include "organ_list_model.hpp"

#include <data/reconstruction.hpp>

namespace sight::module::ui::qml::model
{

organ_list_model::organ_list_model() noexcept =
    default;

//------------------------------------------------------------------------------

organ_list_model::~organ_list_model() noexcept =
    default;

//------------------------------------------------------------------------------

QHash<int, QByteArray> organ_list_model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[organ_name_role]     = "organName";
    roles[visibility_role]     = "organVisibility";
    roles[structure_type_role] = "structureType";
    return roles;
}

//------------------------------------------------------------------------------

void organ_list_model::update_model_series(const data::model_series::sptr& _model_series)
{
    beginResetModel();
    m_model_series = _model_series;
    endResetModel();
}

//------------------------------------------------------------------------------

int organ_list_model::rowCount(const QModelIndex& /*parent*/) const
{
    int count = 0;
    if(m_model_series)
    {
        count = static_cast<int>(m_model_series->get_reconstruction_db().size());
    }

    return count;
}

//------------------------------------------------------------------------------

QVariant organ_list_model::data(const QModelIndex& _index, int _role) const
{
    if(_index.row() < 0)
    {
        return {};
    }

    if(!m_model_series)
    {
        return {};
    }

    const auto reconstructions = m_model_series->get_reconstruction_db();
    const std::size_t nb_rec   = reconstructions.size();
    const auto idx             = static_cast<size_t>(_index.row());

    if(idx >= nb_rec)
    {
        return {};
    }

    const auto& rec = reconstructions[idx];
    switch(_role)
    {
        case visibility_role:
            return rec->get_is_visible();

            break;

        case organ_name_role:
            return QString::fromStdString(rec->get_organ_name());

            break;

        case structure_type_role:
            return QString::fromStdString(rec->get_structure_type());

            break;

        default:
            break;
    }

    return {};
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::model

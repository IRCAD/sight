/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/image/matrix_regressor.hpp"

#include <core/com/signal.hxx>

#include <filter/image/matrix_regressor.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

matrix_regressor::matrix_regressor()
= default;

//-----------------------------------------------------------------------------

matrix_regressor::~matrix_regressor()
= default;

//-----------------------------------------------------------------------------

void matrix_regressor::configuring()
{
}

//-----------------------------------------------------------------------------

void matrix_regressor::starting()
{
}

//-----------------------------------------------------------------------------

void matrix_regressor::updating()
{
    const auto matrix_list = m_matrixList.lock();
    const auto point_list  = m_pointList.lock();

    SIGHT_ASSERT(s_MATRIX_LIST_IN << " does not exist", matrix_list);
    SIGHT_ASSERT(s_POINT_LIST_IN << " does not exist", point_list);

    const auto optimal_matrix = m_optimalMatrix.lock();

    SIGHT_ASSERT("'optimalMatrix' does not exist", optimal_matrix);

    std::vector<sight::filter::image::matrix_regressor::point_t> pt_list;

    // Convert the point list.
    for(const auto& pt : point_list->getPoints())
    {
        const auto& pt_coords = pt->getCoord();
        pt_list.emplace_back(pt_coords[0], pt_coords[1], pt_coords[2], 1.);
    }

    if(!matrix_list->empty() && !pt_list.empty())
    {
        sight::filter::image::matrix_regressor regressor(matrix_list.get_shared(), pt_list);

        data::matrix4::csptr init_val =
            std::dynamic_pointer_cast<data::matrix4>((*matrix_list)[0]);

        data::matrix4::sptr res = regressor.minimize(*init_val, 1., 1e-4, 1e-4);
        optimal_matrix->deep_copy(res);

        m_sigComputed->async_emit();
    }
}

//-----------------------------------------------------------------------------

void matrix_regressor::stopping()
{
}

//-----------------------------------------------------------------------------

service::connections_t matrix_regressor::auto_connections() const
{
    return {
        {s_MATRIX_LIST_IN, data::vector::ADDED_OBJECTS_SIG, service::slots::UPDATE},
        {s_MATRIX_LIST_IN, data::vector::REMOVED_OBJECTS_SIG, service::slots::UPDATE},
        {s_MATRIX_LIST_IN, data::vector::MODIFIED_SIG, service::slots::UPDATE},
        {s_POINT_LIST_IN, data::point_list::POINT_ADDED_SIG, service::slots::UPDATE},
        {s_POINT_LIST_IN, data::point_list::POINT_REMOVED_SIG, service::slots::UPDATE},
        {s_POINT_LIST_IN, data::point_list::MODIFIED_SIG, service::slots::UPDATE}
    };
}

} // namespace sight::module::filter::image.

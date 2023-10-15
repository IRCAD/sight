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
    const auto matrixList = m_matrixList.lock();
    const auto pointList  = m_pointList.lock();

    SIGHT_ASSERT(s_MATRIX_LIST_IN << " does not exist", matrixList);
    SIGHT_ASSERT(s_POINT_LIST_IN << " does not exist", pointList);

    const auto optimalMatrix = m_optimalMatrix.lock();

    SIGHT_ASSERT("'optimalMatrix' does not exist", optimalMatrix);

    std::vector<sight::filter::image::matrix_regressor::PointType> ptList;

    // Convert the point list.
    for(const auto& pt : pointList->getPoints())
    {
        const auto& ptCoords = pt->getCoord();
        ptList.emplace_back(ptCoords[0], ptCoords[1], ptCoords[2], 1.);
    }

    if(!matrixList->empty() && !ptList.empty())
    {
        sight::filter::image::matrix_regressor regressor(matrixList.get_shared(), ptList);

        data::matrix4::csptr initVal =
            std::dynamic_pointer_cast<data::matrix4>((*matrixList)[0]);

        data::matrix4::sptr res = regressor.minimize(*initVal, 1., 1e-4, 1e-4);
        optimalMatrix->deep_copy(res);

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

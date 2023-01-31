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

#include "modules/filter/image/SMatrixRegressor.hpp"

#include <core/com/Signal.hxx>

#include <filter/image/MatrixRegressor.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

SMatrixRegressor::SMatrixRegressor()
= default;

//-----------------------------------------------------------------------------

SMatrixRegressor::~SMatrixRegressor()
= default;

//-----------------------------------------------------------------------------

void SMatrixRegressor::configuring()
{
}

//-----------------------------------------------------------------------------

void SMatrixRegressor::starting()
{
}

//-----------------------------------------------------------------------------

void SMatrixRegressor::updating()
{
    const auto matrixList = m_matrixList.lock();
    const auto pointList  = m_pointList.lock();

    SIGHT_ASSERT(s_MATRIX_LIST_IN << " does not exist", matrixList);
    SIGHT_ASSERT(s_POINT_LIST_IN << " does not exist", pointList);

    const auto optimalMatrix = m_optimalMatrix.lock();

    SIGHT_ASSERT("'optimalMatrix' does not exist", optimalMatrix);

    std::vector<sight::filter::image::MatrixRegressor::PointType> ptList;

    // Convert the point list.
    for(const auto& pt : pointList->getPoints())
    {
        const auto& ptCoords = pt->getCoord();
        ptList.emplace_back(ptCoords[0], ptCoords[1], ptCoords[2], 1.);
    }

    if(!matrixList->empty() && !ptList.empty())
    {
        sight::filter::image::MatrixRegressor regressor(matrixList.get_shared(), ptList);

        data::Matrix4::csptr initVal =
            data::Matrix4::dynamicCast((*matrixList)[0]);

        data::Matrix4::sptr res = regressor.minimize(*initVal, 1., 1e-4, 1e-4);
        optimalMatrix->deepCopy(res);

        m_sigComputed->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SMatrixRegressor::stopping()
{
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMatrixRegressor::getAutoConnections() const
{
    return {
        {s_MATRIX_LIST_IN, data::Vector::s_ADDED_OBJECTS_SIG, IService::slots::s_UPDATE},
        {s_MATRIX_LIST_IN, data::Vector::s_REMOVED_OBJECTS_SIG, IService::slots::s_UPDATE},
        {s_MATRIX_LIST_IN, data::Vector::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_POINT_LIST_IN, data::PointList::s_POINT_ADDED_SIG, IService::slots::s_UPDATE},
        {s_POINT_LIST_IN, data::PointList::s_POINT_REMOVED_SIG, IService::slots::s_UPDATE},
        {s_POINT_LIST_IN, data::PointList::s_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

} // namespace sight::module::filter::image.

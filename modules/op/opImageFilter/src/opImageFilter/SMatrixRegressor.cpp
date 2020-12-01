/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "opImageFilter/SMatrixRegressor.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwServices/macros.hpp>

#include <imageFilterOp/MatrixRegressor.hpp>

namespace opImageFilter
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opImageFilter::SMatrixRegressor, ::fwData::Vector)

static const ::fwServices::IService::KeyType s_MATRIX_LIST_IN = "matrixList";
static const ::fwServices::IService::KeyType s_POINT_LIST_IN        = "pointList";
static const ::fwServices::IService::KeyType s_OPTIMAL_MATRIX_INOUT = "optimalMatrix";

//-----------------------------------------------------------------------------

SMatrixRegressor::SMatrixRegressor()
{

}

//-----------------------------------------------------------------------------

SMatrixRegressor::~SMatrixRegressor()
{

}

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
    const auto matrixList = this->getLockedInput< ::fwData::Vector >(s_MATRIX_LIST_IN);
    const auto pointList  = this->getLockedInput< ::fwData::PointList >(s_POINT_LIST_IN);

    SLM_ASSERT("'matrixList' does not exist", matrixList);
    SLM_ASSERT("'pointList' does not exist", pointList);

    const auto optimalMatrix = this->getLockedInOut< ::fwData::TransformationMatrix3D >(s_OPTIMAL_MATRIX_INOUT);

    SLM_ASSERT("'optimalMatrix' does not exist", optimalMatrix);

    std::vector< ::imageFilterOp::MatrixRegressor::PointType > ptList;

    // Convert the point list.
    for(const auto& pt : pointList->getPoints())
    {
        const auto& ptCoords = pt->getCoord();
        ptList.push_back( ::imageFilterOp::MatrixRegressor::PointType(ptCoords[0], ptCoords[1], ptCoords[2], 1.));
    }

    if(!matrixList->empty() && !ptList.empty())
    {
        ::imageFilterOp::MatrixRegressor regressor(matrixList.get_shared(), ptList);

        ::fwData::TransformationMatrix3D::csptr initVal =
            ::fwData::TransformationMatrix3D::dynamicCast((*matrixList)[0]);

        ::fwData::TransformationMatrix3D::sptr res = regressor.minimize(initVal, 1., 1e-4, 1e-4);
        optimalMatrix->deepCopy(res);

        m_sigComputed->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SMatrixRegressor::stopping()
{

}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMatrixRegressor::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_LIST_IN, ::fwData::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_LIST_IN, ::fwData::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_LIST_IN, ::fwData::Vector::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace opImageFilter.

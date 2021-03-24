/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#pragma once

#include "modules/filter/image/config.hpp"

#include <core/base.hpp>

#include <filter/image/MatrixRegressor.hpp>

#include <service/IOperator.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service computing the matrix that best fits a list of matrices.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent when the 'average' matrix is computed.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::SMatrixRegressor">
       <in key="matrixList" uid="..." autoConnect="true" />
       <in key="pointList" uid="..." />
       <inout key="optimalMatrix" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrixList [sight::data::Vector]: The list of matrices used to compute an optimal one.
 * - \b pointList [sight::data::PointList]: A list of relevant points used to evaluate the distance to the optimal
 * matrix.
 *
 * @subsection In-Out In-Out
 * - \b optimalMatrix [sight::data::Matrix4]: The optimal matrix.
 */
class MODULE_FILTER_IMAGE_CLASS_API SMatrixRegressor : public service::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SMatrixRegressor, service::IOperator)

    /// Does nothing.
    MODULE_FILTER_IMAGE_API SMatrixRegressor();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_API ~SMatrixRegressor() override;

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Compute the optimal matrix.
    MODULE_FILTER_IMAGE_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Vector::s_ADDED_OBJECTS_SIG of s_MATRIX_LIST_IN to s_UPDATE_SLOT
     * Connect data::Vector::s_REMOVED_OBJECTS_SIG of s_MATRIX_LIST_IN to s_UPDATE_SLOT
     * Connect data::Vector::s_MODIFIED_SIG of s_MATRIX_LIST_IN to s_UPDATE_SLOT
     * Connect data::PointList::s_POINT_ADDED_SIG of s_POINT_LIST_IN to s_UPDATE_SLOT
     * Connect data::PointList::s_POINT_REMOVED_SIG of s_POINT_LIST_IN to s_UPDATE_SLOT
     * Connect data::PointList::s_MODIFIED_SIG of s_POINT_LIST_IN to s_UPDATE_SLOT
     */
    MODULE_FILTER_IMAGE_API KeyConnectionsMap getAutoConnections() const override;

};

} // namespace sight::module::filter::image.

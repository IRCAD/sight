/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/geometry/vision/config.hpp"

#include <data/matrix_tl.hpp>
#include <data/point_list.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief This service update a pointlist with the center of the marker (from a matrixTL) when the extractMarker slot is
 * called
 * This service can be used to save the displacement of a marker in time. (each point of the pointlist will be a
 * position).
 *
 * @section Slots Slots
 * - \b addPoint() : Add marker position in the pointList. Position is extracted from matrixTL.
 * - \b clear() : Clear the pointList.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="markerToPoint" type="sight::module::geometry::vision::marker_to_point">
         <in key="matrixTL" uid="matrixTL" />
         <inout key="pointList" uid="markersPL" />
     </service>
   @endcode
 * @subsection Input Input
 * - \b matrixTL [sight::data::matrix_tl]: timeline for tool matrices.
 * @subsection In-Out In-Out
 * - \b pointList [sight::data::Pointlist]: marker position.
 */
class MODULE_GEOMETRY_VISION_CLASS_API marker_to_point : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(marker_to_point, service::controller);

    /**
     * @name Slots API
     * @{
     */
    MODULE_GEOMETRY_VISION_API static const core::com::slots::key_t ADD_POINT_SLOT;

    MODULE_GEOMETRY_VISION_API static const core::com::slots::key_t CLEAR_SLOT;
    ///@}

    /// Constructor
    MODULE_GEOMETRY_VISION_API marker_to_point() noexcept;

    /// Destructor
    MODULE_GEOMETRY_VISION_API ~marker_to_point() noexcept override;

protected:

    /// Does nothing
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void updating() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void stopping() override;

    /// Slot called to extract position from the latest matrix of the MatrixTL and push it in the pointList
    MODULE_GEOMETRY_VISION_API void add_point();

    /// Slot called to clear the pointlist
    MODULE_GEOMETRY_VISION_API void clear();

    data::ptr<data::matrix_tl, data::access::in> m_matrix_tl {this, "matrixTL"};
    data::ptr<data::point_list, data::access::inout> m_point_list {this, "pointList"};
};

} //namespace sight::module::geometry::vision

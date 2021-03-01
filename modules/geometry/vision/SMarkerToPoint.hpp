/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <service/IController.hpp>

#include <string>
#include <vector>

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
     <service uid="markerToPoint" type="::sight::module::geometry::vision::SMarkerToPoint">
         <in key="matrixTL" uid="matrixTL" />
         <inout key="pointList" uid="markersPL" />
     </service>
   @endcode
 * @subsection Input Input
 * - \b matrixTL [sight::data::MatrixTL]: timeline for tool matrices.
 * @subsection In-Out In-Out
 * - \b pointList [sight::data::Pointlist]: marker position.
 */
class MODULE_GEOMETRY_VISION_CLASS_API SMarkerToPoint : public service::IController
{
public:
    SIGHT_DECLARE_SERVICE(SMarkerToPoint, service::IController)

    /// Data xml keys
    static const service::IService::KeyType s_MATRIXTL_INPUT;
    static const service::IService::KeyType s_POINTLIST_INOUT;

    /**
     * @name Slots API
     * @{
     */
    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_ADD_POINT_SLOT;

    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_CLEAR_SLOT;
    ///@}

    /// Constructor
    MODULE_GEOMETRY_VISION_API SMarkerToPoint() noexcept;

    /// Destructor
    MODULE_GEOMETRY_VISION_API ~SMarkerToPoint() noexcept;

protected:

    /// Does nothing
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void swapping() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void updating() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void stopping() override;

    /// Slot called to extract position from the latest matrix of the MatrixTL and push it in the pointList
    MODULE_GEOMETRY_VISION_API void addPoint();

    /// Slot called to clear the pointlist
    MODULE_GEOMETRY_VISION_API void clear();
};

} //namespace sight::module::geometry::vision

/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "ctrlPicking/config.hpp"

#include <fwData/Point.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IController.hpp>

namespace ctrlPicking
{

/**
 * @brief Add point in a ::fwData::PointList
 *
 * @section Slots Slots
 * - \b pick(::fwDataTools::PickingInfo): Add or remove the closest point to the picking position, actions occur when
 * `CTRL` is pressed.
 * - \b clearPoints(): Remove all points from the points lists.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::ctrlPicking::SManagePointList">
            <inout key="pointList" uid="..." />
            <in key="matrix" uid="..." />
            <config max="0" removable="true" label="false" />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b pointList [::fwData::PointList]: Target point list.
 *
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D](optional): Apply a transformation matrix on a picked point.
 *
 * @subsection Configuration Configuration:
 * - \b max (optional, default=0): Set the maximum number of points contained in the point list, if it's 0, the capacity
 * is set to the maximum. If the maximum is reached, the oldest point added to the list will be removed.
 * - \b removable (optional, default=true): Allow points to be removed.
 * - \b label (optional, default=false): Add an ID label to added points.
 */
class CTRLPICKING_CLASS_API SManagePointList : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(SManagePointList, ::fwServices::IController)

    /// Initialize slots.
    CTRLPICKING_API SManagePointList() noexcept;

    /// Destroys the service.
    CTRLPICKING_API virtual ~SManagePointList() noexcept final;

private:

    /// Configures the service.
    virtual void configuring() final;

    /// Does nothing.
    virtual void starting() final;

    /// Does nothing.
    virtual void updating() final;

    /// Does nothing.
    virtual void stopping() final;

    /**
     * @brief SLOT: Add or remove a point from picking informations.
     * @param _info picking informations.
     */
    void pick(::fwDataTools::PickingInfo _info) const;

    /**
     * @brief Adds a point in the point list
     * @param _point the point added to the list.
     */
    void addPoint(const ::fwData::Point::sptr _point) const;

    /**
     * @brief Removes a point from the point list.
     * @param _point the point removed from the list.
     */
    void removePoint(const ::fwData::Point::csptr _point) const;

    /// SLOT: Clears the point list.
    void clearPoints() const;

    /// Maximum number of contained points.
    size_t m_max {0};

    /// Allows points to be removed.
    bool m_removable {true};

    /// Allows to add an ID label the points.
    bool m_label {false};

};
} // ctrlPicking

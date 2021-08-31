/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <data/PointList.hpp>
#include <data/tools/PickingInfo.hpp>

#include <service/IController.hpp>

#include <modules/ui/viz/config.hpp>

namespace sight::module::ui::viz
{

/**
 * @brief   Add points in a data::PointList
 *
 * @section Slots Slots
 * - \b pick(data::tools::PickingInfo) : Add or remove the closest point to the picking position, removal occurs when
 * `CTRL` is pressed.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::viz::SAddPoint">
            <inout key="pointList" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b pointList [sight::data::PointList]: target points list.
 */
class MODULE_UI_VIZ_CLASS_API SAddPoint : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SAddPoint, service::IController);

    /**
     * @name Slots API
     * @{
     */

    /// SLOT: Use data from the picking info to add or remove a point.
    MODULE_UI_VIZ_API static const core::com::Slots::SlotKeyType s_PICK_SLOT;

    /// SLOT: Clear the point list.
    MODULE_UI_VIZ_API static const core::com::Slots::SlotKeyType s_CLEAR_POINTS_SLOT;
    /** @} */

    /// Constructor. Creates slot.
    MODULE_UI_VIZ_API SAddPoint() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_VIZ_API virtual ~SAddPoint() noexcept;

protected:

    MODULE_UI_VIZ_API void configuring() override;

    /// Does nothing
    MODULE_UI_VIZ_API void starting() override;

    /// Does nothing
    MODULE_UI_VIZ_API void stopping() override;

    /// Does nothing
    MODULE_UI_VIZ_API void updating() override;

private:

    ///  Add a point in the point list
    void addPoint(const data::Point::sptr);

    ///  Remove a point from the point list
    void removePoint(const data::Point::csptr);

    /**
     * @name Slots methods
     * @{
     */

    /// SLOT: Add or remove a point.
    void pick(data::tools::PickingInfo);

    /// SLOT: Clear the point list.
    void clearPoints();

    data::ptr<data::PointList, data::Access::inout> m_pointList {this, "pointList"};
};

} // uiVisuOgre

/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <uiVisuOgre/config.hpp>

#include <fwServices/IController.hpp>

fwCorePredeclare( (fwData)(Object) )

namespace uiVisuOgre
{

/**
 * @brief   Add points in a ::fwData::PointList
 *
 * @section Slots Slots
 * - \b addPoint(::fwData::Point) : add a new point in the list.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiVisuOgre::SAddPoint">
            <inout key="pointList" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b pointList [::fwData::PointList]: target points list.
 */
class UIVISUOGRE_CLASS_API SAddPoint : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SAddPoint)(::fwServices::IController) )

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT;
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT;
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_CLEAR_POINTS_SLOT;
    /** @} */

    /// Constructor. Creates slot.
    UIVISUOGRE_API SAddPoint() noexcept;

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SAddPoint() noexcept;

protected:
    UIVISUOGRE_API virtual void configuring() override;

    /// Does nothing
    UIVISUOGRE_API virtual void starting() override;

    /// Does nothing
    UIVISUOGRE_API virtual void stopping() override;

    /// Does nothing
    UIVISUOGRE_API virtual void updating() override;

private:

    /**
     * @name Slots methods
     * @{
     */

    /// SLOT : Sends a signal with the clicked poin coordinates.
    void addPoint(SPTR(::fwData::Object) _pointObject);

    /// SLOT : Sends a signal with the clicked poin coordinates.
    void removePoint(SPTR(::fwData::Object) _pointObject);

    /// SLOT : Clear the point list.
    void clearPoints();

    /**
     * @}
     */
};
} // uiVisuOgre

/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
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

#include "uiMeasurement/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IController.hpp>

namespace uiMeasurement
{
/**
 * @brief   SManageLandmark adds a point into a pointlist on ctrl-left click.
 *
 * @section Slots Slots
 * - \b createLandmark(::fwDataTools::PickingInfo): add a point into a pointlist on ctrl-left click using PickingInfo.
 * - \b clearPointlist(): Clear all points from pointlist
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiMeasurement::SManageLandmark" >
       <inout key="landmarks" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b landmarks [::fwData::PointList]: the pointlist containing the added points.
 */
class UIMEASUREMENT_CLASS_API SManageLandmark : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SManageLandmark, ::fwServices::IController);

    /**
     * @brief Constructor.
     */
    UIMEASUREMENT_API SManageLandmark() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SManageLandmark() noexcept
    {
    }

    /**
     * @name Slot API
     * @{
     */
    UIMEASUREMENT_API static const ::fwCom::Slots::SlotKeyType s_CREATE_LANDMARK_SLOT;
    UIMEASUREMENT_API static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT;
    ///@}

protected:

    /**
     * @brief This method is used to configure the service.
     */
    UIMEASUREMENT_API void configuring() override;

    /// This method is used to initialize the service.
    UIMEASUREMENT_API void starting() override;

    /// Does nothing.
    UIMEASUREMENT_API void stopping() override;

    /// Does nothing.
    UIMEASUREMENT_API void updating() override;

private:

    /// Slot: Each time a point is created, adds an ID label to the point.
    void createLandmark(::fwDataTools::PickingInfo info);

    /// Slot: Clear the pointlist
    void clearPointlist();

    size_t m_counter; ///< Counter used as point ID.

};

} //namespace uiMeasurement

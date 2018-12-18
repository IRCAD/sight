/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "videoTools/config.hpp"

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/fwID.hpp>

namespace videoTools
{
/**
 * @brief This service listens to a picking interaction and fills the clicked point list.
 *
 * @section Slots Slots
 * - \b getInteraction(::fwDataTools::PickingInfo info): retrieves the 2d coordinates (z= 0) from picking info.
 *
 * @code{.xml}
   <service uid="..." type="::videoTools::SImagePicker" >
        <inout key="pointList" uid="..." />
   </service>
   @endcode
 * @section InOut InOut
 * - \b pointList [::fwData::PointList] : Pointlist of clicked points
 **/

class VIDEOTOOLS_CLASS_API SImagePicker : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SImagePicker)(::fwServices::IController) );

    /**
     * @name Slots API
     * @{
     */
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT;
    ///@}

    /**
     * @name Data API
     * @{
     */
    VIDEOTOOLS_API static const ::fwServices::IService::KeyType s_POINTLIST_INOUT;
    /** @} */

    /// Does nothing
    VIDEOTOOLS_API SImagePicker() noexcept;

    /// Does nothing
    VIDEOTOOLS_API virtual ~SImagePicker() noexcept;

protected:
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Does nothing.
    VIDEOTOOLS_API virtual void starting() override;

    /// Does nothing.
    VIDEOTOOLS_API virtual void stopping() override;

    /// Does nothing.
    VIDEOTOOLS_API virtual void updating() override;

    /// Does nothing.
    VIDEOTOOLS_API virtual void configuring() override;

private:

    /// Adds a point in the pointlist.
    void addPoint(const std::array<double, 3>& currentPoint);

    /// Removes the last point in the pointlist.
    void removeLastPoint();

    /// Slot: retrieves the 2d coordinates from the interaction point.
    void getInteraction(::fwDataTools::PickingInfo info);

};

} //namespace videoTools

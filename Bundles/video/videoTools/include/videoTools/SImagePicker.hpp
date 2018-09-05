/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

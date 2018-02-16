/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
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
 * @code{.xml}
   <service uid="..." type="::uiVideo::SImagePicker" >
        <inout key="pointList" uid="..." />
   </service>
   @endcode
 * - \b pointList [::fwData::PointList] : Pointlist of clicked points
 **/

class VIDEOTOOLS_CLASS_API SImagePicker : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SImagePicker)(::fwServices::IController) );

    /// Contructor. Do nothing.
    VIDEOTOOLS_API SImagePicker() noexcept;

    /// Destructor. Do nothing.
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
    void addPoint(const std::array<double, 3>& curentPoint);

    /// Removes the last point in the pointlist.
    void removeLastPoint();

    /// Slot: retrieves the image from the interaction point.
    void getInteraction(::fwDataTools::PickingInfo info);

    /// PointList of the clicked points in the video
    ::fwData::PointList::sptr m_pointList;

};

} //namespace videoTools

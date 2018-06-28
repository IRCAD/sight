/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoFilter/config.hpp"
#include "videoFilter/IVideoFilter.hpp"

#include <arData/FrameTL.hpp>

#include <opencv2/core/mat.hpp>

namespace videoFilter
{

/**
 * @brief   Service that receives frames, split them in four regions, and push these new smaller frames in four
 * timelines.
 *
 * @section Slots Slots
 * - \b cropFrame(::fwCore::HiResClock::HiResClockType) : splits the current frame.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="cropVideoQuad" type="::videoFilter::SCropVideoQuad">
        <in key="quadFrameTL" uid="..." autoConnect="yes"/>
        <inout key="frameTL1" uid="..." />
        <inout key="frameTL2" uid="..." />
        <inout key="frameTL3" uid="..." />
        <inout key="frameTL4" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b quadFrameTL [::arData::FrameTL]: frame to split in 4.
 * @subsection In-Out In-Out
 * - \b frameTL1 [::arData::FrameTL]: first timeline containing a part of the quadFrameTL.
 * - \b frameTL2 [::arData::FrameTL]: second timeline containing a part of the quadFrameTL.
 * - \b frameTL3 [::arData::FrameTL]: third timeline containing a part of the quadFrameTL.
 * - \b frameTL4 [::arData::FrameTL]: fourth timeline containing a part of the quadFrameTL.
 */
class VIDEOFILTER_CLASS_API SCropVideoQuad : public ::videoFilter::IVideoFilter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SCropVideoQuad)(::videoFilter::IVideoFilter) );

    /// Constructor. Do nothing.
    VIDEOFILTER_API SCropVideoQuad() noexcept;

    /// Destructor. Do nothing.
    VIDEOFILTER_API virtual ~SCropVideoQuad() noexcept;

    VIDEOFILTER_API static const ::fwCom::Slots::SlotKeyType s_CROP_FRAME_SLOT;

protected:

    /// Initialize the layout and the camera.
    VIDEOFILTER_API virtual void starting() override;

    /// Destroy the layout.
    VIDEOFILTER_API virtual void stopping() override;

    /// Do nothing.
    VIDEOFILTER_API virtual void updating() override;

    VIDEOFILTER_API virtual void configuring() override;

private:

    /// Slot to crop the current frame
    void cropFrame(::fwCore::HiResClock::HiResClockType timestamp);

    void pushFrameInTimeline(::cv::Mat& imgIn,
                             const ::cv::Rect& roi,
                             ::arData::FrameTL::sptr& frameTL,
                             ::fwCore::HiResClock::HiResClockType timestamp);

    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;
};
} //namespace videoFilter

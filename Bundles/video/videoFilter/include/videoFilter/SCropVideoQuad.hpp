#ifndef __VIDEOFILTER_SCROPVIDEOQUAD_HPP__
#define __VIDEOFILTER_SCROPVIDEOQUAD_HPP__

#include "videoFilter/config.hpp"
#include <videoFilter/IVideoFilter.hpp>

#include <arData/FrameTL.hpp>

#include <opencv2/core/mat.hpp>

namespace videoFilter
{

/**
 * @brief   Defines the service which grabs video frames.
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

#endif /*__VIDEOFILTER_SCROPVIDEOQUAD_HPP__*/

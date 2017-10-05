/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SGRABBERPROXY_HPP__
#define __VIDEOTOOLS_SGRABBERPROXY_HPP__

#include "videoTools/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwServices/IHasServices.hpp>

namespace arData
{
class Camera;
}

namespace videoTools
{

/**
 * @brief   Defines the service which grab video frame.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t) : Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b startCamera() : Start playing the camera or the video.
 * - \b stopCamera() : Stop playing the camera or the video.
 * - \b pauseCamera() : Pause the video, it has no effect when playing a camera.
 * - \b loopVideo() : Toggle the loop of the playing.
 * - \b setPositionVideo(int) : Force the current time in the video.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoQt::SGrabberProxy">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 */
class VIDEOTOOLS_CLASS_API SGrabberProxy :  public ::arServices::IGrabber,
                                            public ::fwServices::IHasServices
{
public:

    fwCoreServiceClassDefinitionsMacro( (SGrabberProxy)(::arServices::IGrabber) );

    /// Constructor. Do nothing.
    VIDEOTOOLS_API SGrabberProxy() noexcept;

    /// Destructor. Do nothing.
    VIDEOTOOLS_API virtual ~SGrabberProxy() noexcept;

    /**
     * @name Slots API
     * @{
     */
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_RECONFIGURE_SLOT;
    ///@}

protected:

    /// Initialize the layout and the camera.
    VIDEOTOOLS_API virtual void starting() final;

    /// Destroy the layout.
    VIDEOTOOLS_API virtual void stopping() final;

    /// Do nothing.
    VIDEOTOOLS_API virtual void updating() final;

    /// Do nothing.
    VIDEOTOOLS_API virtual void configuring() final;

    /// SLOT : Initialize and start camera (restart camera if is already started)
    VIDEOTOOLS_API virtual void startCamera() final;

    /// SLOT : Stop camera
    VIDEOTOOLS_API virtual void stopCamera() final;

    /// SLOT : Pause camera
    VIDEOTOOLS_API virtual void pauseCamera() final;

    /// SLOT : enable/disable loop in video
    VIDEOTOOLS_API virtual void toggleLoopMode() final;

    /// SLOT : set the new position in the video.
    VIDEOTOOLS_API virtual void setPosition(int64_t position) final;

private:

    void reconfigure();

    /// state of the loop mode
    bool m_loopVideo { false };

    /// grabber implementation chosen by the user
    std::string m_grabberImpl;

    /// actual grabber service
    ::arServices::IGrabber::sptr m_service;
};

} // namespace videoTools

#endif /*__VIDEOTOOLS_SGRABBERPROXY_HPP__*/

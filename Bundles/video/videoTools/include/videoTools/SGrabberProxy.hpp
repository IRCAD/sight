/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoTools/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IHasServices.hpp>

namespace arData
{
class Camera;
}

namespace videoTools
{

/**
 * @brief   This service allows you to select a frame grabber implementation at runtime.
 *
 * Actually as its name indicates it, this service is a proxy and behaves like exactly a video frame grabber. The
 * difference is that it selects a valid video grabber implementation when playing is requested. If there are several
 * implementation available, a dialog proposes the user to make a choice.
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
 * - \b reconfigure() : Allows to erase the implementation choice, so that the selection routine is ran again, thus,
 * potentially the selection dialog is shown.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::videoQt::SGrabberProxy">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <config>
                <camera type="RGBD" />
                <selection mode="include" />
                <addSelection service="::videoQt::SFrameGrabber" />
                <config id="cvGrabberConfig" service="::videoOpenCV::SFrameGrabber" />
            </config>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 *  - \b type (optional, default="RGB"): allows to filter for RGB or RGBD grabbers
 *  - \b selection
 *      - \b mode (optional) : must be include (to add the selection to selector list ) or exclude (to exclude the
 * selection of the selector list).
 *  - \b addSelection
 *      - \b service (mandatory) : name of the service to include/exclude to the choice list of the selector.
 *  - \b config
 *      - \b id (mandatory) : the id of the configuration to use.
 *      - \b service (mandatory) :  the name of the service.
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
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_RECONFIGURE_SLOT;
    ///@}

protected:

    /// Does nothing.
    VIDEOTOOLS_API virtual void starting() final;

    /// Stop the underlying grabber, destroy it, and empty the input FrameTl.
    VIDEOTOOLS_API virtual void stopping() final;

    /// Does nothing.
    VIDEOTOOLS_API virtual void updating() final;

    /// Parses the XML configuration of the service.
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
    VIDEOTOOLS_API virtual void setPosition(std::int64_t position) final;

private:

    typedef std::pair<std::string, ::fwRuntime::ConfigurationElement::csptr > ServiceConfigPair;

    /// SLOT : reset the grabber implementation and stop the current playback.
    void reconfigure();

    /// SLOT : position in the video has changed.
    void modifyPosition(int64_t position);

    /// SLOT : duration of the video has changed.
    void modifyDuration(int64_t position);

    enum class CameraType : std::uint8_t
    {
        RGB,
        RGBD,
    };

    /// Camera type (RGB, RGBD,...)
    CameraType m_type { CameraType::RGB };

    /// grabber implementation chosen by the user
    std::string m_grabberImpl;

    /// Config to use with the current grabber.
    std::string m_grabberConfig;

    /// actual grabber service
    std::vector< ::arServices::IGrabber::sptr > m_services;

    /// connections with service signals
    ::fwCom::helper::SigSlotConnection m_connections;

    /// List of services to be included or excluded.
    std::set< std::string > m_selectedServices;

    /// Map that specifies all configuration extensions for a service.
    std::map< std::string, std::vector< std::string > > m_serviceToConfig;

    /// state of the loop mode
    bool m_loopVideo { false };

    /// Configure if selected services are excluded (true) or included (false).
    bool m_exclude { false };
};

} // namespace videoTools

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
 * - \b nextImage(): display the next image in step by step mode. Does nothing if not supported by the selected grabber.
 * - \b previousImage(): display the previous image in step by step mode. Does nothing if not supported by the
 * selected grabber.
 * - \b setStep(): set the step value between two images when calling nextImage/previousImage. Does nothing if not
 * supported by the selected grabber.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::videoTools::SGrabberProxy">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <inout key="depthTL" uid="..." />
            <config>
                <camera type="RGBD" />
                <selection mode="include" />
                <addSelection service="::videoQt::SFrameGrabber" />
                <config id="cvGrabberConfig" service="::videoOpenCV::SFrameGrabber" />
                <gui title="Please select the camera to use" />
            </config>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 * - \b depthTL [::arData::FrameTL] (optional): timeline where to extract the depth frames.
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
 *  - \b gui
 *      - \b title (optional) : title of the grabber selector window.
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

    /// SLOT : Initialize and start camera (restart camera if is already started).
    VIDEOTOOLS_API virtual void startCamera() final;

    /// SLOT : Stop camera.
    VIDEOTOOLS_API virtual void stopCamera() final;

    /// SLOT : Pause camera.
    VIDEOTOOLS_API virtual void pauseCamera() final;

    /// SLOT : enable/disable loop in video.
    VIDEOTOOLS_API virtual void toggleLoopMode() final;

    /// SLOT : set the new position in the video.
    VIDEOTOOLS_API virtual void setPosition(std::int64_t position) final;

    /// SLOT : get the previous image in frame by frame mode.
    VIDEOTOOLS_API virtual void previousImage() override;

    /// SLOT : get the next image in frame by frame mode.
    VIDEOTOOLS_API virtual void nextImage() override;

    /// SLOT : set step used on readPrevious/readNext slots.
    VIDEOTOOLS_API virtual void setStep(int step, std::string key) override;

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

    /// Grabber implementation chosen by the user.
    std::string m_grabberImpl;

    /// Config to use with the current grabber.
    std::string m_grabberConfig;

    /// Title of the GUI selector window
    std::string m_guiTitle { "Please select a video grabber implementation" };

    /// Actual grabber service.
    std::vector< ::arServices::IGrabber::sptr > m_services;

    /// Connections with service signals.
    ::fwCom::helper::SigSlotConnection m_connections;

    /// List of services to be included or excluded.
    std::set< std::string > m_selectedServices;

    /// Map that specifies all configuration extensions for a service.
    std::map< std::string, std::vector< std::string > > m_serviceToConfig;

    /// State of the loop mode.
    bool m_loopVideo { false };

    /// Configure if selected services are excluded (true) or included (false).
    bool m_exclude { false };
};

} // namespace videoTools

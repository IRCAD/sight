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

#include "videoVLC/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwTools/Failed.hpp>

#include <vlc/vlc.h>

namespace arData
{
class Camera;
}

namespace videoVLC
{

class VlcCallbackProxy;

/**
 * @brief   VLC player
 *
 * Defines the service which grab video frame.
 *
 * \b Tags: FILE,STREAM,DEVICE
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
        <service type="::videoVLC::SFrameGrabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <networkCaching>500</networkCaching>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 * - \b networkCaching(optional, default: 500): Caching value for network resources, in milliseconds [0 .. 60000].
 */
class VIDEOVLC_CLASS_API SFrameGrabber : public ::arServices::IGrabber
{

public:

    fwCoreServiceClassDefinitionsMacro( (SFrameGrabber)(::arServices::IGrabber) )

    /// Constructor. Do nothing.
    VIDEOVLC_API SFrameGrabber() noexcept;

    /// Destructor. Do nothing.
    VIDEOVLC_API virtual ~SFrameGrabber() noexcept;

protected:

    /// Initialize the layout and the camera.
    VIDEOVLC_API virtual void starting() override;

    /// Destroys the layout.
    VIDEOVLC_API virtual void stopping() override;

    /// Does nothing.
    VIDEOVLC_API virtual void updating() override;

    /**
     * @code{.xml}
       <service uid="${GENERIC_UID}_VideoGrabber" impl="::videoVLC::SFrameGrabber" autoConnect="no">
        <cameraFwId>cameraID</cameraFwId>
       </service>
       @endcode
     **/
    VIDEOVLC_API virtual void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started)
    virtual void startCamera() override;
    /// SLOT : Stop camera
    virtual void stopCamera() override;
    /// SLOT : Pause camera
    virtual void pauseCamera() override;
    /// SLOT : set the new position in the video.
    virtual void setPosition(int64_t position) override;
    /// SLOT : enable/disable the loop mode in video.
    virtual void toggleLoopMode() override;

    /// SLOT : Select camera
    void selectCamera(const std::string& cameraID);
    /// Gets camera from m_cameraID
    CSPTR(::arData::Camera) getCamera();

private:

    /// Private event handler
    static void onEventCallback(const struct libvlc_event_t* event, void* userData);

    static void exportVLCPluginsEnv(const std::string& path);

    /// FwID of arData::Camera
    std::string m_cameraID;

    /// state of the loop mode
    bool m_loopVideo;

    libvlc_instance_t* m_vlcInstance;
    libvlc_media_player_t* m_vlcPlayer;
    libvlc_media_t* m_vlcMedia;
    VlcCallbackProxy* m_vlcProxy;

    /// Caching value for network resources, in milliseconds [0 .. 60000].
    std::string m_networkCaching;
};
}

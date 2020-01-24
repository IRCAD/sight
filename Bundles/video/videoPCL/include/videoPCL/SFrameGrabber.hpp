/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "videoPCL/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwCore/mt/types.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/Failed.hpp>

#include <filesystem>

namespace arData
{
class Camera;
}

namespace videoPCL
{

/**
 * @brief   Defines the service grabbing video frame.
 *
 * @note Only 'file' source is currently managed.
 * @note You can load 2D point cloud in a folder like img_<timestamp>.pcd (ex. img_642752427.pcd). The service uses
 * the timestamp to order the frames and to push them in the timeline.
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
        <service type="::videoPCL::SFrameGrabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <fps>30</fps>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 * - \b fps (optional) : target playback frame rate (default: 30).
 */
class VIDEOPCL_CLASS_API SFrameGrabber : public ::arServices::IGrabber
{

public:

    fwCoreServiceMacro(SFrameGrabber, ::arServices::IGrabber)

    /// Constructor. Do nothing.
    VIDEOPCL_API SFrameGrabber() noexcept;

    /// Destructor. Do nothing.
    VIDEOPCL_API virtual ~SFrameGrabber() noexcept;

protected:

    /// Initialize the layout and the camera.
    VIDEOPCL_API virtual void starting() override;

    /// Destroy the layout.
    VIDEOPCL_API virtual void stopping() override;

    /// Do nothing.
    VIDEOPCL_API virtual void updating() override;

    /// Do nothing.
    VIDEOPCL_API virtual void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started)
    virtual void startCamera() override;

    /// SLOT : Stop camera
    virtual void stopCamera() override;

    /// SLOT : Pause camera
    virtual void pauseCamera() override;

    /// SLOT : enable/disable loop in video
    virtual void toggleLoopMode() override;

    /// SLOT : set the new position in the video.
    virtual void setPosition(int64_t position) override;

private:

    typedef std::vector< std::filesystem::path > ImageFilesType;

    /// Initializes the image reader, start the timer
    void readImages(const std::filesystem::path& folder, const std::string& extension);

    /// Reads the next image
    void grabImage();

    /// state of the loop mode
    bool m_loopVideo;

    /// state of the timeline initialization
    bool m_isInitialized;

    /// fps used to read the video
    unsigned int m_fps;

    /// counter used by the image reader
    size_t m_imageCount;

    ::fwThread::Timer::sptr m_timer;

    /// Worker for the grabVideo or grabFrame timer
    ::fwThread::Worker::sptr m_worker;

    /// list of image paths to read
    ImageFilesType m_imageToRead;

    /// Mutex to protect concurrent access for m_imageToRead
    mutable ::fwCore::mt::Mutex m_mutex;
};

} // namespace videoOpenCV

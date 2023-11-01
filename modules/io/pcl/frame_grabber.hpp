/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/io/pcl/config.hpp"

#include <core/mt/types.hpp>
#include <core/thread/timer.hpp>
#include <core/tools/failed.hpp>

#include <io/__/service/grabber.hpp>

#include <filesystem>

namespace sight::data
{

class Camera;

}

namespace sight::module::io::pcl
{

/**
 * @brief   Defines the service grabbing video frame.
 *
 * @note Only 'file' source is currently managed.
 * @note You can load 2D point cloud in a folder like img_<timestamp>.pcd (ex. img_642752427.pcd). The service uses
 * the timestamp to order the frames and to push them in the timeline.
 *
 * @section Signals Signals
 * - \b position_modified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b duration_modified(std::int64_t) : Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b start_camera() : Start playing the camera or the video.
 * - \b stop_camera() : Stop playing the camera or the video.
 * - \b pause_camera() : Pause the video, it has no effect when playing a camera.
 * - \b loop_video() : Toggle the loop of the playing.
 * - \b set_position_video(int) : Force the current time in the video.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::pcl::frame_grabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <fps>30</fps>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [sight::data::frame_tl]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 * - \b fps (optional) : target playback frame rate (default: 30).
 */
class MODULE_IO_PCL_CLASS_API frame_grabber : public sight::io::service::grabber
{
public:

    SIGHT_DECLARE_SERVICE(frame_grabber, sight::io::service::grabber);

    /// Constructor. Do nothing.
    MODULE_IO_PCL_API frame_grabber() noexcept;

    /// Destructor. Do nothing.
    MODULE_IO_PCL_API ~frame_grabber() noexcept override = default;

protected:

    /// Initialize the layout and the camera.
    MODULE_IO_PCL_API void starting() override;

    /// Destroy the layout.
    MODULE_IO_PCL_API void stopping() override;

    /// Do nothing.
    MODULE_IO_PCL_API void updating() override;

    /// Do nothing.
    MODULE_IO_PCL_API void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started)
    void startCamera() override;

    /// SLOT : Stop camera
    void stopCamera() override;

    /// SLOT : Pause camera
    void pauseCamera() override;

    /// SLOT : enable/disable loop in video
    void toggleLoopMode() override;

    /// SLOT : set the new position in the video.
    void setPosition(int64_t position) override;

private:

    typedef std::vector<std::filesystem::path> image_files_t;

    /// Initializes the image reader, start the timer
    void readImages(const std::filesystem::path& folder, const std::string& extension);

    /// Reads the next image
    void grabImage();

    /// state of the loop mode
    bool m_loopVideo {false};

    /// state of the timeline initialization
    bool m_isInitialized {false};

    /// fps used to read the video
    unsigned int m_fps {30};

    /// counter used by the image reader
    std::size_t m_imageCount {0};

    core::thread::timer::sptr m_timer;

    /// Worker for the grabVideo or grabFrame timer
    core::thread::worker::sptr m_worker;

    /// list of image paths to read
    image_files_t m_imageToRead;

    /// Mutex to protect concurrent access for m_imageToRead
    mutable core::mt::mutex m_mutex;

    data::ptr<data::camera, data::access::in> m_camera {this, s_CAMERA_INPUT};
};

} // namespace sight::module::io::video

/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/mt/types.hpp>
#include <core/thread/timer.hpp>
#include <core/tools/failed.hpp>

#include <io/__/service/grabber.hpp>

#include <opencv2/videoio.hpp>

#include <filesystem>

namespace sight::data
{

class Camera;

} // namespace sight::data

namespace sight::module::io::video
{

/**
 * @brief   OpenCV player
 *
 * Defines the service grabbing video frame with OpenCV.
 *
 * @note Only file source is currently managed.
 * @note You can load images in a folder like img_<timestamp>.<ext> (ex. img_642752427.jpg). The service uses
 * the timestamp to order the frames and to push them in the timeline.
 *
 * \b Tags: FILE,DEVICE,STREAM
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
 * - \b next_image() : Read the next image (in frame-by-frame mode).
 * - \b previous_image() : Read the previous image (in frame-by-frame mode).
 * - \b set_step(int step, std::string key) : set the step value between two images when calling
 * nextImage/previous_image
 * slots on oneShot mode (supported key: "step")
 * - \b add_roi_center(sight::data::point::sptr): Adds a new region fo interest center.
 * - \b remove_roi_center(sight::data::point::sptr): Removes a region of interest via its center.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::video::frame_grabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <fps>30</fps>
            <oneShot>false</oneShot>
            <createTimestamp>false</createTimestamp>
            <useTimelapse>true</useTimelapse>
            <defaultDuration>5000</defaultDuration>
            <step>5</step>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [sight::data::frame_tl]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 * - \b fps (optional) : target playback frame rate when playing an image sequence (default: 30).
 * - \b useTimelapse (optional): if true, the difference between two image's timestamps will be use as timer duration,
 *      the 'fps' value will be ignore.
 * - \b oneShot (optional) : Use frame by frame mode, using nextImage and previousImage
 *  (only available if reading set of images) (default: false).
 * - \b createTimestamp (optional) : create a new timestamp instead of using name of image
 * (only available if reading set of images) (default: false).
 * - \b step (optional): value to jump between two images when calling readNext/readPrevious slots (default: 1)
 * - \b defaultDuration (optional): if duration (between each frames) cannot be deduced with filename
 * this value is used (default: 5000), this is a very advanced option.
 * It will have not effects if reading a video or if a timestamp can be deduced from images filenames
 * (ex. img_642752427.jpg).
 */
class frame_grabber : public sight::io::service::grabber
{
public:

    SIGHT_DECLARE_SERVICE(frame_grabber, sight::io::service::grabber);

    /// Constructor. Do nothing.
    frame_grabber() noexcept;

    /// Destructor. Do nothing.
    ~frame_grabber() noexcept override;

    void set_parameter(ui::parameter_t _value, std::string _key) override;

protected:

    /// Initialize the layout and the camera.
    void starting() override;

    /// Destroy the layout.
    void stopping() override;

    /// Do nothing.
    void updating() override;

    /// Do nothing.
    void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started).
    void start_camera() override;

    /// SLOT : Stop camera.
    void stop_camera() override;

    /// SLOT : Pause camera.
    void pause_camera() override;

    /// SLOT : enable/disable loop in video.
    void toggle_loop_mode() override;

    /// SLOT : set the new position in the video.
    void set_position(int64_t _position) override;

    /// SLOT : read the next image (only in file mode, and if m_oneShot is enabled).
    void next_image() override;

    /// SLOT : read the previous image (only in file mode, and if m_oneShot is enabled).
    void previous_image() override;

    /// SLOT: Set step used on readPrevious/readNext slots
    void set_step(int _step, std::string _key) override;

    /// SLOT: Adds a region of interest center. Currently this function implements image zoom.
    ///       The added point can be forwarded from a picker in singlePointMode for example.
    void add_roi_center(sight::data::point::sptr _p) final;

    /// SLOT: Removes a region of interest center. Currently this function relates to image zoom.
    ///       The removed point can be forwarded from a picker in singlePointMode for example.
    void remove_roi_center(sight::data::point::sptr _p) final;

private:

    using image_files_t      = std::vector<std::filesystem::path>;
    using image_timestamps_t = std::vector<double>;

    /// Initializes the video reader, start the timer.
    void read_video(const std::filesystem::path& _file);

    /// Initializes the usb device reader, start the timer.
    void read_device(const data::camera& _cam);

    /// Initializes the ffmpeg based stream grabber, start the timer.
    void read_stream(const data::camera& _cam);

    /// Initializes the image reader, start the timer.
    void read_images(const std::filesystem::path& _folder, const std::string& _extension);

    /// Reads the next video frame.
    void grab_video();

    /// Reads the next image.
    void grab_image();

    /// Updates the image if zoom is requested
    void update_zoom(cv::Mat _image);

    /// State of the loop mode.
    bool m_loop_video {false};

    /// State of the timeline initialization.
    bool m_is_initialized {false};

    /// Fps used to read the video.
    unsigned int m_fps {30};

    /// Counter used by the image reader.
    std::size_t m_image_count {0};

    core::thread::timer::sptr m_timer;

    /// Worker for the grabVideo or grabFrame timer.
    core::thread::worker::sptr m_worker;

    /// OpenCV video grabber.
    cv::VideoCapture m_video_capture;

    /// List of image paths to read.
    image_files_t m_image_to_read;

    /// List of the image timestamps.
    image_timestamps_t m_image_timestamps;

    /// Zoom factor
    int m_zoom_factor {2};

    /// Zoom center, if defined
    std::optional<std::array<int, 2> > m_zoom_center;

    /// Mutex to protect concurrent access for m_videoCapture and m_imageToRead.
    mutable core::mt::mutex m_mutex;

    /// Frame -by-frame mode (true if enabled, false otherwise).
    bool m_one_shot {false};

    /// If true: create a new timestamp when reading image, if false: use the name of the image file as timestamp.
    bool m_create_new_ts {false};

    /// If true: the difference between two image's timestamps will be use as timer duration.
    bool m_use_timelapse {true};

    /// If true: the grabber is paused.
    bool m_is_paused {false};

    /// If timestamp cannot be deduced from filename use the default duration (5000ms).
    double m_default_duration {5000};

    /// Step between two images when calling nexImage()/previous_image() slots
    std::uint64_t m_step {1};

    /// Step value updated in set_step() slot used to compute a shift value when calling nextImage()/previous_image()
    std::uint64_t m_step_changed {1};

    /// Total number of frames in a video file.
    std::size_t m_video_frames_nb {0};

    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA_INPUT};
};

} // namespace sight::module::io::video

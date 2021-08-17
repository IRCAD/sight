/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/io/video/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/mt/types.hpp>
#include <core/thread/Timer.hpp>
#include <core/tools/Failed.hpp>

#include <service/IGrabber.hpp>

#include <opencv2/videoio.hpp>

#include <filesystem>

namespace sight::data
{

class Camera;

}

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
 * - \b positionModified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t) : Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b startCamera() : Start playing the camera or the video.
 * - \b stopCamera() : Stop playing the camera or the video.
 * - \b pauseCamera() : Pause the video, it has no effect when playing a camera.
 * - \b loopVideo() : Toggle the loop of the playing.
 * - \b setPositionVideo(int) : Force the current time in the video.
 * - \b nextImage() : Read the next image (in frame-by-frame mode).
 * - \b previousImage() : Read the previous image (in frame-by-frame mode).
 * - \b setStep(int step, std::string key) : set the step value between two images when calling nextImage/previousImage
 * slots on oneShot mode (supported key: "step")
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::video::SFrameGrabber">
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
 * - \b camera [sight::data::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [sight::data::FrameTL]: timeline where to extract the video frames.
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
class MODULE_IO_VIDEO_CLASS_API SFrameGrabber : public service::IGrabber
{
public:

    SIGHT_DECLARE_SERVICE(SFrameGrabber, sight::service::IGrabber);

    /// Constructor. Do nothing.
    MODULE_IO_VIDEO_API SFrameGrabber() noexcept;

    /// Destructor. Do nothing.
    MODULE_IO_VIDEO_API virtual ~SFrameGrabber() noexcept;

protected:

    /// Initialize the layout and the camera.
    MODULE_IO_VIDEO_API void starting() override;

    /// Destroy the layout.
    MODULE_IO_VIDEO_API void stopping() override;

    /// Do nothing.
    MODULE_IO_VIDEO_API void updating() override;

    /// Do nothing.
    MODULE_IO_VIDEO_API void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started).
    void startCamera() override;

    /// SLOT : Stop camera.
    void stopCamera() override;

    /// SLOT : Pause camera.
    void pauseCamera() override;

    /// SLOT : enable/disable loop in video.
    void toggleLoopMode() override;

    /// SLOT : set the new position in the video.
    void setPosition(int64_t position) override;

    /// SLOT : read the next image (only in file mode, and if m_oneShot is enabled).
    void nextImage() override;

    /// SLOT : read the previous image (only in file mode, and if m_oneShot is enabled).
    void previousImage() override;

    /// SLOT: Set step used on readPrevious/readNext slots
    void setStep(int step, std::string key) override;

private:

    typedef std::vector<std::filesystem::path> ImageFilesType;
    typedef std::vector<double> ImageTimestampsType;

    /// Initializes the video reader, start the timer.
    void readVideo(const std::filesystem::path& file);

    /// Initializes the usb device reader, start the timer.
    void readDevice(const CSPTR(data::Camera)_cam);

    /// Initializes the ffmpeg based stream grabber, start the timer.
    void readStream(const CSPTR(data::Camera)_cam);

    /// Initializes the image reader, start the timer.
    void readImages(const std::filesystem::path& folder, const std::string& extension);

    /// Reads the next video frame.
    void grabVideo();

    /// Reads the next image.
    void grabImage();

    /// State of the loop mode.
    bool m_loopVideo;

    /// State of the timeline initialization.
    bool m_isInitialized;

    /// Fps used to read the video.
    unsigned int m_fps;

    /// Counter used by the image reader.
    size_t m_imageCount;

    core::thread::Timer::sptr m_timer;

    /// Worker for the grabVideo or grabFrame timer.
    core::thread::Worker::sptr m_worker;

    /// OpenCV video grabber.
    ::cv::VideoCapture m_videoCapture;

    /// List of image paths to read.
    ImageFilesType m_imageToRead;

    /// List of the image timestamps.
    ImageTimestampsType m_imageTimestamps;

    /// Mutex to protect concurrent access for m_videoCapture and m_imageToRead.
    mutable core::mt::Mutex m_mutex;

    /// Frame -by-frame mode (true if enabled, false otherwise).
    bool m_oneShot;

    /// If true: create a new timestamp when reading image, if false: use the name of the image file as timestamp.
    bool m_createNewTS;

    /// If true: the difference between two image's timestamps will be use as timer duration.
    bool m_useTimelapse;

    /// If true: the grabber is paused.
    bool m_isPaused;

    /// If timestamp cannot be deduced from filename use the default duration (5000ms).
    double m_defaultDuration;

    /// Step between two images when calling nexImage()/previousImage() slots
    unsigned long m_step;

    /// Step value updated in setStep() slot used to compute a shift value when calling nextImage()/previousImage()
    unsigned long m_stepChanged;

    /// Total number of frames in a video file.
    size_t m_videoFramesNb;

    data::ptr<data::Camera, data::Access::in> m_camera {this, s_CAMERA_INPUT};
};

} // namespace sight::module::io::video

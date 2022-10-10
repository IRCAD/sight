/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SFrameGrabber.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/Camera.hpp>
#include <data/FrameTL.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <regex>

namespace sight::module::io::video
{

static const core::com::Slots::SlotKeyType s_SET_STEP_SLOT = "setStep";

// -----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() noexcept
{
    newSlot(s_SET_STEP_SLOT, &SFrameGrabber::setStep, this);
}

// -----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() noexcept =
    default;

// -----------------------------------------------------------------------------

void SFrameGrabber::starting()
{
    m_worker = core::thread::Worker::New();
}

// -----------------------------------------------------------------------------

void SFrameGrabber::stopping()
{
    this->stopCamera();

    m_worker->stop();
    m_worker.reset();
}

// -----------------------------------------------------------------------------

void SFrameGrabber::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    m_fps = config.get<unsigned int>("fps", 30);

    m_oneShot = config.get<bool>("oneShot", false);

    m_createNewTS = config.get<bool>("createTimestamp", false);

    m_useTimelapse = config.get<bool>("useTimelapse", true);

    m_defaultDuration = config.get<double>("defaultDuration", m_defaultDuration);

    SIGHT_FATAL_IF("Fps setting is set to " << m_fps << " but should be in ]0;60].", m_fps == 0 || m_fps > 60);

    m_step = config.get<std::uint64_t>("step", m_step);
    SIGHT_ASSERT("Step value is set to " << m_step << " but should be > 0.", m_step > 0);
    m_stepChanged = m_step;
}

// -----------------------------------------------------------------------------

void SFrameGrabber::updating()
{
}

// -----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    if(this->started())
    {
        // Do not reset if we are already started
        return;
    }

    const auto camera = m_camera.lock();

    if(camera->getCameraSource() == data::Camera::FILE)
    {
        std::filesystem::path file = camera->getVideoFile();

        // For compatibility with old calibration with absolute path
        if(!file.is_absolute())
        {
            const std::filesystem::path videoDir(ui::base::Preferences().get("VIDEO_DIR_PREF", std::string()));
            file = videoDir / file;
        }

        file = file.lexically_normal();

        m_isPaused = false;

        const std::filesystem::path ext = file.extension();

        if(ext.string() == ".png" || ext.string() == ".jpg" || ext.string() == ".tiff" || ext.string() == ".bmp")
        {
            this->readImages(file.parent_path(), ext.string());
        }
        else
        {
            this->readVideo(file);
        }
    }
    else if(camera->getCameraSource() == data::Camera::DEVICE)
    {
        this->readDevice(*camera);
    }
    else if(camera->getCameraSource() == data::Camera::STREAM)
    {
        this->readStream(*camera);
    }
    else
    {
        this->setStartState(false);
        sight::ui::base::dialog::MessageDialog::show(
            "Grabber",
            "This video source is not managed by this grabber."
        );
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    m_isPaused = !m_isPaused;
    if(m_timer)
    {
        m_isPaused ? m_timer->stop() : m_timer->start();
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    core::mt::ScopedLock lock(m_mutex);

    if(m_timer)
    {
        if(m_timer->isRunning())
        {
            m_timer->stop();
        }

        m_timer.reset();
    }

    if(m_videoCapture.isOpened())
    {
        m_videoCapture.release();
    }

    m_imageToRead.clear();
    m_imageTimestamps.clear();
    m_imageCount = 0;

    if(m_isInitialized)
    {
        // Clear the timeline: send a black frame
        const auto sigPosition = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
        sigPosition->asyncEmit(static_cast<std::int64_t>(-1));

        const auto sigDuration = this->signal<DurationModifiedSignalType>(s_DURATION_MODIFIED_SIG);
        sigDuration->asyncEmit(static_cast<std::int64_t>(-1));

        const auto frameTL = m_frame.lock();
        sight::module::io::video::SFrameGrabber::clearTimeline(*frameTL);

        const auto sig = this->signal<IGrabber::CameraStoppedSignalType>(IGrabber::s_CAMERA_STOPPED_SIG);
        sig->asyncEmit();

        this->setStartState(false);
    }

    m_isInitialized = false;
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readVideo(const std::filesystem::path& file)
{
    core::mt::ScopedLock lock(m_mutex);

    m_videoCapture.open(file.string());

    if(m_videoCapture.isOpened())
    {
        m_timer = m_worker->createTimer();

        const auto fps = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_FPS));
        m_videoFramesNb = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_FRAME_COUNT));

        if(fps == 0)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Video error",
                "Cannot read FPS from video file. Please check the video format.",
                sight::ui::base::dialog::MessageDialog::CRITICAL
            );
            return;
        }

        const auto sigDuration = this->signal<DurationModifiedSignalType>(s_DURATION_MODIFIED_SIG);
        sigDuration->asyncEmit(static_cast<std::int64_t>((m_videoFramesNb / fps) * 1000));

        const auto sigPosition = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
        sigPosition->asyncEmit(0);

        core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(1000 / fps);

        m_timer->setFunction([this](auto&& ...){grabVideo();});
        m_timer->setDuration(duration);
        m_timer->start();

        this->setStartState(true);
        auto sig = this->signal<IGrabber::CameraStartedSignalType>(IGrabber::s_CAMERA_STARTED_SIG);
        sig->asyncEmit();
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Grabber",
            "This file cannot be opened: " + file.string() + "."
        );

        this->setStartState(false);
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readDevice(const data::Camera& _camera)
{
    core::mt::ScopedLock lock(m_mutex);

    const std::string device = _camera.getCameraID();
    const int index          = _camera.getIndex();

#ifdef __linux__
    // On linux the V4L backend can read from device id (/dev/video...)
    m_videoCapture.open(device);
    if(!m_videoCapture.isOpened())
    {
        //try with index
        if(index >= 0)
        {
            m_videoCapture.open(index);
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Grabber",
                "This device cannot be opened: " + device + " at index: " + std::to_string(index)
            );
        }
    }

    // Only way to capture 1080p with v4l.
    m_videoCapture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
#else
    //On other platforms (at least on MacOS, we should use the index given by Qt)
    if(index >= 0)
    {
        m_videoCapture.open(index);
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Grabber",
            "This device cannot be opened: " + device + " at index: " + std::to_string(index)
        );
    }
#endif

    if(m_videoCapture.isOpened())
    {
        m_timer = m_worker->createTimer();
        float fps = _camera.getMaximumFrameRate();
        fps = fps <= 0.F ? 30.F : fps;
        const std::size_t height = _camera.getHeight();
        const std::size_t width  = _camera.getWidth();

        m_videoCapture.set(cv::CAP_PROP_FPS, static_cast<int>(fps));
        m_videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(width));
        m_videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(height));

        core::thread::Timer::TimeDurationType duration =
            std::chrono::milliseconds(1000 / static_cast<std::size_t>(fps));

        m_timer->setFunction([this](auto&& ...){grabVideo();});
        m_timer->setDuration(duration);
        m_timer->start();

        this->setStartState(true);
        auto sig = this->signal<IGrabber::CameraStartedSignalType>(IGrabber::s_CAMERA_STARTED_SIG);
        sig->asyncEmit();
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Grabber",
            "This device:" + device + " at index: " + std::to_string(index) + "cannot be openned."
        );

        this->setStartState(false);
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readStream(const data::Camera& _camera)
{
    core::mt::ScopedLock lock(m_mutex);

    m_videoCapture.open(_camera.getStreamUrl());

    if(m_videoCapture.isOpened())
    {
        m_timer = m_worker->createTimer();
        float fps = _camera.getMaximumFrameRate();
        fps = fps <= 0.F ? 30.F : fps;
        const std::size_t height = _camera.getHeight();
        const std::size_t width  = _camera.getWidth();

        m_videoCapture.set(cv::CAP_PROP_FPS, static_cast<int>(fps));
        m_videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(width));
        m_videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(height));

        const core::thread::Timer::TimeDurationType duration =
            std::chrono::milliseconds(1000 / static_cast<std::size_t>(fps));

        m_timer->setFunction([this](auto&& ...){grabVideo();});
        m_timer->setDuration(duration);
        m_timer->start();

        this->setStartState(true);
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Grabber",
            "This stream:" + _camera.getStreamUrl() + " cannot be opened."
        );

        this->setStartState(false);
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readImages(const std::filesystem::path& folder, const std::string& extension)
{
    core::mt::ScopedLock lock(m_mutex);

    std::filesystem::directory_iterator currentEntry(folder);
    std::filesystem::directory_iterator endEntry;
    for( ; currentEntry != endEntry ; ++currentEntry)
    {
        std::filesystem::path entryPath = *currentEntry;

        if(entryPath.extension() == extension)
        {
            m_imageToRead.push_back(entryPath);
        }
    }

    // Sort in alphabetical order (ex: img_001, img_002...)
    std::sort(m_imageToRead.begin(), m_imageToRead.end());

    if(!m_imageToRead.empty())
    {
        // Find the timestamps of all the images
        double stubTimestamp = 0.;
        for(const std::filesystem::path& imagePath : m_imageToRead)
        {
            const std::string imageName = imagePath.filename().string();
            static const std::regex s_TIMESTAMP("[^0-9]*([0-9]{5,})[^0-9]*");
            std::smatch match;
            if(std::regex_match(imageName, match, s_TIMESTAMP))
            {
                const std::string timestampStr = match[1].str();
                m_imageTimestamps.push_back(std::stod(timestampStr));
            }
            else
            {
                SIGHT_WARN(
                    "Could not find a timestamp in file name: " + imageName
                    + ". Generating a timestamp duration of: " + std::to_string(m_defaultDuration)
                    + "ms."
                );

                m_imageTimestamps.push_back(stubTimestamp);

                stubTimestamp += m_defaultDuration;
            }
        }

        const std::string file = m_imageToRead.front().string();
        const cv::Mat image    = cv::imread(file, cv::IMREAD_UNCHANGED);

        const int width  = image.size().width;
        const int height = image.size().height;
        const int type   = image.type();

        if(width > 0 && height > 0)
        {
            const auto w = static_cast<std::size_t>(width);
            const auto h = static_cast<std::size_t>(height);

            auto frameTL = m_frame.lock();

            switch(type)
            {
                case CV_8UC1:
                    frameTL->initPoolSize(
                        w,
                        h,
                        core::Type::UINT8,
                        sight::data::FrameTL::PixelFormat::GRAY_SCALE
                    );
                    break;

                case CV_8UC3:
                    frameTL->initPoolSize(
                        w,
                        h,
                        core::Type::UINT8,
                        sight::data::FrameTL::PixelFormat::RGB
                    );
                    break;

                case CV_8UC4:
                    frameTL->initPoolSize(
                        w,
                        h,
                        core::Type::UINT8,
                        sight::data::FrameTL::PixelFormat::RGBA
                    );
                    break;

                case CV_16UC1:
                    frameTL->initPoolSize(
                        w,
                        h,
                        core::Type::UINT16,
                        sight::data::FrameTL::PixelFormat::GRAY_SCALE
                    );
                    break;

                default:
                    sight::ui::base::dialog::MessageDialog::show(
                        "Grabber",
                        "This file cannot be opened: " + file + "."
                    );
                    return;
            }
        }

        m_isInitialized = true;
        this->setStartState(true);

        const auto sigDuration = this->signal<DurationModifiedSignalType>(s_DURATION_MODIFIED_SIG);

        std::int64_t videoDuration = 0;
        if(!m_useTimelapse)
        {
            videoDuration = static_cast<std::int64_t>(m_imageToRead.size() * m_fps);
        }
        else
        {
            videoDuration = static_cast<std::int64_t>(m_imageTimestamps.back())
                            - static_cast<std::int64_t>(m_imageTimestamps.front());
        }

        sigDuration->asyncEmit(videoDuration);

        const auto sigPosition = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
        sigPosition->asyncEmit(0);

        if(m_oneShot)
        {
            m_timer = m_worker->createTimer();
            m_timer->setOneShot(true);
            m_timer->setFunction([this](auto&& ...){grabImage();});
            m_timer->setDuration(std::chrono::milliseconds(0));
            m_timer->start();
        }
        else
        {
            m_timer = m_worker->createTimer();

            core::thread::Timer::TimeDurationType duration;
            if(!m_useTimelapse)
            {
                duration = std::chrono::milliseconds(1000 / m_fps);
            }
            else if(m_imageToRead.size() >= 2)
            {
                duration = std::chrono::milliseconds(
                    static_cast<std::int64_t>(m_imageTimestamps[1] - m_imageTimestamps[0])
                );
                m_timer->setOneShot(true);
            }
            else
            {
                SIGHT_ERROR("Only one image to read, set 'oneShot' mode to true.");
                return;
            }

            m_timer->setFunction([this](auto&& ...){grabImage();});
            m_timer->setDuration(duration);
            m_timer->start();
        }

        auto sig = this->signal<IGrabber::CameraStartedSignalType>(IGrabber::s_CAMERA_STARTED_SIG);
        sig->asyncEmit();
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::grabVideo()
{
    core::mt::ScopedLock lock(m_mutex);

    if(m_videoCapture.isOpened())
    {
        const double timestamp = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>
                                                         (std::chrono::system_clock::now().time_since_epoch()).count());

        const bool isGrabbed = m_videoCapture.grab();

        if(isGrabbed)
        {
            auto frameTL = m_frame.lock();

            cv::Mat image;
            m_videoCapture.retrieve(image);

            if(!m_isInitialized)
            {
                const auto width  = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
                const auto height = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

                const std::size_t w = static_cast<std::size_t>(image.size().width);
                const std::size_t h = static_cast<std::size_t>(image.size().height);

                if(width != w || height != h)
                {
                    SIGHT_ERROR(
                        "This video cannot be read, the frame size is not expected. expected: "
                        << width << " x " << height << ", actual: " << w << " x " << h
                    );
                    return;
                }

                switch(image.type())
                {
                    case CV_8UC1:
                        frameTL->initPoolSize(
                            width,
                            height,
                            core::Type::UINT8,
                            sight::data::FrameTL::PixelFormat::GRAY_SCALE
                        );
                        break;

                    case CV_8UC3:
                        frameTL->initPoolSize(
                            width,
                            height,
                            core::Type::UINT8,
                            sight::data::FrameTL::PixelFormat::RGB
                        );
                        break;

                    case CV_8UC4:
                        frameTL->initPoolSize(
                            width,
                            height,
                            core::Type::UINT8,
                            sight::data::FrameTL::PixelFormat::RGBA
                        );
                        break;

                    case CV_16UC1:
                        frameTL->initPoolSize(
                            width,
                            height,
                            core::Type::UINT16,
                            sight::data::FrameTL::PixelFormat::GRAY_SCALE
                        );
                        break;

                    default:
                        sight::ui::base::dialog::MessageDialog::show(
                            "Grabber",
                            "This video cannot be read, the video type is not managed."
                        );
                        return;
                }

                m_isInitialized = true;
            }

            // Get time slider position.
            const auto ms          = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_POS_MSEC));
            const auto sigPosition = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
            sigPosition->asyncEmit(static_cast<std::int64_t>(ms));

            // Get the buffer of the timeline to fill
            SPTR(data::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
            std::uint8_t* frameBuffOut = bufferOut->addElement(0);

            // Create an OpenCV mat that aliases the buffer created from the output timeline.
            cv::Mat imgOut(image.size(), image.type(), (void*) frameBuffOut, cv::Mat::AUTO_STEP);

            if(image.type() == CV_8UC3)
            {
                // Convert the read image from BGR to RGB.
                cv::cvtColor(image, imgOut, cv::COLOR_BGR2RGB);
            }
            else if(image.type() == CV_8UC4)
            {
                // Convert the read image from BGRA to RGBA.
                cv::cvtColor(image, imgOut, cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(imgOut);
            }

            frameTL->pushObject(bufferOut);

            const auto sig =
                frameTL->signal<data::TimeLine::ObjectPushedSignalType>(data::TimeLine::s_OBJECT_PUSHED_SIG);
            sig->asyncEmit(timestamp);
        }

        if(m_loopVideo)
        {
            // Loop the video.
            const auto currentF = static_cast<std::size_t>(m_videoCapture.get(cv::CAP_PROP_POS_FRAMES));

            if(currentF == m_videoFramesNb)
            {
                m_videoCapture.set(cv::CAP_PROP_POS_MSEC, 0.);
            }
        }
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::grabImage()
{
    const double t0 = core::HiResClock::getTimeInMilliSec();

    core::mt::ScopedLock lock(m_mutex);

    // When using time lapse, the timer is set to "one shot": it is stopped when this method is called and re-started
    // at the end of it. So we need to add a boolean to check if the grabber is paused when the method is called.
    if(!m_isPaused && m_imageCount < m_imageToRead.size())
    {
        const auto frameTL = m_frame.lock();

        const std::filesystem::path imagePath = m_imageToRead[m_imageCount];

        const cv::Mat image                        = cv::imread(imagePath.string(), cv::IMREAD_UNCHANGED);
        core::HiResClock::HiResClockType timestamp = NAN;

        //create a new timestamp
        if(m_createNewTS)
        {
            timestamp = core::HiResClock::getTimeInMilliSec();
        }
        //use the image timestamp
        else
        {
            timestamp = m_imageTimestamps[m_imageCount];
        }

        SIGHT_DEBUG("Reading image index " << m_imageCount << " with timestamp " << timestamp);

        const std::size_t width  = static_cast<std::size_t>(image.size().width);
        const std::size_t height = static_cast<std::size_t>(image.size().height);

        if(width == frameTL->getWidth() && height == frameTL->getHeight())
        {
            const auto sigPosition = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
            sigPosition->asyncEmit(static_cast<std::int64_t>(m_imageCount) * 30);

            // Get the buffer of the timeline to fill
            SPTR(data::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
            std::uint8_t* frameBuffOut = bufferOut->addElement(0);

            // Create an openCV mat that aliases the buffer created from the output timeline
            cv::Mat imgOut(image.size(), image.type(), (void*) frameBuffOut, cv::Mat::AUTO_STEP);

            if(image.type() == CV_8UC3)
            {
                // convert the readded image from BGR to RGB
                cv::cvtColor(image, imgOut, cv::COLOR_BGR2RGB);
            }
            else if(image.type() == CV_8UC4)
            {
                // convert the readded image from BGRA to RGBA
                cv::cvtColor(image, imgOut, cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(imgOut);
            }

            frameTL->pushObject(bufferOut);

            const auto sig =
                frameTL->signal<data::TimeLine::ObjectPushedSignalType>(data::TimeLine::s_OBJECT_PUSHED_SIG);
            sig->asyncEmit(timestamp);

            const double t1          = core::HiResClock::getTimeInMilliSec();
            const double elapsedTime = t1 - t0;

            if(m_useTimelapse)
            {
                double nextDuration = 0.;

                const std::size_t currentImage = m_imageCount;
                const double currentTime       = m_imageTimestamps[currentImage] + elapsedTime;

                // If the next image delay is already passed, drop the image and check the next one.
                while(nextDuration < elapsedTime && m_imageCount + m_step < m_imageTimestamps.size())
                {
                    nextDuration  = m_imageTimestamps[m_imageCount + m_step] - currentTime;
                    m_imageCount += m_step;
                }

                // If it is the last image: stop the timer or loop
                if(m_imageCount + m_step == m_imageToRead.size())
                {
                    m_timer->stop();
                    if(m_loopVideo)
                    {
                        m_imageCount = 0;
                        core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(1000 / m_fps);
                        m_timer->setDuration(duration);
                        m_timer->start();
                    }
                }
                else
                {
                    core::thread::Timer::TimeDurationType duration =
                        std::chrono::milliseconds(static_cast<std::int64_t>(nextDuration));
                    m_timer->stop();
                    m_timer->setDuration(duration);
                    m_timer->start();
                }
            }
            else
            {
                m_imageCount += m_step;
            }
        }
        else
        {
            SIGHT_ERROR("Images doesn't have the same size.");
        }
    }
    else if(!m_isPaused && m_loopVideo)
    {
        m_imageCount = 0;
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::toggleLoopMode()
{
    m_loopVideo = !m_loopVideo;
}

// -----------------------------------------------------------------------------

void SFrameGrabber::setPosition(int64_t position)
{
    core::mt::ScopedLock lock(m_mutex);

    if(m_videoCapture.isOpened())
    {
        m_videoCapture.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(position));
    }
    else if(!m_imageToRead.empty())
    {
        const auto newPos = static_cast<std::size_t>(position / 30);
        if(newPos < m_imageToRead.size())
        {
            m_imageCount = newPos;
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::nextImage()
{
    if(m_oneShot)
    {
        // Compute difference between a possible step change in setStep() slot and the current step value
        const std::int64_t shift = static_cast<std::int64_t>(m_stepChanged)
                                   - static_cast<std::int64_t>(m_step);
        const std::int64_t shiftedImageCount = static_cast<std::int64_t>(m_imageCount) + shift;

        if(shiftedImageCount < static_cast<std::int64_t>(m_imageToRead.size()))
        {
            // Update image position index
            m_imageCount = static_cast<std::size_t>(shiftedImageCount);
            m_step       = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Grabber",
                "No more image to read."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::previousImage()
{
    if(m_oneShot)
    {
        if(m_imageCount - m_step >= m_stepChanged)
        {
            // Compute difference between a possible step change in setStep() slot and the current step value
            const std::int64_t shift = static_cast<std::int64_t>(m_stepChanged)
                                       - static_cast<std::int64_t>(m_step);
            const std::int64_t shiftedimageCount = static_cast<std::int64_t>(m_imageCount) - shift;

            // Update image position index
            // m_imageCount is pointing to next image, so -1 = present image
            m_imageCount = static_cast<std::size_t>(shiftedimageCount) - (2 * m_step);
            m_step       = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Grabber",
                "No previous image."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::setStep(int step, std::string key)
{
    if(key == "step")
    {
        SIGHT_ASSERT("Needed step value (" << step << ") should be > 0.", step > 0);
        // Save the changed step value
        m_stepChanged = static_cast<std::uint64_t>(step);
    }
    else
    {
        SIGHT_WARN("Only 'step' key is supported (current key value is : '" << key << "').");
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video

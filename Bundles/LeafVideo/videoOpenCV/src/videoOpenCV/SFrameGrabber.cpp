/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOpenCV/SFrameGrabber.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace videoOpenCV
{

static const ::fwServices::IService::KeyType s_FRAMETL = "frameTL";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::arServices::IGrabber, ::videoOpenCV::SFrameGrabber, ::arData::FrameTL);

// -----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() throw() :
    m_loopVideo(false),
    m_isInitialized(false),
    m_imageCount(0)
{
    m_worker = ::fwThread::Worker::New();
}

// -----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() throw()
{
}

// -----------------------------------------------------------------------------

void SFrameGrabber::starting() throw(::fwTools::Failed)
{
}

// -----------------------------------------------------------------------------

void SFrameGrabber::stopping() throw(::fwTools::Failed)
{
    this->stopCamera();
}

// -----------------------------------------------------------------------------

void SFrameGrabber::configuring()  throw ( ::fwTools::Failed )
{

}

// -----------------------------------------------------------------------------

void SFrameGrabber::updating() throw ( ::fwTools::Failed )
{
}

// -----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    if (m_timer)
    {
        this->stopCamera();
    }

    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >("camera");

    if (camera->getCameraSource() == ::arData::Camera::FILE)
    {
        ::boost::filesystem::path file = camera->getVideoFile();
        ::boost::filesystem::path videoDir(::arPreferences::getVideoDir());

        // For compatibility with old calibration with absolute path
        if (!file.is_absolute())
        {
            file = videoDir / file;
        }

        ::boost::filesystem::path ext = file.extension();

        if (ext.string() == ".png" || ext.string() == ".jpg" || ext.string() == ".tiff" )
        {
            this->readImages(file.parent_path(), ext.string());
        }
        else
        {
            this->readVideo(file);
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Grabber",
            "This video source is not managed by this grabber.");
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    m_timer->isRunning() ? m_timer->stop() : m_timer->start();
}

// -----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_timer)
    {
        if (m_timer->isRunning())
        {
            m_timer->stop();
        }
        m_timer.reset();
    }
    if (m_videoCapture.isOpened())
    {
        m_videoCapture.release();
    }
    m_imageToRead.clear();
    m_imageCount = 0;

    if (m_isInitialized)
    {
        // Clear the timeline: send a black frame
        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(static_cast<std::int64_t>(-1));

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(-1));

        ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

        const ::fwCore::HiResClock::HiResClockType timestamp = frameTL->getNewerTimestamp() + 1;

        SPTR(::arData::FrameTL::BufferType) buffer = frameTL->createBuffer(timestamp);
        ::boost::uint8_t* destBuffer               = reinterpret_cast< ::boost::uint8_t* >( buffer->addElement(0) );

        std::fill(destBuffer,
                  destBuffer + frameTL->getWidth() * frameTL->getHeight() * frameTL->getNumberOfComponents(), 0);

        // push buffer and notify
        frameTL->clearTimeline();
        frameTL->pushObject(buffer);

        auto sigTL = frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sigTL->asyncEmit(timestamp);

        auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
            ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
        sig->asyncEmit();
    }
    m_isInitialized = false;
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readVideo(const ::boost::filesystem::path& file)
{
    ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

    ::fwCore::mt::ScopedLock lock(m_mutex);

    m_videoCapture.open(file.string());

    if (m_videoCapture.isOpened())
    {
        m_timer = m_worker->createTimer();

        size_t fps      = static_cast<size_t>(m_videoCapture.get(::cv::CAP_PROP_FPS));
        size_t nbFrames = static_cast<size_t>(m_videoCapture.get(::cv::CAP_PROP_FRAME_COUNT));

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(nbFrames * fps));

        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(0);

        ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(1000 / fps);

        m_timer->setFunction(std::bind(&SFrameGrabber::grabVideo, this));
        m_timer->setDuration(duration);
        m_timer->start();
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Grabber",
            "This file cannot be opened: " + file.string() + ".");
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::readImages(const ::boost::filesystem::path& folder, const std::string& extension)
{
    ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

    ::fwCore::mt::ScopedLock lock(m_mutex);

    ::boost::filesystem::directory_iterator currentEntry(folder);
    ::boost::filesystem::directory_iterator endEntry;
    for(; currentEntry != endEntry; ++currentEntry)
    {
        ::boost::filesystem::path entryPath = *currentEntry;

        if (entryPath.extension() == extension)
        {
            m_imageToRead.push_back(entryPath);
        }
    }

    // Sort in alphabetical order (ex: img_001, img_002...)
    std::sort(m_imageToRead.begin(), m_imageToRead.end());

    if (!m_imageToRead.empty())
    {
        std::string file = m_imageToRead.front().string();
        ::cv::Mat image = ::cv::imread(file, ::cv::IMREAD_UNCHANGED);

        int width  = image.size().width;
        int height = image.size().height;
        int type   = image.type();

        if (width > 0 && height > 0)
        {
            size_t w = static_cast<size_t>(width);
            size_t h = static_cast<size_t>(height);
            switch (type)
            {
                case CV_8UC3:
                    frameTL->initPoolSize(w, h, ::fwTools::Type::s_UINT8, 3);
                    break;
                case CV_8UC4:
                    frameTL->initPoolSize(w, h, ::fwTools::Type::s_UINT8, 4);
                    break;
                case CV_16UC1:
                    frameTL->initPoolSize(w, h, ::fwTools::Type::s_UINT16, 1);
                    break;
                default:
                    ::fwGui::dialog::MessageDialog::showMessageDialog(
                        "Grabber",
                        "This file cannot be opened: " + file + ".");
                    return;
            }
        }
        m_isInitialized = true;

        /// FIXME allow to configure the timestamp (or read it in the image name ?)
        size_t fps = 33;

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(m_imageToRead.size() * fps));

        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(0);

        m_timer = m_worker->createTimer();

        ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(fps);

        m_timer->setFunction(std::bind(&SFrameGrabber::grabImage, this));
        m_timer->setDuration(duration);
        m_timer->start();
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::grabVideo()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_videoCapture.isOpened())
    {
        ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

        ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        bool isGrabbed = m_videoCapture.grab();

        if (isGrabbed)
        {
            ::cv::Mat image;
            m_videoCapture.retrieve(image);

            if (!m_isInitialized)
            {
                size_t width  = static_cast<size_t>(m_videoCapture.get(::cv::CAP_PROP_FRAME_WIDTH));
                size_t height = static_cast<size_t>(m_videoCapture.get(::cv::CAP_PROP_FRAME_HEIGHT));

                size_t w = static_cast<size_t>(image.size().width);
                size_t h = static_cast<size_t>(image.size().height);

                if (width != w || height != h)
                {
                    OSLM_ERROR("This video cannot be read, the frame size is not expected. expected: "
                               << width << " x " << height << ", actual: " << w << " x " << h);
                    return;
                }

                switch (image.type())
                {
                    case CV_8UC3:
                        frameTL->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 3);
                        break;
                    case CV_8UC4:
                        frameTL->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 4);
                        break;
                    case CV_16UC1:
                        frameTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
                        break;
                    default:
                        ::fwGui::dialog::MessageDialog::showMessageDialog(
                            "Grabber",
                            "This video cannot be read, the video type is not managed.");
                        return;
                }
                m_isInitialized = true;
            }

            // Get time slider position
            size_t ms        = static_cast<size_t>(m_videoCapture.get(::cv::CAP_PROP_POS_MSEC));
            auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
            sigPosition->asyncEmit(static_cast<std::int64_t>(ms));

            // Get the buffer of the timeline to fill
            SPTR(::arData::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
            ::boost::uint8_t* frameBuffOut                = bufferOut->addElement(0);

            // Create an openCV mat that aliases the buffer created from the output timeline
            ::cv::Mat imgOut(image.size(), image.type(), (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

            if (image.type() == CV_8UC3)
            {
                // convert the readded image from BGR to RGB
                ::cv::cvtColor(image, imgOut, ::cv::COLOR_BGR2RGB);
            }
            else if (image.type() == CV_8UC4)
            {
                // convert the readded image from BGRA to RGBA
                ::cv::cvtColor(image, imgOut, ::cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(imgOut);
            }

            frameTL->pushObject(bufferOut);

            auto sig =
                frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG);
            sig->asyncEmit(timestamp);
        }

        if (m_loopVideo)
        {
            // loop the video
            double ratio = m_videoCapture.get(::cv::CAP_PROP_POS_AVI_RATIO);
            if (ratio == 1)
            {
                m_videoCapture.set(::cv::CAP_PROP_POS_MSEC, 0);
            }
        }
    }
}

// -----------------------------------------------------------------------------

void SFrameGrabber::grabImage()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_imageCount < m_imageToRead.size())
    {
        ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

        const ::boost::filesystem::path imagePath = m_imageToRead[m_imageCount];

        const std::string imageName = imagePath.filename().string();
        static const boost::regex s_TIMESTAMP("[^0-9]*([0-9]*)[^0-9]*");
        boost::smatch match;
        if (!boost::regex_match(imageName, match, s_TIMESTAMP))
        {
            SLM_ERROR("Could not find a timestamp in file name: " + imageName);
            return;
        }
        const std::string timestampStr = match[1].str();

        ::cv::Mat image = ::cv::imread(imagePath.string(), ::cv::IMREAD_UNCHANGED);

        ::fwCore::HiResClock::HiResClockType timestamp = std::stod(timestampStr);

        const size_t width  = static_cast<size_t>(image.size().width);
        const size_t height = static_cast<size_t>(image.size().height);

        if (width == frameTL->getWidth() && height == frameTL->getHeight())
        {

            auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
            sigPosition->asyncEmit(static_cast<std::int64_t>(m_imageCount)  * 30);

            // Get the buffer of the timeline to fill
            SPTR(::arData::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
            ::boost::uint8_t* frameBuffOut                = bufferOut->addElement(0);

            // Create an openCV mat that aliases the buffer created from the output timeline
            ::cv::Mat imgOut(image.size(), image.type(), (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

            if (image.type() == CV_8UC3)
            {
                // convert the readded image from BGR to RGB
                ::cv::cvtColor(image, imgOut, ::cv::COLOR_BGR2RGB);
            }
            else if (image.type() == CV_8UC4)
            {
                // convert the readded image from BGRA to RGBA
                ::cv::cvtColor(image, imgOut, ::cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(imgOut);
            }

            frameTL->pushObject(bufferOut);

            auto sig =
                frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG);
            sig->asyncEmit(timestamp);

            m_imageCount++;
        }
        else
        {
            SLM_ERROR("Images doesn't have the same size.");
        }
    }
    else if (m_loopVideo)
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
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_videoCapture.isOpened())
    {
        m_videoCapture.set(::cv::CAP_PROP_POS_MSEC, static_cast<int>(position));
    }
    else if (!m_imageToRead.empty())
    {
        size_t newPos = static_cast<size_t>(position / 30);
        if (newPos < m_imageToRead.size())
        {
            m_imageCount = newPos;
        }
    }
}

// -----------------------------------------------------------------------------

} // namespace videoOpenCV

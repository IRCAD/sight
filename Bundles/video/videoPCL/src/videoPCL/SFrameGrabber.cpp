/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoPCL/SFrameGrabber.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

#include <pcl/common/transforms.h>
#include <pcl/io/vtk_lib_io.h>

#include <cstdint>

namespace videoPCL
{

static const ::fwServices::IService::KeyType s_FRAMETL = "frameTL";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::arServices::IGrabber, ::videoPCL::SFrameGrabber, ::arData::FrameTL);

//------------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() noexcept :
    m_loopVideo(false),
    m_isInitialized(false),
    m_fps(30),
    m_imageCount(0)
{
    m_worker = ::fwThread::Worker::New();
}

//------------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() noexcept
{
}

//------------------------------------------------------------------------------

void SFrameGrabber::starting()
{
}

//------------------------------------------------------------------------------

void SFrameGrabber::stopping()
{
    this->stopCamera();
}

//------------------------------------------------------------------------------

void SFrameGrabber::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_fps = config.get<unsigned int>("fps", 30);

    OSLM_ASSERT("Fps setting is set to " << m_fps << " but should be in ]0;60].", m_fps > 0 && m_fps <= 60);
}

//------------------------------------------------------------------------------

void SFrameGrabber::updating()
{
}

//------------------------------------------------------------------------------

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
        const ::boost::filesystem::path videoDir(::arPreferences::getVideoDir());

        // For compatibility with old calibration with absolute path
        if (!file.is_absolute())
        {
            file = videoDir / file;
        }

        const ::boost::filesystem::path ext = file.extension();

        if (ext.string() == ".pcd" )
        {
            this->readImages(file.parent_path(), ext.string());
        }
        else
        {
            OSLM_ERROR("Wrong file format. The format should be *.pcd.");
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Grabber",
            "This video source is not managed by this grabber.");
    }
}

//------------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    if (m_timer)
    {
        m_timer->isRunning() ? m_timer->stop() : m_timer->start();
    }
}

//------------------------------------------------------------------------------

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
        std::uint8_t* destBuffer = reinterpret_cast< std::uint8_t* >( buffer->addElement(0) );

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

//------------------------------------------------------------------------------

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
        ::pcl::PointCloud< ::pcl::PointXYZ> inputCloud;
        if (::pcl::io::loadPCDFile< ::pcl::PointXYZ> (m_imageToRead.front().string(), inputCloud) == -1)
        {
            SLM_ERROR("Couldn't read input pointcloud file " +  m_imageToRead.front().string());
        }

        const size_t width  = inputCloud.width;
        const size_t height = inputCloud.height;

        if (width != 0 && height != 0)
        {
            frameTL->initPoolSize(width, height, ::fwTools::Type::s_FLOAT, 3);
        }
        else
        {
            SLM_ERROR("Image width or height is equal to 0.");
            return;
        }
        m_isInitialized = true;

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(m_imageToRead.size() * m_fps));

        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(0);

        m_timer = m_worker->createTimer();

        ::fwThread::Timer::TimeDurationType duration = std::chrono::milliseconds(1000/m_fps);

        m_timer->setFunction(std::bind(&SFrameGrabber::grabImage, this));
        m_timer->setDuration(duration);
        m_timer->start();
    }
}

//------------------------------------------------------------------------------

void SFrameGrabber::grabImage()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_imageCount < m_imageToRead.size())
    {
        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL);

        const ::boost::filesystem::path imagePath = m_imageToRead[m_imageCount];

        const std::string imageName = imagePath.filename().string();
        static const ::boost::regex s_TIMESTAMP("[^0-9]*([0-9]*)[^0-9]*");
        ::boost::smatch match;
        if (!::boost::regex_match(imageName, match, s_TIMESTAMP))
        {
            SLM_ERROR("Could not find a timestamp in file name: " + imageName);
            return;
        }
        const std::string timestampStr = match[1].str();

        ::pcl::PointCloud< ::pcl::PointXYZ> inputCloud;
        if (::pcl::io::loadPCDFile< ::pcl::PointXYZ> (imagePath.string(), inputCloud) == -1)
        {
            SLM_ERROR("Couldn't read input pointcloud file " +  imagePath.string());
            return;
        }

        const ::fwCore::HiResClock::HiResClockType timestamp = std::stod(timestampStr);

        const size_t width  = static_cast<size_t>(inputCloud.width);
        const size_t height = static_cast<size_t>(inputCloud.height);

        if (width == frameTL->getWidth() && height == frameTL->getHeight())
        {
            auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
            sigPosition->asyncEmit(static_cast<std::int64_t>(m_imageCount)  * m_fps);

            // Get the buffer of the timeline to fill
            SPTR(::arData::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
            float* frameBuffOut = reinterpret_cast< float* >( bufferOut->addElement(0));

            for(const auto& pt : inputCloud.points)
            {
                frameBuffOut[0] = pt.x;
                frameBuffOut[1] = pt.y;
                frameBuffOut[2] = pt.z;
                frameBuffOut   += 3;
            }

            frameTL->pushObject(bufferOut);

            auto sig =
                frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG);
            sig->asyncEmit(timestamp);

            m_imageCount++;
        }
        else
        {
            SLM_ERROR("Images don't have the same size.");
        }
    }
    else if (m_loopVideo)
    {
        m_imageCount = 0;
    }
}

//------------------------------------------------------------------------------

void SFrameGrabber::toggleLoopMode()
{
    m_loopVideo = !m_loopVideo;
}

//------------------------------------------------------------------------------

void SFrameGrabber::setPosition(int64_t position)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (!m_imageToRead.empty())
    {
        const size_t newPos = static_cast<size_t>(position / 30);
        if (newPos < m_imageToRead.size())
        {
            m_imageCount = newPos;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace videoPCL

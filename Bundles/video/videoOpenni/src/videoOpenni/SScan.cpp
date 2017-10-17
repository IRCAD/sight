/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOpenni/SScan.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace videoOpenni
{

// -----------------------------------------------------------------------------

// Despite this grabber inherits from ::arServices::IRGBDGrabber, we register it as ::arServices::IGrabber since it
// is compatible with this interface. Indeed it is capable to output only the color frame.
fwServicesRegisterMacro( ::arServices::IRGBDGrabber, ::videoOpenni::SScan);
fwServicesRegisterObjectMacro( ::videoOpenni::SScan, ::arData::FrameTL );
fwServicesRegisterObjectMacro( ::videoOpenni::SScan, ::arData::FrameTL );
fwServicesRegisterObjectMacro( ::videoOpenni::SScan, ::arData::FrameTL );

const ::fwCom::Slots::SlotKeyType SScan::s_TAKE_SNAPSHOT_FRAME = "takeSnapshot";

// -----------------------------------------------------------------------------

SScan::SScan() noexcept :
    m_streams(nullptr),
    m_capture(false),
    m_pause(false)
{
    newSlot( s_TAKE_SNAPSHOT_FRAME, &SScan::takeSnapshot, this );

    // Do not register this slot in the service, we want to put it on its own worker
    m_slotPresentFrame = ::fwCom::newSlot( &SScan::presentFrame, this );

    // Create a worker to fill the timelines
    m_worker = ::fwThread::Worker::New();
    m_slotPresentFrame->setWorker(m_worker);
}

// -----------------------------------------------------------------------------

SScan::~SScan() noexcept
{

}

// -----------------------------------------------------------------------------

void SScan::configuring()
{
}

// -----------------------------------------------------------------------------

void SScan::starting()
{
    m_depthTL = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
    m_colorTL = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
    m_irTL    = this->getInOut< ::arData::FrameTL>(s_IRTL_INOUT);

    m_snapshotDepthTL = this->getInOut< ::arData::FrameTL>("snapshotTLDepth");
    m_snapshotColorTL = this->getInOut< ::arData::FrameTL>("snapshotTLColors");
    m_snapshotIRTL    = this->getInOut< ::arData::FrameTL>("snapshotTLIR");

    m_status = ::openni::OpenNI::initialize();
    if (m_status != ::openni::STATUS_OK)
    {
        this->stopping();
        SLM_ASSERT("Initialize failed." << ::openni::OpenNI::getExtendedError(), m_status != ::openni::STATUS_OK);
    }

    m_status = m_camera.open(::openni::ANY_DEVICE);
    if (m_status != ::openni::STATUS_OK)
    {
        OSLM_ERROR("Device open failed." << ::openni::OpenNI::getExtendedError());
        return;
    }

    // Create configured streams
    size_t index = 0;
    m_streams = new ::openni::VideoStream*[3];

    if(m_depthTL != nullptr || m_snapshotDepthTL != nullptr)
    {
        m_status = m_depthStream.create(m_camera, ::openni::SENSOR_DEPTH);
        m_depthStream.setMirroringEnabled(false);

        m_camera.setProperty(ONI_DEVICE_PROPERTY_IMAGE_REGISTRATION, ONI_IMAGE_REGISTRATION_OFF);

        if(m_snapshotDepthTL != nullptr)
        {
            // Initialize timeline
            const size_t width  = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionX());
            const size_t height = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionY());

            m_snapshotDepthTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
            m_snapshotDepthTL->setMaximumSize(50);
        }

        m_streamMap[index] = &m_depthStream;
        m_streams[index++] = &m_depthStream;
    }

    if(m_colorTL != nullptr || m_snapshotColorTL != nullptr)
    {
        m_status = m_colorStream.create(m_camera, ::openni::SENSOR_COLOR);
        m_colorStream.setMirroringEnabled(false);

        if(m_snapshotColorTL != nullptr)
        {
            // Initialize timeline
            const size_t width  = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionX());
            const size_t height = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionY());

            m_snapshotColorTL->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 4);
            m_snapshotColorTL->setMaximumSize(50);
        }

        m_streamMap[index] = &m_colorStream;
        m_streams[index++] = &m_colorStream;
    }

    if(m_irTL != nullptr || m_snapshotIRTL != nullptr)
    {
        m_status = m_irStream.create(m_camera, ::openni::SENSOR_IR);
        m_irStream.setMirroringEnabled(false);

        if(m_snapshotIRTL != nullptr)
        {
            // Initialize timeline
            const size_t width  = static_cast<size_t>(m_irStream.getVideoMode().getResolutionX());
            const size_t height = static_cast<size_t>(m_irStream.getVideoMode().getResolutionY());

            m_snapshotIRTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
            m_snapshotIRTL->setMaximumSize(50);
        }

        m_streamMap[index] = &m_irStream;
        m_streams[index++] = &m_irStream;
    }
}

// -----------------------------------------------------------------------------

void SScan::stopping()
{
    this->stopCamera();

    // Stop the worker
    m_worker->stop();

    m_streamMap.clear();

    if (m_streams != nullptr)
    {
        delete [] m_streams;
        m_streams = nullptr;
    }

    m_irStream.destroy();
    m_colorStream.destroy();
    m_depthStream.destroy();

    if (m_camera.isValid())
    {
        m_camera.close();
    }

    ::openni::OpenNI::shutdown();

}

// -----------------------------------------------------------------------------

void SScan::updating()
{
}

// -----------------------------------------------------------------------------

void SScan::startCamera()
{
    this->stopCamera();

    if (m_status != ::openni::STATUS_OK)
    {
        OSLM_ERROR("Device open failed." << ::openni::OpenNI::getExtendedError());
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Sense Error",
            "Sense device is not available. Please check if it is plugged in.",
            ::fwGui::dialog::IMessageDialog::CRITICAL);
        return;
    }

    if(m_depthTL != nullptr)
    {
        if (m_status == ::openni::STATUS_OK)
        {
            m_status = m_depthStream.start();
            if (m_status != ::openni::STATUS_OK)
            {
                m_depthStream.destroy();
                SLM_ASSERT("Couldn't start depth stream." << ::openni::OpenNI::getExtendedError(),
                           m_status == ::openni::STATUS_OK);
            }

        }
        else
        {
            SLM_ASSERT("Couldn't find depth stream." << ::openni::OpenNI::getExtendedError(),
                       m_status == ::openni::STATUS_OK);
        }

        // Initialize timeline
        const size_t width  = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionX());
        const size_t height = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionY());

        m_depthTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
        m_depthTL->setMaximumSize(50);
    }

    if(m_colorTL != nullptr)
    {
        if (m_status == ::openni::STATUS_OK)
        {
            m_status = m_colorStream.start();
            if (m_status != ::openni::STATUS_OK)
            {
                m_colorStream.destroy();
                SLM_ASSERT("Couldn't start color stream." << ::openni::OpenNI::getExtendedError(),
                           m_status == ::openni::STATUS_OK);
            }
        }
        else
        {
            SLM_ASSERT("Couldn't find color stream." << ::openni::OpenNI::getExtendedError(),
                       m_status == ::openni::STATUS_OK);
        }

        // Initialize timeline
        const size_t width  = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionX());
        const size_t height = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionY());

        m_colorTL->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 4);
        m_colorTL->setMaximumSize(50);
    }

    if(m_irTL != nullptr)
    {
        if (m_status == ::openni::STATUS_OK)
        {
            m_status = m_irStream.start();
            if (m_status != ::openni::STATUS_OK)
            {
                m_irStream.destroy();
                SLM_ASSERT("Couldn't start IR stream." << ::openni::OpenNI::getExtendedError(),
                           m_status == ::openni::STATUS_OK);
            }
        }
        else
        {
            SLM_ASSERT("Couldn't find IR stream." << ::openni::OpenNI::getExtendedError(),
                       m_status == ::openni::STATUS_OK);
        }

        // Initialize timeline
        const size_t width  = static_cast<size_t>(m_irStream.getVideoMode().getResolutionX());
        const size_t height = static_cast<size_t>(m_irStream.getVideoMode().getResolutionY());

        m_irTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
        m_irTL->setMaximumSize(50);
    }

    m_slotPresentFrame->asyncRun();

    auto sig = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

void SScan::stopCamera()
{
    auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
    sig->asyncEmit();

    if (m_depthStream.isValid())
    {
        m_depthStream.stop();
    }
    if (m_colorStream.isValid())
    {
        m_colorStream.stop();
    }
    if (m_irStream.isValid())
    {
        m_irStream.stop();
    }

    m_pause = false;
}

// -----------------------------------------------------------------------------

void SScan::pauseCamera()
{
    m_pause = !m_pause;
}

//------------------------------------------------------------------------------

void SScan::takeSnapshot()
{
    std::array<bool, 3> startedStreams;
    startedStreams.fill(false);

    // Stop all streams to avoid conflicts with simultaneous streams
    for(auto& stream : m_streamMap)
    {
        startedStreams[stream.first] = true;
        stream.second->stop();
    }

    m_capture = true;

    // Capture depth
    if(m_snapshotDepthTL != nullptr)
    {
        m_status = m_depthStream.start();
        OSLM_ERROR_IF("Couldn't start depth stream." << ::openni::OpenNI::getExtendedError(),
                      m_status != ::openni::STATUS_OK);
        m_slotPresentFrame->asyncRun().wait_for(std::chrono::seconds(5));
        m_depthStream.stop();
    }

    // Capture color
    if(m_snapshotColorTL != nullptr)
    {
        m_status = m_colorStream.start();
        OSLM_ERROR_IF("Couldn't start color stream." << ::openni::OpenNI::getExtendedError(),
                      m_status != ::openni::STATUS_OK);
        m_slotPresentFrame->asyncRun().wait_for(std::chrono::seconds(5));
        m_colorStream.stop();
    }

    // Capture IR
    if(m_snapshotIRTL != nullptr)
    {
        m_status = m_irStream.start();
        OSLM_ERROR_IF("Couldn't start IR stream." << ::openni::OpenNI::getExtendedError(),
                      m_status != ::openni::STATUS_OK);
        m_slotPresentFrame->asyncRun().wait_for(std::chrono::seconds(5));
        m_irStream.stop();
    }

    m_capture = false;

    // Restart previous streams
    for(auto& stream : m_streamMap)
    {
        if(startedStreams[stream.first])
        {
            stream.second->start();
        }
    }

    m_slotPresentFrame->asyncRun();
}

// -----------------------------------------------------------------------------

void SScan::presentFrame()
{
    // Listening the streams
    int changedIndex;
    int frameCaptureCount = 5;

    while (::openni::OpenNI::waitForAnyStream(m_streams, static_cast<int>(m_streamMap.size()), &changedIndex, 1000) ==
           ::openni::STATUS_OK)
    {
        if(m_pause)
        {
            continue;
        }

        const ::openni::VideoStream* const changedVideoStream = m_streamMap[static_cast<size_t>(changedIndex)];

        const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        if (changedVideoStream == &m_depthStream)
        {
            m_depthFrame.release();
            m_depthStream.readFrame(&m_depthFrame);

            ::arData::FrameTL::sptr depthTL = m_capture ? m_snapshotDepthTL : m_depthTL;

            SPTR(::arData::FrameTL::BufferType) depthBuffer = depthTL->createBuffer(timestamp);
            uint16_t* destDepthBuffer = reinterpret_cast< uint16_t* >( depthBuffer->addElement(0) );

            // Filling depth buffer
            if (m_depthFrame.isValid())
            {
                ::openni::DepthPixel* pDepth = (::openni::DepthPixel*)m_depthFrame.getData();
                memcpy(destDepthBuffer, pDepth,  static_cast<size_t>(m_depthFrame.getDataSize()));
            }

            // Push buffer and notify
            depthTL->pushObject(depthBuffer);
            auto sig = depthTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
            sig->asyncEmit(timestamp);
        }

        if (changedVideoStream == &m_colorStream)
        {
            m_colorFrame.release();
            m_colorStream.readFrame(&m_colorFrame);

            ::arData::FrameTL::sptr colorTL = m_capture ? m_snapshotColorTL : m_colorTL;

            SPTR(::arData::FrameTL::BufferType) colorBuffer = colorTL->createBuffer(timestamp);
            uint32_t* destColorBuffer = reinterpret_cast< uint32_t* >( colorBuffer->addElement(0) );

            // Filling color buffer
            if (m_colorFrame.isValid())
            {
                const size_t sizeData = static_cast<size_t>(m_colorFrame.getHeight() * m_colorFrame.getWidth() * 3);
                const uint8_t* buffer = reinterpret_cast< const uint8_t* >(m_colorFrame.getData());
                for (size_t idx = 0; idx < sizeData; idx += 3)
                {
                    *destColorBuffer++ = (buffer[idx] & 0xFFu)
                                         | (buffer[idx+1] & 0xFFu) << 8
                                         | (buffer[idx+2] & 0xFFu) << 16
                                         | (255u & 0xFF) << 24u;
                }
            }

            colorTL->pushObject(colorBuffer);
            auto sig = colorTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
            sig->asyncEmit(timestamp);

        }

        if (changedVideoStream == &m_irStream)
        {
            m_irFrame.release();
            m_irStream.readFrame(&m_irFrame);

            ::arData::FrameTL::sptr irTL = m_capture ? m_snapshotIRTL : m_irTL;

            SPTR(::arData::FrameTL::BufferType) irBuffer = irTL->createBuffer(timestamp);
            uint16_t* destIRBuffer = reinterpret_cast< uint16_t* >( irBuffer->addElement(0) );

            // Filling depth buffer
            if (m_irStream.isValid())
            {
                ::openni::Grayscale16Pixel* pIR = (::openni::Grayscale16Pixel*)m_irFrame.getData();
                memcpy(destIRBuffer, pIR,  static_cast<size_t>(m_irFrame.getDataSize()));
            }

            // Push buffer and notify
            irTL->pushObject(irBuffer);
            auto sig = irTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
            sig->asyncEmit(timestamp);
        }

        // Exit asap if we are capturing, however we need to skip some frames especially for the color stream; the
        // first frame are a bit blueish...
        if(m_capture && frameCaptureCount-- == 0)
        {
            return;
        }
    }
}

// -----------------------------------------------------------------------------

} // namespace videoOpenni


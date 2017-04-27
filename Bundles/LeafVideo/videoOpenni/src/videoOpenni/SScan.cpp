/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOpenni/SScan.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace videoOpenni
{

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::arServices::IGrabber, ::videoOpenni::SScan);

// -----------------------------------------------------------------------------

SScan::SScan() throw() :
    m_streams(nullptr)
{
    // Do not register the slot in the service, we want to put it on its own worker
    m_slotPresentFrame = ::fwCom::newSlot( &SScan::presentFrame, this );

    // Create a worker to fill the timelines
    m_worker = ::fwThread::Worker::New();
    m_slotPresentFrame->setWorker(m_worker);
}

// -----------------------------------------------------------------------------

SScan::~SScan() throw()
{

}

// -----------------------------------------------------------------------------

void SScan::configuring()  throw ( ::fwTools::Failed )
{
}

// -----------------------------------------------------------------------------

void SScan::starting() throw(::fwTools::Failed)
{
    m_depthTimeline     = this->getInOut< ::arData::FrameTL>("frameTLDepth");
    m_colorTimeline     = this->getInOut< ::arData::FrameTL>("frameTLColors");
    m_positionsTimeline = this->getInOut< ::arData::FrameTL>("frameTLPositions");

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

    m_streams = new ::openni::VideoStream*[2];
}

// -----------------------------------------------------------------------------

void SScan::stopping() throw(::fwTools::Failed)
{
    this->stopCamera();

    // Stop the worker
    m_worker->stop();

    if (m_streams != nullptr)
    {
        delete [] m_streams;
        m_streams = nullptr;
    }
    m_depthStream.destroy();

    m_colorStream.destroy();

    if (m_camera.isValid())
    {
        m_camera.close();
    }

    ::openni::OpenNI::shutdown();

}

// -----------------------------------------------------------------------------

void SScan::updating() throw ( ::fwTools::Failed )
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

    // Initializes OpenNI streams
    m_status = m_depthStream.create(m_camera, ::openni::SENSOR_DEPTH);
    if (m_status == ::openni::STATUS_OK)
    {
        m_status = m_depthStream.start();
        if (m_status != ::openni::STATUS_OK)
        {
            m_depthStream.destroy();
            SLM_ASSERT("SimpleViewer: Couldn't start depth stream." << ::openni::OpenNI::getExtendedError(),
                       m_status == ::openni::STATUS_OK);
        }

        if( m_camera.isPropertySupported(ONI_DEVICE_PROPERTY_IMAGE_REGISTRATION) )
        {
            m_camera.setProperty(ONI_DEVICE_PROPERTY_IMAGE_REGISTRATION, ONI_IMAGE_REGISTRATION_OFF);
        }
        else
        {
            OSLM_ERROR("Depth to color registration is not supported, color matching will be wrong.");
        }
    }
    else
    {
        SLM_ASSERT("SimpleViewer: Couldn't find depth stream." << ::openni::OpenNI::getExtendedError(),
                   m_status == ::openni::STATUS_OK);
    }

    m_status = m_colorStream.create(m_camera, ::openni::SENSOR_COLOR);
    if (m_status == ::openni::STATUS_OK)
    {
        m_status = m_colorStream.start();
        if (m_status != ::openni::STATUS_OK)
        {
            m_colorStream.destroy();
            SLM_ASSERT("SimpleViewer: Couldn't start color stream." << ::openni::OpenNI::getExtendedError(),
                       m_status == ::openni::STATUS_OK);
        }
    }
    else
    {
        SLM_ASSERT("SimpleViewer: Couldn't find color stream." << ::openni::OpenNI::getExtendedError(),
                   m_status == ::openni::STATUS_OK);
    }

    if (!m_depthStream.isValid() || !m_colorStream.isValid())
    {
        this->stopCamera();
        SLM_ASSERT("SimpleViewer: No valid streams." << ::openni::OpenNI::getExtendedError(),
                   m_status == ::openni::STATUS_OK);
    }

    m_depthStream.setMirroringEnabled(false);
    m_colorStream.setMirroringEnabled(false);

    m_streams[0] = &m_depthStream;
    m_streams[1] = &m_colorStream;

    // Initialize timelines
    const size_t widthDepth  = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionX());
    const size_t heightDepth = static_cast<size_t>(m_depthStream.getVideoMode().getResolutionY());
    const size_t widthColor  = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionX());
    const size_t heightColor = static_cast<size_t>(m_colorStream.getVideoMode().getResolutionY());

    m_depthTimeline->initPoolSize(widthDepth, heightDepth, ::fwTools::Type::s_UINT16, 1);
    m_colorTimeline->initPoolSize(widthColor, heightColor, ::fwTools::Type::s_UINT8, 4);
    m_depthTimeline->setMaximumSize(50);
    m_colorTimeline->setMaximumSize(50);
    if(m_positionsTimeline)
    {
        m_positionsTimeline->initPoolSize(widthDepth, heightDepth, ::fwTools::Type::s_FLOAT, 3);
        m_positionsTimeline->setMaximumSize(50);
    }

    m_slotPresentFrame->asyncRun();

    auto sig = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

void SScan::stopCamera()
{
    if (m_depthStream.isValid())
    {
        m_depthStream.stop();
    }
    if (m_colorStream.isValid())
    {
        m_colorStream.stop();
    }

    auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

void SScan::pauseCamera()
{
    // TODO
}

// -----------------------------------------------------------------------------

void SScan::presentFrame()
{
    // Listening the streams
    int changedIndex;

    while (::openni::OpenNI::waitForAnyStream(m_streams, 2, &changedIndex, 1000) == ::openni::STATUS_OK)
    {
        ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
        if (changedIndex == 0)
        {
            m_depthFrame.release();
            m_depthStream.readFrame(&m_depthFrame);
        }
        if (changedIndex == 1)
        {
            m_colorFrame.release();
            m_colorStream.readFrame(&m_colorFrame);
        }

        SPTR(::arData::FrameTL::BufferType) depthBuffer = m_depthTimeline->createBuffer(timestamp);
        uint16_t* destDepthBuffer = reinterpret_cast< uint16_t* >( depthBuffer->addElement(0) );

        SPTR(::arData::FrameTL::BufferType) colorBuffer = m_colorTimeline->createBuffer(timestamp);
        uint32_t* destColorBuffer = reinterpret_cast< uint32_t* >( colorBuffer->addElement(0) );

        SPTR(::arData::FrameTL::BufferType) positionsBuffer;
        float* destPositionsBuffer;
        if (m_positionsTimeline)
        {
            positionsBuffer     = m_positionsTimeline->createBuffer(timestamp);
            destPositionsBuffer = reinterpret_cast< float* >( positionsBuffer->addElement(0) );
        }

        // Filling depth buffer
        if (m_depthFrame.isValid())
        {
            ::openni::DepthPixel* pDepth = (::openni::DepthPixel*)m_depthFrame.getData();
            memcpy(destDepthBuffer, pDepth,  static_cast<size_t>(m_depthFrame.getDataSize()));
            if (m_positionsTimeline)
            {
                const int height = m_depthFrame.getHeight();
                const int width  = m_depthFrame.getWidth();
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        ::openni::CoordinateConverter::convertDepthToWorld(m_depthStream, x, y, *pDepth,
                                                                           &destPositionsBuffer[0],
                                                                           &destPositionsBuffer[1],
                                                                           &destPositionsBuffer[2]);
                        ++pDepth;
                        destPositionsBuffer += 3;
                    }
                }
            }
        }
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

        // Push buffer and notify
        m_depthTimeline->pushObject(depthBuffer);
        auto sig = m_depthTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);

        m_colorTimeline->pushObject(colorBuffer);
        sig = m_colorTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);

        if (m_positionsTimeline)
        {
            m_positionsTimeline->pushObject(positionsBuffer);
            sig = m_positionsTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
            sig->asyncEmit(timestamp);
        }
    }
}

// -----------------------------------------------------------------------------

} // namespace videoOpenni


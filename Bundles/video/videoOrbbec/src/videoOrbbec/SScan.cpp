/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "videoOrbbec/SScan.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <memory>
#include <thread>

namespace videoOrbbec
{

//------------------------------------------------------------------------------

SScan::SScan() noexcept :
    SScanBase(),
    m_depthTL(),
    m_colorTL(),
    m_rgbCamera(nullptr),
    m_oniStatus(),
    m_depthStream(),
    m_workerDepth(::fwThread::Worker::New()),
    m_pause(false),
    m_slotPresentDepthFrame(::fwCom::newSlot( &SScan::presentDepthFrame, this ))
{
    // Create a worker to fill the timelines
    m_slotPresentDepthFrame->setWorker(m_workerDepth);
}

//------------------------------------------------------------------------------

SScan::~SScan() noexcept
{
    this->stopCamera();
}

//------------------------------------------------------------------------------

// To start the service
void SScan::starting()
{
    m_colorTL = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
    m_depthTL = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
}

//------------------------------------------------------------------------------

void SScan::startCamera()
{
    try
    {
        this->stopCamera();

        if(!isReady())
        {
            tryInitialize();
        }

        // Initialize OpenNI and OpenCV video streams
        m_oniStatus = m_depthStream.create(*m_oniDevice, ::openni::SENSOR_DEPTH);
        FW_RAISE_IF("Cannot create depth stream", m_oniStatus != ::openni::STATUS_OK);
        m_oniStatus = m_depthStream.start();
        FW_RAISE_IF("Cannot open depth stream", m_oniStatus != ::openni::STATUS_OK);
        m_depthStream.setMirroringEnabled(false);

        // Initialize timelines
        const auto depthWidth  = m_depthStream.getVideoMode().getResolutionX(),
                   depthHeight = m_depthStream.getVideoMode().getResolutionY();

        const auto colorWidth  = m_rgbCamera->getWidth(),
                   colorHeight = m_rgbCamera->getHeight();

        if (m_depthTL)
        {
            m_depthTL->initPoolSize(static_cast<size_t>(depthWidth), static_cast<size_t>(depthHeight),
                                    fwTools::Type::s_UINT16, 1);
            m_depthTL->setMaximumSize(50);
        }

        m_colorTL->initPoolSize(colorWidth, colorHeight, fwTools::Type::s_UINT8, 4);
        m_colorTL->setMaximumSize(50);

#ifdef __linux__
        // open device with id
        m_rgbGrabber.open(m_rgbCamera->getCameraID());
#else
        // open device with index
        m_rgbGrabber.open(m_rgbCamera->getIndex());
#endif
        if(!m_rgbGrabber.isOpened())
        {
            throw std::runtime_error("cannot open camera: " + m_rgbCamera->getCameraID() + ".");
        }
        else
        {
            m_rgbGrabber.set(::cv::CAP_PROP_FPS, 30);
            m_rgbGrabber.set(::cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(colorWidth));
            m_rgbGrabber.set(::cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(colorHeight));

        }

        m_isGrabbing = true;
        m_slotPresentDepthFrame->asyncRun();

        auto sig = this->signal< ::arServices::IGrabber::CameraStartedSignalType>(
            ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
        sig->asyncEmit();
    }
    catch(std::exception const& e)
    {
        errorPopup(std::string("Cannot start camera: ") + e.what());
        throw;
    }
}

//------------------------------------------------------------------------------

void SScan::tryInitialize()
{
    try
    {
        auto cameraInput = this->getInput< ::fwData::Object >(s_CAMERA_INPUT);
        auto camera      = ::arData::Camera::dynamicConstCast(cameraInput);

        if(camera)
        {
            m_rgbCamera = camera;
        }
        else
        {
            auto cameraSeries = ::arData::CameraSeries::dynamicConstCast(cameraInput);
            if(cameraSeries)
            {
                size_t numCamerasInSeries = cameraSeries->getNumberOfCameras();
                SLM_ASSERT("Camera Series is empty", numCamerasInSeries);

                // Assume same source on all cameras
                m_rgbCamera = cameraSeries->getCamera(0);
            }
        }

        FW_RAISE_IF("OpenNI initialization failed: " << ::openni::OpenNI::getExtendedError(),
                    ::openni::OpenNI::initialize() != ::openni::STATUS_OK);
        // Find and initialize the OpenNI device
        detectCameraOpenni();
        FW_RAISE_IF("Astra Pro camera not detected", !m_oniDevice || !m_oniDevice->isValid());

    }
    catch(std::exception const& e)
    {
        errorPopup(std::string("SScan initialization error: ") + e.what());
        throw;
    }
}

//------------------------------------------------------------------------------

bool SScan::isReady() const noexcept
{
    return m_oniDevice && m_rgbCamera;
}

//------------------------------------------------------------------------------

void SScan::stopCamera()
{
    // It's important to lock m_oniDevice because ::openni::OpenNI::waitForAnyStream
    // (called in presentDepthFrame()) crashes if it's called when m_oniDevice is null.
    m_grabbingMutex.lock();
    {
        m_depthStream.stop();
        m_rgbGrabber.release();

        m_oniDevice = nullptr;

        m_pause      = false;
        m_isGrabbing = false;
    }
    m_grabbingMutex.unlock();

    this->signal< ::arServices::IGrabber::CameraStoppedSignalType>(::arServices::IGrabber::s_CAMERA_STOPPED_SIG)
    ->asyncEmit();
}

// -----------------------------------------------------------------------------

void SScan::pauseCamera()
{
    m_grabbingMutex.lock();
    {
        m_pause = !m_pause;
    }
    m_grabbingMutex.unlock();
}

//------------------------------------------------------------------------------

void SScan::presentDepthFrame()
{
    ::openni::VideoFrameRef depthFrame;
    int index;
    auto streamPtr = &m_depthStream;
    bool run       = true;

    // grab depth & rgb
    while(run)
    {
        // Always lock before the conditional section.
        // Unlock as soon as possible in order to avoid deadlocks.
        m_grabbingMutex.lock();
        if(m_rgbGrabber.isOpened() &&
           ::openni::OpenNI::waitForAnyStream(&streamPtr, 1, &index, 1000) == ::openni::STATUS_OK)
        {
            run = m_isGrabbing;
            if(m_pause)
            {
                m_grabbingMutex.unlock();
                continue;
            }

            bool isGrabbed = m_rgbGrabber.grab();
            if (m_depthTL)
            {
                m_depthStream.readFrame(&depthFrame);
            }

            auto timestamp = std::chrono::duration_cast< std::chrono::milliseconds >
                                 (std::chrono::system_clock::now().time_since_epoch()).count();

            if(depthFrame.isValid())
            {
                auto depthBuffer    = m_depthTL->createBuffer(timestamp);
                auto depthBufferPtr = reinterpret_cast<uint16_t*>(depthBuffer->addElement(0));
                auto depthPixels    = reinterpret_cast<const uint16_t*>(depthFrame.getData());
                memcpy(depthBufferPtr, depthPixels, static_cast<size_t>(depthFrame.getDataSize()));
                m_depthTL->pushObject(depthBuffer);

                m_depthTL->signal< ::arData::TimeLine::ObjectPushedSignalType>(::arData::TimeLine::s_OBJECT_PUSHED_SIG)
                ->asyncEmit(timestamp);

            }

            if (isGrabbed)
            {
                ::cv::Mat image;
                m_rgbGrabber.retrieve(image);
                m_grabbingMutex.unlock();

                // Get the buffer of the timeline to fill
                SPTR(::arData::FrameTL::BufferType) bufferOut = m_colorTL->createBuffer(timestamp);
                std::uint8_t* frameBuffOut = bufferOut->addElement(0);

                // Create an OpenCV mat that aliases the buffer created from the output timeline.
                ::cv::Mat imgOut(image.size(), CV_8UC4, (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

                ::cv::cvtColor(image, imgOut, ::cv::COLOR_BGR2RGBA);

                m_colorTL->pushObject(bufferOut);

                auto sig =
                    m_colorTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                        ::arData::TimeLine::s_OBJECT_PUSHED_SIG);
                sig->asyncEmit(timestamp);

            }
            else
            {
                m_grabbingMutex.unlock();
            }

            if(depthFrame.isValid() && isGrabbed)
            {
                this->signal< ::arServices::IGrabber::FramePresentedSignalType>(
                    ::arServices::IGrabber::s_FRAME_PRESENTED_SIG)
                ->asyncEmit();
            }
        }
        else
        {
            run = false;
            m_grabbingMutex.unlock();
        }
    }
}

//------------------------------------------------------------------------------

} // videoOrbbec

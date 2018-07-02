/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOrbbec/SScanIR.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwVideoQt/helper/formats.hpp>
#include <fwVideoQt/Registry.hpp>

#include <QCameraInfo>

#include <memory>

namespace videoOrbbec
{

const std::string s_QT_CAMERA_STRING = "Astra Pro HD Camera";

const ::fwServices::IService::KeyType SScanIR::s_IRTL_INOUT = "irTL";

//------------------------------------------------------------------------------

SScanIR::SScanIR() noexcept :
    QObject(),
    SScanBase(),
    m_depthTL(),
    m_colorTL(),
    m_rgbCamera(::arData::Camera::New()),
    m_oniStatus(),
    m_depthStream(),
    m_irStream(),
    m_workerIR(::fwThread::Worker::New()),
    m_workerDepth(::fwThread::Worker::New()),
    m_colorWorkerThread(),
    m_qtPlayer(nullptr),
    m_workerColor(new ColorFrameWorker(*this)),
    m_slotPresentDepthFrame(::fwCom::newSlot( &SScanIR::presentDepthFrame, this )),
    m_slotPresentIRFrame(::fwCom::newSlot( &SScanIR::presentIRFrame, this ))
{
    // Create a worker to fill the timelines
    m_slotPresentDepthFrame->setWorker(m_workerDepth);
    m_slotPresentIRFrame->setWorker(m_workerIR);
    qInstallMessageHandler(SScanBase::qtMessageHandler);
}

//------------------------------------------------------------------------------

SScanIR::~SScanIR() noexcept
{
    delete m_qtPlayer;
    delete m_workerColor;
}

//------------------------------------------------------------------------------

// To start the service
void SScanIR::starting()
{
    m_colorTL = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
    m_depthTL = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
    m_IRTL    = this->getInOut< ::arData::FrameTL>(s_IRTL_INOUT);

    m_workerColor->init(m_colorTL);
    // Qt signal/slot connections
    FW_RAISE_IF("Cannot connect QThread::started to SScanIR::workerStarted.",
                !QObject::connect(&m_colorWorkerThread, SIGNAL(started()), this, SLOT(workerStarted())));
    FW_RAISE_IF("Cannot connect QThread::finished to SScanIR::workerStopped.",
                !QObject::connect(&m_colorWorkerThread, SIGNAL(finished()), this, SLOT(workerStopped())));
}

//------------------------------------------------------------------------------

void SScanIR::startCamera()
{
    if(this->getWorker() != ::fwServices::registry::ActiveWorkers::getDefaultWorker())
    {
        const auto errMsg = "Astra grabbers currently don't support being run on a worker other than the main thread.";
        errorPopup(errMsg);
        FW_RAISE(errMsg);
    }
    try
    {
        this->stopCamera();

        if(!isReady())
        {
            tryInitialize();
        }

        // Initialize OpenNI and Qt video streams
        m_oniStatus = m_depthStream.create(*m_oniDevice, ::openni::SENSOR_DEPTH);
        FW_RAISE_IF("Cannot create depth stream", m_oniStatus != ::openni::STATUS_OK);
        m_oniStatus = m_depthStream.start();
        FW_RAISE_IF("Cannot open depth stream", m_oniStatus != ::openni::STATUS_OK);

        m_oniStatus = m_irStream.create(*m_oniDevice, ::openni::SENSOR_IR);
        FW_RAISE_IF("Cannot create IR stream", m_oniStatus != ::openni::STATUS_OK);

        auto sensorInfo        = m_oniDevice->getSensorInfo(::openni::SENSOR_IR);
        auto const& videoModes = sensorInfo->getSupportedVideoModes();
        for(int i = 0; i != videoModes.getSize(); ++i)
        {
            auto mode = videoModes[i];
            OSLM_DEBUG("Video mode: (" << mode.getResolutionX() << ", " << mode.getResolutionY() << ")"
                                       << "@" << mode.getFps() << " format: " << mode.getPixelFormat());
        }
        m_irStream.setVideoMode(videoModes[4]);

        m_oniStatus = m_irStream.start();
        FW_RAISE_IF("Cannot open IR stream", m_oniStatus != ::openni::STATUS_OK);

        // Frames are mirrored horizontally on linux
        m_depthStream.setMirroringEnabled(false);
        m_irStream.setMirroringEnabled(false);

        m_qtPlayer = ::fwVideoQt::Registry::getInstance().requestPlayer(m_rgbCamera);

        // Initialize timelines
        const auto irWidth     = m_irStream.getVideoMode().getResolutionX(),
                   irHeight    = m_irStream.getVideoMode().getResolutionY();
        const auto depthWidth  = m_depthStream.getVideoMode().getResolutionX(),
                   depthHeight = m_depthStream.getVideoMode().getResolutionY();
        const auto colorWidth  = m_rgbCamera->getWidth(),
                   colorHeight = m_rgbCamera->getHeight();
        m_depthTL->initPoolSize(depthWidth, depthHeight, fwTools::Type::s_UINT16, 1);
        m_depthTL->setMaximumSize(50);
        m_IRTL->initPoolSize(irWidth, irHeight, fwTools::Type::s_UINT16, 1);
        m_IRTL->setMaximumSize(50);
        m_colorTL->initPoolSize(colorWidth, colorHeight, fwTools::Type::s_UINT8, 4);
        m_colorTL->setMaximumSize(50);

        // Start workers
        m_workerColor->moveToThread(&m_colorWorkerThread);
        FW_RAISE_IF("Cannot connect videoQt::Player::frameAvailable to worker.",
                    !QObject::connect(m_qtPlayer, SIGNAL(frameAvailable(QVideoFrame const&)),
                                      m_workerColor, SLOT(presentFrame(QVideoFrame const&))));
        m_qtPlayer->play();
        m_colorWorkerThread.start();

        m_slotPresentDepthFrame->asyncRun();
        m_slotPresentIRFrame->asyncRun();

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

void SScanIR::tryInitialize()
{
    try
    {
        FW_RAISE_IF("OpenNI initialization failed: " << ::openni::OpenNI::getExtendedError(),
                    ::openni::OpenNI::initialize() != ::openni::STATUS_OK);
        // Find and initialize the OpenNI device
        detectCameraOpenni();
        FW_RAISE_IF("Astra Pro camera not detected", !m_oniDevice || !m_oniDevice->isValid());
        const auto astraDeviceInfo = m_oniDevice->getDeviceInfo();

        // Then, find the Qt color camera corresponding the the Astra Pro.
        m_rgbCamera = detectCameraQt();

    }
    catch(std::exception const& e)
    {
        errorPopup(std::string("SScan initialization error: ") + e.what());
        throw;
    }
}

//------------------------------------------------------------------------------

bool SScanIR::isReady() const noexcept
{
    return m_oniDevice && m_rgbCamera;
}
//
//------------------------------------------------------------------------------

void SScanIR::stopCamera()
{
    m_depthStream.stop();
    m_irStream.stop();
    if(m_qtPlayer)
    {
        FW_RAISE_IF("Cannot disconnect player from worker.",
                    !QObject::disconnect(m_qtPlayer, SIGNAL(frameAvailable(QVideoFrame)),
                                         m_workerColor, SLOT(presentFrame(QVideoFrame))));
        m_qtPlayer->stop();
        ::fwVideoQt::Registry::getInstance().releasePlayer(m_qtPlayer);
        m_qtPlayer = nullptr;
        m_colorWorkerThread.quit();
    }
    this->signal< ::arServices::IGrabber::CameraStoppedSignalType>(::arServices::IGrabber::s_CAMERA_STOPPED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SScanIR::presentIRFrame()
{
    ::openni::VideoFrameRef irFrame;
    int index;
    auto streamPtr = &m_irStream;
    while(::openni::OpenNI::waitForAnyStream(&streamPtr, 1, &index, 1000) == ::openni::STATUS_OK)
    {
        m_irStream.readFrame(&irFrame);
        if(irFrame.isValid())
        {
            auto timestamp   = ::fwCore::HiResClock::getTimeInMilliSec();
            auto irBuffer    = m_IRTL->createBuffer(timestamp);
            auto irBufferPtr = reinterpret_cast<uint16_t*>(irBuffer->addElement(0));
            auto irPixels    = reinterpret_cast<const uint16_t*>(irFrame.getData());
            memcpy(irBufferPtr, irPixels, irFrame.getDataSize());
            m_IRTL->pushObject(irBuffer);
            m_IRTL->signal< ::arData::TimeLine::ObjectPushedSignalType>(::arData::TimeLine::s_OBJECT_PUSHED_SIG)
            ->asyncEmit(timestamp);
            this->signal< ::arServices::IGrabber::FramePresentedSignalType>(
                ::arServices::IGrabber::s_FRAME_PRESENTED_SIG)
            ->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------
void SScanIR::presentDepthFrame()
{
    ::openni::VideoFrameRef depthFrame;
    int index;
    auto streamPtr = &m_depthStream;
    while(::openni::OpenNI::waitForAnyStream(&streamPtr, 1, &index, 1000) == ::openni::STATUS_OK)
    {
        m_depthStream.readFrame(&depthFrame);
        if(depthFrame.isValid())
        {
            auto timestamp      = ::fwCore::HiResClock::getTimeInMilliSec();
            auto depthBuffer    = m_depthTL->createBuffer(timestamp);
            auto depthBufferPtr = reinterpret_cast<uint16_t*>(depthBuffer->addElement(0));
            auto depthPixels    = reinterpret_cast<const uint16_t*>(depthFrame.getData());
            memcpy(depthBufferPtr, depthPixels, depthFrame.getDataSize());
            m_depthTL->pushObject(depthBuffer);
            m_depthTL->signal< ::arData::TimeLine::ObjectPushedSignalType>(::arData::TimeLine::s_OBJECT_PUSHED_SIG)
            ->asyncEmit(timestamp);
            this->signal< ::arServices::IGrabber::FramePresentedSignalType>(
                ::arServices::IGrabber::s_FRAME_PRESENTED_SIG)
            ->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

void SScanIR::workerStarted() const
{
    SLM_INFO("Color worker thread started.");
}

//------------------------------------------------------------------------------

void SScanIR::workerStopped() const
{
    SLM_INFO("Color worker thread stopped.");
}

} // namespace videoOrbbec

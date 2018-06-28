/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOrbbec/SScan.hpp"

#include "videoOrbbec/ColorFrameWorker.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <videoQt/helper/formats.hpp>
#include <videoQt/player/VideoRegistry.hpp>

#include <QCameraInfo>

#include <memory>
#include <thread>

namespace videoOrbbec
{

//------------------------------------------------------------------------------

SScan::SScan() noexcept :
    QObject(),
    SScanBase(),
    m_depthTL(),
    m_colorTL(),
    m_rgbCamera(nullptr),
    m_oniStatus(),
    m_depthStream(),
    m_workerDepth(::fwThread::Worker::New()),
    m_colorWorkerThread(),
    m_qtPlayer(nullptr),
    m_workerColor(new ColorFrameWorker(*this)),
    m_pause(false),
    m_slotPresentDepthFrame(::fwCom::newSlot( &SScan::presentDepthFrame, this ))
{
    // Create a worker to fill the timelines
    m_slotPresentDepthFrame->setWorker(m_workerDepth);
    qInstallMessageHandler(SScanBase::qtMessageHandler);
}

//------------------------------------------------------------------------------

SScan::~SScan() noexcept
{
    delete m_qtPlayer;
    delete m_workerColor;
}

//------------------------------------------------------------------------------

// To start the service
void SScan::starting()
{
    m_colorTL = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
    m_depthTL = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
    m_workerColor->init(m_colorTL);

    // Qt signal/slot connections
    FW_RAISE_IF("Cannot connect QThread::started to SScan::workerStarted.",
                !QObject::connect(&m_colorWorkerThread, SIGNAL(started()), this, SLOT(workerStarted())));
    FW_RAISE_IF("Cannot connect QThread::finished to SScan::workerStopped.",
                !QObject::connect(&m_colorWorkerThread, SIGNAL(finished()), this, SLOT(workerStopped())));
}

//------------------------------------------------------------------------------

void SScan::startCamera()
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
        m_depthStream.setMirroringEnabled(false);

        m_qtPlayer = ::videoQt::player::VideoRegistry::getInstance().requestPlayer(m_rgbCamera);

        // Initialize timelines
        const auto depthWidth  = m_depthStream.getVideoMode().getResolutionX(),
                   depthHeight = m_depthStream.getVideoMode().getResolutionY();
        const auto colorWidth  = m_rgbCamera->getWidth(),
                   colorHeight = m_rgbCamera->getHeight();
        m_depthTL->initPoolSize(depthWidth, depthHeight, fwTools::Type::s_UINT16, 1);
        m_depthTL->setMaximumSize(50);
        m_colorTL->initPoolSize(colorWidth, colorHeight, fwTools::Type::s_UINT8, 4);
        m_colorTL->setMaximumSize(50);

        // Start workers
        m_workerColor->moveToThread(&m_colorWorkerThread);
        FW_RAISE_IF("Cannot connect QVideoPlayer::frameAvailable to worker.",
                    !QObject::connect(m_qtPlayer, SIGNAL(frameAvailable(QVideoFrame const&)),
                                      m_workerColor, SLOT(presentFrame(QVideoFrame const&))));
        m_qtPlayer->play();
        m_colorWorkerThread.start();

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
        FW_RAISE_IF("OpenNI initialization failed: " << ::openni::OpenNI::getExtendedError(),
                    ::openni::OpenNI::initialize() != ::openni::STATUS_OK);
        // Find and initialize the OpenNI device
        detectCameraOpenni();
        FW_RAISE_IF("Astra Pro camera not detected", !m_oniDevice || !m_oniDevice->isValid());

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

bool SScan::isReady() const noexcept
{
    return m_oniDevice && m_rgbCamera;
}

//------------------------------------------------------------------------------

void SScan::stopCamera()
{
    m_depthStream.stop();
    if(m_qtPlayer)
    {
        FW_RAISE_IF("Cannot disconnect player from worker.",
                    !QObject::disconnect(m_qtPlayer, SIGNAL(frameAvailable(QVideoFrame)),
                                         m_workerColor, SLOT(presentFrame(QVideoFrame))));
        m_qtPlayer->stop();
        ::videoQt::player::VideoRegistry::getInstance().releasePlayer(m_qtPlayer);
        m_qtPlayer = nullptr;
        m_colorWorkerThread.quit();
    }
    this->signal< ::arServices::IGrabber::CameraStoppedSignalType>(::arServices::IGrabber::s_CAMERA_STOPPED_SIG)
    ->asyncEmit();

    m_pause = false;
    m_workerColor->setPause(false);
}

// -----------------------------------------------------------------------------

void SScan::pauseCamera()
{
    m_pause = !m_pause;
    m_workerColor->setPause(m_pause);
}

//------------------------------------------------------------------------------

void SScan::presentDepthFrame()
{
    ::openni::VideoFrameRef depthFrame;
    int index;
    auto streamPtr = &m_depthStream;
    while(::openni::OpenNI::waitForAnyStream(&streamPtr, 1, &index, 1000) == ::openni::STATUS_OK)
    {
        if(m_pause)
        {
            continue;
        }
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

void SScan::workerStarted() const
{
    SLM_INFO("Color worker thread started.");
}

//------------------------------------------------------------------------------

void SScan::workerStopped() const
{
    SLM_INFO("Color worker thread stopped.");
}

} // videoOrbbec

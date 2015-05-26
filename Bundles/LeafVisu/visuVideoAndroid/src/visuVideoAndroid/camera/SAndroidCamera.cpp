/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVideoAndroid/camera/SAndroidCamera.hpp"

#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hxx>

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <sstream>

namespace visuVideoAndroid
{
namespace camera
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::visuVideoAndroid::camera::SAndroidCamera, ::extData::FrameTL );

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SAndroidCamera::s_FRAME_FETCHED_SIG      = "frameFetched";
const ::fwCom::Signals::SignalKeyType SAndroidCamera::s_GRAY_FRAME_FETCHED_SIG = "grayFrameFetched";
const ::fwCom::Signals::SignalKeyType SAndroidCamera::s_CAMERA_OPENED_SIG      = "cameraOpened";

const ::fwCom::Slots::SlotKeyType SAndroidCamera::s_OPEN_CAMERA_SLOT = "openCamera";

//-----------------------------------------------------------------------------

SAndroidCamera::SAndroidCamera() throw()
{
    SLM_TRACE_FUNC();

    m_sigFrameFetched = FrameFetchedSignalType::New();
    ::fwCom::HasSignals::m_signals(s_FRAME_FETCHED_SIG, m_sigFrameFetched);

    m_sigGrayFrameFetched = GrayFrameFetchedSignalType::New();
    ::fwCom::HasSignals::m_signals(s_GRAY_FRAME_FETCHED_SIG, m_sigGrayFrameFetched);

    m_sigCameraOpened = CameraOpenedSignalType::New();
    ::fwCom::HasSignals::m_signals(s_CAMERA_OPENED_SIG, m_sigCameraOpened);

    m_slotOpenCamera = ::fwCom::newSlot( &SAndroidCamera::openCamera, this );
    ::fwCom::HasSlots::m_slots( s_OPEN_CAMERA_SLOT, m_slotOpenCamera);


    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    m_cameraId  = 0;
    m_autoFocus = true;
    m_width     = 320;
    m_height    = 240;
}

//-----------------------------------------------------------------------------

SAndroidCamera::~SAndroidCamera() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SAndroidCamera::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr cfg;
    cfg = m_configuration->findConfigurationElement("cameraId");
    std::istringstream(cfg->getValue()) >> m_cameraId;

    cfg = m_configuration->findConfigurationElement("autoFocus");
    std::istringstream(cfg->getValue()) >> std::boolalpha >> m_autoFocus;

    cfg = m_configuration->findConfigurationElement("width");
    std::istringstream(cfg->getValue()) >> m_width;

    cfg = m_configuration->findConfigurationElement("height");
    std::istringstream(cfg->getValue()) >> m_height;

    cfg = m_configuration->findConfigurationElement("fps");
    std::istringstream(cfg->getValue()) >> m_frameRate;

    m_timeline = this->getObject< ::extData::FrameTL >();
}

//-----------------------------------------------------------------------------

void SAndroidCamera::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_camera = new ::arAndroidTools::Camera();
    m_camera->m_sigFrameFetched.connect(bind(&SAndroidCamera::fetchFrame, this, _1));
    m_camera->setWidth(m_width);
    m_camera->setHeight(m_height);
    m_camera->setFrameRate(m_frameRate);
    m_camera->setAutoFocus(m_autoFocus);

}

//-----------------------------------------------------------------------------

void SAndroidCamera::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SAndroidCamera::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SAndroidCamera::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->openCamera();
}

//-----------------------------------------------------------------------------

void SAndroidCamera::openCamera()
{
    SLM_TRACE_FUNC();

    m_camera->open( m_cameraId);

    m_sigCameraOpened->asyncEmit( m_camera->getFormat(), m_camera->getWidth(), m_camera->getHeight() );

    const size_t width  = static_cast<size_t>(m_camera->getWidth());
    const size_t height = static_cast<size_t>(m_camera->getHeight());
    m_timeline->setMaximumSize(10);
    m_timeline->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 4);
}

//-----------------------------------------------------------------------------

void SAndroidCamera::fetchFrame(unsigned char* rgb)
{
    // Recopy of frame grabber ...
    SLM_TRACE_FUNC();
    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

    SPTR(::extData::FrameTL::BufferType) buffer = m_timeline->createBuffer(timestamp);
//    ::boost::uint64_t* destBuffer               = reinterpret_cast< ::boost::uint64_t* >( buffer->addElement(0) );

    ::boost::uint32_t* destBuffer = reinterpret_cast< ::boost::uint32_t* >( buffer->addElement(0) );

    const int width  = m_camera->getWidth();
    const int height = m_camera->getHeight();

    OSLM_DEBUG(" Fetched frame size = "<<m_camera->getWidth()<<" x "<< m_camera->getHeight());
    OSLM_DEBUG(" Fetched frame rate = "<<m_camera->getFrameRate());

    const ::boost::uint32_t* frameBuffer = reinterpret_cast< const ::boost::uint32_t *>( rgb );
    const unsigned int size              = static_cast<unsigned int>(width*height);

    std::copy(frameBuffer,frameBuffer+size,destBuffer);

    delete[] rgb;

    //push buffer
    m_timeline->pushObject(buffer);

    //Notify
    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = m_timeline->signal< ::extData::TimeLine::ObjectPushedSignalType >(::extData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);

}

//-----------------------------------------------------------------------------

}
}


/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/SFrameRecorder.hpp"

#include <fwCom/Slots.hxx>
#include <fwComEd/helper/Vector.hpp>
#include <fwComEd/InteractionMsg.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IController.hpp>

#include <boost/filesystem.hpp>

#include <QImage>
#include <QString>

namespace videoQt
{

fwServicesRegisterMacro( ::fwServices::IController, ::videoQt::SFrameRecorder, ::extData::FrameTL);

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_SAVE_FRAME_SLOT   = "saveFrame";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_START_RECORD_SLOT = "startRecord";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_STOP_RECORD_SLOT  = "stopRecord";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_PAUSE_RECORD_SLOT = "pauseRecord";

SFrameRecorder::SFrameRecorder() throw() : m_count(0),
                                           m_isRecording(false),
                                           m_isPaused(false)
{
    m_slotSaveFrame   = ::fwCom::newSlot( &SFrameRecorder::saveFrame, this);
    m_slotStartRecord = ::fwCom::newSlot( &SFrameRecorder::startRecord, this);
    m_slotStopRecord  = ::fwCom::newSlot( &SFrameRecorder::stopRecord, this);
    m_slotPauseRecord = ::fwCom::newSlot( &SFrameRecorder::pauseRecord, this);

    ::fwCom::HasSlots::m_slots(s_SAVE_FRAME_SLOT, m_slotSaveFrame)
        (s_START_RECORD_SLOT, m_slotStartRecord)
        (s_STOP_RECORD_SLOT, m_slotStopRecord)
        (s_PAUSE_RECORD_SLOT, m_slotPauseRecord);

#ifdef COM_LOG
    ::fwCom::HasSlots::m_slots.setID();
#endif

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SFrameRecorder::~SFrameRecorder() throw()
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::starting() throw(::fwTools::Failed)
{
    ::boost::filesystem::create_directories(m_path);
    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//-----------------------------------------------------------------------------

void SFrameRecorder::stopping() throw(::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::configuring()  throw ( ::fwTools::Failed )
{
    ConfigurationType config = m_configuration->findConfigurationElement("path");
    SLM_ASSERT("Missing <path> tag", config);

    m_path = config->getValue();
}

//-----------------------------------------------------------------------------

void SFrameRecorder::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::saveFrame(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording && !m_isPaused)
    {
        ::extData::FrameTL::sptr frameTL = this->getObject< ::extData::FrameTL >();

        CSPTR(::extData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
        OSLM_WARN_IF("No frame found in timeline for timestamp : " << timestamp, !buffer);

        if(buffer)
        {
            int width  = static_cast<int>(frameTL->getWidth());
            int height = static_cast<int>(frameTL->getHeight());
            QImage image(width, height, QImage::Format_ARGB32);

            ::boost::uint64_t* imageBuffer = reinterpret_cast< ::boost::uint64_t *>( image.bits() );
            const ::boost::uint64_t* frameBuffer =
                reinterpret_cast< const ::boost::uint64_t *>( &buffer->getElement(0) );

            const unsigned int size = static_cast<unsigned int>(width * height) >> 1;

            for(unsigned int idx = 0; idx < size; ++idx)
            {
                const ::boost::uint64_t pixel = *frameBuffer++;

                *imageBuffer++ = (pixel & 0xFF00FF00FF00FF00) | (pixel & 0x000000FF000000FF) << 16
                                 | (pixel & 0x00FF000000FF0000) >> 16;
            }

            std::stringstream str;
            str.width(8);
            str.fill('0');
            str << ++m_count;
            std::string filename(m_path.string() + "/" + "img_" + str.str() + ".bmp");
            image.save(QString::fromStdString(filename));
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SFrameRecorder::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::extData::FrameTL::s_OBJECT_PUSHED_SIG, s_SAVE_FRAME_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

void SFrameRecorder::startRecord()
{
    m_connections->disconnect();
    m_connections->connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
    m_isRecording = true;
    m_isPaused    = false;
}

//------------------------------------------------------------------------------

void SFrameRecorder::stopRecord()
{
    m_connections->disconnect();
    m_isRecording = false;
}
//------------------------------------------------------------------------------

void SFrameRecorder::pauseRecord()
{
    if (m_isRecording && !m_isPaused)
    {
        m_connections->disconnect();
        m_isPaused = true;
    }
    else if (m_isRecording && m_isPaused)
    {
        m_connections->connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
        m_isPaused = false;
    }
}

//------------------------------------------------------------------------------

} // namespace videoQt

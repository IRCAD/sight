/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/SFrameRecorder.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwDataTools/helper/Vector.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/macros.hpp>

#include <QImage>
#include <QString>

#include <boost/filesystem.hpp>

namespace videoQt
{

fwServicesRegisterMacro( ::fwServices::IController, ::videoQt::SFrameRecorder, ::arData::FrameTL);

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_SAVE_FRAME_SLOT   = "saveFrame";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_START_RECORD_SLOT = "startRecord";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_STOP_RECORD_SLOT  = "stopRecord";
const ::fwCom::Slots::SlotKeyType SFrameRecorder::s_PAUSE_RECORD_SLOT = "pauseRecord";

static const ::fwServices::IService::KeyType s_FRAMETL_INPUT = "frameTL";

//-----------------------------------------------------------------------------

SFrameRecorder::SFrameRecorder() noexcept : m_count(0),
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



    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SFrameRecorder::~SFrameRecorder() noexcept
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::starting()
{
    ::boost::filesystem::create_directories(m_path);
}

//-----------------------------------------------------------------------------

void SFrameRecorder::stopping()
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::configuring()
{
    ConfigurationType config = m_configuration->findConfigurationElement("path");
    SLM_ASSERT("Missing <path> tag", config);

    m_path = config->getValue();
}

//-----------------------------------------------------------------------------

void SFrameRecorder::updating()
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::saveFrame(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording && !m_isPaused)
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(s_FRAMETL_INPUT);

        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
        OSLM_WARN_IF("No frame found in timeline for timestamp : " << timestamp, !buffer);

        if(buffer)
        {
            int width  = static_cast<int>(frameTL->getWidth());
            int height = static_cast<int>(frameTL->getHeight());
            QImage image(width, height, QImage::Format_ARGB32);

            ::boost::uint64_t* imageBuffer = reinterpret_cast< ::boost::uint64_t*>( image.bits() );
            const ::boost::uint64_t* frameBuffer =
                reinterpret_cast< const ::boost::uint64_t*>( &buffer->getElement(0) );

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

void SFrameRecorder::startRecord()
{
    ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(s_FRAMETL_INPUT);

    if (frameTL->getType() != ::fwTools::Type::s_UINT8 || frameTL->getNumberOfComponents() != 4)
    {
        SLM_ERROR("Frame type not managed. Only image with type 'uint8' with 4 components are managed.");
        return;
    }

    m_connections.disconnect();
    m_connections.connect(frameTL, ::arData::FrameTL::s_OBJECT_PUSHED_SIG,
                          this->getSptr(), s_SAVE_FRAME_SLOT);
    m_isRecording = true;
    m_isPaused    = false;
}

//------------------------------------------------------------------------------

void SFrameRecorder::stopRecord()
{
    m_connections.disconnect();
    m_isRecording = false;
}
//------------------------------------------------------------------------------

void SFrameRecorder::pauseRecord()
{
    if (m_isRecording && !m_isPaused)
    {
        m_connections.disconnect();
        m_isPaused = true;
    }
    else if (m_isRecording && m_isPaused)
    {
        m_connections.connect(this->getInput< ::arData::FrameTL >(s_FRAMETL_INPUT),
                              ::arData::FrameTL::s_OBJECT_PUSHED_SIG,
                              this->getSptr(), s_SAVE_FRAME_SLOT);

        m_isPaused = false;
    }
}

//------------------------------------------------------------------------------

} // namespace videoQt

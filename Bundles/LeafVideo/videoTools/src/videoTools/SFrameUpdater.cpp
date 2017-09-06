/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SFrameUpdater.hpp"

#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Object.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Worker.hpp>

fwServicesRegisterMacro( ::fwServices::IController, ::videoTools::SFrameUpdater, ::fwData::Composite );

namespace videoTools
{
//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SFrameUpdater::s_UPDATE_FRAME_SLOT        = "updateFrame";
const ::fwCom::Signals::SignalKeyType SFrameUpdater::s_RENDER_REQUESTED_SIG = "renderRequested";

SFrameUpdater::SFrameUpdater() noexcept :
    m_lastTimestamp(0),
    m_lastMatrixUpdatedKey(""),
    m_imageInitialized(false)
{
    m_slotUpdateFrame = ::fwCom::newSlot(&SFrameUpdater::updateFrame, this);
    ::fwCom::HasSlots::m_slots(s_UPDATE_FRAME_SLOT, m_slotUpdateFrame);

    m_sigRenderRequested = RenderRequestedSignalType::New();
    m_signals( s_RENDER_REQUESTED_SIG,  m_sigRenderRequested);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//-----------------------------------------------------------------------------

SFrameUpdater::~SFrameUpdater() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrameUpdater::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( "frameTL", ::arData::TimeLine::s_OBJECT_PUSHED_SIG,
                      ::videoTools::SFrameUpdater::s_UPDATE_FRAME_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SFrameUpdater::starting()
{
    m_imageInitialized = false;

    m_frameTL = this->getInput< ::arData::FrameTL>("frameTL");
    m_image   = this->getInOut< ::fwData::Image>("frame");
}

//-----------------------------------------------------------------------------

void SFrameUpdater::configuring()
{
}

//-----------------------------------------------------------------------------

void SFrameUpdater::stopping()
{
    m_connections.disconnect();
}

//-----------------------------------------------------------------------------

void SFrameUpdater::updating()
{
}

//-----------------------------------------------------------------------------

void SFrameUpdater::updateFrame( ::fwCore::HiResClock::HiResClockType timestamp )
{
    if (timestamp > m_lastTimestamp)
    {
        ::fwData::Image::SizeType size(3);
        size[0] = m_frameTL->getWidth();
        size[1] = m_frameTL->getHeight();
        size[2] = 1;

        // Check if image dimensions has changed
        if(size != m_image->getSize())
        {
            m_imageInitialized = false;
        }

        if(!m_imageInitialized)
        {
            const ::fwData::Image::SpacingType::value_type voxelSize = 1;
            m_image->allocate(size, m_frameTL->getType(), m_frameTL->getNumberOfComponents());
            ::fwData::Image::OriginType origin(3, 0);

            m_image->setOrigin(origin);
            ::fwData::Image::SpacingType spacing(3, voxelSize);
            m_image->setSpacing(spacing);
            m_image->setWindowWidth(1);
            m_image->setWindowCenter(0);
            m_imageInitialized = true;

            //Notify (needed for instance to update the texture in ::visuVTKARAdaptor::SVideoAdapter)
            auto sig =
                m_image->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );

            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }

        this->updateImage();
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SFrameUpdater::updateImage()
{
    //Lock image & copy buffer
    {
        ::fwData::mt::ObjectWriteLock destLock(m_image);
        ::fwData::Array::sptr array = m_image->getDataArray();

        ::fwDataTools::helper::Array arrayHelper(array);

        ::fwCore::HiResClock::HiResClockType timestamp = m_frameTL->getNewerTimestamp();
        CSPTR(::arData::FrameTL::BufferType) buffer    = m_frameTL->getClosestBuffer(timestamp);

        OSLM_WARN_IF("Buffer not found with timestamp "<< timestamp, !buffer );
        if(buffer)
        {
            m_lastTimestamp = timestamp;

            const ::boost::uint8_t* frameBuff = &buffer->getElement(0);

            ::arData::timeline::Buffer::BufferDataType index = arrayHelper.begin< ::boost::uint8_t >();

            std::copy( frameBuff, frameBuff+buffer->getSize(), index);

            //Notify
            auto sig =
                m_image->signal< ::fwData::Image::BufferModifiedSignalType >( ::fwData::Image::s_BUFFER_MODIFIED_SIG );
            sig->asyncEmit();
        }
    }

}

//-----------------------------------------------------------------------------

void SFrameUpdater::requestRender()
{
    m_sigRenderRequested->asyncEmit();
}

} //namespace videoTools

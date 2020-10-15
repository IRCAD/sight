/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "videoTools/SFrameUpdater.hpp"

#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Worker.hpp>

fwServicesRegisterMacro( ::fwServices::IController, ::videoTools::SFrameUpdater, ::fwData::Composite )

namespace videoTools
{
//-----------------------------------------------------------------------------

//  Public slot
const ::fwCom::Slots::SlotKeyType SFrameUpdater::s_UPDATE_FRAME_SLOT = "updateFrame";

// Private slot
const ::fwCom::Slots::SlotKeyType s_RESET_TIMELINE_SLOT = "reset";

const ::fwCom::Signals::SignalKeyType SFrameUpdater::s_RENDER_REQUESTED_SIG = "renderRequested";

//-----------------------------------------------------------------------------

SFrameUpdater::SFrameUpdater() noexcept :
    m_lastTimestamp(0),
    m_imageInitialized(false)
{
    newSignal<RenderRequestedSignalType>(s_RENDER_REQUESTED_SIG);

    newSlot(s_UPDATE_FRAME_SLOT, &SFrameUpdater::updateFrame, this);
    newSlot(s_RESET_TIMELINE_SLOT, &SFrameUpdater::resetTimeline, this);
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
    connections.push( "frameTL", ::arData::TimeLine::s_CLEARED_SIG, s_RESET_TIMELINE_SLOT );

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
        ::fwData::Image::Size size;
        size[0] = m_frameTL->getWidth();
        size[1] = m_frameTL->getHeight();
        size[2] = 0;

        // Check if image dimensions has changed
        if(size != m_image->getSize2())
        {
            m_imageInitialized = false;
        }

        if(!m_imageInitialized)
        {
            ::fwData::mt::ObjectWriteLock destLock(m_image);

            ::fwData::Image::PixelFormat format;
            // FIXME currently, frameTL doesn't manage formats, so we assume that the frame are GrayScale, RGB or RGBA
            switch (m_frameTL->getNumberOfComponents())
            {
                case 1:
                    format = ::fwData::Image::GRAY_SCALE;
                    break;
                case 3:
                    format = ::fwData::Image::RGB;
                    break;
                case 4:
                    format = ::fwData::Image::RGBA;
                    break;
                default:
                    SLM_ERROR("Number of compenent not managed")
                    return;
            }

            m_image->resize(size, m_frameTL->getType(), format);
            const ::fwData::Image::Origin origin = {0., 0., 0.};
            m_image->setOrigin2(origin);
            const ::fwData::Image::Spacing spacing = {1., 1., 0.};
            m_image->setSpacing2(spacing);
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
    ::fwData::mt::ObjectWriteLock destLock(m_image);
    const auto dumpLock = m_image->lock();

    const ::fwCore::HiResClock::HiResClockType timestamp = m_frameTL->getNewerTimestamp();
    CSPTR(::arData::FrameTL::BufferType) buffer = m_frameTL->getClosestBuffer(timestamp);

    SLM_WARN_IF("Buffer not found with timestamp "<< timestamp, !buffer );
    if(buffer)
    {
        m_lastTimestamp = timestamp;

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        auto iter                     = m_image->begin<std::uint8_t>();

        std::copy( frameBuff, frameBuff+buffer->getSize(), iter);

        //Notify
        auto sig =
            m_image->signal< ::fwData::Image::BufferModifiedSignalType >( ::fwData::Image::s_BUFFER_MODIFIED_SIG );
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SFrameUpdater::requestRender()
{
    auto sig = this->signal<RenderRequestedSignalType>(s_RENDER_REQUESTED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SFrameUpdater::resetTimeline()
{
    // Reset the last timestamp in case the grabber uses a different time scale
    m_lastTimestamp = std::numeric_limits<double>::lowest();
}

//-----------------------------------------------------------------------------

} //namespace videoTools

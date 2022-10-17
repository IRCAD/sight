/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "SFrameUpdater.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hpp>

#include <data/Composite.hpp>
#include <data/Object.hpp>
#include <data/timeline/Buffer.hpp>

#include <service/macros.hpp>

namespace sight::module::sync
{

//-----------------------------------------------------------------------------

//  Public slot
const core::com::Slots::SlotKeyType SFrameUpdater::s_UPDATE_FRAME_SLOT = "updateFrame";

// Private slot
const core::com::Slots::SlotKeyType s_RESET_TIMELINE_SLOT = "reset";

const core::com::Signals::SignalKeyType SFrameUpdater::s_RENDER_REQUESTED_SIG = "renderRequested";

//-----------------------------------------------------------------------------

SFrameUpdater::SFrameUpdater() noexcept
{
    newSignal<RenderRequestedSignalType>(s_RENDER_REQUESTED_SIG);

    newSlot(s_UPDATE_FRAME_SLOT, &SFrameUpdater::updateFrame, this);
    newSlot(s_RESET_TIMELINE_SLOT, &SFrameUpdater::resetTimeline, this);
}

//-----------------------------------------------------------------------------

SFrameUpdater::~SFrameUpdater() noexcept =
    default;

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFrameUpdater::getAutoConnections() const
{
    return {
        {"frameTL", data::TimeLine::s_OBJECT_PUSHED_SIG, module::sync::SFrameUpdater::s_UPDATE_FRAME_SLOT},
        {"frameTL", data::TimeLine::s_CLEARED_SIG, s_RESET_TIMELINE_SLOT}
    };
}

//-----------------------------------------------------------------------------

void SFrameUpdater::starting()
{
    m_imageInitialized = false;
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

void SFrameUpdater::updateFrame(core::HiResClock::HiResClockType timestamp)
{
    if(timestamp > m_lastTimestamp)
    {
        {
            const auto frameTL = m_frameTL.lock();
            const auto image   = m_image.lock();

            data::Image::Size size;
            size[0] = frameTL->getWidth();
            size[1] = frameTL->getHeight();
            size[2] = 0;

            // Check if image dimensions has changed
            if(size != image->getSize())
            {
                m_imageInitialized = false;
            }

            if(!m_imageInitialized)
            {
                data::Image::PixelFormat format {data::Image::PixelFormat::UNDEFINED};
                // FIXME currently, frameTL doesn't manage formats, so we assume that the frame are GrayScale, RGB or
                // RGBA
                switch(frameTL->numComponents())
                {
                    case 1:
                        format = data::Image::GRAY_SCALE;
                        break;

                    case 3:
                        format = data::Image::RGB;
                        break;

                    case 4:
                        format = data::Image::RGBA;
                        break;

                    default:
                        SIGHT_ERROR("Number of components not managed")
                        return;
                }

                image->resize(size, frameTL->getType(), format);
                const data::Image::Origin origin = {0., 0., 0.};
                image->setOrigin(origin);
                const data::Image::Spacing spacing = {1., 1., 0.};
                image->setSpacing(spacing);
                image->setWindowWidth({1});
                image->setWindowCenter({0});
                m_imageInitialized = true;

                //Notify
                auto sig = image->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);

                {
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }

        this->updateImage();
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SFrameUpdater::updateImage()
{
    const auto frameTL = m_frameTL.lock();
    const auto image   = m_image.lock();

    const core::HiResClock::HiResClockType timestamp = frameTL->getNewerTimestamp();
    CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

    SIGHT_WARN_IF("Buffer not found with timestamp " << timestamp, !buffer);
    if(buffer)
    {
        m_lastTimestamp = timestamp;

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        auto iter                     = image->begin<std::uint8_t>();

        std::copy(frameBuff, frameBuff + buffer->getSize(), iter);

        auto sig = image->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG);
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

} //namespace sight::module::sync

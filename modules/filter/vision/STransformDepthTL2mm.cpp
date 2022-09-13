/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "STransformDepthTL2mm.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

namespace sight::module::filter::vision
{

static const core::com::Slots::SlotKeyType s_COMPUTE_SLOT = "compute";

//------------------------------------------------------------------------------

STransformDepthTL2mm::STransformDepthTL2mm()
{
    newSlot(s_COMPUTE_SLOT, &STransformDepthTL2mm::compute, this);
}

//------------------------------------------------------------------------------

STransformDepthTL2mm::~STransformDepthTL2mm()
= default;

//------------------------------------------------------------------------------

void STransformDepthTL2mm::starting()
{
}

//------------------------------------------------------------------------------

void STransformDepthTL2mm::stopping()
{
}

//------------------------------------------------------------------------------

void STransformDepthTL2mm::configuring()
{
}

//------------------------------------------------------------------------------

void STransformDepthTL2mm::compute(core::HiResClock::HiResClockType timestamp)
{
    if(timestamp > m_lastTimestamp)
    {
        const auto originFrameTL = m_originFrameTL.lock();
        SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_TL_INPUT << "' timeline", originFrameTL);
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT("missing '" << s_CAMERA_SET_INPUT << "' cameraSet", camera_set);
        data::Camera::csptr depthCamera = camera_set->get_camera(0);

        auto scaledFrameTL = m_scaledDepthTL.lock();
        SIGHT_ASSERT("missing '" << s_SCALED_FRAME_TL_INOUT << "' timeline", scaledFrameTL);

        const double scale = depthCamera->getScale();

        const auto depthBufferObj = originFrameTL->getClosestBuffer(timestamp);

        if(depthBufferObj)
        {
            const std::size_t width  = originFrameTL->getWidth();
            const std::size_t height = originFrameTL->getHeight();
            const std::size_t size   = width * height;

            if(scaledFrameTL->getWidth() == 0 || scaledFrameTL->getHeight() == 0)
            {
                scaledFrameTL->initPoolSize(
                    width,
                    height,
                    core::Type::UINT16,
                    data::FrameTL::PixelFormat::GRAY_SCALE
                );
            }

            const auto* depthBufferIn = reinterpret_cast<const std::uint16_t*>(&depthBufferObj->getElement(0));

            SPTR(data::FrameTL::BufferType) depthBufferOutObj = scaledFrameTL->createBuffer(timestamp);

            auto* depthBufferOut = reinterpret_cast<std::uint16_t*>(depthBufferOutObj->addElement(0));

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                *depthBufferOut++ = static_cast<std::uint16_t>((*depthBufferIn++) * scale);
            }

            scaledFrameTL->pushObject(depthBufferOutObj);

            auto sig =
                scaledFrameTL->signal<data::TimeLine::ObjectPushedSignalType>(
                    data::TimeLine::s_OBJECT_PUSHED_SIG
                );
            sig->asyncEmit(timestamp);
            m_sigComputed->asyncEmit();
        }

        m_lastTimestamp = timestamp;
    }
}

//------------------------------------------------------------------------------

void STransformDepthTL2mm::updating()
{
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap STransformDepthTL2mm::getAutoConnections() const
{
    return {{s_ORIGIN_FRAME_TL_INPUT, data::BufferTL::s_OBJECT_PUSHED_SIG, s_COMPUTE_SLOT}};
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision

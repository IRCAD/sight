/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "transform_depth_tl2mm.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::module::filter::vision
{

static const core::com::slots::key_t COMPUTE_SLOT = "compute";

//------------------------------------------------------------------------------

transform_depth_tl2mm::transform_depth_tl2mm()
{
    new_slot(COMPUTE_SLOT, &transform_depth_tl2mm::compute, this);
}

//------------------------------------------------------------------------------

transform_depth_tl2mm::~transform_depth_tl2mm()
= default;

//------------------------------------------------------------------------------

void transform_depth_tl2mm::starting()
{
}

//------------------------------------------------------------------------------

void transform_depth_tl2mm::stopping()
{
}

//------------------------------------------------------------------------------

void transform_depth_tl2mm::configuring()
{
}

//------------------------------------------------------------------------------

void transform_depth_tl2mm::compute(core::hires_clock::type timestamp)
{
    if(timestamp > m_lastTimestamp)
    {
        const auto originFrameTL = m_originFrameTL.lock();
        SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_TL_INPUT << "' timeline", originFrameTL);
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT("missing '" << s_CAMERA_SET_INPUT << "' cameraSet", camera_set);
        data::camera::csptr depthCamera = camera_set->get_camera(0);

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
                    core::type::UINT16,
                    data::frame_tl::PixelFormat::GRAY_SCALE
                );
            }

            const auto* depthBufferIn = reinterpret_cast<const std::uint16_t*>(&depthBufferObj->getElement(0));

            SPTR(data::frame_tl::BufferType) depthBufferOutObj = scaledFrameTL->createBuffer(timestamp);

            auto* depthBufferOut = reinterpret_cast<std::uint16_t*>(depthBufferOutObj->addElement(0));

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                *depthBufferOut++ = static_cast<std::uint16_t>((*depthBufferIn++) * scale);
            }

            scaledFrameTL->pushObject(depthBufferOutObj);

            auto sig =
                scaledFrameTL->signal<data::timeline::signals::pushed_t>(
                    data::timeline::signals::PUSHED
                );
            sig->async_emit(timestamp);
            m_sigComputed->async_emit();
        }

        m_lastTimestamp = timestamp;
    }
}

//------------------------------------------------------------------------------

void transform_depth_tl2mm::updating()
{
}

//-----------------------------------------------------------------------------

service::connections_t transform_depth_tl2mm::auto_connections() const
{
    return {{s_ORIGIN_FRAME_TL_INPUT, data::timeline::signals::PUSHED, COMPUTE_SLOT}};
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision

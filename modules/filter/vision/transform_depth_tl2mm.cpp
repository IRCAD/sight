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

void transform_depth_tl2mm::compute(core::hires_clock::type _timestamp)
{
    if(_timestamp > m_lastTimestamp)
    {
        const auto origin_frame_tl = m_originFrameTL.lock();
        SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_TL_INPUT << "' timeline", origin_frame_tl);
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT("missing '" << s_CAMERA_SET_INPUT << "' cameraSet", camera_set);
        data::camera::csptr depth_camera = camera_set->get_camera(0);

        auto scaled_frame_tl = m_scaledDepthTL.lock();
        SIGHT_ASSERT("missing '" << s_SCALED_FRAME_TL_INOUT << "' timeline", scaled_frame_tl);

        const double scale = depth_camera->getScale();

        const auto depth_buffer_obj = origin_frame_tl->getClosestBuffer(_timestamp);

        if(depth_buffer_obj)
        {
            const std::size_t width  = origin_frame_tl->getWidth();
            const std::size_t height = origin_frame_tl->getHeight();
            const std::size_t size   = width * height;

            if(scaled_frame_tl->getWidth() == 0 || scaled_frame_tl->getHeight() == 0)
            {
                scaled_frame_tl->initPoolSize(
                    width,
                    height,
                    core::type::UINT16,
                    data::frame_tl::PixelFormat::GRAY_SCALE
                );
            }

            const auto* depth_buffer_in = reinterpret_cast<const std::uint16_t*>(&depth_buffer_obj->getElement(0));

            SPTR(data::frame_tl::buffer_t) depth_buffer_out_obj = scaled_frame_tl->createBuffer(_timestamp);

            auto* depth_buffer_out = reinterpret_cast<std::uint16_t*>(depth_buffer_out_obj->addElement(0));

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                *depth_buffer_out++ = static_cast<std::uint16_t>((*depth_buffer_in++) * scale);
            }

            scaled_frame_tl->pushObject(depth_buffer_out_obj);

            auto sig =
                scaled_frame_tl->signal<data::timeline::signals::pushed_t>(
                    data::timeline::signals::PUSHED
                );
            sig->async_emit(_timestamp);
            m_sigComputed->async_emit();
        }

        m_lastTimestamp = _timestamp;
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

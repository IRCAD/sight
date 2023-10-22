/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "transform_depth_map2mm.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/image.hpp>

namespace sight::module::filter::vision
{

//------------------------------------------------------------------------------

transform_depth_map2mm::transform_depth_map2mm()
= default;

//------------------------------------------------------------------------------

transform_depth_map2mm::~transform_depth_map2mm()
= default;

//------------------------------------------------------------------------------

void transform_depth_map2mm::starting()
{
}

//------------------------------------------------------------------------------

void transform_depth_map2mm::stopping()
{
}

//------------------------------------------------------------------------------

void transform_depth_map2mm::configuring()
{
}

//------------------------------------------------------------------------------

void transform_depth_map2mm::updating()
{
    const auto camera_set = m_camera_set.lock();
    SIGHT_ASSERT("missing '" << s_CAMERA_SET_INPUT << "' cameraSet", camera_set);

    data::camera::csptr depth_camera = camera_set->get_camera(0);

    const double scale = depth_camera->getScale();

    auto origin_frame = m_originDepth.lock();
    SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_INPUT << "' image", origin_frame);

    const auto type = origin_frame->getType();
    if(type != core::type::UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size = origin_frame->size();

    auto scaled_frame = m_scaledDepth.lock();
    SIGHT_ASSERT("missing '" << s_SCALED_FRAME_INOUT << "' image", scaled_frame);

    if(size != scaled_frame->size())
    {
        scaled_frame->resize(size, origin_frame->getType(), origin_frame->getPixelFormat());

        const data::image::Origin origin = {0., 0., 0.};
        scaled_frame->setOrigin(origin);
        const data::image::Spacing spacing = {1., 1., 1.};
        scaled_frame->setSpacing(spacing);
        scaled_frame->setWindowWidth({1});
        scaled_frame->setWindowCenter({0});
    }

    const auto orig_dump_lock   = origin_frame->dump_lock();
    const auto scaled_dump_lock = scaled_frame->dump_lock();

    auto depth_buffer_in_itr    = origin_frame->begin<std::uint16_t>();
    const auto depth_buffer_end = origin_frame->end<std::uint16_t>();
    auto depth_buffer_out_itr   = scaled_frame->begin<std::uint16_t>();

    for( ; depth_buffer_in_itr != depth_buffer_end ; ++depth_buffer_in_itr, ++depth_buffer_out_itr)
    {
        *depth_buffer_out_itr = static_cast<std::uint16_t>((*depth_buffer_in_itr) * scale);
    }

    auto sig = scaled_frame->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
    sig->async_emit();

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision

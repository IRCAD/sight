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

    data::camera::csptr depthCamera = camera_set->get_camera(0);

    const double scale = depthCamera->getScale();

    auto originFrame = m_originDepth.lock();
    SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_INPUT << "' image", originFrame);

    const auto type = originFrame->getType();
    if(type != core::type::UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size = originFrame->size();

    auto scaledFrame = m_scaledDepth.lock();
    SIGHT_ASSERT("missing '" << s_SCALED_FRAME_INOUT << "' image", scaledFrame);

    if(size != scaledFrame->size())
    {
        scaledFrame->resize(size, originFrame->getType(), originFrame->getPixelFormat());

        const data::image::Origin origin = {0., 0., 0.};
        scaledFrame->setOrigin(origin);
        const data::image::Spacing spacing = {1., 1., 1.};
        scaledFrame->setSpacing(spacing);
        scaledFrame->setWindowWidth({1});
        scaledFrame->setWindowCenter({0});
    }

    const auto origDumpLock   = originFrame->dump_lock();
    const auto scaledDumpLock = scaledFrame->dump_lock();

    auto depthBufferInItr     = originFrame->begin<std::uint16_t>();
    const auto depthBufferEnd = originFrame->end<std::uint16_t>();
    auto depthBufferOutItr    = scaledFrame->begin<std::uint16_t>();

    for( ; depthBufferInItr != depthBufferEnd ; ++depthBufferInItr, ++depthBufferOutItr)
    {
        *depthBufferOutItr = static_cast<std::uint16_t>((*depthBufferInItr) * scale);
    }

    auto sig = scaledFrame->signal<data::image::ModifiedSignalType>(data::image::MODIFIED_SIG);
    sig->async_emit();

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision

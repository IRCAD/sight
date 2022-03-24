/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "STransformDepthMap2mm.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Image.hpp>

namespace sight::module::filter::vision
{

//------------------------------------------------------------------------------

STransformDepthMap2mm::STransformDepthMap2mm()
{
}

//------------------------------------------------------------------------------

STransformDepthMap2mm::~STransformDepthMap2mm()
{
}

//------------------------------------------------------------------------------

void STransformDepthMap2mm::starting()
{
}

//------------------------------------------------------------------------------

void STransformDepthMap2mm::stopping()
{
}

//------------------------------------------------------------------------------

void STransformDepthMap2mm::configuring()
{
}

//------------------------------------------------------------------------------

void STransformDepthMap2mm::updating()
{
    const auto cameraSeries = m_cameraSeries.lock();
    SIGHT_ASSERT("missing '" << s_CAMERA_SERIES_INPUT << "' cameraSeries", cameraSeries);

    data::Camera::csptr depthCamera = cameraSeries->getCamera(0);

    const double scale = depthCamera->getScale();

    auto originFrame = m_originDepth.lock();
    SIGHT_ASSERT("missing '" << s_ORIGIN_FRAME_INPUT << "' image", originFrame);

    const auto type = originFrame->getType();
    if(type != core::Type::UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size = originFrame->getSize();

    auto scaledFrame = m_scaledDepth.lock();
    SIGHT_ASSERT("missing '" << s_SCALED_FRAME_INOUT << "' image", scaledFrame);

    if(size != scaledFrame->getSize())
    {
        scaledFrame->resize(size, originFrame->getType(), originFrame->getPixelFormat());

        const data::Image::Origin origin = {0., 0., 0.};
        scaledFrame->setOrigin(origin);
        const data::Image::Spacing spacing = {1., 1., 1.};
        scaledFrame->setSpacing(spacing);
        scaledFrame->setWindowWidth(1);
        scaledFrame->setWindowCenter(0);
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

    auto sig = scaledFrame->signal<data::Image::ModifiedSignalType>(data::Image::s_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision

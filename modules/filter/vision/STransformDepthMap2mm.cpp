/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include <data/CameraSeries.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/tools/helper/Array.hpp>
#include <data/tools/helper/ArrayGetter.hpp>

#include <service/macros.hpp>

namespace sight::modules::filter::vision
{

static const service::IService::KeyType s_CAMERA_SERIES_INPUT = "cameraSeries";
static const service::IService::KeyType s_ORIGIN_FRAME_INPUT  = "originDepth";
static const service::IService::KeyType s_SCALED_FRAME_INOUT  = "scaledDepth";

fwServicesRegisterMacro( ::sight::service::IOperator, ::sight::modules::filter::vision::STransformDepthMap2mm)

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
    data::CameraSeries::csptr cameraSeries = this->getInput< data::CameraSeries >(s_CAMERA_SERIES_INPUT);
    SLM_ASSERT("missing '" + s_CAMERA_SERIES_INPUT + "' cameraSeries", cameraSeries);
    data::Camera::csptr depthCamera = cameraSeries->getCamera(0);

    const double scale = depthCamera->getScale();

    auto originFrame = this->getInput< data::Image >(s_ORIGIN_FRAME_INPUT);
    SLM_ASSERT("missing '" + s_ORIGIN_FRAME_INPUT + "' image", originFrame);

    data::mt::ObjectReadLock originLock(originFrame);

    const auto type = originFrame->getType();
    if(type != core::tools::Type::s_UINT16)
    {
        SLM_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size = originFrame->getSize2();

    auto scaledFrame = this->getInOut< data::Image >(s_SCALED_FRAME_INOUT);
    SLM_ASSERT("missing '" + s_SCALED_FRAME_INOUT + "' image", scaledFrame);

    data::mt::ObjectWriteLock scaledFrameLock(scaledFrame);

    if(size != scaledFrame->getSize2())
    {
        scaledFrame->resize(size, originFrame->getType(), originFrame->getPixelFormat());

        if (scaledFrame->getNumberOfComponents() != originFrame->getNumberOfComponents())
        {
            FW_DEPRECATED_MSG("Pixel format is not properly defined.", "sight 22.0");
            scaledFrame->setNumberOfComponents(originFrame->getNumberOfComponents());
            scaledFrame->resize();
        }

        const data::Image::Origin origin = {0., 0., 0.};
        scaledFrame->setOrigin2(origin);
        const data::Image::Spacing spacing = {1., 1., 1.};
        scaledFrame->setSpacing2(spacing);
        scaledFrame->setWindowWidth(1);
        scaledFrame->setWindowCenter(0);
    }

    const auto origDumpLock   = originFrame->lock();
    const auto scaledDumpLock = scaledFrame->lock();

    auto depthBufferInItr     = originFrame->begin< std::uint16_t >();
    const auto depthBufferEnd = originFrame->end< std::uint16_t >();
    auto depthBufferOutItr    = scaledFrame->begin< std::uint16_t >();

    for (; depthBufferInItr != depthBufferEnd; ++depthBufferInItr, ++depthBufferOutItr)
    {
        *depthBufferOutItr = static_cast<std::uint16_t>((*depthBufferInItr)*scale);
    }

    auto sig = scaledFrame->signal< data::Image::ModifiedSignalType >(data::Image::s_MODIFIED_SIG );
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::filter::vision

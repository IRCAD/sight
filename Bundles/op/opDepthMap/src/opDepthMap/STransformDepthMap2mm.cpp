/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
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

#include "opDepthMap/STransformDepthMap2mm.hpp"

#include <arData/CameraSeries.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>

#include <fwServices/macros.hpp>

namespace opDepthMap
{

static const ::fwServices::IService::KeyType s_CAMERA_SERIES_INPUT = "cameraSeries";
static const ::fwServices::IService::KeyType s_ORIGIN_FRAME_INPUT  = "originDepth";
static const ::fwServices::IService::KeyType s_SCALED_FRAME_INOUT  = "scaledDepth";

fwServicesRegisterMacro(::fwServices::IOperator, ::opDepthMap::STransformDepthMap2mm)

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
    ::arData::CameraSeries::csptr cameraSeries = this->getInput< ::arData::CameraSeries >(s_CAMERA_SERIES_INPUT);
    SLM_ASSERT("missing '" + s_CAMERA_SERIES_INPUT + "' cameraSeries", cameraSeries);
    ::arData::Camera::csptr depthCamera = cameraSeries->getCamera(0);

    const double scale = depthCamera->getScale();

    auto originFrame = this->getInput< ::fwData::Image >(s_ORIGIN_FRAME_INPUT);
    SLM_ASSERT("missing '" + s_ORIGIN_FRAME_INPUT + "' image", originFrame);

    ::fwData::mt::ObjectReadLock originLock(originFrame);

    const auto type = originFrame->getType();
    if(type != ::fwTools::Type::s_UINT16)
    {
        OSLM_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size = originFrame->getSize2();

    auto scaledFrame = this->getInOut< ::fwData::Image >(s_SCALED_FRAME_INOUT);
    SLM_ASSERT("missing '" + s_SCALED_FRAME_INOUT + "' image", scaledFrame);

    ::fwData::mt::ObjectWriteLock scaledFrameLock(scaledFrame);

    if(size != scaledFrame->getSize2())
    {
        scaledFrame->resize(size, originFrame->getType(), originFrame->getPixelFormat());

        if (scaledFrame->getNumberOfComponents() != originFrame->getNumberOfComponents())
        {
            FW_DEPRECATED_MSG("Pixel format is not properly defined.", "sight 22.0");
            scaledFrame->setNumberOfComponents(originFrame->getNumberOfComponents());
            scaledFrame->resize();
        }

        const ::fwData::Image::Origin origin = {0., 0., 0.};
        scaledFrame->setOrigin2(origin);
        const ::fwData::Image::Spacing spacing = {1., 1., 1.};
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

    auto sig = scaledFrame->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG );
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace opDepthMap

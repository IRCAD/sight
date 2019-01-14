/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "opDepthMap/STransformDepthTL2mm.hpp"

#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

namespace opDepthMap
{

static const ::fwCom::Slots::SlotKeyType s_COMPUTE_SLOT = "compute";

static const ::fwServices::IService::KeyType s_CAMERA_SERIES_INPUT   = "cameraSeries";
static const ::fwServices::IService::KeyType s_ORIGIN_FRAME_TL_INPUT = "originDepthTL";
static const ::fwServices::IService::KeyType s_SCALED_FRAME_TL_INOUT = "scaledDepthTL";

fwServicesRegisterMacro(::fwServices::IOperator, ::opDepthMap::STransformDepthTL2mm);

//------------------------------------------------------------------------------

STransformDepthTL2mm::STransformDepthTL2mm() :
    m_lastTimestamp(0)
{
    newSlot(s_COMPUTE_SLOT, &STransformDepthTL2mm::compute, this);
}

//------------------------------------------------------------------------------

STransformDepthTL2mm::~STransformDepthTL2mm()
{
}

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

void STransformDepthTL2mm::compute(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (timestamp > m_lastTimestamp)
    {
        ::arData::FrameTL::csptr originFrameTL = this->getInput< ::arData::FrameTL >(s_ORIGIN_FRAME_TL_INPUT);
        SLM_ASSERT("missing '" + s_ORIGIN_FRAME_TL_INPUT + "' timeline", originFrameTL);
        ::arData::CameraSeries::csptr cameraSeries = this->getInput< ::arData::CameraSeries >(s_CAMERA_SERIES_INPUT);
        SLM_ASSERT("missing '" + s_CAMERA_SERIES_INPUT + "' cameraSeries", cameraSeries);
        ::arData::Camera::csptr depthCamera   = cameraSeries->getCamera(0);
        ::arData::FrameTL::sptr scaledFrameTL = this->getInOut< ::arData::FrameTL >(s_SCALED_FRAME_TL_INOUT);
        SLM_ASSERT("missing '" + s_SCALED_FRAME_TL_INOUT + "' timeline", scaledFrameTL);

        const double scale = depthCamera->getScale();

        const auto depthBufferObj = originFrameTL->getClosestBuffer(timestamp);

        if (depthBufferObj)
        {
            const size_t width  = originFrameTL->getWidth();
            const size_t height = originFrameTL->getHeight();
            const size_t size   = width * height;

            if (scaledFrameTL->getWidth() == 0 || scaledFrameTL->getHeight() == 0)
            {
                scaledFrameTL->initPoolSize(width, height, ::fwTools::Type::s_UINT16, 1);
            }

            const std::uint16_t* depthBufferIn = reinterpret_cast<const std::uint16_t*>(&depthBufferObj->getElement(0));

            SPTR(::arData::FrameTL::BufferType) depthBufferOutObj = scaledFrameTL->createBuffer(timestamp);

            std::uint16_t* depthBufferOut = reinterpret_cast<std::uint16_t*>(depthBufferOutObj->addElement(0));

            for (size_t i = 0; i < size; ++i)
            {
                *depthBufferOut++ = static_cast<std::uint16_t>((*depthBufferIn++)*scale);
            }

            scaledFrameTL->pushObject(depthBufferOutObj);

            auto sig =
                scaledFrameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG);
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

::fwServices::IService::KeyConnectionsMap STransformDepthTL2mm::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( s_ORIGIN_FRAME_TL_INPUT, ::arData::BufferTL::s_OBJECT_PUSHED_SIG, s_COMPUTE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace opDepthMap

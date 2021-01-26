/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "videoFilter/SCropVideoQuad.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Type.hpp>

#include <services/macros.hpp>

namespace videoFilter
{

fwServicesRegisterMacro( ::videoFilter::IVideoFilter, ::videoFilter::SCropVideoQuad, ::sight::data::FrameTL)

const core::com::Slots::SlotKeyType SCropVideoQuad::s_CROP_FRAME_SLOT = "cropFrame";

const services::IService::KeyType s_FRAMETL_INPUT  = "quadFrameTL";
const services::IService::KeyType s_FRAMETL1_INOUT = "frameTL1";
const services::IService::KeyType s_FRAMETL2_INOUT = "frameTL2";
const services::IService::KeyType s_FRAMETL3_INOUT = "frameTL3";
const services::IService::KeyType s_FRAMETL4_INOUT = "frameTL4";

//-----------------------------------------------------------------------------

SCropVideoQuad::SCropVideoQuad() noexcept
{
    newSlot(s_CROP_FRAME_SLOT, &SCropVideoQuad::cropFrame, this);
}

//-----------------------------------------------------------------------------

SCropVideoQuad::~SCropVideoQuad() noexcept
{

}

//-----------------------------------------------------------------------------

void SCropVideoQuad::starting()
{
}

//-----------------------------------------------------------------------------

void SCropVideoQuad::stopping()
{
}

//-----------------------------------------------------------------------------

void SCropVideoQuad::configuring()
{
}

//-----------------------------------------------------------------------------

void SCropVideoQuad::updating()
{
}

//----------------------------------------------------------------------------

void SCropVideoQuad::cropFrame(core::HiResClock::HiResClockType timestamp)
{
    data::FrameTL::csptr frameTL = this->getInput< data::FrameTL >(s_FRAMETL_INPUT);

    core::HiResClock::HiResClockType newerTimestamp = frameTL->getNewerTimestamp();
    const CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(newerTimestamp);

    data::FrameTL::sptr frameTL1 = this->getInOut< data::FrameTL >(s_FRAMETL1_INOUT);
    data::FrameTL::sptr frameTL2 = this->getInOut< data::FrameTL >(s_FRAMETL2_INOUT);
    data::FrameTL::sptr frameTL3 = this->getInOut< data::FrameTL >(s_FRAMETL3_INOUT);
    data::FrameTL::sptr frameTL4 = this->getInOut< data::FrameTL >(s_FRAMETL4_INOUT);

    const size_t width  = frameTL->getWidth();
    const size_t height = frameTL->getHeight();

    const size_t widthTL1  = frameTL1->getWidth();
    const size_t heightTL1 = frameTL1->getHeight();

    const size_t widthCropped  = width / 2;
    const size_t heightCropped = height / 2;

    if (widthCropped != widthTL1 || heightCropped != heightTL1)
    {
        const size_t numberOfComponents = frameTL->getNumberOfComponents();
        const core::tools::Type type    = frameTL->getType();

        frameTL1->clearTimeline();
        frameTL1->initPoolSize(widthCropped, heightCropped, type, numberOfComponents);
        frameTL2->clearTimeline();
        frameTL2->initPoolSize(widthCropped, heightCropped, type, numberOfComponents);
        frameTL3->clearTimeline();
        frameTL3->initPoolSize(widthCropped, heightCropped, type, numberOfComponents);
        frameTL4->clearTimeline();
        frameTL4->initPoolSize(widthCropped, heightCropped, type, numberOfComponents);
    }

    if(buffer)
    {
        const std::uint8_t* frameBuff = &buffer->getElement(0);

        // The cv image that will be processed
        ::cv::Mat imgIn(height, width, CV_8UC4, (void*)frameBuff, ::cv::Mat::AUTO_STEP);
        const ::cv::Rect roi1(0, 0, widthCropped, heightCropped);
        const ::cv::Rect roi2(widthCropped, 0, widthCropped, heightCropped);
        const ::cv::Rect roi3(0, heightCropped, widthCropped, heightCropped);
        const ::cv::Rect roi4(widthCropped, heightCropped, widthCropped, heightCropped);

        this->pushFrameInTimeline(imgIn, roi1, frameTL1, newerTimestamp);
        this->pushFrameInTimeline(imgIn, roi2, frameTL2, newerTimestamp);
        this->pushFrameInTimeline(imgIn, roi3, frameTL3, newerTimestamp);
        this->pushFrameInTimeline(imgIn, roi4, frameTL4, newerTimestamp);
    }
}

//----------------------------------------------------------------------------

void SCropVideoQuad::pushFrameInTimeline(::cv::Mat& imgIn,
                                         const ::cv::Rect& roi,
                                         data::FrameTL::sptr& frameTL,
                                         core::HiResClock::HiResClockType timestamp)
{
    const size_t width  = frameTL->getWidth();
    const size_t height = frameTL->getHeight();

    // Get the buffer of the timeline to fill
    SPTR(data::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
    std::uint8_t* frameBuffOut = bufferOut->addElement(0);

    // Create an openCV mat that aliases the buffer created from the output timeline
    ::cv::Mat imgOut(height, width, imgIn.type(), (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    ::cv::Mat croppedImage = imgIn(roi);

    croppedImage.copyTo(imgOut);

    // push buffer and notify
    frameTL->pushObject(bufferOut);
    data::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = frameTL->signal< data::TimeLine::ObjectPushedSignalType >(data::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//----------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SCropVideoQuad::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_FRAMETL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, s_CROP_FRAME_SLOT );

    return connections;
}

//----------------------------------------------------------------------------
} // namespace videoFilter

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoFilter/SCropVideoQuad.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

namespace videoFilter
{

fwServicesRegisterMacro( ::videoFilter::IVideoFilter, ::videoFilter::SCropVideoQuad, ::arData::FrameTL);

const ::fwCom::Slots::SlotKeyType SCropVideoQuad::s_CROP_FRAME_SLOT = "cropFrame";

const ::fwServices::IService::KeyType s_FRAMETL_INPUT  = "quadFrameTL";
const ::fwServices::IService::KeyType s_FRAMETL1_INOUT = "frameTL1";
const ::fwServices::IService::KeyType s_FRAMETL2_INOUT = "frameTL2";
const ::fwServices::IService::KeyType s_FRAMETL3_INOUT = "frameTL3";
const ::fwServices::IService::KeyType s_FRAMETL4_INOUT = "frameTL4";

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

void SCropVideoQuad::cropFrame(::fwCore::HiResClock::HiResClockType timestamp)
{
    ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(s_FRAMETL_INPUT);

    ::fwCore::HiResClock::HiResClockType newerTimestamp = frameTL->getNewerTimestamp();
    const CSPTR(::arData::FrameTL::BufferType) buffer   = frameTL->getClosestBuffer(newerTimestamp);

    ::arData::FrameTL::sptr frameTL1 = this->getInOut< ::arData::FrameTL >(s_FRAMETL1_INOUT);
    ::arData::FrameTL::sptr frameTL2 = this->getInOut< ::arData::FrameTL >(s_FRAMETL2_INOUT);
    ::arData::FrameTL::sptr frameTL3 = this->getInOut< ::arData::FrameTL >(s_FRAMETL3_INOUT);
    ::arData::FrameTL::sptr frameTL4 = this->getInOut< ::arData::FrameTL >(s_FRAMETL4_INOUT);

    const size_t width  = frameTL->getWidth();
    const size_t height = frameTL->getHeight();

    const size_t widthTL1  = frameTL1->getWidth();
    const size_t heightTL1 = frameTL1->getHeight();

    const size_t widthCropped  = width / 2;
    const size_t heightCropped = height / 2;

    if (widthCropped != widthTL1 || heightCropped != heightTL1)
    {
        const size_t numberOfComponents = frameTL->getNumberOfComponents();
        const ::fwTools::Type type      = frameTL->getType();

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
                                         ::arData::FrameTL::sptr& frameTL,
                                         ::fwCore::HiResClock::HiResClockType timestamp)
{
    const size_t width  = frameTL->getWidth();
    const size_t height = frameTL->getHeight();

    // Get the buffer of the timeline to fill
    SPTR(::arData::FrameTL::BufferType) bufferOut = frameTL->createBuffer(timestamp);
    std::uint8_t* frameBuffOut = bufferOut->addElement(0);

    // Create an openCV mat that aliases the buffer created from the output timeline
    ::cv::Mat imgOut(height, width, imgIn.type(), (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    ::cv::Mat croppedImage = imgIn(roi);

    croppedImage.copyTo(imgOut);

    // push buffer and notify
    frameTL->pushObject(bufferOut);
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SCropVideoQuad::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_FRAMETL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_CROP_FRAME_SLOT );

    return connections;
}

//----------------------------------------------------------------------------
} // namespace videoFilter

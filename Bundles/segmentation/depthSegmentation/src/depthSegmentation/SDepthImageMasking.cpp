/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "depthSegmentation/SDepthImageMasking.hpp"

#include <arData/FrameTL.hpp>

#include <cvIO/FrameTL.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>

namespace depthSegmentation
{

fwServicesRegisterMacro( ::fwServices::IOperator, ::depthSegmentation::SDepthImageMasking);

const ::fwCom::Slots::SlotKeyType SDepthImageMasking::s_SET_BACKGROUND_SLOT = "setBackground";

const ::fwServices::IService::KeyType s_MASK_KEY          = "mask";
const ::fwServices::IService::KeyType s_VIDEO_TL_KEY      = "videoTL";
const ::fwServices::IService::KeyType s_VIDEO_MASK_TL_KEY = "videoMaskTL";

// ------------------------------------------------------------------------------

SDepthImageMasking::SDepthImageMasking() noexcept
{
    newSlot( s_SET_BACKGROUND_SLOT, &SDepthImageMasking::setBackground, this );
}

// ------------------------------------------------------------------------------

SDepthImageMasking::~SDepthImageMasking() noexcept
{
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::starting()
{
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::stopping()
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SDepthImageMasking::getAutoConnections() const
{
    KeyConnectionsMap connections;

    return connections;
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::updating()
{

}

// ------------------------------------------------------------------------------

void SDepthImageMasking::setBackground()
{
    auto mask = this->getInput< ::fwData::Image >(s_MASK_KEY);
    auto videoTL = this->getInput< ::arData::FrameTL >(s_VIDEO_TL_KEY);

    ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::arData::FrameTL::BufferType) videoBuffer      = videoTL->getClosestBuffer(currentTimestamp);
    if(!videoBuffer)
    {
        OSLM_ERROR("Buffer not found with timestamp " << currentTimestamp);
        return;
    }
    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    //convert the ::arData::FrameTL videoTL to an OpenCV image
    const ::cv::Mat videoCV = ::cvIO::FrameTL::moveToCv(videoTL, frameBuffOutVideo);

    // convert the ::fwData::Image mask to an OpenCV image
     ::cv::Mat maskCV = ::cvIO::Image::copyToCv(mask);

        ::cv::cvtColor(maskCV, maskCV, cv::COLOR_BGR2GRAY);

        maskCV = (maskCV > 0);

    ::cv::imshow("Depth", maskCV);

    // Initialize the mask timeline
    auto videoMaskTL = this->getInOut< ::arData::FrameTL >(s_VIDEO_MASK_TL_KEY);
    videoMaskTL->initPoolSize(videoTL->getWidth(), videoTL->getHeight(), ::fwTools::Type::s_UINT8, 4);
}

// ------------------------------------------------------------------------------

} // namespace depthSegmentation

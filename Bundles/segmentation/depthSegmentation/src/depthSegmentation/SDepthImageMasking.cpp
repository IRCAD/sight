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

#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>

#include <librealsense2/rs.hpp>

namespace depthSegmentation
{

const ::fwCom::Slots::SlotKeyType SDepthImageMasking::s_SET_BACKGROUND_SLOT = "setBackground";
const ::fwCom::Slots::SlotKeyType SDepthImageMasking::s_SET_THRESHOLD_SLOT  = "setThreshold";

fwServicesRegisterMacro( ::fwServices::IOperator, ::depthSegmentation::SDepthImageMasking)

const ::fwServices::IService::KeyType s_MASK_IMAGE_KEY = "maskImage";
const ::fwServices::IService::KeyType s_VIDEO_IMAGE_KEY      = "videoImage";
const ::fwServices::IService::KeyType s_DEPTH_IMAGE_KEY      = "depthImage";
const ::fwServices::IService::KeyType s_FOREGROUND_IMAGE_KEY = "foregroundImage";

// ------------------------------------------------------------------------------

SDepthImageMasking::SDepthImageMasking() noexcept
{
    newSlot( s_SET_BACKGROUND_SLOT, &SDepthImageMasking::setBackground, this );
    newSlot( s_SET_THRESHOLD_SLOT, &SDepthImageMasking::setThreshold, this );
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

    connections.push(s_DEPTH_IMAGE_KEY, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::updating()
{
    if(!m_cvDepthMaskImage.empty())
    {
        ::fwData::Image::sptr videoImage = this->getInOut< ::fwData::Image >(s_VIDEO_IMAGE_KEY);
        ::fwData::Image::sptr depthImage = this->getInOut< ::fwData::Image >(s_DEPTH_IMAGE_KEY);

        ::cv::Mat cvVideoImage = ::cvIO::Image::moveToCv(videoImage);
        ::cv::Mat cvDepthImage = ::cvIO::Image::moveToCv(depthImage);

        ::cv::Mat cvMaskedDepth;
        cvDepthImage.copyTo(cvMaskedDepth, m_cvMaskImage);

        ::cv::Mat cvForegroundImage = (cvMaskedDepth < (m_cvDepthMaskImage - m_threshold));

        ::cv::Mat morphElem = ::cv::getStructuringElement(::cv::MORPH_ELLIPSE, ::cv::Size(7, 7));
        ::cv::dilate(cvForegroundImage, cvForegroundImage, morphElem);
        ::cv::erode(cvForegroundImage, cvForegroundImage, morphElem);

        ::cv::Mat cvMaskedVideo = ::cv::Mat::zeros(cvVideoImage.rows, cvVideoImage.cols, cvVideoImage.type());
        cvVideoImage.copyTo(cvMaskedVideo, cvForegroundImage);

        ::fwData::Image::sptr foregroundImage = this->getInOut< ::fwData::Image >(s_FOREGROUND_IMAGE_KEY);
        ::cvIO::Image::copyFromCv(foregroundImage, cvMaskedVideo);

        auto sig = foregroundImage->signal< ::fwData::Image::BufferModifiedSignalType >(
            ::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::setBackground()
{
    ::fwData::Image::sptr maskImage  = this->getInOut< ::fwData::Image >(s_MASK_IMAGE_KEY);
    ::fwData::Image::sptr depthImage = this->getInOut< ::fwData::Image >(s_DEPTH_IMAGE_KEY);
    if(maskImage && depthImage)
    {
        ::cv::Mat cvDepthImage = ::cvIO::Image::moveToCv(depthImage);
        m_cvMaskImage          = ::cvIO::Image::moveToCv(maskImage);
        ::cv::cvtColor(m_cvMaskImage, m_cvMaskImage, cv::COLOR_BGRA2GRAY);

        m_cvMaskImage = (m_cvMaskImage > 0);

        if(m_cvDepthMaskImage.empty())
        {
            m_cvDepthMaskImage = ::cv::Mat::zeros(cvDepthImage.rows, cvDepthImage.cols, cvDepthImage.type());
        }
        cvDepthImage.copyTo(m_cvDepthMaskImage, m_cvMaskImage);
    }
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::setThreshold(int _threshold)
{
    m_threshold = _threshold;
}

// ------------------------------------------------------------------------------

} // namespace depthSegmentation

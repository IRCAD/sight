/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <cvIO/Image.hpp>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <services/macros.hpp>

#include <librealsense2/rs.hpp>

namespace depthSegmentation
{

static const core::com::Slots::SlotKeyType s_SET_BACKGROUND_SLOT = "setBackground";
static const core::com::Slots::SlotKeyType s_SET_THRESHOLD_SLOT  = "setThreshold";

static const services::IService::KeyType s_MASK_IMAGE_KEY       = "maskImage";
static const services::IService::KeyType s_VIDEO_IMAGE_KEY      = "videoImage";
static const services::IService::KeyType s_DEPTH_IMAGE_KEY      = "depthImage";
static const services::IService::KeyType s_FOREGROUND_IMAGE_KEY = "foregroundImage";

fwServicesRegisterMacro( ::sight::services::IOperator, ::depthSegmentation::SDepthImageMasking)

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

::services::IService::KeyConnectionsMap SDepthImageMasking::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_DEPTH_IMAGE_KEY, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::updating()
{
    if(!m_cvDepthMaskImage.empty())
    {
        const data::Image::csptr videoImage = this->getInput< data::Image >(s_VIDEO_IMAGE_KEY);
        const data::Image::csptr depthImage = this->getInput< data::Image >(s_DEPTH_IMAGE_KEY);

        if(videoImage && depthImage)
        {
            data::mt::ObjectReadLock lockVideoImage(videoImage);
            data::mt::ObjectReadLock lockDepthImage(depthImage);

            const ::cv::Mat cvVideoImage = ::cvIO::Image::moveToCv(videoImage);
            const ::cv::Mat cvDepthImage = ::cvIO::Image::moveToCv(depthImage);

            ::cv::Mat cvMaskedDepth;
            cvDepthImage.copyTo(cvMaskedDepth, m_cvMaskImage);

            ::cv::Mat cvForegroundImage = (cvMaskedDepth < (m_cvDepthMaskImage - m_threshold));

            ::cv::Mat morphElem = ::cv::getStructuringElement(::cv::MORPH_ELLIPSE, ::cv::Size(7, 7));
            ::cv::dilate(cvForegroundImage, cvForegroundImage, morphElem);
            ::cv::erode(cvForegroundImage, cvForegroundImage, morphElem);

            ::cv::Mat cvMaskedVideo = ::cv::Mat::zeros(cvVideoImage.rows, cvVideoImage.cols, cvVideoImage.type());
            cvVideoImage.copyTo(cvMaskedVideo, cvForegroundImage);

            data::Image::sptr foregroundImage = this->getInOut< data::Image >(s_FOREGROUND_IMAGE_KEY);

            data::mt::ObjectWriteLock lockForegroundImage(foregroundImage);

            ::cvIO::Image::copyFromCv(foregroundImage, cvMaskedVideo);

            const auto sig = foregroundImage->signal< data::Image::BufferModifiedSignalType >(
                data::Image::s_BUFFER_MODIFIED_SIG);
            sig->asyncEmit();

            m_sigComputed->asyncEmit();
        }
    }
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::setBackground()
{
    const data::Image::csptr maskImage  = this->getInput< data::Image >(s_MASK_IMAGE_KEY);
    const data::Image::csptr depthImage = this->getInput< data::Image >(s_DEPTH_IMAGE_KEY);
    if(maskImage && depthImage && (maskImage->getType() != core::tools::Type::s_UNSPECIFIED_TYPE) &&
       (depthImage->getType() != core::tools::Type::s_UNSPECIFIED_TYPE))
    {
        data::mt::ObjectReadLock lockMaskImage(maskImage);
        data::mt::ObjectReadLock lockDepthImage(depthImage);

        const ::cv::Mat cvDepthImage = ::cvIO::Image::moveToCv(depthImage);
        m_cvMaskImage = ::cvIO::Image::moveToCv(maskImage);
        if(m_cvMaskImage.channels() == 4)
        {
            ::cv::cvtColor(m_cvMaskImage, m_cvMaskImage, cv::COLOR_BGRA2GRAY);
        }
        else if(m_cvMaskImage.channels() == 3)
        {
            ::cv::cvtColor(m_cvMaskImage, m_cvMaskImage, cv::COLOR_BGR2GRAY);
        }

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

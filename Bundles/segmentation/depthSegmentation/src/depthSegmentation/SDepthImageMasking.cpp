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

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>

#include <librealsense2/rs.hpp>

namespace depthSegmentation
{

fwServicesRegisterMacro( ::fwServices::IOperator, ::depthSegmentation::SDepthImageMasking)

const ::fwServices::IService::KeyType s_MASK_IMAGE_KEY = "maskImage";
const ::fwServices::IService::KeyType s_VIDEO_IMAGE_KEY = "videoImage";
const ::fwServices::IService::KeyType s_DEPTH_IMAGE_KEY = "depthImage";

// ------------------------------------------------------------------------------

SDepthImageMasking::SDepthImageMasking() noexcept
{
    m_elemLess = ::cv::getStructuringElement(::cv::MORPH_RECT, ::cv::Size(4, 4), ::cv::Point(3, 3));
    m_elemMore = ::cv::getStructuringElement(::cv::MORPH_RECT, ::cv::Size(7, 7), ::cv::Point(6, 6));
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
    ::fwData::Image::sptr videoImage = this->getInOut< ::fwData::Image >(s_VIDEO_IMAGE_KEY);
    ::fwData::Image::sptr depthImage = this->getInOut< ::fwData::Image >(s_DEPTH_IMAGE_KEY);

    ::cv::Mat cvNearDepthImage = ::cvIO::Image::moveToCv(depthImage);
    ::cv::Mat cvVideoImage     = ::cvIO::Image::moveToCv(videoImage);
    ::cv::cvtColor(cvVideoImage, cvVideoImage, CV_BGRA2BGR);

    ::cv::Mat cvNearDepthImage8u;
    cvNearDepthImage.convertTo(cvNearDepthImage8u, CV_8UC1);

    ::cv::Mat cvFarDepthImage8u = cvNearDepthImage8u.clone();

    create_mask_from_depth(cvNearDepthImage8u, 180, ::cv::THRESH_BINARY);
    create_mask_from_depth(cvFarDepthImage8u, 100, ::cv::THRESH_BINARY_INV);

    ::cv::Mat mask(cvNearDepthImage.size(), CV_8UC1);
    mask.setTo(::cv::Scalar::all(::cv::GC_BGD));
    mask.setTo(::cv::GC_PR_BGD, cvFarDepthImage8u == 0);
    mask.setTo(::cv::GC_PR_FGD, cvNearDepthImage8u == 0);

    ::cv::Mat bgModel, fgModel;
    //::cv::grabCut(cvVideoImage, mask, ::cv::Rect(), bgModel, fgModel, 1, ::cv::GC_INIT_WITH_MASK);

    //::cv::Mat3b foreground = ::cv::Mat3b::zeros(cvVideoImage.rows, cvVideoImage.cols);
    //cvVideoImage.copyTo(foreground, (mask == ::cv::GC_FGD) | (mask == ::cv::GC_PR_FGD));

    ::fwData::Image::sptr maskImage = this->getInOut< ::fwData::Image >(s_MASK_IMAGE_KEY);
    ::cv::Mat cvMaskImage    = ::cvIO::Image::moveToCv(maskImage);

    ::cv::imshow("mask", cvMaskImage);
}

// ------------------------------------------------------------------------------

void SDepthImageMasking::create_mask_from_depth(::cv::Mat& _depth, int _thresh, ::cv::ThresholdTypes _type)
{
    ::cv::threshold(_depth, _depth, _thresh, 255, _type);
    //::cv::dilate(_depth, _depth, m_elemLess);
    //::cv::erode(_depth, _depth, m_elemMore);
}

} // namespace depthSegmentation

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/Camera.hpp"

namespace cvIO
{

//------------------------------------------------------------------------------

std::tuple< ::cv::Mat, ::cv::Size, ::cv::Mat> Camera::copyToCv(const arData::Camera::csptr& _src)
{
    ::cv::Mat intrinsic = ::cv::Mat::eye(3, 3, CV_64F);

    intrinsic.at<double>(0, 0) = _src->getFx();
    intrinsic.at<double>(1, 1) = _src->getFy();
    intrinsic.at<double>(0, 2) = _src->getCx();
    intrinsic.at<double>(1, 2) = _src->getCy();

    ::cv::Size imgSize;
    imgSize.width  = static_cast<int>(_src->getWidth());
    imgSize.height = static_cast<int>(_src->getHeight());

    ::cv::Mat distortionCoeffs = ::cv::Mat::zeros(5, 1, CV_64F);

    for (size_t i = 0; i < 5; ++i)
    {
        distortionCoeffs.at<double>(static_cast<int>(i)) = _src->getDistortionCoefficient()[i];
    }

    return std::make_tuple(intrinsic, imgSize, distortionCoeffs);
}

} //namespace cvIO

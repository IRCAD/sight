/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/Camera.hpp"

namespace cvIO
{

//------------------------------------------------------------------------------

void Camera::copyToCv(const arData::Camera::csptr& _src, cv::Mat& _intrinsic, cv::Size& _imgSize,
                      cv::Mat& _distortionCoeffs)
{
    _intrinsic = ::cv::Mat::eye(3, 3, CV_64F);

    _intrinsic.at<double>(0, 0) = _src->getFx();
    _intrinsic.at<double>(1, 1) = _src->getFy();
    _intrinsic.at<double>(0, 2) = _src->getCx();
    _intrinsic.at<double>(1, 2) = _src->getCy();

    _imgSize.width  = static_cast<int>(_src->getWidth());
    _imgSize.height = static_cast<int>(_src->getHeight());

    _distortionCoeffs = ::cv::Mat::zeros(5, 1, CV_64F);

    for (size_t i = 0; i < 5; ++i)
    {
        _distortionCoeffs.at<double>(static_cast<int>(i)) = _src->getDistortionCoefficient()[i];
    }
}

} //namespace cvIO

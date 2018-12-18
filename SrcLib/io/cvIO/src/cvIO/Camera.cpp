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

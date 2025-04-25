/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "camera.hpp"

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

std::tuple<cv::Mat, cv::Size, cv::Mat> camera::copy_to_cv(const data::camera::csptr& _src)
{
    cv::Mat intrinsic = cv::Mat::eye(3, 3, CV_64F);

    intrinsic.at<double>(0, 0) = _src->get_fx();
    intrinsic.at<double>(1, 1) = _src->get_fy();
    intrinsic.at<double>(0, 2) = _src->get_cx();
    intrinsic.at<double>(1, 2) = _src->get_cy();

    cv::Size img_size;
    img_size.width  = static_cast<int>(_src->get_width());
    img_size.height = static_cast<int>(_src->get_height());

    cv::Mat distortion_coeffs = cv::Mat::zeros(5, 1, CV_64F);

    for(std::size_t i = 0 ; i < 5 ; ++i)
    {
        distortion_coeffs.at<double>(static_cast<int>(i)) = _src->get_distortion_coefficient()[i];
    }

    return std::make_tuple(intrinsic, img_size, distortion_coeffs);
}

} //namespace sight::io::opencv

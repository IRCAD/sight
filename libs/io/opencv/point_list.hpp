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

#pragma once

#include "io/opencv/config.hpp"

#include <data/point_list.hpp>

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

/**
 * @brief Point and point list conversion methods.
 */
class IO_OPENCV_CLASS_API point_list
{
public:

    /// Copy the first two coordinates of the Sight point to the OpenCV point.
    IO_OPENCV_API static void copyToCv(const data::point::csptr& _src, cv::Point2d& _dst);

    /// Copy a Sight 2D point list to OpenCV.
    IO_OPENCV_API static void copyToCv(const data::point_list::csptr& _src, std::vector<cv::Point2d>& _dst);

    /// Convert OpenCV points to a Sight point list.
    IO_OPENCV_API static void copy_from_cv(const std::vector<cv::Point2d>& _src, const data::point_list::sptr& _dst);
};

} //namespace sight::io::opencv

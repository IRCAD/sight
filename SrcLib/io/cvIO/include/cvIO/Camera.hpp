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

#pragma once

#include "cvIO/config.hpp"

#include <arData/Camera.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief Camera calibration conversion methods.
 */
class CVIO_CLASS_API Camera
{
public:

    /// Copy the intrinsic matrix, the image size and the distortion coefficients from a f4s camera.
    CVIO_API static std::tuple< ::cv::Mat, ::cv::Size, ::cv::Mat> copyToCv(const ::arData::Camera::csptr& _src);
};

} // namespace cvIO

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

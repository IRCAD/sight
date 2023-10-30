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

#include <data/matrix4.hpp>

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

/**
 * @brief The matrix helper class contains static method to convert data::matrix4 to opencv Mat.
 */
class IO_OPENCV_CLASS_API matrix
{
public:

    /**
     * @brief copy_from_cv: copies values from OpenCV structure to Sight data.
     * @param _src [cv::Matx44f]: OpenCV 4x4 double matrix.
     * @param _dst [data::matrix4::sptr]: TransformationMatrix (need to be initialized).
     */
    IO_OPENCV_API static void copy_from_cv(const cv::Matx44d& _src, data::matrix4::sptr& _dst);

    /**
     * @brief copyToCv: copies values from Sight Matrix4 to OpenCV structure.
     * @param _src [data::matrix4::csptr]: Sight matrix to be copied.
     * @param _dst [cv::Matx44d]: OpenCV 4x4 double matrix.
     */
    IO_OPENCV_API static void copy_to_cv(const data::matrix4::csptr& _src, cv::Matx44d& _dst);

    /**
     * @brief copy_from_cv: copies values from OpenCV structure to Sight data.
     * @param _src [cv::Matx44f]: OpenCV 4x4 float matrix.
     * @param _dst [data::matrix4::sptr]: TransformationMatrix (need to be initialized).
     */
    IO_OPENCV_API static void copy_from_cv(const cv::Matx44f& _src, data::matrix4::sptr& _dst);

    /**
     * @brief copyToCv: copies values from Sight Matrix4 to OpenCV structure.
     * @param _src [data::matrix4::csptr]: Sight matrix to be copied.
     * @param _dst [cv::Matx44f]: OpenCV 4x4 float matrix.
     */
    IO_OPENCV_API static void copy_to_cv(const data::matrix4::csptr& _src, cv::Matx44f& _dst);

    /**
     * @brief copy_from_cv: copies values from OpenCV structure (rotation & translation matrix) to Sight data.
     * @param _rvec [cv::Mat]: OpenCV 1x3 rotation vector (cast in CV_64F).
     * @param _tvec [cv::Mat]: OpenCV 1x3 translation vector (cast in CV_64F).
     * @param _dst [data::matrix4::sptr]: TransformationMatrix (need to be initialized).
     */
    IO_OPENCV_API static void copy_from_cv(
        const cv::Mat& _rvec,
        const cv::Mat& _tvec,
        data::matrix4::sptr& _dst
    );

    /**
     * @brief copyToCv copies values from Sight Matrix4 to OpenCV structures.
     * @param _src [data::matrix4::csptr]: Sight matrix to be copied.
     * @param _rvec [cv::Mat]: OpenCV 1x3 rotation vector of type CV_64F.
     * @param _tvec [cv::Mat]: OpenCV 1x3 translation vector of type CV_64F.
     */
    IO_OPENCV_API static void copy_to_cv(
        const data::matrix4::csptr& _src,
        cv::Mat& _rvec,
        cv::Mat& _tvec
    );

    /**
     * @brief copy_from_cv copies values from OpenCV structure to Sight data.
     * @param _src [cv::Mat] OpenCV 4x4 matrix (cast in CV_64F).
     * @param _dst [data::matrix4::sptr]:TransformationMatrix (need to be initialized).
     */
    IO_OPENCV_API static void copy_from_cv(const cv::Mat& _src, data::matrix4::sptr& _dst);

    /**
     * @brief copyToCv: copies values from Sight Matrix4 to OpenCV structure.
     * @param _src [data::matrix4::csptr]: Sight matrix to be copied.
     * @param _dst [cv::Mat]: OpenCV 4x4 matrix of type CV_64F.
     */
    IO_OPENCV_API static void copy_to_cv(const data::matrix4::csptr& _src, cv::Mat& _dst);
};

} //namespace sight::io::opencv

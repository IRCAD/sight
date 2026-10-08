/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <sight/io/opencv/config.hpp>

#include <data/matrix4.hpp>

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

/**
 * @brief The matrix helper class contains static method to convert data::matrix4 to opencv Mat.
 */
class SIGHT_IO_OPENCV_CLASS_API matrix
{
public:

    /**
     * @brief copy_from_cv: copies values from OpenCV structure to Sight data.
     * @param _src [cv::Matx<T, 4, 4>]: OpenCV 4x4 matrix of type T.
     * @param _dst [data::matrix4&]: TransformationMatrix (need to be initialized).
     */
    template<typename T = float>
    static void copy_from_cv(const cv::Matx<T, 4, 4>& _src, data::matrix4& _dst);

    /**
     * @brief copy_from_cv: copies values from OpenCV structure (rotation & translation matrix) to Sight data.
     * @param _rvec [cv::Mat]: OpenCV 1x3 rotation vector (cast in CV_64F).
     * @param _tvec [cv::Mat]: OpenCV 1x3 translation vector (cast in CV_64F).
     * @param _dst [data::matrix4&]: TransformationMatrix (need to be initialized).
     */
    SIGHT_IO_OPENCV_API static void copy_from_cv(const cv::Mat& _rvec, const cv::Mat& _tvec, data::matrix4& _dst);

    /**
     * @brief copy_from_cv copies values from OpenCV structure to Sight data.
     * @param _src [cv::Mat] OpenCV 4x4 matrix (cast in CV_64F).
     * @param _dst [data::matrix4&]:TransformationMatrix (need to be initialized).
     */
    SIGHT_IO_OPENCV_API static void copy_from_cv(const cv::Mat& _src, data::matrix4& _dst);

    /**
     * @brief copy_to_cv: copies values from Sight Matrix4 to OpenCV structure.
     * @param _src [data::matrix4]: Sight matrix to be copied.
     * @tparam T: OpenCV matrix type (e.g., CV_32F, CV_64F).
     * @return [cv::Mat]: OpenCV 4x4 matrix of type T.
     */
    template<typename T = float>
    static cv::Mat copy_to_cv(const data::matrix4& _src);

    /**
     * @brief copyToCv copies values from Sight Matrix4 to OpenCV structures.
     * @param _src [data::matrix4::csptr]: Sight matrix to be copied.
     * @param _rvec [cv::Mat]: OpenCV 1x3 rotation vector of type CV_64F.
     * @param _tvec [cv::Mat]: OpenCV 1x3 translation vector of type CV_64F.
     */
    SIGHT_IO_OPENCV_API static void copy_to_cv(const data::matrix4& _src, cv::Mat& _rvec, cv::Mat& _tvec);
};

//-----------------------------------------------------------------------------

template<typename T>
void matrix::copy_from_cv(const cv::Matx<T, 4, 4>& _src, data::matrix4& _dst)
{
    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            _dst(i, j) = static_cast<T>(_src(i, j));
        }
    }
}

//-----------------------------------------------------------------------------

template<typename T>
inline cv::Mat matrix::copy_to_cv(const data::matrix4& _src)
{
    cv::Mat dst = cv::Mat::zeros(4, 4, cv::DataType<T>::type);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            dst.at<typename cv::DataType<T>::value_type>(i, j) = static_cast<cv::DataType<T>::value_type>(_src(i, j));
        }
    }

    return dst;
}

} //namespace sight::io::opencv

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

#include "matrix.hpp"

#include <opencv2/calib3d.hpp>

namespace sight::io::opencv
{

//-----------------------------------------------------------------------------

void matrix::copy_from_cv(const cv::Matx44d& _src, data::matrix4::sptr& _dst)
{
    SIGHT_ASSERT("sight::data::matrix4 is null", _dst);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            (*_dst)(i, j) = _src(i, j);
        }
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_to_cv(const data::matrix4::csptr& _src, cv::Matx44d& _dst)
{
    SIGHT_ASSERT("sight::data::matrix4 is null", _src);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            _dst(i, j) = (*_src)(i, j);
        }
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_from_cv(const cv::Matx44f& _src, data::matrix4::sptr& _dst)
{
    SIGHT_ASSERT("sight::data::matrix4 is null", _dst);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            (*_dst)(i, j) = static_cast<double>(_src(i, j));
        }
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_to_cv(const data::matrix4::csptr& _src, cv::Matx44f& _dst)
{
    SIGHT_ASSERT("sight::data::matrix4 is null", _src);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            _dst(i, j) = static_cast<float>((*_src)(i, j));
        }
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_from_cv(const cv::Mat& _rvec, const cv::Mat& _tvec, data::matrix4::sptr& _dst)
{
    // Check validity of _rvec & _tvec.
    SIGHT_ASSERT("Rotation vector 'rvec' should be of size 1x3", _rvec.size().width == 1 && _rvec.size().height == 3);
    SIGHT_ASSERT("Rotation vector 'tvec' should be of size 1x3", _tvec.size().width == 1 && _tvec.size().height == 3);
    SIGHT_ASSERT(
        "Could not convert opencv matrix with multi-channels",
        _rvec.channels() == 1 && _tvec.channels() == 1
    );
    // Check that _dst has correctly been initialized.
    SIGHT_ASSERT("sight::data::matrix4 is null", _dst);

    SIGHT_WARN_IF(
        "OpenCV matrices has non-double type but will be cast in 'double'.",
        _rvec.type() != CV_64F || _tvec.type() != CV_64F
    );

    cv::Mat d_rvec;
    cv::Mat d_tvec;
    _rvec.convertTo(d_rvec, CV_64F);
    _tvec.convertTo(d_tvec, CV_64F);

    cv::Mat r_mat;
    cv::Rodrigues(d_rvec, r_mat);

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            (*_dst)(i, j) = r_mat.at<double>(i, j);
        }

        (*_dst)(i, 3) = d_tvec.at<double>(i);
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_to_cv(const data::matrix4::csptr& _src, cv::Mat& _rvec, cv::Mat& _tvec)
{
    // Check that _src is not null.
    SIGHT_ASSERT("sight::data::matrix4 is null", _src);

    cv::Mat mat4x4;
    // reinitialize _R & _T.
    _rvec = cv::Mat(1, 3, CV_64F);
    _tvec = cv::Mat(1, 3, CV_64F);

    // First convert Sight to OpenCV 4x4 matrix.
    copy_to_cv(_src, mat4x4);

    // Extract translation (_tvec) from mat4x4.
    _tvec = mat4x4(cv::Rect(3, 0, 1, 3));

    // Convert to rotation vector.
    cv::Rodrigues(mat4x4(cv::Rect(0, 0, 3, 3)), _rvec);
}

//-----------------------------------------------------------------------------

void matrix::copy_from_cv(const cv::Mat& _src, data::matrix4::sptr& _dst)
{
    SIGHT_ASSERT("Size of OpenCV matrix should be 4x4", _src.cols == 4 && _src.rows == 4);
    SIGHT_ASSERT("Could not convert opencv matrix with multi-channels", _src.channels() == 1);
    SIGHT_ASSERT("sight::data::matrix4 is null", _dst);

    SIGHT_WARN_IF(
        "OpenCV matrix has non-double type but will be cast in 'double'.",
        _src.type() != CV_64F
    );

    cv::Mat dmat;
    _src.convertTo(dmat, CV_64F);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            (*_dst)(i, j) = dmat.at<double>(i, j);
        }
    }
}

//-----------------------------------------------------------------------------

void matrix::copy_to_cv(const data::matrix4::csptr& _src, cv::Mat& _dst)
{
    SIGHT_ASSERT("sight::data::matrix4 is null", _src);

    // Reinitialize _dst.
    _dst = cv::Mat(4, 4, CV_64F);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            _dst.at<double>(i, j) = (*_src)(i, j);
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace sight::io::opencv

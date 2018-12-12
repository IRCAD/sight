/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "calibration3d/ReprojectionError.hpp"

namespace calibration3d
{

//-----------------------------------------------------------------------------

ReprojectionError::ReprojectionError(const ::cv::Mat& _cameraMat, const ::cv::Mat& _distCoef,
                                     const ::cv::Point2f& _imagePoints, const ::cv::Point3f& _objectPoints,
                                     const ::cv::Mat& _extrinsic) :
    m_imagePoint(_imagePoints),
    m_objectPoint(_objectPoints),
    m_extrinsic(_extrinsic),
    m_cameraMatrix(_cameraMat),
    m_distCoef(_distCoef)

{

}

//-----------------------------------------------------------------------------

::ceres::CostFunction* ReprojectionError::Create(const ::cv::Mat& _cameraMatrix,
                                                 const ::cv::Mat& _distCoef,
                                                 const ::cv::Point2f& _imagePoints,
                                                 const ::cv::Point3f& _objectPoints,
                                                 const ::cv::Mat& _extrinsic)
{
    ReprojectionError* rep = new ReprojectionError(_cameraMatrix, _distCoef, _imagePoints, _objectPoints, _extrinsic);
    return (new ::ceres::NumericDiffCostFunction<ReprojectionError, ::ceres::FORWARD, 2, 6>(rep));
}

//-----------------------------------------------------------------------------

}// namespace calibration3d

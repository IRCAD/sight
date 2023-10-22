/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "ReprojectionError.hpp"

namespace sight::geometry::vision::detail
{

//-----------------------------------------------------------------------------

ReprojectionError::ReprojectionError(
    cv::Mat _camera_mat,
    cv::Mat _dist_coef,
    cv::Point2f _image_points,
    cv::Point3f _object_points,
    cv::Mat _extrinsic
) :
    m_imagePoint(std::move(_image_points)),
    m_objectPoint(std::move(_object_points)),
    m_extrinsic(std::move(_extrinsic)),
    m_cameraMatrix(std::move(_camera_mat)),
    m_distCoef(std::move(_dist_coef))
{
}

//-----------------------------------------------------------------------------

::ceres::CostFunction* ReprojectionError::Create(
    const cv::Mat& _camera_matrix,
    const cv::Mat& _dist_coef,
    const cv::Point2f& _image_points,
    const cv::Point3f& _object_points,
    const cv::Mat& _extrinsic
)
{
    auto* rep = new ReprojectionError(_camera_matrix, _dist_coef, _image_points, _object_points, _extrinsic);
    return new ::ceres::NumericDiffCostFunction<ReprojectionError, ::ceres::FORWARD, 2, 6>(rep);
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::vision::detail

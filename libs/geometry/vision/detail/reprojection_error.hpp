/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#pragma once

#include <sight/geometry/vision/config.hpp>

#ifdef WIN32
// Needed for Ceres with C++20
    #define M_2_SQRTPI 1.12837916709551257390
#endif

#include <ceres/ceres.h>

#include <opencv2/opencv.hpp>

namespace sight::geometry::vision::detail
{

/**
 * @brief The ReprojectionError is used to compute reprojection error in a non-linear least squares problem.
 * this class is used as functor in a ::ceres::Problem.
 * @see http://ceres-solver.org/nnls_tutorial.html#hello-world
 */
class SIGHT_GEOMETRY_VISION_CLASS_API reprojection_error
{
public:

    /**
     * @brief ReprojectionError constructor
     * @param _camera_mat: camera matrix 3x3 (with Fx, Fy, Cx, Cy)
     * @param _dist_coef: distortion coefficients
     * @param _image_points: a 2d point in the image
     * @param _object_points: the corresponding 3d point
     * @param _extrinsic    extrinsic matrix
     */
    SIGHT_GEOMETRY_VISION_API reprojection_error(
        cv::Mat _camera_mat,
        cv::Mat _dist_coef,
        cv::Point2f _image_points,
        cv::Point3f _object_points,
        cv::Mat _extrinsic
    );
    /**
     * @brief operator() is a templated method, which assumes that all its inputs and outputs are of some type T.
     *  The use of templating here allows Ceres to call CostFunctor::operator<T>(),
     * with T=double when just the value of the residual is needed,
     * and with a special type T=Jet when the Jacobians are needed
     * @param _pose         inputs parameters (here 3 rotations and 3 translations)
     * @param _residuals    differences in x and y between a 2d point and it's reprojection.
     */
    template<typename T>
    bool operator()(const T* _pose, T* _residuals) const;

    /**
     * @brief factory to hide the construction of the CostFunction object from the client code.
     * @param _camera_matrix camera matrix 3x3 (with Fx, Fy, Cx, Cy)
     * @param _dist_coef distortion coefficients
     * @param _image_points: a 2d point in the image
     * @param _object_points: the corresponding 3d point
     * @param _extrinsic    the extrinsic matrix
     * @return a pointer to a ::ceres::CostFunction
     */

    SIGHT_GEOMETRY_VISION_API static ::ceres::CostFunction* create(
        const cv::Mat& _camera_matrix,
        const cv::Mat& _dist_coef,
        const cv::Point2f& _image_points,
        const cv::Point3f& _object_points,
        const cv::Mat& _extrinsic
    );

private:

    cv::Point2f m_image_point;  ///< point on image
    cv::Point3f m_object_point; ///< corresponding point in scene
    cv::Mat m_extrinsic;        ///< extrinsic matrix
    cv::Mat m_camera_matrix;    ///< camera calibration (Fx, Fy, Cx, Cy)
    cv::Mat m_dist_coef;        ///< camera distorsion
};

//-----------------------------------------------------------------------------

template<typename T>
bool reprojection_error::operator()(const T* const _pose, T* _residuals) const
{
    //Use OpenCV template structures since this operator is templated.
    // Conversion to OpenCv Mat
    const cv::Mat rvec_pose = (cv::Mat_<T>(3, 1) << _pose[0], _pose[1], _pose[2]);
    const cv::Mat tvec_pose = (cv::Mat_<T>(3, 1) << _pose[3], _pose[4], _pose[5]);
    cv::Mat rot_mat_pose    = cv::Mat_<T>(3, 3);

    cv::Rodrigues(rvec_pose, rot_mat_pose); // rotation vector to matrix.

    cv::Mat transform_pose = cv::Mat_<T>::eye(4, 4);

    // Copy in transform_pose
    transform_pose(cv::Range(0, 3), cv::Range(0, 3)) = rot_mat_pose * 1;
    transform_pose(cv::Range(0, 3), cv::Range(3, 4)) = tvec_pose * 1;

    // compute real pose (extrinsic mat * pose)
    // Note: extrinsic can be identity if we use reference camera
    const cv::Mat transform_pose_extrinsic = m_extrinsic * transform_pose;

    //matrix to rotation vector.
    cv::Mat rvec_pose_extrinsic;
    cv::Rodrigues(transform_pose_extrinsic(cv::Range(0, 3), cv::Range(0, 3)), rvec_pose_extrinsic);

    std::vector<cv::Point_<T> > point_reprojected(1); // 2d point
    const std::vector<cv::Point3_<T> > point3d_vector = {{
        {T(m_object_point.x),
         T(m_object_point.y),
         T(m_object_point.z)
        }
    }
    };

    // Reproject the point with new pose
    cv::projectPoints(
        point3d_vector,
        rvec_pose_extrinsic,
        transform_pose_extrinsic(cv::Range(0, 3), cv::Range(3, 4)),
        m_camera_matrix,
        m_dist_coef,
        point_reprojected
    );

    // The error is the difference between the predicted and observed position.
    _residuals[0] = T(m_image_point.x) - T(point_reprojected[0].x);
    _residuals[1] = T(m_image_point.y) - T(point_reprojected[0].y);

    return true;
}

//-----------------------------------------------------------------------------

} //namespace sight::geometry::vision::detail

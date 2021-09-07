/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "geometry/vision/config.hpp"

#include <ceres/ceres.h>

#include <opencv2/opencv.hpp>

namespace sight::geometry::vision
{

/**
 * @brief The ReprojectionError is used to compute reprojection error in a non-linear least squares problem.
 * this class is used as functor in a ::ceres::Problem.
 * @see http://ceres-solver.org/nnls_tutorial.html#hello-world
 */
class GEOMETRY_VISION_CLASS_API ReprojectionError
{
public:

    /**
     * @brief ReprojectionError constructor
     * @param _cameraMat: camera matrix 3x3 (with Fx, Fy, Cx, Cy)
     * @param _distCoef: distorsion coefficients
     * @param _imagePoints: a 2d point in the image
     * @param _objectPoints: the coressponding 3d point
     * @param _rotMat: rotation matrix
     * @param _tvec: translation vector
     */
    GEOMETRY_VISION_API ReprojectionError(
        const ::cv::Mat& _cameraMat,
        const ::cv::Mat& _distCoef,
        const ::cv::Point2f& _imagePoints,
        const ::cv::Point3f& _objectPoints,
        const ::cv::Mat& _extrinsic
    );
    /**
     * @brief operator() is a templated method, which assumes that all its inputs and outputs are of some type T.
     *  The use of templating here allows Ceres to call CostFunctor::operator<T>(),
     * with T=double when just the value of the residual is needed,
     * and with a special type T=Jet when the Jacobians are needed
     * @param pose: inputs parameters (here 3 rotations and 3 translations)
     * @param residuals: differences in x and y between a 2d point and it's reprojection.
     */
    template<typename T>
    bool operator()(const T* const pose, T* residuals) const;

    /**
     * @brief Factory to hide the construction of the CostFunction object from the client code.
     * @param _cameraMat: camera matrix 3x3 (with Fx, Fy, Cx, Cy)
     * @param _distCoef: distorsion coefficients
     * @param _imagePoints: a 2d point in the image
     * @param _objectPoints: the coressponding 3d point
     * @param _rotMat: rotation matrix
     * @param _tvec: translation vector
     * @return a pointer to a ::ceres::CostFunction
     */

    GEOMETRY_VISION_API static ::ceres::CostFunction* Create(
        const ::cv::Mat& _cameraMatrix,
        const ::cv::Mat& _distCoef,
        const ::cv::Point2f& _imagePoints,
        const ::cv::Point3f& _objectPoints,
        const ::cv::Mat& _extrinsic
    );

private:

    ::cv::Point2f m_imagePoint;  ///< point on image
    ::cv::Point3f m_objectPoint; ///< corresponding point in scene
    ::cv::Mat m_extrinsic;       ///< extrinsic matrix
    ::cv::Mat m_cameraMatrix;    ///< camera calibration (Fx, Fy, Cx, Cy)
    ::cv::Mat m_distCoef;        ///< camera distorsion
};

//-----------------------------------------------------------------------------

template<typename T>
bool ReprojectionError::operator()(const T* const pose, T* residuals) const
{
    //Use OpenCV template structures since this operator is templated.
    // Conversion to OpenCv Mat
    const ::cv::Mat rvecPose = (::cv::Mat_<T>(3, 1) << pose[0], pose[1], pose[2]);
    const ::cv::Mat tvecPose = (::cv::Mat_<T>(3, 1) << pose[3], pose[4], pose[5]);
    ::cv::Mat rotMatPose     = ::cv::Mat_<T>(3, 3);

    ::cv::Rodrigues(rvecPose, rotMatPose); // rotation vector to matrix.

    ::cv::Mat transformPose = ::cv::Mat_<T>::eye(4, 4);

    // Copy in transform_pose
    transformPose(::cv::Range(0, 3), ::cv::Range(0, 3)) = rotMatPose * 1;
    transformPose(::cv::Range(0, 3), ::cv::Range(3, 4)) = tvecPose * 1;

    // compute real pose (extrinsic mat * pose)
    // Note: extrinsic can be identity if we use reference camera
    const ::cv::Mat transformPoseExtrinsic = m_extrinsic * transformPose;

    //matrix to rotation vector.
    ::cv::Mat rvecPoseExtrinsic;
    ::cv::Rodrigues(transformPoseExtrinsic(::cv::Range(0, 3), ::cv::Range(0, 3)), rvecPoseExtrinsic);

    std::vector< ::cv::Point_<T> > pointReprojected(1); // 2d point
    const std::vector< ::cv::Point3_<T> > point3dVector = {{
        {T(m_objectPoint.x),
         T(m_objectPoint.y),
         T(m_objectPoint.z)
        }
    }
    };

    // Reproject the point with new pose
    ::cv::projectPoints(
        point3dVector,
        rvecPoseExtrinsic,
        transformPoseExtrinsic(::cv::Range(0, 3), ::cv::Range(3, 4)),
        m_cameraMatrix,
        m_distCoef,
        pointReprojected
    );

    // The error is the difference between the predicted and observed position.
    residuals[0] = T(m_imagePoint.x) - T(pointReprojected[0].x);
    residuals[1] = T(m_imagePoint.y) - T(pointReprojected[0].y);

    return true;
}

//-----------------------------------------------------------------------------

} //namespace sight::geometry::vision

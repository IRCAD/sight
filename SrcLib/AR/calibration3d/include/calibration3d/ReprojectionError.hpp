/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATION3D_REPROJECTIONERROR_HPP__
#define __CALIBRATION3D_REPROJECTIONERROR_HPP__

#include "calibration3d/config.hpp"

#include <ceres/ceres.h>

#include <opencv2/opencv.hpp>

namespace calibration3d
{
/**
 * @brief The ReprojectionError is used to compute reprojection error in a non-linear least squares problem.
 * this class is used as functor in a ::ceres::Problem.
 * @see http://ceres-solver.org/nnls_tutorial.html#hello-world
 */
class CALIBRATION3D_CLASS_API ReprojectionError
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
    CALIBRATION3D_API ReprojectionError( const ::cv::Mat& _cameraMat, const ::cv::Mat& _distCoef,
                                         const ::cv::Point2f& _imagePoints, const ::cv::Point3f& _objectPoints,
                                         const ::cv::Mat& _rotMat, const ::cv::Mat& _tvec);
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

    static ::ceres::CostFunction* Create(const cv::Mat& _cameraMatrix,
                                         const cv::Mat& _distCoef,
                                         const cv::Point2f& _imagePoints,
                                         const cv::Point3f& _objectPoints,
                                         const cv::Mat& _rotMat,
                                         const cv::Mat& _tvec)
    {
        return (new ::ceres::NumericDiffCostFunction<ReprojectionError, ::ceres::FORWARD, 2, 6>(
                    new ReprojectionError(_cameraMatrix, _distCoef, _imagePoints, _objectPoints, _rotMat, _tvec)));
    }

private:

    ::cv::Point2f m_imagePoint;  ///< point on image
    ::cv::Point3f m_objectPoint;  ///< corresponding point in scene
    ::cv::Mat m_rotMat;           ///< extrinsic calib rotation
    ::cv::Mat m_tvec;         ///< extrinsic calib translation
    ::cv::Mat m_cameraMatrix; ///< camera calibration (Fx, Fy, Cx, Cy)
    ::cv::Mat m_distCoef; ///< camera distorsion

};

//-----------------------------------------------------------------------------

template< typename T>
bool ReprojectionError::operator()(const T* const pose, T* residuals) const
{
    //Use OpenCV template structures since this operator is templated.
    // Conversion to OpenCv Mat
    ::cv::Mat rvec_pose   = (::cv::Mat_<T>(3, 1) << pose[0], pose[1], pose[2]);
    ::cv::Mat tvec_pose   = (::cv::Mat_<T>(3, 1) << pose[3], pose[4], pose[5]);
    ::cv::Mat rotMat_pose = ::cv::Mat_<T>::eye(3, 3);

    ::cv::Rodrigues(rvec_pose, rotMat_pose); // rotation vector to matrix.

    ::cv::Mat transform_pose = ::cv::Mat_<T>::eye(4, 4); //identity

    //Copy in transform_pose
    transform_pose( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = rotMat_pose * 1;
    transform_pose( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = tvec_pose * 1;

    ::cv::Mat extrinsic = ::cv::Mat_<T>::eye(4, 4); //identity

    //Copy in extrinsic
    extrinsic( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = m_rotMat * 1;
    extrinsic( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = m_tvec * 1;

    // compute real pose (extrinsic mat * pose)
    //Note: extrinsic can be identity if we use reference camera
    ::cv::Mat transform_poseExtrinsic = extrinsic * transform_pose;

    ::cv::Mat rotMat_poseExtrinsic = ::cv::Mat_<T>::eye(3, 3); //identity
    //Copy in rotMat_poseExtrinsic
    rotMat_poseExtrinsic = transform_poseExtrinsic(::cv::Range(0, 3), ::cv::Range(0, 3));

    ::cv::Mat tvec_poseExtrinsic = ::cv::Mat_<T>::zeros(3, 1); //zeros
    //Copy values in tvec_poseExtrinsic
    tvec_poseExtrinsic.at<T>(0) = transform_poseExtrinsic.at<T>(0, 3);
    tvec_poseExtrinsic.at<T>(1) = transform_poseExtrinsic.at<T>(1, 3);
    tvec_poseExtrinsic.at<T>(2) = transform_poseExtrinsic.at<T>(2, 3);

    ::cv::Mat rvec_poseExtrinsic;
    ::cv::Rodrigues(rotMat_poseExtrinsic, rvec_poseExtrinsic); //matrix to rotation vector.

    std::vector< ::cv::Point_<T> > point_reprojected(1);// 2d point
    std::vector< ::cv::Point3_<T> > point3d_vector(1); //3d object point
    point3d_vector[0].x = T(m_objectPoint.x);
    point3d_vector[0].y = T(m_objectPoint.y);
    point3d_vector[0].z = T(m_objectPoint.z);

    //reproject the point with new pose
    ::cv::projectPoints(point3d_vector, rvec_poseExtrinsic, tvec_poseExtrinsic, m_cameraMatrix,
                        m_distCoef,
                        point_reprojected);

    // The error is the difference between the predicted and observed position.
    residuals[0] = T(m_imagePoint.x) - T(point_reprojected[0].x);
    residuals[1] = T(m_imagePoint.y) - T(point_reprojected[0].y);

    return true;
}

//-----------------------------------------------------------------------------

} //namespace calibration3d
#endif //__CALIBRATION3D_REPROJECTIONERROR_HPP__

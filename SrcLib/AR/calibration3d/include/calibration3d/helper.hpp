/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATION3D_HELPER_HPP__
#define __CALIBRATION3D_HELPER_HPP__

#include "calibration3d/config.hpp"

#include <opencv2/calib3d.hpp>

#include <vector>

namespace calibration3d
{

/**
 * @brief helper contains useful functions related to calibration or 3d reconstruction.
 */
namespace helper
{

typedef std::pair< double, std::vector< ::cv::Point2f > > ErrorAndPointsType;

/**
 * @brief compute the mean error of reprojection
 * @param _objectPoints : vector of 3d points (to be reporjected)
 * @param _imagePoints: vector of 2d points (reference)
 * @param _rvecs: rotation vector
 * @param _tvecs: translation vector
 * @param _cameraMatrix: camera matrix (fx, fy, cx, cy)
 * @param _distCoeffs: distorsion coefficients of the camera
 * @return std::pair< double, std::vector< ::cv::Point2f > >, first element is the arithmetical root-squared mean error
 * and the second element is a vector of reprojected 2d points (for display purpose)
 */
CALIBRATION3D_API ErrorAndPointsType computeReprojectionError( const std::vector< ::cv::Point3f >& _objectPoints,
                                                               const std::vector< ::cv::Point2f >& _imagePoints,
                                                               const ::cv::Mat& _rvecs, const ::cv::Mat& _tvecs,
                                                               const ::cv::Mat& _cameraMatrix,
                                                               const ::cv::Mat& _distCoeffs);

/**
 * @brief cameraPoseMonocular compute pose from monocular camera
 * @param _objectPoints: vector of 3d points corresponding to the object
 * @param _imagePoints: vector of 2d points
 * @param _cameraMatrix: camera matrix (fx, fy, cx, cy)
 * @param _distCoeffs: distorsion coefficients of the camera
 * @param _flag: ::cv::solvePnP method to use
 * @return ::cv::Mat (4x4 with float values) corresponding to the camera pose
 */
CALIBRATION3D_API ::cv::Matx44f cameraPoseMonocular(const std::vector< ::cv::Point3f >& _objectPoints,
                                                    const std::vector< ::cv::Point2f >& _imagePoints,
                                                    const ::cv::Mat _cameraMatrix,
                                                    const ::cv::Mat& _distCoeffs,
                                                    const int _flag = CV_ITERATIVE);

/**
 * @brief cameraPoseStereo compute pose from stereo camera
 * @param _objectPoints: vector of 3d points corresponding to the object
 * @param _cameraMatrix1: camera matrix (fx, fy, cx, cy) for reference camera
 * @param _distCoeffs1: distorsion coefficients of camera1
 * @param _cameraMatrix2: camera matrix (fx, fy, cx, cy) for second camera
 * @param _distCoeffs2: distorsion coefficients of camera2
 * @param _imgPoints1: vector of 2d points in camera 1
 * @param _imgPoints2: vector of 2d points in camera 2
 * @param _R: rotation matrix from camera1 to camera2
 * @param _T: translation vector from camera1 to camera2
 * @return
 */
CALIBRATION3D_API ::cv::Matx44f cameraPoseStereo(const std::vector< ::cv::Point3f>& _objectPoints,
                                                 const ::cv::Mat& _cameraMatrix1, const ::cv::Mat& _distCoeffs1,
                                                 const ::cv::Mat& _cameraMatrix2, const ::cv::Mat& _distCoeffs2,
                                                 const std::vector< ::cv::Point2f >& _imgPoints1,
                                                 const std::vector< ::cv::Point2f >& _imgPoints2, const ::cv::Mat& _R,
                                                 const ::cv::Mat& _T);

}

} //namespace calibration3d

#endif //__CALIBRATION3D_HELPER_HPP__

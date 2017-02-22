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
 *
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
 * @return double arithmetical mean error
 */
CALIBRATION3D_API ErrorAndPointsType computeReprojectionError( const std::vector< ::cv::Point3f >& _objectPoints,
                                                               const std::vector< ::cv::Point2f >& _imagePoints,
                                                               const ::cv::Mat& _rvecs, const ::cv::Mat& _tvecs,
                                                               const ::cv::Mat& _cameraMatrix,
                                                               const ::cv::Mat& _distCoeffs);

}

} //namespace calibration3d

#endif //__CALIBRATION3D_HELPER_HPP__

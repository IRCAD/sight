/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

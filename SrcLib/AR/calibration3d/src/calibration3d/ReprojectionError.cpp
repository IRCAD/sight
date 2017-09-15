/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "calibration3d/ReprojectionError.hpp"

namespace calibration3d
{

//-----------------------------------------------------------------------------

ReprojectionError::ReprojectionError(const ::cv::Mat& _cameraMat, const ::cv::Mat& _distCoef,
                                     const ::cv::Point2f& _imagePoints, const ::cv::Point3f& _objectPoints,
                                     const ::cv::Mat& _rotMat, const ::cv::Mat& _tvec) :
    m_imagePoint(_imagePoints),
    m_objectPoint(_objectPoints),
    m_rotMat(_rotMat),
    m_tvec(_tvec),
    m_cameraMatrix(_cameraMat),
    m_distCoef(_distCoef)

{

}

//-----------------------------------------------------------------------------

}// namespace calibration3d

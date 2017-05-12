/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "calibration3d/helper.hpp"

#include "fwCore/spyLog.hpp"

namespace calibration3d
{
namespace helper
{

//-----------------------------------------------------------------------------

ErrorAndPointsType computeReprojectionError(const std::vector< ::cv::Point3f >& _objectPoints,
                                            const std::vector< ::cv::Point2f >& _imagePoints,
                                            const ::cv::Mat& _rvecs, const ::cv::Mat& _tvecs,
                                            const ::cv::Mat& _cameraMatrix, const ::cv::Mat& _distCoeffs)
{

    ErrorAndPointsType errorAndProjectedPoints;
    std::vector< ::cv::Point2f > imagePoints2;
    int totalPoints = 0;
    double totalErr = 0, err;

    //projection
    ::cv::projectPoints( ::cv::Mat(_objectPoints), _rvecs, _tvecs, _cameraMatrix,
                         _distCoeffs, imagePoints2);

    errorAndProjectedPoints.second = imagePoints2;

    //difference
    err = ::cv::norm(::cv::Mat(_imagePoints), ::cv::Mat(imagePoints2), CV_L2);

    int n = static_cast<int>(_objectPoints.size());
    totalErr    += err*err;
    totalPoints += n;

    errorAndProjectedPoints.first = std::sqrt(totalErr/totalPoints);
    return errorAndProjectedPoints;
}

//-----------------------------------------------------------------------------

::cv::Matx44f cameraPoseMonocular(const std::vector< ::cv::Point3f>& _objectPoints,
                                  const std::vector< ::cv::Point2f>& _imagePoints, const ::cv::Mat _cameraMatrix,
                                  const ::cv::Mat& _distCoeffs, const int _flag)
{

    SLM_ASSERT("There should be the same number of 3d points than 2d points",
               _objectPoints.size() == _imagePoints.size());

    ::cv::Mat rvec, tvec, R, T;
    T = ::cv::Mat::eye(4, 4, CV_64F);

    //solvePnP
    ::cv::solvePnP(_objectPoints, _imagePoints, _cameraMatrix, _distCoeffs, rvec, tvec, false, _flag);

    // to matrix
    ::cv::Rodrigues(rvec, R); // R is 3x3

    T( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = R * 1; // copies R into T
    T( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = tvec * 1; // copies tvec into T

    return ::cv::Matx44f(T);

}

//-----------------------------------------------------------------------------

::cv::Matx44f cameraPoseStereo(const ::cv::Mat _cameraMatrix1, const ::cv::Mat& _distCoeffs1,
                               const ::cv::Mat _cameraMatrix2, const ::cv::Mat& _distCoeffs2,
                               const std::vector< ::cv::Point2f >& _imgPoints1,
                               const std::vector< ::cv::Point2f >& _imgPoints2,
                               const ::cv::Size _imgSize, const ::cv::Mat _R, const ::cv::Mat _T)
{

    ::cv::Mat R1, R2, P1, P2, Q;
    ::cv::stereoRectify(_cameraMatrix1, _distCoeffs1, _cameraMatrix2, _distCoeffs2, _imgSize, _R, _T, R1, R2, P1, P2,
                        Q);

    SLM_ASSERT("There should be the same number of points in the two views",
               _imgPoints1.size() == _imgPoints2.size());

    ::cv::Mat p3d;
    std::vector< ::cv::Point3f > p3d_corrected;
    ::cv::triangulatePoints(P1, P2, _imgPoints1, _imgPoints2, p3d);

    for(int i = 0; i < p3d.size[0]; ++i)
    {
        ::cv::Point3f p;
        p.x = p3d.at<float>(i, 0) / p3d.at<float>(i, 3);
        p.y = p3d.at<float>(i, 1) / p3d.at<float>(i, 3);
        p.z = p3d.at<float>(i, 2) / p3d.at<float>(i, 3);

        p3d_corrected.push_back(p);
    }

    return ::cv::Matx44f();
}

//-----------------------------------------------------------------------------

}//namespace calibration3d
}//namespace helper


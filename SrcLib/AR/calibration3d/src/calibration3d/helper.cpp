/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "calibration3d/helper.hpp"

#include "calibration3d/ReprojectionError.hpp"

#include "fwCore/spyLog.hpp"

#include <ceres/ceres.h>

#include <thread>

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

::cv::Matx44f cameraPoseStereo(const std::vector< ::cv::Point3f>& _objectPoints,
                               const ::cv::Mat _cameraMatrix1, const ::cv::Mat& _distCoeffs1,
                               const ::cv::Mat _cameraMatrix2, const ::cv::Mat& _distCoeffs2,
                               const std::vector< ::cv::Point2f >& _imgPoints1,
                               const std::vector< ::cv::Point2f >& _imgPoints2,
                               const ::cv::Mat _R, const ::cv::Mat _T)
{

    //1. initialize solution with solvePnP
    ::cv::Mat rvec, tvec, R, T;
    T = ::cv::Mat::eye(4, 4, CV_64F);

    ::cv::Mat cam1R = ::cv::Mat::eye(3, 3, CV_64F);
    ::cv::Mat cam1T = ::cv::Mat::zeros(3, 1, CV_64F);

    ::cv::solvePnP(_objectPoints, _imgPoints1, _cameraMatrix1, _distCoeffs1, rvec, tvec, false, CV_ITERATIVE);

    std::vector<double> optimVector;
    //rotation
    optimVector.push_back(rvec.at<double>(0));
    optimVector.push_back(rvec.at<double>(1));
    optimVector.push_back(rvec.at<double>(2));
    //translation
    optimVector.push_back(tvec.at<double>(0));
    optimVector.push_back(tvec.at<double>(1));
    optimVector.push_back(tvec.at<double>(2));

    //2. Creation of ceres problem
    //Minimization of sum of reprojection error for each points in each images

    ::ceres::Problem problem;

    //Cost function for image 1
    for(size_t i = 0; i < _imgPoints1.size(); ++i)
    {
        ::ceres::CostFunction* cost_function = ReprojectionError::Create(_cameraMatrix1,
                                                                         _distCoeffs1,
                                                                         _imgPoints1[i],
                                                                         _objectPoints[i],
                                                                         cam1R,
                                                                         cam1T);

        problem.AddResidualBlock(cost_function,
                                 NULL,
                                 optimVector.data()
                                 );

    }

//    //image 2
    for(size_t i = 0; i < _imgPoints2.size(); ++i)
    {
        ::ceres::CostFunction* cost_function = ReprojectionError::Create(_cameraMatrix2,
                                                                         _distCoeffs2,
                                                                         _imgPoints2[i],
                                                                         _objectPoints[i],
                                                                         _R,
                                                                         _T);

        problem.AddResidualBlock(cost_function,
                                 NULL,
                                 optimVector.data()
                                 );
    }

    // Make Ceres automatically detect the bundle structure. Note that the
    // standard solver, SPARSE_NORMAL_CHOLESKY, also works fine but it is slower
    // for standard bundle adjustment problems.
    ::ceres::Solver::Options options;
    options.trust_region_strategy_type   = ceres::LEVENBERG_MARQUARDT;
    options.minimizer_progress_to_stdout = false;
    options.gradient_tolerance           = 1e-16;
    options.function_tolerance           = 1e-16;
    options.max_num_iterations           = 100;

    int numthreads = std::thread::hardware_concurrency() / 2;

    options.num_threads = numthreads;

    ::ceres::Solver::Summary summary;
    ::ceres::Solve(options, &problem, &summary);

    SLM_DEBUG("Ceres report : "+ summary.FullReport());

    ::cv::Mat finalRVec = (::cv::Mat_<double>(3, 1) <<optimVector[0], optimVector[1], optimVector[2] );
    ::cv::Mat finalTVec = (::cv::Mat_<double>(3, 1) <<optimVector[3], optimVector[4], optimVector[5] );

    ::cv::Rodrigues(finalRVec, R); //Rotation vec. to matrix

    T( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = R * 1; // copies R into T
    T( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = finalTVec * 1; // copies tvec into T

    return ::cv::Matx44f(T);
}

//-----------------------------------------------------------------------------

}//namespace calibration3d
}//namespace helper


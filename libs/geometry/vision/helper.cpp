/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "helper.hpp"

#include "geometry/vision/detail/ReprojectionError.hpp"

#include <core/spy_log.hpp>

#include <geometry/eigen/helper.hpp>

#include <cmath>
#include <thread>

namespace sight::geometry::vision::helper
{

//-----------------------------------------------------------------------------

ErrorAndPointsType computeReprojectionError(
    const std::vector<cv::Point3f>& _objectPoints,
    const std::vector<cv::Point2f>& _imagePoints,
    const cv::Mat& _rvecs,
    const cv::Mat& _tvecs,
    const cv::Mat& _cameraMatrix,
    const cv::Mat& _distCoeffs
)
{
    ErrorAndPointsType errorAndProjectedPoints;
    std::vector<cv::Point2f> imagePoints2;
    int totalPoints = 0;
    double totalErr = 0;
    double err      = NAN;

    //projection
    cv::projectPoints(
        cv::Mat(_objectPoints),
        _rvecs,
        _tvecs,
        _cameraMatrix,
        _distCoeffs,
        imagePoints2
    );

    errorAndProjectedPoints.second = imagePoints2;

    //difference
    err = cv::norm(cv::Mat(_imagePoints), cv::Mat(imagePoints2), cv::NORM_L2);

    int n = static_cast<int>(_objectPoints.size());
    totalErr    += err * err;
    totalPoints += n;

    errorAndProjectedPoints.first = std::sqrt(totalErr / totalPoints);

    return errorAndProjectedPoints;
}

//-----------------------------------------------------------------------------

cv::Matx44f cameraPoseMonocular(
    const std::vector<cv::Point3f>& _objectPoints,
    const std::vector<cv::Point2f>& _imagePoints,
    const cv::Mat _cameraMatrix,
    const cv::Mat& _distCoeffs,
    const int _flag
)
{
    SIGHT_ASSERT(
        "There should be the same number of 3d points than 2d points",
        _objectPoints.size() == _imagePoints.size()
    );

    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat R;
    cv::Mat T;
    T = cv::Mat::eye(4, 4, CV_64F);

    //solvePnP
    cv::solvePnP(_objectPoints, _imagePoints, _cameraMatrix, _distCoeffs, rvec, tvec, false, _flag);

    // to matrix
    cv::Rodrigues(rvec, R); // R is 3x3

    T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1;    // copies R into T
    T(cv::Range(0, 3), cv::Range(3, 4)) = tvec * 1; // copies tvec into T

    return cv::Matx44f(T);
}

//-----------------------------------------------------------------------------

cv::Matx44f cameraPoseStereo(
    const std::vector<cv::Point3f>& _objectPoints,
    const cv::Mat& _cameraMatrix1,
    const cv::Mat& _distCoeffs1,
    const cv::Mat& _cameraMatrix2,
    const cv::Mat& _distCoeffs2,
    const std::vector<cv::Point2f>& _imgPoints1,
    const std::vector<cv::Point2f>& _imgPoints2,
    const cv::Mat& _r,
    const cv::Mat& _t
)
{
    //1. initialize solution with solvePnP
    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat R;
    cv::Mat T;
    T = cv::Mat::eye(4, 4, CV_64F);

    cv::Mat extrinsic = cv::Mat::eye(4, 4, CV_64F);

    extrinsic(cv::Range(0, 3), cv::Range(0, 3)) = _r * 1;
    extrinsic(cv::Range(0, 3), cv::Range(3, 4)) = _t * 1;

    cv::solvePnP(
        _objectPoints,
        _imgPoints1,
        _cameraMatrix1,
        _distCoeffs1,
        rvec,
        tvec,
        false,
        cv::SOLVEPNP_ITERATIVE
    );

    std::vector<double> optimVector = {{
        rvec.at<double>(0), rvec.at<double>(1), rvec.at<double>(2),
        tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2)
    }
    };

    //2. Creation of ceres problem
    //Minimization of sum of reprojection error for each points in each images

    ::ceres::Problem problem;

    //Cost function for image 1
    for(std::size_t i = 0 ; i < _imgPoints1.size() ; ++i)
    {
        ::ceres::CostFunction* cost_function = detail::ReprojectionError::Create(
            _cameraMatrix1,
            _distCoeffs1,
            _imgPoints1[i],
            _objectPoints[i],
            cv::Mat::eye(4, 4, CV_64F)
        );
        problem.AddResidualBlock(
            cost_function,
            nullptr,
            optimVector.data()
        );
    }

    //image 2
    for(std::size_t i = 0 ; i < _imgPoints2.size() ; ++i)
    {
        ::ceres::CostFunction* cost_function = detail::ReprojectionError::Create(
            _cameraMatrix2,
            _distCoeffs2,
            _imgPoints2[i],
            _objectPoints[i],
            extrinsic
        );
        problem.AddResidualBlock(
            cost_function,
            nullptr,
            optimVector.data()
        );
    }

    ::ceres::Solver::Options options;
    options.linear_solver_type           = ::ceres::SPARSE_NORMAL_CHOLESKY;
    options.trust_region_strategy_type   = ::ceres::LEVENBERG_MARQUARDT;
    options.minimizer_progress_to_stdout = false;
    options.gradient_tolerance           = 1e-8;
    options.function_tolerance           = 1e-8;
    options.max_num_iterations           = 100;

    int numthreads = static_cast<int>(std::thread::hardware_concurrency() / 2);

    options.num_threads = numthreads;

    ::ceres::Solver::Summary summary;
    ::ceres::Solve(options, &problem, &summary);

    SIGHT_DEBUG("Ceres report : " + summary.FullReport());

    cv::Mat finalRVec = (cv::Mat_<double>(3, 1) << optimVector[0], optimVector[1], optimVector[2]);
    cv::Mat finalTVec = (cv::Mat_<double>(3, 1) << optimVector[3], optimVector[4], optimVector[5]);

    cv::Rodrigues(finalRVec, R); //Rotation vec. to matrix

    T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1;         // copies R into T
    T(cv::Range(0, 3), cv::Range(3, 4)) = finalTVec * 1; // copies tvec into T

    return cv::Matx44f(T);
}

//-----------------------------------------------------------------------------

void calibratePointingTool(
    const data::vector::csptr _matricesVector,
    data::matrix4::sptr _calibrationMatrix,
    data::matrix4::sptr _centerMatrix
)
{
    if(_matricesVector->size() < 4)
    {
        SIGHT_WARN("Number of points when computing the tool calibration should be more than 5.");
        return;
    }

    geometry::eigen::helper::EigenMatrix matrixSum;
    matrixSum.fill(0.);
    Eigen::Vector4d vectorSum;
    vectorSum.fill(0);

    for(const auto& i : *_matricesVector)
    {
        data::matrix4::csptr m1 = std::dynamic_pointer_cast<data::matrix4>(i);
        SIGHT_ASSERT("This element of the vector is not a data::matrix4", m1);
        geometry::eigen::helper::EigenMatrix xyz1;
        xyz1.fill(0.);
        xyz1(0, 0) = (*m1)(0, 3);
        xyz1(0, 1) = (*m1)(1, 3);
        xyz1(0, 2) = (*m1)(2, 3);
        xyz1(0, 3) = 1.0;

        matrixSum = matrixSum + xyz1.transpose() * xyz1;
        vectorSum = vectorSum + xyz1.squaredNorm() * Eigen::Vector4d(xyz1(0, 0), xyz1(0, 1), xyz1(0, 2), xyz1(0, 3));
    }

    geometry::eigen::helper::EigenMatrix tempMatrix;
    tempMatrix.fill(0.);
    tempMatrix(0, 0) = vectorSum[0];
    tempMatrix(0, 1) = vectorSum[1];
    tempMatrix(0, 2) = vectorSum[2];
    tempMatrix(0, 3) = vectorSum[3];
    tempMatrix       = -tempMatrix* matrixSum.inverse();

    const double a = -1. * tempMatrix(0, 0) / 2.;
    const double b = -1. * tempMatrix(0, 1) / 2.;
    const double c = -1. * tempMatrix(0, 2) / 2.;

    Eigen::Vector3d translation;
    translation.fill(0);
    for(const auto& i : *_matricesVector)
    {
        data::matrix4::csptr m1 = std::dynamic_pointer_cast<data::matrix4>(i);
        SIGHT_ASSERT("This element of the vector is not a data::matrix4", m1);
        const geometry::eigen::helper::EigenMatrix pointMatrix = geometry::eigen::helper::toEigen(m1);
        geometry::eigen::helper::EigenMatrix centerMatrix(pointMatrix);
        const geometry::eigen::helper::EigenMatrix pointMatrixInverse = pointMatrix.inverse();

        centerMatrix(0, 3) = a;
        centerMatrix(1, 3) = b;
        centerMatrix(2, 3) = c;

        const geometry::eigen::helper::EigenMatrix calibrationMatrix = pointMatrixInverse * centerMatrix;
        translation(0) += calibrationMatrix(0, 3);
        translation(1) += calibrationMatrix(1, 3);
        translation(2) += calibrationMatrix(2, 3);
    }

    translation /= static_cast<double>(_matricesVector->size());

    (*_calibrationMatrix)(0, 3) = translation(0);
    (*_calibrationMatrix)(1, 3) = translation(1);
    (*_calibrationMatrix)(2, 3) = translation(2);

    (*_centerMatrix)(0, 3) = a;
    (*_centerMatrix)(1, 3) = b;
    (*_centerMatrix)(2, 3) = c;
}

//-----------------------------------------------------------------------------

data::point_list::sptr detectChessboard(
    const cv::Mat& _img,
    std::size_t _xDim,
    std::size_t _yDim,
    float _scale
)
{
    data::point_list::sptr pointlist;

    SIGHT_ASSERT("Expected 8bit pixel components, this image has: " << 8 * _img.elemSize1(), _img.elemSize1() == 1);

    // Ensure that we have a true depth-less 2D image.
    const cv::Mat img2d = _img.dims == 3 ? _img.reshape(0, 2, _img.size + 1) : _img;

    cv::Mat grayImg;
    if(_img.channels() == 1)
    {
        grayImg = img2d;
    }
    else
    {
        const auto cvtMethod = _img.channels() == 3 ? cv::COLOR_RGB2GRAY : cv::COLOR_RGBA2GRAY;

        cv::cvtColor(img2d, grayImg, cvtMethod);
    }

    const cv::Size boardSize(static_cast<int>(_xDim) - 1, static_cast<int>(_yDim) - 1);
    std::vector<cv::Point2f> corners;

    const int flags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FILTER_QUADS
                      | cv::CALIB_CB_FAST_CHECK;

    cv::Mat detectionImage;

    if(_scale < 1.F)
    {
        cv::resize(grayImg, detectionImage, cv::Size(), _scale, _scale);
    }
    else
    {
        detectionImage = grayImg;
    }

    const bool patternWasFound = cv::findChessboardCorners(detectionImage, boardSize, corners, flags);

    if(patternWasFound)
    {
        // Rescale points to get their coordinates in the full scale image.
        const auto rescale = [_scale](cv::Point2f& _pt){_pt = _pt / _scale;};
        std::ranges::for_each(corners, rescale);

        // Refine points coordinates in the full scale image.
        cv::TermCriteria term(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
        cv::cornerSubPix(grayImg, corners, cv::Size(5, 5), cv::Size(-1, -1), term);

        pointlist = std::make_shared<data::point_list>();
        data::point_list::PointListContainer& points = pointlist->getPoints();
        points.reserve(corners.size());

        const auto cv2SightPt = [](const cv::Point2f& p){return std::make_shared<data::point>(p.x, p.y);};
        std::ranges::transform(corners, std::back_inserter(points), cv2SightPt);
    }

    return pointlist;
}

// ----------------------------------------------------------------------------

} // namespace sight::geometry::vision::helper

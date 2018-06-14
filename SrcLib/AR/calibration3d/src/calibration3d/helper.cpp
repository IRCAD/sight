/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "calibration3d/helper.hpp"

#include "calibration3d/ReprojectionError.hpp"

#include <eigenTools/helper.hpp>

#include <fwCore/spyLog.hpp>

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
                               const ::cv::Mat& _cameraMatrix1, const ::cv::Mat& _distCoeffs1,
                               const ::cv::Mat& _cameraMatrix2, const ::cv::Mat& _distCoeffs2,
                               const std::vector< ::cv::Point2f >& _imgPoints1,
                               const std::vector< ::cv::Point2f >& _imgPoints2,
                               const ::cv::Mat& _R, const ::cv::Mat& _T)
{

    //1. initialize solution with solvePnP
    ::cv::Mat rvec, tvec, R, T;
    T = ::cv::Mat::eye(4, 4, CV_64F);

    ::cv::Mat extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    extrinsic( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = _R * 1;
    extrinsic( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = _T * 1;

    ::cv::solvePnP(_objectPoints, _imgPoints1, _cameraMatrix1, _distCoeffs1, rvec, tvec, false, CV_ITERATIVE);

    std::vector<double> optimVector = {{
                                           rvec.at<double>(0), rvec.at<double>(1), rvec.at<double>(2),
                                           tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2)
                                       }};

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
                                                                         ::cv::Mat::eye(4, 4, CV_64F));
        problem.AddResidualBlock(cost_function,
                                 nullptr,
                                 optimVector.data()
                                 );

    }

    //image 2
    for(size_t i = 0; i < _imgPoints2.size(); ++i)
    {
        ::ceres::CostFunction* cost_function = ReprojectionError::Create(_cameraMatrix2,
                                                                         _distCoeffs2,
                                                                         _imgPoints2[i],
                                                                         _objectPoints[i],
                                                                         extrinsic);
        problem.AddResidualBlock(cost_function,
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

    int numthreads = std::thread::hardware_concurrency() / 2;

    options.num_threads = numthreads;

    ::ceres::Solver::Summary summary;
    ::ceres::Solve(options, &problem, &summary);

    SLM_DEBUG("Ceres report : "+ summary.FullReport());

    ::cv::Mat finalRVec = ( ::cv::Mat_<double>(3, 1) <<optimVector[0], optimVector[1], optimVector[2] );
    ::cv::Mat finalTVec = ( ::cv::Mat_<double>(3, 1) <<optimVector[3], optimVector[4], optimVector[5] );

    ::cv::Rodrigues(finalRVec, R); //Rotation vec. to matrix

    T( ::cv::Range(0, 3), ::cv::Range(0, 3) ) = R * 1; // copies R into T
    T( ::cv::Range(0, 3), ::cv::Range(3, 4) ) = finalTVec * 1; // copies tvec into T

    return ::cv::Matx44f(T);
}

//-----------------------------------------------------------------------------

void calibratePointingTool(const ::fwData::Vector::csptr _matricesVector,
                           ::fwData::TransformationMatrix3D::sptr _calibrationMatrix,
                           ::fwData::TransformationMatrix3D::sptr _centerMatrix)
{
    const ::fwData::Vector::ContainerType matrices = _matricesVector->getContainer();
    const size_t nbrMatrices                       = matrices.size();

    if (nbrMatrices < 4)
    {
        SLM_WARN("Number of points when computing the tool calibration should be more than 5.");
        return;
    }

    ::eigenTools::helper::EigenMatrix matrixSum;
    matrixSum.fill(0.);
    ::Eigen::Vector4d vectorSum;
    vectorSum.fill(0);

    for (size_t i = 0; i < nbrMatrices; ++i)
    {
        ::fwData::TransformationMatrix3D::csptr m1 = ::fwData::TransformationMatrix3D::dynamicCast(matrices.at(i));
        SLM_ASSERT("This element of the vector is not a ::fwData::TransformationMatrix3D", m1);
        ::eigenTools::helper::EigenMatrix xyz1;
        xyz1.fill(0.);
        xyz1(0, 0) = m1->getCoefficient(0, 3);
        xyz1(0, 1) = m1->getCoefficient(1, 3);
        xyz1(0, 2) = m1->getCoefficient(2, 3);
        xyz1(0, 3) = 1.0;

        matrixSum = matrixSum + xyz1.transpose() * xyz1;
        vectorSum = vectorSum + xyz1.squaredNorm() * ::Eigen::Vector4d(xyz1(0, 0), xyz1(0, 1), xyz1(0, 2), xyz1(0, 3));
    }

    ::eigenTools::helper::EigenMatrix tempMatrix;
    tempMatrix.fill(0.);
    tempMatrix(0, 0) = vectorSum[0];
    tempMatrix(0, 1) = vectorSum[1];
    tempMatrix(0, 2) = vectorSum[2];
    tempMatrix(0, 3) = vectorSum[3];
    tempMatrix       = -tempMatrix* matrixSum.inverse();

    const double a = -1. * tempMatrix(0, 0) / 2.;
    const double b = -1. * tempMatrix(0, 1) / 2.;
    const double c = -1. * tempMatrix(0, 2) / 2.;

    ::Eigen::Vector3d translation;
    translation.fill(0);
    for (size_t i = 0; i < nbrMatrices; ++i)
    {
        ::fwData::TransformationMatrix3D::csptr m1 = ::fwData::TransformationMatrix3D::dynamicCast(matrices.at(i));
        SLM_ASSERT("This element of the vector is not a ::fwData::TransformationMatrix3D", m1);
        const ::eigenTools::helper::EigenMatrix pointMatrix = ::eigenTools::helper::toEigen(m1->getCoefficients());
        ::eigenTools::helper::EigenMatrix centerMatrix(pointMatrix);
        const ::eigenTools::helper::EigenMatrix pointMatrixInverse = pointMatrix.inverse();

        centerMatrix(0, 3) = a;
        centerMatrix(1, 3) = b;
        centerMatrix(2, 3) = c;

        const ::eigenTools::helper::EigenMatrix calibrationMatrix = pointMatrixInverse * centerMatrix;
        translation(0) += calibrationMatrix(0, 3);
        translation(1) += calibrationMatrix(1, 3);
        translation(2) += calibrationMatrix(2, 3);
    }

    translation /= static_cast<double>(nbrMatrices);

    _calibrationMatrix->setCoefficient(0, 3, translation(0));
    _calibrationMatrix->setCoefficient(1, 3, translation(1));
    _calibrationMatrix->setCoefficient(2, 3, translation(2));

    _centerMatrix->setCoefficient(0, 3, a);
    _centerMatrix->setCoefficient(1, 3, b);
    _centerMatrix->setCoefficient(2, 3, c);
}

//-----------------------------------------------------------------------------

cv::Ptr< ::cv::aruco::Dictionary> generateArucoDictionary(const size_t _width, const size_t _height,
                                                          const int _markerSizeInBits)
{
    //Determine which dictionary to use
    // nb of markers (< 50,< 100 < 250, < 1000)
    const size_t nbMarkers = (_width * _height) / 2;
    ::cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary = ::cv::aruco::DICT_6X6_100;
    if(_markerSizeInBits == 4)
    {
        if(nbMarkers <= 50)
        {
            dictionary = ::cv::aruco::DICT_4X4_50;
        }
        else if(nbMarkers <= 100)
        {
            dictionary = ::cv::aruco::DICT_4X4_100;
        }
        else if(nbMarkers <= 250)
        {
            dictionary = ::cv::aruco::DICT_4X4_250;
        }
        else
        {
            dictionary = ::cv::aruco::DICT_4X4_1000;
        }
    }
    else if(_markerSizeInBits == 5)
    {
        if(nbMarkers <= 50)
        {
            dictionary = ::cv::aruco::DICT_5X5_50;
        }
        else if(nbMarkers <= 100)
        {
            dictionary = ::cv::aruco::DICT_5X5_100;
        }
        else if(nbMarkers <= 250)
        {
            dictionary = ::cv::aruco::DICT_5X5_250;
        }
        else
        {
            dictionary = ::cv::aruco::DICT_5X5_1000;
        }
    }
    else if(_markerSizeInBits == 6)
    {
        if(nbMarkers <= 50)
        {
            dictionary = ::cv::aruco::DICT_6X6_50;
        }
        else if(nbMarkers <= 100)
        {
            dictionary = ::cv::aruco::DICT_6X6_100;
        }
        else if(nbMarkers <= 250)
        {
            dictionary = ::cv::aruco::DICT_6X6_250;
        }
        else
        {
            dictionary = ::cv::aruco::DICT_6X6_1000;
        }
    }
    else if(_markerSizeInBits == 7)
    {
        if(nbMarkers <= 50)
        {
            dictionary = ::cv::aruco::DICT_7X7_50;
        }
        else if(nbMarkers <= 100)
        {
            dictionary = ::cv::aruco::DICT_7X7_100;
        }
        else if(nbMarkers <= 250)
        {
            dictionary = ::cv::aruco::DICT_7X7_250;
        }
        else
        {
            dictionary = ::cv::aruco::DICT_7X7_1000;
        }
    }
    else
    {
        OSLM_ERROR("Cannot generate dictionary with marker size of: "<<_markerSizeInBits);
    }

    return ::cv::aruco::generateCustomDictionary(static_cast<int>(nbMarkers), _markerSizeInBits,
                                                 ::cv::aruco::getPredefinedDictionary(dictionary));
}

//-----------------------------------------------------------------------------

}//namespace calibration3d
}//namespace helper

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "helperTest.hpp"

#include <calibration3d/helper.hpp>

#include <opencv2/core.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::calibration3d::ut::helperTest );

namespace calibration3d
{
namespace ut
{

//------------------------------------------------------------------------------

void helperTest::setUp()
{

}

//------------------------------------------------------------------------------

void helperTest::tearDown()
{
}

//------------------------------------------------------------------------------

void helperTest::reprojectionIdentity()
{
    std::vector< ::cv::Point3f > objectPoints;
    objectPoints.push_back( ::cv::Point3f(-30, 30, 0));
    objectPoints.push_back( ::cv::Point3f( 30, 30, 0));
    objectPoints.push_back( ::cv::Point3f( 30, -30, 0));
    objectPoints.push_back( ::cv::Point3f(-30, -30, 0));

    std::vector< ::cv::Point2f > imagePoints;
    imagePoints.push_back( ::cv::Point2f(-30, 30));
    imagePoints.push_back( ::cv::Point2f( 30, 30));
    imagePoints.push_back( ::cv::Point2f( 30, -30));
    imagePoints.push_back( ::cv::Point2f(-30, -30));

    ::cv::Mat rvec, tvec;
    ::cv::Mat cameraMatrix;
    ::cv::Mat distorsion;

    rvec = ::cv::Mat::zeros(3,1,CV_64F);
    tvec = ::cv::Mat::zeros(3,1,CV_64F);

    cameraMatrix = ::cv::Mat::eye(3,3,CV_64F);
    distorsion = ::cv::Mat::zeros(5,1,CV_64F);

    ::calibration3d::helper::ErrorAndPointsType result =
            ::calibration3d::helper::computeReprojectionError(objectPoints, imagePoints,
                                                      rvec, tvec, cameraMatrix, distorsion);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., result.first, 10e-6);

    for(unsigned int i = 0; i< 4; ++i)
    {
        ::cv::Point2f expected = imagePoints[i];
        ::cv::Point2f actual = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.x),
                                     static_cast<double>(actual.x), 10e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.y),
                                     static_cast<double>(actual.y), 10e-6);

    }

}
//------------------------------------------------------------------------------

void helperTest::reprojectionRealDatasetTest1()
{

    std::vector< ::cv::Point3f > objectPoints;
    objectPoints.push_back(::cv::Point3f(-30, 30, 0));
    objectPoints.push_back(::cv::Point3f( 30, 30, 0));
    objectPoints.push_back(::cv::Point3f( 30, -30, 0));
    objectPoints.push_back(::cv::Point3f(-30, -30, 0));

    std::vector< ::cv::Point2f > imagePoints;

    ::cv::Mat rvec, tvec;
    ::cv::Mat cameraMatrix;
    ::cv::Mat distorsion;

    rvec = ::cv::Mat::zeros(3,1,CV_64F);
    tvec = ::cv::Mat::zeros(3,1,CV_64F);

    cameraMatrix = ::cv::Mat::eye(3,3,CV_64F);
    distorsion = ::cv::Mat::zeros(5,1,CV_64F);


    //Values from real dataset 1
    imagePoints.push_back(::cv::Point2f(220.92876f, 224.95102f));
    imagePoints.push_back(::cv::Point2f(393.3707f, 216.15828f));
    imagePoints.push_back(::cv::Point2f(416.92828f, 365.78033f));
    imagePoints.push_back(::cv::Point2f(207.39848f, 375.8844f));

    rvec.at<double>(0) = 2.428872194668891;
    rvec.at<double>(1) = -0.04640821030480094;
    rvec.at<double>(2) = -0.05408638946393874;

    tvec.at<double>(0) = 2.276957511901855;
    tvec.at<double>(1) = 2.249284982681274;
    tvec.at<double>(2) = 197.2704315185547;

    cameraMatrix.at<double>(0,0) = 621.8917806423808; //fx
    cameraMatrix.at<double>(0,2) = 302.178791918929; //cx

    cameraMatrix.at<double>(1,1) = 640.3221665123945; //fy
    cameraMatrix.at<double>(1,2) = 281.4344544571003; //cy

    distorsion.at<double>(0) = 0.06356924027204514; //k1
    distorsion.at<double>(1) = -0.07869192957878113; //k2
    distorsion.at<double>(2) = -0.01339129731059074; //p1
    distorsion.at<double>(3) = -0.007336273789405823; //p2
    distorsion.at<double>(4) = 0.6075565814971924; //k3

    std::vector< ::cv::Point2f > reprojectedPoints_expected;
    reprojectedPoints_expected.push_back(::cv::Point2f(220.89903f, 224.99107f));
    reprojectedPoints_expected.push_back(::cv::Point2f(393.38748f, 216.12437f));
    reprojectedPoints_expected.push_back(::cv::Point2f(416.922f, 365.82419f));
    reprojectedPoints_expected.push_back(::cv::Point2f(207.41541f, 375.83548f));

    const double expectedErr = 0.0462637;

    ::calibration3d::helper::ErrorAndPointsType result =
            ::calibration3d::helper::computeReprojectionError(objectPoints, imagePoints,
                                                      rvec, tvec, cameraMatrix, distorsion);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedErr, result.first, 10e-6);

    for(unsigned int i = 0; i< 4; ++i)
    {
        ::cv::Point2f expected = reprojectedPoints_expected[i];
        ::cv::Point2f actual = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.x),
                                     static_cast<double>(actual.x), 10e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.y),
                                     static_cast<double>(actual.y), 10e-6);

    }


}

//------------------------------------------------------------------------------

void helperTest::reprojectionRealDatasetTest2()
{

    std::vector< ::cv::Point3f > objectPoints;
    objectPoints.push_back(::cv::Point3f(-30, 30, 0));
    objectPoints.push_back(::cv::Point3f( 30, 30, 0));
    objectPoints.push_back(::cv::Point3f( 30, -30, 0));
    objectPoints.push_back(::cv::Point3f(-30, -30, 0));

    std::vector< ::cv::Point2f > imagePoints;

    ::cv::Mat rvec, tvec;
    ::cv::Mat cameraMatrix;
    ::cv::Mat distorsion;

    rvec = ::cv::Mat::zeros(3,1,CV_64F);
    tvec = ::cv::Mat::zeros(3,1,CV_64F);

    cameraMatrix = ::cv::Mat::eye(3,3,CV_64F);
    distorsion = ::cv::Mat::zeros(5,1,CV_64F);


    //Values from real dataset 1
    imagePoints.push_back(::cv::Point2f(232.76308f, 227.31705f));
    imagePoints.push_back(::cv::Point2f(410.1149f, 220.90685f));
    imagePoints.push_back(::cv::Point2f(434.63507f, 372.6174f));
    imagePoints.push_back(::cv::Point2f(216.75545f, 379.34464f));

    rvec.at<double>(0) = 2.397315382812963;
    rvec.at<double>(1) = -0.02181686018178059;
    rvec.at<double>(2) = -0.05900882764341142;

    tvec.at<double>(0) = 6.422468185424805;
    tvec.at<double>(1) = 3.302493572235107;
    tvec.at<double>(2) = 191.1024627685547;

    cameraMatrix.at<double>(0,0) = 621.8917806423808; //fx
    cameraMatrix.at<double>(0,2) = 302.178791918929; //cx

    cameraMatrix.at<double>(1,1) = 640.3221665123945; //fy
    cameraMatrix.at<double>(1,2) = 281.4344544571003; //cy

    distorsion.at<double>(0) = 0.06356924027204514; //k1
    distorsion.at<double>(1) = -0.07869192957878113; //k2
    distorsion.at<double>(2) = -0.01339129731059074; //p1
    distorsion.at<double>(3) = -0.007336273789405823; //p2
    distorsion.at<double>(4) = 0.6075565814971924; //k3

    std::vector< ::cv::Point2f > reprojectedPoints_expected;
    reprojectedPoints_expected.push_back(::cv::Point2f(232.92502f, 227.35114f));
    reprojectedPoints_expected.push_back(::cv::Point2f(409.92566f, 220.82785f));
    reprojectedPoints_expected.push_back(::cv::Point2f(434.73633f, 372.69672f));
    reprojectedPoints_expected.push_back(::cv::Point2f(216.67587f, 379.3006f));

    const double expectedErr = 0.153504;

    ::calibration3d::helper::ErrorAndPointsType result =
            ::calibration3d::helper::computeReprojectionError(objectPoints, imagePoints,
                                                      rvec, tvec, cameraMatrix, distorsion);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedErr, result.first, 10e-6);

    for(unsigned int i = 0; i< 4; ++i)
    {
        ::cv::Point2f expected = reprojectedPoints_expected[i];
        ::cv::Point2f actual = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.x),
                                     static_cast<double>(actual.x), 10e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.y),
                                     static_cast<double>(actual.y), 10e-6);

    }
}

//------------------------------------------------------------------------------

}//namespace ut
}//namespace calibration3d

/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "helperTest.hpp"

#include <calibration3d/helper.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwTest/Data.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::calibration3d::ut::helperTest );

namespace calibration3d
{
namespace ut
{

using ExpectedChessboardType = std::vector< ::fwData::Point::PointCoordArrayType >;

//------------------------------------------------------------------------------

static inline const ::cv::Mat readRGBImage(const std::string _fname)
{
    ::cv::Mat img = ::cv::imread(_fname, ::cv::IMREAD_COLOR);
    ::cv::cvtColor(img, img, ::cv::COLOR_BGR2RGB);

    return img;
}

//------------------------------------------------------------------------------

static inline void compareChessboards(const ExpectedChessboardType& _expected,
                                      const ::fwData::PointList::csptr& _detected)
{
    CPPUNIT_ASSERT_EQUAL(_expected.size(), _detected->getPoints().size());

    for(std::uint32_t i = 0; i < _expected.size(); ++i)
    {
        const auto& expectedCoords = _expected[i];
        const auto& detectedCoords = _detected->getPoints()[i]->getCoord();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedCoords[0], detectedCoords[0], 0.01);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedCoords[1], detectedCoords[1], 0.01);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, detectedCoords[2], 0.0);
    }
}

//------------------------------------------------------------------------------

void helperTest::setUp()
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));
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

    rvec = ::cv::Mat::zeros(3, 1, CV_64F);
    tvec = ::cv::Mat::zeros(3, 1, CV_64F);

    cameraMatrix = ::cv::Mat::eye(3, 3, CV_64F);
    distorsion   = ::cv::Mat::zeros(5, 1, CV_64F);

    ::calibration3d::helper::ErrorAndPointsType result =
        ::calibration3d::helper::computeReprojectionError(objectPoints, imagePoints,
                                                          rvec, tvec, cameraMatrix, distorsion);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., result.first, 10e-6);

    for(unsigned int i = 0; i < 4; ++i)
    {
        ::cv::Point2f expected = imagePoints[i];
        ::cv::Point2f actual   = result.second[i];

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

    rvec = ::cv::Mat::zeros(3, 1, CV_64F);
    tvec = ::cv::Mat::zeros(3, 1, CV_64F);

    cameraMatrix = ::cv::Mat::eye(3, 3, CV_64F);
    distorsion   = ::cv::Mat::zeros(5, 1, CV_64F);

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

    cameraMatrix.at<double>(0, 0) = 621.8917806423808; //fx
    cameraMatrix.at<double>(0, 2) = 302.178791918929; //cx

    cameraMatrix.at<double>(1, 1) = 640.3221665123945; //fy
    cameraMatrix.at<double>(1, 2) = 281.4344544571003; //cy

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

    for(unsigned int i = 0; i < 4; ++i)
    {
        ::cv::Point2f expected = reprojectedPoints_expected[i];
        ::cv::Point2f actual   = result.second[i];

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

    rvec = ::cv::Mat::zeros(3, 1, CV_64F);
    tvec = ::cv::Mat::zeros(3, 1, CV_64F);

    cameraMatrix = ::cv::Mat::eye(3, 3, CV_64F);
    distorsion   = ::cv::Mat::zeros(5, 1, CV_64F);

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

    cameraMatrix.at<double>(0, 0) = 621.8917806423808; //fx
    cameraMatrix.at<double>(0, 2) = 302.178791918929; //cx

    cameraMatrix.at<double>(1, 1) = 640.3221665123945; //fy
    cameraMatrix.at<double>(1, 2) = 281.4344544571003; //cy

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

    for(unsigned int i = 0; i < 4; ++i)
    {
        ::cv::Point2f expected = reprojectedPoints_expected[i];
        ::cv::Point2f actual   = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.x),
                                     static_cast<double>(actual.x), 10e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(expected.y),
                                     static_cast<double>(actual.y), 10e-6);

    }
}

//------------------------------------------------------------------------------

void helperTest::toolCalibrationBasic()
{
    const std::uint8_t nbMatrices = 34;
    ::fwData::Vector::sptr matricesVector = ::fwData::Vector::New();

    ::fwData::TransformationMatrix3D::sptr resCenterMatrix    = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr resTransformMatrix = ::fwData::TransformationMatrix3D::New();

    const ::glm::dvec3 center(0., 0., 0.);
    const double radius = 18;
    const ::glm::dvec3 translation(radius, 0, 0);

    // generates matrices
    for (size_t i = 0; i < nbMatrices; ++i)
    {
        const double angleInDegrees = rand()%180;
        const double angle          = ::glm::radians(angleInDegrees);

        const double x = rand()%100 / 100.;
        const double y = rand()%100 / 100.;
        const double z = rand()%100 / 100.;

        ::glm::dvec3 axis(x, y, z);
        axis = glm::normalize(axis);

        ::glm::dmat4 matrix = ::glm::translate(::glm::dmat4(1.), center);

        matrix = ::glm::rotate(matrix, angle, axis);

        matrix = ::glm::translate(matrix, translation);

        ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
        ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(mat, matrix);

        matricesVector->getContainer().push_back(mat);
    }
    ::calibration3d::helper::calibratePointingTool(matricesVector, resTransformMatrix, resCenterMatrix);

    for (std::uint8_t i = 0; i < 3; ++i)
    {
        for (std::uint8_t j = 0; j < 3; ++j)
        {

            const double val = (i == j ? 1. : 0.);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("transform["+ std::to_string(i) + "][" + std::to_string(j) + "]",
                                                 val, resTransformMatrix->getCoefficient(i, j), 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("center["+ std::to_string(i) + "][" + std::to_string(j) + "]",
                                                 val, resCenterMatrix->getCoefficient(i, j), 0.00001);
        }
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("transform[" + std::to_string(i) + "][3]",
                                             -translation[i], resTransformMatrix->getCoefficient(i, 3), 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("center[" + std::to_string(i) + "][3]",
                                             center[i], resCenterMatrix->getCoefficient(i, 3), 0.00001);
    }
}

//------------------------------------------------------------------------------

void helperTest::toolCalibration()
{
    const std::uint8_t nbMatrices = 46;
    ::fwData::Vector::sptr matricesVector = ::fwData::Vector::New();

    ::fwData::TransformationMatrix3D::sptr resCenterMatrix    = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr resTransformMatrix = ::fwData::TransformationMatrix3D::New();

    const ::glm::dvec3 center(36., 52., -530.);
    const ::glm::dvec3 translation(-459.45, 46.6, -88.54);

    // generates matrices
    for (size_t i = 0; i < nbMatrices; ++i)
    {
        const double angleInDegrees = rand()%360;
        const double angle          = ::glm::radians(angleInDegrees);

        const double x = rand()%100 / 100.;
        const double y = rand()%100 / 100.;
        const double z = rand()%100 / 100.;

        ::glm::dvec3 axis(x, y, z);
        axis = glm::normalize(axis);

        ::glm::dmat4 matrix = ::glm::translate(::glm::dmat4(1.), center);
        matrix              = ::glm::rotate(matrix, angle, axis);
        matrix              = ::glm::translate(matrix, translation);

        ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();
        ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(mat, matrix);

        matricesVector->getContainer().push_back(mat);
    }
    ::calibration3d::helper::calibratePointingTool(matricesVector, resTransformMatrix, resCenterMatrix);

    for (std::uint8_t i = 0; i < 3; ++i)
    {
        for (std::uint8_t j = 0; j < 3; ++j)
        {

            const double val = (i == j ? 1. : 0.);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("transform["+ std::to_string(i) + "][" + std::to_string(j) + "]",
                                                 val, resTransformMatrix->getCoefficient(i, j), 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("center["+ std::to_string(i) + "][" + std::to_string(j) + "]",
                                                 val, resCenterMatrix->getCoefficient(i, j), 0.00001);
        }
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("transform[" + std::to_string(i) + "][3]",
                                             -translation[i], resTransformMatrix->getCoefficient(i, 3), 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("center[" + std::to_string(i) + "][3]",
                                             center[i], resCenterMatrix->getCoefficient(i, 3), 0.00001);
    }
}

//------------------------------------------------------------------------------

void helperTest::generateCharucoDict()
{
    //Test extrem cases
    int x[32] =
    {4, 50, 51, 100, 101, 250, 251, 1000,
     4, 50, 51, 100, 101, 250, 251, 1000,
     4, 50, 51, 100, 101, 250, 251, 1000,
     4, 50, 51, 100, 101, 250, 251, 1000};

    int bits[32] = {4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7};

    ::cv::Ptr< ::cv::aruco::Dictionary> generatedDict;

    //Test case 4x4_50
    for(int ii = 0; ii < 32; ++ii)
    {
        ::cv::Ptr< ::cv::aruco::Dictionary> DictExpected =
            ::cv::aruco::generateCustomDictionary(x[ii], bits[ii], ::cv::aruco::getPredefinedDictionary(ii/2));

        CPPUNIT_ASSERT_NO_THROW( generatedDict = ::calibration3d::helper::generateArucoDictionary(x[ii], 2, bits[ii]));

        CPPUNIT_ASSERT_EQUAL_MESSAGE("markerSize are not equal", DictExpected->markerSize, generatedDict->markerSize);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("maxCorrectionBits are not equal", DictExpected->maxCorrectionBits,
                                     generatedDict->maxCorrectionBits);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("bytesList.size are not equal", DictExpected->bytesList.size,
                                     generatedDict->bytesList.size);

        for(int i = 0; i < DictExpected->bytesList.rows; ++i)
        {
            for(int j = 0; j < DictExpected->bytesList.cols; ++j)
            {
                for(int k = 0; k < DictExpected->bytesList.channels(); ++k)
                {
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Values of bytesList are not equal",
                                                 DictExpected->bytesList.at< ::cv::Vec4b >(i, j)[k],
                                                 generatedDict->bytesList.at< ::cv::Vec4b >(i, j)[k]);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void helperTest::chessboardDetectionTest()
{
    const auto calibDataDir = ::fwTest::Data::dir() / "sight" / "calibration";
    {
        const ::cv::Mat chessRgb0 = readRGBImage((calibDataDir / "chessboardRGB0.tiff").string());

        CPPUNIT_ASSERT_EQUAL(false, chessRgb0.empty());
        CPPUNIT_ASSERT_EQUAL(3, chessRgb0.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), chessRgb0.elemSize1());

        const ::fwData::PointList::csptr detectedChess = ::calibration3d::helper::detectChessboard(chessRgb0,
                                                                                                   9, 6, 0.25f);

        const ExpectedChessboardType expectedChessboard {
            {743.796, 429.966, 0.0},
            {822.543, 406.12, 0.0},
            {903.78, 383.864, 0.0},
            {984.956, 361.093, 0.0},
            {1065.97, 338.673, 0.0},
            {1146.06, 317.624, 0.0},
            {1225.52, 297.342, 0.0},
            {1302.24, 276.42, 0.0},
            {762.267, 508.076, 0.0},
            {843.178, 484.55, 0.0},
            {925.297, 459.935, 0.0},
            {1007.55, 436.705, 0.0},
            {1089.97, 413.711, 0.0},
            {1171.73, 393.006, 0.0},
            {1251.21, 370.877, 0.0},
            {1328.73, 350.586, 0.0},
            {781.57, 587.452, 0.0},
            {863.595, 563.583, 0.0},
            {946.918, 539.969, 0.0},
            {1030.23, 515.795, 0.0},
            {1114.83, 492.528, 0.0},
            {1197.12, 468.886, 0.0},
            {1277.37, 446.899, 0.0},
            {1355.74, 424.96, 0.0},
            {802.265, 668.699, 0.0},
            {885.736, 644.777, 0.0},
            {970.339, 621.625, 0.0},
            {1055.98, 597.247, 0.0},
            {1139.21, 573.536, 0.0},
            {1222.75, 548.947, 0.0},
            {1304.78, 525.552, 0.0},
            {1383.77, 502.059, 0.0},
            {824.78, 751.572, 0.0},
            {909.374, 728.127, 0.0},
            {994.385, 704.509, 0.0},
            {1080.42, 680.52, 0.0},
            {1165.74, 656.121, 0.0},
            {1249.02, 630.135, 0.0},
            {1331.61, 605.354, 0.0},
            {1411.49, 581.081, 0.0}
        };

        compareChessboards(expectedChessboard, detectedChess);
    }

    {
        const ::cv::Mat chessRgb1 = readRGBImage((calibDataDir / "chessboardRGB1.tiff").string());

        CPPUNIT_ASSERT_EQUAL(false, chessRgb1.empty());
        CPPUNIT_ASSERT_EQUAL(3, chessRgb1.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), chessRgb1.elemSize1());

        const ::fwData::PointList::csptr detectedChess = ::calibration3d::helper::detectChessboard(chessRgb1,
                                                                                                   9, 6, 0.25f);

        const ExpectedChessboardType expectedChessboard {
            {739.34, 394.467, 0.0},
            {822.398, 400.638, 0.0},
            {907.387, 406.695, 0.0},
            {994.247, 412.994, 0.0},
            {1083.21, 420.174, 0.0},
            {1172.81, 426.662, 0.0},
            {1262.31, 434.583, 0.0},
            {1350.74, 442.525, 0.0},
            {715.396, 444.915, 0.0},
            {801.192, 451.006, 0.0},
            {890.182, 457.649, 0.0},
            {980.747, 464.722, 0.0},
            {1073.72, 471.985, 0.0},
            {1167.43, 480.636, 0.0},
            {1261.21, 487.35, 0.0},
            {1353.4, 496.396, 0.0},
            {688.777, 498.619, 0.0},
            {777.508, 506.315, 0.0},
            {870.244, 514.487, 0.0},
            {965.658, 522.376, 0.0},
            {1061.78, 530.548, 0.0},
            {1160.71, 538.417, 0.0},
            {1258.89, 546.984, 0.0},
            {1356.3, 555.451, 0.0},
            {660.464, 558.024, 0.0},
            {752.264, 566.962, 0.0},
            {849.34, 576.219, 0.0},
            {948.697, 585.038, 0.0},
            {1049.92, 593.705, 0.0},
            {1153.28, 603.106, 0.0},
            {1256.79, 611.626, 0.0},
            {1359.38, 620.055, 0.0},
            {629.884, 622.767, 0.0},
            {726.468, 633.131, 0.0},
            {825.885, 643.774, 0.0},
            {930.79, 653.911, 0.0},
            {1037.31, 664.543, 0.0},
            {1145.59, 674.313, 0.0},
            {1254.46, 683.415, 0.0},
            {1361.34, 691.433, 0.0}
        };

        compareChessboards(expectedChessboard, detectedChess);
    }

    {
        const ::cv::Mat chessGray = ::cv::imread((calibDataDir / "chessboardGray.tiff").string(),
                                                 ::cv::IMREAD_GRAYSCALE);

        CPPUNIT_ASSERT_EQUAL(false, chessGray.empty());
        CPPUNIT_ASSERT_EQUAL(1, chessGray.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), chessGray.elemSize1());

        const ::fwData::PointList::csptr detectedChess = ::calibration3d::helper::detectChessboard(chessGray,
                                                                                                   9, 6, 0.25f);

        const ExpectedChessboardType expectedChessboard {
            {933.376, 504.662, 0.0},
            {980.189, 463.743, 0.0},
            {1033.96, 427.51, 0.0},
            {1081.23, 392.296, 0.0},
            {1126.85, 357.625, 0.0},
            {1169.53, 325.472, 0.0},
            {1209.91, 295.648, 0.0},
            {1248.9, 266.503, 0.0},
            {984.026, 557.646, 0.0},
            {1034.66, 516.435, 0.0},
            {1083.63, 477.243, 0.0},
            {1129.72, 440.497, 0.0},
            {1174.63, 404.916, 0.0},
            {1215.77, 371.629, 0.0},
            {1255.68, 339.41, 0.0},
            {1293.53, 309.586, 0.0},
            {1034.53, 610.859, 0.0},
            {1084.45, 568.443, 0.0},
            {1132.77, 527.856, 0.0},
            {1179.05, 490.443, 0.0},
            {1221.37, 452.432, 0.0},
            {1262.46, 417.321, 0.0},
            {1300.72, 384.633, 0.0},
            {1337.88, 353.154, 0.0},
            {1086.83, 664.58, 0.0},
            {1135.66, 621.173, 0.0},
            {1182.46, 578.872, 0.0},
            {1227.32, 539.044, 0.0},
            {1268.66, 501.042, 0.0},
            {1309.61, 464.375, 0.0},
            {1346.99, 429.715, 0.0},
            {1382.89, 397.553, 0.0},
            {1138.79, 718.459, 0.0},
            {1186.3, 673.672, 0.0},
            {1232.29, 629.863, 0.0},
            {1275.78, 588.117, 0.0},
            {1316.88, 549.465, 0.0},
            {1355.55, 511.947, 0.0},
            {1392.14, 476.584, 0.0},
            {1426.73, 442.136, 0.0}
        };

        compareChessboards(expectedChessboard, detectedChess);

        const ::fwData::PointList::csptr detectedChess2 = ::calibration3d::helper::detectChessboard(chessGray,
                                                                                                    8, 5, 0.25f);
        CPPUNIT_ASSERT(!detectedChess2);
    }
}

//------------------------------------------------------------------------------

void helperTest::chessboardDetectionScaleTest()
{
    const auto calibDataDir = ::fwTest::Data::dir() / "sight" / "calibration";

    const ::cv::Mat chessRgb0 = readRGBImage((calibDataDir / "chessboardRGB0.tiff").string());

    const ::fwData::PointList::csptr detectedChessFullScale =
        ::calibration3d::helper::detectChessboard(chessRgb0, 9, 6, 1.f);

    const ::fwData::PointList::csptr detectedChessQuarterScale =
        ::calibration3d::helper::detectChessboard(chessRgb0, 9, 6, 0.25f);

    CPPUNIT_ASSERT_EQUAL(detectedChessFullScale->getPoints().size(), detectedChessQuarterScale->getPoints().size());

    for(std::uint32_t i = 0; i < detectedChessFullScale->getPoints().size(); ++i)
    {
        const auto& fullScaleCoords    = detectedChessFullScale->getPoints()[i]->getCoord();
        const auto& quarterScaleCoords = detectedChessQuarterScale->getPoints()[i]->getCoord();

        for(std::uint8_t j = 0; j < 3; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fullScaleCoords[j], quarterScaleCoords[j], 0.5);
        }
    }
}

}//namespace ut
}//namespace calibration3d

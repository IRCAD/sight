/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#define GLOG_NO_ABBREVIATED_SEVERITIES // This bypasses a compile error on Windows.

#include "HandEyeCalibrationTest.hpp"

#include <handEyeCalibration/camodocal/EigenUtils.h>
#include <handEyeCalibration/camodocal/gpl.h>
#include <handEyeCalibration/camodocal/HandEyeCalibration.h>
#include <handEyeCalibration/HandEyeApi.hpp>

#include <eigenTools/helper.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>

#include <fwTest/Data.hpp>

#include <ceres/ceres.h>

#include <array>
#include <ctime>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::handEyeCalibration::ut::HandEyeCalibrationTest );

namespace handEyeCalibration
{
namespace ut
{

// ------------------------------------------------------------------------------

void HandEyeCalibrationTest::setUp()
{
    // Set up context before running a test.
    // initialize random
    std::srand(static_cast<unsigned int>(std::time(0)));
}

// ------------------------------------------------------------------------------

void HandEyeCalibrationTest::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void HandEyeCalibrationTest::checkCalibration()
{

    ::camodocal::HandEyeCalibration::setVerbose(false);

    ::Eigen::Matrix4d H_12_expected = ::Eigen::Matrix4d::Identity();
    H_12_expected.block<3, 3>(0, 0) =
        ::Eigen::AngleAxisd(0.4, ::Eigen::Vector3d(0.1, 0.2, 0.3).normalized()).toRotationMatrix();
    H_12_expected.block<3, 1>(0, 3) << 0.5, 0.6, 0.7;

    std::vector< ::Eigen::Vector3d, ::Eigen::aligned_allocator< ::Eigen::Vector3d> > rvecs1, tvecs1, rvecs2, tvecs2;

    const size_t motionCount = 2;
    for (size_t i = 0; i < motionCount; ++i)
    {
        double droll  = ::camodocal::d2r( ::camodocal::random< double >(-10.0, 10.0) );
        double dpitch = ::camodocal::d2r(::camodocal::random< double >(-10.0, 10.0));
        double dyaw   = ::camodocal::d2r(::camodocal::random< double >(-10.0, 10.0));
        double dx     = ::camodocal::random< double >(-10.0, 10.0);
        double dy     = ::camodocal::random< double >(-10.0, 10.0);
        double dz     = ::camodocal::random< double >(-10.0, 10.0);

        ::Eigen::Matrix3d R;
        R = ::Eigen::AngleAxisd(dyaw, ::Eigen::Vector3d::UnitZ()) *
            ::Eigen::AngleAxisd(dpitch, ::Eigen::Vector3d::UnitY()) *
            ::Eigen::AngleAxisd(droll, ::Eigen::Vector3d::UnitX());

        ::Eigen::Matrix4d H = ::Eigen::Matrix4d::Identity();
        H.block<3, 3>(0, 0) = R;
        H.block<3, 1>(0, 3) << dx, dy, dz;

        ::Eigen::Matrix4d H_ = H.inverse();
        H                    = H_;

        ::Eigen::Vector3d rvec1, tvec1, rvec2, tvec2;

        ::Eigen::AngleAxisd angleAxis1((H_12_expected* H* H_12_expected.inverse()).block<3, 3>(0, 0));
        rvec1 = angleAxis1.angle() * angleAxis1.axis();

        tvec1 = (H_12_expected * H * H_12_expected.inverse()).block<3, 1>(0, 3);

        ::Eigen::AngleAxisd angleAxis2(H.block<3, 3>(0, 0));
        rvec2 = angleAxis2.angle() * angleAxis2.axis();

        tvec2 = H.block<3, 1>(0, 3);

        rvecs1.push_back(rvec1);
        tvecs1.push_back(tvec1);
        rvecs2.push_back(rvec2);
        tvecs2.push_back(tvec2);
    }

    ::Eigen::Matrix4d H_12;

    ::camodocal::HandEyeCalibration::estimateHandEyeScrew(rvecs1, tvecs1, rvecs2, tvecs2, H_12);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Elements differ at (" + std::to_string(i) + ","  + std::to_string(
                                                     j) + ")", H_12_expected(i, j), H_12(i, j), 0.001);
        }
    }

    rvecs1.clear();
    rvecs2.clear();
    tvecs1.clear();
    tvecs2.clear();
}

// ------------------------------------------------------------------------------

void HandEyeCalibrationTest::checkMultipleCalibration()
{
    const unsigned int nb_tests = 4;
    for(unsigned int i = 0; i < nb_tests; ++i)
    {
        checkCalibration();
    }

}

// ------------------------------------------------------------------------------

void HandEyeCalibrationTest::checkApi()
{
    ::camodocal::HandEyeCalibration::setVerbose(false);

    ::Eigen::Matrix4d H_12_expected = ::Eigen::Matrix4d::Identity();

    H_12_expected.block<3, 3>(0, 0) =
        ::Eigen::AngleAxisd(0.4, ::Eigen::Vector3d(0.1, 0.2, 0.3).normalized()).toRotationMatrix();

    H_12_expected.block<3, 1>(0, 3) << 0.5, 0.6, 0.7;

    ::fwData::TransformationMatrix3D::sptr fw_H_12_expected = ::eigenTools::helper::toF4s(H_12_expected);

    std::vector< ::fwData::TransformationMatrix3D::csptr > handEyeInputs1, handEyeInputs2;

    const size_t motionCount = 2;
    for (size_t i = 0; i < motionCount; ++i)
    {
        const double droll  = ::camodocal::d2r( ::camodocal::random(-10.0, 10.0) );
        const double dpitch = ::camodocal::d2r(::camodocal::random(-10.0, 10.0));
        const double dyaw   = ::camodocal::d2r(::camodocal::random(-10.0, 10.0));
        const double dx     = ::camodocal::random(-1.0, 1.0);
        const double dy     = ::camodocal::random(-1.0, 1.0);
        const double dz     = ::camodocal::random(-1.0, 1.0);

        ::Eigen::Matrix3d R;
        R = ::Eigen::AngleAxisd(dyaw, ::Eigen::Vector3d::UnitZ()) *
            ::Eigen::AngleAxisd(dpitch, ::Eigen::Vector3d::UnitY()) *
            ::Eigen::AngleAxisd(droll, ::Eigen::Vector3d::UnitX());

        ::Eigen::Matrix4d H = ::Eigen::Matrix4d::Identity();
        H.block<3, 3>(0, 0) = R;
        H.block<3, 1>(0, 3) << dx, dy, dz;

        ::Eigen::Matrix4d H_ = H.inverse();
        H                    = H_;

        ::fwData::TransformationMatrix3D::sptr fw_H = ::eigenTools::helper::toF4s(H);

        const ::Eigen::Matrix4d H12_ = H_12_expected * H
                                       * H_12_expected.inverse();

        ::fwData::TransformationMatrix3D::sptr fw_H12_ = ::eigenTools::helper::toF4s(H12_);

        handEyeInputs1.push_back(fw_H);
        handEyeInputs2.push_back(fw_H12_);
    }

    ::handEyeCalibration::HandEyeApi he_api;
    he_api.setTransformLists(handEyeInputs1, handEyeInputs2);
    ::fwData::TransformationMatrix3D::sptr fw_H_12 = he_api.computeHandEye();

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Elements differ at (" + std::to_string(i) + ","  + std::to_string(
                                                     j) + ")", fw_H_12_expected->getCoefficient(i, j),
                                                 fw_H_12->getCoefficient(i, j), 0.00001);
        }
    }
}
// ------------------------------------------------------------------------------
void HandEyeCalibrationTest::checkApi2()
{
    ::fwData::TransformationMatrix3D::sptr expectedMat            = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr mat1                   = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr mat2                   = ::fwData::TransformationMatrix3D::New();
    ::fwDataIO::reader::TransformationMatrix3DReader::sptr reader =
        ::fwDataIO::reader::TransformationMatrix3DReader::New();
    const ::boost::filesystem::path expectedMatrixPath( ::fwTest::Data::dir() / "fw4spl/arHandEye/expectedMatrix.trf" );
    CPPUNIT_ASSERT_MESSAGE("The file '" + expectedMatrixPath.string() + "' does not exist",
                           ::boost::filesystem::exists(expectedMatrixPath));
    reader->setObject( expectedMat );
    reader->setFile(expectedMatrixPath);
    reader->read();

    const boost::filesystem::path path1(::fwTest::Data::dir() / "fw4spl/arHandEye/matrix1");
    const boost::filesystem::path path2(::fwTest::Data::dir() / "fw4spl/arHandEye/matrix2");
    CPPUNIT_ASSERT_MESSAGE("The directory '" + path1.string() + "' does not exist",
                           ::boost::filesystem::exists(path1));
    CPPUNIT_ASSERT_MESSAGE("The directory '" + path2.string() + "' does not exist",
                           ::boost::filesystem::exists(path2));
    boost::filesystem::directory_iterator it2(path2);

    std::vector< ::fwData::TransformationMatrix3D::csptr > matList1, matList2;

    for(boost::filesystem::directory_iterator it(path1), end; it != end; ++it, ++it2)
    {
        if(boost::filesystem::is_regular_file(it->status()))
        {
            // get matrix
            ::fwDataIO::reader::TransformationMatrix3DReader::sptr reader =
                ::fwDataIO::reader::TransformationMatrix3DReader::New();
            reader->setObject( mat1 );
            std::string str = path1.string() + "/";
            str += it->path().filename().string();
            reader->setFile(str);
            reader->read();

            ::fwDataIO::reader::TransformationMatrix3DReader::sptr reader2 =
                ::fwDataIO::reader::TransformationMatrix3DReader::New();
            reader2->setObject( mat2 );
            std::string str2 = path2.string() + "/";
            str2 += it2->path().filename().string();
            reader2->setFile(str2);
            reader2->read();

            matList1.push_back(mat1);
            matList2.push_back(mat2);
        }
    }

    // compute hand eye calibration
    ::handEyeCalibration::HandEyeApi he_api;
    he_api.setTransformLists(matList1, matList2);
    ::fwData::TransformationMatrix3D::sptr actualMatrix = he_api.computeHandEye();

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Elements differ at (" + std::to_string(i) + ","  + std::to_string(
                                                     j) + ")", expectedMat->getCoefficient(i, j),
                                                 actualMatrix->getCoefficient(i, j), 0.000001);
        }
    }
}

// ------------------------------------------------------------------------------

} // namespace ut
} // namespace fwMath

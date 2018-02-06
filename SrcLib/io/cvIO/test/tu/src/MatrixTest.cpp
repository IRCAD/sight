/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MatrixTest.hpp"

#include <cvIO/Matrix.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <opencv2/core.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::MatrixTest );

namespace cvIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void MatrixTest::setUp()
{

}

//-----------------------------------------------------------------------------

void MatrixTest::tearDown()
{

}

//-----------------------------------------------------------------------------

void MatrixTest::copyFromCv()
{
    ::cv::Matx44f cvMat                          = ::cv::Matx44f::eye();
    ::fwData::TransformationMatrix3D::sptr fwMat = ::fwData::TransformationMatrix3D::New();

    //identity test
    ::cvIO::Matrix::copyFromCv(cvMat, fwMat);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("values are not the equals at [" + std::to_string(i)
                                                 + " ; " + std::to_string(j) + "]"
                                                 , static_cast<double>(cvMat(i, j)),
                                                 fwMat->getCoefficient(i, j), 1e-6);
        }
    }

    // 1 to 16 values
    cvMat = ::cv::Matx44f(1.f, 2.f, 3.f, 4.f,
                          5.f, 6.f, 7.f, 8.f,
                          9.f, 10.f, 11.f, 12.f,
                          13.f, 14.f, 15.f, 16.f);

    //values test
    ::cvIO::Matrix::copyFromCv(cvMat, fwMat);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("values are not the equals at [" + std::to_string(i)
                                                 + " ; " + std::to_string(j) + "]",
                                                 static_cast<double>(cvMat(i, j)),
                                                 fwMat->getCoefficient(i, j), 1e-6);
        }
    }
}

//-----------------------------------------------------------------------------

void MatrixTest::copyToCv()
{

    ::fwData::TransformationMatrix3D::sptr fwMat = ::fwData::TransformationMatrix3D::New();
    ::cv::Matx44f cvMat                          = ::cv::Matx44f::eye();

    //identity test
    ::cvIO::Matrix::copyToCv(fwMat, cvMat);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("values are not the equals at [" + std::to_string(i)
                                                 + " ; " + std::to_string(j) + "]"
                                                 ,fwMat->getCoefficient(i, j),
                                                 static_cast<double>(cvMat(i, j)) , 1e-6);
        }
    }


    ::fwData::TransformationMatrix3D::TMCoefArray array = {{0.16, 0.15, 0.14, 0.13,
                                                            12.0, 11.0, 10.0, 9.0,
                                                            0.08, 0.07, 0.06, 0.05,
                                                            40.0, 30.0, 20.0, 10.0}};
    fwMat->setCoefficients(array);

    //values test
    ::cvIO::Matrix::copyToCv(fwMat, cvMat);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("values are not the equals at [" + std::to_string(i)
                                                 + " ; " + std::to_string(j) + "]",
                                                 static_cast<double>(cvMat(i, j)),
                                                 fwMat->getCoefficient(i, j), 1e-6);
        }
    }

}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace cvIO

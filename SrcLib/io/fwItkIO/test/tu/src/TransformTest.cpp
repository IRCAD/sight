/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TransformTest.hpp"

#include <fwItkIO/helper/Transform.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <itkMatrix.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::TransformTest );

namespace fwItkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void TransformTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TransformTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransformTest::testConversion()
{
    itk::Matrix<double, 4, 4> mat;
    ::fwData::TransformationMatrix3D::sptr f4sMat = ::fwData::TransformationMatrix3D::New();

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; j++)
        {
            mat(i, j) = i+ j*4.0;
        }
    }

    ::fwItkIO::helper::Transform::convertFromITK(mat, f4sMat);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; j++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(mat(i, j), f4sMat->getCoefficient(i, j), 0);
        }
    }

    ::fwData::TransformationMatrix3D::csptr f4sMat2 = f4sMat;

    itk::Matrix<double, 4, 4> mat2 = ::fwItkIO::helper::Transform::convertToITK(f4sMat2);

    for(std::uint8_t i = 0; i < 4; ++i)
    {
        for(std::uint8_t j = 0; j < 4; j++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(mat(i, j), mat2(i, j), 0);
        }
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO

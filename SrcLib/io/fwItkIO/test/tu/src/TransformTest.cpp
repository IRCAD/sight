/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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
    ::itk::Matrix<double, 4, 4> mat;
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

    ::itk::Matrix<double, 4, 4> mat2 = ::fwItkIO::helper::Transform::convertToITK(f4sMat2);

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

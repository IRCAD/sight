/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <data/Matrix4.hpp>

#include <io/itk/helper/Transform.hpp>

#include <itkMatrix.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::TransformTest);

namespace sight::io::itk
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
    data::Matrix4::sptr sightMat = data::Matrix4::New();

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            mat(i, j) = i + j * 4.0;
        }
    }

    io::itk::helper::Transform::convertFromITK(mat, sightMat);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), sightMat->getCoefficient(i, j));
        }
    }

    data::Matrix4::csptr sightMat2 = sightMat;

    ::itk::Matrix<double, 4, 4> mat2 = io::itk::helper::Transform::convertToITK(sightMat2);

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), mat2(i, j));
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::io::itk

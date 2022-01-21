/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "Matrix4Test.hpp"

#include <data/Matrix4.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::Matrix4Test);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void Matrix4Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Matrix4Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Matrix4Test::getterSetterByCoef()
{
    double identity[] = {1., 0., 0., 0.,
                         0., 1., 0., 0.,
                         0., 0., 1., 0.,
                         0., 0., 0., 1.
    };

    data::Matrix4::sptr mat = data::Matrix4::New();

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    double coefs[] = {2, -2, .3, .12,
                      4, 8.9, 4.2, 1.2,
                      7.8, -12.1, 2.3, 1.2,
                      .3, 1.21, -3.1, 1.2
    };

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            mat->setCoefficient(i, j, coefs[i * 4 + j]);
        }
    }

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), coefs[i * 4 + j]);
        }
    }
}

//------------------------------------------------------------------------------

void Matrix4Test::getterSetterByArray()
{
    data::Matrix4::TMCoefArray identity = {1., 0., 0., 0.,
                                           0., 1., 0., 0.,
                                           0., 0., 1., 0.,
                                           0., 0., 0., 1.
    };

    data::Matrix4::sptr mat = data::Matrix4::New();

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    data::Matrix4::TMCoefArray coefs = {2, -2, .3, .12,
                                        4, 8.9, 4.2, 1.2,
                                        7.8, -12.1, 2.3, 1.2,
                                        .3, 1.21, -3.1, 1.2
    };

    mat->setCoefficients(coefs);

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), coefs[i * 4 + j]);
        }
    }

    const data::Matrix4::TMCoefArray& coefs2 = mat->getCoefficients();
    for(std::size_t i = 0 ; i < 16 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(coefs2[i], coefs[i]);
    }
}

//------------------------------------------------------------------------------

void Matrix4Test::getterSetterByMatrix()
{
    data::Matrix4::MatrixType matrix4x4 = {{{0., 0., 1., 100.},
        {0., 1., 0., 200.},
        {1., 0., 0., 3000.},
        {0., 0., 0., 1.
        }
    }
    };

    data::Matrix4::sptr mat = data::Matrix4::New();

    mat->setMatrix4x4(matrix4x4);

    const auto mat2 = mat->getMatrix4x4();

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(matrix4x4[i][j], mat2[i][j], 10e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(matrix4x4[i][j], mat->getCoefficient(i, j), 10e-8);
        }
    }

    data::Matrix4::TMCoefArray coefs = {1, -2, .3, .4,
                                        5.5, 6, 7.77, 8.,
                                        0.09, 10., -11., 1.2,
                                        0, 0, 0, 1.
    };

    mat->setCoefficients(coefs);

    const auto matFromCoefs = mat->getMatrix4x4();

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(coefs[i * 4 + j], matFromCoefs[i][j], 10e-8);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data

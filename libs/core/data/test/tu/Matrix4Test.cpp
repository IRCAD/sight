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

namespace sight::data::ut
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
    const std::array identity {
        1., 0., 0., 0.,
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
            CPPUNIT_ASSERT_EQUAL((*mat)(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    const std::array coefs = {
        2., -2., .3, .12,
        4., 8.9, 4.2, 1.2,
        7.8, -12.1, 2.3, 1.2,
        .3, 1.21, -3.1, 1.2
    };

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*mat)(i, j) = coefs[i * 4 + j];
        }
    }

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL((*mat)(i, j), coefs[i * 4 + j]);
        }
    }

    // Test == operator
    data::Matrix4::sptr mat2 = data::Matrix4::New();
    CPPUNIT_ASSERT(*mat != *mat2);

    for(size_t i = 0 ; i < 4 ; ++i)
    {
        for(size_t j = 0 ; j < 4 ; ++j)
        {
            (*mat2)(i, j) = coefs[i * 4 + j];
        }
    }

    CPPUNIT_ASSERT(*mat == *mat2);
}

//------------------------------------------------------------------------------

void Matrix4Test::getterSetterByArray()
{
    const std::array identity = {1., 0., 0., 0.,
                                 0., 1., 0., 0.,
                                 0., 0., 1., 0.,
                                 0., 0., 0., 1.
    };
    data::Matrix4 mat;

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    data::Matrix4::container_type coefs = {2, -2, .3, .12,
                                           4, 8.9, 4.2, 1.2,
                                           7.8, -12.1, 2.3, 1.2,
                                           .3, 1.21, -3.1, 1.2
    };

    mat = {2, -2, .3, .12,
           4, 8.9, 4.2, 1.2,
           7.8, -12.1, 2.3, 1.2,
           .3, 1.21, -3.1, 1.2
    };

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), coefs[i * 4 + j]);
        }
    }

    for(std::size_t i = 0 ; i < 16 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(mat[i], coefs[i]);
    }
}

//------------------------------------------------------------------------------

void Matrix4Test::rowMatrixAccessor()
{
    const std::array identity {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };

    data::Matrix4 mat;

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    const std::array coefs = {
        2., -2., .3, .12,
        4., 8.9, 4.2, 1.2,
        7.8, -12.1, 2.3, 1.2,
        .3, 1.21, -3.1, 1.2
    };

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            mat(i, j) = coefs[i * 4 + j];
        }
    }

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), coefs[i * 4 + j]);
        }
    }

    // Test == operator
    data::Matrix4 mat2;
    CPPUNIT_ASSERT(mat != mat2);

    for(size_t i = 0 ; i < 4 ; ++i)
    {
        for(size_t j = 0 ; j < 4 ; ++j)
        {
            mat2(i, j) = coefs[i * 4 + j];
        }
    }

    CPPUNIT_ASSERT(mat == mat2);
}

//------------------------------------------------------------------------------

void Matrix4Test::equalityTest()
{
    auto matrix1 = data::Matrix4::New();
    auto matrix2 = data::Matrix4::New();

    CPPUNIT_ASSERT(*matrix1 == *matrix2 && !(*matrix1 != *matrix2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
    *matrix1 = __VA_ARGS__; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Matrices should be different when the coefficients of the first matrix are set to " #__VA_ARGS__, \
        *matrix1 != *matrix2 && !(*matrix1 == *matrix2) \
    ); \
    *matrix2 = __VA_ARGS__; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Matrices should be equal when coefficients of both matrices are set to " #__VA_ARGS__, \
        *matrix1 == *matrix2 && !(*matrix1 != *matrix2) \
    );

    TEST({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0});
    TEST({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut

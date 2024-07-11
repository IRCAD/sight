/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "matrix4_test.hpp"

#include <data/matrix4.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::matrix4_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void matrix4_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void matrix4_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void matrix4_test::getter_setter_by_coef()
{
    const std::array identity {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };

    data::matrix4::sptr mat = std::make_shared<data::matrix4>();

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL((*mat)(i, j), identity[i * 4 + j]);
        }
    }

    CPPUNIT_ASSERT_EQUAL(true, mat->is_identity());
    CPPUNIT_ASSERT(*mat == mat->identity());

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

    CPPUNIT_ASSERT_EQUAL(false, mat->is_identity());
    CPPUNIT_ASSERT(*mat != mat->identity());
    CPPUNIT_ASSERT(*mat == coefs);

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL((*mat)(i, j), coefs[i * 4 + j]);
        }
    }

    // Test == operator
    data::matrix4::sptr mat2 = std::make_shared<data::matrix4>();
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

void matrix4_test::getter_setter_by_array()
{
    const std::array identity = {1., 0., 0., 0.,
                                 0., 1., 0., 0.,
                                 0., 0., 1., 0.,
                                 0., 0., 0., 1.
    };
    data::matrix4 mat;

    // Check default initialization
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    data::matrix4::container_t coefs = {2, -2, .3, .12,
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

void matrix4_test::row_matrix_accessor()
{
    const std::array identity {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };

    data::matrix4 mat;

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
    data::matrix4 mat2;
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

void matrix4_test::equality_test()
{
    auto matrix1 = std::make_shared<data::matrix4>();
    auto matrix2 = std::make_shared<data::matrix4>();

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

void matrix4_test::position_test()
{
    data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

    const auto& position = matrix.position();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.12, position[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, position[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, position[2], 1e-6);

    matrix.set_position(std::vector<double> {1.1, 2.2, 3.3});
    const auto& modified = matrix.position();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1, modified[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.2, modified[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3, modified[2], 1e-6);
}

//------------------------------------------------------------------------------

void matrix4_test::orientation_test()
{
    data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

    const auto& sparse = matrix.orientation(false);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2., sparse[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4., sparse[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.8, sparse[2], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-2., sparse[3], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.9, sparse[4], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-12.1, sparse[5], 1e-6);

    const auto& full = matrix.orientation(true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.3, full[6], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.2, full[7], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.3, full[8], 1e-6);

    matrix.set_orientation(std::vector<double> {1.1, 2.2, 3.3, 4.4, 5.5, 6.6});

    const auto& modified = matrix.orientation(true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1, modified[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.2, modified[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3, modified[2], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.4, modified[3], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.5, modified[4], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.6, modified[5], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.63, modified[6], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.26, modified[7], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.63, modified[8], 1e-6);

    matrix.set_orientation(std::vector<double> {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9});
    const auto& modified_b = matrix.orientation(true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1, modified_b[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.2, modified_b[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3, modified_b[2], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.4, modified_b[3], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.5, modified_b[4], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.6, modified_b[5], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.7, modified_b[6], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.8, modified_b[7], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.9, modified_b[8], 1e-6);
}

//------------------------------------------------------------------------------

void matrix4_test::value_test()
{
    const data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

    const auto& values = matrix.values();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2., values[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-2., values[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.3, values[2], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.12, values[3], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4, values[4], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.9, values[5], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.2, values[6], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, values[7], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.8, values[8], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-12.1, values[9], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.3, values[10], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, values[11], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.3, values[12], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.21, values[13], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.1, values[14], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.2, values[15], 1e-6);
}

//------------------------------------------------------------------------------

void matrix4_test::initialization_test()
{
    const data::matrix4 matrix({1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.});
    const data::matrix4 matrix2 = std::vector<double> {
        1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
    };

    CPPUNIT_ASSERT(core::is_equal(matrix, matrix2));

    const data::matrix4 matrix3 {
        1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
    };

    CPPUNIT_ASSERT(core::is_equal(matrix, matrix3));

    const data::matrix4 matrix3b = {
        1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
    };

    CPPUNIT_ASSERT(core::is_equal(matrix, matrix3b));

    data::matrix4 matrix4;
    CPPUNIT_ASSERT(!core::is_equal(matrix, matrix4));
    matrix4 = {1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.};
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix4));
    matrix4 = data::matrix4::identity();
    CPPUNIT_ASSERT(!core::is_equal(matrix, matrix4));
    matrix4 = matrix.values();
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix4));

    // Test if the fields is correctly set
    matrix4.set_field("test", std::make_shared<data::matrix4>());
    CPPUNIT_ASSERT(matrix != matrix4);
    matrix4 = matrix.values();
    CPPUNIT_ASSERT(sight::core::is_equal(matrix, matrix4));

    // copy from other container
    std::vector<double> vector {1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.};
    const data::matrix4 matrix5 = vector;
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix5));

    const data::matrix4 matrix6(vector);
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix6));

    // yeah we don't have to support that, but we can...
    std::list<std::uint8_t> list {1, 0, 0, 0, 0, 1, 0, 180, 0, 0, 1, 0, 0, 0, 0, 1};
    const data::matrix4 matrix7 = list;
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix7));

    const data::matrix4 matrix8(list);
    CPPUNIT_ASSERT(core::is_equal(matrix, matrix8));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut

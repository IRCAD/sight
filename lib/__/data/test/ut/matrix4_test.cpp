/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/matrix4.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::matrix4")
{
//------------------------------------------------------------------------------

    TEST_CASE("getter_setter_by_coef")
    {
        const std::array identity {
            1., 0., 0., 0.,
            0., 1., 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.
        };

        sight::data::matrix4::sptr mat = std::make_shared<sight::data::matrix4>();

        // Check default initialization
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ((*mat)(i, j), identity[i * 4 + j]);
            }
        }

        CHECK_EQ(true, mat->is_identity());
        CHECK(*mat == mat->identity());

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

        CHECK_EQ(false, mat->is_identity());
        CHECK(*mat != mat->identity());
        CHECK(*mat == coefs);

        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ((*mat)(i, j), coefs[i * 4 + j]);
            }
        }

        // Test == operator
        sight::data::matrix4::sptr mat2 = std::make_shared<sight::data::matrix4>();
        CHECK(*mat != *mat2);

        for(size_t i = 0 ; i < 4 ; ++i)
        {
            for(size_t j = 0 ; j < 4 ; ++j)
            {
                (*mat2)(i, j) = coefs[i * 4 + j];
            }
        }

        CHECK(*mat == *mat2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("getter_setter_by_array")
    {
        const std::array identity = {1., 0., 0., 0.,
                                     0., 1., 0., 0.,
                                     0., 0., 1., 0.,
                                     0., 0., 0., 1.
        };
        sight::data::matrix4 mat;

        // Check default initialization
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ(mat(i, j), identity[i * 4 + j]);
            }
        }

        // Call setter and check getter
        sight::data::matrix4::container_t coefs = {2, -2, .3, .12,
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
                CHECK_EQ(mat(i, j), coefs[i * 4 + j]);
            }
        }

        for(std::size_t i = 0 ; i < 16 ; ++i)
        {
            CHECK_EQ(mat[i], coefs[i]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("row_matrix_accessor")
    {
        const std::array identity {
            1., 0., 0., 0.,
            0., 1., 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.
        };

        sight::data::matrix4 mat;

        // Check default initialization
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ(mat(i, j), identity[i * 4 + j]);
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
                CHECK_EQ(mat(i, j), coefs[i * 4 + j]);
            }
        }

        // Test == operator
        sight::data::matrix4 mat2;
        CHECK(mat != mat2);

        for(size_t i = 0 ; i < 4 ; ++i)
        {
            for(size_t j = 0 ; j < 4 ; ++j)
            {
                mat2(i, j) = coefs[i * 4 + j];
            }
        }

        CHECK(mat == mat2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto matrix1 = std::make_shared<sight::data::matrix4>();
        auto matrix2 = std::make_shared<sight::data::matrix4>();

        CHECK(*matrix1 == *matrix2);
        CHECK(!(*matrix1 != *matrix2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
            *matrix1 = __VA_ARGS__; \
            CHECK_MESSAGE( \
                *matrix1 != *matrix2, \
                "Matrices should be different when the coefficients of the first matrix are set to " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                !(*matrix1 == *matrix2), \
                "Matrices should be different when the coefficients of the first matrix are set to " #__VA_ARGS__ \
            ); \
            *matrix2 = __VA_ARGS__; \
            CHECK_MESSAGE( \
                *matrix1 == *matrix2, \
                "Matrices should be equal when coefficients of both matrices are set to " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                !(*matrix1 != *matrix2), \
                "Matrices should be equal when coefficients of both matrices are set to " #__VA_ARGS__ \
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

    TEST_CASE("position")
    {
        sight::data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

        const auto& position = matrix.position();
        CHECK_EQ(.12, doctest::Approx(position[0]).epsilon(1e-6));
        CHECK_EQ(1.2, doctest::Approx(position[1]).epsilon(1e-6));
        CHECK_EQ(1.2, doctest::Approx(position[2]).epsilon(1e-6));

        matrix.set_position(std::vector<double> {1.1, 2.2, 3.3});
        const auto& modified = matrix.position();
        CHECK_EQ(1.1, doctest::Approx(modified[0]).epsilon(1e-6));
        CHECK_EQ(2.2, doctest::Approx(modified[1]).epsilon(1e-6));
        CHECK_EQ(3.3, doctest::Approx(modified[2]).epsilon(1e-6));
    }

//------------------------------------------------------------------------------

    TEST_CASE("orientation")
    {
        sight::data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

        const auto& full = matrix.orientation();
        CHECK_EQ(2., doctest::Approx(full[0]).epsilon(1e-6));
        CHECK_EQ(-2., doctest::Approx(full[1]).epsilon(1e-6));
        CHECK_EQ(.3, doctest::Approx(full[2]).epsilon(1e-6));
        CHECK_EQ(4, doctest::Approx(full[3]).epsilon(1e-6));
        CHECK_EQ(8.9, doctest::Approx(full[4]).epsilon(1e-6));
        CHECK_EQ(4.2, doctest::Approx(full[5]).epsilon(1e-6));
        CHECK_EQ(7.8, doctest::Approx(full[6]).epsilon(1e-6));
        CHECK_EQ(-12.1, doctest::Approx(full[7]).epsilon(1e-6));
        CHECK_EQ(2.3, doctest::Approx(full[8]).epsilon(1e-6));

        matrix.set_orientation(std::vector<double> {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9});

        const auto& modified = matrix.orientation();
        CHECK_EQ(1.1, doctest::Approx(modified[0]).epsilon(1e-6));
        CHECK_EQ(2.2, doctest::Approx(modified[1]).epsilon(1e-6));
        CHECK_EQ(3.3, doctest::Approx(modified[2]).epsilon(1e-6));
        CHECK_EQ(4.4, doctest::Approx(modified[3]).epsilon(1e-6));
        CHECK_EQ(5.5, doctest::Approx(modified[4]).epsilon(1e-6));
        CHECK_EQ(6.6, doctest::Approx(modified[5]).epsilon(1e-6));
        CHECK_EQ(7.7, doctest::Approx(modified[6]).epsilon(1e-6));
        CHECK_EQ(8.8, doctest::Approx(modified[7]).epsilon(1e-6));
        CHECK_EQ(9.9, doctest::Approx(modified[8]).epsilon(1e-6));
    }

//------------------------------------------------------------------------------

    TEST_CASE("value")
    {
        const sight::data::matrix4 matrix({
            2, -2, .3, .12,
            4, 8.9, 4.2, 1.2,
            7.8, -12.1, 2.3, 1.2,
            .3, 1.21, -3.1, 1.2
        });

        const auto& values = matrix.values();
        CHECK_EQ(2., doctest::Approx(values[0]).epsilon(1e-6));
        CHECK_EQ(-2., doctest::Approx(values[1]).epsilon(1e-6));
        CHECK_EQ(.3, doctest::Approx(values[2]).epsilon(1e-6));
        CHECK_EQ(.12, doctest::Approx(values[3]).epsilon(1e-6));
        CHECK_EQ(4, doctest::Approx(values[4]).epsilon(1e-6));
        CHECK_EQ(8.9, doctest::Approx(values[5]).epsilon(1e-6));
        CHECK_EQ(4.2, doctest::Approx(values[6]).epsilon(1e-6));
        CHECK_EQ(1.2, doctest::Approx(values[7]).epsilon(1e-6));
        CHECK_EQ(7.8, doctest::Approx(values[8]).epsilon(1e-6));
        CHECK_EQ(-12.1, doctest::Approx(values[9]).epsilon(1e-6));
        CHECK_EQ(2.3, doctest::Approx(values[10]).epsilon(1e-6));
        CHECK_EQ(1.2, doctest::Approx(values[11]).epsilon(1e-6));
        CHECK_EQ(.3, doctest::Approx(values[12]).epsilon(1e-6));
        CHECK_EQ(1.21, doctest::Approx(values[13]).epsilon(1e-6));
        CHECK_EQ(-3.1, doctest::Approx(values[14]).epsilon(1e-6));
        CHECK_EQ(1.2, doctest::Approx(values[15]).epsilon(1e-6));
    }

//------------------------------------------------------------------------------

    TEST_CASE("initialization")
    {
        const sight::data::matrix4 matrix({1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.});
        const sight::data::matrix4 matrix2 = std::vector<double> {
            1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
        };

        CHECK(sight::core::is_equal(matrix, matrix2));

        const sight::data::matrix4 matrix3 {
            1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
        };

        CHECK(sight::core::is_equal(matrix, matrix3));

        const sight::data::matrix4 matrix3b = {
            1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.
        };

        CHECK(sight::core::is_equal(matrix, matrix3b));

        sight::data::matrix4 matrix4;
        CHECK(!sight::core::is_equal(matrix, matrix4));
        matrix4 = {1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.};
        CHECK(sight::core::is_equal(matrix, matrix4));
        matrix4 = sight::data::matrix4::identity();
        CHECK(!sight::core::is_equal(matrix, matrix4));
        matrix4 = matrix.values();
        CHECK(sight::core::is_equal(matrix, matrix4));

        // Test if the fields is correctly set
        matrix4.set_field("test", std::make_shared<sight::data::matrix4>());
        CHECK(matrix != matrix4);
        matrix4 = matrix.values();
        CHECK(sight::core::is_equal(matrix, matrix4));

        // copy from other container
        std::vector<double> vector {1., 0., 0., 0., 0., 1., 0., 180., 0., 0., 1., 0., 0., 0., 0., 1.};
        const sight::data::matrix4 matrix5 = vector;
        CHECK(sight::core::is_equal(matrix, matrix5));

        const sight::data::matrix4 matrix6(vector);
        CHECK(sight::core::is_equal(matrix, matrix6));

        // yeah we don't have to support that, but we can...
        std::list<std::uint8_t> list {1, 0, 0, 0, 0, 1, 0, 180, 0, 0, 1, 0, 0, 0, 0, 1};
        const sight::data::matrix4 matrix7 = list;
        CHECK(sight::core::is_equal(matrix, matrix7));

        const sight::data::matrix4 matrix8(list);
        CHECK(sight::core::is_equal(matrix, matrix8));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::matrix4")

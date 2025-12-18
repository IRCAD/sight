/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <doctest/doctest.h>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/data/matrix4.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <cmath>

//------------------------------------------------------------------------------

TEST_SUITE("sight::geometry::data::matrix4")
{
    TEST_CASE("identity_matrix")
    {
        sight::data::matrix4 tm1;
        sight::data::matrix4 tm2;
        sight::data::matrix4 tm3;
        sight::data::matrix4 tm4;

        auto p1 = std::make_shared<sight::data::point>(1.0F, 2.3F, 5.1F);
        auto p2 = std::make_shared<sight::data::point>();

        bool should_be_true = sight::geometry::data::is_identity(tm1);
        CHECK_MESSAGE(should_be_true, "Matrix4 should be identity");
        // dummy precision
        should_be_true = sight::geometry::data::is_identity(tm1, 1);
        CHECK_MESSAGE(should_be_true, "Matrix4 should be identity");
        // high precision
        should_be_true = sight::geometry::data::is_identity(tm1, 1e-17);
        CHECK_MESSAGE(should_be_true, "Matrix4 should be identity");

        tm1(0, 2) = 3.4;
        tm1(1, 3) = 18;

        bool should_be_false = sight::geometry::data::is_identity(tm1);
        CHECK_MESSAGE(should_be_false == false, "Matrix4 should not be identity");
        //Test with dummy precision
        should_be_true = sight::geometry::data::is_identity(tm1, 20);
        CHECK_MESSAGE(should_be_true, "Matrix4 should be identity with a precision of 20");
        //Test with nice precision
        should_be_false = sight::geometry::data::is_identity(tm1, 1e-14);
        CHECK_MESSAGE(should_be_false == false, "Matrix4 should not be identity");

        sight::geometry::data::identity(tm1);
        sight::geometry::data::identity(tm2);
        CHECK(sight::geometry::data::is_identity(tm1));
        CHECK(sight::geometry::data::is_identity(tm2));

#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::multiply");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                geometry::data::multiply(tm1, tm2, tm3);
            }
        }
#else
        sight::geometry::data::multiply(tm1, tm2, tm3);
#endif
        CHECK(sight::geometry::data::is_identity(tm3));

#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::invert");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                geometry::data::invert(tm1, tm4);
            }
        }
#else
        sight::geometry::data::invert(tm1, tm4);
#endif
        CHECK(sight::geometry::data::is_identity(tm4));

#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::multiply");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                geometry::data::multiply(tm1, p1, p2);
            }
        }
#else
        sight::geometry::data::multiply(tm1, *p1, *p2);
#endif
        CHECK_EQ((*p2)[0], (*p1)[0]);
        CHECK_EQ((*p2)[1], (*p1)[1]);
        CHECK_EQ((*p2)[2], (*p1)[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("matrix")
    {
        sight::data::matrix4 tm1;
        sight::data::matrix4 tm2;
        sight::data::matrix4 tm3;
        sight::data::matrix4 tm4;

        sight::geometry::data::identity(tm2);

        sight::data::matrix4::container_t tm1_coefs {};
        for(std::size_t i = 0 ; i < 16 ; ++i)
        {
            tm1_coefs[i] = double(i + 1);
        }

        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                tm2(i, j) = std::abs(double(i) - double(j)) + 1.;
            }
        }

        tm1 = tm1_coefs;
        tm4 = tm1_coefs;

        // Test matrix-matrix multiplication
        sight::geometry::data::multiply(tm1, tm2, tm3);
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                double val = 0;
                for(std::size_t k = 0 ; k < 4 ; ++k)
                {
                    val += tm1(i, k) * tm2(k, j);
                }

                CHECK_EQ(tm3(i, j), val);
            }
        }

        // Test invert
        sight::geometry::data::invert(tm2, tm4);
        CHECK(tm4(0, 0) == doctest::Approx(-0.4).epsilon(1e-5));
        CHECK(tm4(0, 1) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(0, 2) == doctest::Approx(0.).epsilon(1e-5));
        CHECK(tm4(0, 3) == doctest::Approx(0.1).epsilon(1e-5));
        CHECK(tm4(1, 0) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(1, 1) == doctest::Approx(-1.).epsilon(1e-5));
        CHECK(tm4(1, 2) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(1, 3) == doctest::Approx(0.).epsilon(1e-5));
        CHECK(tm4(2, 0) == doctest::Approx(0.).epsilon(1e-5));
        CHECK(tm4(2, 1) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(2, 2) == doctest::Approx(-1.).epsilon(1e-5));
        CHECK(tm4(2, 3) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(3, 0) == doctest::Approx(0.1).epsilon(1e-5));
        CHECK(tm4(3, 1) == doctest::Approx(0.).epsilon(1e-5));
        CHECK(tm4(3, 2) == doctest::Approx(0.5).epsilon(1e-5));
        CHECK(tm4(3, 3) == doctest::Approx(-0.4).epsilon(1e-5));

        sight::data::matrix4::container_t tm2_coefs = {3.1, 1., -7.9689, 4.9,
                                                       5., -21., -1.3646, 14.4,
                                                       9., -7.2, -23.36, 79.04,
                                                       0.1, -3., -1.234, -49.94
        };
        tm2 = tm2_coefs;
        sight::geometry::data::invert(tm2, tm4);

        CHECK(tm4(0, 0) == doctest::Approx(0.885131908604589).epsilon(1e-4));
        CHECK(tm4(0, 1) == doctest::Approx(0.190212289294002).epsilon(1e-4));
        CHECK(tm4(0, 2) == doctest::Approx(-0.29581348830958).epsilon(1e-4));
        CHECK(tm4(0, 3) == doctest::Approx(-0.326489683580158).epsilon(1e-4));

        CHECK(tm4(1, 0) == doctest::Approx(0.185258037237573).epsilon(1e-4));
        CHECK(tm4(1, 1) == doctest::Approx(-0.00768780306376934).epsilon(1e-4));
        CHECK(tm4(1, 2) == doctest::Approx(-0.0586854220133396).epsilon(1e-4));
        CHECK(tm4(1, 3) == doctest::Approx(-0.0769210199757416).epsilon(1e-4));

        CHECK(tm4(2, 0) == doctest::Approx(0.232796743379289).epsilon(1e-4));
        CHECK(tm4(2, 1) == doctest::Approx(0.0724476129133489).epsilon(1e-4));
        CHECK(tm4(2, 2) == doctest::Approx(-0.118830425964518).epsilon(1e-4));
        CHECK(tm4(2, 3) == doctest::Approx(-0.144341353618837).epsilon(1e-4));

        CHECK(tm4(3, 0) == doctest::Approx(-0.015108772570731).epsilon(1e-4));
        CHECK(tm4(3, 1) == doctest::Approx(-0.000947451265806253).epsilon(1e-4));
        CHECK(tm4(3, 2) == doctest::Approx(0.00586927638865189).epsilon(1e-4));
        CHECK(tm4(3, 3) == doctest::Approx(-0.0124903619956978).epsilon(1e-4));

        // Test matrix-vector multiplication
        auto p1 = std::make_shared<sight::data::point>(1.0F, 2.3F, 5.1F);
        auto p2 = std::make_shared<sight::data::point>();

        sight::geometry::data::multiply(tm1, *p1, *p2);
        CHECK((*p2)[0] == doctest::Approx(24.9).epsilon(1e-5));
        CHECK((*p2)[1] == doctest::Approx(62.5).epsilon(1e-5));
        CHECK((*p2)[2] == doctest::Approx(100.1).epsilon(1e-5));

        sight::geometry::data::identity(tm1);
        sight::geometry::data::identity(tm2);
        CHECK(sight::geometry::data::is_identity(tm1));
        CHECK(sight::geometry::data::is_identity(tm2));
    }

//------------------------------------------------------------------------------

    TEST_CASE("glm_getter_setter")
    {
        sight::data::matrix4::container_t coefs = {2, -2, .3, .12,
                                                   4, 8.9, 4.2, 1.2,
                                                   7.8, -12.1, 2.3, 1.2,
                                                   .3, 1.21, -3.1, 1.2
        };

        sight::data::matrix4 mat;
        mat = coefs;

        // Test getter
        glm::dmat4x4 glm_mat;

#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::to_glm_mat");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                glmMat = geometry::data::to_glm_mat(mat);
            }
        }
#else
        glm_mat = sight::geometry::data::to_glm_mat(mat);
#endif

        // Matrix4 is stored row-major
        // glm matrices are stored column-major
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ(glm_mat[int(i)][int(j)], coefs[i + j * 4]);
            }
        }

        // Test setter
        std::array coefs2 = {11., -2., -.3, -.74,
                             .214, 82.9, 9.2, -5.2,
                             17.8, -2.1, 2.3, 1.2,
                             .13, 0.1, -0.1, 0.2
        };

        glm_mat = glm::make_mat4<double>(coefs2.data());
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::from_glm_mat");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                geometry::data::from_glm_mat(mat, glmMat);
            }
        }
#else
        sight::geometry::data::from_glm_mat(mat, glm_mat);
#endif

        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                CHECK_EQ(mat(i, j), coefs2[i + j * 4]);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("orthogonality")
    {
        sight::data::matrix4::container_t coefs = {2, -2, .3, .12,
                                                   4, 8.9, 4.2, 1.2,
                                                   7.8, -12.1, 2.3, 1.2,
                                                   .3, 1.21, -3.1, 1.2
        };

        sight::data::matrix4 mat;
        mat = coefs;

        CHECK_EQ(sight::geometry::data::is_orthogonal(mat), false);

        coefs = {
            0., 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -1., 40.,
            0., 0., 0., 1.
        };

        mat = coefs;

        CHECK_EQ(sight::geometry::data::is_orthogonal(mat), true);

        coefs = {
            0.001, 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -0.9999, 40.,
            0., 0., 0., 1.
        };

        mat = coefs;

        CHECK_EQ(sight::geometry::data::is_orthogonal(mat), false);

        coefs = {
            0.057, 0.99824, 0.01589, 80,
            0.98927, -0.05433, -0.13557, 3.79,
            -0.01344, 0.02345, -0.99063, -19,
            0, 0, 0, 1
        };

        mat = coefs;

        CHECK_EQ(sight::geometry::data::is_orthogonal(mat), false);

        coefs = {
            0.00107661, -0.02771048, -0.999615411, 0.51901669,
            0.549932075, 0.834904851, -0.02255222, 5.2933552,
            0.835208689, -0.54969629, 0.016137752, -20.156430847,
            0, 0, 0, 1
        };

        mat = coefs;

        CHECK_EQ(true, sight::geometry::data::is_orthogonal(mat));

        coefs = {
            0.304829245, 0.646320775, -0.699534551, 6.729738147,
            -0.386013738, -0.587616669, -0.7111259, 0.900330841,
            -0.870673605, 0.486801918, 0.070365953, 9.34067625,
            0, 0, 0, 1
        };

        mat = coefs;

        CHECK_EQ(sight::geometry::data::is_orthogonal(mat), true);
    }
}

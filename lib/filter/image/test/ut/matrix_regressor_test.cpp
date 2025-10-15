/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <filter/image/matrix_regressor.hpp>

#include <geometry/data/matrix4.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::matrix_regressor")
{
//------------------------------------------------------------------------------

    TEST_CASE("identity")
    {
        auto id       = std::make_shared<sight::data::matrix4>();
        auto mat_list = std::make_shared<sight::data::vector>();

        for(int i = 0 ; i < 5 ; ++i)
        {
            mat_list->push_back(id);
        }

        const std::vector<sight::filter::image::matrix_regressor::point_t> pt_list = {
            {
                {0., 0., 0., 1.},
                {0., 0., 1., 1.},
                {0., 1., 0., 1.},
                {0., 1., 1., 1.},
                {1., 0., 0., 1.},
                {1., 0., 1., 1.},
                {1., 1., 0., 1.},
                {1., 1., 1., 1.}
            }
        };

        sight::filter::image::matrix_regressor regressor(mat_list, pt_list);

        sight::data::matrix4::sptr res = regressor.minimize(*id);

        for(std::size_t i = 0 ; i < 16 ; ++i)
        {
            const double expected = (*id)[i];
            const double result   = (*res)[i];

            CHECK_EQ(doctest::Approx(expected).epsilon(1e-3), result);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("avg_translation")
    {
        sight::data::matrix4 id;

        auto trans1 = std::make_shared<sight::data::matrix4>();
        auto trans2 = std::make_shared<sight::data::matrix4>();

        glm::dmat4 t1 = glm::translate(glm::dmat4(1.), glm::dvec3(3, 3, 3));
        glm::dmat4 t2 = glm::translate(glm::dmat4(1.), glm::dvec3(5, 5, 5));

        sight::geometry::data::from_glm_mat(*trans1, t1);
        sight::geometry::data::from_glm_mat(*trans2, t2);

        auto mat_list = std::make_shared<sight::data::vector>();
        mat_list->push_back(trans1);
        mat_list->push_back(trans2);

        const std::vector<sight::filter::image::matrix_regressor::point_t> pt_list = {
            {
                {0., 0., 0., 1.},
                {0., 0., 1., 1.},
                {0., 1., 0., 1.},
                {0., 1., 1., 1.},
                {1., 0., 0., 1.},
                {1., 0., 1., 1.},
                {1., 1., 0., 1.},
                {1., 1., 1., 1.}
            }
        };

        sight::filter::image::matrix_regressor regressor(mat_list, pt_list);

        sight::data::matrix4::sptr res = regressor.minimize(id);

        const glm::dmat4 trans_expected = glm::translate(glm::dmat4(1.), glm::dvec3(4, 4, 4));
        sight::data::matrix4 expected_mat;
        sight::geometry::data::from_glm_mat(expected_mat, trans_expected);

        for(std::size_t i = 0 ; i < 16 ; ++i)
        {
            const double expected = expected_mat[i];
            const double result   = (*res)[i];

            CHECK_EQ(doctest::Approx(expected).epsilon(1e-3), result);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("avg_rotation")
    {
        auto id  = std::make_shared<sight::data::matrix4>();
        auto rot = std::make_shared<sight::data::matrix4>();

        glm::dmat4 r1 = glm::rotate(glm::dmat4(1.), glm::pi<double>() / 2., glm::dvec3(0., 0., 1.));
        sight::geometry::data::from_glm_mat(*rot, r1);

        auto mat_list = std::make_shared<sight::data::vector>();
        mat_list->push_back(id);
        mat_list->push_back(rot);

        const std::vector<sight::filter::image::matrix_regressor::point_t> pt_list = {
            {
                {0., 0., 0., 1.},
                {0., 0., 1., 1.},
                {0., 1., 0., 1.},
                {0., 1., 1., 1.},
                {1., 0., 0., 1.},
                {1., 0., 1., 1.},
                {1., 1., 0., 1.},
                {1., 1., 1., 1.}
            }
        };

        sight::filter::image::matrix_regressor regressor(mat_list, pt_list);

        sight::data::matrix4::sptr res = regressor.minimize(*id, 1., 1e-5, 1e-5);

        glm::dmat4 glm_res = sight::geometry::data::to_glm_mat(*res);

        // Extract the rotation from the result.
        double scale = std::pow(glm::determinant(glm_res), 1. / 3.);

        // Remove the scale from the matrix. This is required by the glm::toQuat() function.
        glm::dvec3 angles = glm::eulerAngles(glm::toQuat(glm_res / scale));

        CHECK(glm::all(glm::epsilonEqual(angles, glm::dvec3(0., 0., glm::pi<double>() / 4.), 1e-3)));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::matrix_regressor")

/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "matrix_regressor_test.hpp"

#include <filter/image/matrix_regressor.hpp>

#include <geometry/data/matrix4.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::matrix_regressor_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void matrix_regressor_test::setUp()
{
}

//------------------------------------------------------------------------------

void matrix_regressor_test::tearDown()
{
}

//------------------------------------------------------------------------------

void matrix_regressor_test::identityTest()
{
    auto id = std::make_shared<data::matrix4>();

    auto matList = std::make_shared<data::vector>();

    for(int i = 0 ; i < 5 ; ++i)
    {
        matList->push_back(id);
    }

    const std::vector<matrix_regressor::PointType> ptList = {{
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

    matrix_regressor regressor(matList, ptList);

    data::matrix4::sptr res = regressor.minimize(*id);

    for(std::size_t i = 0 ; i < 16 ; ++i)
    {
        const double expected = (*id)[i];
        const double result   = (*res)[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, result, 1e-3);
    }
}

//------------------------------------------------------------------------------

void matrix_regressor_test::avgTranslationTest()
{
    data::matrix4 id;
    auto trans1 = std::make_shared<data::matrix4>();
    auto trans2 = std::make_shared<data::matrix4>();

    glm::dmat4 t1 = glm::translate(glm::dmat4(1.), glm::dvec3(3, 3, 3));
    glm::dmat4 t2 = glm::translate(glm::dmat4(1.), glm::dvec3(5, 5, 5));

    geometry::data::setTF3DFromMatrix(*trans1, t1);
    geometry::data::setTF3DFromMatrix(*trans2, t2);

    auto matList = std::make_shared<data::vector>();

    matList->push_back(trans1);
    matList->push_back(trans2);

    const std::vector<matrix_regressor::PointType> ptList = {{
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

    matrix_regressor regressor(matList, ptList);

    data::matrix4::sptr res = regressor.minimize(id);

    const glm::dmat4 transExpected = glm::translate(glm::dmat4(1.), glm::dvec3(4, 4, 4));
    data::matrix4 expectedMat;

    geometry::data::setTF3DFromMatrix(expectedMat, transExpected);

    for(std::size_t i = 0 ; i < 16 ; ++i)
    {
        const double expected = expectedMat[i];
        const double result   = (*res)[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, result, 1e-3);
    }
}

//------------------------------------------------------------------------------

void matrix_regressor_test::avgRotationTest()
{
    data::matrix4::sptr id  = std::make_shared<data::matrix4>();
    data::matrix4::sptr rot = std::make_shared<data::matrix4>();

    glm::dmat4 r1 = glm::rotate(glm::dmat4(1.), glm::pi<double>() / 2., glm::dvec3(0., 0., 1.));

    geometry::data::setTF3DFromMatrix(*rot, r1);

    auto matList = std::make_shared<data::vector>();

    matList->push_back(id);
    matList->push_back(rot);

    const std::vector<matrix_regressor::PointType> ptList = {{
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

    matrix_regressor regressor(matList, ptList);

    data::matrix4::sptr res = regressor.minimize(*id, 1., 1e-5, 1e-5);

    glm::dmat4 glmRes = geometry::data::getMatrixFromTF3D(*res);

    // Extract the rotation from the result.
    double scale = std::pow(glm::determinant(glmRes), 1. / 3.);

    // Remove the scale from the matrix. This is required by the glm::toQuat() function.
    glm::dvec3 angles = glm::eulerAngles(glm::toQuat(glmRes / scale));

    CPPUNIT_ASSERT(glm::all(glm::epsilonEqual(angles, glm::dvec3(0., 0., glm::pi<double>() / 4.), 1e-3)));
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut

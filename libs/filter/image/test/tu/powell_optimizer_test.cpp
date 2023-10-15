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

// cspell:ignore NOLINT

#include "powell_optimizer_test.hpp"

#include <filter/image/powell_optimizer.hpp>

#define _USE_MATH_DEFINES // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
#include <math.h> // NOLINT(hicpp-deprecated-headers,modernize-deprecated-headers)

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::powell_optimizer_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void powell_optimizer_test::setUp()
{
}

//------------------------------------------------------------------------------

void powell_optimizer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void powell_optimizer_test::parabolaTest()
{
    filter::image::powell_optimizer::OptimizedFunctionType xSquared =
        [](const filter::image::powell_optimizer::FunctionParametersType& p)
        {
            return p[0] * p[0];
        };

    filter::image::powell_optimizer optimizer(xSquared, 1e-9, 1e-9, 1.0, 200);

    filter::image::powell_optimizer::FunctionParametersType initParams(1);
    filter::image::powell_optimizer::FunctionParametersType finalParams(1);
    initParams[0] = 9.0;
    finalParams   = optimizer.optimize(initParams);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParams[0], 1e-9);
}

//------------------------------------------------------------------------------

void powell_optimizer_test::ackleyTest()
{
    filter::image::powell_optimizer::OptimizedFunctionType ackleyFunction =
        [](const filter::image::powell_optimizer::FunctionParametersType& p)
        {
            const double x   = p[0];
            const double y   = p[1];
            const double res = -20 * std::exp(-0.2 * std::sqrt(0.5 * (x * x + y * y)))
                               - std::exp(0.5 * (std::cos(2 * M_PI * x) + std::cos(2 * M_PI * y))) + M_E + 20;
            return res;
        };

    filter::image::powell_optimizer optimizer(ackleyFunction, 1e-9, 1e-9, 1.0, 200);

    filter::image::powell_optimizer::FunctionParametersType initParams(2);
    filter::image::powell_optimizer::FunctionParametersType finalParams(2);
    initParams[0] = 1.0;
    initParams[1] = 1.5;

    finalParams = optimizer.optimize(initParams);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParams[0], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParams[1], 1e-9);
}

//------------------------------------------------------------------------------

void powell_optimizer_test::sphereFunctionTest()
{
    // n-dimensional sphere function.
    filter::image::powell_optimizer::OptimizedFunctionType sphereFunction =
        [](const filter::image::powell_optimizer::FunctionParametersType& p)
        {
            double res(0.);
            for(const auto e : p)
            {
                res += e * e;
            }

            return res;
        };

    filter::image::powell_optimizer optimizer(sphereFunction, 1e-9, 1e-9, 1.0, 200);

    // 2D sphere function test.
    filter::image::powell_optimizer::FunctionParametersType initParams(2);
    filter::image::powell_optimizer::FunctionParametersType finalParams(2);
    initParams[0] = 1.0;
    initParams[1] = 1.5;

    finalParams = optimizer.optimize(initParams);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParams[0], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParams[1], 1e-9);

    // 5D sphere function test.
    filter::image::powell_optimizer::FunctionParametersType initParams2(5);
    filter::image::powell_optimizer::FunctionParametersType finalParams2(5);
    initParams2[0] = 1.0;
    initParams2[1] = 1.5;
    initParams2[2] = 0.9;
    initParams2[3] = 0.4;
    initParams2[4] = 1.2;

    finalParams2 = optimizer.optimize(initParams2);

    for(const double finalParam : finalParams2)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0, finalParam, 1e-9);
    }
}

//------------------------------------------------------------------------------

void powell_optimizer_test::bealeFunctionTest()
{
    filter::image::powell_optimizer::OptimizedFunctionType bealeFunction =
        [](const filter::image::powell_optimizer::FunctionParametersType& p)
        {
            const double x = p[0];
            const double y = p[1];

            return std::pow(1.5 - x + x * y, 2.) + std::pow(2.25 - x + x * std::pow(y, 2.), 2.)
                   + std::pow(2.625 - x + x * std::pow(y, 3.), 2.);
        };

    filter::image::powell_optimizer optimizer(bealeFunction, 1e-9, 1e-9, 1.0, 200);

    filter::image::powell_optimizer::FunctionParametersType initParams(2);
    filter::image::powell_optimizer::FunctionParametersType finalParams(2);
    initParams[0] = 0;
    initParams[1] = 0;

    finalParams = optimizer.optimize(initParams);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(3., finalParams[0], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, finalParams[1], 1e-9);
}

} // namespace sight::filter::image::ut

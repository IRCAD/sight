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

// cspell:ignore NOLINT

#include <filter/image/detail/powell_optimizer.hpp>

#include <boost/math/constants/constants.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::detail::powell_optimizer")
{
//------------------------------------------------------------------------------

    TEST_CASE("parabola")
    {
        sight::filter::image::detail::powell_optimizer::optimized_function_t x_squared =
            [](const sight::filter::image::detail::powell_optimizer::function_parameters_t& _p)
            {
                return _p[0] * _p[0];
            };

        sight::filter::image::detail::powell_optimizer optimizer(x_squared, 1e-9, 1e-9, 1.0, 200);

        sight::filter::image::detail::powell_optimizer::function_parameters_t init_params(1);
        sight::filter::image::detail::powell_optimizer::function_parameters_t final_params(1);

        init_params[0] = 9.0;

        final_params = optimizer.optimize(init_params);

        CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_params[0]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("ackley")
    {
        sight::filter::image::detail::powell_optimizer::optimized_function_t ackley_function =
            [](const sight::filter::image::detail::powell_optimizer::function_parameters_t& _p)
            {
                const double x  = _p[0];
                const double y  = _p[1];
                const double pi = boost::math::constants::pi<double>();
                const double e  = boost::math::constants::e<double>();

                const double res = -20 * std::exp(-0.2 * std::sqrt(0.5 * (x * x + y * y)))
                                   - std::exp(0.5 * (std::cos(2 * pi * x) + std::cos(2 * pi * y)))
                                   + e + 20;

                return res;
            };

        sight::filter::image::detail::powell_optimizer optimizer(ackley_function, 1e-9, 1e-9, 1.0, 200);

        sight::filter::image::detail::powell_optimizer::function_parameters_t init_params(2);
        sight::filter::image::detail::powell_optimizer::function_parameters_t final_params(2);

        init_params[0] = 1.0;
        init_params[1] = 1.5;

        final_params = optimizer.optimize(init_params);

        CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_params[0]);
        CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_params[1]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("sphere_function")
    {
        // n-dimensional sphere function.
        sight::filter::image::detail::powell_optimizer::optimized_function_t sphere_function =
            [](const sight::filter::image::detail::powell_optimizer::function_parameters_t& _p)
            {
                double res(0.);

                for(const auto e : _p)
                {
                    res += e * e;
                }

                return res;
            };

        sight::filter::image::detail::powell_optimizer optimizer(sphere_function, 1e-9, 1e-9, 1.0, 200);

        // 2D sphere function test.
        sight::filter::image::detail::powell_optimizer::function_parameters_t init_params(2);
        sight::filter::image::detail::powell_optimizer::function_parameters_t final_params(2);

        init_params[0] = 1.0;
        init_params[1] = 1.5;

        final_params = optimizer.optimize(init_params);

        CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_params[0]);
        CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_params[1]);

        // 5D sphere function test.
        sight::filter::image::detail::powell_optimizer::function_parameters_t init_params2(5);
        sight::filter::image::detail::powell_optimizer::function_parameters_t final_params2(5);

        init_params2[0] = 1.0;
        init_params2[1] = 1.5;
        init_params2[2] = 0.9;
        init_params2[3] = 0.4;
        init_params2[4] = 1.2;

        final_params2 = optimizer.optimize(init_params2);

        for(const double final_param : final_params2)
        {
            CHECK_EQ(doctest::Approx(0).epsilon(1e-9), final_param);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("beale_function")
    {
        sight::filter::image::detail::powell_optimizer::optimized_function_t beale_function =
            [](const sight::filter::image::detail::powell_optimizer::function_parameters_t& _p)
            {
                const double x = _p[0];
                const double y = _p[1];

                return std::pow(1.5 - x + x * y, 2.)
                       + std::pow(2.25 - x + x * std::pow(y, 2.), 2.)
                       + std::pow(2.625 - x + x * std::pow(y, 3.), 2.);
            };

        sight::filter::image::detail::powell_optimizer optimizer(beale_function, 1e-9, 1e-9, 1.0, 200);

        sight::filter::image::detail::powell_optimizer::function_parameters_t init_params(2);
        sight::filter::image::detail::powell_optimizer::function_parameters_t final_params(2);

        init_params[0] = 0;
        init_params[1] = 0;

        final_params = optimizer.optimize(init_params);

        CHECK_EQ(doctest::Approx(3.).epsilon(1e-9), final_params[0]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(1e-9), final_params[1]);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::detail::powell_optimizer")

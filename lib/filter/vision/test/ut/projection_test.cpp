/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <filter/vision/projection.hpp>

#include <doctest/doctest.h>

#include <cmath>

TEST_SUITE("sight::filter::vision::projection")
{
    TEST_CASE("point")
    {
        {
            const std::size_t p_x   = 42;
            const std::size_t p_y   = 53;
            const std::size_t depth = 155;

            const std::size_t width  = 640;
            const std::size_t height = 480;
            const double cx          = 321.3;
            const double cy          = 239.3;
            const double fx          = 565.53;
            const double fy          = 563.25;

            double x = NAN;
            double y = NAN;
            double z = NAN;
            sight::filter::vision::project_pixel(p_x, p_y, static_cast<double>(depth), cx, cy, fx, fy, x, y, z);
            std::size_t p_x2 = 0;
            std::size_t p_y2 = 0;
            bool success     = sight::filter::vision::project_point(x, y, z, cx, cy, fx, fy, width, height, p_x2, p_y2);

            CHECK(success);
            CHECK_EQ(p_x, p_x2);
            CHECK_EQ(p_y, p_y2);

            CHECK(double(depth * (p_x - cx) / fx) == doctest::Approx(x).epsilon(0.0001));
            CHECK(double(depth * (p_y - cy) / fy) == doctest::Approx(y).epsilon(0.0001));
            CHECK(double(depth) == doctest::Approx(z).epsilon(0.0001));
        }

        {
            const double x = 63.45;
            const double y = 25.4;
            const double z = 156;

            const std::size_t width  = 640;
            const std::size_t height = 480;
            const double cx          = 321.3;
            const double cy          = 239.3;
            const double fx          = 565.53;
            const double fy          = 563.25;

            std::size_t p_x = 0;
            std::size_t p_y = 0;
            bool success    = sight::filter::vision::project_point(x, y, z, cx, cy, fx, fy, width, height, p_x, p_y);
            CHECK(success);
            double x2 = NAN;
            double y2 = NAN;
            double z2 = NAN;
            sight::filter::vision::project_pixel(p_x, p_y, z, cx, cy, fx, fy, x2, y2, z2);
            CHECK(x == doctest::Approx(x2).epsilon(0.1));
            CHECK(y == doctest::Approx(y2).epsilon(0.1));
            CHECK(z == doctest::Approx(z2).epsilon(0.001));
        }
    }
}

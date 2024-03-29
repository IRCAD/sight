/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "projection_test.hpp"

#include <filter/vision/projection.hpp>

#include <cmath>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::vision::ut::projection_test);

namespace sight::filter::vision::ut
{

//------------------------------------------------------------------------------

void projection_test::setUp()
{
}

//------------------------------------------------------------------------------

void projection_test::tearDown()
{
}

//------------------------------------------------------------------------------

void projection_test::projection()
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
        filter::vision::project_pixel(p_x, p_y, static_cast<double>(depth), cx, cy, fx, fy, x, y, z);
        std::size_t p_x2 = 0;
        std::size_t p_y2 = 0;
        bool success     = filter::vision::project_point(x, y, z, cx, cy, fx, fy, width, height, p_x2, p_y2);

        CPPUNIT_ASSERT(success);
        CPPUNIT_ASSERT_EQUAL(p_x, p_x2);
        CPPUNIT_ASSERT_EQUAL(p_y, p_y2);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(depth * (p_x - cx) / fx, x, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(depth * (p_y - cy) / fy, y, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(double(depth), z, 0.0001);
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
        bool success    = filter::vision::project_point(x, y, z, cx, cy, fx, fy, width, height, p_x, p_y);
        CPPUNIT_ASSERT(success);
        double x2 = NAN;
        double y2 = NAN;
        double z2 = NAN;
        filter::vision::project_pixel(p_x, p_y, z, cx, cy, fx, fy, x2, y2, z2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(x, x2, 0.1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(y, y2, 0.1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(z, z2, 0.001);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::vision::ut

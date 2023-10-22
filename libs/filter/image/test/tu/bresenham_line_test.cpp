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

#include "bresenham_line_test.hpp"

#include <data/helper/medical_image.hpp>

#include <filter/image/bresenham_line.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::bresenham_line_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void bresenham_line_test::setUp()
{
}

//------------------------------------------------------------------------------

void bresenham_line_test::tearDown()
{
}

//------------------------------------------------------------------------------

void bresenham_line_test::straightLineTest()
{
    {
        const filter::image::bresenham_line::coordinates_t start_point = {{0, 0, 0}};
        const filter::image::bresenham_line::coordinates_t end_point   = {{0, 0, 1024}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(bresenham_line::Orientation::X_AXIS, start_point, end_point);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(end_point[2] - start_point[2] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = start_point[2] ; i <= end_point[2] ; ++i)
        {
            const auto& path_elt = *iter;

            const filter::image::bresenham_line::coordinates_t expected_elt = {{0, 0, i}};

            CPPUNIT_ASSERT(path_elt == expected_elt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::coordinates_t start_point = {{25, 2057, 3}};
        const filter::image::bresenham_line::coordinates_t end_point   = {{25, 1313, 3}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Z_AXIS,
                start_point,
                end_point
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(start_point[1] - end_point[1] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = start_point[1] ; i >= end_point[1] ; --i)
        {
            const auto& path_elt = *iter;

            const filter::image::bresenham_line::coordinates_t expected_elt = {{25, i, 3}};

            CPPUNIT_ASSERT(path_elt == expected_elt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::coordinates_t start_point = {{90, 67, 68}};
        const filter::image::bresenham_line::coordinates_t end_point   = {{90, 67, 54}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Y_AXIS,
                start_point,
                end_point
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(start_point[2] - end_point[2] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = start_point[2] ; i >= end_point[2] ; --i)
        {
            const auto& path_elt = *iter;

            const filter::image::bresenham_line::coordinates_t expected_elt = {{90, 67, i}};

            CPPUNIT_ASSERT(path_elt == expected_elt);
            ++iter;
        }
    }
    {
        const filter::image::bresenham_line::coordinates_t start_point = {{90, 67, 68}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Y_AXIS,
                start_point,
                start_point
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), result.size());

        CPPUNIT_ASSERT(start_point == result[0]);
    }
}

//------------------------------------------------------------------------------

void bresenham_line_test::obliqueLineTest()
{
    {
        const filter::image::bresenham_line::coordinates_t start_point = {{1, 1, 42}};
        const filter::image::bresenham_line::coordinates_t end_point   = {{11, 5, 42}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Z_AXIS,
                start_point,
                end_point
            );

        CPPUNIT_ASSERT(!result.empty());

        const bresenham_line::path_t expected_path = {
            {{1, 1, 42}},
            {{2, 1, 42}},
            {{3, 2, 42}},
            {{4, 2, 42}},
            {{5, 3, 42}},
            {{6, 3, 42}},
            {{7, 3, 42}},
            {{8, 4, 42}},
            {{9, 4, 42}},
            {{10, 5, 42}},
            {{11, 5, 42}}
        };

        CPPUNIT_ASSERT_EQUAL(expected_path.size(), result.size());

        auto iter = result.begin();
        for(const auto& expected_path_elt : expected_path)
        {
            const auto& res_elt = *iter;
            CPPUNIT_ASSERT(expected_path_elt == res_elt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::coordinates_t start_point = {{1, 4, 1}};
        const filter::image::bresenham_line::coordinates_t end_point   = {{1, 2, 5}};

        const filter::image::bresenham_line::path_t result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::X_AXIS,
                start_point,
                end_point
            );

        CPPUNIT_ASSERT(!result.empty());

        const bresenham_line::path_t expected_path = {
            {{1, 4, 1}},
            {{1, 4, 2}},
            {{1, 3, 3}},
            {{1, 3, 4}},
            {{1, 2, 5}},
        };

        CPPUNIT_ASSERT_EQUAL(expected_path.size(), result.size());

        auto iter = result.begin();
        for(const auto& expected_path_elt : expected_path)
        {
            const auto& res_elt = *iter;
            CPPUNIT_ASSERT(expected_path_elt == res_elt);
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut

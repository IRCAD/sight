/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <filter/image/bresenham_line.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::bresenham_line")
{
//------------------------------------------------------------------------------

    TEST_CASE("straight_line")
    {
        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{0, 0, 0}};
            const sight::filter::image::bresenham_line::coordinates_t end_point   = {{0, 0, 1024}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::x_axis,
                start_point,
                end_point
            );

            CHECK(!result.empty());
            CHECK_EQ(std::size_t(end_point[2] - start_point[2] + 1), result.size());

            auto iter = result.begin();

            for(std::size_t i = start_point[2] ; i <= end_point[2] ; ++i)
            {
                const auto& path_elt                                                   = *iter;
                const sight::filter::image::bresenham_line::coordinates_t expected_elt = {{0, 0, i}};

                CHECK(path_elt == expected_elt);
                ++iter;
            }
        }

        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{25, 2057, 3}};
            const sight::filter::image::bresenham_line::coordinates_t end_point   = {{25, 1313, 3}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::z_axis,
                start_point,
                end_point
            );

            CHECK(!result.empty());
            CHECK_EQ(std::size_t(start_point[1] - end_point[1] + 1), result.size());

            auto iter = result.begin();

            for(std::size_t i = start_point[1] ; i >= end_point[1] ; --i)
            {
                const auto& path_elt                                                   = *iter;
                const sight::filter::image::bresenham_line::coordinates_t expected_elt = {{25, i, 3}};

                CHECK(path_elt == expected_elt);
                ++iter;
            }
        }

        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{90, 67, 68}};
            const sight::filter::image::bresenham_line::coordinates_t end_point   = {{90, 67, 54}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::y_axis,
                start_point,
                end_point
            );

            CHECK(!result.empty());
            CHECK_EQ(std::size_t(start_point[2] - end_point[2] + 1), result.size());

            auto iter = result.begin();

            for(std::size_t i = start_point[2] ; i >= end_point[2] ; --i)
            {
                const auto& path_elt                                                   = *iter;
                const sight::filter::image::bresenham_line::coordinates_t expected_elt = {{90, 67, i}};

                CHECK(path_elt == expected_elt);
                ++iter;
            }
        }

        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{90, 67, 68}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::y_axis,
                start_point,
                start_point
            );

            CHECK(!result.empty());
            CHECK_EQ(std::size_t(1), result.size());
            CHECK(start_point == result[0]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("oblique_line")
    {
        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{1, 1, 42}};
            const sight::filter::image::bresenham_line::coordinates_t end_point   = {{11, 5, 42}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::z_axis,
                start_point,
                end_point
            );

            CHECK(!result.empty());

            const sight::filter::image::bresenham_line::path_t expected_path = {
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

            CHECK_EQ(expected_path.size(), result.size());

            auto iter = result.begin();

            for(const auto& expected_path_elt : expected_path)
            {
                const auto& res_elt = *iter;
                CHECK(expected_path_elt == res_elt);
                ++iter;
            }
        }

        {
            const sight::filter::image::bresenham_line::coordinates_t start_point = {{1, 4, 1}};
            const sight::filter::image::bresenham_line::coordinates_t end_point   = {{1, 2, 5}};

            const auto result = sight::filter::image::bresenham_line::draw(
                sight::filter::image::bresenham_line::orientation::x_axis,
                start_point,
                end_point
            );

            CHECK(!result.empty());

            const sight::filter::image::bresenham_line::path_t expected_path = {
                {{1, 4, 1}},
                {{1, 4, 2}},
                {{1, 3, 3}},
                {{1, 3, 4}},
                {{1, 2, 5}},
            };

            CHECK_EQ(expected_path.size(), result.size());

            auto iter = result.begin();

            for(const auto& expected_path_elt : expected_path)
            {
                const auto& res_elt = *iter;
                CHECK(expected_path_elt == res_elt);
                ++iter;
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::bresenham_line")

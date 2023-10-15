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

#include <data/helper/MedicalImage.hpp>

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
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{0, 0, 0}};
        const filter::image::bresenham_line::CoordinatesType END_POINT   = {{0, 0, 1024}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(bresenham_line::Orientation::X_AXIS, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(END_POINT[2] - START_POINT[2] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = START_POINT[2] ; i <= END_POINT[2] ; ++i)
        {
            const auto& pathElt = *iter;

            const filter::image::bresenham_line::CoordinatesType expectedElt = {{0, 0, i}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{25, 2057, 3}};
        const filter::image::bresenham_line::CoordinatesType END_POINT   = {{25, 1313, 3}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Z_AXIS,
                START_POINT,
                END_POINT
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(START_POINT[1] - END_POINT[1] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = START_POINT[1] ; i >= END_POINT[1] ; --i)
        {
            const auto& pathElt = *iter;

            const filter::image::bresenham_line::CoordinatesType expectedElt = {{25, i, 3}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{90, 67, 68}};
        const filter::image::bresenham_line::CoordinatesType END_POINT   = {{90, 67, 54}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Y_AXIS,
                START_POINT,
                END_POINT
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(START_POINT[2] - END_POINT[2] + 1), result.size());

        auto iter = result.begin();
        for(std::size_t i = START_POINT[2] ; i >= END_POINT[2] ; --i)
        {
            const auto& pathElt = *iter;

            const filter::image::bresenham_line::CoordinatesType expectedElt = {{90, 67, i}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }
    {
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{90, 67, 68}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Y_AXIS,
                START_POINT,
                START_POINT
            );

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), result.size());

        CPPUNIT_ASSERT(START_POINT == result[0]);
    }
}

//------------------------------------------------------------------------------

void bresenham_line_test::obliqueLineTest()
{
    {
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{1, 1, 42}};
        const filter::image::bresenham_line::CoordinatesType END_POINT   = {{11, 5, 42}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::Z_AXIS,
                START_POINT,
                END_POINT
            );

        CPPUNIT_ASSERT(!result.empty());

        const bresenham_line::PathType expectedPath = {
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

        CPPUNIT_ASSERT_EQUAL(expectedPath.size(), result.size());

        auto iter = result.begin();
        for(const auto& expectedPathElt : expectedPath)
        {
            const auto& resElt = *iter;
            CPPUNIT_ASSERT(expectedPathElt == resElt);
            ++iter;
        }
    }

    {
        const filter::image::bresenham_line::CoordinatesType START_POINT = {{1, 4, 1}};
        const filter::image::bresenham_line::CoordinatesType END_POINT   = {{1, 2, 5}};

        const filter::image::bresenham_line::PathType result =
            filter::image::bresenham_line::draw(
                filter::image::bresenham_line::Orientation::X_AXIS,
                START_POINT,
                END_POINT
            );

        CPPUNIT_ASSERT(!result.empty());

        const bresenham_line::PathType expectedPath = {
            {{1, 4, 1}},
            {{1, 4, 2}},
            {{1, 3, 3}},
            {{1, 3, 4}},
            {{1, 2, 5}},
        };

        CPPUNIT_ASSERT_EQUAL(expectedPath.size(), result.size());

        auto iter = result.begin();
        for(const auto& expectedPathElt : expectedPath)
        {
            const auto& resElt = *iter;
            CPPUNIT_ASSERT(expectedPathElt == resElt);
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut

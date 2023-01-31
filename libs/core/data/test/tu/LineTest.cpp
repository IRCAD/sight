/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "LineTest.hpp"

#include <data/Line.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::LineTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void LineTest::equalityTest()
{
    auto line1 = data::Line::New();
    auto line2 = data::Line::New();

    CPPUNIT_ASSERT(*line1 == *line2 && !(*line1 != *line2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(fieldName, ...) \
    line1->set ## fieldName(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Lines should be different when " #fieldName " is modified", \
        *line1 != *line2 && !(*line1 == *line2) \
    ); \
    line1->set ## fieldName(line2->get ## fieldName()); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Lines should be equal when " #fieldName " is copied", \
        *line1 == *line2 && !(*line1 != *line2) \
    );

    TEST(Position, data::Point::New(1., 0., 0.));
    TEST(Position, data::Point::New(0., 1., 0.));
    TEST(Position, data::Point::New(0., 0., 1.));
    TEST(Direction, data::Point::New(1., 0., 0.));
    TEST(Direction, data::Point::New(0., 1., 0.));
    TEST(Direction, data::Point::New(0., 0., 1.));

    #undef TEST
}

} // namespace sight::data::ut

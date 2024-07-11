/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "line_test.hpp"

#include <data/line.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::line_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void line_test::equality_test()
{
    auto line1 = std::make_shared<data::line>();
    auto line2 = std::make_shared<data::line>();

    CPPUNIT_ASSERT(*line1 == *line2 && !(*line1 != *line2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(fieldName, ...) \
            line1->set_ ## fieldName(__VA_ARGS__); \
            CPPUNIT_ASSERT_MESSAGE( \
                "Lines should be different when " #fieldName " is modified", \
                *line1 != *line2 && !(*line1 == *line2) \
            ); \
            line1->set_ ## fieldName(line2->get_ ## fieldName()); \
            CPPUNIT_ASSERT_MESSAGE( \
                "Lines should be equal when " #fieldName " is copied", \
                *line1 == *line2 && !(*line1 != *line2) \
            );

    TEST(position, std::make_shared<data::point>(1., 0., 0.));
    TEST(position, std::make_shared<data::point>(0., 1., 0.));
    TEST(position, std::make_shared<data::point>(0., 0., 1.));
    TEST(direction, std::make_shared<data::point>(1., 0., 0.));
    TEST(direction, std::make_shared<data::point>(0., 1., 0.));
    TEST(direction, std::make_shared<data::point>(0., 0., 1.));

    #undef TEST
}

} // namespace sight::data::ut

/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <data/line.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::line")
{
//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto line1 = std::make_shared<sight::data::line>();
        auto line2 = std::make_shared<sight::data::line>();

        CHECK(*line1 == *line2);
        CHECK(!(*line1 != *line2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(fieldName, ...) \
            line1->set_ ## fieldName(__VA_ARGS__); \
            CHECK_MESSAGE( \
                *line1 != *line2, \
                "Lines should be different when " #fieldName " is modified" \
            ); \
            CHECK_MESSAGE( \
                !(*line1 == *line2), \
                "Lines should be different when " #fieldName " is modified" \
            ); \
            line1->set_ ## fieldName(line2->get_ ## fieldName()); \
            CHECK_MESSAGE( \
                *line1 == *line2, \
                "Lines should be equal when " #fieldName " is copied" \
            ); \
            CHECK_MESSAGE( \
                !(*line1 != *line2), \
                "Lines should be equal when " #fieldName " is copied" \
            );

        TEST(position, std::make_shared<sight::data::point>(1., 0., 0.));
        TEST(position, std::make_shared<sight::data::point>(0., 1., 0.));
        TEST(position, std::make_shared<sight::data::point>(0., 0., 1.));
        TEST(direction, std::make_shared<sight::data::point>(1., 0., 0.));
        TEST(direction, std::make_shared<sight::data::point>(0., 1., 0.));
        TEST(direction, std::make_shared<sight::data::point>(0., 0., 1.));

    #undef TEST
    }
} // TEST_SUITE("sight::data::line")

/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <data/point.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::point")
{
//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        // shallow copy
        {
            sight::data::point::sptr p1 = std::make_shared<sight::data::point>(1.F, 2.F, 3.F);
            sight::data::point::sptr p2 = std::make_shared<sight::data::point>();

            CHECK_NOTHROW(p2->shallow_copy(p1));
            CHECK_EQ((*p1)[0], (*p2)[0]);
            CHECK_EQ((*p1)[1], (*p2)[1]);
            CHECK_EQ((*p1)[2], (*p2)[2]);
        }

        // Deep copy
        {
            sight::data::point::sptr p1 = std::make_shared<sight::data::point>(1.F, 2.F, 3.F);
            sight::data::point::sptr p2 = std::make_shared<sight::data::point>();

            CHECK_NOTHROW(p2->deep_copy(p1));
            CHECK_EQ((*p1)[0], (*p2)[0]);
            CHECK_EQ((*p1)[1], (*p2)[1]);
            CHECK_EQ((*p1)[2], (*p2)[2]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("getter")
    {
        sight::data::point::sptr p1 = std::make_shared<sight::data::point>();

        *p1 = {0., 1., 10.};

        CHECK_EQ(0., (*p1)[0]);
        CHECK_EQ(1., (*p1)[1]);
        CHECK_EQ(10., (*p1)[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("setter")
    {
        sight::data::point::sptr p1 = std::make_shared<sight::data::point>();

        sight::data::point::point_coord_array_t expected = {0.1, 0.2, 0.3};

        *p1 = {expected};

        CHECK_EQ(expected[0], (*p1)[0]);
        CHECK_EQ(expected[1], (*p1)[1]);
        CHECK_EQ(expected[2], (*p1)[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("label")
    {
        sight::data::point::sptr p = std::make_shared<sight::data::point>(1., 2., 3.);
        const std::string label    = "TestPoint";
        p->set_label(label);

        const auto actual_label = p->get_label();

        CHECK_EQ(label, actual_label);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto point1 = std::make_shared<sight::data::point>();
        auto point2 = std::make_shared<sight::data::point>();

        CHECK(*point1 == *point2);
        CHECK(!(*point1 != *point2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            point1->op; \
            CHECK_MESSAGE( \
                *point1 != *point2, \
                "Points should be different when using " #op " on the first one" \
            ); \
            CHECK_MESSAGE( \
                !(*point1 == *point2), \
                "Points should be different when using " #op " on the first one" \
            ); \
            point2->op; \
            CHECK_MESSAGE( \
                *point1 == *point2, \
                "Points should be equal when using " #op " on both" \
            ); \
            CHECK_MESSAGE( \
                !(*point1 != *point2), \
                "Points should be equal when using " #op " on both" \
            );

        TEST(operator=({1, 0, 0}));
        TEST(operator=({0, 1, 0}));
        TEST(operator=({0, 0, 1}));
        TEST(set_label("1"));

    #undef TEST
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::point")

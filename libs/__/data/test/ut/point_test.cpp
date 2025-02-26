/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "point_test.hpp"

#include <data/point.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::point_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void point_test::setUp()
{
}

//------------------------------------------------------------------------------

void point_test::tearDown()
{
}

//------------------------------------------------------------------------------

void point_test::copy_test()
{
    // shallow copy
    {
        data::point::sptr p1 = std::make_shared<data::point>(1.F, 2.F, 3.F);
        data::point::sptr p2 = std::make_shared<data::point>();

        CPPUNIT_ASSERT_NO_THROW(p2->shallow_copy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[0], p2->get_coord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[1], p2->get_coord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[2], p2->get_coord()[2]);
    }

    // Deep copy
    {
        data::point::sptr p1 = std::make_shared<data::point>(1.F, 2.F, 3.F);
        data::point::sptr p2 = std::make_shared<data::point>();

        CPPUNIT_ASSERT_NO_THROW(p2->deep_copy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[0], p2->get_coord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[1], p2->get_coord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->get_coord()[2], p2->get_coord()[2]);
    }
}

//------------------------------------------------------------------------------

void point_test::getter_test()
{
    data::point::sptr p1 = std::make_shared<data::point>();

    p1->set_coord({0., 1., 10.});

    const auto coords = p1->get_coord();

    CPPUNIT_ASSERT_EQUAL(0., coords[0]);
    CPPUNIT_ASSERT_EQUAL(1., coords[1]);
    CPPUNIT_ASSERT_EQUAL(10., coords[2]);
}

//------------------------------------------------------------------------------

void point_test::setter_test()
{
    data::point::sptr p1 = std::make_shared<data::point>();

    data::point::point_coord_array_t expected = {0.1, 0.2, 0.3};

    p1->set_coord(expected);
    const auto actual = p1->get_coord();

    CPPUNIT_ASSERT_EQUAL(expected[0], actual[0]);
    CPPUNIT_ASSERT_EQUAL(expected[1], actual[1]);
    CPPUNIT_ASSERT_EQUAL(expected[2], actual[2]);
}

//------------------------------------------------------------------------------

void point_test::label_test()
{
    data::point::sptr p     = std::make_shared<data::point>(1., 2., 3.);
    const std::string label = "TestPoint";
    p->set_label(label);

    const auto actual_label = p->get_label();

    CPPUNIT_ASSERT_EQUAL(label, actual_label);
}

//------------------------------------------------------------------------------

void point_test::equality_test()
{
    auto point1 = std::make_shared<data::point>();
    auto point2 = std::make_shared<data::point>();

    CPPUNIT_ASSERT(*point1 == *point2 && !(*point1 != *point2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            point1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Points should be different when using " #op " on the first one", \
                *point1 != *point2 && !(*point1 == *point2) \
            ); \
            point2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Points should be equal when using " #op " on both", \
                *point1 == *point2 && !(*point1 != *point2) \
            );

    TEST(set_coord({1, 0, 0}));
    TEST(set_coord({0, 1, 0}));
    TEST(set_coord({0, 0, 1}));
    TEST(set_label("1"));

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut

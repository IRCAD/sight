/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "PointTest.hpp"

#include <data/Point.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::PointTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void PointTest::setUp()
{
}

//------------------------------------------------------------------------------

void PointTest::tearDown()
{
}

//------------------------------------------------------------------------------

void PointTest::copyTest()
{
    // shallow copy
    {
        data::Point::sptr p1 = std::make_shared<data::Point>(1.F, 2.F, 3.F);
        data::Point::sptr p2 = std::make_shared<data::Point>();

        CPPUNIT_ASSERT_NO_THROW(p2->shallow_copy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
    }

    // Deep copy
    {
        data::Point::sptr p1 = std::make_shared<data::Point>(1.F, 2.F, 3.F);
        data::Point::sptr p2 = std::make_shared<data::Point>();

        CPPUNIT_ASSERT_NO_THROW(p2->deep_copy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
    }
}

//------------------------------------------------------------------------------

void PointTest::getterTest()
{
    data::Point::sptr p1 = std::make_shared<data::Point>();

    p1->setCoord({0., 1., 10.});

    const auto coords = p1->getCoord();

    CPPUNIT_ASSERT_EQUAL(0., coords[0]);
    CPPUNIT_ASSERT_EQUAL(1., coords[1]);
    CPPUNIT_ASSERT_EQUAL(10., coords[2]);
}

//------------------------------------------------------------------------------

void PointTest::setterTest()
{
    data::Point::sptr p1 = std::make_shared<data::Point>();

    data::Point::PointCoordArrayType expected = {0.1, 0.2, 0.3};

    p1->setCoord(expected);
    const auto actual = p1->getCoord();

    CPPUNIT_ASSERT_EQUAL(expected[0], actual[0]);
    CPPUNIT_ASSERT_EQUAL(expected[1], actual[1]);
    CPPUNIT_ASSERT_EQUAL(expected[2], actual[2]);
}

//------------------------------------------------------------------------------

void PointTest::labelTest()
{
    data::Point::sptr p     = std::make_shared<data::Point>(1., 2., 3.);
    const std::string label = "TestPoint";
    p->setLabel(label);

    const auto actual_label = p->getLabel();

    CPPUNIT_ASSERT_EQUAL(label, actual_label);
}

//------------------------------------------------------------------------------

void PointTest::equalityTest()
{
    auto point1 = std::make_shared<data::Point>();
    auto point2 = std::make_shared<data::Point>();

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

    TEST(setCoord({1, 0, 0}));
    TEST(setCoord({0, 1, 0}));
    TEST(setCoord({0, 0, 1}));
    TEST(setLabel("1"));

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut

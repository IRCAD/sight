/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

namespace sight::data
{

namespace ut
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
        data::Point::sptr p1 = data::Point::New(1.f, 2.f, 3.f);
        data::Point::sptr p2 = data::Point::New();

        CPPUNIT_ASSERT_NO_THROW(p2->shallowCopy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
    }

    // Deep copy
    {
        data::Point::sptr p1 = data::Point::New(1.f, 2.f, 3.f);
        data::Point::sptr p2 = data::Point::New();

        CPPUNIT_ASSERT_NO_THROW(p2->deepCopy(p1));
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[0], p2->getCoord()[0]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[1], p2->getCoord()[1]);
        CPPUNIT_ASSERT_EQUAL(p1->getCoord()[2], p2->getCoord()[2]);
    }
}

//------------------------------------------------------------------------------

void PointTest::getterTest()
{
    data::Point::sptr p1 = data::Point::New();

    p1->setCoord({0., 1., 10.});

    const auto coords = p1->getCoord();

    CPPUNIT_ASSERT_EQUAL(0., coords[0]);
    CPPUNIT_ASSERT_EQUAL(1., coords[1]);
    CPPUNIT_ASSERT_EQUAL(10., coords[2]);
}

//------------------------------------------------------------------------------

void PointTest::setterTest()
{
    data::Point::sptr p1 = data::Point::New();

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
    data::Point::sptr p     = data::Point::New(1., 2., 3.);
    const std::string label = "TestPoint";
    p->setLabel(label);

    const auto actual_label = p->getLabel();

    CPPUNIT_ASSERT_EQUAL(label, actual_label);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data

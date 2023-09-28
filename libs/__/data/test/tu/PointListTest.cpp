/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "PointListTest.hpp"

#include <data/Point.hpp>
#include <data/PointList.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::PointListTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void PointListTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PointListTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PointListTest::copyTest()
{
    data::PointList::sptr pl1 = std::make_shared<data::PointList>();
    data::PointList::sptr pl2 = std::make_shared<data::PointList>();
    data::PointList::sptr pl3 = std::make_shared<data::PointList>();

    data::Point::sptr point1 = std::make_shared<data::Point>(1.0F, 2.0F, 3.0F);

    pl1->pushBack(point1);

    CPPUNIT_ASSERT_NO_THROW(pl2->shallow_copy(pl1));

    CPPUNIT_ASSERT_EQUAL(pl1->getPoints()[0], pl2->getPoints()[0]);
}

//------------------------------------------------------------------------------

void PointListTest::getterTest()
{
    data::PointList::sptr pl1 = std::make_shared<data::PointList>();
    data::Point::sptr point1  = std::make_shared<data::Point>(1.0F, 2.0F, 3.0F);

    pl1->pushBack(point1);

    data::Point::sptr point2 = pl1->getPoints()[0];

    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[0], point2->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[1], point2->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[2], point2->getCoord()[2]);

    data::PointList::PointListContainer& container = pl1->getPoints();

    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[0], container[0]->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[1], container[0]->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[2], container[0]->getCoord()[2]);
}

//------------------------------------------------------------------------------

void PointListTest::setterTest()
{
    data::PointList::sptr pl1 = std::make_shared<data::PointList>();
    data::Point::sptr point1  = std::make_shared<data::Point>(1.0F, 2.0F, 3.0F);
    data::Point::sptr point2  = std::make_shared<data::Point>(4.0F, 5.0F, 6.0F);

    std::vector<data::Point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->setPoints(vec));

    data::PointList::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::pushTest()
{
    data::PointList::sptr pl1 = std::make_shared<data::PointList>();
    data::Point::sptr point1  = std::make_shared<data::Point>(1.0F, 2.0F, 3.0F);
    data::Point::sptr point2  = std::make_shared<data::Point>(4.0F, 5.0F, 6.0F);

    std::vector<data::Point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->pushBack(point1));
    CPPUNIT_ASSERT_NO_THROW(pl1->pushBack(point2));

    data::PointList::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::removeTest()
{
    const std::size_t nbPoints = 42;
    data::PointList::sptr pl   = std::make_shared<data::PointList>();

    // Remove first
    {
        // Build a list
        for(std::size_t i = 0 ; i < nbPoints ; ++i)
        {
            const auto p = std::make_shared<data::Point>(.0F, .0F, .0F);
            pl->pushBack(p);
        }

        // remove the first
        std::size_t size = nbPoints;
        while(!pl->getPoints().empty())
        {
            pl->remove(0);
            CPPUNIT_ASSERT_EQUAL(--size, pl->getPoints().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), pl->getPoints().size());

    // Remove last
    {
        // Build a list
        for(std::size_t i = 0 ; i < nbPoints ; ++i)
        {
            const auto p = std::make_shared<data::Point>(.0F, .0F, .0F);
            pl->pushBack(p);
        }

        // remove the last
        std::size_t size = nbPoints;
        while(!pl->getPoints().empty())
        {
            const std::size_t index = pl->getPoints().size() - 1;
            pl->remove(index);
            CPPUNIT_ASSERT_EQUAL(--size, pl->getPoints().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), pl->getPoints().size());

    // Check that the correct one is removed
    {
        // Build a list
        for(std::size_t i = 0 ; i < nbPoints ; ++i)
        {
            const auto p = std::make_shared<data::Point>(static_cast<float>(i), .0F, .0F);
            pl->pushBack(p);
        }

        std::size_t size = nbPoints;
        while(!pl->getPoints().empty())
        {
            const std::size_t index = size / 2;
            const auto ref          = pl->getPoints()[index];
            pl->remove(index);
            CPPUNIT_ASSERT_EQUAL(--size, pl->getPoints().size());
            for(const auto& p : pl->getPoints())
            {
                CPPUNIT_ASSERT(ref->getCoord()[0] != p->getCoord()[0]);
            }
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::clearTest()
{
    const std::size_t nbPoints = 42;
    data::PointList::sptr pl   = std::make_shared<data::PointList>();

    CPPUNIT_ASSERT(pl->getPoints().empty());
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().empty());

    // Build a list
    for(std::size_t i = 0 ; i < nbPoints ; i++)
    {
        const auto p = std::make_shared<data::Point>(.0F, .0F, .0F);
        pl->pushBack(p);
    }

    CPPUNIT_ASSERT(pl->getPoints().size() == nbPoints);
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().empty());
}

} // namespace sight::data::ut

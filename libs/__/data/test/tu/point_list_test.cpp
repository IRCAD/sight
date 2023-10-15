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

#include "point_list_test.hpp"

#include <data/point.hpp>
#include <data/point_list.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::point_list_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void point_list_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void point_list_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void point_list_test::copyTest()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point_list::sptr pl2 = std::make_shared<data::point_list>();
    data::point_list::sptr pl3 = std::make_shared<data::point_list>();

    data::point::sptr point1 = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);

    pl1->pushBack(point1);

    CPPUNIT_ASSERT_NO_THROW(pl2->shallow_copy(pl1));

    CPPUNIT_ASSERT_EQUAL(pl1->getPoints()[0], pl2->getPoints()[0]);
}

//------------------------------------------------------------------------------

void point_list_test::getterTest()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);

    pl1->pushBack(point1);

    data::point::sptr point2 = pl1->getPoints()[0];

    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[0], point2->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[1], point2->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[2], point2->getCoord()[2]);

    data::point_list::PointListContainer& container = pl1->getPoints();

    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[0], container[0]->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[1], container[0]->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->getCoord()[2], container[0]->getCoord()[2]);
}

//------------------------------------------------------------------------------

void point_list_test::setterTest()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);
    data::point::sptr point2   = std::make_shared<data::point>(4.0F, 5.0F, 6.0F);

    std::vector<data::point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->setPoints(vec));

    data::point_list::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::pushTest()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);
    data::point::sptr point2   = std::make_shared<data::point>(4.0F, 5.0F, 6.0F);

    std::vector<data::point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->pushBack(point1));
    CPPUNIT_ASSERT_NO_THROW(pl1->pushBack(point2));

    data::point_list::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::removeTest()
{
    const std::size_t nbPoints = 42;
    data::point_list::sptr pl  = std::make_shared<data::point_list>();

    // Remove first
    {
        // Build a list
        for(std::size_t i = 0 ; i < nbPoints ; ++i)
        {
            const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
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
            const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
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
            const auto p = std::make_shared<data::point>(static_cast<float>(i), .0F, .0F);
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

void point_list_test::clearTest()
{
    const std::size_t nbPoints = 42;
    data::point_list::sptr pl  = std::make_shared<data::point_list>();

    CPPUNIT_ASSERT(pl->getPoints().empty());
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().empty());

    // Build a list
    for(std::size_t i = 0 ; i < nbPoints ; i++)
    {
        const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
        pl->pushBack(p);
    }

    CPPUNIT_ASSERT(pl->getPoints().size() == nbPoints);
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().empty());
}

} // namespace sight::data::ut

/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::PointListTest );

namespace fwData
{
namespace ut
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
    ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
    ::fwData::PointList::sptr pl2 = ::fwData::PointList::New();
    ::fwData::PointList::sptr pl3 = ::fwData::PointList::New();

    ::fwData::Point::sptr point1 = ::fwData::Point::New(1.0f, 2.0f, 3.0f);

    pl1->pushBack(point1);

    CPPUNIT_ASSERT_NO_THROW( pl2->shallowCopy(pl1) );

    CPPUNIT_ASSERT_EQUAL(pl1->getPoints()[0], pl2->getPoints()[0]);

}

//------------------------------------------------------------------------------

void PointListTest::getterTest()
{
    ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
    ::fwData::Point::sptr point1  = ::fwData::Point::New(1.0f, 2.0f, 3.0f);

    pl1->pushBack(point1);

    ::fwData::Point::sptr point2 = pl1->getPoints()[0];

    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[0], point2->getCoord()[0], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[1], point2->getCoord()[1], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[2], point2->getCoord()[2], 10e-6);

    ::fwData::PointList::PointListContainer& container = pl1->getPoints();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[0], container[0]->getCoord()[0], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[1], container[0]->getCoord()[1], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[2], container[0]->getCoord()[2], 10e-6);

    const ::fwData::PointList::PointListContainer& ccontainer = pl1->getPoints();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[0], ccontainer[0]->getCoord()[0], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[1], ccontainer[0]->getCoord()[1], 10e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[2], ccontainer[0]->getCoord()[2], 10e-6);

}

//------------------------------------------------------------------------------

void PointListTest::setterTest()
{
    ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
    ::fwData::Point::sptr point1  = ::fwData::Point::New(1.0f, 2.0f, 3.0f);
    ::fwData::Point::sptr point2  = ::fwData::Point::New(4.0f, 5.0f, 6.0f);

    std::vector< ::fwData::Point::sptr > vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW( pl1->setPoints(vec) );

    ::fwData::PointList::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0; p < vec.size(); ++p)
    {
        for(unsigned int i = 0; i < 3; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i], 10e-6);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::pushTest()
{
    ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
    ::fwData::Point::sptr point1  = ::fwData::Point::New(1.0f, 2.0f, 3.0f);
    ::fwData::Point::sptr point2  = ::fwData::Point::New(4.0f, 5.0f, 6.0f);

    std::vector< ::fwData::Point::sptr > vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW( pl1->pushBack(point1) );
    CPPUNIT_ASSERT_NO_THROW( pl1->pushBack(point2) );

    ::fwData::PointList::PointListContainer& container = pl1->getPoints();

    for(unsigned p = 0; p < vec.size(); ++p)
    {
        for(unsigned int i = 0; i < 3; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i], 10e-6);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::removeTest()
{
    const size_t nbPoints = 42;
    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    // Remove first
    {
        // Build a list
        for(size_t i = 0; i < nbPoints; ++i)
        {
            const auto p = ::fwData::Point::New(.0f, .0f, .0f);
            pl->pushBack(p);
        }

        // remove the first
        size_t size = nbPoints;
        while(pl->getPoints().size())
        {
            pl->remove(0);
            CPPUNIT_ASSERT_EQUAL(--size, pl->getPoints().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), pl->getPoints().size());

    // Remove last
    {
        // Build a list
        for(size_t i = 0; i < nbPoints; ++i)
        {
            const auto p = ::fwData::Point::New(.0f, .0f, .0f);
            pl->pushBack(p);
        }

        // remove the last
        size_t size = nbPoints;
        while(pl->getPoints().size())
        {
            const size_t index = pl->getPoints().size()-1;
            pl->remove( index );
            CPPUNIT_ASSERT_EQUAL(--size, pl->getPoints().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), pl->getPoints().size());

    // Check that the correct one is removed
    {
        // Build a list
        for(size_t i = 0; i < nbPoints; ++i)
        {
            const auto p = ::fwData::Point::New(static_cast<float>(i), .0f, .0f);
            pl->pushBack(p);
        }

        size_t size = nbPoints;
        while(pl->getPoints().size())
        {
            const size_t index = size / 2;
            const auto ref     = pl->getPoints()[index];
            pl->remove( index );
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
    const size_t nbPoints = 42;
    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    CPPUNIT_ASSERT(pl->getPoints().size() == 0);
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().size() == 0);

    // Build a list
    for(size_t i = 0; i < nbPoints; i++)
    {
        const auto p = ::fwData::Point::New(.0f, .0f, .0f);
        pl->pushBack(p);
    }

    CPPUNIT_ASSERT(pl->getPoints().size() == nbPoints);
    pl->clear();
    CPPUNIT_ASSERT(pl->getPoints().size() == 0);
}

} //namespace ut
} //namespace fwData

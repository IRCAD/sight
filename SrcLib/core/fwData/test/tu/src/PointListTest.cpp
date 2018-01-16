/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    ::fwData::PointList::PointListContainer& container = pl1->getRefPoints();

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

    ::fwData::PointList::PointListContainer& container = pl1->getRefPoints();

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

    ::fwData::PointList::PointListContainer& container = pl1->getRefPoints();

    for(unsigned p = 0; p < vec.size(); ++p)
    {
        for(unsigned int i = 0; i < 3; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(vec[p]->getCoord()[i], container[p]->getCoord()[i], 10e-6);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData

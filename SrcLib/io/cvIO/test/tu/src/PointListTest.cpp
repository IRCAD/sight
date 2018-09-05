/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "PointListTest.hpp"

#include <cvIO/PointList.hpp>

#include <fwData/PointList.hpp>

#include <opencv2/core.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::PointListTest );

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

static void assert2dPointEquality(const ::fwData::Point::csptr& _pt, const ::cv::Point2d& _cvPt)
{
    const auto& coords = _pt->getCoord();
    CPPUNIT_ASSERT_EQUAL(coords[0], _cvPt.x);
    CPPUNIT_ASSERT_EQUAL(coords[1], _cvPt.y);
    CPPUNIT_ASSERT_EQUAL(coords[2], 0.);
}

//-----------------------------------------------------------------------------

void PointListTest::setUp()
{

}

//-----------------------------------------------------------------------------

void PointListTest::tearDown()
{

}

//-----------------------------------------------------------------------------

void PointListTest::copyFromCv()
{
    std::vector< ::cv::Point2d > cvPointList;

    for(std::uint16_t i = 0; i < 512; ++i)
    {
        cvPointList.push_back( ::cv::Point2d(double(i), double(i * 2 + 3)));
    }

    ::fwData::PointList::sptr pl = ::fwData::PointList::New();
    ::cvIO::PointList::copyFromCv(cvPointList, pl);

    for(std::uint16_t i = 0; i < 512; ++i)
    {
        ::fwData::Point::csptr pt = pl->getPoints()[i];
        assert2dPointEquality(pt, cvPointList[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(pt->getCoord()[0]));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 2  + 3), std::uint16_t(pt->getCoord()[1]));
        CPPUNIT_ASSERT_EQUAL(0., pt->getCoord()[2]);
    }
}

//-----------------------------------------------------------------------------

void PointListTest::copyToCv()
{
    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    for(std::uint16_t i = 0; i < 512; ++i)
    {
        ::fwData::Point::sptr point = ::fwData::Point::New(double(i), double(i * 3 + 5), 0.);
        pl->getPoints().push_back(point);
    }

    std::vector< ::cv::Point2d > cvPointList;
    ::cvIO::PointList::copyToCv(pl, cvPointList);

    for(std::uint16_t i = 0; i < 512; ++i)
    {
        assert2dPointEquality(pl->getPoints()[i], cvPointList[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(cvPointList[i].x));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 3 + 5), std::uint16_t(cvPointList[i].y));
    }
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace cvIO

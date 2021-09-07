/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include <data/PointList.hpp>

#include <io/opencv/PointList.hpp>

#include <opencv2/core.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::opencv::ut::PointListTest);

namespace sight::io::opencv
{

namespace ut
{

//------------------------------------------------------------------------------

static void assert2dPointEquality(const data::Point::csptr& _pt, const ::cv::Point2d& _cvPt)
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
    std::vector< ::cv::Point2d> cvPointList;

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        cvPointList.push_back(::cv::Point2d(double(i), double(i * 2 + 3)));
    }

    data::PointList::sptr pl = data::PointList::New();
    io::opencv::PointList::copyFromCv(cvPointList, pl);

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        data::Point::csptr pt = pl->getPoints()[i];
        assert2dPointEquality(pt, cvPointList[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(pt->getCoord()[0]));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 2 + 3), std::uint16_t(pt->getCoord()[1]));
        CPPUNIT_ASSERT_EQUAL(0., pt->getCoord()[2]);
    }
}

//-----------------------------------------------------------------------------

void PointListTest::copyToCv()
{
    data::PointList::sptr pl = data::PointList::New();

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        data::Point::sptr point = data::Point::New(double(i), double(i * 3 + 5), 0.);
        pl->getPoints().push_back(point);
    }

    std::vector< ::cv::Point2d> cvPointList;
    io::opencv::PointList::copyToCv(pl, cvPointList);

    for(std::uint16_t i = 0 ; i < 512 ; ++i)
    {
        assert2dPointEquality(pl->getPoints()[i], cvPointList[i]);
        CPPUNIT_ASSERT_EQUAL(i, std::uint16_t(cvPointList[i].x));
        CPPUNIT_ASSERT_EQUAL(std::uint16_t(i * 3 + 5), std::uint16_t(cvPointList[i].y));
    }
}

//-----------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::opencv

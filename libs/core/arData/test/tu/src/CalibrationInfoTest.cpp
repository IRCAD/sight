/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "CalibrationInfoTest.hpp"

#include <arData/CalibrationInfo.hpp>

#include <core/spyLog.hpp>
#include <core/tools/Type.hpp>

#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::CalibrationInfoTest );

namespace arData
{
namespace ut
{

//------------------------------------------------------------------------------

void CalibrationInfoTest::setUp()
{
    // Set up context before running a test.
    ::fwTest::generator::Image::initRand();
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::tearDown()
{
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::calibrationInfoTest()
{
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::New();

    data::Image::sptr img = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage( img, core::tools::Type::s_INT16);

    data::PointList::sptr pl = data::PointList::New();

    data::Point::sptr pt1 = data::Point::New( 1.0, 2.0, 3.0 );
    data::Point::sptr pt2 = data::Point::New( 4.0, 5.0, 6.0 );
    data::Point::sptr pt3 = data::Point::New( 7.0, 8.0, 9.0 );

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    //Testing values

    ::arData::CalibrationInfo::ImageContainerType imgList = calInfo->getImageContainer();

    CPPUNIT_ASSERT_EQUAL(size_t(1), imgList.size());
    CPPUNIT_ASSERT_EQUAL(img, imgList.front());

    ::arData::CalibrationInfo::PointListContainerType plList = calInfo->getPointListContainer();

    CPPUNIT_ASSERT_EQUAL(size_t(1), plList.size());
    CPPUNIT_ASSERT_EQUAL(pl, plList.front());

    data::PointList::sptr pl1 = calInfo->getPointList(imgList.front());
    CPPUNIT_ASSERT_EQUAL(pl, pl1);

    data::Image::sptr img1 = calInfo->getImage(plList.front());
    CPPUNIT_ASSERT_EQUAL(img, img1);

    calInfo->removeRecord(0);

    data::PointList::sptr pl2 = calInfo->getPointList(img);
    CPPUNIT_ASSERT_EQUAL(data::PointList::sptr(), pl2);

    data::Image::sptr img2 = calInfo->getImage(pl);
    CPPUNIT_ASSERT_EQUAL(data::Image::sptr(), img2);

    CPPUNIT_ASSERT(calInfo->getImageContainer().empty());
    CPPUNIT_ASSERT(calInfo->getPointListContainer().empty());
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::shallowCopyTest()
{
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::New();

    data::Image::sptr img = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage( img, core::tools::Type::s_INT16);

    data::PointList::sptr pl = data::PointList::New();

    data::Point::sptr pt1 = data::Point::New( 1.0, 2.0, 3.0 );
    data::Point::sptr pt2 = data::Point::New( 4.0, 5.0, 6.0 );
    data::Point::sptr pt3 = data::Point::New( 7.0, 8.0, 9.0 );

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::New();
    calInfo2->shallowCopy(calInfo);

    CPPUNIT_ASSERT(calInfo->getImageContainer() == calInfo2->getImageContainer());
    CPPUNIT_ASSERT(calInfo->getPointListContainer() == calInfo2->getPointListContainer());
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::deepCopyTest()
{
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::New();

    data::Image::sptr img = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage( img, core::tools::Type::s_INT16);

    data::PointList::sptr pl = data::PointList::New();

    data::Point::sptr pt1 = data::Point::New( 1.0, 2.0, 3.0 );
    data::Point::sptr pt2 = data::Point::New( 4.0, 5.0, 6.0 );
    data::Point::sptr pt3 = data::Point::New( 7.0, 8.0, 9.0 );

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::New();
    calInfo2->deepCopy(calInfo);

    CPPUNIT_ASSERT_EQUAL(calInfo2->getImageContainer().size(), calInfo2->getPointListContainer().size());

    CPPUNIT_ASSERT_EQUAL(calInfo->getImageContainer().size(), calInfo2->getImageContainer().size());
    CPPUNIT_ASSERT_EQUAL(calInfo->getPointListContainer().size(), calInfo2->getPointListContainer().size());

    ::arData::CalibrationInfo::ImageContainerType::const_iterator iterImg1, iterImg2;
    iterImg1 = calInfo->getImageContainer().begin();
    iterImg2 = calInfo2->getImageContainer().begin();

    ::arData::CalibrationInfo::PointListContainerType::const_iterator iterPl1, iterPl2;
    iterPl1 = calInfo->getPointListContainer().begin();
    iterPl2 = calInfo2->getPointListContainer().begin();

    while(iterImg1 != calInfo->getImageContainer().end())
    {
        CPPUNIT_ASSERT(*iterImg1 != *iterImg2);
        CPPUNIT_ASSERT(*iterPl1 != *iterPl2);

        CPPUNIT_ASSERT(::fwTest::helper::compare(*iterImg1, *iterImg2));
        CPPUNIT_ASSERT(::fwTest::helper::compare(*iterPl1, *iterPl2));

        ++iterPl1;
        ++iterPl2;

        ++iterImg1;
        ++iterImg2;
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace arData

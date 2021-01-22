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

#include "detail/reflection/CalibrationInfoTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <arData/CalibrationInfo.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwTest/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::detail::reflection::ut::CalibrationInfoTest );

namespace arData
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void CalibrationInfoTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::tearDown()
{
    //Hack: force link with arData
    ::arData::CalibrationInfo::sptr obj = ::arData::CalibrationInfo::New();
    obj->getClassname();
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::propertiesTest()
{
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::New();

    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage( img, core::tools::Type::s_INT16);

    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    ::fwData::Point::sptr pt1 = ::fwData::Point::New( 1.0, 2.0, 3.0 );
    ::fwData::Point::sptr pt2 = ::fwData::Point::New( 4.0, 5.0, 6.0 );
    ::fwData::Point::sptr pt3 = ::fwData::Point::New( 7.0, 8.0, 9.0 );

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    const ::arData::ut::DataCampHelper::PropertiesNameType dataProperties = { "fields",
                                                                              "image_container",
                                                                              "pointlist_container"};

    ::arData::ut::DataCampHelper::visitProperties(calInfo->getClassname(), dataProperties);
    ::arData::ut::DataCampHelper::compareObjectPropertyValue(calInfo, "@image_container.0", img);
    ::arData::ut::DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0", pl);
    ::arData::ut::DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.0", pt1);
    ::arData::ut::DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.1", pt2);
    ::arData::ut::DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.2", pt3);

}

//------------------------------------------------------------------------------
} // namespace ut
} // namespace reflection
} // namespace detail
} // namespace ardata

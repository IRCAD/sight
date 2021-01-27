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

#include "DataCampHelper.hpp"

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::data::detail::reflection::ut::CalibrationInfoTest );

namespace sight::data
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
    data::CalibrationInfo::sptr obj = data::CalibrationInfo::New();
    obj->getClassname();
}

//------------------------------------------------------------------------------

void CalibrationInfoTest::propertiesTest()
{
    data::CalibrationInfo::sptr calInfo = data::CalibrationInfo::New();

    data::Image::sptr img = data::Image::New();
    utestData::generator::Image::generateRandomImage( img, core::tools::Type::s_INT16);

    data::PointList::sptr pl = data::PointList::New();

    data::Point::sptr pt1 = data::Point::New( 1.0, 2.0, 3.0 );
    data::Point::sptr pt2 = data::Point::New( 4.0, 5.0, 6.0 );
    data::Point::sptr pt3 = data::Point::New( 7.0, 8.0, 9.0 );

    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);
    pl->getPoints().push_back(pt3);

    calInfo->addRecord(img, pl);

    const DataCampHelper::PropertiesNameType dataProperties = { "fields",
                                                                "image_container",
                                                                "pointlist_container"};

    DataCampHelper::visitProperties(calInfo->getClassname(), dataProperties);
    DataCampHelper::compareObjectPropertyValue(calInfo, "@image_container.0", img);
    DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0", pl);
    DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.0", pt1);
    DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.1", pt2);
    DataCampHelper::compareObjectPropertyValue(calInfo, "@pointlist_container.0.points.2", pt3);

}

//------------------------------------------------------------------------------
} // namespace ut
} // namespace reflection
} // namespace detail
} // namespace ardata

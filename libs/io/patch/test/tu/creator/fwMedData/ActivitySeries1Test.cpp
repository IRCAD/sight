/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "ActivitySeries1Test.hpp"

#include "atoms/String.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/helper/Object.hpp"
#include "io/patch/structural/creator/data/ActivitySeries1.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::patch::structural::ut::creator::data::ActivitySeries1Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace creator
{

namespace data
{

//------------------------------------------------------------------------------

void ActivitySeries1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::createTest()
{
    using namespace io::patch::structural::creator::data;
    ActivitySeries1::sptr actSeriesCreator = ActivitySeries1::New();

    CPPUNIT_ASSERT(actSeriesCreator);
    const std::string& classname = actSeriesCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("sight::data::ActivitySeries") == classname);
    const std::string& objectVersion = actSeriesCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion);

    sight::atoms::Object::sptr actSeries = actSeriesCreator->create();

    const std::string& actSeriesClassname = io::atoms::patch::helper::getClassname(actSeries);
    CPPUNIT_ASSERT_MESSAGE(
        "Classname must be equal",
        std::string("sight::data::ActivitySeries") == actSeriesClassname
    );
    const std::string& actSeriesObjectVersion = io::atoms::patch::helper::getVersion(actSeries);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == actSeriesObjectVersion);

    CPPUNIT_ASSERT(actSeries->getAttribute("data"));
    CPPUNIT_ASSERT(actSeries->getAttribute("activity_config_id"));
}

//------------------------------------------------------------------------------

void ActivitySeries1Test::copyTest()
{
    using namespace io::patch::structural::creator::data;
    ActivitySeries1 actSeriesCreator;

    const std::string& classname = actSeriesCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal", std::string("sight::data::ActivitySeries") == classname);
    const std::string& objectVersion = actSeriesCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersion);

    ActivitySeries1* actSeriesCreator2 = new ActivitySeries1(actSeriesCreator);

    CPPUNIT_ASSERT(actSeriesCreator2);
    const std::string& classnameActivitySeriesCreator2 = actSeriesCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE(
        "Classname must be equal",
        std::string("sight::data::ActivitySeries") == classnameActivitySeriesCreator2
    );

    const std::string& objectVersionActivitySeriesCreator2 = actSeriesCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal", std::string("1") == objectVersionActivitySeriesCreator2);

    delete (actSeriesCreator2);
}

//------------------------------------------------------------------------------

} //namespace data

} //namespace creator

} //namespace ut

} //namespace sight::io::patch::structural

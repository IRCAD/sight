/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ActivityTest.hpp"

#include <data/Integer.hpp>
#include <data/String.hpp>

#include <ranges>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ActivityTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ActivityTest::setUp()
{
    // Set up context before running a test.
    m_activity = std::make_shared<data::Activity>();
}

//------------------------------------------------------------------------------

void ActivityTest::tearDown()
{
    // Clean up after the test run.
    m_activity.reset();
}

//------------------------------------------------------------------------------

void ActivityTest::activityConfigIdTest()
{
    const data::Activity::ConfigIdType activityConfigId = "Visu2D";
    CPPUNIT_ASSERT(m_activity);
    m_activity->setActivityConfigId(activityConfigId);
    CPPUNIT_ASSERT_EQUAL(activityConfigId, m_activity->getActivityConfigId());

    auto activity2 = std::make_shared<data::Activity>();
    CPPUNIT_ASSERT(*activity2 != *m_activity);

    std::ranges::copy(*m_activity, std::inserter(*activity2, activity2->begin()));
    activity2->setActivityConfigId(m_activity->getActivityConfigId());
    CPPUNIT_ASSERT(*activity2 == *m_activity);
}

//------------------------------------------------------------------------------

void ActivityTest::dataTest()
{
    auto activity2 = std::make_shared<data::Activity>();
    CPPUNIT_ASSERT(m_activity);
    CPPUNIT_ASSERT(activity2);

    // Both data are "empty"
    CPPUNIT_ASSERT(*activity2 == *m_activity);

    std::ranges::copy(*m_activity, std::inserter(*activity2, activity2->begin()));
    activity2->setActivityConfigId(m_activity->getActivityConfigId());
    CPPUNIT_ASSERT(*activity2 == *m_activity);
}

//------------------------------------------------------------------------------

void ActivityTest::equalityTest()
{
    auto activity1 = std::make_shared<data::Activity>();
    auto activity2 = std::make_shared<data::Activity>();

    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));

    activity1->setActivityConfigId("1");
    CPPUNIT_ASSERT(*activity1 != *activity2 && !(*activity1 == *activity2));
    activity2->setActivityConfigId(activity1->getActivityConfigId());
    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));

    (*activity1)["data"] = std::make_shared<data::Integer>(2);
    CPPUNIT_ASSERT(*activity1 != *activity2 && !(*activity1 == *activity2));
    std::ranges::copy(*activity1, std::inserter(*activity2, activity2->begin()));
    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));
}

//------------------------------------------------------------------------------

void ActivityTest::shallow_copyTest()
{
    CPPUNIT_ASSERT(m_activity);
    m_activity->setActivityConfigId("MyActivity");
    (*m_activity)["data"] = std::make_shared<data::String>("Hello world");

    auto activity2 = std::make_shared<data::Activity>();
    CPPUNIT_ASSERT(*m_activity != *activity2);
    activity2->shallow_copy(m_activity);
    CPPUNIT_ASSERT(*m_activity == *activity2);
}

} //namespace sight::data::ut

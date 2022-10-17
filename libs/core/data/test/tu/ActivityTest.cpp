/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Composite.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ActivityTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ActivityTest::setUp()
{
    // Set up context before running a test.
    m_activity = data::Activity::New();
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

    auto series2 = data::Activity::New();
    CPPUNIT_ASSERT(*series2 != *m_activity);

    series2->setData(m_activity->getData());
    series2->setActivityConfigId(m_activity->getActivityConfigId());
    CPPUNIT_ASSERT(*series2 == *m_activity);
}

//------------------------------------------------------------------------------

void ActivityTest::dataTest()
{
    auto data = data::Composite::New();
    CPPUNIT_ASSERT(m_activity);
    CPPUNIT_ASSERT(data);
    m_activity->setData(data);
    CPPUNIT_ASSERT_EQUAL(data, m_activity->getData());

    auto series2 = data::Activity::New();
    // Both data are "empty"
    CPPUNIT_ASSERT(*series2 == *m_activity);

    series2->setData(m_activity->getData());
    series2->setActivityConfigId(m_activity->getActivityConfigId());
    CPPUNIT_ASSERT(*series2 == *m_activity);
}

} //namespace sight::data::ut

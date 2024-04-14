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

#include "activity_test.hpp"

#include <data/integer.hpp>
#include <data/string.hpp>

#include <ranges>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::activity_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void activity_test::setUp()
{
    // Set up context before running a test.
    m_activity = std::make_shared<data::activity>();
}

//------------------------------------------------------------------------------

void activity_test::tearDown()
{
    // Clean up after the test run.
    m_activity.reset();
}

//------------------------------------------------------------------------------

void activity_test::activity_config_id_test()
{
    const data::activity::config_id_t activity_config_id = "Visu2D";
    CPPUNIT_ASSERT(m_activity);
    m_activity->set_activity_config_id(activity_config_id);
    CPPUNIT_ASSERT_EQUAL(activity_config_id, m_activity->get_activity_config_id());

    auto activity2 = std::make_shared<data::activity>();
    CPPUNIT_ASSERT(*activity2 != *m_activity);

    std::ranges::copy(*m_activity, std::inserter(*activity2, activity2->begin()));
    activity2->set_activity_config_id(m_activity->get_activity_config_id());
    CPPUNIT_ASSERT(*activity2 == *m_activity);
}

//------------------------------------------------------------------------------

void activity_test::data_test()
{
    auto activity2 = std::make_shared<data::activity>();
    CPPUNIT_ASSERT(m_activity);
    CPPUNIT_ASSERT(activity2);

    // Both data are "empty"
    CPPUNIT_ASSERT(*activity2 == *m_activity);

    std::ranges::copy(*m_activity, std::inserter(*activity2, activity2->begin()));
    activity2->set_activity_config_id(m_activity->get_activity_config_id());
    CPPUNIT_ASSERT(*activity2 == *m_activity);
}

//------------------------------------------------------------------------------

void activity_test::equality_test()
{
    auto activity1 = std::make_shared<data::activity>();
    auto activity2 = std::make_shared<data::activity>();

    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));

    activity1->set_activity_config_id("1");
    CPPUNIT_ASSERT(*activity1 != *activity2 && !(*activity1 == *activity2));
    activity2->set_activity_config_id(activity1->get_activity_config_id());
    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));

    (*activity1)["data"] = std::make_shared<data::integer>(2);
    CPPUNIT_ASSERT(*activity1 != *activity2 && !(*activity1 == *activity2));
    std::ranges::copy(*activity1, std::inserter(*activity2, activity2->begin()));
    CPPUNIT_ASSERT(*activity1 == *activity2 && !(*activity1 != *activity2));
}

//------------------------------------------------------------------------------

void activity_test::shallow_copy_test()
{
    CPPUNIT_ASSERT(m_activity);
    m_activity->set_activity_config_id("MyActivity");
    (*m_activity)["data"] = std::make_shared<data::string>("Hello world");

    auto activity2 = std::make_shared<data::activity>();
    CPPUNIT_ASSERT(*m_activity != *activity2);
    activity2->shallow_copy(m_activity);
    CPPUNIT_ASSERT(*m_activity == *activity2);
}

} //namespace sight::data::ut

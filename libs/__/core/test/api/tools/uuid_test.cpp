/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "uuid_test.hpp"

#include <core/object.hpp>
#include <core/tools/uuid.hpp>

#include <functional>
#include <future>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::uuidtest);

namespace sight::core::tools::ut
{

//-----------------------------------------------------------------------------

void uuidtest::setUp()
{
    // Set up context before running a test.
    m_object = std::make_shared<core::object>();
    m_uuid   = core::tools::uuid::generate();
}

//-----------------------------------------------------------------------------

void uuidtest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void uuidtest::object_uuidtest()
{
    const std::string uuid = "myUUID";

    core::object::sptr obj = std::make_shared<core::object>();

    CPPUNIT_ASSERT(!core::object::from_uuid(uuid));

    obj->set_uuid(uuid);

    CPPUNIT_ASSERT(core::object::from_uuid(uuid));
    CPPUNIT_ASSERT_EQUAL(uuid, obj->get_uuid());
    CPPUNIT_ASSERT_EQUAL(obj, core::object::from_uuid(uuid));

    auto obj2          = std::make_shared<core::object>();
    std::string uui_d2 = obj2->get_uuid();
    CPPUNIT_ASSERT_EQUAL(obj2, core::object::from_uuid(uui_d2));
}

//-----------------------------------------------------------------------------

void uuidtest::conccurent_access_on_uuid_map_test()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 10 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, run_uuidcreation));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void uuidtest::run_uuidcreation()
{
    std::string uuid = core::tools::uuid::generate();

    core::object::sptr obj = std::make_shared<core::object>();

    CPPUNIT_ASSERT(!core::object::from_uuid(uuid));

    obj->set_uuid(uuid);

    CPPUNIT_ASSERT_EQUAL(uuid, obj->get_uuid());
    CPPUNIT_ASSERT_EQUAL(obj, core::object::from_uuid(uuid));

    auto obj2          = std::make_shared<core::object>();
    std::string uui_d2 = obj2->get_uuid();
    CPPUNIT_ASSERT_EQUAL(obj2, core::object::from_uuid(uui_d2));
}

//-----------------------------------------------------------------------------

void uuidtest::conccurent_access_on_same_obj_uuidtest()
{
    const auto fn = [this](auto&& ...){run_access_to_object_uuid();};
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 10 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, fn));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void uuidtest::run_access_to_object_uuid()
{
    m_object->set_uuid(m_uuid);

    CPPUNIT_ASSERT(core::object::from_uuid(m_uuid));
    CPPUNIT_ASSERT_EQUAL(m_uuid, m_object->get_uuid());
    CPPUNIT_ASSERT_EQUAL(m_object, core::object::from_uuid(m_uuid));
}

//-----------------------------------------------------------------------------

} // namespace sight::core::tools::ut

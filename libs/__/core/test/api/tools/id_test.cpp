/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "id_test.hpp"

#include <core/tools/failed.hpp>
#include <core/tools/id.hpp>
#include <core/tools/object.hpp>
#include <core/tools/uuid.hpp>

#include <chrono>
#include <future>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::id_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------P-----------

void id_test::setUp()
{
}

//-----------------------------------------------------------------------------

void id_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void id_test::object_id_test()
{
    const std::string id = "myID";

    core::tools::object::sptr obj = std::make_shared<core::tools::object>();

    CPPUNIT_ASSERT(core::tools::id::exist(id) == false);

    obj->set_id(id);

    CPPUNIT_ASSERT(core::tools::id::exist(id));
    CPPUNIT_ASSERT_EQUAL(id, obj->get_id());
    CPPUNIT_ASSERT_EQUAL(obj, core::tools::id::get_object(id));

    core::tools::object::sptr obj2 = std::make_shared<core::tools::object>();

    CPPUNIT_ASSERT(obj2->has_id() == false);
    CPPUNIT_ASSERT_THROW(obj2->get_id(core::tools::id::policy::must_exist), core::tools::failed);

    std::string fwid = obj2->get_id(core::tools::id::policy::generate);
    CPPUNIT_ASSERT_NO_THROW(obj2->get_id(core::tools::id::policy::must_exist));

    CPPUNIT_ASSERT(obj2->has_id() == true);
    CPPUNIT_ASSERT(core::tools::id::exist(fwid));

    obj2->reset_id();
    CPPUNIT_ASSERT(obj2->has_id() == false);

    CPPUNIT_ASSERT(core::tools::id::exist(fwid) == false);
    CPPUNIT_ASSERT(!core::tools::id::get_object(fwid));
}

//-----------------------------------------------------------------------------

void id_test::concurrent_access_on_id_map_test()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 10 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, run_id_creation));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void id_test::run_id_creation()
{
    const std::string id = core::tools::uuid::generate();

    core::tools::object::sptr obj = std::make_shared<core::tools::object>();

    CPPUNIT_ASSERT(core::tools::id::exist(id) == false);

    obj->set_id(id);

    CPPUNIT_ASSERT(core::tools::id::exist(id));

    CPPUNIT_ASSERT_EQUAL(id, obj->get_id());

    CPPUNIT_ASSERT_EQUAL(obj, core::tools::id::get_object(id));

    core::tools::object::sptr obj2 = std::make_shared<core::tools::object>();

    CPPUNIT_ASSERT(obj2->has_id() == false);
    CPPUNIT_ASSERT_THROW(obj2->get_id(core::tools::id::policy::must_exist), core::tools::failed);

    const std::string id2 = obj2->get_id(core::tools::id::policy::generate);
    CPPUNIT_ASSERT_NO_THROW(obj2->get_id(core::tools::id::policy::must_exist));

    CPPUNIT_ASSERT(obj2->has_id() == true);
    CPPUNIT_ASSERT(core::tools::id::exist(id2));

    obj2->reset_id();
    CPPUNIT_ASSERT(obj2->has_id() == false);

    CPPUNIT_ASSERT(core::tools::id::exist(id2) == false);
    CPPUNIT_ASSERT(!core::tools::id::get_object(id2));
}

//-----------------------------------------------------------------------------

} // namespace sight::core::tools::ut

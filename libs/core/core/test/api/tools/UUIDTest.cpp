/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "UUIDTest.hpp"

#include <core/tools/Object.hpp>
#include <core/tools/UUID.hpp>

#include <functional>
#include <future>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::UUIDTest);

namespace sight::core::tools::ut
{

//-----------------------------------------------------------------------------

void UUIDTest::setUp()
{
    // Set up context before running a test.
    m_object = std::make_shared<core::tools::Object>();
    m_uuid   = core::tools::UUID::generateUUID();
}

//-----------------------------------------------------------------------------

void UUIDTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void UUIDTest::objectUUIDTest()
{
    const std::string UUID = "myUUID";

    core::tools::Object::sptr obj = std::make_shared<core::tools::Object>();

    CPPUNIT_ASSERT(!core::tools::Object::fromUUID(UUID));

    obj->setUUID(UUID);

    CPPUNIT_ASSERT(core::tools::Object::fromUUID(UUID));
    CPPUNIT_ASSERT_EQUAL(UUID, obj->getUUID());
    CPPUNIT_ASSERT_EQUAL(obj, core::tools::Object::fromUUID(UUID));

    auto obj2         = std::make_shared<core::tools::Object>();
    std::string UUID2 = obj2->getUUID();
    CPPUNIT_ASSERT_EQUAL(obj2, core::tools::Object::fromUUID(UUID2));
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnUUIDMapTest()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 10 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, runUUIDCreation));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void UUIDTest::runUUIDCreation()
{
    std::string UUID = core::tools::UUID::generateUUID();

    core::tools::Object::sptr obj = std::make_shared<core::tools::Object>();

    CPPUNIT_ASSERT(!core::tools::Object::fromUUID(UUID));

    obj->setUUID(UUID);

    CPPUNIT_ASSERT_EQUAL(UUID, obj->getUUID());
    CPPUNIT_ASSERT_EQUAL(obj, core::tools::Object::fromUUID(UUID));

    auto obj2         = std::make_shared<core::tools::Object>();
    std::string UUID2 = obj2->getUUID();
    CPPUNIT_ASSERT_EQUAL(obj2, core::tools::Object::fromUUID(UUID2));
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnSameObjUUIDTest()
{
    const auto fn = [this](auto&& ...){runAccessToObjectUUID();};
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

void UUIDTest::runAccessToObjectUUID()
{
    m_object->setUUID(m_uuid);

    CPPUNIT_ASSERT(core::tools::Object::fromUUID(m_uuid));
    CPPUNIT_ASSERT_EQUAL(m_uuid, m_object->getUUID());
    CPPUNIT_ASSERT_EQUAL(m_object, core::tools::Object::fromUUID(m_uuid));
}

//-----------------------------------------------------------------------------

} // namespace sight::core::tools::ut

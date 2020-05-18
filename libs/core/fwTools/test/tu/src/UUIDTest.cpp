/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwTools/Object.hpp>
#include <fwTools/UUID.hpp>

#include <functional>
#include <future>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::UUIDTest );

namespace fwTools
{
namespace ut
{

//-----------------------------------------------------------------------------

void UUIDTest::setUp()
{
    // Set up context before running a test.
    m_object = std::make_shared< ::fwTools::Object >();
    m_uuid   = ::fwTools::UUID::generateUUID();
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

    ::fwTools::Object::sptr obj = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    ::fwTools::UUID::set(obj, UUID);

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL(  UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = std::make_shared< ::fwTools::Object >();
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnUUIDMapTest()
{
    const auto fn = std::bind(&UUIDTest::runUUIDCreation, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 10; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void UUIDTest::runUUIDCreation()
{
    std::string UUID = ::fwTools::UUID::generateUUID();

    ::fwTools::Object::sptr obj = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    CPPUNIT_ASSERT( ::fwTools::UUID::set(obj, UUID));

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL( UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = std::make_shared< ::fwTools::Object >();
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnSameObjUUIDTest()
{
    const auto fn = std::bind(&UUIDTest::runAccessToObjectUUID, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 10; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void UUIDTest::runAccessToObjectUUID()
{
    ::fwTools::UUID::set(m_object, m_uuid);

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(m_uuid) );
    CPPUNIT_ASSERT_EQUAL( m_uuid, ::fwTools::UUID::get(m_object) );
    CPPUNIT_ASSERT_EQUAL( m_object, ::fwTools::UUID::get(m_uuid) );
    CPPUNIT_ASSERT(::fwTools::UUID::set(m_object, m_uuid) == false);
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

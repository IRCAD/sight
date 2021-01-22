/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "FwIDTest.hpp"

#include <core/tools/Failed.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Object.hpp>
#include <core/tools/UUID.hpp>

#include <chrono>
#include <future>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( core::tools::ut::FwIDTest );

namespace fwTools
{
namespace ut
{

//-----------------------------------------------------------------------------

void FwIDTest::setUp()
{
}

//-----------------------------------------------------------------------------

void FwIDTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void FwIDTest::objectFwIDTest()
{
    const std::string fwID = "myID";

    core::tools::Object::sptr obj = std::make_shared< core::tools::Object >();

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID) );
    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );
    CPPUNIT_ASSERT_EQUAL( obj, core::tools::fwID::getObject(fwID) );

    core::tools::Object::sptr obj2 = std::make_shared< core::tools::Object >();

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW(obj2->getID(core::tools::fwID::MUST_EXIST), core::tools::Failed);

    std::string fwid = obj2->getID(core::tools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW(obj2->getID(core::tools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( core::tools::fwID::exist(fwid) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwid) == false );
    CPPUNIT_ASSERT( !core::tools::fwID::getObject(fwid) );
}

//-----------------------------------------------------------------------------

void FwIDTest::concurrentAccessOnFwIDMapTest()
{
    const auto fn = std::bind(&FwIDTest::runFwIDCreation, this);
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

void FwIDTest::runFwIDCreation()
{
    const std::string fwID = core::tools::UUID::generateUUID();

    core::tools::Object::sptr obj = std::make_shared< core::tools::Object >();

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID) );

    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );

    CPPUNIT_ASSERT_EQUAL( obj, core::tools::fwID::getObject(fwID) );

    core::tools::Object::sptr obj2 = std::make_shared< core::tools::Object >();

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW(obj2->getID(core::tools::fwID::MUST_EXIST), core::tools::Failed);

    const std::string fwID2 = obj2->getID(core::tools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW(obj2->getID(core::tools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID2) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( core::tools::fwID::exist(fwID2) == false );
    CPPUNIT_ASSERT( !core::tools::fwID::getObject(fwID2) );
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

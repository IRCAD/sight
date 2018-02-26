/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FwIDTest.hpp"

#include <fwTools/Failed.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>
#include <fwTools/UUID.hpp>

#include <chrono>
#include <future>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::FwIDTest );

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

    ::fwTools::Object::sptr obj = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) );
    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::fwID::getObject(fwID) );

    ::fwTools::Object::sptr obj2 = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW(obj2->getID(::fwTools::fwID::MUST_EXIST), ::fwTools::Failed);

    std::string fwid = obj2->getID(::fwTools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW(obj2->getID(::fwTools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) == false );
    CPPUNIT_ASSERT( !::fwTools::fwID::getObject(fwid) );
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
    const std::string fwID = ::fwTools::UUID::generateUUID();

    ::fwTools::Object::sptr obj = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) );

    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );

    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::fwID::getObject(fwID) );

    ::fwTools::Object::sptr obj2 = std::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW(obj2->getID(::fwTools::fwID::MUST_EXIST), ::fwTools::Failed);

    const std::string fwID2 = obj2->getID(::fwTools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW(obj2->getID(::fwTools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID2) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID2) == false );
    CPPUNIT_ASSERT( !::fwTools::fwID::getObject(fwID2) );
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

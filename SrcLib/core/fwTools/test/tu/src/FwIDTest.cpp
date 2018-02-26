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
    // Set up context before running a test.
    m_object = std::make_shared< ::fwTools::Object >();
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

void FwIDTest::conccurentAccessOnFwIDMapTest()
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

void FwIDTest::conccurentAccessOnSameObjFwIDTest()
{
    auto fn = std::bind(&FwIDTest::runAccessToObjectFwID, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 10; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        future.wait_for(std::chrono::seconds(1));
        future.get(); // Trigger exceptions
    }
}

//-----------------------------------------------------------------------------

void FwIDTest::runAccessToObjectFwID()
{
    std::string id = m_object->getID();
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(id) );
    CPPUNIT_ASSERT( m_object->hasID() );

    std::this_thread::sleep_for( std::chrono::milliseconds(200));

    CPPUNIT_ASSERT_EQUAL(  id, m_object->getID() );

    CPPUNIT_ASSERT_EQUAL( m_object, ::fwTools::fwID::getObject(id) );

    std::this_thread::sleep_for( std::chrono::milliseconds(200));

    m_object->resetID();
    CPPUNIT_ASSERT( m_object->hasID() == false );
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(id) == false );

}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

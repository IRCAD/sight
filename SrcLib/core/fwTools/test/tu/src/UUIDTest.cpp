/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwTools/Object.hpp>

#include <fwTest/helper/Thread.hpp>

#include "UUIDTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::UUIDTest );

namespace fwTools
{
namespace ut
{

void UUIDTest::setUp()
{
    // Set up context before running a test.
}

void UUIDTest::tearDown()
{
    // Clean up after the test run.
}

void UUIDTest::objectUUIDTest()
{
    const std::string UUID = "myUUID" ;

    ::fwTools::Object::sptr obj = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    ::fwTools::UUID::set(obj, UUID);

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL(  UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = ::fwTools::Object::New() ;
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}


void UUIDTest::threadSafeTest()
{
    ::fwTest::helper::Thread thread(::boost::bind(&UUIDTest::runUUIDCreation, this));
    ::fwTest::helper::Thread thread2(::boost::bind(&UUIDTest::runUUIDCreation, this));
    ::fwTest::helper::Thread thread3(::boost::bind(&UUIDTest::runUUIDCreation, this));

    CPPUNIT_ASSERT(thread.timedJoin(100));
    CPPUNIT_ASSERT(thread2.timedJoin(100));
    CPPUNIT_ASSERT(thread3.timedJoin(100));


    if (thread.hasFailed())
    {
        throw thread.getException();
    }
    if (thread2.hasFailed())
    {
        throw thread2.getException();
    }
    if (thread3.hasFailed())
    {
        throw thread3.getException();
    }
}

void UUIDTest::runUUIDCreation()
{
    std::string UUID = ::fwTools::UUID::generateUUID();

    ::fwTools::Object::sptr obj = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    CPPUNIT_ASSERT( ::fwTools::UUID::set(obj, UUID));

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL( UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = ::fwTools::Object::New() ;
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}

} // namespace ut
} // namespace fwTools

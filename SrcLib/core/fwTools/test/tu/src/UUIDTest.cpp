/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwTools/Object.hpp>

#include "UUIDTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UUIDTest );

void UUIDTest::setUp()
{
    // Set up context before running a test.
}

void UUIDTest::tearDown()
{
    // Clean up after the test run.
}

void UUIDTest::methode1()
{
    const std::string UUID = "myUUID" ;

    ::fwTools::Object::sptr obj = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID, ::fwTools::UUID::Version::SIMPLE) == false);

    ::fwTools::UUID::impose(obj, UUID, ::fwTools::UUID::Version::SIMPLE);

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID, ::fwTools::UUID::Version::SIMPLE) );
    CPPUNIT_ASSERT( ::fwTools::UUID::supervise(obj) );
    CPPUNIT_ASSERT_EQUAL( ::fwTools::UUID::get(obj), UUID );
    CPPUNIT_ASSERT_EQUAL( ::fwTools::UUID::get< ::fwTools::Object >(UUID), obj );
}

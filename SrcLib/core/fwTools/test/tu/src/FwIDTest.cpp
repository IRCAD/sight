/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <fwTest/helper/Thread.hpp>

#include "FwIDTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::FwIDTest );

namespace fwTools
{
namespace ut
{

void FwIDTest::setUp()
{
    // Set up context before running a test.
}

void FwIDTest::tearDown()
{
    // Clean up after the test run.
}

void FwIDTest::objectFwIDTest()
{
    const std::string fwID = "myID" ;

    ::fwTools::Object::sptr obj = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) );
    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::fwID::getObject(fwID) );

    ::fwTools::Object::sptr obj2 = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW (obj2->getID(::fwTools::fwID::MUST_EXIST), ::fwTools::Failed);

    std::string fwid = obj2->getID(::fwTools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW (obj2->getID(::fwTools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) == false );
    CPPUNIT_ASSERT( !::fwTools::fwID::getObject(fwid) );
}


void FwIDTest::threadSafeTest()
{
    ::fwTest::helper::Thread thread(::boost::bind(&FwIDTest::runFwIDCreation, this));
    ::fwTest::helper::Thread thread2(::boost::bind(&FwIDTest::runFwIDCreation, this));
    ::fwTest::helper::Thread thread3(::boost::bind(&FwIDTest::runFwIDCreation, this));

    CPPUNIT_ASSERT(thread.timedJoin(5000));
    CPPUNIT_ASSERT(thread2.timedJoin(5000));
    CPPUNIT_ASSERT(thread3.timedJoin(5000));


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

void FwIDTest::runFwIDCreation()
{
    const std::string fwID = ::fwTools::UUID::generateUUID() ;

    ::fwTools::Object::sptr obj = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) == false);

    obj->setID(fwID);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwID) );

    CPPUNIT_ASSERT_EQUAL(  fwID, obj->getID() );

    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::fwID::getObject(fwID) );

    ::fwTools::Object::sptr obj2 = ::fwTools::Object::New() ;

    CPPUNIT_ASSERT(obj2->hasID() == false);
    CPPUNIT_ASSERT_THROW (obj2->getID(::fwTools::fwID::MUST_EXIST), ::fwTools::Failed);

    std::string fwid = obj2->getID(::fwTools::fwID::GENERATE);
    CPPUNIT_ASSERT_NO_THROW (obj2->getID(::fwTools::fwID::MUST_EXIST));

    CPPUNIT_ASSERT(obj2->hasID() == true);
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) );

    obj2->resetID();
    CPPUNIT_ASSERT(obj2->hasID() == false);

    CPPUNIT_ASSERT( ::fwTools::fwID::exist(fwid) == false );
    CPPUNIT_ASSERT( !::fwTools::fwID::getObject(fwid) );
}

} // namespace ut
} // namespace fwTools

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FwIDTest.hpp"

#include <fwTools/fwID.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <fwTest/helper/Thread.hpp>

#include <chrono>
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

//-----------------------------------------------------------------------------

void FwIDTest::conccurentAccessOnFwIDMapTest()
{
    const unsigned int nbThreads = 10;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i = 0; i<nbThreads; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = std::shared_ptr< ::fwTest::helper::Thread >(
            new ::fwTest::helper::Thread(std::bind(&FwIDTest::runFwIDCreation, this)));
        threads.push_back(thread);
    }

    for (int i = 0; i<nbThreads; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
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

//-----------------------------------------------------------------------------

void FwIDTest::conccurentAccessOnSameObjFwIDTest()
{
    const unsigned int nbThreads = 10;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i = 0; i<nbThreads; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = std::shared_ptr< ::fwTest::helper::Thread >(
            new ::fwTest::helper::Thread(std::bind(&FwIDTest::runAccessToObjectFwID, this)));
        threads.push_back(thread);
    }

    for (int i = 0; i<nbThreads; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
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

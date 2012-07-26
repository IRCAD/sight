/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

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

//-----------------------------------------------------------------------------

void UUIDTest::setUp()
{
    // Set up context before running a test.
    m_object = ::boost::make_shared< ::fwTools::Object >();
    m_uuid = ::fwTools::UUID::generateUUID();
}

//-----------------------------------------------------------------------------

void UUIDTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void UUIDTest::objectUUIDTest()
{
    const std::string UUID = "myUUID" ;

    ::fwTools::Object::sptr obj = ::boost::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    ::fwTools::UUID::set(obj, UUID);

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL(  UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = ::boost::make_shared< ::fwTools::Object >();
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnUUIDMapTest()
{
    const unsigned int nbThreads = 10;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i=0 ; i<nbThreads ; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = ::boost::shared_ptr< ::fwTest::helper::Thread >(
                new ::fwTest::helper::Thread(::boost::bind(&UUIDTest::runUUIDCreation, this)));
        threads.push_back(thread);
    }

    for (int i=0 ; i<nbThreads ; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
    }

    for (int i=0 ; i<nbThreads ; ++i)
    {
        if (threads[i]->hasFailed())
        {
            throw threads[i]->getException();
        }
    }
}

//-----------------------------------------------------------------------------

void UUIDTest::runUUIDCreation()
{
    std::string UUID = ::fwTools::UUID::generateUUID();

    ::fwTools::Object::sptr obj = ::boost::make_shared< ::fwTools::Object >();

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) == false);

    CPPUNIT_ASSERT( ::fwTools::UUID::set(obj, UUID));

    CPPUNIT_ASSERT( ::fwTools::UUID::exist(UUID) );
    CPPUNIT_ASSERT_EQUAL( UUID, ::fwTools::UUID::get(obj) );
    CPPUNIT_ASSERT_EQUAL( obj, ::fwTools::UUID::get(UUID) );

    ::fwTools::Object::sptr obj2 = ::boost::make_shared< ::fwTools::Object >();
    std::string uuid = ::fwTools::UUID::get(obj2);
    CPPUNIT_ASSERT_EQUAL( obj2, ::fwTools::UUID::get(uuid) );
}

//-----------------------------------------------------------------------------

void UUIDTest::conccurentAccessOnSameObjUUIDTest()
{
    const unsigned int nbThreads = 10;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i=0 ; i<nbThreads ; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = ::boost::shared_ptr< ::fwTest::helper::Thread >(
                new ::fwTest::helper::Thread(::boost::bind(&UUIDTest::runAccessToObjectUUID, this)));
        threads.push_back(thread);
    }

    for (int i=0 ; i<nbThreads ; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
    }

    for (int i=0 ; i<nbThreads ; ++i)
    {
        if (threads[i]->hasFailed())
        {
            throw threads[i]->getException();
        }
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
